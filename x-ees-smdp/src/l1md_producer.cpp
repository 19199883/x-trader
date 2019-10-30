#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <functional>   // std::bind
#include "l1md_producer.h"
#include "quote_cmn_utility.h"
#include <tinyxml.h>
#include <tinystr.h>

using namespace std::placeholders;
using namespace std;

CThostFtdcDepthMarketDataField* L1MDProducerHelper::GetLastDataImp(
			const char *contract, 
			int32_t last_index,
			CThostFtdcDepthMarketDataField *buffer, 
			int32_t buffer_size,
			int32_t traverse_count) 
{
	CThostFtdcDepthMarketDataField* data = NULL;

	// 全息行情需要一档行情时，从缓存最新位置向前查找13个位置（假设有13个主力合约），找到即停
	int i = 0;
	for(; i<traverse_count; i++){
		int data_index = last_index - i;
		if(data_index < 0){
			data_index = data_index + buffer_size;
		}

		CThostFtdcDepthMarketDataField &tmp = buffer[data_index];
#ifdef ONE_PRODUCT_ONE_CONTRACT
		// 如果一个交易程序中一个品种只有一种合约，那么只需要比较品种部分即可
		if ((contract[0]==tmp.InstrumentID[0] &&
			 contract[1]==tmp.InstrumentID[1])){
#else
		if(IsEqualContract(contract, tmp.InstrumentID)){
#endif
			data = &tmp; 
			break;
		}
	}

	return data;
}

L1MDProducer::L1MDProducer(struct vrt_queue  *queue) : module_name_("L1MDProducer")
{
	l1data_cursor_ = L1MD_BUFFER_SIZE - 1;
	is_multicast_ = false;
	ended_ = false;
    api_ = NULL;
	clog_warning("[%s] L1MD_BUFFER_SIZE:%d;",module_name_,L1MD_BUFFER_SIZE);

	ParseConfig();

	// init dominant contracts
	memset(dominant_contracts_, 0, sizeof(dominant_contracts_));
	contract_count_ = LoadDominantContracts(config_.contracts_file, dominant_contracts_);
	max_traverse_count_ = contract_count_ * 4;

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

    TiXmlElement *l1md_node = RootElement->FirstChildElement("L1Md");
	if (l1md_node != NULL){
		strcpy(config_.efh_sf_eth, l1md_node->Attribute("efh_sf_eth"));

		strcpy(config_.mcIp, l1md_node->Attribute("mcIp"));
		int mcPort = 0;
		 l1md_node->QueryIntAttribute("mcPort", &mcPort);
		this->config_.mcPort = mcPort;

		strcpy(config_.mcLocalIp, l1md_node->Attribute("mcLocalIp"));
		int localUDPPort = 0;
		 l1md_node->QueryIntAttribute("mcLocalPort", &localUDPPort);
		this->config_.mcLocalPort = localUDPPort;

		strcpy(config_.userid, l1md_node->Attribute("userid"));
		strcpy(config_.password, l1md_node->Attribute("password"));

		strcpy(config_.is_multicast, l1md_node->Attribute("isMulticast"));
		if(strcmp(config_.is_multicast, "true")==0){
			is_multicast_ = true;
		}else{ 
			is_multicast_ = false;
		}
		clog_warning("[%s] is_multicast:%d ", module_name_,is_multicast_); 

	} else{
		clog_error("[%s] x-shmd.config error: L1Md node missing.", module_name_); 
	}
	
	// contracts file
    TiXmlElement *contracts_file_node = RootElement->FirstChildElement("Subscription");
	if (contracts_file_node != NULL){
		strcpy(config_.contracts_file, contracts_file_node->Attribute("contracts"));
	} else { clog_error("[%s] x-shmd.config error: Subscription node missing.", module_name_); }
}

L1MDProducer::~L1MDProducer()
{
}


int32_t L1MDProducer::Push(const CThostFtdcDepthMarketDataField& md){
	l1data_cursor_++;
	if (l1data_cursor_ % L1MD_BUFFER_SIZE == 0){
		l1data_cursor_ = 0;
	}
	md_buffer_[l1data_cursor_] = md;
	return l1data_cursor_;
}

CThostFtdcDepthMarketDataField* L1MDProducer::GetData(int32_t index)
{
	return &md_buffer_[index];
}

// lic
CThostFtdcDepthMarketDataField* L1MDProducer::GetLastDataForIllegaluser(const char *contract)
{
	CThostFtdcDepthMarketDataField* data = L1MDProducerHelper::GetLastDataImp(
		contract,0,md_buffer_,L1MD_BUFFER_SIZE,L1MD_BUFFER_SIZE);
	return data;
}

CThostFtdcDepthMarketDataField* L1MDProducer::GetLastData(const char *contract, int32_t last_index)
{
	CThostFtdcDepthMarketDataField* data = L1MDProducerHelper::GetLastDataImp(
		contract,last_index,md_buffer_,L1MD_BUFFER_SIZE,max_traverse_count_);
	return data;
}

bool L1MDProducer::IsDominant(const char *contract)
{
#ifdef PERSISTENCE_ENABLED 
	// 持久化行情时，需要记录所有合约
	//clog_warning("[%s] return TRUE in IsDominant.",module_name_);
	return true;
#else
	return IsDominantImp((char*)contract, dominant_contracts_, contract_count_);
#endif
}

/////////////////
//A使用飞马UDP行情
/////////////////
#ifdef FEMAS_TOPSPEED_QUOTE
void L1MDProducer::OnFrontConnected()
{
    clog_warning("[%s] shfe_ex(CTP): OnFrontConnected", module_name_);

    CThostFtdcReqUserLoginField req_login;
    memset(&req_login, 0, sizeof(CThostFtdcReqUserLoginField));
    api_->ReqUserLogin(&req_login, 0);

    clog_warning("[%s] CTP - request logon", module_name_);
}

void L1MDProducer::OnFrontDisconnected(int nReason)
{
    clog_warning("[%s] CTP - OnFrontDisconnected, nReason: %d", module_name_, nReason);
}

void L1MDProducer::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, 
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    int error_code = pRspInfo ? pRspInfo->ErrorID : 0;
    clog_warning("[%s] CTP - OnRspUserLogin, error code: %d", module_name_,error_code);

	std::ifstream is;
	int count = 0;
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
			pp_instruments_[count] = new char(strlen(contr.c_str())+1);
			strcpy(pp_instruments_[count],contr.c_str());
			clog_warning("[%s] ThostFtdcMdApi subscribe:%d, %s",module_name_, count, 
						pp_instruments_[count]);
			start_pos = end_pos + 1;
			count++;
		}
	}else { clog_error("[%s] ThostFtdcMdApi can't open: %s", module_name_, config_.contracts_file); }

    if (error_code == 0){
        int err = api_->SubscribeMarketData(pp_instruments_, count);
			clog_warning("[%s] TSubscribeMarketData:%d",module_name_,err);
    } else {
        std::string err_str("null");
        if (pRspInfo && pRspInfo->ErrorMsg[0] != '\0') {
            err_str = pRspInfo->ErrorMsg;
        }
        clog_warning("[%s] CTP-Logon fail, error code: %d; error info: %s",module_name_,
			error_code, err_str.c_str());
    }
}

