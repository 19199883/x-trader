#include <functional>   // std::bind
#include "md_producer.h"
#include "perfctx.h"

using namespace std::placeholders;
using namespace std;

MDProducer::MDProducer(const ConfigData &cfg,struct vrt_queue  *queue)
:module_name_("MDProducer"),cfg_(cfg)
{
	ended_ = false;
    api_ = NULL;

	InitMDApi(cfg)

	ParseConfig();

	clog_warning("[%s] MD_BUFFER_SIZE: %d;", module_name_, MD_BUFFER_SIZE);

	(this->producer_ = vrt_producer_new("md_producer", 1, queue));
	clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0){
		this->producer_ ->yield	= vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		this->producer_ ->yield	= vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		this->producer_ ->yield	 = vrt_yield_strategy_hybrid();
	}

	auto f_shfemarketdata = std::bind(&MDProducer::OnShfeMarketData, this,_1);
	md_provider_->SetQuoteDataHandler(f_shfemarketdata);
}


void MDProducer::InitMDApi(const ConfigData &cfg)
{
    const LogonConfig &logon_cfg = cfg_.Logon_config();
    int topic = atoi(logon_cfg.topic.c_str());

	int port = -1;
	string ip = "";
    for(const std::string &v : logon_cfg.quote_provider_addrs) {
		size_t ipstr_start = v.find("//")+2;
		size_t ipstr_end = v.find(":",ipstr_start);
		ip = v.substr (ipstr_start, ipstr_end-ipstr_start);
		port = stoi(v.substr(ipstr_end+1));
	}
	char *ip_c_str = (char*)ip.c_str();
    api_ = CMdclientApi::Create(this,port,ip_c_str);
	MY_LOG_INFO("CMdclientApi ip:%s, port:%d",ip.c_str(),port);

	string contr_file = "";
    SubsribeDatas code_list = cfg_.Subscribe_datas();
    BOOST_FOREACH(const std::string &value, code_list){ contr_file = value; }
	std::ifstream is;
	is.open (contr_file);
	string contrs = "";
	if (is) {
		getline(is, contrs);
		size_t start_pos = 0;
		size_t end_pos = 0;
		string contr = "";
		while ((end_pos=contrs.find(" ",start_pos)) != string::npos){
			contr = contrs.substr (start_pos, end_pos-start_pos);
			api_->Subscribe((char*)contr.c_str());
			MY_LOG_INFO("CMdclientApi subscribe:%s",contr.c_str());
			start_pos = end_pos + 1;
		}
		if(contr.size()>0){
			string contr = contrs.substr (start_pos);
			api_->Subscribe((char*)contr.c_str());
			MY_LOG_INFO("CMdclientApi subscribe:%s",contr.c_str());
		}
	}
	else { MY_LOG_INFO("CMdclientApi can't open: %s",contr_file.c_str()); }

    int err = api_->Start();
	MY_LOG_INFO("CMdclientApi start: %d",err);
}

void MDProducer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    

	// yield strategy
    TiXmlElement *comp_node = RootElement->FirstChildElement("Disruptor");
	if (comp_node != NULL){
		strcpy(config_.yield, comp_node->Attribute("yield"));
	} else { clog_error("[%s] x-trader.config error: Disruptor node missing.", module_name_); }
}

MDProducer::~MDProducer(){
    if (api_) {
		int err = api_->Stop();
		MY_LOG_INFO("CMdclientApi stop: %d",err);
        api_ = NULL;
    }
}

void MDProducer::OnRtnDepthMarketData(CDepthMarketDataField *p)
{
	RalaceInvalidValue_Femas(*p);
	CDepthMarketDataField q_level1 = *p;
	// TODO: send to disruptor queue
	if (quote_data_handler_) { quote_data_handler_(&q_level1); }
}

