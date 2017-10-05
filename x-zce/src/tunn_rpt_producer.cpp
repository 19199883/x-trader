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

// done
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

// done
TunnRptProducer::TunnRptProducer(struct vrt_queue  *queue)
: module_name_("TunnRptProducer")
{
	ended_ = false;
	memset(rpt_buffer_,0,sizeof(rpt_buffer_));
	
	clog_info("[%s] RPT_BUFFER_SIZE: %d;", module_name_, RPT_BUFFER_SIZE);
    // check api version
    clog_info("[%s] TapTradeAPIVersion:%s",module_name_,GetTapTradeAPIVersion());

	this->ParseConfig();
	ESUNNYPacker::InitNewOrder(config_.GetAccount());
    memset(&cancel_req_, 0, sizeof(cancel_req_));

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

// done
TunnRptProducer::~TunnRptProducer()
{
    if (api_) {
        api_->Disconnect();
        api_ = NULL;
    }
}

// done
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

// done
int TunnRptProducer::ReqOrderInsert(int32_t localorderid,TAPIUINT32 *session, TapAPINewOrder *p)
{
#ifdef LATENCY_MEASURE
	high_resolution_clock::time_point t0 = high_resolution_clock::now();
#endif
	int ret = api_->ReqInsertOrder(session,p);
	session_localorderid_map_[session_id] = localorderid;
#ifdef LATENCY_MEASURE
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		int latency = (t1.time_since_epoch().count() - t0.time_since_epoch().count()) / 1000;	
		clog_info("[%s] ReqOrderInsert latency:%d us", 
					module_name_,latency); 
#endif
	if (ret != 0){
		clog_warning("[%s] ReqInsertOrder - return:%d, session_id:%u,localorderid:%ld",
				module_name_,ret, session_id,localorderid);
	}else {
		clog_debug("[%s] ReqInsertOrder - return:%d, session_id:%u,localorderid:%ld",
				module_name_,ret, session_id,localorderid);
	}

	return ret;
}

// 撤单操作请求
// done
int TunnRptProducer::ReqOrderAction(int32_t counter)
{
#ifdef LATENCY_MEASURE
	high_resolution_clock::time_point t0 = high_resolution_clock::now();
#endif
	TAPIUINT32 sessionID;
    cancel_req_.ServerFlag = tunnrpt_table_[counter].server_flag;
    memcpy(cancel_req_.OrderNo,tunnrpt_table_[counter].order_no, 
				sizeof(cancel_req_.OrderNo));
	int ret = api_->CancelOrder(&session_id,&cancel_req_);
#ifdef LATENCY_MEASURE
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		int latency = (t1.time_since_epoch().count() - t0.time_since_epoch().count()) / 1000;	
		clog_info("[%s] ReqOrderAction latency:%d us", 
					module_name_,latency); 
#endif

	if (ret != 0){
		clog_warning("[%s] CancelOrder - return:%d, session_id:%d, "
			"counter of original order:%d,server flag:%c,order no:%s", 
			module_name_,ret,session_id,counter,cancel_req_.ServerFlag,
			cancel_req_.OrderNo);
	} else {
		clog_debug("[%s] CancelOrder - return:%d, session_id:%d, "
			"counter of original order:%d,server flag:%c,order no:%s", 
			module_name_,ret,session_id,counter,cancel_req_.ServerFlag,
			cancel_req_.OrderNo);
	}

	return ret;
}

// done
void TunnRptProducer::OnConnect()
{
    clog_info("[%s] OnConnect.", module_name_);
}

// done
void TunnRptProducer::OnRspLogin(TAPIINT32 errorCode, const TapAPITradeLoginRspInfo* loginRspInfo)
{
    clog_info("[%s] OnRspLogin: errorCode:%d,%s",
        module_name_,
		errorCode, ESUNNYDatatypeFormater::ToString(loginRspInfo).c_str());
}

// done
void TunnRptProducer::OnAPIReady()
{
    clog_info("[%s] OnAPIReady",module_name_);	
}

// done
void TunnRptProducer::OnDisconnect(TAPIINT32 reasonCode)
{
    clog_error("[%s] OnDisconnect, reasonCode:%d",module_name_,reasonCode);
}

// done
void TunnRptProducer::OnRspChangePassword(TAPIUINT32 sessionID, TAPIINT32 errorCode)
{
    clog_info("[%s] OnRspChangePassword.", module_name_);
}

// done
void TunnRptProducer::OnRspSetReservedInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			const TAPISTR_50 info)
{
    clog_info("[%s] OnRspSetReservedInfo.", module_name_);
}

// done
void TunnRptProducer::OnRspQryAccount(TAPIUINT32 sessionID, TAPIUINT32 errorCode,
			TAPIYNFLAG isLast, const TapAPIAccountInfo* info)
{
    clog_info("[%s] OnRspQryAccount.", module_name_);
}

// done
void TunnRptProducer::OnRspQryFund(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			TAPIYNFLAG isLast, const TapAPIFundData* info)
{
    clog_info("[%s] OnRspQryFund.", module_name_);
}

// done
void TunnRptProducer::OnRtnFund(const TapAPIFundData* info) 
{
    clog_info("[%s] OnRtnFund.", module_name_);
}

// done
void TunnRptProducer::OnRspQryExchange(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			TAPIYNFLAG isLast, const TapAPIExchangeInfo* info)
{
    clog_info("[%s] OnRspQryExchange.", module_name_);
}

