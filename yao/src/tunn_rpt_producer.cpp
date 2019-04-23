#include <thread>
#include <sys/stat.h>
#include <sys/types.h>
#include<sys/types.h>
#include<dirent.h>
#include <unistd.h>
#include <chrono>
#include <ctime>
#include <ratio>
#include <stdio.h>
#include "tunn_rpt_producer.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "ctp_data_formater.h"

using namespace std::chrono;

CThostFtdcInputOrderField CtpFieldConverter::new_order_;
CThostFtdcInputOrderActionField CtpFieldConverter::cancel_order_;

TunnRptProducer::TunnRptProducer(struct vrt_queue  *queue)
: module_name_("TunnRptProducer")
{
	ended_ = false;
	position_ready_ = false;

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

	//Create Directory
    char cur_path[256];
    char full_path[256];
    getcwd(cur_path, sizeof(cur_path));
    sprintf(full_path, "%s/ctp_flow_dir_%s", cur_path, this->config_.userid.c_str());
	if (opendir(full_path) == NULL) mkdir(full_path, 0755);    
    api_ = CThostFtdcTraderApi::CreateFtdcTraderApi(full_path); 
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
	int ret = api_->ReqOrderInsert(pInputOrder, OEDERINSERT_REQUESTID); // requestid==1，表示是下单请求
#ifdef LATENCY_MEASURE
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		int latency = (t1.time_since_epoch().count() - t0.time_since_epoch().count()) / 1000;	
		clog_warning("[%s] ReqOrderInsert latency:%d us", 
					module_name_,latency); 
#endif
	
	// report rejected if ret!=0
	if (ret != 0){
		clog_warning("[%s] ReqOrderInsert- ret=%d - %s", 
			module_name_, ret, CtpDatatypeFormater::ToString(pInputOrder).c_str());
	}else {
		clog_info("[%s] ReqOrderInsert - ret=%d - %s", 
			module_name_, ret, CtpDatatypeFormater::ToString(pInputOrder).c_str());
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
	p->SessionID = this->sessionid_;
    p->FrontID = this->frontid_;		
	int ret = api_->ReqOrderAction(p, ORDERCANCEL_REQUESTID); // requestid==2，表示是撤单请求
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
    
	int rtn = api_->ReqUserLogin2(&login_data, 0);

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
	this->frontid_ = pRspUserLogin->FrontID;	
	this->sessionid_ = pRspUserLogin->SessionID;	
	strncpy(this->MaxOrderRef_, pRspUserLogin->MaxOrderRef, sizeof(this->MaxOrderRef_));
	this->counter_ = GetCounterByLocalOrderID(stoi(this->MaxOrderRef_));
	
    if (pRspInfo==NULL || 0==pRspInfo->ErrorID) {
		clog_warning("[%s] OnRspUserLogin successfully.", module_name_);
		
		// TODO:
		CtpFieldConverter::InitNewOrder(this->config_.userid.c_str(), this->config_.brokerid.c_str());
		CtpFieldConverter::InitCancelOrder(this->config_.userid.c_str(), 
			this->config_.brokerid.c_str(), this->frontid_, this->sessionid_);
	
		CThostFtdcSettlementInfoConfirmField req;
        memset(&req, 0, sizeof(req));
        strncpy(req.BrokerID, this->config_.brokerid.c_str(), sizeof(TThostFtdcBrokerIDType));
        strncpy(req.InvestorID, this->config_.userid.c_str(), sizeof(TThostFtdcInvestorIDType));
        int ret = api_->ReqSettlementInfoConfirm(&req, 0);
        clog_warning("[%s] ReqSettlementInfoConfirm, return: %d", module_name_, ret);
		
		// 查询仓位
		CThostFtdcQryInvestorPositionField a = { 0 };
		strncpy(a.BrokerID, this->config_.brokerid.c_str(), sizeof(a.BrokerID));
		strncpy(a.InvestorID, this->config_.userid.c_str(), sizeof(a.InvestorID));
		strncpy(a.InstrumentID, "", sizeof(a.InstrumentID));//不填写合约则返回所有持仓
		int rtn = api_->ReqQryInvestorPosition(&a, 0);
		if(0 != rtn){
			 clog_error("[%s] ReqQryInvestorPosition, return: %d", module_name_, ret);
			 exit (EXIT_FAILURE);
		}

    }else {
		clog_error("[%s] OnRspUserLogin, error: %d, msg: %s", module_name_, 
			pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
}

void TunnRptProducer::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{    
     clog_warning("[%s] OnRspSettlementInfoConfirm: requestid = %d, last_flag=%d \n%s \n%s",
         module_name_,
		 nRequestID, bIsLast,
         CtpDatatypeFormater::ToString(pSettlementInfoConfirm).c_str(),
         CtpDatatypeFormater::ToString(pRspInfo).c_str());    
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
		int rtn = api_->ReqUserLogout(&logoutinfo, 0);
		
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
		rpt.ErrorID = pRspInfo->ErrorID;

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		(vrt_producer_claim(producer_, &vvalue));
		ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
		ivalue->index = cursor;
		ivalue->data = TUNN_RPT;
		
		clog_info("[%s] OnRspInsertOrder: index,%d; data,%d; LocalOrderID:%s",
			module_name_, ivalue->index, ivalue->data, pInputOrder->OrderRef);
		
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
}
		
// done
void TunnRptProducer::OnRspError(CThostFtdcRspInfoField *pRspInfo, 
	int nRequestID, bool bIsLast)
{
	if (pRspInfo==NULL || 0==pRspInfo->ErrorID) {				
    }else {		
		 clog_error("[%s] OnRspError:%s",
        module_name_,		
        CtpDatatypeFormater::ToString(pRspInfo).c_str());	
	}
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
	strcpy(rpt.OrderSysID, pOrder->OrderSysID);

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

// 看CThostFtdcOrderField.VolumeTraded是否有值
void TunnRptProducer::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	if (ended_) return;

    clog_info("[%s] OnRtnTrade:%s", module_name_, CtpDatatypeFormater::ToString(pTrade).c_str());

	int32_t cursor = Push();
	struct TunnRpt &rpt = rpt_buffer_[cursor];
	rpt.LocalOrderID = stoi(pTrade->OrderRef);
	// TODO: 因为没有状态，所以需要在strategy中根据累计成交量来确定部分成交还是完全成交
	rpt.OrderStatus = THOST_FTDC_OST_Unknown; // 因ctp状态与成交是分开返回的
	rpt.MatchedAmount = pTrade->Volume;
	rpt.MatchedPrice = pTrade->Price;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = cursor;
	ivalue->data = TUNN_RPT;

	clog_info("[%s] OnRtnTrade: index,%d; data,%d; LocalOrderID:%ld",
		module_name_, ivalue->index, ivalue->data, rpt.LocalOrderID);

	(vrt_producer_publish(producer_));
}

//  看《CTP问题列表》，该回调可以忽略。需要验证
void TunnRptProducer::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, 
	CThostFtdcRspInfoField *pRspInfo)
{
		if (ended_) return;
	
	 clog_info("[%s] OnErrRtnOrderInsert:%s %s;",
        module_name_,
		CtpDatatypeFormater::ToString(pInputOrder).c_str(),
        CtpDatatypeFormater::ToString(pRspInfo).c_str());
		
	if (pRspInfo==NULL || 0==pRspInfo->ErrorID) {				
    }else {		
		 clog_error("[%s] OnErrRtnOrderInsert:%s %s",
        module_name_,
		CtpDatatypeFormater::ToString(pInputOrder).c_str(),
        CtpDatatypeFormater::ToString(pRspInfo).c_str());
				
		int32_t cursor = Push();		
		struct TunnRpt &rpt = rpt_buffer_[cursor];	 // LocalOrderID也只需要赋值一次
		
		rpt.LocalOrderID = stoi(pInputOrder->OrderRef);
		rpt.OrderStatus = THOST_FTDC_OAS_Rejected;
		rpt.ErrorID = pRspInfo->ErrorID;

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		(vrt_producer_claim(producer_, &vvalue));
		ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
		ivalue->index = cursor;
		ivalue->data = TUNN_RPT;
		
		clog_info("[%s] OnErrRtnOrderInsert: index,%d; data,%d; LocalOrderID:%s",
			module_name_, ivalue->index, ivalue->data, pInputOrder->OrderRef);
		
		(vrt_producer_publish(producer_));		
	}
}

// done
void TunnRptProducer::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, 
	CThostFtdcRspInfoField *pRspInfo)
{
	if (pRspInfo==NULL || 0==pRspInfo->ErrorID) {				
    }else {		
		 clog_error("[%s] pOrderAction:%s",
        module_name_,		
        CtpDatatypeFormater::ToString(pRspInfo).c_str());		
	}
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

void TunnRptProducer::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, 
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)

{
	clog_warning("[%s] OnRspQryInvestorPosition:%s %s,isLast:%d",
        module_name_,
		CtpDatatypeFormater::ToString(pInvestorPosition).c_str(),
        CtpDatatypeFormater::ToString(pRspInfo).c_str(),
		bIsLast);	
	if (pRspInfo==NULL || 0==pRspInfo->ErrorID) {		
		positions_[pInvestorPosition->InstrumentID] = *pInvestorPosition;
    }else{
		clog_error("[%s] OnRspQryInvestorPosition:%s %s,isLast:%d",
        module_name_,
		CtpDatatypeFormater::ToString(pInvestorPosition).c_str(),
        CtpDatatypeFormater::ToString(pRspInfo).c_str(),
		bIsLast);	
		
		exit (EXIT_FAILURE);
	}
	
	if(bIsLast) position_ready_ = true;
}

bool TunnRptProducer::IsReady()
{
	if(position_ready_){
		return true;
	}else{
		return false;
	}
}

long TunnRptProducer::NewLocalOrderID(int32_t strategyid)
{	
	counter_++;
    long localorderid = strategyid+ counter_ * 1000;			
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
