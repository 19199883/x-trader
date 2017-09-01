#include <thread>         
#include <chrono>        
#include <algorithm>    
#include "uni_consumer.h"
#include "pos_calcu.h"
#include "perfctx.h"

UniConsumer::UniConsumer(struct vrt_queue  *queue, MDProducer *md_producer, 
			TunnRptProducer *tunn_rpt_producer,
			PendingSigProducer* pendingsig_producer)
: module_name_("uni_consumer"),running_(true), 
  md_producer_(md_producer),
  tunn_rpt_producer_(tunn_rpt_producer),
  pendingsig_producer_(pendingsig_producer)
{
#if FIND_STRATEGIES == 1
	// unordered_multimap 
	//clog_info("[%s] method for finding strategies by contract:unordered_multimap", module_name_);
#endif

#if FIND_STRATEGIES == 2 // two-dimensional array
	memset(stra_idx_table_, -1, sizeof(stra_idx_table_));
	memset(cont_straidx_map_table_, -1, sizeof(cont_straidx_map_table_));
	clog_info("[%s] method for finding strategies by contract:two-dimensional array ", module_name_);
#endif	

#if FIND_STRATEGIES == 3
	// strcmp
	//clog_info("[%s] method for finding strategies by contract:strcmp", module_name_);
#endif

	clog_info("[%s] STRA_TABLE_SIZE: %d;", module_name_, STRA_TABLE_SIZE);

	(this->consumer_ = vrt_consumer_new(module_name_, queue));
	this->consumer_->yield = vrt_yield_strategy_threaded();

	this->pproxy_ = CLoadLibraryProxy::CreateLoadLibraryProxy();
	this->pproxy_->setModuleLoadLibrary(new CModuleLoadLibraryLinux());
	this->pproxy_->setBasePathLibrary(this->pproxy_->getexedir());

	ParseConfig();
	
	// pos_calc, check offline strategies
	pos_calc *calc = pos_calc::instance();
	list<string> stras;
	calc->get_stras(stras);
	for (string stra : stras){
		bool online = false;
		for (auto sett : strategy_settings_){  
			if (sett.file == stra){
				online = true;
				break;
			}
		}
		if (!online){
			clog_warning("[%s] pos_calc error: offline(%s)", module_name_, stra.c_str());
		}
	}

	// create Stratedy objects
	CreateStrategies();
}

UniConsumer::~UniConsumer()
{
	running_ = false;

	if (pproxy_ != NULL){
		pproxy_->DeleteLoadLibraryProxy();
		pproxy_ = NULL;
	}

//	if (this->consumer_ != NULL){
//		vrt_consumer_free(this->consumer_);
//		this->consumer_ = NULL;
//		clog_info("[%s] release uni_consumer.", module_name_);
//	}
}

void UniConsumer::ParseConfig()
{
	std::string config_file = "x-trader.config";
	TiXmlDocument doc = TiXmlDocument(config_file.c_str());
    doc.LoadFile();
    TiXmlElement *root = doc.RootElement();    
    TiXmlElement *strategies_ele = root->FirstChildElement("strategies");
	if (strategies_ele != 0){
		TiXmlElement *strategy_ele = strategies_ele->FirstChildElement();
		while (strategy_ele != 0){
			StrategySetting strategy_setting = this->CreateStrategySetting(strategy_ele);
			this->strategy_settings_.push_back(strategy_setting);
			strategy_ele = strategy_ele->NextSiblingElement();			
		}
	}  
}

StrategySetting UniConsumer::CreateStrategySetting(const TiXmlElement *ele)
{
	StrategySetting setting;
	strcpy(setting.config.st_name, ele->Attribute("model_file"));
	setting.config.st_id = stoi(ele->Attribute("id"));
	setting.file = ele->Attribute("model_file");
	// log_id
	setting.config.log_id = setting.config.st_id *10 + 0;
	// log_name
	strcpy(setting.config.log_name, ele->Attribute("log_name"));
	// iv_id
	setting.config.iv_id = setting.config.st_id *10 + 1;
	// iv_name
	strcpy(setting.config.iv_name ,ele->Attribute("iv_name"));
	// ev_id
	setting.config.ev_id = setting.config.st_id *10 + 2;
	// ev_name
	const char * name = ele->Attribute("ev_name");
	strcpy(setting.config.ev_name, ele->Attribute("ev_name"));

	int counter = 0;
	const TiXmlElement* symbol_ele = ele->FirstChildElement();		
	while (symbol_ele != 0)	{		
		symbol_t &tmp = setting.config.symbols[counter];

		symbol_ele->QueryIntAttribute("max_pos", &tmp.max_pos);
		strcpy(tmp.name, symbol_ele->Attribute("name"));
		string exc_str = symbol_ele->Attribute("exchange");
		tmp.exchange = static_cast<exchange_names>(exc_str[0]);
		symbol_ele->QueryIntAttribute("symbol_log_id",&tmp.symbol_log_id);
		strcpy(tmp.symbol_log_name, symbol_ele->Attribute("symbol_log_name"));

		symbol_ele = symbol_ele->NextSiblingElement();
		counter++;
	}	//end while (symbol_ele != 0)
	setting.config.symbols_cnt = counter;
	   
	return setting;
}

