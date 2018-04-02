#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <functional>   // std::bind
#include "l1md_producer.h"
#include "quote_cmn_utility.h"
#include <tinyxml.h>
#include <tinystr.h>

using namespace std::placeholders;
using namespace std;

L1MDProducer::L1MDProducer(struct vrt_queue  *queue) : module_name_("L1MDProducer")
{
	// TODO:
#ifdef PERSISTENCE_ENABLED 
    p_my_shfe_md_save_ = new QuoteDataSave<MYShfeMarketData>("my_shfe_md", 
		MY_SHFE_MD_QUOTE_TYPE);
	p_save_best_and_deep_ = new QuoteDataSave<MDBestAndDeep_MY>("bestanddeepquote",
		DCE_MDBESTANDDEEP_QUOTE_TYPE);
#endif

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
    api_ = CThostFtdcMdApi::CreateFtdcMdApi();
    api_->RegisterSpi(this);
	api_->RegisterFront(config_.addr.c_str());
	MY_LOG_INFO("CTP - RegisterFront, addr: %s", config_.addr.c_str());
    api_->Init();
}

void L1MDProducer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-trader.config");
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

L1MDProducer::~L1MDProducer()
{
    if (api_){
        api_->RegisterSpi(NULL);
        api_->Release();
        api_ = NULL;
    }

#ifdef PERSISTENCE_ENABLED 
    if (p_my_shfe_md_save_) delete p_my_shfe_md_save_;
    if (p_save_best_and_deep_) delete p_save_best_and_deep_;
#endif
}

void L1MDProducer::End()
{
	if(!ended_){
		ended_ = true;

		if (api_) {
			int err = api_->Stop();
			clog_warning("CMdclientApi stop: %d",err);
			api_ = NULL;
		}

		vrt_producer_eof(producer_);
		clog_warning("[%s] End exit", module_name_);
	}
	fflush (Log::fp);
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

bool L1MDProducer::IsDominant(const char *contract)
{
	return IsDominantImp(contract, dominant_contracts_, dominant_contract_count_);
}

void L1MDProducer::ToString(CDepthMarketDataField &data)
{
	clog_warning("CDepthMarketDataField\n"
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



//////////////////////////
void L1MDProducer::OnFrontConnected()
{
    MY_LOG_INFO("shfe_ex(CTP): OnFrontConnected");

    CThostFtdcReqUserLoginField req_login;
    memset(&req_login, 0, sizeof(CThostFtdcReqUserLoginField));
    api_->ReqUserLogin(&req_login, 0);

    MY_LOG_INFO("CTP - request logon");
}

void L1MDProducer::OnFrontDisconnected(int nReason)
{    
    MY_LOG_ERROR("CTP - OnFrontDisconnected, nReason: %d", nReason);    
}

void L1MDProducer::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, 
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    int error_code = pRspInfo ? pRspInfo->ErrorID : 0;
    MY_LOG_INFO("CTP - OnRspUserLogin, error code: %d", error_code);

    if (error_code == 0){
		int count = dominant_contract_count_;
		char *sub_contracts[60];
		for(int i=0; i<count; i++){
			sub_contracts[i] = dominant_contracts_[i];
		}
        api_->SubscribeMarketData(sub_contracts, count);
        MY_LOG_INFO("CTP - SubMarketData, codelist: %s", instruments_.c_str());        
    }else{
        std::string err_str("null");
        if (pRspInfo && pRspInfo->ErrorMsg[0] != '\0'){
            err_str = pRspInfo->ErrorMsg;
        }
        MY_LOG_WARN("CTP - Logon fail, error code: %d; error info: %s", error_code, err_str.c_str());
    }
}

void L1MDProducer::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo,
    int nRequestID, bool bIsLast)
{
    MY_LOG_DEBUG("CTP - OnRspSubMarketData, code: %s", pSpecificInstrument->InstrumentID);    
}

void L1MDProducer::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    MY_LOG_DEBUG("CTP - OnRspUnSubMarketData, code: %s", pSpecificInstrument->InstrumentID);
}

