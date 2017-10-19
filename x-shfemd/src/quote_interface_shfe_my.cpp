#include <thread>         // std::thread
#include "quote_interface_shfe_my.h"

MYQuoteData::MYQuoteData(const SubscribeContracts *subscribe,const std::string &provider)
	: module_name_("MYQuoteData"),running_(true),seq_no_(0),server_(0)
{
	l1_md_last_index_ = L1MD_NPOS;

	// clog init
	clog_set_minimum_level(CLOG_LEVEL_WARNING);
	clog_fp_ = fopen("./x-shfemd.log","w+");
	clog_handler_ = clog_stream_handler_new_fp(clog_fp_, true, "%l %m");
	clog_handler_push_process(clog_handler_);

	ParseConfig();
	
	// disruptor init
	clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	queue_ = vrt_queue_new("x-shfemd queue",vrt_hybrid_value_type(),QUEUE_SIZE);
	fulldepth_md_producer_ = new FullDepthMDProducer(queue_);
	l1_md_producer_ = new L1MDProducer(queue_);
	consumer_ = vrt_consumer_new(module_name_, queue_);
	if(strcmp(config_.yield, "threaded") == 0){
		consumer_ ->yield = vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		consumer_ ->yield = vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		consumer_ ->yield = vrt_yield_strategy_hybrid();
	}

	for (int i=0; i<10; i++){
		repairers_[i] = new repairer(fulldepth_md_producer_ );
		repairers_[i]->server_ = i;
	}

	consumer_thread_ = new thread(&MYQuoteData::Start, this);
	consumer_thread_->detach();
}

MYQuoteData::~MYQuoteData()
{
	Stop();

	vrt_queue_free(queue_);
	delete fulldepth_md_producer_;
	delete l1_md_producer_;

	for (int i=0; i<10; i++){
		delete repairers_[i];
	}

	fflush (clog_fp_);
	clog_handler_free(clog_handler_);
}

void MYQuoteData::SetQuoteDataHandler(std::function<void(const SHFEQuote*)> quote_handler)
{
	// do not support
}

void MYQuoteData::SetQuoteDataHandler(std::function<void(const CDepthMarketDataField*)> quote_handler)
{
	// do not support
}

void MYQuoteData::SetQuoteDataHandler(std::function<void(const MYShfeMarketData*)> quote_handler)
{
	fulldepthmd_handler_ = quote_handler;
}


void MYQuoteData::ParseConfig()
{
	std::string config_file = "x-trader.config";
	TiXmlDocument doc = TiXmlDocument(config_file.c_str());
    doc.LoadFile();
    TiXmlElement *root = doc.RootElement();    

	// yield strategy
    TiXmlElement *comp_node = root->FirstChildElement("Disruptor");
	if (comp_node != NULL){
		strcpy(config_.yield, comp_node->Attribute("yield"));
		clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	} else { clog_error("[%s] x-trader.config error: Disruptor node missing.", module_name_); }

}

void MYQuoteData::Start()
{
	clog_debug("[%s] thread id:%ld", module_name_,std::this_thread::get_id() );

	running_  = true;
	int rc = 0;
	struct vrt_value *vvalue;
	while (running_ &&
		   (rc = vrt_consumer_next(consumer_, &vvalue)) != VRT_QUEUE_EOF) {
		if (rc == 0) {
			struct vrt_hybrid_value *ivalue = cork_container_of(vvalue, struct vrt_hybrid_value,
						parent);
			switch (ivalue->data){
				case L1_MD:
					ProcL1MdData(ivalue->index);
					break;
				case FULL_DEPTH_MD:
					ProcFullDepthData(ivalue->index);
					break;
				default:
					clog_error("[%s] [start] unexpected index: %d", module_name_, ivalue->index);
					break;
			}
		}
	} // end while (running_ &&

	if (rc == VRT_QUEUE_EOF) {
		clog_warning("[%s] [start] rev EOF.", module_name_);
	}
	clog_warning("[%s] start exit.", module_name_);
}

