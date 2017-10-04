#include <thread>
#include <chrono>
#include <ctime>
#include <ratio>
#include <stdio.h>
#include "tunn_rpt_producer.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "esunny_data_formater.h"

using namespace std::chrono;

static std::string ReadAuthCode()
{
    char l[1024];
    std::string auth_code;
    ifstream auth_cfg("auth_code.dat");
    while (auth_cfg.getline(l, 1023)) {
        auth_code.append(l);
    }

    return auth_code;
}

TunnRptProducer::TunnRptProducer(struct vrt_queue  *queue)
: module_name_("TunnRptProducer")
{
	ready_ = false;
	ended_ = false;
	for(auto &item : cancel_requests_) item = false;
	
	clog_info("[%s] RPT_BUFFER_SIZE: %d;", module_name_, RPT_BUFFER_SIZE);
    // check api version
    clog_info("[%s] TapTradeAPIVersion:%s",module_name_,GetTapTradeAPIVersion());

	this->ParseConfig();
	ESUNNYPacker::InitNewOrder(config_.GetAccount());

	struct vrt_producer  *producer = vrt_producer_new("tunnrpt_producer", 1, queue);
	this->producer_ = producer;
	clog_info("[%s] yield:%s", module_name_, config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0){
		this->producer_ ->yield = vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		this->producer_ ->yield = vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		this->producer_ ->yield = vrt_yield_strategy_hybrid();
	}

	// create esunny object
    TapAPIApplicationInfo auth_info;
    std::string auth_code = ReadAuthCode();
    clog_info("[%s] AuthCode: %s",module_name_,auth_code.c_str());
    strncpy(auth_info.AuthCode, auth_code.c_str(), sizeof(auth_info.AuthCode));
    strcpy(auth_info.KeyOperationLogPath, "");
    TAPIINT32 result;
    api_ = CreateTapTradeAPI(&auth_info, result);
    if (!api_ || result != TAPIERROR_SUCCEED) {
        clog_warning("[%s] CreateTapTradeAPI result:%d",module_name_,result);
        return;
    }
    api_->SetAPINotify(this);
	// address for front machine
	IPAndPortNum ip_port = ParseIPAndPortNum(config_.address);
	api_->SetHostAddress(ip_port.first.c_str(), ip_port.second);
	clog_info("SetHostAddress, addr: %s:%d", ip_port.first.c_str(), ip_port.second);
    //登录服务器
    TapAPITradeLoginAuth stLoginAuth;
    memset(&stLoginAuth, 0, sizeof(stLoginAuth));
    strcpy(stLoginAuth.UserNo, config_.userid.c_str());
    strcpy(stLoginAuth.Password, config_.password.c_str());
    stLoginAuth.ISModifyPassword = APIYNFLAG_NO;
    stLoginAuth.ISDDA = APIYNFLAG_NO;
	stLoginAuth.NoticeIgnoreFlag = TAPI_NOTICE_IGNORE_FUND | TAPI_NOTICE_IGNORE_POSITIONPROFIT |
		TAPI_NOTICE_IGNORE_FILL | TAPI_NOTICE_IGNORE_POSITION | TAPI_NOTICE_IGNORE_CLOSE | 
		TAPI_NOTICE_IGNORE_POSITIONPROFIT;
    result = api_->Login(&stLoginAuth);
    if (TAPIERROR_SUCCEED != result) {
        clog_error("[%s] Login Error, result:%d",module_name_,result);
    }
}

TunnRptProducer::~TunnRptProducer()
{
    if (api_) {
        api_->Disconnect();
        api_ = NULL;
    }
}