void MDProducer::RalaceInvalidValue_Femas(CDepthMarketDataField &d)
{
    d.Turnover = InvalidToZeroD(d.Turnover);
    d.LastPrice = InvalidToZeroD(d.LastPrice);
    d.UpperLimitPrice = InvalidToZeroD(d.UpperLimitPrice);
    d.LowerLimitPrice = InvalidToZeroD(d.LowerLimitPrice);
    d.HighestPrice = InvalidToZeroD(d.HighestPrice);
    d.LowestPrice = InvalidToZeroD(d.LowestPrice);
    d.OpenPrice = InvalidToZeroD(d.OpenPrice);
    d.ClosePrice = InvalidToZeroD(d.ClosePrice);
    d.PreClosePrice = InvalidToZeroD(d.PreClosePrice);
    d.OpenInterest = InvalidToZeroD(d.OpenInterest);
    d.PreOpenInterest = InvalidToZeroD(d.PreOpenInterest);

    d.BidPrice1 = InvalidToZeroD(d.BidPrice1);
    d.BidPrice2 = InvalidToZeroD(d.BidPrice2);
    d.BidPrice3 = InvalidToZeroD(d.BidPrice3);
    d.BidPrice4 = InvalidToZeroD(d.BidPrice4);
    d.BidPrice5 = InvalidToZeroD(d.BidPrice5);

	d.AskPrice1 = InvalidToZeroD(d.AskPrice1);
    d.AskPrice2 = InvalidToZeroD(d.AskPrice2);
    d.AskPrice3 = InvalidToZeroD(d.AskPrice3);
    d.AskPrice4 = InvalidToZeroD(d.AskPrice4);
    d.AskPrice5 = InvalidToZeroD(d.AskPrice5);

	d.SettlementPrice = InvalidToZeroD(d.SettlementPrice);
	d.PreSettlementPrice = InvalidToZeroD(d.PreSettlementPrice);

    d.PreDelta = InvalidToZeroD(d.PreDelta);
    d.CurrDelta = InvalidToZeroD(d.CurrDelta);
}

MYQuoteData* MDProducer::build_quote_provider(SubscribeContracts &subscription) {
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    
	TiXmlElement* MarketData = RootElement->FirstChildElement("MarketData");
	if (NULL != MarketData) {
		string md_config = MarketData->Attribute("config");
		clog_warning("[%s] MarketData.config: %s", module_name_, md_config.c_str());
		return new MYQuoteData(&subs_, md_config);
	}
	else{
		clog_error("[%s] can not find 'MarkerData' node.", module_name_);
		return NULL;
	}
}

void MDProducer::End()
{
	if(!ended_){
		ended_ = true;
		(vrt_producer_eof(producer_flag1_));
		(vrt_producer_eof(producer_flag_other_));
		clog_warning("[%s] End exit", module_name_);
	}
}

void MDProducer::OnShfeMarketData(const MYShfeMarketData * md)
{
	clog_debug("[%s] thread id:%ld", module_name_,std::this_thread::get_id() );

	if (ended_) return;

	// 目前三个市场，策略支持的品种的合约长度是：5或6个字符
	if (strlen(md->InstrumentID) > 6) return;

#ifdef LATENCY_MEASURE
	// latency measure
	static int cnt = 0;
	perf_ctx::insert_t0(cnt);
	cnt++;
#endif
		struct vrt_producer* producer_ = NULL;
		if(md->data_flag==1){
			producer_ =  producer_flag1_;
		}else{
			producer_ =  producer_flag_other_;
		}

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		(vrt_producer_claim(producer_, &vvalue));
		ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
		ivalue->index = push(*md);
		ivalue->data = SHFEMARKETDATA;
		(vrt_producer_publish(producer_));

	clog_debug("[%s] rev ShfeMarketData: index,%d; data,%d; contract:%s; time:%s",
				module_name_, ivalue->index, ivalue->data, md->InstrumentID,
				md->GetQuoteTime().c_str());
}

int32_t MDProducer::push(const MYShfeMarketData& md){
	static int32_t shfemarketdata_cursor = MD_BUFFER_SIZE - 1;
	shfemarketdata_cursor++;
	if (shfemarketdata_cursor%MD_BUFFER_SIZE == 0){
		shfemarketdata_cursor = 0;
	}
	shfemarketdata_buffer_[shfemarketdata_cursor] = md;

	clog_debug("[%s] push MDBestAndDeep: cursor,%d; contract:%s; time:%s",
				module_name_, shfemarketdata_cursor, md.InstrumentID, 
				md.GetQuoteTime().c_str());

	return shfemarketdata_cursor;
}

MYShfeMarketData* MDProducer::GetShfeMarketData(int32_t index)
{
	return &shfemarketdata_buffer_[index];
}

