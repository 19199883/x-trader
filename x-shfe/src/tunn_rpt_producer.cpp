#include <stdlib.h>     /* atol */
#include <thread>
#include "tunn_rpt_producer.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "femas_data_formater.h"

TunnRptProducer::TunnRptProducer(struct vrt_queue  *queue)
: module_name_("TunnRptProducer")
{
	ended_ = false;

	this->ParseConfig();

	clog_info("[%s] RPT_BUFFER_SIZE: %d;", module_name_, RPT_BUFFER_SIZE);

	struct vrt_producer  *producer = vrt_producer_new("tunnrpt_producer", 1, queue);

	//rip_check(producer = vrt_producer_new("tunnrpt_producer", 1, queue));

	this->producer_ = producer;
	this->producer_ ->yield = vrt_yield_strategy_threaded();

	// create femas object
    api_ = CUstpFtdcTraderApi::CreateFtdcTraderApi();
    api_->RegisterSpi(this);
    api_->SubscribePrivateTopic(USTP_TERT_QUICK);
    api_->SubscribePublicTopic(USTP_TERT_QUICK);

	char addr[2048];
	strcpy(addr, this->config_.address.c_str());
	api_->RegisterFront(addr);
	api_->Init();
	clog_info("[%s] femas Api init.", module_name_);

}

TunnRptProducer::~TunnRptProducer()
{
//	if (this->producer_ != NULL){
//		vrt_producer_free(this->producer_);
//		this->producer_ = NULL;
//		clog_info("[%s] release tunnrpt_producer.", module_name_);
//	}

    if (api_) {
        api_->Release();
        api_ = NULL;
    }
}

void TunnRptProducer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    
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

int TunnRptProducer::ReqOrderInsert(CUstpFtdcInputOrderField *p)
{
	int ret = api_->ReqInsertOrder(p, 0);
	
	// report rejected if ret!=0
	if (ret != 0){
		clog_warning("[%s] ReqInsertOrder- ret=%d - %s", 
			module_name_, ret, FEMASDatatypeFormater::ToString(p).c_str());

		struct TunnRpt rpt;
		memset(&rpt, 0, sizeof(rpt));
		rpt.LocalOrderID = atol(p->UserOrderLocalID);
		rpt.OrderStatus = USTP_FTDC_OS_Canceled;
		rpt.ErrorID = ret;

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		(vrt_producer_claim(producer_, &vvalue));
		ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
		ivalue->index = Push(rpt);
		ivalue->data = TUNN_RPT;
		(vrt_producer_publish(producer_));
	}else {
		clog_debug("[%s] ReqInsertOrder - ret=%d - %s", 
			module_name_, ret, X1DatatypeFormater::ToString(p).c_str());
	}

	return ret;
}

// 撤单操作请求
int TunnRptProducer::ReqOrderAction(CUstpFtdcOrderActionField *p)
{
	int ret = api_->ReqCancelOrder(p, 0);

	if (ret != 0){
		clog_warning("[%s] ReqCancelOrder - ret=%d - %s", 
			module_name_, ret, FEMASDatatypeFormater::ToString(p).c_str());
	} else {
		clog_debug("[%s] ReqCancelOrder - ret=%d - %s", 
			module_name_, ret, FEMASDatatypeFormater::ToString(p).c_str());
	}

	return ret;
}

void TunnRptProducer::ReqLogin()
{
    CUstpFtdcReqUserLoginField login_data;
    memset(&login_data, 0, sizeof(CUstpFtdcReqUserLoginField));
    strncpy(login_data.BrokerID, config_.brokerid.c_str(), sizeof(TUstpFtdcBrokerIDType));
    strncpy(login_data.UserID, config_.userid.c_str(), sizeof(TUstpFtdcUserIDType));
    strncpy(login_data.Password, config_.password.c_str(), sizeof(TUstpFtdcPasswordType));
	int rtn = api_->ReqUserLogin(&login_data, 0);
	
    clog_info("[%s] ReqLogin:  err_no,%d",module_name_, rtn );
    clog_info("[%s] ReqLogin:   %s", 
			module_name_, FEMASDatatypeFormater::ToString(&login_data).c_str());
}