void TunnRptProducer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    

	// yield strategy
    TiXmlElement *comp_node = RootElement->FirstChildElement("Disruptor");
	if (comp_node != NULL){
			strcpy(config_.yield, comp_node->Attribute("yield"));
	} else { clog_error("[%s] x-trader.config error: Disruptor node missing.", module_name_); }

    TiXmlElement *tunn_node = RootElement->FirstChildElement("Tunnel");
	if (tunn_node != NULL){
		this->config_.address = tunn_node->Attribute("address");
		this->config_.brokerid = tunn_node->Attribute("brokerid");
		this->config_.userid = tunn_node->Attribute("userid");
		this->config_.password = tunn_node->Attribute("password");

		clog_info("[%s] tunn config:address:%s; brokerid:%s; userid:%s; password:%s",
					module_name_, 
					this->config_.address.c_str(), 
					this->config_.brokerid.c_str(),
					this->config_.userid.c_str(),
					this->config_.password.c_str());
	} else { clog_error("[%s] x-trader.config error: Tunnel node missing.", module_name_); }
}

int TunnRptProducer::ReqOrderInsert(TAPIUINT32 *session, TapAPINewOrder *p)
{
#ifdef LATENCY_MEASURE
	high_resolution_clock::time_point t0 = high_resolution_clock::now();
#endif
	int ret = api_->ReqInsertOrder(session,p);
#ifdef LATENCY_MEASURE
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		int latency = (t1.time_since_epoch().count() - t0.time_since_epoch().count()) / 1000;	
		clog_info("[%s] ReqOrderInsert latency:%d us", 
					module_name_,latency); 
#endif
	if (ret != 0){
		clog_warning("[%s] ReqInsertOrder - return:%d, session_id:%d",
				module_name_,ret, session_id);
	}else {
		clog_debug("[%s] ReqInsertOrder - return:%d, session_id:%d",
				module_name_,ret, session_id);
	}

	return ret;
}

// 撤单操作请求
int TunnRptProducer::ReqOrderAction(CX1FtdcCancelOrderField *p)
{
#ifdef LATENCY_MEASURE
	high_resolution_clock::time_point t0 = high_resolution_clock::now();
#endif
	int counter = GetCounterByLocalOrderID(p->RequestID);
	cancel_requests_[counter] = true;
	int ret = api_->ReqCancelOrder(p);
#ifdef LATENCY_MEASURE
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		int latency = (t1.time_since_epoch().count() - t0.time_since_epoch().count()) / 1000;	
		clog_warning("[%s] ReqOrderAction latency:%d us", 
					module_name_,latency); 
#endif

	if (ret != 0){
		clog_warning("[%s] ReqCancelOrder - ret=%d - %s", 
			module_name_, ret, X1DatatypeFormater::ToString(p).c_str());
	} else {
		clog_debug("[%s] ReqCancelOrder - ret=%d - %s", 
			module_name_, ret, X1DatatypeFormater::ToString(p).c_str());
	}

	return ret;
}

void TunnRptProducer::OnConnect()
{
    clog_info("[%s] OnConnect.", module_name_);
}

void TunnRptProducer::OnRspLogin(TAPIINT32 errorCode, const TapAPITradeLoginRspInfo* loginRspInfo)
{
    clog_info("[%s] OnRspLogin: errorCode:%d,%s",
        module_name_,
		errorCode, ESUNNYDatatypeFormater::ToString(loginRspInfo).c_str());

    if (errorCode == TAPIERROR_SUCCEED) { }
}

void TunnRptProducer::OnAPIReady()
{
    clog_info("[%s] OnAPIReady",module_name_);	
}

void TunnRptProducer::OnDisconnect(TAPIINT32 reasonCode)
{
    clog_error("[%s] OnDisconnect, reasonCode:%d",module_name_,reasonCode);
}

void TunnRptProducer::OnRspChangePassword(TAPIUINT32 sessionID, TAPIINT32 errorCode)
{
    clog_info("[%s] OnRspChangePassword.", module_name_);
}
}

void TunnRptProducer::OnRspSetReservedInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			const TAPISTR_50 info)
{
    clog_info("[%s] OnRspSetReservedInfo.", module_name_);
}

void TunnRptProducer::OnRspQryAccount(TAPIUINT32 sessionID, TAPIUINT32 errorCode,
			TAPIYNFLAG isLast, const TapAPIAccountInfo* info)
{
    clog_info("[%s] OnRspQryAccount.", module_name_);
}

