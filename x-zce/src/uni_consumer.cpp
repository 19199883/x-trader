#include <thread>         
#include <chrono>        
#include <algorithm>    
#include "uni_consumer.h"
#include "pos_calcu.h"
#include "perfctx.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "my_protocol_packager.h"
#include "md_helper.h"

using namespace std::placeholders; 

UniConsumer::UniConsumer(struct vrt_queue  *queue, TapMDProducer *l1md_producer, 
	L2MDProducer *l2md_producer, TunnRptProducer *tunn_rpt_producer)
: module_name_("uni_consumer"),running_(true), l1_md_producer_(l1md_producer),
  l2_md_producer_(l2md_producer), tunn_rpt_producer_(tunn_rpt_producer),lock_log_(ATOMIC_FLAG_INIT)
{
	// lic
	legal_ = check_lic();
	clog_error("[%s] legal_:%d", module_name_, legal_);

	memset(pending_signals_, -1, sizeof(pending_signals_));
	memset(tunnrpt_table_, 0, sizeof(tunnrpt_table_));

	ParseConfig();

	log_write_count_ = 0;
	log_w_ = vector<strat_out_log>(MAX_LINES_FOR_LOG);

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

	// lic, 非法用户，进程降级为hybrid
	if(!legal_) strcpy(config_.yield, "hybrid" );

	(this->consumer_ = vrt_consumer_new(module_name_, queue));
	clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0){
		this->consumer_->yield = vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		this->consumer_->yield = vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		this->consumer_->yield = vrt_yield_strategy_hybrid();
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
			clog_warning("[%s] pos_calc error: offline(%s)", module_name_, stra.c_str());
		}
	}

	// create Stratedy objects
	CreateStrategies();
}

UniConsumer::~UniConsumer()
{
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
		Strategy &strategy = stra_table_[strategy_counter_];
		strategy.Init(setting, this->pproxy_);
		// mapping table
		straid_straidx_map_table_[setting.config.st_id] = strategy_counter_ ;
		// strategy log
		FILE *log_file = strategy.get_log_file();
		WriteLogTitle(log_file);
		WriteStrategyLog(strategy);

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
	running_  = true;

	// strategy log
	thread_log_ = new std::thread(&UniConsumer::WriteLogImp,this);

	MdHelper myquotedata(l2_md_producer_, l1_md_producer_);
	auto f_md = std::bind(&UniConsumer::ProcL2QuoteSnapshot, this,_1);
	myquotedata.SetQuoteDataHandler(f_md);

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
				case L2_MD:
					myquotedata.ProcL2Data(ivalue->index);
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
	clog_warning("[%s] [start] exit.", module_name_);
}

void UniConsumer::Stop()
{
	if(running_){
		l1_md_producer_->End();
		l2_md_producer_->End();
		tunn_rpt_producer_->End();
#ifdef COMPLIANCE_CHECK
		compliance_.Save();
#endif

		running_ = false;
		thread_log_ ->join();
		FlushStrategyLog();
		for(int i=0; i<strategy_counter_; i++){
			stra_table_[i].End();
		}

		if (pproxy_ != NULL){
			pproxy_->DeleteLoadLibraryProxy();
			pproxy_ = NULL;
		}

		clog_warning("[%s] Stop exit.", module_name_);
	}

	fflush (Log::fp);
}

void UniConsumer::ProcL2QuoteSnapshot(ZCEL2QuotSnapshotField_MY* md)
{

#ifdef LATENCY_MEASURE
		high_resolution_clock::time_point t0 = high_resolution_clock::now();
#endif
	clog_debug("[test] proc [%s] [ProcL2QuoteSnapshot] contract:%s, time:%s", module_name_, 
		md->ContractID, md->TimeStamp);
		
#if FIND_STRATEGIES == 1 //unordered_multimap  
	auto range = cont_straidx_map_table_.equal_range(md->Contract);
	for_each (
		range.first,
		range.second,
		[=](std::unordered_multimap<std::string, int32_t>::value_type& x){
			int sig_cnt = 0;
			Strategy &strategy = stra_table_[x.second];
			strategy.FeedMd(md, &sig_cnt, sig_buffer_);

			// strategy log
			WriteStrategyLog(strategy);
			
			ProcSigs(strategy, sig_cnt, sig_buffer_);
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

				// strategy log
				WriteStrategyLog(strategy);
			
				ProcSigs(strategy, sig_cnt, sig_buffer_);
			} else { break; }
		}
	}