void L1MDProducer::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    clog_warning("[%s] CTP - OnRspSubMarketData, code: %s", module_name_, pSpecificInstrument->InstrumentID);
    if (bIsLast) { }
}

void L1MDProducer::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    clog_warning("[%s] CTP - OnRspUnSubMarketData, code: %s", module_name_, pSpecificInstrument->InstrumentID);
}

void L1MDProducer::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
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

void L1MDProducer::InitMDApi()
{
	if(is_multicast_){
		api_ = CThostFtdcMdApi::CreateFtdcMdApi("", true, true);
	}else{
		api_ = CThostFtdcMdApi::CreateFtdcMdApi("", false, false);
	}
    api_->RegisterSpi(this);
	char addr[100];
	sprintf (addr, "tcp://%s:%d", config_.mcIp, config_.mcPort);
	api_->RegisterFront(addr);
    api_->Init();
	clog_warning("[%s] CTP - RegisterFront, addr: %s", module_name_, addr);
}

void L1MDProducer::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *data)
{
	if (ended_) return;

	// 抛弃非主力合约
	if(!(IsDominant(data->InstrumentID))) return;

	// TODO: commented for debug
	char buffer[5120];
	clog_info("[%s] rev lev1 data:%s",
				module_name_,
				ShfeLev2Formater::Format(*data,buffer) );

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	vrt_producer_claim(producer_, &vvalue);
	ivalue = cork_container_of(vvalue, struct vrt_hybrid_value,parent);
	ivalue->index = Push(*data);
	ivalue->data = L1_MD;
	vrt_producer_publish(producer_);

}

void L1MDProducer::End()
{
	if(!ended_){
		ended_ = true;

		if (api_) {
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
#endif