void MYQuoteData::Stop()
{
	if(running_){
		running_ = false;
		fulldepth_md_producer_->End();
		l1_md_producer_->End();
		clog_warning("[%s] End exit", module_name_);
	}
}

// done:
void MYQuoteData::ProcFullDepthData(int32_t index)
{
	MDPackEx* md = fulldepth_md_producer_->GetData(index);
	int new_svr = md->content.seqno % 10;
	if (new_svr != server_) { 
		clog_info("[%s] server from %d to %d",module_name_, server_, new_svr); 
	}

	repairers_[new_svr]->rev(index);

	bool empty = true;
	char cur_contract[10];
	strcpy(cur_contract,"");
	char new_contract[10];
	strcpy(new_contract,"");
	Reset();
	MDPackEx* data = repairers_[new_svr]->next(empty);
	while (!empty) { 
		if(strcmp(cur_contract, "") == 0){ // 为空，表示第一次进入循环
			strcpy(cur_contract,data->content.instrument);
		}
		strcpy(new_contract,data->content.instrument);

		if(strcmp(cur_contract,new_contract) != 0){
			FillFullDepthInfo();
			Send(cur_contract);
			Reset();
		}
		
		// 别放到买卖对用缓冲，待该合约的数据都接受完后，统一处理
		if(data->content.direction == SHFE_FTDC_D_Buy){
			buy_data_cursor_ = buy_data_cursor_ + 1;
			buy_data_buffer_[buy_data_cursor_] = data;
		}else if(data->content.direction == SHFE_FTDC_D_Sell){
			sell_data_cursor_ = sell_data_cursor_ + 1;
			sell_data_buffer_[sell_data_cursor_] = data;
		}
		strcpy(cur_contract,data->content.instrument);

		data = repairers_[new_svr]->next(empty);
		if(empty){ 
			FillFullDepthInfo();
			Send(cur_contract); 
			Reset();
		}
	}

	server_ = new_svr;
}

void MYQuoteData::FillBuyFullDepthInfo()
{
    target_data_.buy_total_volume = 0;
    target_data_.buy_weighted_avg_price = 0;
    double amount = 0;

	bool damaged = false;
	// VPair数据计数器，用于计数从尾部开始最多30笔数据，用于复制盘口30档数据用
	int price30_count = MY_SHFE_QUOTE_PRICE_POS_COUNT - 1; 
	double price30[MY_SHFE_QUOTE_PRICE_POS_COUNT] = {0};
	int vol30[MY_SHFE_QUOTE_PRICE_POS_COUNT] = {0};
	for(int i=buy_data_cursor_; i>=0; i--){ // 从尾部向前遍历MDPackEx数据 
		MDPackEx *src_mdpackex = buy_data_buffer_[i];
		if (src_mdpackex->damaged) damaged = true;

		for(int j=src_mdpackex->content.count-1; j>=0; j--){ //从尾部向前遍历PVPair数据 
			PVPair &src_pvpaire = src_mdpackex->content.data[j];

			// 处理30档买方向数据
			if(price30_count >= 0){
				price30[price30_count] = src_pvpaire.price;  
				vol30[price30_count] = src_pvpaire.volume;  
				price30_count = price30_count - 1;
			}

			// 计算总委买量
			target_data_.buy_total_volume += src_pvpaire.volume;
			amount += src_pvpaire.price * src_pvpaire.volume;
		} // for(int j=buy_data_buffer_[i].content.count-1; j>=0; j--)//从尾部向前遍历PVPair数据 
	} // for(int i=buy_data_cursor_; i>=0; i--) // 从尾部向前遍历MDPackEx数据 
	
	// 计算均价
	if(damaged) target_data_.buy_total_volume = 0;
	if (target_data_.buy_total_volume > 0){
		target_data_.buy_weighted_avg_price = amount / target_data_.buy_total_volume;
	}
	// 拷贝盘口30档买方向数据
	memcpy(target_data_.buy_volume, vol30, sizeof(vol30));
	memcpy(target_data_.buy_price, price30, sizeof(price30));
}

