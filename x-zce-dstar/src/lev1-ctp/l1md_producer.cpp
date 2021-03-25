#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <functional>   // std::bind
#include "lev1_producer.h"
#include "quote_cmn_utility.h"
#include <tinyxml.h>
#include <tinystr.h>

using namespace std::placeholders;
using namespace std;


Lev1Producer::Lev1Producer(struct vrt_queue  *queue) 
		: module_name_("Lev1Producer")
{
	l1data_cursor_ = L1MD_BUFFER_SIZE - 1;
	is_multicast_ = false;
	ended_ = false;
    api_ = NULL;
	clog_warning("[%s] L1MD_BUFFER_SIZE:%d;",
				module_name_,
				L1MD_BUFFER_SIZE);

	ParseConfig();

	// init dominant contracts
	memset(dominant_contracts_, 0, sizeof(dominant_contracts_));
	contract_count_ = LoadDominantContracts(
				config_.contracts_file, 
				dominant_contracts_);

	memset(&md_buffer_, 0, sizeof(md_buffer_));

	this->producer_ = vrt_producer_new(
				"l1md_producer", 
				1, 
				queue);
	clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0)
	{
		this->producer_ ->yield	= vrt_yield_strategy_threaded();
	}
	else if(strcmp(config_.yield, "spin") == 0)
	{
		this->producer_ ->yield	= vrt_yield_strategy_spin_wait();
	}
	else if(strcmp(config_.yield, "hybrid") == 0)
	{
		this->producer_ ->yield	 = vrt_yield_strategy_hybrid();
	}
}

void Lev1Producer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    

	// yield strategy
    TiXmlElement *disruptor_node = 
		RootElement->FirstChildElement("Disruptor");
	if (disruptor_node != NULL)
	{
		strcpy(config_.yield, disruptor_node->Attribute("yield"));
	} 
	else 
	{ 
		clog_error("[%s] x-shmd.config error: "
					"Disruptor node missing.", 
					module_name_); 
	}

    TiXmlElement *l1md_node = RootElement->FirstChildElement("L1Md");
	if (l1md_node != NULL)
	{
		strcpy(config_.mcIp, l1md_node->Attribute("mcIp"));
		int mcPort = 0;
		 l1md_node->QueryIntAttribute("mcPort", &mcPort);
		this->config_.mcPort = mcPort;

		strcpy(config_.is_multicast, l1md_node->Attribute("isMulticast"));
		if(strcmp(config_.is_multicast, "true")==0)
		{
			is_multicast_ = true;
		}
		else
		{ 
			is_multicast_ = false;
		}
		clog_warning("[%s] is_multicast:%d ", 
					module_name_,
					is_multicast_); 

	} 
	else
	{
		clog_error("[%s] x-shmd.config error: L1Md "
					"node missing.", 
					module_name_); 
	}
	
	// contracts file
    TiXmlElement *contracts_file_node = 
		RootElement->FirstChildElement("Subscription");
	if (contracts_file_node != NULL)
	{
		strcpy(config_.contracts_file, 
					contracts_file_node->Attribute("contracts"));
	}
	else 
	{
		clog_error("[%s] x-shmd.config error: "
					"Subscription node missing.", 
					module_name_); 
	}
}

Lev1Producer::~Lev1Producer()
{
}


int32_t Lev1Producer::Push(const CThostFtdcDepthMarketDataField &md)
{
	l1data_cursor_++;
	if (l1data_cursor_ % L1MD_BUFFER_SIZE == 0)
	{
		l1data_cursor_ = 0;
	}
	md_buffer_[l1data_cursor_] = md;
	return l1data_cursor_;
}

CThostFtdcDepthMarketDataField* Lev1Producer::GetData(int32_t index)
{
	return &md_buffer_[index];
}


/*
 * contract: SR801
 */