void UniConsumer::GetKeys(const char* contract, int &key1, int &key2)
{
	key1 = 0;
	key2 = 0;
	int i = 0;
	for (; i < strlen(contract); i++){
		if (isalpha(contract[i])) key1 += contract[i]; 
		else{ break;}
	}

	key2 = atoi(contract +i);   
}

#if FIND_STRATEGIES== 2 // two-dimensional array
int32_t UniConsumer::GetEmptyNode()
{
	for(int i=0; i < STRA_TABLE_SIZE; i++){
		if(stra_idx_table_[i][0] < 0) return i;
	}
}
#endif

void UniConsumer::CreateStrategies()
{
	strategy_counter_ = 0;
	for (auto &setting : this->strategy_settings_){
		stra_table_[strategy_counter_].Init(setting, this->pproxy_);
		// mapping table
		straid_straidx_map_table_[setting.config.st_id] = strategy_counter_ ;

#if FIND_STRATEGIES == 1 //unordered_multimap  
		// only support one contract for one strategy
		cont_straidx_map_table_.emplace(setting.config.symbols[0].name, strategy_counter_);
#endif

#if FIND_STRATEGIES == 2 // two-dimensional array
		int key1 = 0;
		int key2 = 0;
		GetKeys(setting.config.symbols[0].name,key1,key2);
		int32_t cur_node = -1;
		if (cont_straidx_map_table_[key1][key2] < 0){
			cur_node = GetEmptyNode();
			cont_straidx_map_table_[key1][key2] = cur_node;
		} else { cur_node = cont_straidx_map_table_[key1][key2]; }
		for(int i=0; i < STRA_TABLE_SIZE; i++){
			if(stra_idx_table_[cur_node][i] < 0){
				stra_idx_table_[cur_node][i] = strategy_counter_;
				break;
			}
		}
#endif

		clog_info("[%s] [CreateStrategies] id:%d; contract: %s; maxvol: %d; so:%s ", 
					module_name_, stra_table_[strategy_counter_].GetId(),
					stra_table_[strategy_counter_].GetContract(), 
					stra_table_[strategy_counter_].GetMaxPosition(), 
					stra_table_[strategy_counter_].GetSoFile());

		strategy_counter_++;
	}
}

void UniConsumer::Start()
{
	running_  = true;

	int rc = 0;
	struct vrt_value  *vvalue;
	while (running_ &&
		   (rc = vrt_consumer_next(consumer_, &vvalue)) != VRT_QUEUE_EOF) {
		if (rc == 0) {
			struct vrt_hybrid_value *ivalue = cork_container_of(vvalue, struct vrt_hybrid_value, parent);
			switch (ivalue->data){
				case BESTANDDEEP:
					ProcBestAndDeep(ivalue->index);
					break;
				case ORDERSTATISTIC:
					ProcOrderStatistic(ivalue->index);
					break;
				case PENDING_SIGNAL:
					ProcPendingSig(ivalue->index);
					break;
				case TUNN_RPT:
					ProcTunnRpt(ivalue->index);
					break;
				default:
					clog_info("[%s] [start] unexpected index: %d", module_name_, ivalue->index);
					break;
			}
		}
	} // end while (running_ &&

	if (rc == VRT_QUEUE_EOF) {
		clog_info("[%s] [start] rev EOF.", module_name_);
	}
}

void UniConsumer::Stop()
{
	running_ = false;
	md_producer_->End();
	tunn_rpt_producer_->End();
	pendingsig_producer_->End();
}

