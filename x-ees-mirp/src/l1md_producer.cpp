#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <functional>   // std::bind
#include "l1md_producer.h"
#include "quote_cmn_utility.h"
#include <tinyxml.h>
#include <tinystr.h>

using namespace std::placeholders;
using namespace std;

CDepthMarketDataField* L1MDProducerHelper::GetLastDataImp(
			const char *contract, 
			int32_t last_index,
			CDepthMarketDataField *buffer, 
			int32_t buffer_size,
			int32_t traverse_count) 
{
	CDepthMarketDataField* data = NULL;

	// 全息行情需要一档行情时，从缓存最新位置向前查找13个位置（假设有13个主力合约），找到即停
	int i = 0;
	for(; i<traverse_count; i++){
		int data_index = last_index - i;
		if(data_index < 0){
			data_index = data_index + buffer_size;
		}

		CDepthMarketDataField &tmp = buffer[data_index];
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

	memset(&quote_level1_,0,sizeof(quote_level1_));
	// init dominant contracts
	memset(dominant_contracts_, 0, sizeof(dominant_contracts_));
	contract_count_ = LoadDominantContracts(config_.contracts_file, dominant_contracts_);
	max_traverse_count_ = contract_count_ * 4;

#ifdef PERSISTENCE_ENABLED 
    p_level1_save_ = new QuoteDataSave<CDepthMarketDataField>("quote_level1", SHFE_EX_QUOTE_TYPE);
#endif

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
#ifdef PERSISTENCE_ENABLED 
    if (p_level1_save_) delete p_level1_save_;
#endif
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

// lic
CDepthMarketDataField* L1MDProducer::GetLastDataForIllegaluser(const char *contract)
{
	CDepthMarketDataField* data = L1MDProducerHelper::GetLastDataImp(
		contract,0,md_buffer_,L1MD_BUFFER_SIZE,L1MD_BUFFER_SIZE);
	return data;
}

CDepthMarketDataField* L1MDProducer::GetLastData(const char *contract, int32_t last_index)
{
	CDepthMarketDataField* data = L1MDProducerHelper::GetLastDataImp(
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

void L1MDProducer::Convert(CDepthMarketDataField &quote_level1,const CThostFtdcDepthMarketDataField &ctp_data)
{
    strcpy(quote_level1.InstrumentID, ctp_data.InstrumentID); 
    quote_level1.PreSettlementPrice =	ctp_data.PreSettlementPrice; 
    quote_level1.PreClosePrice =		ctp_data.PreClosePrice;      
    quote_level1.PreOpenInterest =		ctp_data.PreOpenInterest;  
    quote_level1.OpenPrice =			ctp_data.OpenPrice;       
    quote_level1.HighestPrice =			ctp_data.HighestPrice; 
    quote_level1.LowestPrice =			ctp_data.LowestPrice;   
    //quote_level1.ClosePrice =			ctp_data.ClosePrice;          
    //quote_level1.SettlementPrice =		ctp_data.SettlementPrice; 
    quote_level1.UpperLimitPrice =		ctp_data.UpperLimitPrice; 
    quote_level1.LowerLimitPrice =		ctp_data.LowerLimitPrice; 
    quote_level1.PreDelta =				ctp_data.PreDelta;            
    quote_level1.CurrDelta =			ctp_data.CurrDelta;           

    // memcpy(quote_level1.UpdateTime, ctp_data.UpdateTime, 9);       
    // quote_level1.UpdateMillisec = ctp_data.UpdateMillisec;      
	
}

void L1MDProducer::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    int error_code = pRspInfo ? 0 : pRspInfo->ErrorID;
    if (error_code != 0) {
        clog_warning("[%s] CTP - OnRspError, code: %d; info: %s", module_name_, error_code, pRspInfo->ErrorMsg);
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

	// TODO: 考虑直接使用CTP行情定义，这样可以减少一次拷贝
	Convert(quote_level1_, *data);
	
	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	vrt_producer_claim(producer_, &vvalue);
	ivalue = cork_container_of(vvalue, struct vrt_hybrid_value,parent);
	ivalue->index = Push(quote_level1_);
	ivalue->data = L1_MD;
	vrt_producer_publish(producer_);

#ifdef PERSISTENCE_ENABLED 
    timeval t;
    gettimeofday(&t, NULL);
    p_level1_save_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, &quote_level1_);
#endif
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

///////////////////////////////////
// 使用盛立API接收行情
//////////////////////////////////////
#ifdef EES_UDP_TOPSPEED_QUOTE

EESQuoteApi *L1MDProducer::LoadQuoteApi()
{
	m_handle =  dlopen(EES_QUOTE_DLL_NAME, RTLD_LAZY);
	if (!m_handle){
		clog_error("[%s] 加载EES行情动态库(%s)失败", module_name_,EES_QUOTE_DLL_NAME);
		return NULL;
	}

	funcCreateEESQuoteApi createFun = (funcCreateEESQuoteApi)dlsym(m_handle, 
				CREATE_EES_QUOTE_API_NAME);
	if (!createFun){
		clog_error("[%s] 获取EES创建函数地址失败!", module_name_);
		return NULL;
	}

	m_distoryFun = (funcDestroyEESQuoteApi)dlsym(m_handle, DESTROY_EES_QUOTE_API_NAME);
	if (!createFun){
		clog_error("[%s] 获取EES销毁函数地址失败!", module_name_);
		return NULL;
	}

	api_ = createFun();
	if (!api_){
		clog_error("[%s] 创建EES行情对象失败!", module_name_);
		return false;
	}
}

void L1MDProducer::UnloadQuoteApi()
{
	if (api_){
		api_->DisConnServer();
		clog_warning("[%s] DisConnServer!", module_name_);
		m_distoryFun(api_);
		api_ = NULL;
		m_distoryFun = NULL;
	}

	if (m_handle){
		dlclose(m_handle);
		m_handle = NULL;
	}
}

void L1MDProducer::InitMDApi()
{
    api_ = LoadQuoteApi();

	// tcp
	EqsTcpInfo tcp_info; 
	memset(&tcp_info, 0, sizeof(EqsTcpInfo));
	strcpy(tcp_info.m_eqsIp, this->config_.mcIp);
	tcp_info.m_eqsPort = this->config_.mcPort;
	vector<EqsTcpInfo> vecTcp;
	vecTcp.push_back(tcp_info);
	bool rtn = api_->ConnServer(vecTcp, this);
	clog_warning("[%s] ConnectServer invoke:%d - mc ip:%s, mc port:%d,",
				module_name_, (int)rtn, vecTcp[0].m_eqsIp, vecTcp[0].m_eqsPort);

	// multicast
//	EqsMulticastInfo emi;
//	strcpy(emi. m_mcIp, this->config_.mcIp);
//	emi.m_mcPort = this->config_.mcPort;
//	strcpy(emi. m_mcLoacalIp, this->config_.mcLocalIp);
//	emi. m_mcLocalPort = this->config_.mcLocalPort;	
//	strcpy(emi.m_exchangeId, "SHFE");
//	vector<EqsMulticastInfo> vecEmi;
//	vecEmi.push_back(emi);
//	bool rtn = this->api_->InitMulticast(vecEmi, this);
//	clog_warning("[%s] InitMulticast invoke:%d - mc ip:%s, mc port:%d,"
//				"mc local ip:%s, mc local port:%d, exchange:%s",
//				module_name_, (int)rtn, vecEmi[0].m_mcIp, vecEmi[0].m_mcPort, 
//				vecEmi[0].m_mcLoacalIp, vecEmi[0].m_mcLocalPort, vecEmi[0].m_exchangeId);
}


void L1MDProducer::OnEqsConnected()
{
	clog_warning("[%s] EES Quote connected.", module_name_);
	EqsLoginParam loginParam;
	strcpy(loginParam.m_loginId, this->config_.userid);
	strcpy(loginParam.m_password, this->config_.password);
	this->api_->LoginToEqs(loginParam);
	clog_warning("[%s] LoginToEqs-user id:%s, pwd:%s.", 
				module_name_, loginParam.m_loginId, loginParam.m_password);
}

void L1MDProducer::OnLoginResponse(bool bSuccess, const char* pReason)
{
	clog_warning("[%s] EES OnLoginResponse-sucess:%d, reason:%s.", 
				module_name_, bSuccess, pReason);

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
			this->api_->RegisterSymbol(EesEqsIntrumentType::EQS_FUTURE, (char*)contr.c_str());
			clog_warning("[%s] EES subscribe:%s",module_name_,contr.c_str());
			start_pos = end_pos + 1;
		}
	}else { clog_error("[%s] EES can't open: %s", module_name_, config_.contracts_file); }
}

void L1MDProducer::OnSymbolRegisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess)
{
	clog_warning("[%s] OnSymbolRegisterResponse-sucess:%d, symbol:%s.", 
				module_name_, (int)bSuccess, pSymbol);
}

void L1MDProducer::OnEqsDisconnected()
{
	clog_warning("[%s] EES Quote disconnected.", module_name_);
}

void L1MDProducer::OnQuoteUpdated(EesEqsIntrumentType chInstrumentType, 
			EESMarketDepthQuoteData* data_src)
{
	clog_info("[%s] OnQuoteUpdated invoked.", module_name_);
	
	if(EQS_FUTURE != chInstrumentType) return;

	if (ended_) return;

	// 抛弃非主力合约
	if(!(IsDominant(data_src->InstrumentID))) return;

	CDepthMarketDataField data;
	RalaceInvalidValue_EES(*data_src,data);
	
	// debug
	//ToString(data);
	//clog_info("[%s] OnRtnDepthMarketData InstrumentID:%s,UpdateTime:%s,UpdateMillisec:%d",
	//	module_name_,data->InstrumentID,data->UpdateTime,data->UpdateMillisec);

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	vrt_producer_claim(producer_, &vvalue);
	ivalue = cork_container_of(vvalue, struct vrt_hybrid_value,parent);
	ivalue->index = Push(data);
	ivalue->data = L1_MD;
	vrt_producer_publish(producer_);

#ifdef PERSISTENCE_ENABLED 
    timeval t;
    gettimeofday(&t, NULL);
    p_level1_save_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, data);
#endif
}

