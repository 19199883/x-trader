#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <functional>   // std::bind
#include "l1md_producer.h"
#include "quote_cmn_utility.h"
#include <tinyxml.h>
#include <tinystr.h>

using namespace std::placeholders;
using namespace std;

CDepthMarketDataField* L1MDProducerHelper::GetLastDataImp(const char *contract, int32_t last_index,
	CDepthMarketDataField *buffer, int32_t buffer_size, int32_t dominant_contract_count)
{
	CDepthMarketDataField* data = NULL;

	// 全息行情需要一档行情时，从缓存最新位置向前查找13个位置（假设有13个主力合约），找到即停
	int i = 0;
	for(; i<dominant_contract_count; i++){
		int data_index = last_index - i;
		if(data_index < 0){
			data_index = data_index + buffer_size;
		}

		CDepthMarketDataField &tmp = buffer[data_index];
		if(strcmp(contract, tmp.InstrumentID)==0){
			data = &tmp; 
			break;
		}
	}

	//clog_info("GetLastDataImp:dominant_contract_count:%d;i:%d;contract:%s",
	//	dominant_contract_count, i, contract);

	return data;
}

L1MDProducer::L1MDProducer(struct vrt_queue  *queue) : module_name_("L1MDProducer")
{
	l1data_cursor_ = L1MD_BUFFER_SIZE - 1;
	ended_ = false;
    api_ = NULL;
	clog_warning("[%s] L1MD_BUFFER_SIZE:%d;",module_name_,L1MD_BUFFER_SIZE);

	ParseConfig();

	// init dominant contracts
	memset(dominant_contracts_, 0, sizeof(dominant_contracts_));
	dominant_contract_count_ = LoadDominantContracts(config_.contracts_file, dominant_contracts_);

	memset(&md_buffer_, 0, sizeof(md_buffer_));
	InitMDApi();

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
    api_ = CMdclientApi::Create(this,config_.port,config_.ip);
	clog_warning("CMdclientApi ip:%s, port:%d",config_.ip,config_.port);

	std::ifstream is;
	is.open (config_.contracts_file);
	string contrs = "";
	if (is) {
		getline(is, contrs);
		contrs += " ";
		size_t start_pos = 0;
		size_t end_pos = 0;
		string contr = "";
		while ((end_pos=contrs.find(" ",start_pos)) != string::npos){
			contr = contrs.substr (start_pos, end_pos-start_pos);
			api_->Subscribe((char*)contr.c_str());
			clog_warning("CMdclientApi subscribe:%s",contr.c_str());
			start_pos = end_pos + 1;
		}
	}else { clog_error("CMdclientApi can't open: %s",config_.contracts_file); }

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
		strcpy(config_.yield, disruptor_node->Attribute("yield"));
	} else { clog_error("[%s] x-shmd.config error: Disruptor node missing.", module_name_); }

	// addr
    TiXmlElement *l1md_node = RootElement->FirstChildElement("L1Md");
	if (l1md_node != NULL){
		config_.addr = l1md_node->Attribute("addr");
	} else { clog_error("[%s] x-shmd.config error: L1Md node missing.", module_name_); }
	
	// contracts file
    TiXmlElement *contracts_file_node = RootElement->FirstChildElement("Subscription");
	if (contracts_file_node != NULL){
		strcpy(config_.contracts_file, contracts_file_node->Attribute("contracts"));
	} else { clog_error("[%s] x-shmd.config error: Subscription node missing.", module_name_); }

	size_t ipstr_start = config_.addr.find("//")+2;
	size_t ipstr_end = config_.addr.find(":",ipstr_start);
	strcpy(config_.ip, config_.addr.substr(ipstr_start,ipstr_end-ipstr_start).c_str());
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

	// 抛弃非主力合约
	if(!(IsDominant(data->InstrumentID))) return;

	RalaceInvalidValue_Femas(*data);
	
	//clog_info("[%s] OnRtnDepthMarketData InstrumentID:%s,UpdateTime:%s,UpdateMillisec:%d",
	//	module_name_,data->InstrumentID,data->UpdateTime,data->UpdateMillisec);

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	vrt_producer_claim(producer_, &vvalue);
	ivalue = cork_container_of(vvalue, struct vrt_hybrid_value,parent);
	ivalue->index = Push(*data);
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