void L1MDProducer::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *p)
{    
	if (ended_) return;

	if(p->ExchangeID==THOST_FTDC_EIDT_SHFE){
		RalaceInvalidValue_CTP(*p);
		Convert(*p,l1md_buffer_);
		// TODO: distinguish two types of market data
		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		vrt_producer_claim(producer_, &vvalue);
		ivalue = cork_container_of(vvalue, struct vrt_hybrid_value,parent);
		ivalue->index = Push(*data);
		ivalue->data = FULL_DEPTH_MD;
		vrt_producer_publish(producer_);
		// TODO:

		// TODO:
#ifdef PERSISTENCE_ENABLED 
		timeval t;
		gettimeofday(&t, NULL);
		// 存起来
		p_save_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, &q_level1);
#endif		
	}else if(p->ExchangeID==THOST_FTDC_EIDT_DCE){
		RalaceInvalidValue_CTP(*p);
		CDepthMarketDataField q_level1 = Convert(*p);

		// TODO: distinguish two types of market data
		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		vrt_producer_claim(producer_, &vvalue);
		ivalue = cork_container_of(vvalue, struct vrt_hybrid_value,parent);
		ivalue->index = Push(*data);
		ivalue->data = BESTANDDEEP;
		vrt_producer_publish(producer_);
	// TODO:

	// TODO:
#ifdef PERSISTENCE_ENABLED 
		timeval t;
		gettimeofday(&t, NULL);
		// 存起来
		p_save_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, &q_level1);
#endif		
	}

	// debug
	// ToString(*data);
	
}

void L1MDProducer::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    int error_code = pRspInfo ? 0 : pRspInfo->ErrorID;
    if (error_code != 0){
        MY_LOG_INFO("CTP - OnRspError, code: %d; info: %s", error_code, pRspInfo->ErrorMsg);
    }
}

void L1MDProducer::RalaceInvalidValue_CTP(CThostFtdcDepthMarketDataField &d)
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
    d.AveragePrice = InvalidToZeroD(d.AveragePrice);

    d.BidPrice1 = InvalidToZeroD(d.BidPrice1);
//    d.BidPrice2 = InvalidToZeroD(d.BidPrice2);
//    d.BidPrice3 = InvalidToZeroD(d.BidPrice3);
//    d.BidPrice4 = InvalidToZeroD(d.BidPrice4);
//    d.BidPrice5 = InvalidToZeroD(d.BidPrice5);

    d.AskPrice1 = InvalidToZeroD(d.AskPrice1);
//    d.AskPrice2 = InvalidToZeroD(d.AskPrice2);
//    d.AskPrice3 = InvalidToZeroD(d.AskPrice3);
//    d.AskPrice4 = InvalidToZeroD(d.AskPrice4);
//    d.AskPrice5 = InvalidToZeroD(d.AskPrice5);

    d.SettlementPrice = InvalidToZeroD(d.SettlementPrice);
    d.PreSettlementPrice = InvalidToZeroD(d.PreSettlementPrice);

//    d.PreDelta = InvalidToZeroD(d.PreDelta);
//    d.CurrDelta = InvalidToZeroD(d.CurrDelta);
}