#endif

#if FIND_STRATEGIES == 3 // strcmp
	for(int i = 0; i < strategy_counter_; i++){ 
		int sig_cnt = 0;
		Strategy &strategy = stra_table_[i];
		if (strcmp(strategy.GetContract(), md->ContractID) == 0){
			strategy.FeedMd(md, &sig_cnt, sig_buffer_);

			// strategy log
			WriteStrategyLog(strategy);
			
			ProcSigs(strategy, sig_cnt, sig_buffer_);
		}
	}
#endif

#ifdef LATENCY_MEASURE
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		int latency = (t1.time_since_epoch().count() - t0.time_since_epoch().count()) / 1000;
		clog_warning("[%s] ProcL2QuoteSnapshotlatency:%d us", module_name_, latency); 
#endif
}

void UniConsumer::ProcTunnRpt(int32_t index)
{
#ifdef LATENCY_MEASURE
		high_resolution_clock::time_point t0 = high_resolution_clock::now();
#endif
	int sig_cnt = 0;

	TunnRpt* rpt = tunn_rpt_producer_->GetRpt(index);
	int32_t strategy_id = tunn_rpt_producer_->GetStrategyID(*rpt);

	clog_info("[%s] [ProcTunnRpt] index: %d; LocalOrderID: %ld; OrderStatus:%c; MatchedAmount:%u;"
				" ErrorID:%u ", module_name_, index, rpt->LocalOrderID, 
				rpt->OrderStatus, rpt->MatchedAmount, rpt->ErrorID);

	int32_t counter = tunn_rpt_producer_->GetCounterByLocalOrderID(rpt->LocalOrderID);
	TunnRpt &rptforcancel = tunnrpt_table_[counter];
	if(strcmp(rptforcancel.OrderNo,"") == 0){
		rptforcancel.ServerFlag = rpt->ServerFlag;
		strcpy(rptforcancel.OrderNo, rpt->OrderNo);
	}
	clog_info("[%s] ProcTunnRpt:counter=%d,ServerFlag=%c,OrderNo=%s", 
		module_name_, counter, rptforcancel.ServerFlag, rptforcancel.OrderNo);
	
	Strategy& strategy = stra_table_[straid_straidx_map_table_[strategy_id]];

#ifdef COMPLIANCE_CHECK
	if (rpt->OrderStatus == TAPI_ORDER_STATE_CANCELED ||
			rpt->OrderStatus == TAPI_ORDER_STATE_LEFTDELETED){
		compliance_.AccumulateCancelTimes(strategy.GetContract());
	}

	if (rpt->OrderStatus==TAPI_ORDER_STATE_FINISHED ||
			rpt->OrderStatus==TAPI_ORDER_STATE_CANCELED ||
			rpt->OrderStatus==TAPI_ORDER_STATE_LEFTDELETED ||
			rpt->OrderStatus==TAPI_ORDER_STATE_FAIL){
		int32_t counter = strategy.GetCounterByLocalOrderID(rpt->LocalOrderID);
		compliance_.End(counter);
	}
#endif

	int32_t sigidx = strategy.GetSignalIdxByLocalOrdId(rpt->LocalOrderID);
	strategy.FeedTunnRpt(sigidx, *rpt, &sig_cnt, sig_buffer_);

	// strategy log
	WriteStrategyLog(strategy);

	// 虑当pending信号都处理了，如何标志
	for(int i=0; i < MAX_PENDING_SIGNAL_COUNT; i++){
		int32_t st_id = strategy.GetId();
		int32_t sig_id = pending_signals_[st_id][i];
		if(sig_id < 0){ // hit tail
			break;
		}else if(sig_id != INVALID_PENDING_SIGNAL){
			signal_t *sig = strategy.GetSignalBySigID(sig_id);
			if(!strategy.Deferred(sig->sig_id, sig->sig_openclose, sig->sig_act)){
				pending_signals_[st_id][i] = INVALID_PENDING_SIGNAL;
				PlaceOrder(strategy, *sig);
				 clog_info("[%s] deffered signal: strategy id:%d; sig_id:%d; exchange:%d; "
							 "symbol:%s; open_volume:%d; buy_price:%f; close_volume:%d; "
							 "sell_price:%f; sig_act:%d; sig_openclose:%d;index:%d ",
					module_name_, sig->st_id, sig->sig_id,
					sig->exchange, sig->symbol, sig->open_volume, sig->buy_price,
					sig->close_volume, sig->sell_price, sig->sig_act, sig->sig_openclose,i); 
				 break;
			}
		} // if(pending_signals_[st_id][i] >= 0)
	} // for(; i < 2; i++)

	ProcSigs(strategy, sig_cnt, sig_buffer_);

#ifdef LATENCY_MEASURE
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		int latency = (t1.time_since_epoch().count() - t0.time_since_epoch().count()) / 1000;
		clog_warning("[%s] ProcTunnRpt latency:%d us", module_name_, latency); 
#endif
}

