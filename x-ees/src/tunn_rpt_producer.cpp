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

	// create EES object
    api_ = LoadTunnelApi();
    RESULT err = api_->ConnServer(api_config_,this);
    clog_warning("[%s] ConnServer-err:%d", module_name_,err);
   // api_->SubscribePrivateTopic(USTP_TERT_QUICK);
   // api_->SubscribePublicTopic(USTP_TERT_QUICK);

	char addr[2048];
	strcpy(addr, this->config_.address.c_str());
	api_->RegisterFront(addr);
	api_->Init();
	clog_warning("[%s] femas Api init.", module_name_);

}

EESTraderApi *TunnRptProducer::LoadTunnelApi()
{
	m_handle =  dlopen(EES_TRADER_DLL_NAME, RTLD_LAZY);
	if (!m_handle){
		printf("加载EES行情动态库(%s)失败\n", EES_TRADER_DLL_NAME);
		return false;
	}

	funcCreateEESTraderApi createFun = (funcCreateEESTraderApi)dlsym(m_handle, CREATE_EES_TRADER_API_NAME);
	if (!createFun){
		printf("获取EES创建函数地址失败!\n");
		return false;
	}

	m_distoryFun = (funcDestroyEESTraderApi)dlsym(m_handle, DESTROY_EES_TRADER_API_NAME);
	if (!createFun){
		printf("获取EES销毁函数地址失败!\n");
		return false;
	}

	api_ = createFun();
	if (!api_){
		printf("创建EES行情对象失败!\n");
		return false;
	}
}

void *unnRptProducer::UnloadTunnelApi()
{
	if (api_){
		api_->DisConnServer();
		m_distoryFun(api_);
		api_ = NULL;
		m_distoryFun = NULL;
	}

	if (m_handle){
		dlclose(m_handle);
		m_handle = NULL;
	}
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
		// TODO: api_config_
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

void TunnRptProducer::OnConnection(ERR_NO errNo, const char* pErrStr )
{
    clog_warning("[%s] OnConnection-err:%d; err info:%s", module_name_,errNo,pErrStr);
	this->ReqLogin();
}

void OnDisConnection(ERR_NO errNo, const char* pErrStr )
{
    clog_warning("[%s] OnDisConnection-err:%d; err info:%s", module_name_,errNo,pErrStr);
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
	RESULT err = api_->UserLogon(config_.userid.c_str(),config_.password.c_str(),NULL,NULL);
    clog_warning("[%s] ReqLogin-err_no,%d; user:%s; pwd:%d",module_name_,err,
		config_.userid.c_str(),config_.password.c_str());
}

void TunnRptProducer::OnUserLogon(EES_LogonResponse* pLogon)
{
	counter_ = GetCounterByLocalOrderID(pLogon->m_MaxToken);
	counter_++;
    clog_warning("[%s] OnUserLogon-result:%d;user:%s;counter_:%u", module_name_,
		pLogon->m_Result,pLogon->m_UserId,this->counter_);
}

void TunnRptProducer::OnOrderAccept(EES_OrderAcceptField* pAccept )
{
	if (ended_) return;

	int32_t cursor = Push();
	struct TunnRpt &rpt = rpt_buffer_[cursor];
	rpt.LocalOrderID = pAccept->m_ClientOrderToken;

    clog_info("[%s] OnOrderAccept:%s", module_name_,
		EESDatatypeFormater::ToString(pAccept).c_str());

	if (pAccept->m_OrderState==EES_OrderState_order_dead){
		time_t rawtime;
		time (&rawtime);
		clog_error("[%s][%s] OnRspOrderInsert:%s",module_name_,ctime(&rawtime),
			EESDatatypeFormater::ToString(pAccept).c_str());
		rpt.OrderStatus = SIG_STATUS_CANCEL;
		rpt.ErrorID = EES_OrderState_order_dead;
	}else{
		rpt.OrderStatus = SIG_STATUS_ENTRUSTED;
	}

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = cursor;
	ivalue->data = TUNN_RPT;
	(vrt_producer_publish(producer_));
}

void TunnRptProducer::OnOrderReject(EES_OrderRejectField* pReject)
{
	if (ended_) return;

	time_t rawtime;
	time (&rawtime);
	clog_error("[%s][%s] OnOrderReject:%s", module_name_,ctime(&rawtime),
		EESDatatypeFormater::ToString(pReject).c_str());

	int32_t cursor = Push();
	struct TunnRpt &rpt = rpt_buffer_[cursor];
	rpt.LocalOrderID = pReject->m_ClientOrderToken;
	rpt.OrderStatus = SIG_STATUS_CANCEL;
	rpt.ErrorID = pReject->EES_ReasonCode;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = cursor;
	ivalue->data = TUNN_RPT;
	(vrt_producer_publish(producer_));
}

void TunnRptProducer::OnOrderMarketAccept(EES_OrderMarketAcceptField* pAccept)
{
	if (ended_) return;

	int32_t cursor = Push();
	struct TunnRpt &rpt = rpt_buffer_[cursor];
	rpt.LocalOrderID = pAccept->m_ClientOrderToken;
	rpt.OrderStatus = SIG_STATUS_ENTRUSTED;

    clog_info("[%s] OnOrderMarketAccept:%s", module_name_,
		EESDatatypeFormater::ToString(pAccept).c_str());

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = cursor;
	ivalue->data = TUNN_RPT;
	(vrt_producer_publish(producer_));
}

void TunnRptProducer::OnOrderMarketReject(EES_OrderMarketRejectField* pReject)
{
	if (ended_) return;

	time_t rawtime;
	time (&rawtime);
	clog_error("[%s][%s] OnOrderMarketReject:%s", module_name_,ctime(&rawtime),
		EESDatatypeFormater::ToString(pReject).c_str());

	int32_t cursor = Push();
	struct TunnRpt &rpt = rpt_buffer_[cursor];
	rpt.LocalOrderID = pReject->m_ClientOrderToken;
	rpt.OrderStatus = SIG_STATUS_CANCEL;
	rpt.ErrorID = 3;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = cursor;
	ivalue->data = TUNN_RPT;
	(vrt_producer_publish(producer_));
}

void TunnRptProducer::OnOrderExecution(EES_OrderExecutionField* pExec )
{
	if (ended_) return;

    clog_info("[%s] OnOrderExecution:%s", 
		module_name_, EESDatatypeFormater::ToString(pExec).c_str());

	int32_t cursor = Push();
	struct TunnRpt &rpt = rpt_buffer_[cursor];
	rpt.LocalOrderID = pExec->m_ClientOrderToken;
	rpt.OrderStatus = TUNN_ORDER_STATUS_UNDEFINED;
	rpt.MatchedAmount = pExec->m_Quantity;
	rpt.TradePrice= pExec->m_Price;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = cursor;
	ivalue->data = TUNN_RPT;
	(vrt_producer_publish(producer_));
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

		this->UnloadTunnelApi();

		(vrt_producer_eof(producer_));
		clog_warning("[%s] End exit", module_name_);
	}
	fflush (Log::fp);
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
