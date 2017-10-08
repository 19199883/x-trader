#include <functional>   // std::bind
#include "l1md_producer.h"
#include "quote_cmn_utility.h"

using namespace std::placeholders;
using namespace std;

L1MDProducer::L1MDProducer(struct vrt_queue  *queue) : module_name_("L1MDProducer")
{
	ended_ = false;
    api_ = NULL;

	ParseConfig();
	InitMDApi()

	clog_warning("[%s] L1MD_BUFFER_SIZE:%d;",module_name_,L1MD_BUFFER_SIZE);

	this->producer_ = vrt_producer_new("l1md_producer", 1, queue);
	clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0){
		this->producer_ ->yield	= vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		this->producer_ ->yield	= vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		this->producer_ ->yield	 = vrt_yield_strategy_hybrid();
	}
}

void L1MDProducer::InitMDApi()
{
    api_ = CMdclientApi::Create(this,config_.port.c_str(),config_.ip.c_str());
	clog_warning("CMdclientApi ip:%s, port:%d",config_.ip.c_str(),config_.port);

	std::ifstream is;
	is.open (config_.contracts_file);
	string contrs = "";
	if (is) {
		getline(is, contrs);
		size_t start_pos = 0;
		size_t end_pos = 0;
		string contr = "";
		while ((end_pos=contrs.find(" ",start_pos)) != string::npos){
			contr = contrs.substr (start_pos, end_pos-start_pos);
			api_->Subscribe((char*)contr.c_str());
			clog_warning("CMdclientApi subscribe:%s",contr.c_str());
			start_pos = end_pos + 1;
		}
		if(contr.size()>0){
			string contr = contrs.substr(start_pos);
			api_->Subscribe((char*)contr.c_str());
			clog_warning("CMdclientApi subscribe:%s",contr.c_str());
		}
	}else { clog_error("CMdclientApi can't open: %s",config_.contracts_file.c_str()); }

    int err = api_->Start();
	clog_warning("CMdclientApi start: %d",err);
}

void L1MDProducer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-shmd.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    

	// yield strategy
    TiXmlElement *disruptor_node = RootElement->FirstChildElement("Disruptor");
	if (disruptor_node != NULL){
		config_.yield = disruptor_node->Attribute("yield");
	} else { clog_error("[%s] x-shmd.config error: Disruptor node missing.", module_name_); }

	// addr
    TiXmlElement *l1md_node = RootElement->FirstChildElement("L1Md");
	if (l1md_node != NULL){
		config_.addr = l1md_node->Attribute("addr");
	} else { clog_error("[%s] x-shmd.config error: L1Md node missing.", module_name_); }
	
	// contracts file
    TiXmlElement *contracts_file_node = RootElement->FirstChildElement("Subscription");
	if (contracts_file_node != NULL){
		config_.contracts_file = contracts_file_node->Attribute("contracts");
	} else { clog_error("[%s] x-shmd.config error: Subscription node missing.", module_name_); }

	size_t ipstr_start = config_.addr.find("//")+2;
	size_t ipstr_end = config_.addr.find(":",ipstr_start);
	config_.ip = config_.addr.substr(ipstr_start,ipstr_end-ipstr_start);
	config_.port = stoi(config_.addr.substr(ipstr_end+1));
}

L1MDProducer::~L1MDProducer(){
    if (api_) {
		int err = api_->Stop();
		clog_warning("CMdclientApi stop: %d",err);
        api_ = NULL;
    }
}

void L1MDProducer::OnRtnDepthMarketData(CDepthMarketDataField *data)
{
	if (ended_) return;

	// 目前三个市场，策略支持的品种的合约长度是：5或6个字符
	if (strlen(md->InstrumentID) > 6) return;

	RalaceInvalidValue_Femas(*data);

	clog_info("[%s] OnRtnDepthMarketData InstrumentID:%s,UpdateTime:%s,UpdateMillisec:%d",
			module_name_,data->InstrumentID,data.UpdateTime,data.UpdateMillisec);
	
	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	vrt_producer_claim(producer_, &vvalue);
	ivalue = cork_container_of(vvalue, struct vrt_hybrid_value,parent);
	ivalue->index = push(*md);
	ivalue->data = L1_MD;
	vrt_producer_publish(producer_);
}

void L1MDProducer::RalaceInvalidValue_Femas(CDepthMarketDataField &d)
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

void L1MDProducer::End()
{
	if(!ended_){
		ended_ = true;
		vrt_producer_eof(producer_);
		clog_warning("[%s] End exit", module_name_);
	}
}

int32_t L1MDProducer::push(const CDepthMarketDataField& md){
	static int32_t l1data_cursor = L1MD_BUFFER_SIZE - 1;
	l1data_cursor++;
	if (l1data_cursor % L1MD_BUFFER_SIZE == 0){
		l1data_cursor = 0;
	}
	md_buffer_[l1data_cursor] = md;
	return l1data_cursor;
}

CDepthMarketDataField* L1MDProducer::GetData(int32_t index)
{
	return &md_buffer_[index];
}