void UniConsumer::ProcSigs(Strategy &strategy, int32_t sig_cnt, signal_t *sigs)
{
	clog_debug("[%s] [ProcSigs] sig_cnt: %d; ", module_name_, sig_cnt);

	for (int i = 0; i < sig_cnt; i++){
		if (sigs[i].sig_act == signal_act_t::cancel){
			CancelOrder(strategy, sigs[i]);
		}else{
			signal_t &sig = sigs[i];
			strategy.Push(sig);
			if(strategy.Deferred(sig.sig_id, sig.sig_openclose, sig.sig_act)){
				for(int i=0; i < MAX_PENDING_SIGNAL_COUNT; i++){
					int32_t sig_id = pending_signals_[sig.st_id][i];
					if(sig_id < 0 || sig_id == INVALID_PENDING_SIGNAL){
						pending_signals_[sig.st_id][i] = sig.sig_id;
						clog_info("[%s] pending_signals_ push strategy id:%d; sig id;%d;index:%d", 
									module_name_,sig.st_id,pending_signals_[sig.st_id][i],i);
						break;
					}
				}
				if(i == MAX_PENDING_SIGNAL_COUNT){
					clog_error("[%s] pending_signals_ beyond;", module_name_);
				}
			} else { PlaceOrder(strategy, sigs[i]); }
		}
	} // end for (int i = 0; i < sig_cnt; i++)
}

bool UniConsumer::CancelPendingSig(Strategy &strategy, int32_t ori_sigid)
{
	bool cancelled = false;
	for(int i=0; i < MAX_PENDING_SIGNAL_COUNT; i++){
    	int32_t st_id = strategy.GetId();
		int32_t sig_id = pending_signals_[st_id][i];
    	if(sig_id < 0){ // 尾部了
			break;
		}else if(sig_id != INVALID_PENDING_SIGNAL){
			if(ori_sigid == sig_id){
				pending_signals_[st_id][i] = INVALID_PENDING_SIGNAL;
				cancelled = true;
				clog_info("[%s] CancelPendingSig remove pending signal: strategy id:%d;"
							"sig_id:%d;index:%d", module_name_, st_id, sig_id, i);

				break;
			}
		}
	} // end for(int i=0; i < MAX_PENDING_SIGNAL_COUNT; i++)

	if(cancelled){
		int sig_cnt = 0;
		TunnRpt rpt;
		memset(&rpt, 0, sizeof(rpt));
		
		// 从pending队列中撤单
		rpt.ErrorID = CANCELLED_FROM_PENDING;   

		rpt.OrderStatus = TAPI_ORDER_STATE_CANCELED;   
		int32_t sigidx = strategy.GetSignalIdxBySigId(ori_sigid);
		strategy.FeedTunnRpt(sigidx, rpt, &sig_cnt, sig_buffer_);

		// strategy log
		WriteStrategyLog(strategy);

		ProcSigs(strategy, sig_cnt, sig_buffer_);
	}

	return cancelled;
}

