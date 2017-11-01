#include <thread>         
#include <functional>   // std::bind
#include <chrono>        
#include <algorithm>    
#include "uni_consumer.h"
#include "pos_calcu.h"
#include "perfctx.h"
#include <tinyxml.h>
#include <tinystr.h>

using namespace std::placeholders; 

CUstpFtdcInputOrderField FemasFieldConverter::new_order_;
CUstpFtdcOrderActionField FemasFieldConverter::cancel_order_;

UniConsumer::UniConsumer(struct vrt_queue  *queue, FullDepthMDProducer *fulldepth_md_producer, 
	L1MDProducer *l1_md_producer,  TunnRptProducer *tunn_rpt_producer)
: module_name_("uni_consumer"),running_(true), 
  fulldepth_md_producer_(fulldepth_md_producer),
  l1_md_producer_(l1_md_producer),
  tunn_rpt_producer_(tunn_rpt_producer)
{
	memset(pending_signals_, -1, sizeof(pending_signals_));
	ParseConfig();
	FemasFieldConverter::InitNewOrder(tunn_rpt_producer_->config_);
	FemasFieldConverter::InitCancelOrder(tunn_rpt_producer_->config_);

#if FIND_STRATEGIES == 1
	unordered_multimap 
	clog_warning("[%s] method for finding strategies by contract:unordered_multimap", module_name_);
#endif

#if FIND_STRATEGIES == 2 // two-dimensional array
	memset(stra_idx_table_, -1, sizeof(stra_idx_table_));
	memset(cont_straidx_map_table_, -1, sizeof(cont_straidx_map_table_));
	clog_warning("[%s] method for finding strategies by contract:two-dimensional array ", module_name_);
#endif	

#if FIND_STRATEGIES == 3 // strcmp
	clog_warning("[%s] method for finding strategies by contract:strcmp", module_name_);
#endif

	clog_warning("[%s] STRA_TABLE_SIZE: %d;", module_name_, STRA_TABLE_SIZE);

	(this->consumer_ = vrt_consumer_new(module_name_, queue));
	clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0){
		this->consumer_ ->yield = vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		this->consumer_ ->yield = vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		this->consumer_ ->yield = vrt_yield_strategy_hybrid();
	}


	this->pproxy_ = CLoadLibraryProxy::CreateLoadLibraryProxy();
	this->pproxy_->setModuleLoadLibrary(new CModuleLoadLibraryLinux());
	this->pproxy_->setBasePathLibrary(this->pproxy_->getexedir());

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
			clog_error("[%s] pos_calc error: offline(%s)", module_name_, stra.c_str());
		}
	}

	// create Stratedy objects
	CreateStrategies();
}

UniConsumer::~UniConsumer()
{
	running_ = false;

	delete fulldepth_md_producer_;
	delete l1_md_producer_;

	if (pproxy_ != NULL){
		pproxy_->DeleteLoadLibraryProxy();
		pproxy_ = NULL;
	}
}

void UniConsumer::ParseConfig()
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

		clog_warning("[%s] [CreateStrategies] id:%d; contract: %s; maxvol: %d; so:%s ", 
					module_name_, stra_table_[strategy_counter_].GetId(),
					stra_table_[strategy_counter_].GetContract(), 
					stra_table_[strategy_counter_].GetMaxPosition(), 
					stra_table_[strategy_counter_].GetSoFile());

		strategy_counter_++;
	}
}