void L1MDProducer::RalaceInvalidValue_EES(EESMarketDepthQuoteData &data_src, 
			CDepthMarketDataField &data_dest)
{
    strcpy(data_dest.TradingDay, data_src.TradingDay);
    strcpy(data_dest.InstrumentID, data_src.InstrumentID);
    // strcpy(data_dest.ExchangeID, data_src.ExchangeID);
    //strcpy(data_dest.ExchangeInstID, data_src.ExchangeInstID);
    data_dest.LastPrice =			InvalidToZeroD(data_src.LastPrice);
	data_dest.PreSettlementPrice =	InvalidToZeroD(data_src.PreSettlementPrice);
    data_dest.PreClosePrice =		InvalidToZeroD(data_src.PreClosePrice);
    data_dest.PreOpenInterest =		InvalidToZeroD(data_src.PreOpenInterest);
    data_dest.OpenPrice =			InvalidToZeroD(data_src.OpenPrice);
    data_dest.HighestPrice =		InvalidToZeroD(data_src.HighestPrice);
    data_dest.LowestPrice =			InvalidToZeroD(data_src.LowestPrice);
    data_dest.Volume =				data_src.Volume;
    data_dest.Turnover =			InvalidToZeroD(data_src.Turnover);
    data_dest.OpenInterest =		InvalidToZeroD(data_src.OpenInterest);
    data_dest.ClosePrice =			InvalidToZeroD(data_src.ClosePrice);
	data_dest.SettlementPrice =		InvalidToZeroD(data_src.SettlementPrice);
    data_dest.UpperLimitPrice =		InvalidToZeroD(data_src.UpperLimitPrice);
    data_dest.LowerLimitPrice =		InvalidToZeroD(data_src.LowerLimitPrice);
    data_dest.PreDelta =			InvalidToZeroD(data_src.PreDelta);
    data_dest.CurrDelta =			InvalidToZeroD(data_src.CurrDelta);
    strcpy(data_dest.UpdateTime, data_src.UpdateTime);
    data_dest.UpdateMillisec =		data_src.UpdateMillisec;

    data_dest.BidPrice1 =			InvalidToZeroD(data_src.BidPrice1);
    data_dest.BidVolume1 =			data_src.BidVolume1;
	data_dest.AskPrice1 =			InvalidToZeroD(data_src.AskPrice1);
    data_dest.AskVolume1 =			data_src.AskVolume1;

//    data_dest.BidPrice2 =			InvalidToZeroD(data_src.BidPrice2);
//    data_dest.BidVolume2 =			data_src.BidVolume2;
//	data_dest.AskPrice2 =			InvalidToZeroD(data_src.AskPrice2);
//    data_dest.AskVolume2 =			data_src.AskVolume2;
//
//    data_dest.BidPrice3 =			InvalidToZeroD(data_src.BidPrice3);
//    data_dest.BidVolume3 =			data_src.BidVolume3;
//	data_dest.AskPrice3 =			InvalidToZeroD(data_src.AskPrice3);
//    data_dest.AskVolume3 =			data_src.AskVolume3;
//
//    data_dest.BidPrice4 =			InvalidToZeroD(data_src.BidPrice4);
//    data_dest.BidVolume4 =			data_src.BidVolume4;
//	data_dest.AskPrice4 =			InvalidToZeroD(data_src.AskPrice4);
//    data_dest.AskVolume4 =			data_src.AskVolume4;
//
//    data_dest.BidPrice5 =			InvalidToZeroD(data_src.BidPrice5);
//    data_dest.BidVolume5 =			data_src.BidVolume5;
//	data_dest.AskPrice5 =			InvalidToZeroD(data_src.AskPrice5);
//    data_dest.AskVolume5 =			data_src.AskVolume5;
//
    data_dest.AveragePrice =		InvalidToZeroD(data_src.AveragePrice);
}


