#include <thread>         // std::thread
#include "quote_interface_shfe_my.h"

std::string MYQuoteData::ToString(const MYShfeMarketData &d) {
	clog_info("MYShfeMarketData: instrument:%s, data_flag:%d,buy_total_volume:%d;"
				"sell_total_volume:%d; buy_weighted_avg_price:%lf; sell_weighted_avg_price:%lf",
				d.InstrumentID, 
				d.data_flag, 
				d.buy_total_volume,
				d.sell_total_volume,
				d.buy_weighted_avg_price,
				d.sell_weighted_avg_price);

	clog_info("dir:buy; price, volume");
	for(int i = 0; i < 30; i++) {
		 clog_info("price%d: %lf, volume%d: %d\n", i, d.buy_price[i], i, d.buy_volume[i]);
	}

	clog_info("dir:sell; price, volume");
	for(int i = 0; i < 30; i++) {
		 clog_debug("price%d: %lf, volume%d: %d\n", i, d.sell_price[i], i, d.sell_volume[i]);
	}
  
  return "";
}

MYQuoteData::MYQuoteData(FullDepthMDProducer *fulldepth_md_producer, L1MDProducer *l1_md_producer)
	: fulldepth_md_producer_(fulldepth_md_producer), 
	  l1_md_producer_(l1_md_producer),
	  module_name_("MYQuoteData")
{
	ready_ = false;
	l1_md_last_index_ = L1MD_NPOS;
	Reset();

#ifdef PERSISTENCE_ENABLED 
    p_my_shfe_md_save_ = new QuoteDataSave<MYShfeMarketData>("my_shfe_md", MY_SHFE_MD_QUOTE_TYPE);
#endif
}

MYQuoteData::~MYQuoteData()
{
#ifdef PERSISTENCE_ENABLED 
    if (p_my_shfe_md_save_) delete p_my_shfe_md_save_;
#endif

	clog_warning("[%s] ~MYQuoteData invoked.", module_name_);
}

void MYQuoteData::ProcFullDepthData(int32_t index)
{
	CShfeFtdcMBLMarketDataField* md = fulldepth_md_producer_->GetData(index);
	if(!ready_) {
		if(strlen(md->InstrumentID) != 4) { // instrumentID="last"
			clog_info("[%s] ready_=%d. return", module_name_, ready_);
			return; 
		}
		else{
			ready_ = true;
			clog_info("[%s] ready_=%d. ", module_name_, ready_);
			return;
		}
	}
	
	// 结束一个数据帧，开始新的数据帧接收
	if(strlen(md->InstrumentID) == 4) { // instrumentID="last"
		PopData(cur_contract_);
		while(buy_read_cursor_ < buy_write_cursor_){ // 最后的数据是涨停数据
			PopData("zzzzzz"); // 方便后边的逻辑
		}
		Reset();
	}
	else{
		if(data->content.direction == SHFE_FTDC_D_Buy){
			buy_data_buffer_[buy_write_cursor_] = *md;
			buy_write_cursor_++;
		}
		else{
			strcpy(new_contract_, md->InstrumentID);
			if(0==cur_contract_[0]){ // 在当前数据帧内首次接收卖方向数据
				strcpy(cur_contract_, md->InstrumentID);
			}
			if(!IsSameContract(new_contract_, cur_contract_)){ // 抽取当前合约的完整数据
				PopData(cur_contract_);
				strcpy(cur_contract_, new_contract);
			}

			sell_data_buffer_[sell_write_cursor_] = *md;
			sell_write_cursor_++;
		}
	}
}

/*
 *	buy_queue_contract_start:
 *			-1表示无效位置，不需要对该队列进行操作，
 *			函数返回时，使其指向最新读位置
 *
 */