void UniConsumer::Start()
{
	clog_debug("[%s] thread id:%ld", module_name_,std::this_thread::get_id() );

	running_  = true;

	MYQuoteData myquotedata(fulldepth_md_producer_, l1_md_producer_);
	auto f_shfemarketdata = std::bind(&UniConsumer::ProcShfeMarketData, this,_1);
	myquotedata.SetQuoteDataHandler(f_shfemarketdata);

	int rc = 0;
	struct vrt_value  *vvalue;
	while (running_ &&
		   (rc = vrt_consumer_next(consumer_, &vvalue)) != VRT_QUEUE_EOF) {
		if (rc == 0) {
			struct vrt_hybrid_value *ivalue = cork_container_of(vvalue, struct vrt_hybrid_value, parent);
			switch (ivalue->data){
				case L1_MD:
					myquotedata.ProcL1MdData(ivalue->index);
					break;
				case FULL_DEPTH_MD:
					myquotedata.ProcFullDepthData(ivalue->index);
					break;
				case TUNN_RPT:
					ProcTunnRpt(ivalue->index);
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

void UniConsumer::Stop()
{
	if(running_){
		running_ = false;
		l1_md_producer_->End();
		fulldepth_md_producer_->End();
		tunn_rpt_producer_->End();
#ifdef COMPLIANCE_CHECK
		compliance_.Save();
#endif

		for(int i=0; i<strategy_counter_; i++){
			stra_table_[i].End();
		}
		clog_warning("[%s] End exit", module_name_);
	}
}

void UniConsumer::ProcShfeMarketData(MYShfeMarketData* md)
{
#if FIND_STRATEGIES == 1 //unordered_multimap  
	auto range = cont_straidx_map_table_.equal_range(md->InstrumentID);
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
		if (strcmp(strategy.GetContract(), md->InstrumentID) == 0){
			strategy.FeedMd(md, &sig_cnt, sig_buffer_);
			ProcSigs(strategy, sig_cnt, sig_buffer_);
		}
	}
#endif
}

void UniConsumer::ProcTunnRpt(int32_t index)
{
	int sig_cnt = 0;

	TunnRpt* rpt = tunn_rpt_producer_->GetRpt(index);
	int32_t strategy_id = tunn_rpt_producer_->GetStrategyID(*rpt);

	clog_info("[%s] [ProcTunnRpt] index: %d; LocalOrderID: %ld; OrderStatus:%c; MatchedAmount:%d;"
				"ErrorID:%d ",
				module_name_, index, rpt->LocalOrderID, rpt->OrderStatus, rpt->MatchedAmount,
				 rpt->ErrorID);

	Strategy& strategy = stra_table_[straid_straidx_map_table_[strategy_id]];

#ifdef COMPLIANCE_CHECK
	if (rpt->OrderStatus == USTP_FTDC_OS_PartTradedNotQueueing ||
			rpt->OrderStatus == USTP_FTDC_OS_Canceled){
		compliance_.AccumulateCancelTimes(strategy.GetContract());
	}

	if (rpt->OrderStatus == USTP_FTDC_OS_AllTraded ||
			rpt->OrderStatus == USTP_FTDC_OS_PartTradedNotQueueing ||
			rpt->OrderStatus == USTP_FTDC_OS_Canceled){
		int32_t counter = strategy.GetCounterByLocalOrderID(rpt->LocalOrderID);
		compliance_.End(counter);
	}
#endif

	strategy.FeedTunnRpt(*rpt, &sig_cnt, sig_buffer_);
	
	if (!strategy.HasFrozenPosition()){
		int i = 0;
		for(; i<MAX_PENDING_SIGNAL_COUNT; i++){
			int32_t st_id = strategy.GetId();
			if(pending_signals_[st_id][i] >= 0){
				int32_t sig_id = pending_signals_[st_id][i];
				pending_signals_[st_id][i] = -1;
				signal_t *sig = strategy.GetSignalBySigID(sig_id);
				PlaceOrder(strategy, *sig);
				 clog_debug("[%s] pending_signals_ get: strategy id:%d; sig_id:%d; exchange:%d; "
							 "symbol:%s; open_volume:%d; buy_price:%f; close_volume:%d; sell_price:%f; "
							 "sig_act:%d; sig_openclose:%d; ",
						module_name_, sig->st_id, sig->sig_id,
						sig->exchange, sig->symbol, sig->open_volume, sig->buy_price,
						sig->close_volume, sig->sell_price, sig->sig_act, sig->sig_openclose); 
			}
		}
	}

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
			signal_t &sig = sigs[i];
			strategy.Push(sig);
			if(strategy.Deferred(sig.sig_id, sig.sig_openclose, sig.sig_act)){
				int i = 0;
				for(; i < MAX_PENDING_SIGNAL_COUNT; i++){
					if(pending_signals_[sig.st_id][i] < 0){
						pending_signals_[sig.st_id][i] = sig.sig_id;
						clog_debug("[%s] pending_signals_ push st id:%d; sig id;%d", 
									module_name_,sig.st_id,pending_signals_[sig.st_id][i]);
						break;
					}
				}
				if(i == MAX_PENDING_SIGNAL_COUNT){
					clog_error("[%s] pending_signals_ beyond;", module_name_);
				}
			} else { PlaceOrder(strategy, sigs[i]); }
		}
	}
}

void UniConsumer::CancelOrder(Strategy &strategy,signal_t &sig)
{
	if (!strategy.HasFrozenPosition()){
		clog_info("[%s] CancelOrder: ignore request due to frozen position.", module_name_); 
		return;
	}
	
	long localorderid = tunn_rpt_producer_->NewLocalOrderID(strategy.GetId());
	long ori_local_order_id = strategy.GetLocalOrderID(sig.orig_sig_id);
    CUstpFtdcOrderActionField *order =  FemasFieldConverter::Convert(localorderid, 
				ori_local_order_id);
	int rtn = tunn_rpt_producer_->ReqOrderAction(order);

	// TODO: debug
	if(0 == ori_local_order_id){
		clog_warning("[%s] CancelOrder: UserOrderActionLocalID:%s; UserOrderLocalID:%s; result:%d", 
			module_name_, order->UserOrderActionLocalID,order->UserOrderLocalID, rtn); 

		signal_t* ori_sig = GetSignalBySigID(sig.orig_sig_id);
		 clog_warning("[%s] CancelOrder ori signal: strategy id:%d; sig_id:%d; "
			 " symbol:%s; open_volume:%d; buy_price:%f; "
			 "close_volume:%d; sell_price:%f; sig_act:%d; sig_openclose:%d; orig_sig_id:%d",
			module_name_, ori_sig.st_id, ori_sig.sig_id, sig.symbol, ori_sig.open_volume, 
			ori_sig.buy_price, ori_sig.close_volume, ori_sig.sell_price, ori_sig.sig_act, 
			ori_sig.sig_openclose, ori_sig.orig_sig_id); 
	}

	clog_info("[%s] CancelOrder: UserOrderActionLocalID:%s; UserOrderLocalID:%s; result:%d", 
		module_name_, order->UserOrderActionLocalID,order->UserOrderLocalID, rtn); 
	if(rtn != 0){
		clog_error("[%s] CancelOrder: UserOrderActionLocalID:%s; UserOrderLocalID:%s; result:%d", 
			module_name_, order->UserOrderActionLocalID,order->UserOrderLocalID, rtn); 
	}


#ifdef LATENCY_MEASURE
		int latency = perf_ctx::calcu_latency(sig.st_id, sig.sig_id);
        if(latency > 0) clog_info("[%s] cancel latency:%d us", module_name_, latency); 
#endif
}

void UniConsumer::PlaceOrder(Strategy &strategy,const signal_t &sig)
{
	int vol = strategy.GetVol(sig);
	int32_t updated_vol = strategy.GetAvailableVol(sig.sig_id, sig.sig_openclose, sig.sig_act, vol);
	long localorderid = tunn_rpt_producer_->NewLocalOrderID(strategy.GetId());
	strategy.PrepareForExecutingSig(localorderid, sig, updated_vol);

	CUstpFtdcInputOrderField *ord =  FemasFieldConverter::Convert(sig, localorderid, updated_vol);
#ifdef COMPLIANCE_CHECK
	int32_t counter = strategy.GetCounterByLocalOrderID(localorderid);
	bool result = compliance_.TryReqOrderInsert(counter, ord->InstrumentID, ord->LimitPrice,
				ord->Direction, ord->OffsetFlag);
	if(result){
#endif
		int32_t rtn = tunn_rpt_producer_->ReqOrderInsert(ord);
		if(rtn != 0){ // feed rejeted info
			TunnRpt rpt;
			memset(&rpt, 0, sizeof(rpt));
			rpt.LocalOrderID = localorderid;
			rpt.OrderStatus = USTP_FTDC_OS_Canceled;
			rpt.ErrorID = rtn;
			int sig_cnt = 0;
			strategy.FeedTunnRpt(rpt, &sig_cnt, sig_buffer_);
			ProcSigs(strategy, sig_cnt, sig_buffer_);

			clog_error("[%s] PlaceOrder rtn:%d; LocalOrderID: %s", module_name_, 
						rtn, ord->UserOrderLocalID);
		}
#ifdef COMPLIANCE_CHECK
	}else{
		clog_error("[%s] matched with myself:%s", module_name_, ord->UserOrderLocalID);

		// feed rejeted info
		TunnRpt rpt;
		memset(&rpt, 0, sizeof(rpt));
		rpt.LocalOrderID = localorderid;
		rpt.OrderStatus = USTP_FTDC_OS_Canceled;
		rpt.ErrorID = 5;
		int sig_cnt = 0;
		strategy.FeedTunnRpt(rpt, &sig_cnt, sig_buffer_);
		ProcSigs(strategy, sig_cnt, sig_buffer_);
	}
#endif

#ifdef LATENCY_MEASURE
	int latency = perf_ctx::calcu_latency(sig.st_id, sig.sig_id);
	if(latency > 0) clog_info("[%s] place latency:%d us", module_name_, latency); 
#endif
}