void L1MDProducer::End()
{
	if(!ended_){
		ended_ = true;

		if (api_) {
			this->UnloadQuoteApi();
			clog_warning("[%s] Level quote stop.", module_name_);
			api_ = NULL;
		}

		vrt_producer_eof(producer_);
		clog_warning("[%s] End exit", module_name_);
	}
	fflush (Log::fp);
}
#endif

////////////////////////////////////
// 使用efh_sf_api接收行情
/////////////////////////////////
#ifdef EES_EFH_SF_TOPSPEED_QUOTE
L1MDProducer L1MDProducer::This;
void L1MDProducer::InitMDApi()
{
	L1MDProducer::This = this;
	char ip[20];
	p_SlEfh	= (struct SlEfhQuote*)sl_create_eth_sf_api(config_.efh_sf_eth,
				config_.ip, config_.port);
	if(NULL==p_SlEfh){
		clog_error("[%s] l_create_efh_sf_api-failed.", module_name_);
	}else{
		clog_warning("[%s] sl_create_efh_sf_api-suceeded ip:%s, port:%d",
					module_name_, config_.ip,config_.port);
	}
	int nret = sl_start_etf_quote(p_SlEfh, callback_efh_quote); 
	clog_warning("[%s] sl_start_etf_quote-nret:%d", module_name_, nret);
}

