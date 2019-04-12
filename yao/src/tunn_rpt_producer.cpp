#include <thread>
#include <chrono>
#include <ctime>
#include <ratio>
#include <stdio.h>
#include "tunn_rpt_producer.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "ctp_data_formater.h"

using namespace std::chrono;

TunnRptProducer::TunnRptProducer(struct vrt_queue  *queue)
: module_name_("TunnRptProducer")
{
	ended_ = false;

	for(auto &item : cancel_requests_) item = false;
	memset(rpt_buffer_,0,sizeof(rpt_buffer_));

	this->ParseConfig();

	clog_warning("[%s] RPT_BUFFER_SIZE: %d;", module_name_, RPT_BUFFER_SIZE);

	struct vrt_producer  *producer = vrt_producer_new("tunnrpt_producer", 1, queue);
	this->producer_ = producer;
	clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0){
		this->producer_ ->yield = vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		this->producer_ ->yield = vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		this->producer_ ->yield = vrt_yield_strategy_hybrid();
	}

	// create ctp object
	char addr[2048];
	strcpy(addr, this->config_.address.c_str());
	const char* ver = CThostFtdcTraderApi::GetApiVersion();
	clog_warning("[%s] ctp version: %s.", module_name_, ver);

    api_ = CThostFtdcTraderApi::CreateFtdcTraderApi(); 
    api_->RegisterSpi(this);
    api_->SubscribePrivateTopic(THOST_TERT_QUICK);
    api_->SubscribePublicTopic(THOST_TERT_QUICK);
	api_->RegisterFront(addr);
	clog_warning("[%s] RegisterFront, addr: %s", module_name_, addr);
	api_->Init(); 
}

TunnRptProducer::~TunnRptProducer()
{
//	if (this->producer_ != NULL){
//		vrt_producer_free(this->producer_);
//		this->producer_ = NULL;
//		clog_info("[%s] release tunnrpt_producer.", module_name_);
//	}

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

		clog_warning("[%s] tunn config:address:%s; brokerid:%s; userid:%s; password:%s",
					module_name_, 
					this->config_.address.c_str(), 
					this->config_.brokerid.c_str(),
					this->config_.userid.c_str(),
					this->config_.password.c_str());
	} else { clog_error("[%s] x-trader.config error: Tunnel node missing.", module_name_); }
}

// done
int TunnRptProducer::ReqOrderInsert(CThostFtdcInputOrderField *pInputOrder)
{
#ifdef LATENCY_MEASURE
	high_resolution_clock::time_point t0 = high_resolution_clock::now();
#endif
	int ret = api_->ReqInsertOrder(p, 0);
#ifdef LATENCY_MEASURE
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		int latency = (t1.time_since_epoch().count() - t0.time_since_epoch().count()) / 1000;	
		clog_warning("[%s] ReqOrderInsert latency:%d us", 
					module_name_,latency); 
#endif
	
	// report rejected if ret!=0
	if (ret != 0){
		clog_warning("[%s] ReqInsertOrder- ret=%d - %s", 
			module_name_, ret, CtpDatatypeFormater::ToString(p).c_str());
	}else {
		clog_info("[%s] ReqInsertOrder - ret=%d - %s", 
			module_name_, ret, CtpDatatypeFormater::ToString(p).c_str());
	}

	return ret;
}

// done
// 撤单操作请求
int TunnRptProducer::ReqOrderAction(CThostFtdcInputOrderActionField *p)
{
#ifdef LATENCY_MEASURE
	high_resolution_clock::time_point t0 = high_resolution_clock::now();
#endif
	int localordid = stoi(p->OrderRef);
	int counter = GetCounterByLocalOrderID(localordid);
	cancel_requests_[counter] = true;
	int ret = api_->ReqCancelOrder(p, 0);
#ifdef LATENCY_MEASURE
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		int latency = (t1.time_since_epoch().count() - t0.time_since_epoch().count()) / 1000;	
		clog_warning("[%s] ReqOrderAction latency:%d us", 
					module_name_,latency); 
#endif

	if (ret != 0){
		clog_warning("[%s] ReqCancelOrder - ret=%d - %s", 
			module_name_, ret, CtpDatatypeFormater::ToString(p).c_str());
	} else {
		clog_info("[%s] ReqCancelOrder - ret=%d - %s", 
			module_name_, ret, CtpDatatypeFormater::ToString(p).c_str());
	}

	return ret;
}