void TunnRptProducer::OnRspQryFund(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			TAPIYNFLAG isLast, const TapAPIFundData* info)
{
    clog_info("[%s] OnRspQryFund.", module_name_);
}

void TunnRptProducer::OnRtnFund(const TapAPIFundData* info) 
{
    clog_info("[%s] OnRtnFund.", module_name_);
}

void TunnRptProducer::OnRspQryExchange(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			TAPIYNFLAG isLast, const TapAPIExchangeInfo* info)
{
    clog_info("[%s] OnRspQryExchange.", module_name_);
}

void TunnRptProducer::OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			TAPIYNFLAG isLast, const TapAPICommodityInfo* info)
{
    clog_info("[%s] OnRspQryCommodity.", module_name_);
}

void TunnRptProducer::OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, 
			TAPIYNFLAG isLast, const TapAPITradeContractInfo* info)
{
    clog_info("[%s] OnRspQryContract: sessionID:%u, errorCode:%d, isLast:%c, %s",
        module_name_,sessionID, errorCode, isLast, 
		ESUNNYDatatypeFormater::ToString(info).c_str());

}

void TunnRptProducer::OnRtnContract(const TapAPITradeContractInfo* info)
{
    clog_info("[%s] OnRtnContract.", module_name_);
}

void TunnRptProducer::OnRspUserLogout(struct CX1FtdcRspUserLogoutInfoField* pf, struct CX1FtdcRspErrorField * pe)
{
    clog_info("[%s] OnRspUserLogout:%s %s",
        module_name_,
		X1DatatypeFormater::ToString(pf).c_str(),
        X1DatatypeFormater::ToString(pe).c_str());
}


void TunnRptProducer::End()
{
	ended_ = true;
	(vrt_producer_eof(producer_));
}

void TunnRptProducer::OnRspInsertOrder(struct CX1FtdcRspOperOrderField* pfield, struct CX1FtdcRspErrorField* perror)
{
    clog_debug("[%s] OnRspInsertOrder ended_:%d", ended_);

	if (ended_) return;

    clog_debug("[%s] OnRspInsertOrder:%s %s",
        module_name_,
		X1DatatypeFormater::ToString(pfield).c_str(),
        X1DatatypeFormater::ToString(perror).c_str());

	if ((pfield != NULL && pfield->OrderStatus == X1_FTDC_SPD_ERROR) ||
		perror != NULL){
		clog_warning("[%s] OnRspInsertOrder:%s %s",
			module_name_,
			X1DatatypeFormater::ToString(pfield).c_str(),
			X1DatatypeFormater::ToString(perror).c_str());
	}

	struct TunnRpt rpt;
    memset(&rpt, 0, sizeof(rpt));
	if (perror != NULL) {
		rpt.LocalOrderID = perror->LocalOrderID;
		rpt.OrderStatus = X1_FTDC_SPD_ERROR;
		rpt.ErrorID = perror->ErrorID;
	}
	else {
		rpt.LocalOrderID = pfield->LocalOrderID;
		rpt.OrderStatus = pfield->OrderStatus;
	}

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = Push(rpt);
	ivalue->data = TUNN_RPT;

	clog_debug("[%s] OnRspInsertOrder: index,%d; data,%d; LocalOrderID:%ld",
				module_name_, ivalue->index, ivalue->data, pfield->LocalOrderID);

	(vrt_producer_publish(producer_));
}

int32_t TunnRptProducer::Push(const TunnRpt& rpt)
{
	static int32_t cursor = RPT_BUFFER_SIZE - 1;
	cursor++;
	if (cursor%RPT_BUFFER_SIZE == 0){
		cursor = 0;
	}
	rpt_buffer_[cursor] = rpt;

	clog_debug("[%s] push TunnRpt: cursor,%d; LocalOrderID:%ld;",
				module_name_, cursor, rpt.LocalOrderID);

	return cursor;
}