void L1MDProducer::Convert(const CThostFtdcDepthMarketDataField &ctp_data,
	CDepthMarketDataField &quote_level1)
{
    memset(&quote_level1, 0, sizeof(CDepthMarketDataField));

    memcpy(quote_level1.TradingDay, ctp_data.TradingDay, 9); /// char       TradingDay[9];
    //SettlementGroupID[9];       /// char       SettlementGroupID[9];
    //SettlementID ;        /// int            SettlementID;
    quote_level1.LastPrice = ctp_data.LastPrice;           /// double LastPrice;
    quote_level1.PreSettlementPrice = ctp_data.PreSettlementPrice;  /// double PreSettlementPrice;
    quote_level1.PreClosePrice = ctp_data.PreClosePrice;       /// double PreClosePrice;
    quote_level1.PreOpenInterest = ctp_data.PreOpenInterest;     /// double PreOpenInterest;
    quote_level1.OpenPrice = ctp_data.OpenPrice;           /// double OpenPrice;
    quote_level1.HighestPrice = ctp_data.HighestPrice;        /// double HighestPrice;
    quote_level1.LowestPrice = ctp_data.LowestPrice;         /// double LowestPrice;
    quote_level1.Volume = ctp_data.Volume;              /// int            Volume;
    quote_level1.Turnover = ctp_data.Turnover;            /// double Turnover;
    quote_level1.OpenInterest = ctp_data.OpenInterest;        /// double OpenInterest;
    quote_level1.ClosePrice = ctp_data.ClosePrice;          /// double ClosePrice;
    quote_level1.SettlementPrice = ctp_data.SettlementPrice;     /// double SettlementPrice;
    quote_level1.UpperLimitPrice = ctp_data.UpperLimitPrice;     /// double UpperLimitPrice;
    quote_level1.LowerLimitPrice = ctp_data.LowerLimitPrice;     /// double LowerLimitPrice;
//    quote_level1.PreDelta = ctp_data.PreDelta;            /// double PreDelta;
//    quote_level1.CurrDelta = ctp_data.CurrDelta;           /// double CurrDelta;
    memcpy(quote_level1.UpdateTime, ctp_data.UpdateTime, 9);       /// char       UpdateTime[9]; typedef char TThostFtdcTimeType[9];
    quote_level1.UpdateMillisec = ctp_data.UpdateMillisec;      /// int            UpdateMillisec;
    memcpy(quote_level1.InstrumentID, ctp_data.InstrumentID, 31); /// char       InstrumentID[31]; typedef char TThostFtdcInstrumentIDType[31];
    quote_level1.BidPrice1 = ctp_data.BidPrice1;           /// double BidPrice1;
    quote_level1.BidVolume1 = ctp_data.BidVolume1;          /// int            BidVolume1;
    quote_level1.AskPrice1 = ctp_data.AskPrice1;           /// double AskPrice1;
    quote_level1.AskVolume1 = ctp_data.AskVolume1;          /// int            AskVolume1;
//    quote_level1.BidPrice2 = ctp_data.BidPrice2;           /// double BidPrice2;
//    quote_level1.BidVolume2 = ctp_data.BidVolume2;          /// int            BidVolume2;
//    quote_level1.AskPrice2 = ctp_data.AskPrice2;           /// double AskPrice2;
//    quote_level1.AskVolume2 = ctp_data.AskVolume2;          /// int            AskVolume2;
//    quote_level1.BidPrice3 = ctp_data.BidPrice3;           /// double BidPrice3;
//    quote_level1.BidVolume3 = ctp_data.BidVolume3;          /// int            BidVolume3;
//    quote_level1.AskPrice3 = ctp_data.AskPrice3;           /// double AskPrice3;
//    quote_level1.AskVolume3 = ctp_data.AskVolume3;          /// int            AskVolume3;
//    quote_level1.BidPrice4 = ctp_data.BidPrice4;           /// double BidPrice4;
//    quote_level1.BidVolume4 = ctp_data.BidVolume4;          /// int            BidVolume4;
//    quote_level1.AskPrice4 = ctp_data.AskPrice4;           /// double AskPrice4;
//    quote_level1.AskVolume4 = ctp_data.AskVolume4;          /// int            AskVolume4;
//    quote_level1.BidPrice5 = ctp_data.BidPrice5;           /// double BidPrice5;
//    quote_level1.BidVolume5 = ctp_data.BidVolume5;          /// int            BidVolume5;
//    quote_level1.AskPrice5 = ctp_data.AskPrice5;           /// double AskPrice5;
//    quote_level1.AskVolume5 = ctp_data.AskVolume5;          /// int            AskVolume5;
        //ActionDay[9];        /// char       ActionDay[9];
}