void MYQuoteData::FillBuyData(
			const char* contract,
			int &buy_queue_contract_start, 
			int buy_queue_end)
{
    target_data_.buy_total_volume = 0;
    target_data_.buy_weighted_avg_price = 0;
    double amount = 0;

	if(INVALID_CURSOR != buy_queue_contract_start){
		int buy_queue_contract_end = buy_queue_contract_start;
		for(; buy_queue_contract_end<=buy_queue_end; buy_queue_contract_end++){
			if(!IsSameContract(contract,buy_data_buffer_[buy_queue_contract_end])){
				break;
			}
		}
		buy_queue_contract_end--;
		
		// 计数器，用于计数从尾部开始最多30笔数
		int price30_count = MY_SHFE_QUOTE_PRICE_POS_COUNT - 1;  
		double *price30 = target_data_.buy_price;
		int *vol30 = target_data_.buy_volume;
		for(int i=buy_queue_contract_end; i>=buy_queue_contract_start; i--){ // 从尾部向前遍历数据 
			CShfeFtdcMBLMarketDataField &data = buy_data_buffer_[i];
			// 处理30档买方向数据
			if(price30_count >= 0){
				price30[price30_count] = data.price;  
				vol30[price30_count] = data.volume;  
				price30_count = price30_count - 1;
			}

			// 计算总委买量
			target_data_.buy_total_volume += data.volume;
			amount += src_pvpaire.price * data.volume;
		} // for(int i=buy_data_cursor_; i>=0; i--) // 从尾部向前遍历MDPackEx数据 
		
		if (target_data_.buy_total_volume > 0){
			target_data_.buy_weighted_avg_price = amount / target_data_.buy_total_volume;
		}
		buy_queue_contract_start = buy_queue_contract_end + 1;
	}
}

/*
 *	int sell_queue_contract_start:
 *			-1表示无效位置，不需要对该队列进行操作，函数返回时，使其指向最新读位置
 *
 */
void MYQuoteData::FillSellData(
			const char* contract,
			int &sell_queue_contract_start, 
			int sell_queue_contract_end)
{
    target_data_.sell_total_volume = 0;
    target_data_.sell_weighted_avg_price = 0;
    double amount = 0;

	if(INVALID_CURSOR != sell_queue_contract_start){
		// 数据计数器，用于计数从尾部开始最多30笔数据，用于复制盘口30档数据用
		int price30_count = MY_SHFE_QUOTE_PRICE_POS_COUNT - 1; 
		double *price30 = target_data_.sell_price;
		int *vol30 = target_data_.sell_volume;
		for(int i=sell_queue_contract_end_; i>=sell_queue_contract_start; i--){ // 从尾部向前遍历数据 
			CShfeFtdcMBLMarketDataField *data = sell_data_buffer_[i];
			// 处理30档卖方向数据
			if(price30_count >= 0){
				price30[price30_count] = data.price;  
				vol30[price30_count] = data.volume;  
				price30_count = price30_count - 1;
			}

			// 计算总委卖量
			target_data_.sell_total_volume += data.volume;
			amount += src_pvpaire.price * data.volume;
		} // for(int i=sell_data_cursor_; i>=0; i--) // 从尾部向前遍历数据 
		
		// 计算均价
		if (target_data_.sell_total_volume > 0){
			target_data_.sell_weighted_avg_price = amount / target_data_.sell_total_volume;
		}

		sell_queue_contract_start = sell_queue_contract_end + 1;
	}
}

void MYQuoteData::Send(const char* contract)
{
	CDepthMarketDataField* l1_md = NULL;

	// 合并一档行情
	if(l1_md_last_index_ != L1MD_NPOS){
		 l1_md =  l1_md_producer_->GetLastData(contract, l1_md_last_index_);
	}
	if(NULL != l1_md){
		target_data_.data_flag = 6; 
		memcpy(&target_data_, l1_md, sizeof(CDepthMarketDataField));
	} else{
		strcpy(target_data_.InstrumentID, contract);
		target_data_.data_flag = 5; 
	}

	// 发给数据客户
	if (fulldepthmd_handler_ != NULL) { fulldepthmd_handler_(&target_data_); }

#ifdef PERSISTENCE_ENABLED 
    timeval t;
    gettimeofday(&t, NULL);
    p_my_shfe_md_save_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, &target_data_);
#endif
}

void MYQuoteData::SetQuoteDataHandler(std::function<void(MYShfeMarketData*)> quote_handler)
{
	clog_warning("[%s] SetQuoteDataHandler invoked.", module_name_);
	fulldepthmd_handler_ = quote_handler;
}