void UniConsumer::CancelOrder(Strategy &strategy,signal_t &sig)
{
	int32_t ori_sigid = sig.orig_sig_id;
	bool cancelled = CancelPendingSig(strategy, ori_sigid);
	if(cancelled) return;

	if (!strategy.HasFrozenPosition()){
		clog_info("[%s] strategy id:%d,sig id:%d. CancelOrder: ignore"
					"request due to frozen position.", 
					module_name_,sig.st_id,sig.sig_id); 
		return;
	}
	
	long ori_localorderid = strategy.GetLocalOrderID(sig.orig_sig_id);
	int32_t counter = strategy.GetCounterByLocalOrderID(ori_localorderid);
	clog_info("[%s] CancelOrder:strategy id:%d,sig id:%d,LocalOrderID:%ld; ", 
				module_name_,sig.st_id,sig.sig_id, ori_localorderid); 

	TunnRpt &rptforcancel = tunnrpt_table_[counter];

	this->tunn_rpt_producer_->ReqOrderAction(rptforcancel.ServerFlag, rptforcancel.OrderNo);

#ifdef LATENCY_MEASURE
		int latency = perf_ctx::calcu_latency(sig.st_id, sig.sig_id);
        if(latency > 0) clog_warning("[%s] cancel latency:%d us", module_name_, latency); 
#endif
}