// done
void TunnRptProducer::OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			TAPIYNFLAG isLast, const TapAPICommodityInfo* info)
{
    clog_info("[%s] OnRspQryCommodity.", module_name_);
}

// done
void TunnRptProducer::OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, 
			TAPIYNFLAG isLast, const TapAPITradeContractInfo* info)
{
    clog_info("[%s] OnRspQryContract: sessionID:%u, errorCode:%d, isLast:%c, %s",
        module_name_,sessionID, errorCode, isLast, 
		ESUNNYDatatypeFormater::ToString(info).c_str());
}

// done
void TunnRptProducer::OnRtnContract(const TapAPITradeContractInfo* info)
{
    clog_info("[%s] OnRtnContract.", module_name_);
}

// done
void TunnRptProducer::End()
{
	ended_ = true;
	(vrt_producer_eof(producer_));
}

// done
void TunnRptProducer::OnRtnOrder(const TapAPIOrderInfoNotice* info)
{
    clog_debug("[%s] OnRtnOrder:%s",module_name_, 
				ESUNNYDatatypeFormater::ToString(info).c_str());

	if (ended_) return;

	long localorderid = session_localorderid_map_[info->SessionID];
	int32_t counter = GetCounterByLocalOrderID(localorderid);
	tunnrpt &tunnrpt = tunnrpt_table_[counter];
	if(strcmp(tunnrpt.OrderNo,"") == 0){
		tunnrpt.SessionID = info->SessionID;
		tunnrpt.LocalOrderID = localorderid;
		if (info->ErrorCode == TAPIERROR_SUCCEED) {
			tunnrpt.ServerFlag = info->OrderInfo->ServerFlag;
			strcpy(tunnrpt.OrderNo,info->OrderInfo->OrderNo);
		}
	}

    if (info->ErrorCode != TAPIERROR_SUCCEED) {
		tunnrpt.ErrorID = info->ErrorCode;
		tunnrpt.OrderStatus = TAPI_ORDER_STATE_FAIL;
    }else{
		if (info->OrderInfo->OrderState==TAPI_ORDER_STATE_SUBMITA ||
			info->OrderInfo->OrderState==TAPI_ORDER_STATE_QUEUED ||
			info->OrderInfo->OrderState==TAPI_ORDER_STATE_CANCELING
			) {// discard these reports
			return;
		}

		tunnrpt.OrderStatus = info->OrderInfo.OrderState;
		tunnrpt.MatchedAmount = info->OrderInfo.OrderMatchQty;
	}

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	vrt_producer_claim(producer_, &vvalue);
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = counter;
	ivalue->data = TUNN_RPT;
	vrt_producer_publish(producer_);

	if(tunnrpt.OrderStatus==TAPI_ORDER_STATE_FAIL){
		clog_warning("[%s] OnRtnOrder:%s",module_name_, 
				ESUNNYDatatypeFormater::ToString(info).c_str());
	}
}

// done
// 该接口目前没有用到，所有操作结果通过OnRtnOrder返回.
// log this info only to see whether this method will be invoked.
void MYEsunnyTradeSpi::OnRspOrderAction(TAPIUINT32 sessionID, TAPIUINT32 errorCode,
			const TapAPIOrderActionRsp* info)
{
    clog_debug("[%s] OnRspOrderAction:sessionID:%u,errorCode:%d, %s",
        module_name_,sessionID, errorCode, ESUNNYDatatypeFormater::ToString(info).c_str());
}

void MYEsunnyTradeSpi::OnRspQryOrder(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			TAPIYNFLAG isLast, const TapAPIOrderInfo* info)
{
}

void MYEsunnyTradeSpi::OnRspQryOrderProcess(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			TAPIYNFLAG isLast,
    const TapAPIOrderInfo* info)
{
}

void MYEsunnyTradeSpi::OnRspQryFill(TAPIUINT32 sessionID, TAPIINT32 errorCode, 
			TAPIYNFLAG isLast, const TapAPIFillInfo* info) { }

// discard this info
void MYEsunnyTradeSpi::OnRtnFill(const TapAPIFillInfo* info) { }

void MYEsunnyTradeSpi::OnRspQryPosition(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			TAPIYNFLAG isLast,
    const TapAPIPositionInfo* info) { }

void MYEsunnyTradeSpi::OnRtnPosition(const TapAPIPositionInfo* info) { }

void MYEsunnyTradeSpi::OnRspQryClose(TAPIUINT32 sessionID, TAPIINT32 errorCode, 
			TAPIYNFLAG isLast, const TapAPICloseInfo* info) { }

void MYEsunnyTradeSpi::OnRtnClose(const TapAPICloseInfo* info) { }

void MYEsunnyTradeSpi::OnRtnPositionProfit(const TapAPIPositionProfitNotice* info) { }

void MYEsunnyTradeSpi::OnRspQryDeepQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			TAPIYNFLAG isLast,
    const TapAPIDeepQuoteQryRsp* info) { }

void MYEsunnyTradeSpi::OnRspQryExchangeStateInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			TAPIYNFLAG isLast, const TapAPIExchangeStateInfo* info) {
}

void MYEsunnyTradeSpi::OnRtnExchangeStateInfo(const TapAPIExchangeStateInfoNotice* info)
{
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