// done
void TunnRptProducer::ReqLogin()
{
    CThostFtdcReqUserLoginField login_data;
    memset(&login_data, 0, sizeof(CThostFtdcReqUserLoginField));
	strncpy(login_data.BrokerID, this->config_.brokerid.c_str(), sizeof(login_data.BrokerID));
    strncpy(login_data.UserID, this->config_.userid.c_str(), sizeof(login_data.UserID));
    strncpy(login_data.Password, this->config_.password.c_str(), sizeof(login_data.Password));
    
	int rtn = api_->ReqUserLogin2(&login_data);

    clog_warning("[%s] ReqLogin:  err_no,%d",module_name_, rtn );
    clog_warning("[%s] ReqLogin:   %s", 
			module_name_, CtpDatatypeFormater::ToString(&login_data).c_str());
}

// done
void TunnRptProducer::OnFrontConnected()
{
    clog_warning("[%s] OnFrontConnected.", module_name_);
	this->ReqLogin();
}

// done
void TunnRptProducer::OnFrontDisconnected(int nReason)
{
    clog_error("[%s] OnFrontDisconnected, nReason=%d", module_name_, nReason);
}

// done
void TunnRptProducer::OnHeartBeatWarning(int nTimeLapse)
{
	clog_warning("[%s] OnHeartBeatWarning.", module_name_);
}