void TunnRptProducer::OnRspCancelOrder(struct CX1FtdcRspOperOrderField* pfield, struct CX1FtdcRspErrorField* perror)
{
    clog_debug("[%s] OnRspCancelOrder ended_:%d", ended_);

	if (ended_) return;

    clog_debug("[%s] OnRspCancelOrder:%s %s",
        module_name_,
		X1DatatypeFormater::ToString(pfield).c_str(),
        X1DatatypeFormater::ToString(perror).c_str());

	if ((pfield != NULL && pfield->OrderStatus == X1_FTDC_SPD_ERROR) ||
		perror != NULL){
		clog_warning("[%s] OnRspCancelOrder:%s %s",
			module_name_,
			X1DatatypeFormater::ToString(pfield).c_str(),
			X1DatatypeFormater::ToString(perror).c_str());
	}

	if (pfield != NULL &&
		pfield->OrderStatus == X1_FTDC_SPD_IN_CANCELED){
		struct TunnRpt rpt;
		memset(&rpt, 0, sizeof(rpt));
		rpt.LocalOrderID = pfield->LocalOrderID;
		rpt.OrderStatus = pfield->OrderStatus;

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		(vrt_producer_claim(producer_, &vvalue));
		ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
		ivalue->index = Push(rpt);
		ivalue->data = TUNN_RPT;

		clog_debug("[%s] OnRspCancelOrder: index,%d; data,%d; LocalOrderID:%ld",
					module_name_, ivalue->index, ivalue->data, pfield->LocalOrderID);

		(vrt_producer_publish(producer_));
	}
}

void TunnRptProducer::OnRspQryPosition(struct CX1FtdcRspPositionField* pf, struct CX1FtdcRspErrorField* pe, bool bIsLast)
{
}

void TunnRptProducer::OnRspQryPositionDetail(struct CX1FtdcRspPositionDetailField* pf, struct CX1FtdcRspErrorField* pe, bool bIsLast)
{
}

void TunnRptProducer::OnRspCustomerCapital(struct CX1FtdcRspCapitalField* pf, struct CX1FtdcRspErrorField* pe, bool bIsLast)
{
}

void TunnRptProducer::OnRspQryExchangeInstrument(struct CX1FtdcRspExchangeInstrumentField* pf, struct CX1FtdcRspErrorField* pe,
bool bIsLast)
{
}


void TunnRptProducer::OnRtnErrorMsg(struct CX1FtdcRspErrorField* pfield)
{
    clog_debug("[%s] OnRtnErrorMsg:%d", ended_);

	if (ended_) return;

    clog_warning("[%s] OnRtnErrorMsg:%s", module_name_, X1DatatypeFormater::ToString(pfield).c_str());

	int counter = GetCounterByLocalOrderID(pfield->RequestID);
	if(cancel_requests_[counter]) return; // 对于撤单请求的错误，不需报给策略

	struct TunnRpt rpt;
    memset(&rpt, 0, sizeof(rpt));
	rpt.LocalOrderID = pfield->LocalOrderID;
	rpt.OrderStatus = X1_FTDC_SPD_ERROR;
	rpt.ErrorID = pfield->ErrorID;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = Push(rpt);
	ivalue->data = TUNN_RPT;
	(vrt_producer_publish(producer_));

	clog_debug("[%s] OnRtnErrorMsg: index,%d; data,%d; LocalOrderID:%ld",
				module_name_, ivalue->index, ivalue->data, pfield->LocalOrderID);
}

void TunnRptProducer::OnRtnMatchedInfo(struct CX1FtdcRspPriMatchInfoField* pfield)
{
    clog_debug("[%s] OnRtnMatchedInfo ended_:%d", ended_);

	if (ended_) return;

    clog_debug("[%s] OnRtnMatchedInfo:%s", module_name_, X1DatatypeFormater::ToString(pfield).c_str());

	struct TunnRpt rpt;
    memset(&rpt, 0, sizeof(rpt));
	rpt.LocalOrderID = pfield->LocalOrderID;
	rpt.OrderStatus = pfield->OrderStatus;
	rpt.MatchedAmount = pfield->MatchedAmount;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = Push(rpt);
	ivalue->data = TUNN_RPT;

	clog_debug("[%s] OnRtnMatchedInfo: index,%d; data,%d; LocalOrderID:%ld",
				module_name_, ivalue->index, ivalue->data, pfield->LocalOrderID);

	(vrt_producer_publish(producer_));
}