void TunnRptProducer::OnFrontConnected()
{
    clog_info("[%s] OnFrontConnected.", module_name_);
	this->ReqLogin();
}

void TunnRptProducer::OnFrontDisconnected(int nReason)
{
    clog_info("[%s] OnFrontDisconnected, nReason=%d", module_name_, nReason);
}

void TunnRptProducer::OnHeartBeatWarning(int nTimeLapse)
{
    clog_info("[%s] OnHeartBeatWarning, nTimeLapse=%d", module_name_, nTimeLapse);
}

void TunnRptProducer::OnRspUserLogin(CUstpFtdcRspUserLoginField *pfield, 
			CUstpFtdcRspInfoField *perror, int nRequestID, bool bIsLast)
{
	if(pfield != NULL){
		// TODO: process MaxOrderLocalID
		max_order_ref_ = atoll(pRspUserLogin->MaxOrderLocalID);
	}
	
    clog_info("[%s] OnRspUserLogin:%s %s",
        module_name_,
		FEMASDatatypeFormater::ToString(pfield).c_str(),
        FEMASDatatypeFormater::ToString(perror).c_str());

    if (perror == NULL || perror->ErrorID == 0) {
		clog_info("[%s] OnRspUserLogin success.", module_name_);
    }
    else {
		clog_info("[%s] OnRspUserLogin, error: %d", module_name_, perror->ErrorID);
    }
}

void TunnRptProducer::OnRspUserLogout(CUstpFtdcRspUserLogoutField *pf, CUstpFtdcRspInfoField *e,
			int nRequestID,truct CX1FtdcRspUserLogoutInfoField* pf, bool bIsLast)
{
    clog_info("[%s] OnRspUserLogout:%s %s",
        module_name_,
		FEMASDatatypeFormater::ToString(pf).c_str(),
        FEMASDatatypeFormater::ToString(pe).c_str());
}

void TunnRptProducer::End()
{
	ended_ = true;
	(vrt_producer_eof(producer_));
}

void TunnRptProducer::OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{    
        if (pRspInfo){
            clog_warning("[%s] OnRspError: requestid = %d, last_flag=%d %s",
                module_name_, nRequestID, bIsLast, 
				FEMASDatatypeFormater::ToString(pRspInfo).c_str());
        }else{
            TNL_LOG_INFO("OnRspError ");
        }   
}