// done
void TunnRptProducer::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, 
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    clog_warning("[%s] OnRspUserLogin:%s %s",
        module_name_,
		CtpDatatypeFormater::ToString(pRspUserLogin).c_str(),
        CtpDatatypeFormater::ToString(pRspInfo).c_str());

	strcpy(this->TradingDay_, pRspUserLogin->TradingDay);
	this->FrontID_ = pRspUserLogin->FrontID;	
	this->SessionID_ = pRspUserLogin->SessionID;	
	this->MaxOrderRef_ = pRspUserLogin->MaxOrderRef;
	
    if (pRspInfo==NULL || 0==pRspInfo->ErrorID) {
		clog_warning("[%s] OnRspUserLogin successfully.", module_name_);
    }
    else {
		clog_error("[%s] OnRspUserLogin, error: %d, msg: %s", module_name_, 
			pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
}

// done
void TunnRptProducer::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, 
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    clog_info("[%s] OnRspUserLogout:%s %s",
        module_name_,
		CtpDatatypeFormater::ToString(pUserLogout).c_str(),
        CtpDatatypeFormater::ToString(pRspInfo).c_str());
		
	if (pRspInfo==NULL || 0==pRspInfo->ErrorID) {
		clog_warning("[%s] OnRspUserLogout successfully.", module_name_);
		if (NULL != api_) {
			api_->RegisterSpi(NULL);
			api_->Release();
			api_ = NULL;
			clog_warning("[%s]api release.", module_name_);
		}
		(vrt_producer_eof(producer_));
		clog_warning("[%s] End exit", module_name_);
    }
    else {
		clog_error("[%s] OnRspUserLogout, error: %d, msg: %s", module_name_, 
			pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
}

// done
bool TunnRptProducer::IsFinal(TThostFtdcOrderStatusType   OrderStatus)
{
	if(THOST_FTDC_OST_AllTraded==OrderStatus ||
		THOST_FTDC_OST_Canceled==OrderStatus ||
		THOST_FTDC_OST_NoTradeNotQueueing==OrderStatus ||
		THOST_FTDC_OST_PartTradedNotQueueing==OrderStatus ||
		THOST_FTDC_OAS_Rejected==OrderStatus){
			return true;
		}else{
			return false;
		}
}

// done
void TunnRptProducer::End()
{
	if(!ended_){
		CThostFtdcUserLogoutField logoutinfo = {0};		 
		strncpy(logoutinfo.BrokerID, this->config_.brokerid.c_str(), sizeof(logoutinfo.BrokerID));
		strncpy(logoutinfo.UserID, this->config_.userid.c_str(), sizeof(logoutinfo.UserID));		    
		int rtn = api_->ReqUserLogout(&logoutinfo);
		
		ended_ = true;		
	}
}

// done
void TunnRptProducer::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, 
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (ended_) return;
	
	 clog_info("[%s] OnRspOrderInsert:%s %s; bIsLast: %d",
        module_name_,
		CtpDatatypeFormater::ToString(pInputOrder).c_str(),
        CtpDatatypeFormater::ToString(pRspInfo).c_str(),
		bIsLast);
		
	if (pRspInfo==NULL || 0==pRspInfo->ErrorID) {				
    }else {		
		 clog_error("[%s] OnRspOrderInsert:%s %s",
        module_name_,
		CtpDatatypeFormater::ToString(pInputOrder).c_str(),
        CtpDatatypeFormater::ToString(pRspInfo).c_str());
				
		int32_t cursor = Push();		
		struct TunnRpt &rpt = rpt_buffer_[cursor];	 // LocalOrderID也只需要赋值一次	
		rpt.LocalOrderID = stoi(pInputOrder->OrderRef);
		rpt.OrderStatus = THOST_FTDC_OAS_Rejected;
		rpt.ErrorID = perror->ErrorID;

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		(vrt_producer_claim(producer_, &vvalue));
		ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
		ivalue->index = cursor;
		ivalue->data = TUNN_RPT;
	
		clog_info("[%s] OnRspInsertOrder: index,%d; data,%d; LocalOrderID:%s",
			module_name_, ivalue->index, ivalue->data, pfield->OrderRef);
	
		(vrt_producer_publish(producer_));
	}
}

// done
void TunnRptProducer::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, 
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	clog_info("[%s] pInputOrderAction:%s %s; bIsLast: %d",
        module_name_,
		CtpDatatypeFormater::ToString(pInputOrderAction).c_str(),
        CtpDatatypeFormater::ToString(pRspInfo).c_str(),
		bIsLast);
		
	if (pRspInfo==NULL || 0==pRspInfo->ErrorID) {				
    }else {		
		 clog_error("[%s] pInputOrderAction:%s %s",
        module_name_,
		CtpDatatypeFormater::ToString(pInputOrderAction).c_str(),
        CtpDatatypeFormater::ToString(pRspInfo).c_str());
}
		
// done
void TunnRptProducer::OnRspError(CThostFtdcRspInfoField *pRspInfo, 
	int nRequestID, bool bIsLast)
{
	if (pRspInfo==NULL || 0==pRspInfo->ErrorID) {				
    }else {		
		 clog_error("[%s] pInputOrderAction:%s",
        module_name_,		
        CtpDatatypeFormater::ToString(pRspInfo).c_str());	
}
	
// done
void TunnRptProducer::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	if (ended_) return;

    clog_info("[%s] OnRtnOrder:%s", module_name_, CtpDatatypeFormater::ToString(pOrder).c_str());

	if (pOrder->OrderStatus == THOST_FTDC_OAS_Rejected){
		clog_warning("[%s] OnRtnOrder:%s",
			module_name_,
			CtpDatatypeFormater::ToString(pOrder).c_str());
	}

	int32_t cursor = Push();
	struct TunnRpt &rpt = rpt_buffer_[cursor];
	rpt.LocalOrderID = stoi(pOrder->OrderRef);
	rpt.OrderStatus = pOrder->OrderStatus;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = cursor;
	ivalue->data = TUNN_RPT;

	clog_debug("[%s] OnRtnOrder: index,%d; data,%d; OrderRef:%s",
		module_name_, ivalue->index, ivalue->data, pOrder->OrderRef);

	(vrt_producer_publish(producer_));
}

// TODO: 如何自动处理平今仓，平昨仓问题 启动时查仓位

// TODO: to here
void TunnRptProducer::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	if (ended_) return;

    clog_info("[%s] OnRtnTrade:%s", module_name_, CtpDatatypeFormater::ToString(pTrade).c_str());

	int32_t cursor = Push();
	struct TunnRpt &rpt = rpt_buffer_[cursor];
	rpt.LocalOrderID = stoi(pfield->OrderRef);
	// TODO: 因为没有状态，所以需要在strategy中根据累计成交量来确定部分成交还是完全成交
	rpt.OrderStatus = pfield->OrderStatus;
	rpt.MatchedAmount = pfield->MatchedAmount;
	rpt.MatchedPrice = pfield->MatchedPrice;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = cursor;
	ivalue->data = TUNN_RPT;

	clog_info("[%s] OnRtnTrade: index,%d; data,%d; LocalOrderID:%ld",
				module_name_, ivalue->index, ivalue->data, pfield->LocalOrderID);

	(vrt_producer_publish(producer_));
}