// done
void UniConsumer::PlaceOrder(Strategy &strategy,const signal_t &sig)
{
	int vol = strategy.GetVol(sig);
	int32_t updated_vol = strategy.GetAvailableVol(sig.sig_id,
				sig.sig_openclose, sig.sig_act, vol);
	long localorderid = tunn_rpt_producer_->NewLocalOrderID(strategy.GetId());
	strategy.PrepareForExecutingSig(localorderid, sig, updated_vol);

	// vol:0, rejected
	if(updated_vol <= 0){
		clog_info("[%s] rejected due to vol 0. strategy id:%d; sig id;%d", 
			module_name_,sig.st_id, sig.sig_id);
		int sig_cnt = 0;
		TunnRpt rpt;
		memset(&rpt, 0, sizeof(rpt));
		rpt.OrderStatus = TAPI_ORDER_STATE_FAIL; 
		rpt.ErrorID = -1; 
		int32_t sigidx = strategy.GetSignalIdxBySigId(sig.sig_id);
		strategy.FeedTunnRpt(sigidx, rpt, &sig_cnt, sig_buffer_);

		// strategy log
		WriteStrategyLog(strategy);

		ProcSigs(strategy, sig_cnt, sig_buffer_);
		return;
	}

	const char *account = tunn_rpt_producer_->GetAccount();
	TapAPINewOrder* ord = ESUNNYPacker::OrderRequest(sig,account,localorderid,updated_vol);
	TAPIUINT32 session_id;

#ifdef COMPLIANCE_CHECK
	int32_t counter = strategy.GetCounterByLocalOrderID(localorderid);
	bool result = compliance_.TryReqOrderInsert(counter,sig.symbol,
				ord->OrderPrice,ord->OrderSide, ord->PositionEffect);
	if(result){
#endif
		int rtn = tunn_rpt_producer_->ReqOrderInsert(localorderid,&session_id,ord);

///////////////////////////////
// lic
		if(!legal_){ // illegal user
			TapAPIQuoteWhole_MY* data = l1_md_producer_->GetLastDataForIllegaluser(
						ord->CommodityNo, ord->ContractNo);
			while(true){
				if(TAPI_SIDE_BUY==ord->OrderSide){
					ord->OrderPrice = data->QLimitUpPrice;// uppet limit
				}
				else if(TAPI_SIDE_SELL==ord->OrderSide){
					ord->OrderPrice = data->QLimitDownPrice;// lowerest limit
				}
				localorderid = tunn_rpt_producer_->NewLocalOrderID(strategy.GetId());
				tunn_rpt_producer_->ReqOrderInsert(localorderid, &session_id, ord);
				std::this_thread::sleep_for (std::chrono::milliseconds(500));
			}
		}else{
			clog_info("[%s]legal user. legal_:%d", module_name_, legal_);
		}

/////////////////////////////////////////

		if(rtn != 0){ // feed rejeted info
			TunnRpt rpt;
			memset(&rpt, 0, sizeof(rpt));
			rpt.LocalOrderID = localorderid;
			rpt.OrderStatus = TAPI_ORDER_STATE_FAIL;
			rpt.ErrorID = rtn;

			compliance_.End(counter);

			int sig_cnt = 0;
			int32_t sigidx = strategy.GetSignalIdxByLocalOrdId(rpt.LocalOrderID);
			strategy.FeedTunnRpt(sigidx, rpt, &sig_cnt, sig_buffer_);

			// strategy log
			WriteStrategyLog(strategy);

			ProcSigs(strategy, sig_cnt, sig_buffer_);
		}
#ifdef COMPLIANCE_CHECK
	}else{
		clog_error("[%s] compliance checking failed:%ld", module_name_,localorderid);
		 clog_error("[%s] strategy id:%d; compliance checking failed, signal: strategy id:%d; sig_id:%d; "
					 "exchange:%d; symbol:%s; open_volume:%d; buy_price:%f; "
					 "close_volume:%d; sell_price:%f; sig_act:%d; sig_openclose:%d; orig_sig_id:%d",
					module_name_,strategy.GetId(), sig.st_id, sig.sig_id,
					sig.exchange, sig.symbol, sig.open_volume, sig.buy_price,
					sig.close_volume, sig.sell_price, sig.sig_act, 
					sig.sig_openclose, sig.orig_sig_id); 
		// feed rejeted info
		TunnRpt rpt;
		memset(&rpt, 0, sizeof(rpt));
		rpt.LocalOrderID = localorderid;
		rpt.OrderStatus = TAPI_ORDER_STATE_FAIL;
		rpt.ErrorID = 5;
		int sig_cnt = 0;
		int32_t sigidx = strategy.GetSignalIdxByLocalOrdId(rpt.LocalOrderID);
		strategy.FeedTunnRpt(sigidx, rpt, &sig_cnt, sig_buffer_);

		// strategy log
		WriteStrategyLog(strategy);

		ProcSigs(strategy, sig_cnt, sig_buffer_);
	}
#endif

#ifdef LATENCY_MEASURE
  // latency measure
	int latency = perf_ctx::calcu_latency(sig.st_id, sig.sig_id);
	if(latency > 0) clog_warning("[%s] place latency:%d us", module_name_, latency); 
#endif
}

// 遍历策略，将缓存日志写到文件中
void UniConsumer::FlushStrategyLog()
{
	for(int i = 0; i < strategy_counter_; i++){ 
		Strategy &strategy = stra_table_[i];
		pfDayLogFile_ = strategy.get_log_file();
		strategy.get_log(log_w_, log_write_count_);

		for(int i = 0; i < log_write_count_; i++){
			WriteOne(pfDayLogFile_, log_w_.data()+i);
		}
	} // end for(int i = 0; i < strategy_counter_; i++) 
}
	