void MYQuoteData::FillSellFullDepthInfo()
{
    target_data_.sell_total_volume = 0;
    target_data_.sell_weighted_avg_price = 0;
    double amount = 0;

	bool damaged = false;
	// VPair数据计数器，用于计数从尾部开始最多30笔数据，用于复制盘口30档数据用
	int price30_count = MY_SHFE_QUOTE_PRICE_POS_COUNT - 1; 
	double price30[MY_SHFE_QUOTE_PRICE_POS_COUNT] = {0};
	int vol30[MY_SHFE_QUOTE_PRICE_POS_COUNT] = {0};
	for(int i=sell_data_cursor_; i>=0; i--){ // 从尾部向前遍历MDPackEx数据 
		MDPackEx *src_mdpackex = sell_data_buffer_[i];
		if (src_mdpackex->damaged) damaged = true;

		for(int j=src_mdpackex->content.count-1; j>=0; j--){ //从尾部向前遍历PVPair数据 
			PVPair &src_pvpaire = src_mdpackex->content.data[j];

			// 处理30档卖方向数据
			if(price30_count >= 0){
				price30[price30_count] = src_pvpaire.price;  
				vol30[price30_count] = src_pvpaire.volume;  
				price30_count = price30_count - 1;
			}

			// 计算总委卖量
			target_data_.sell_total_volume += src_pvpaire.volume;
			amount += src_pvpaire.price * src_pvpaire.volume;
		} // for(int j=sell_data_buffer_[i].content.count-1; j>=0; j--)//从尾部向前遍历PVPair数据 
	} // for(int i=sell_data_cursor_; i>=0; i--) // 从尾部向前遍历MDPackEx数据 
	
	// 计算均价
	if(damaged) target_data_.sell_total_volume = 0;
	if (target_data_.sell_total_volume > 0){
		target_data_.sell_weighted_avg_price = amount / target_data_.sell_total_volume;
	}
	// 拷贝盘口30档卖方向数据
	memcpy(target_data_.sell_volume, vol30, sizeof(vol30));
	memcpy(target_data_.sell_price, price30, sizeof(price30));
}

// done
void MYQuoteData::FillFullDepthInfo()
{
	FillBuyFullDepthInfo();
	FillSellFullDepthInfo();
}

// done
void MYQuoteData::Send(const char* contract)
{
	CDepthMarketDataField* l1_md = NULL;
	// 合并一档行情
	if(l1_md_last_index_ =! L1MD_NPOS){
		 l1_md =  l1_md_producer_->GetLastData(contract, l1_md_last_index_);
	}
	if(NULL != l1_md){
		target_data_.data_flag = 6; 
		memcpy(&target_data_, l1_md, sizeof(CDepthMarketDataField));
	} else target_data_.data_flag = 5; 

	// 发给数据客户
	if (fulldepthmd_handler_ != NULL) { fulldepthmd_handler_(&target_data_); }
}

// done
void MYQuoteData::ProcL1MdData(int32_t index)
{
	l1_md_last_index_ = index;
	CDepthMarketDataField* md = l1_md_producer_->GetData(index);

	memcpy(target_data_.InstrumentID, md->InstrumentID, sizeof(target_data_.InstrumentID));
	memcpy(&target_data_, md, sizeof(CDepthMarketDataField));
	target_data_.data_flag = 1;
	// 发给数据客户
	if (fulldepthmd_handler_ != NULL) { fulldepthmd_handler_(&target_data_); }
}

// done
void MYQuoteData::Reset()
{
	memset(target_data_.buy_price, 0, sizeof(target_data_.buy_price));
	memset(target_data_.buy_volume, 0, sizeof(target_data_.buy_volume));
	memset(target_data_.sell_price, 0, sizeof(target_data_.sell_price));
	memset(target_data_.sell_volume, 0, sizeof(target_data_.sell_volume));

	buy_data_cursor_ = -1;
	sell_data_cursor_ = -1;
}