void L1MDProducer::Rev(const struct guava_udp_normal* data)
{
	if (ended_) return;

	// 抛弃非主力合约
	if(!(IsDominant(data->m_symbol))) return;

	RalaceInvalidValue_EES(*data);
	
	// debug
	// ToString(*data);

	//clog_info("[%s] OnRtnDepthMarketData InstrumentID:%s,UpdateTime:%s,UpdateMillisec:%d",
	//	module_name_,data->InstrumentID,data->UpdateTime,data->UpdateMillisec);

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	vrt_producer_claim(producer_, &vvalue);
	ivalue = cork_container_of(vvalue, struct vrt_hybrid_value,parent);
	ivalue->index = Push(*data);
	ivalue->data = L1_MD;
	vrt_producer_publish(producer_);

#ifdef PERSISTENCE_ENABLED 
    timeval t;
    gettimeofday(&t, NULL);
    p_level1_save_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, data);
#endif
}

void L1MDProducer::RalaceInvalidValue_EES(CDepthMarketDataField &d)
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
	d.AskPrice1 = InvalidToZeroD(d.AskPrice1);
	d.SettlementPrice = InvalidToZeroD(d.SettlementPrice);
	d.PreSettlementPrice = InvalidToZeroD(d.PreSettlementPrice);

    d.PreDelta = InvalidToZeroD(d.PreDelta);
    d.CurrDelta = InvalidToZeroD(d.CurrDelta);
}


void L1MDProducer::End()
{
	if(!ended_){
		ended_ = true;

		if (api_) {
			sl_stop_efh_quote( p_SlEfh );
			clog_warning("[%s] CMdclientApi stop: %d",module_name_, err);
			api_ = NULL;
		}

		vrt_producer_eof(producer_);
		clog_warning("[%s] End exit", module_name_);
	}
	fflush (Log::fp);
}

void  callback_efh_quote(struct sl_efh_quote* p, const struct guava_udp_normal* p_quote ) 
{
	L1MDProducer::This->Rev(p_quote);
}
#endif