int32_t L1MDProducer::Push(const CDepthMarketDataField& md){
	l1data_cursor_++;
	if (l1data_cursor_ % L1MD_BUFFER_SIZE == 0){
		l1data_cursor_ = 0;
	}
	md_buffer_[l1data_cursor_] = md;
	return l1data_cursor_;
}

CDepthMarketDataField* L1MDProducer::GetData(int32_t index)
{
	return &md_buffer_[index];
}

CDepthMarketDataField* L1MDProducer::GetLastData(const char *contract, int32_t last_index)
{
	CDepthMarketDataField* data = L1MDProducerHelper::GetLastDataImp(contract, last_index, md_buffer_,
				L1MD_BUFFER_SIZE, dominant_contract_count_);
	return data;
}

bool L1MDProducer::IsDominant(const char *contract)
{
	return IsDominantImp(contract, dominant_contracts_, dominant_contract_count_);
}

void L1MDProducer::ToString(CDepthMarketDataField &data)
{
	clog_info("CDepthMarketDataField\n"
		"TradingDay:%s\n"
		"SettlementGroupID:%s\n"
		"SettlementID:%d\n"
		"LastPrice:%f \n"
		"PreSettlementPrice:%f \n"
		"PreClosePrice:%f \n"
		"PreOpenInterest:%f \n"
		"OpenPrice:%f \n"
		"HighestPrice:%f \n"
		"LowestPrice:%f \n"
		"Volume:%d \n"
		"Turnover:%f \n"
		"OpenInterest:%f \n"
		"ClosePrice:%f \n"
		"SettlementPrice:%f \n"
		"UpperLimitPrice:%f \n"
		"LowerLimitPrice:%f \n"
		"PreDelta:%f \n"
		"CurrDelta:%f \n"
		"UpdateTime[9]:%s \n"
		"UpdateMillisec:%d \n"
		"InstrumentID:%s \n"
		"BidPrice1:%f \n"
		"BidVolume1:%d \n"
		"AskPrice1:%f \n"
		"AskVolume1:%d \n"
		"BidPrice2:%f \n"
		"BidVolume2:%d \n"
		"AskPrice2:%f \n"
		"AskVolume2:%d \n"
		"BidPrice3:%f \n"
		"BidVolume3:%d \n"
		"AskPrice3:%f \n"
		"AskVolume3:%d \n"
		"BidPrice4:%f \n"
		"BidVolume4:%d \n"
		"AskPrice4:%f \n"
		"AskVolume4:%d \n"
		"BidPrice5:%f \n"
		"BidVolume5:%d \n"
		"AskPrice5:%f \n"
		"AskVolume5:%d \n"
		"ActionDay:%s \n",
		data.TradingDay,
		data.SettlementGroupID,
		data.SettlementID,
		data.LastPrice,
		data.PreSettlementPrice,
		data. PreClosePrice,
		data.PreOpenInterest,
		data.OpenPrice,
		data. HighestPrice,
		data. LowestPrice,
		data.Volume,
		data.Turnover,
		data.OpenInterest,
		data.ClosePrice,
		data.SettlementPrice,
		data.UpperLimitPrice,
		data.LowerLimitPrice,
		data.PreDelta,
		data.CurrDelta,
		data.UpdateTime,
		data.UpdateMillisec,
		data.InstrumentID,
		data.BidPrice1,
		data.BidVolume1,
		data.AskPrice1,
		data.AskVolume1,
		data.BidPrice2,
		data.BidVolume2,
		data.AskPrice2,
		data.AskVolume2,
		data.BidPrice3,
		data.BidVolume3,
		data.AskPrice3,
		data.AskVolume3,
		data.BidPrice4,
		data.BidVolume4,
		data.AskPrice4,
		data.AskVolume4,
		data.BidPrice5,
		data.BidVolume5,
		data.AskPrice5,
		data.AskVolume5,
		data.ActionDay);

}