void UniConsumer::ProcBestAndDeep(int32_t index)
{
	MDBestAndDeep_MY* md = md_producer_->GetBestAnddeep(index);

	clog_debug("[%s] [ProcBestAndDeep] index: %d; contract: %s", module_name_, index, md->Contract);

#if FIND_STRATEGIES == 1 //unordered_multimap  
	auto range = cont_straidx_map_table_.equal_range(md->Contract);
	for_each (
		range.first,
		range.second,
		[=](std::unordered_multimap<std::string, int32_t>::value_type& x){
			int sig_cnt = 0;
			stra_table_[x.second].FeedMd(md, &sig_cnt, sig_buffer_);
			ProcSigs(stra_table_[x.second], sig_cnt, sig_buffer_);
		}
	);
#endif

#if FIND_STRATEGIES == 2 //  two-dimensional array
	int32_t key1,key2;
	int32_t sig_cnt = 0;
	GetKeys(md->Contract,key1,key2);
	int32_t cur_node = cont_straidx_map_table_[key1][key2]; 
	if (cur_node >= 0){
		for(int i=0; i < STRA_TABLE_SIZE; i++){
			if(stra_idx_table_[cur_node][i] >= 0){
				int32_t stra_idx = stra_idx_table_[cur_node][i];
				Strategy &strategy = stra_table_[stra_idx];
				strategy.FeedMd(md, &sig_cnt, sig_buffer_);
				ProcSigs(strategy, sig_cnt, sig_buffer_);
			} else { break; }
		}
	}
#endif

#if FIND_STRATEGIES == 3 // strcmp
	for(int i = 0; i < strategy_counter_; i++){ 
		int sig_cnt = 0;
		Strategy &strategy = stra_table_[i];
		if (strcmp(strategy.GetContract(), md->Contract) == 0){
			strategy.FeedMd(md, &sig_cnt, sig_buffer_);
			ProcSigs(strategy, sig_cnt, sig_buffer_);
		}
	}
#endif

}

void UniConsumer::ProcOrderStatistic(int32_t index)
{
	MDOrderStatistic_MY* md = md_producer_->GetOrderStatistic(index);

	clog_debug("[%s] [ProcOrderStatistic] index: %d; contract: %s", module_name_, index, md->ContractID);

#if FIND_STRATEGIES == 1 // unordered_multimap
	auto range = cont_straidx_map_table_.equal_range(md->ContractID);
	for_each (
		range.first,
		range.second,
		[=](std::unordered_multimap<std::string, int32_t>::value_type& x){
			int sig_cnt = 0;
			stra_table_[x.second].FeedMd(md, &sig_cnt, sig_buffer_);
			ProcSigs(stra_table_[x.second], sig_cnt, sig_buffer_);
		}
	);
#endif

#if FIND_STRATEGIES == 2 // two-dimensional array
	int32_t key1,key2;
	int32_t sig_cnt = 0;
	GetKeys(md->ContractID,key1,key2);
	int32_t cur_node = cont_straidx_map_table_[key1][key2]; 
	if (cur_node >= 0){
		for(int i=0; i < STRA_TABLE_SIZE; i++){
			if(stra_idx_table_[cur_node][i] >= 0){
				int32_t stra_idx = stra_idx_table_[cur_node][i];
				Strategy &strategy = stra_table_[stra_idx];
				strategy.FeedMd(md, &sig_cnt, sig_buffer_);
				ProcSigs(strategy, sig_cnt, sig_buffer_);
			} else { break; }		
		}
	}
#endif

#if FIND_STRATEGIES == 3 //strcmp
	for(int i = 0; i < strategy_counter_; i++){ 
		int sig_cnt = 0;
		Strategy &strategy = stra_table_[i];
		if (strcmp(strategy.GetContract(), md->ContractID) == 0){
			strategy.FeedMd(md, &sig_cnt, sig_buffer_);
			ProcSigs(strategy, sig_cnt, sig_buffer_);
		}
	}
#endif


}

void UniConsumer::ProcPendingSig(int32_t index)
{
	signal_t* sig = pendingsig_producer_->GetSignal(index);

	clog_debug("[%s] [ProcPendingSig] index: %d; strategy id:%d; sig id: %d", module_name_, index, sig->st_id, sig->sig_id);

	Strategy& strategy = stra_table_[straid_straidx_map_table_[sig->st_id]];
	PlaceOrder(strategy, *sig);
}

void UniConsumer::ProcTunnRpt(int32_t index)
{
	int sig_cnt = 0;

	TunnRpt* rpt = tunn_rpt_producer_->GetRpt(index);
	int32_t strategy_id = tunn_rpt_producer_->GetStrategyID(*rpt);

	clog_debug("[%s] [ProcTunnRpt] index: %d; LocalOrderID: %ld; OrderStatus:%d; MatchedAmount:%ld; CancelAmount:%ld; ErrorID:%d ",
				module_name_, index, rpt->LocalOrderID, rpt->OrderStatus, rpt->MatchedAmount, rpt->CancelAmount, rpt->ErrorID);

	Strategy& strategy = stra_table_[straid_straidx_map_table_[strategy_id]];
	strategy.FeedTunnRpt(*rpt, &sig_cnt, sig_buffer_);
	ProcSigs(strategy, sig_cnt, sig_buffer_);
}