bool Lev1Producer::IsDominant(const char *contract)
{
#ifdef PERSISTENCE_ENABLED 
	// 持久化行情时，需要记录所有合约
	//clog_warning("[%s] return TRUE in IsDominant.",module_name_);
	return true;
#else
	return IsDominantSize3Imp((char*)contract, 
				dominant_contracts_, 
				contract_count_);
#endif
}

void Lev1Producer::ToString(CThostFtdcDepthMarketDataField *data)
{
	clog_info("[%s] CThostFtdcDepthMarketDataField \n"
			"InstrumentID:%s \n"
			"TradingDay:%s\n"
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
			"BidPrice1:%f \n"
			"BidVolume1:%d \n"
			"AskPrice1:%f \n"
			"AskVolume1:%d \n"		
			"ActionDay:%s \n",
			module_name_,
			data->InstrumentID,
			data->TradingDay,
			data->LastPrice,
			data->PreSettlementPrice,
			data-> PreClosePrice,
			data->PreOpenInterest,
			data->OpenPrice,
			data-> HighestPrice,
			data-> LowestPrice,
			data->Volume,
			data->Turnover,
			data->OpenInterest,
			data->ClosePrice,
			data->SettlementPrice,
			data->UpperLimitPrice,
			data->LowerLimitPrice,
			data->PreDelta,
			data->CurrDelta,
			data->UpdateTime,
			data->UpdateMillisec,		
			data->BidPrice1,
			data->BidVolume1,
			data->AskPrice1,
			data->AskVolume1,		
			data->ActionDay);

}

void Lev1Producer::OnFrontConnected()
{
    clog_warning("[%s] shfe_ex(CTP): OnFrontConnected", module_name_);

    CThostFtdcReqUserLoginField req_login;
    memset(&req_login, 0, sizeof(CThostFtdcReqUserLoginField));
    api_->ReqUserLogin(&req_login, 0);

    clog_warning("[%s] CTP - request logon", module_name_);
}

void Lev1Producer::OnFrontDisconnected(int nReason)
{
    clog_warning("[%s] CTP - OnFrontDisconnected, nReason: %d", 
				module_name_, 
				nReason);
}

void Lev1Producer::OnRspUserLogin(
			CThostFtdcRspUserLoginField *pRspUserLogin, 
			CThostFtdcRspInfoField *pRspInfo, 
			int nRequestID, 
			bool bIsLast)
{
	// TODO: 考虑订阅的合约从那里获取. 通过dstar API获取所有合约，
	// 保存到文件里，然后有脚本拷贝到指定目录
    int error_code = pRspInfo ? pRspInfo->ErrorID : 0;
    clog_warning("[%s] CTP - OnRspUserLogin, error code: %d", 
				module_name_,
				error_code);

	std::ifstream is;
	int count = 0;
	is.open (config_.contracts_file);
	string contrs = "";
	if (is) 
	{
		getline(is, contrs);
		size_t start_pos = 0;
		size_t end_pos = 0;
		string contr = "";
		while ((end_pos=contrs.find(" ",start_pos)) != string::npos)
		{
			contr = contrs.substr (start_pos, end_pos-start_pos);
			pp_instruments_[count] = new char(strlen(contr.c_str())+1);
			strcpy(pp_instruments_[count],contr.c_str());
			clog_warning("[%s] ThostFtdcMdApi subscribe:%d, %s",
						module_name_, 
						count, 
						pp_instruments_[count]);
			start_pos = end_pos + 1;
			count++;
		}
	}
	else 
	{ 
		clog_error("[%s] ThostFtdcMdApi can't open: %s", 
					module_name_, 
					config_.contracts_file); 
	}

    if (error_code == 0)
	{
        int err = api_->SubscribeMarketData(pp_instruments_, count);
			clog_warning("[%s] TSubscribeMarketData:%d; count:%d",
						module_name_,
						err,
						count);
    } 
	else
	{
        std::string err_str("null");
        if (pRspInfo && pRspInfo->ErrorMsg[0] != '\0') 
		{
            err_str = pRspInfo->ErrorMsg;
        }
        clog_warning("[%s] CTP-Logon fail, error"
					"code: %d; error info: %s",
					module_name_,
					error_code, 
					err_str.c_str());
    }
}