//////////////////////////////////////
/////////////// old below
void TunnRptProducer::OnRspInsertOrder(struct CX1FtdcRspOperOrderField* pfield, struct CX1FtdcRspErrorField* perror)
{
    clog_debug("[%s] OnRspInsertOrder ended_:%d", ended_);

	if (ended_) return;

    clog_info("[%s] OnRspInsertOrder:%s %s",
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

	int32_t cursor = Push();
	// LocalOrderID也只需要赋值一次
	struct TunnRpt &rpt = rpt_buffer_[cursor];
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
	ivalue->index = cursor;
	ivalue->data = TUNN_RPT;

	clog_info("[%s] OnRspInsertOrder: index,%d; data,%d; LocalOrderID:%ld",
				module_name_, ivalue->index, ivalue->data, pfield->LocalOrderID);

	(vrt_producer_publish(producer_));
}


int32_t TunnRptProducer::Push()
{
	static int32_t cursor = RPT_BUFFER_SIZE - 1;
	cursor++;
	if (cursor%RPT_BUFFER_SIZE == 0){
		cursor = 0;
	}

	return cursor;
}

void TunnRptProducer::OnRspCancelOrder(struct CX1FtdcRspOperOrderField* pfield, struct CX1FtdcRspErrorField* perror)
{
    clog_debug("[%s] OnRspCancelOrder ended_:%d", ended_);

	if (ended_) return;

    clog_info("[%s] OnRspCancelOrder:%s %s",
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
		int32_t cursor = Push();
		struct TunnRpt &rpt = rpt_buffer_[cursor];
		rpt.LocalOrderID = pfield->LocalOrderID;
		rpt.OrderStatus = pfield->OrderStatus;

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		(vrt_producer_claim(producer_, &vvalue));
		ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
		ivalue->index = cursor;
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
	if (ended_) return;

    clog_error("[%s] OnRtnErrorMsg:%s", module_name_, X1DatatypeFormater::ToString(pfield).c_str());

	// :DEBUG 之前出现过一次OnRtnErrorMsg，errorMsg=local order info invalid，nErrorID=24
	// 可能与LocalOrderID一个会话内重复有关。但验证几次，无法重现。
	// 故忽略OnRtnErrorMsg，继续观察
	// return;

	int counter = GetCounterByLocalOrderID(pfield->RequestID);
	if(0 == counter) return; // counter==0,表示不是对应下单请求的回报（下单/撤单请求，counter从1开始）
	if(cancel_requests_[counter]) return; // 对于撤单请求的错误，不需报给策略

	int32_t cursor = Push();
	struct TunnRpt &rpt = rpt_buffer_[cursor];
	rpt.LocalOrderID = pfield->LocalOrderID;
	rpt.OrderStatus = X1_FTDC_SPD_ERROR;
	rpt.ErrorID = pfield->ErrorID;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = cursor;
	ivalue->data = TUNN_RPT;
	(vrt_producer_publish(producer_));

	clog_debug("[%s] OnRtnErrorMsg: index,%d; data,%d; LocalOrderID:%ld",
				module_name_, ivalue->index, ivalue->data, pfield->LocalOrderID);
}

void TunnRptProducer::OnRtnMatchedInfo(struct CX1FtdcRspPriMatchInfoField* pfield)
{
    clog_debug("[%s] OnRtnMatchedInfo ended_:%d", ended_);

	if (ended_) return;

    clog_info("[%s] OnRtnMatchedInfo:%s", module_name_, X1DatatypeFormater::ToString(pfield).c_str());

	int32_t cursor = Push();
	struct TunnRpt &rpt = rpt_buffer_[cursor];
	rpt.LocalOrderID = pfield->LocalOrderID;
	rpt.OrderStatus = pfield->OrderStatus;
	rpt.MatchedAmount = pfield->MatchedAmount;
	rpt.MatchedPrice = pfield->MatchedPrice;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = cursor;
	ivalue->data = TUNN_RPT;

	clog_info("[%s] OnRtnMatchedInfo: index,%d; data,%d; LocalOrderID:%ld",
				module_name_, ivalue->index, ivalue->data, pfield->LocalOrderID);

	(vrt_producer_publish(producer_));
}

void TunnRptProducer::OnRtnOrder(struct CX1FtdcRspPriOrderField* pfield)
{
    clog_debug("[%s] OnRtnOrderended_:%d", ended_);

	if (ended_) return;

    clog_info("[%s] OnRtnOrder:%s", module_name_, X1DatatypeFormater::ToString(pfield).c_str());

	if (pfield->OrderStatus == X1_FTDC_SPD_ERROR){
		clog_warning("[%s] OnRtnOrder:%s",
			module_name_,
			X1DatatypeFormater::ToString(pfield).c_str());
	}

	int32_t cursor = Push();
	struct TunnRpt &rpt = rpt_buffer_[cursor];
	rpt.LocalOrderID = pfield->LocalOrderID;
	rpt.OrderStatus = pfield->OrderStatus;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = cursor;
	ivalue->data = TUNN_RPT;

	clog_debug("[%s] OnRtnOrder: index,%d; data,%d; LocalOrderID:%ld",
				module_name_, ivalue->index, ivalue->data, pfield->LocalOrderID);

	(vrt_producer_publish(producer_));
}

void TunnRptProducer::OnRtnCancelOrder(struct CX1FtdcRspPriCancelOrderField* pfield)
{
    clog_debug("[%s] OnRtnCancelOrder:%d", ended_);

	if (ended_) return;

    clog_info("[%s] OnRtnCancelOrder:%s", module_name_, X1DatatypeFormater::ToString(pfield).c_str());

	if (pfield->OrderStatus == X1_FTDC_SPD_ERROR){
		clog_warning("[%s] OnRtnCancelOrder:%s",
			module_name_,
			X1DatatypeFormater::ToString(pfield).c_str());
	}

	if (pfield->OrderStatus == X1_FTDC_SPD_CANCELED ||
		pfield->OrderStatus == X1_FTDC_SPD_PARTIAL_CANCELED ||
		pfield->OrderStatus == X1_FTDC_SPD_IN_CANCELED){
		int32_t cursor = Push();
		struct TunnRpt &rpt = rpt_buffer_[cursor];
		rpt.LocalOrderID = pfield->LocalOrderID;
		rpt.OrderStatus = pfield->OrderStatus;

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		(vrt_producer_claim(producer_, &vvalue));
		ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
		ivalue->index = cursor;
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
