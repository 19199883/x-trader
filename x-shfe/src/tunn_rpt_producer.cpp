#include <stdlib.h>     /* atol */
#include <thread>
#include "tunn_rpt_producer.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "femas_data_formater.h"

int32_t TunnRptProducer::counter_ = 1;

TunnRptProducer::TunnRptProducer(struct vrt_queue  *queue)
: module_name_("TunnRptProducer")
{
	ended_ = false;

	this->ParseConfig();
	memset(rpt_buffer_,0,sizeof(rpt_buffer_));

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

	// create femas object
    api_ = CUstpFtdcTraderApi::CreateFtdcTraderApi();
    api_->RegisterSpi(this);
    api_->SubscribePrivateTopic(USTP_TERT_QUICK);
    api_->SubscribePublicTopic(USTP_TERT_QUICK);

	char addr[2048];
	strcpy(addr, this->config_.address.c_str());
	api_->RegisterFront(addr);
	api_->Init();
	clog_warning("[%s] femas Api init.", module_name_);

}

TunnRptProducer::~TunnRptProducer()
{
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
		this->config_.investorid = tunn_node->Attribute("investorid");
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

int TunnRptProducer::ReqOrderInsert(CUstpFtdcInputOrderField *p)
{
	int ret = api_->ReqOrderInsert(p, 0);
	
	// report rejected if ret!=0
	if (ret != 0){
		time_t rawtime;
		time (&rawtime);
		clog_error("[%s][%s] ReqOrderInsert- ret=%d - %s", 
			module_name_,ctime(&rawtime),ret, FEMASDatatypeFormater::ToString(p).c_str());
	}else {
		clog_info("[%s] ReqOrderInsert- ret=%d - %s", 
			module_name_, ret, FEMASDatatypeFormater::ToString(p).c_str());
	}

	return ret;
}

// 撤单操作请求
int TunnRptProducer::ReqOrderAction(CUstpFtdcOrderActionField *p)
{
	int ret = api_->ReqOrderAction(p, 0);

	if (ret != 0){
		time_t rawtime;
		time (&rawtime);
		clog_error("[%s][%s] ReqOrderAction- ret=%d - %s", 
			module_name_,ctime(&rawtime),ret, FEMASDatatypeFormater::ToString(p).c_str());
	} else {
		clog_info("[%s] ReqCancelOrder - ret=%d - %s", 
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
	
    clog_warning("[%s] ReqLogin:  err_no,%d",module_name_, rtn );
    clog_warning("[%s] ReqLogin:   %s", 
			module_name_, FEMASDatatypeFormater::ToString(&login_data).c_str());
}

void TunnRptProducer::OnFrontConnected()
{

    clog_warning("[%s] OnFrontConnected.", module_name_);
	this->ReqLogin();
}

void TunnRptProducer::OnFrontDisconnected(int nReason)
{
    clog_warning("[%s] OnFrontDisconnected, nReason=%d", module_name_, nReason);
}

void TunnRptProducer::OnHeartBeatWarning(int nTimeLapse)
{
    clog_warning("[%s] OnHeartBeatWarning, nTimeLapse=%d", module_name_, nTimeLapse);
}

void TunnRptProducer::OnRspUserLogin(CUstpFtdcRspUserLoginField *pfield, 
			CUstpFtdcRspInfoField *perror, int nRequestID, bool bIsLast)
{

	if(pfield != NULL){
		if (strcmp(pfield->MaxOrderLocalID, "") != 0){
			counter_ = GetCounterByLocalOrderID(atol(pfield->MaxOrderLocalID));
			counter_++;
		}
	}
	
    clog_warning("[%s] counter_:%d; OnRspUserLogin:%s %s",
        module_name_,
		counter_,
		FEMASDatatypeFormater::ToString(pfield).c_str(),
        FEMASDatatypeFormater::ToString(perror).c_str());

}

void TunnRptProducer::OnRspUserLogout(CUstpFtdcRspUserLogoutField *pf, CUstpFtdcRspInfoField *pe,
			int nRequestID, bool bIsLast)
{
    clog_warning("[%s] OnRspUserLogout:%s %s",
        module_name_,
		FEMASDatatypeFormater::ToString(pf).c_str(),
        FEMASDatatypeFormater::ToString(pe).c_str());
}

void TunnRptProducer::End()
{
	if(!ended_){
		clog_warning("[%s] last counter:%d", module_name_,counter_);
		ended_ = true;

		if (api_) {
			api_->Release();
			api_ = NULL;
		}

		(vrt_producer_eof(producer_));
		clog_warning("[%s] End exit", module_name_);
	}
}

void TunnRptProducer::OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{    
	time_t rawtime;
	time (&rawtime);
	clog_error("[%s][%s] OnRspError: requestid = %d, last_flag=%d %s",
		module_name_,ctime(&rawtime),nRequestID, bIsLast, 
		FEMASDatatypeFormater::ToString(pRspInfo).c_str());
}

void TunnRptProducer::OnRspOrderInsert(CUstpFtdcInputOrderField *pfield,
			CUstpFtdcRspInfoField *perror, int nRequestID,bool bIsLast)
{
	if (ended_) return;

    clog_info("[%s] OnRspOrderInsert:%s %s",
        module_name_,
		FEMASDatatypeFormater::ToString(pfield).c_str(),
        FEMASDatatypeFormater::ToString(perror).c_str());

	if (perror != NULL && 0 != perror->ErrorID){
		time_t rawtime;
		time (&rawtime);
		clog_error("[%s][%s] OnRspOrderInsert:%s %s",
			module_name_,ctime(&rawtime),
			FEMASDatatypeFormater::ToString(pfield).c_str(),
			FEMASDatatypeFormater::ToString(perror).c_str());
	}

	if (pfield != NULL){
		int32_t cursor = Push();
		// TODO: 考虑采用与x-zce一样的方式，使用cursor，这样可以去掉Push
		// LocalOrderID也只需要赋值一次
		struct TunnRpt &rpt = rpt_buffer_[cursor];
		rpt.LocalOrderID = atol(pfield->UserOrderLocalID);
		// order_respond.entrust_no       = atol(entrust_no);
		if (perror != NULL && 0 != perror->ErrorID) {
			rpt.OrderStatus = USTP_FTDC_OS_Canceled;
			rpt.ErrorID = perror->ErrorID;
		}else{
			rpt.OrderStatus = USTP_FTDC_OS_NoTradeQueueing;
		}

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		(vrt_producer_claim(producer_, &vvalue));
		ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
		ivalue->index = cursor;
		ivalue->data = TUNN_RPT;
		(vrt_producer_publish(producer_));
	} // if ((pfield != NULL)
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

void TunnRptProducer::OnRspOrderAction(CUstpFtdcOrderActionField *pfield,
			CUstpFtdcRspInfoField *perror, int nRequestID,bool bIsLast)
{
	if (ended_) return;

    clog_info("[%s] OnRspOrderAction:%s %s",
        module_name_,
		FEMASDatatypeFormater::ToString(pfield).c_str(),
        FEMASDatatypeFormater::ToString(perror).c_str());

	if (perror != NULL && 0 != perror->ErrorID){
		time_t rawtime;
		time (&rawtime);
		clog_error("[%s][%s] OnRspOrderAction:%s %s",
			module_name_,ctime(&rawtime),
			FEMASDatatypeFormater::ToString(pfield).c_str(),
			FEMASDatatypeFormater::ToString(perror).c_str());
	}
}

void TunnRptProducer::OnErrRtnOrderInsert(CUstpFtdcInputOrderField *pfield,
			CUstpFtdcRspInfoField *perror)
{
	if (ended_) return;

	time_t rawtime;
	time (&rawtime);
	clog_error("[%s][%s] OnErrRtnOrderInsert:%s %s",
        module_name_,ctime(&rawtime),
		FEMASDatatypeFormater::ToString(pfield).c_str(),
		FEMASDatatypeFormater::ToString(perror).c_str());

	// 因OnRsp已经发送该错误了，所以此次错误回报是重复的，丢弃。
	// 具体见《FemasAPI（四所版）开发技术指南.pdf》的“关于错单”
	return;

	if(pfield != NULL && perror != NULL){
		int32_t cursor = Push();
		struct TunnRpt &rpt = rpt_buffer_[cursor];
		rpt.LocalOrderID = atol(pfield->UserOrderLocalID);
		rpt.OrderStatus = USTP_FTDC_OS_Canceled;
		rpt.ErrorID = perror->ErrorID;

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		(vrt_producer_claim(producer_, &vvalue));
		ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
		ivalue->index = cursor;
		ivalue->data = TUNN_RPT;
		(vrt_producer_publish(producer_));
	}
}

void TunnRptProducer::OnRtnTrade(CUstpFtdcTradeField * pfield)
{
	if (ended_) return;

    clog_info("[%s] OnRtnTrade:%s", 
				module_name_, FEMASDatatypeFormater::ToString(pfield).c_str());

	int32_t cursor = Push();
	struct TunnRpt &rpt = rpt_buffer_[cursor];
    //order_return.entrust_no     = atol(rsp->OrderSysID);
	rpt.LocalOrderID = atol(pfield->UserOrderLocalID);
	rpt.OrderStatus = TUNN_ORDER_STATUS_UNDEFINED;
	rpt.MatchedAmount = pfield->TradeVolume;
	rpt.TradePrice= pfield->TradePrice;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = cursor;
	ivalue->data = TUNN_RPT;
	(vrt_producer_publish(producer_));
}

void TunnRptProducer::OnRtnOrder(CUstpFtdcOrderField *pfield)
{
	if (ended_) return;

    clog_info("[%s] OnRtnOrder:%s", module_name_, FEMASDatatypeFormater::ToString(pfield).c_str());

	int32_t cursor = Push();
	struct TunnRpt &rpt = rpt_buffer_[cursor];
    //order_return.entrust_no     = atol(rsp->OrderSysID);
	rpt.LocalOrderID = atol(pfield->UserOrderLocalID);
	rpt.OrderStatus = pfield->OrderStatus;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = cursor;
	ivalue->data = TUNN_RPT;
	(vrt_producer_publish(producer_));
}

void TunnRptProducer::OnErrRtnOrderAction(CUstpFtdcOrderActionField *pfield, 
			CUstpFtdcRspInfoField *perror)
{
	time_t rawtime;
	time (&rawtime);
	clog_error("[%s][%s] OnErrRtnOrderAction:%s %s",
		module_name_,ctime(&rawtime),
		FEMASDatatypeFormater::ToString(pfield).c_str(),
		FEMASDatatypeFormater::ToString(perror).c_str());         
}

long TunnRptProducer::GetCounterByLocalOrderID(long localorderid)
{
	return localorderid / 1000;
}

long TunnRptProducer::NewLocalOrderID(int32_t strategyid)
{
    long localorderid = strategyid+ counter_ * 1000;		
	counter_++;

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