void UniConsumer::ProcSigs(Strategy &strategy, int32_t sig_cnt, signal_t *sigs)
{
	clog_debug("[%s] [ProcSigs] sig_cnt: %d; ", module_name_, sig_cnt);

	for (int i = 0; i < sig_cnt; i++){
		if (sigs[i].sig_act == signal_act_t::cancel){
			CancelOrder(strategy, sigs[i]);
		}
		else{
			PlaceOrder(strategy, sigs[i]);
		}
	}
}

void UniConsumer::CancelOrder(Strategy &strategy,signal_t &sig)
{
	// TODO:
		T_CancelOrder chn_ord;
 		chn_ord.direction = ord.side;
		chn_ord.limit_price = ord.price;
		chn_ord.open_close = ord.position_effect;
		chn_ord.serial_no = ord.cl_ord_id;
		chn_ord.speculator = '0';
		strcpy(chn_ord.stock_code,ord.symbol.c_str());
		chn_ord.volume = ord.volume;
		chn_ord.org_serial_no = ord.orig_cl_ord_id;
		chn_ord.entrust_no = ord.orig_ord_id;
		chn_ord.exchange_type = ord.exchange;
		this->channel->CancelOrder(&chn_ord);



	if (!strategy.HasFrozenPosition()){
		clog_debug("[%s] CancelOrder: ignore request due to frozen position.", module_name_); 
		return;
	}
	
    CX1FtdcCancelOrderField cancel_order;
    memset(&cancel_order, 0, sizeof(CX1FtdcCancelOrderField));
	// get LocalOrderID by signal ID
	cancel_order.LocalOrderID = strategy.GetLocalOrderID(sig.orig_sig_id);
	// only use LocalOrderID to cancel order
    cancel_order.X1OrderID = 0; 
    //strncpy(cancle_order.AccountID, cfg.Logon_config().clientid.c_str(), sizeof(TX1FtdcAccountIDType));
    strncpy(cancel_order.InstrumentID, sig.symbol, sizeof(TX1FtdcInstrumentIDType));

	clog_debug("[%s] CancelOrder: LocalOrderID:%ld; X1OrderID:%ld; contract:%s", 
				module_name_, cancel_order.LocalOrderID, cancel_order.X1OrderID, cancel_order.InstrumentID); 

	this->tunn_rpt_producer_->ReqOrderAction(&cancel_order);

//#ifdef LATENCY_MEASURE
		int latency = perf_ctx::calcu_latency(sig.st_id, sig.sig_id);
        if(latency > 0) clog_warning("[%s] cancel latency:%d us", module_name_, latency); 
//#endif
}

void UniConsumer::PlaceOrder(Strategy &strategy,const signal_t &sig)
{
	// TODO:
		T_PlaceOrder chn_ord;
		chn_ord.direction = ord.side;
		chn_ord.limit_price = ord.price;
		chn_ord.open_close = ord.position_effect;
		chn_ord.order_kind = ord.price_type;
		chn_ord.order_type = ord.ord_type;
		chn_ord.serial_no = ord.cl_ord_id;
		chn_ord.speculator = ord.sah_;
		chn_ord.exchange_type = ord.exchange;
		strcpy(chn_ord.stock_code,ord.symbol.c_str());
		chn_ord.volume = ord.volume;
		this->channel->PlaceOrder(&chn_ord);



	int32_t vol = 0;
	int32_t updated_vol = 0;
	if (sig.sig_openclose == alloc_position_effect_t::open_){
		vol = sig.open_volume;
	} else if (sig.sig_openclose == alloc_position_effect_t::close_){
		vol = sig.close_volume;
	} else{ clog_info("[%s] PlaceOrder: do support sig_openclose value:%d;", module_name_, sig.sig_openclose); }

	if(strategy.Deferred(sig.sig_id, sig.sig_openclose, sig.sig_act, vol, updated_vol)){
		// place signal into disruptor queue
		pendingsig_producer_->Publish(sig);
	} else {
		long localorderid = this->tunn_rpt_producer_->NewLocalOrderID(strategy.GetId());
		strategy.PrepareForExecutingSig(localorderid, sig, updated_vol);

		CX1FtdcInsertOrderField insert_order;
		memset(&insert_order, 0, sizeof(CX1FtdcInsertOrderField));
		X1FieldConverter::Convert(sig, tunn_rpt_producer_->GetAccount(), localorderid, updated_vol, insert_order);

		tunn_rpt_producer_->ReqOrderInsert(&insert_order);

//#ifdef LATENCY_MEASURE
        // latency measure
		int latency = perf_ctx::calcu_latency(sig.st_id, sig.sig_id);
        if(latency > 0) clog_warning("[%s] place latency:%d us", module_name_, latency); 
//#endif
	}
}