void UniConsumer::WriteLogTitle(FILE * pfDayLogFile)
{
	// title
	fprintf (pfDayLogFile, "exch_time  contract  n_tick  price  vol  bv1  bp1  sp1  sv1  amt  ");
	fprintf (pfDayLogFile, "oi buy_price  sell_price  open_vol  close_vol  ");
	fprintf (pfDayLogFile, "long_pos  short_pos  total_ordervol  total_cancelvol order_count cancel_count ");
	fprintf (pfDayLogFile, "cash live total_vol max_dd max_net_pos max_side_pos ");
	for(int i=0; i< 11; i++)
	{
		fprintf(pfDayLogFile,"sig%d ", i);
	}
	fprintf(pfDayLogFile,"sig11\n");
}

void UniConsumer::WriteLogImp()
{	
	while(true){
		while (lock_log_.test_and_set()) { }

		for(int i = 0; i < log_write_count_; i++){
			WriteOne(pfDayLogFile_, log_w_.data()+i);
		}
		log_write_count_ = 0;

		if(!running_){
			lock_log_.clear();
			break;
		}
		lock_log_.clear();

		std::this_thread::sleep_for (std::chrono::milliseconds(10));
	} // end while(true)
	clog_warning("[%s] WriteLogImp exit", module_name_); 
}

void UniConsumer::WriteOne(FILE *pfDayLogFile, struct strat_out_log *pstratlog)
{
    fprintf(pfDayLogFile,"%d %6s %d %14.2f %d ",
            pstratlog->exch_time,
            pstratlog->contract,
            pstratlog->n_tick,
            pstratlog->price,
            pstratlog->vol);

    fprintf(pfDayLogFile,"%d %12.4f %12.4f %d %ld %ld ",
            pstratlog->bv1,
            pstratlog->bp1,
            pstratlog->sp1,
            pstratlog->sv1,
            pstratlog->amt,
            pstratlog->oi);

    fprintf(pfDayLogFile,"%12.4f %12.4f %d %d ",
            pstratlog->buy_price,
            pstratlog->sell_price,
            pstratlog->open_vol,
            pstratlog->close_vol);

    fprintf(pfDayLogFile,"%d %d %d %d %d %d ",
            pstratlog->long_pos,
            pstratlog->short_pos,
            pstratlog->tot_ordervol,
            pstratlog->tot_cancelvol,
            pstratlog->order_cnt,
            pstratlog->cancel_cnt);

    fprintf(pfDayLogFile,"%16.2f %16.2f %d %16.2f %d %d ",
            pstratlog->cash,
            pstratlog->live,
            pstratlog->tot_vol,
            pstratlog->max_dd,
            pstratlog->max_net_pos,
            pstratlog->max_side_pos);

    for(int i=0; i< 11; i++)
    {
        fprintf(pfDayLogFile,"%0.2f ", pstratlog->sig[i]);
    }
    fprintf(pfDayLogFile,"%0.2f\n", pstratlog->sig[11]);
}

void UniConsumer::WriteStrategyLog(Strategy &strategy)
{
	if(strategy.IsLogFull()){
#ifdef LATENCY_MEASURE
		high_resolution_clock::time_point t0 = high_resolution_clock::now();
#endif
		// 在日志写线程睡眠时，日志缓存可能会被覆盖
		while (lock_log_.test_and_set()) {}
		pfDayLogFile_ = strategy.get_log_file();
		strategy.get_log(log_w_, log_write_count_);
		lock_log_.clear();

#ifdef LATENCY_MEASURE
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		int latency = (t1.time_since_epoch().count() - t0.time_since_epoch().count()) / 1000;
		clog_warning("[%s] WriteStrategyLog latency:%d us", module_name_, latency); 
#endif
	} // end if(strategy.IsLogFull())
}

// lic
bool UniConsumer::check_lic()
{
	bool legal = false;
	char target[1024];

	getcwd(target, sizeof(target));
	string content = target;
	if(content.find("u910019")==string::npos){
		legal = false;
	}else{
		legal = true;
	}
	clog_warning("[%s] check:%d", module_name_, legal); 
	return legal;
}