void MYQuoteData::ProcL1MdData(int32_t index)
{
	l1_md_last_index_ = index;
	CDepthMarketDataField* md = l1_md_producer_->GetData(index);

	//clog_info("[%s] ProcL1MdData:constract:%s;index:%d", module_name_, md->InstrumentID, l1_md_last_index_); 

	memcpy(&target_data_, md, sizeof(CDepthMarketDataField));
	target_data_.data_flag = 1;
	
	// 发给数据客户
	if (fulldepthmd_handler_ != NULL) { fulldepthmd_handler_(&target_data_); }

#ifdef PERSISTENCE_ENABLED 
    timeval t;
    gettimeofday(&t, NULL);
    p_my_shfe_md_save_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, &target_data_);
#endif
}

void MYQuoteData::Reset()
{
		buy_write_cursor_ = 0; 
		buy_buy_read_cursor_ = 0;  
		buy_sell_write_cursor_ = 0;
		buy_sell_read_cursor_ = 0; 
		buy_cur_contract_[0] = 0;
		new_contract_[0] = 0;

}

/***
 * 
 * 处理指定的卖方向合约以及当前合约之前的所有数据，
 * 分为普通场景、涨停场景、跌停3种场景.
 *	pop_sell_contract:
 *			 要抽取的在卖队列合约
 */
void MYQuoteData::PopData( const char* pop_sell_contract)
{
	char* buy_contract = "";
	if(buy_read_cursor_ == buy_write_cursor_){ // 买队列已经没有数据
		strcpy(buy_contract, "zzzzzz"); // 方便后边的逻辑
	}
	else{
		buy_contract = buy_data_buffer_[buy_read_cursor_].InstrumentID;
	}

	if(IsSameContract(pop_sell_contract, buy_contract)){ // 普通场景
		PopOneContractData(
					pop_sell_contract,
					buy_read_cursor_,
					buy_write_cursor_ - 1,
					sell_read_cursor_,
					sell_write_cursor_ - 1);
	}
	else if(strcmp(pop_sell_contract,buy_contract)>0){ // 涨停场景
		do{
			PopOneContractData(
					buy_contract,
					buy_read_cursor_,
					buy_write_cursor_ - 1,
					INVALID_CURSOR,
					INVALID_CURSOR);
			if(buy_read_cursor_ >= buy_write_cursor_){
				return; // no data to be processed
			}
			else{
				buy_contract = buy_data_buffer_[buy_read_cursor_].InstrumentID;
			}
		}while(strcmp(pop_sell_contract,buy_contract)>0)

		PopOneContractData(
					pop_sell_contract,
					buy_read_cursor_,
					buy_write_cursor_ - 1,
					sell_read_cursor_,
					sell_write_cursor_ - 1);
	}
	else{	// 跌停场景
		PopOneContractData(
					pop_sell_contract,
					INVALID_CURSOR,
					INVALID_CURSOR,
					sell_read_cursor_,
					sell_write_cursor_ - 1);
	}
}

/*
 * :
 *		从买、卖队列抽取指定合约的数据.
 *	contract:
 *			 要抽取数据的合约
 *	buy_queue_contract_start: 
 *			买队列要抽取的合约的开始位置，函数返回时，使其指向最新读位置. 
 *			-1表示无效位置，不需要对该队列进行操作
 *  buy_queue_end:  
 *			买队列当前可以读的最末位置	 
 *  sell_queue_contract_start:
 *			卖队列要抽取的合约开始位置，函数返回时，使其指向最新读位置.
 *			-1表示无效位置，不需要对该队列进行操作
 *  sell_queue_contract_end:
 *			卖队列要抽取的合约结束位置
 */
void MYQuoteData::PopOneContractData(
			const char* contract,			
			int &buy_queue_contract_start,	
			int buy_queue_end,				
			int &sell_queue_contract_start,	 
			int sell_queue_contract_end)	 
{
	memset(target_data_.buy_price, 0, sizeof(target_data_.buy_price));
	memset(target_data_.buy_volume, 0, sizeof(target_data_.buy_volume));
	memset(target_data_.sell_price, 0, sizeof(target_data_.sell_price));
	memset(target_data_.sell_volume, 0, sizeof(target_data_.sell_volume));

	FillBuyData(contract,buy_queue_contract_start,buy_queue_end);
	FillSellData(contract,sell_queue_contract_start,sell_queue_contract_end);
	Send(contract);
}


bool MYQuoteData::IsSameContract(const char *contract1, const char* contract2)
{
	if(contract1[0]==contract2[0] &&
	   contract1[1]==contract2[1] &&
	   contract1[5]==contract2[5]){
		return true;
	}
	else{
		return false;
	}
}