void TunnRptProducer::OnRtnOrder(struct CX1FtdcRspPriOrderField* pfield)
{
    clog_debug("[%s] OnRtnOrderended_:%d", ended_);

	if (ended_) return;

    clog_debug("[%s] OnRtnOrder:%s", module_name_, X1DatatypeFormater::ToString(pfield).c_str());

	if (pfield->OrderStatus == X1_FTDC_SPD_ERROR){
		clog_warning("[%s] OnRtnOrder:%s",
			module_name_,
			X1DatatypeFormater::ToString(pfield).c_str());
	}

	struct TunnRpt rpt;
    memset(&rpt, 0, sizeof(rpt));
	rpt.LocalOrderID = pfield->LocalOrderID;
	rpt.OrderStatus = pfield->OrderStatus;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = Push(rpt);
	ivalue->data = TUNN_RPT;

	clog_debug("[%s] OnRtnOrder: index,%d; data,%d; LocalOrderID:%ld",
				module_name_, ivalue->index, ivalue->data, pfield->LocalOrderID);

	(vrt_producer_publish(producer_));
}

void TunnRptProducer::OnRtnCancelOrder(struct CX1FtdcRspPriCancelOrderField* pfield)
{
    clog_debug("[%s] OnRtnCancelOrder:%d", ended_);

	if (ended_) return;

    clog_debug("[%s] OnRtnCancelOrder:%s", module_name_, X1DatatypeFormater::ToString(pfield).c_str());

	if (pfield->OrderStatus == X1_FTDC_SPD_ERROR){
		clog_warning("[%s] OnRtnCancelOrder:%s",
			module_name_,
			X1DatatypeFormater::ToString(pfield).c_str());
	}

	if (pfield->OrderStatus == X1_FTDC_SPD_CANCELED ||
		pfield->OrderStatus == X1_FTDC_SPD_PARTIAL_CANCELED ||
		pfield->OrderStatus == X1_FTDC_SPD_IN_CANCELED){
		struct TunnRpt rpt;
		memset(&rpt, 0, sizeof(rpt));
		rpt.LocalOrderID = pfield->LocalOrderID;
		rpt.OrderStatus = pfield->OrderStatus;

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		(vrt_producer_claim(producer_, &vvalue));
		ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
		ivalue->index = Push(rpt);
		ivalue->data = TUNN_RPT;

		clog_debug("[%s] OnRtnCancelOrder: index,%d; data,%d; LocalOrderID:%ld",
					module_name_, ivalue->index, ivalue->data, pfield->LocalOrderID);

		(vrt_producer_publish(producer_));
	}
}

void TunnRptProducer::OnRspQryOrderInfo(struct CX1FtdcRspOrderField* pf, struct CX1FtdcRspErrorField* pe, bool bIsLast)
{
}


void TunnRptProducer::OnRspQryMatchInfo(struct CX1FtdcRspMatchField* pf, struct CX1FtdcRspErrorField* pe, bool bIsLast)
{
}

void TunnRptProducer::OnRtnExchangeStatus(struct CX1FtdcExchangeStatusRtnField* pf)
{
    clog_info("[%s] OnRtnExchangeStatus:%s", module_name_, X1DatatypeFormater::ToString(pf).c_str());
}

long TunnRptProducer::NewLocalOrderID(int32_t strategyid)
{
	static int32_t counter = 1;
    long localorderid = strategyid+ counter * 1000;		
	counter++;

	return localorderid;
}

const char* TunnRptProducer::GetAccount()
{
	return config_.userid.c_str();
}

TunnRpt* TunnRptProducer::GetRpt(int32_t index)
{
	return &(rpt_buffer_[index]);
}

int32_t TunnRptProducer::GetStrategyID(TunnRpt& rpt)
{
	return rpt.LocalOrderID % 1000;
}

int32_t TunnRptProducer::GetCounterByLocalOrderID(long local_ord_id)
{
	return local_ord_id/1000;
}
