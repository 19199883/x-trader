#include <algorithm>    // std::for_each
#include "uni_consumer.h"

UniConsumer::UniConsumer(struct vrt_queue  *queue, MDProducer *md_producer, 
			TunnRptProducer *tunn_rpt_producer,
			PendingSigProducer* pendingsig_producer)
: module_name_("uni_consumer"),running_(true), 
  md_producer_(md_producer),
  tunn_rpt_producer_(tunn_rpt_producer),
  pendingsig_producer_(pendingsig_producer)
{
	clog_info("[%s] STRA_TABLE_SIZE: %d;", module_name_, STRA_TABLE_SIZE);

	(this->consumer_ = vrt_consumer_new(module_name_, queue));
	this->consumer_->yield = vrt_yield_strategy_threaded();

	this->pproxy_ = CLoadLibraryProxy::CreateLoadLibraryProxy();
	this->pproxy_->setModuleLoadLibrary(new CModuleLoadLibraryLinux());
	this->pproxy_->setBasePathLibrary(this->pproxy_->getexedir());

	ParseConfig();
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

void UniConsumer::CreateStrategies()
{
	int32_t i = 0;
	for (auto &setting : this->strategy_settings_){
		stra_table_[i].Init(setting, this->pproxy_);
		// mapping table
		straid_straidx_map_table_.emplace(setting.config.st_id, i);
		// only support one contract for one strategy
		cont_straidx_map_table_.emplace(setting.config.symbols[0].name, i);

		clog_info("[%s] [CreateStrategies] id:%d; contract: %s; maxvol: %d; so:%s ", 
					module_name_, stra_table_[i].GetId(),
					stra_table_[i].GetContract(), 
					stra_table_[i].GetMaxPosition(), 
					stra_table_[i].GetSoFile());

		i++;
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
}

void UniConsumer::ProcBestAndDeep(int32_t index)
{
	MDBestAndDeep_MY* md = md_producer_->GetBestAnddeep(index);

	clog_info("[%s] [ProcBestAndDeep] index: %d; contract: %s", module_name_, index, md->Contract);

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
}

void UniConsumer::ProcOrderStatistic(int32_t index)
{
	MDOrderStatistic_MY* md = md_producer_->GetOrderStatistic(index);

	clog_info("[%s] [ProcOrderStatistic] index: %d; contract: %s", module_name_, index, md->ContractID);

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
}

void UniConsumer::ProcPendingSig(int32_t index)
{
	signal_t* sig = pendingsig_producer_->GetSignal(index);

	clog_info("[%s] [ProcPendingSig] index: %d; sig id: %d", module_name_, index, sig->st_id);

	Strategy& strategy = stra_table_[straid_straidx_map_table_[sig->st_id]];
	PlaceOrder(strategy, *sig);
}

void UniConsumer::ProcTunnRpt(int32_t index)
{
	int sig_cnt = 0;

	TunnRpt* rpt = tunn_rpt_producer_->GetRpt(index);
	int32_t strategy_id = tunn_rpt_producer_->GetStrategyID(*rpt);

	clog_info("[%s] [ProcTunnRpt] index: %d; LocalOrderID: %d", module_name_, index, rpt->LocalOrderID);

	Strategy& strategy = stra_table_[straid_straidx_map_table_[strategy_id]];
	strategy.FeedTunnRpt(*rpt, &sig_cnt, sig_buffer_);
	ProcSigs(strategy, sig_cnt, sig_buffer_);
}

void UniConsumer::ProcSigs(Strategy &strategy, int32_t sig_cnt, signal_t *sigs)
{
	clog_info("[%s] [ProcSigs] sig_cnt: %d; ", module_name_, sig_cnt);

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
    CX1FtdcCancelOrderField cancel_order;
    memset(&cancel_order, 0, sizeof(CX1FtdcCancelOrderField));
	// get LocalOrderID by signal ID
	cancel_order.LocalOrderID = strategy.GetLocalOrderID(sig.orig_sig_id);
	// only use LocalOrderID to cancel order
    cancel_order.X1OrderID = 0; 

	this->tunn_rpt_producer_->ReqOrderAction(&cancel_order);
}

void UniConsumer::PlaceOrder(Strategy &strategy,signal_t &sig)
{
	int32_t vol = 0;
	int32_t updated_vol = 0;
	if (sig.sig_openclose == alloc_position_effect_t::open_){
		vol = sig.open_volume;
	} else if (sig.sig_openclose == alloc_position_effect_t::close_){
		vol = sig.close_volume;
	} else{ clog_info("[%s] PlaceOrder: do support sig_openclose value:%c;", module_name_, sig.sig_openclose); }

	if(strategy.Deferred(sig.sig_openclose, sig.sig_act, vol, updated_vol)){
		// place signal into disruptor queue
		pendingsig_producer_->Push(sig);
	} else {
		long localorderid = this->tunn_rpt_producer_->NewLocalOrderID(strategy.GetId());
		// TODO:need frozed position before placeing order
		strategy.PrepareForExecutingSig(localorderid, sig);

		CX1FtdcInsertOrderField insert_order;
		memset(&insert_order, 0, sizeof(CX1FtdcInsertOrderField));
		X1FieldConverter::Convert(sig, tunn_rpt_producer_->GetAccount(), localorderid, updated_vol, insert_order);

		tunn_rpt_producer_->ReqOrderInsert(&insert_order);
	}
}