void Lev1Producer::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    clog_warning("[%s] CTP - OnRspSubMarketData, code: %s", 
				module_name_, 
				pSpecificInstrument->InstrumentID);
    if (bIsLast) { }
}

void Lev1Producer::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    clog_warning("[%s] CTP - OnRspUnSubMarketData, code: %s", 
				module_name_, 
				pSpecificInstrument->InstrumentID);
}


void Lev1Producer::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    int error_code = pRspInfo ? 0 : pRspInfo->ErrorID;
    if (error_code != 0)
	{
        clog_warning("[%s] CTP - OnRspError, code: %d; info: %s", 
					module_name_, 
					error_code, 
					pRspInfo->ErrorMsg);
    }
}

void Lev1Producer::Start()
{
	InitMDApi();
}

void Lev1Producer::InitMDApi()
{
	if(is_multicast_)
	{
		api_ = CThostFtdcMdApi::CreateFtdcMdApi("", true, true);
	}
	else
	{
		api_ = CThostFtdcMdApi::CreateFtdcMdApi("", false, false);
	}
    api_->RegisterSpi(this);
	char addr[100];
	sprintf (addr, "tcp://%s:%d", config_.mcIp, config_.mcPort);
	api_->RegisterFront(addr);
    api_->Init();
	clog_warning("[%s] CTP - RegisterFront, addr: %s", module_name_, addr);
}

void Lev1Producer::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *data)
{
	if (ended_) return;

	// 抛弃非主力合约
	if(!(IsDominant(data->InstrumentID))) return;

	ReplaceInvalidValue(data);
	
	// debug
	ToString(data );

	//clog_info("[%s] OnRtnDepthMarketData InstrumentID:%s,UpdateTime:%s,UpdateMillisec:%d",
	//	module_name_,quote_level1_.InstrumentID,quote_level1_.UpdateTime,quote_level1_.UpdateMillisec);

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	vrt_producer_claim(producer_, &vvalue);
	ivalue = cork_container_of(vvalue, struct vrt_hybrid_value,parent);
	ivalue->index = Push(*data);
	ivalue->data = L1_MD;
	vrt_producer_publish(producer_);

}

void Lev1Producer::ReplaceInvalidValue(CThostFtdcDepthMarketDataField *data)
{
    data->Turnover = InvalidToZeroD(data->Turnover);
    data->LastPrice = InvalidToZeroD(data->LastPrice);
    data->UpperLimitPrice = InvalidToZeroD(data->UpperLimitPrice);
    data->LowerLimitPrice = InvalidToZeroD(data->LowerLimitPrice);
    data->HighestPrice = InvalidToZeroD(data->HighestPrice);
    data->LowestPrice = InvalidToZeroD(data->LowestPrice);
    data->OpenPrice = InvalidToZeroD(data->OpenPrice);
    data->ClosePrice = InvalidToZeroD(data->ClosePrice);
    data->PreClosePrice = InvalidToZeroD(data->PreClosePrice);
    data->OpenInterest = InvalidToZeroD(data->OpenInterest);
    data->PreOpenInterest = InvalidToZeroD(data->PreOpenInterest);

	data->SettlementPrice = InvalidToZeroD(data->SettlementPrice);
	data->PreSettlementPrice = InvalidToZeroD(data->PreSettlementPrice);

    data->PreDelta = InvalidToZeroD(data->PreDelta);
    data->CurrDelta = InvalidToZeroD(data->CurrDelta);
}

void Lev1Producer::End()
{
	if(!ended_)
	{
		ended_ = true;

		if (api_) 
		{
			api_->RegisterSpi(NULL);
			api_->Release();
			api_ = NULL;
			clog_warning("[%s] ThostFtdcMdApi stop", module_name_);
		}

		vrt_producer_eof(producer_);
		clog_warning("[%s] End exit", module_name_);
	}
	fflush (Log::fp);
}