void TunnRptProducer::OnRspOrderInsert(CUstpFtdcInputOrderField *pfield,
			CUstpFtdcRspInfoField *perror, int nRequestID,bool bIsLast)
{
	if (ended_) return;

    clog_debug("[%s] OnRspInsertOrder:%s %s",
        module_name_,
		FEMASDatatypeFormater::ToString(pfield).c_str(),
        FEMASDatatypeFormater::ToString(perror).c_str());

	if (perror != NULL){
		clog_warning("[%s] OnRspInsertOrder:%s %s",
			module_name_,
			FEMASDatatypeFormater::ToString(pfield).c_str(),
			FEMASDatatypeFormater::ToString(perror).c_str());
	}

	if ((pfield != NULL){
		struct TunnRpt rpt;
		memset(&rpt, 0, sizeof(rpt));
		if (perror != NULL) {
			rpt.LocalOrderID = atol(perror->UserOrderLocalID);
			rpt.OrderStatus = USTP_FTDC_OS_Canceled;
			rpt.ErrorID = perror->ErrorID;
		}
		else {
			rpt.LocalOrderID = atol(pfield->LocalOrderID);
			// TODO:
			// order_respond.entrust_no       = atol(entrust_no);
			rpt.OrderStatus = pfield->OrderStatus;
		}

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		(vrt_producer_claim(producer_, &vvalue));
		ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
		ivalue->index = Push(rpt);
		ivalue->data = TUNN_RPT;
		(vrt_producer_publish(producer_));
		clog_debug("[%s] OnRspInsertOrder: index,%d; data,%d; LocalOrderID:%ld",
					module_name_, ivalue->index, ivalue->data, pfield->LocalOrderID);
	} // if ((pfield != NULL)
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

void TunnRptProducer::OnRspOrderAction(CUstpFtdcOrderActionField *pfield,
			CUstpFtdcRspInfoField *perror, int nRequestID,bool bIsLast)
{
	if (ended_) return;

    clog_debug("[%s] OnRspOrderAction:%s %s",
        module_name_,
		FEMASDatatypeFormater::ToString(pfield).c_str(),
        FEMASDatatypeFormater::ToString(perror).c_str());

	if (perror != NULL){
		clog_warning("[%s] OnRspOrderAction:%s %s",
			module_name_,
			FEMASDatatypeFormater::ToString(pfield).c_str(),
			FEMASDatatypeFormater::ToString(perror).c_str());
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


void TunnRptProducer::OnErrRtnOrderInsert(CUstpFtdcInputOrderField *pfield,
			CUstpFtdcRspInfoField *perror)
{
	if (ended_) return;

	clog_warning("[%s] OnErrRtnOrderInsert:  %s %s",
        module_name_,
		FEMASDatatypeFormater::ToString(pInputOrder).c_str(),
		FEMASDatatypeFormater::ToString(pRspInfo).c_str());

	if(pfield != NULL && perror != NULL){
		struct TunnRpt rpt;
		memset(&rpt, 0, sizeof(rpt));
		rpt.LocalOrderID = atol(pfield->UserOrderLocalID);
		rpt.OrderStatus = USTP_FTDC_OS_Canceled;
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
}

void TunnRptProducer::OnRtnTrade(CUstpFtdcTradeField * pfield)
{
	if (ended_) return;

    clog_debug("[%s] OnRtnMatchedInfo:%s", 
				module_name_, FEMASDatatypeFormater::ToString(pfield).c_str());

	// TODO: 忽略该回报，因为OrderReturn有成交信息
}

void TunnRptProducer::OnRtnOrder(CUstpFtdcOrderField *pfield)
{
	if (ended_) return;

    clog_debug("[%s] OnRtnOrder:%s", module_name_, FEMASDatatypeFormater::ToString(pfield).c_str());

	if (pfield->OrderStatus == USTP_FTDC_OS_Canceled){
		clog_warning("[%s] OnRtnOrder:%s",
			module_name_,
			FEMASDatatypeFormater::ToString(pfield).c_str());
	}

	struct TunnRpt rpt;
    memset(&rpt, 0, sizeof(rpt));
    //order_return.entrust_no     = atol(rsp->OrderSysID);
	rpt.LocalOrderID = atol(pfield->UserOrderLocalID);
	rpt.OrderStatus = pfield->OrderStatus;
	rpt.MatchedAmount = pfield->VolumeTraded;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = Push(rpt);
	ivalue->data = TUNN_RPT;
	(vrt_producer_publish(producer_));

	clog_debug("[%s] OnRtnOrder: index,%d; data,%d; LocalOrderID:%ld",
				module_name_, ivalue->index, ivalue->data, pfield->LocalOrderID);
}

void TunnRptProducer::OnRtnCancelOrder(struct CX1FtdcRspPriCancelOrderField* pfield)
{
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

void MyFemasTradeSpi::OnErrRtnOrderAction(CUstpFtdcOrderActionField *pfield, 
			CUstpFtdcRspInfoField *perror)
{
	clog_warning("[%] OnErrRtnOrderAction:%s %s",
		module_name_,
		FEMASDatatypeFormater::ToString(pfield).c_str(),
		FEMASDatatypeFormater::ToString(perror).c_str());         
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

void TunnRptProducer::OnPackageStart(int nTopicID, int nSequenceNo)
{
    // 不能用于识别初始化时的恢复数据，每单个Rtn消息都有开始结束
    //TNL_LOG_INFO("OnPackageStart, nTopicID:%d, nSequenceNo:%d", nTopicID, nSequenceNo);
}

void TunnRptProducer::OnPackageEnd(int nTopicID, int nSequenceNo)
{
    // 不能用于识别初始化时的恢复数据，每单个Rtn消息都有开始结束
    //TNL_LOG_INFO("OnPackageEnd, nTopicID:%d, nSequenceNo:%d", nTopicID, nSequenceNo);
}
