#include <stdlib.h>     /* atol */
#include <thread>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include "tunn_rpt_producer.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "ees_data_formater.h"

uint32_t TunnRptProducer::counter_ = 1;

TunnRptProducer::TunnRptProducer(struct vrt_queue  *queue)
: module_name_("TunnRptProducer")
{
	api_ = NULL;
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

#ifdef PERSISTENCE_ENABLED
#else
	// create EES object
    api_ = LoadTunnelApi();
    RESULT err = api_->ConnServer(api_config_,this);
    clog_warning("[%s] ConnServer-err=%d-%s", module_name_,err,EESDatatypeFormater::ToString(&api_config_));
#endif
}

EESTraderApi *TunnRptProducer::LoadTunnelApi()
{
	m_handle =  dlopen(EES_TRADER_DLL_NAME, RTLD_LAZY);
	if (!m_handle){
		clog_error("[%s] 加载EES行情动态库(%s)失败", module_name_,EES_TRADER_DLL_NAME);
		return NULL;
	}

	funcCreateEESTraderApi createFun = (funcCreateEESTraderApi)dlsym(m_handle, 
				CREATE_EES_TRADER_API_NAME);
	if (!createFun){
		clog_error("[%s] 获取EES创建函数地址失败!", module_name_);
		return NULL;
	}

	m_distoryFun = (funcDestroyEESTraderApi)dlsym(m_handle, DESTROY_EES_TRADER_API_NAME);
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

void TunnRptProducer::UnloadTunnelApi()
{
#ifdef PERSISTENCE_ENABLED
#else
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
#endif
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
		strcpy(this->api_config_.m_remoteTradeIp,tunn_node->Attribute("remoteTradeIp"));
		int remoteTradeTCPPort = 0;
		tunn_node->QueryIntAttribute("remoteTradeTCPPort", &remoteTradeTCPPort);
		this->api_config_.m_remoteTradeTCPPort = remoteTradeTCPPort;
		int remoteTradeUDPPort = 0;
		tunn_node->QueryIntAttribute("remoteTradeUDPPort", &remoteTradeUDPPort);
		this->api_config_.m_remoteTradeUDPPort = remoteTradeUDPPort;
		strcpy(this->api_config_.m_remoteQueryIp,tunn_node->Attribute("remoteQueryIp"));
		int remoteQueryTCPPort = 0;
		tunn_node->QueryIntAttribute("remoteQueryTCPPort", &remoteQueryTCPPort);
		this->api_config_.m_remoteQueryTCPPort = remoteQueryTCPPort;
		strcpy(this->api_config_.m_LocalTradeIp,tunn_node->Attribute("localTradeIp"));
		int localTradeUDPPort = 0;
		tunn_node->QueryIntAttribute("localTradeUDPPort", &localTradeUDPPort);
		this->api_config_.m_LocalTradeUDPPort = localTradeUDPPort;

		this->config_.mac = tunn_node->Attribute("mac");
		this->config_.brokerid = tunn_node->Attribute("brokerid");
		this->config_.investorid = tunn_node->Attribute("investorid");
		this->config_.userid = tunn_node->Attribute("userid");
		this->config_.password = tunn_node->Attribute("password");
		strcpy(this->appid_, tunn_node->Attribute("appid"));
		strcpy(this->authcode_, tunn_node->Attribute("authcode"));

		clog_warning("[%s] tunn config:brokerid:%s; userid:%s; investor:%s password:%s; mac:%s"
					"m_remoteTradeIp:%s; "
					" m_remoteTradeTCPPort:%d; "
					"m_remoteTradeUDPPort:%d; "
					"m_remoteQueryIp:%s; "
					"m_remoteQueryTCPPort:%d; "
					"m_LocalTradeIp:%s; "
					"m_LocalTradeUDPPort:%d; " ,
					module_name_, 
					this->config_.brokerid.c_str(),
					this->config_.userid.c_str(),
					this->config_.investorid.c_str(),
					this->config_.password.c_str(),
					this->config_.mac.c_str(),
					this->api_config_.m_remoteTradeIp,
					this->api_config_.m_remoteTradeTCPPort,
					this->api_config_.m_remoteTradeUDPPort,
					this->api_config_.m_remoteQueryIp,
					this->api_config_.m_remoteQueryTCPPort,
					this->api_config_.m_LocalTradeIp,
					this->api_config_.m_LocalTradeUDPPort);
	} else { clog_error("[%s] x-trader.config error: Tunnel node missing.", module_name_); }
}

void TunnRptProducer::OnConnection(ERR_NO errNo, const char* pErrStr )
{
    clog_warning("[%s] OnConnection-err:%d; err info:%s", module_name_,errNo,pErrStr);
	this->ReqLogin();
}

void TunnRptProducer::OnDisConnection(ERR_NO errNo, const char* pErrStr )
{
    clog_warning("[%s] OnDisConnection-err:%d; err info:%s", module_name_,errNo,pErrStr);
}

int TunnRptProducer::ReqOrderInsert(EES_EnterOrderField *p)
{
	RESULT ret = api_->EnterOrder(p);
	// report rejected if ret!=0
	if (ret != 0)
	{
		time_t rawtime;
		time (&rawtime);
		clog_error("[%s][%s] EnterOrder- ret=%d - %s", 
			module_name_,ctime(&rawtime),ret, EESDatatypeFormater::ToString(p));
	}
	else 
	{
	//	clog_info("[%s] EnterOrder-ret=%d-%s", 
	//				module_name_, 
	//				ret, 
	//				EESDatatypeFormater::ToString(p));
	}

	return ret;
}

// 撤单操作请求
int TunnRptProducer::ReqOrderAction(EES_CancelOrder *p)
{
	RESULT ret = api_->CancelOrder(p);
	if (ret != 0)
	{
		time_t rawtime;
		time (&rawtime);
		clog_error("[%s][%s] ReqOrderAction- ret=%d - %s", 
			module_name_,ctime(&rawtime),ret, EESDatatypeFormater::ToString(p));
	} 
	else 
	{
		//clog_info("[%s] ReqCancelOrder - ret=%d - %s", 
		//			module_name_, 
		//			ret, 
		//			EESDatatypeFormater::ToString(p));
	}

	return ret;
}

void TunnRptProducer::ReqLogin()
{
	RESULT err = api_->UserLogon(
				config_.userid.c_str(), 
				config_.password.c_str(),
				this->appid_,
				this->authcode_);
    clog_warning("[%s] ReqLogin-err_no,%d; userid:%s; pwd:%s; appid:%s; authcode:%s",
				module_name_,
				err,
				config_.userid.c_str(),
				config_.password.c_str(),
				this->appid_,
				this->authcode_);
}

void TunnRptProducer::OnUserLogon(EES_LogonResponse* pLogon)
{
	counter_ = GetCounterByLocalOrderID(pLogon->m_MaxToken);
	counter_++;

    clog_warning("[%s] OnUserLogon-result:%d; user:%d; counter_:%u; m_MaxToken:%u", 
				module_name_,
				pLogon->m_Result,
				pLogon->m_UserId,
				this->counter_,
				pLogon->m_MaxToken);
	fflush (Log::fp);
}

void TunnRptProducer::OnOrderAccept(EES_OrderAcceptField* pAccept )
{
	if (ended_) return;

	int32_t cursor = Push();
	struct TunnRpt &rpt = rpt_buffer_[cursor];
	rpt.LocalOrderID = pAccept->m_ClientOrderToken;
	rpt.SysOrderID = pAccept->m_MarketOrderToken;

    //clog_info("[%s] OnOrderAccept:%s", module_name_, EESDatatypeFormater::ToString(pAccept));

	if (pAccept->m_OrderState==EES_OrderState_order_dead){
		time_t rawtime;
		time (&rawtime);
		clog_error("[%s][%s] OnOrderAccept:%s",module_name_,ctime(&rawtime),
			EESDatatypeFormater::ToString(pAccept));
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
		EESDatatypeFormater::ToString(pReject));

	int32_t cursor = Push();
	struct TunnRpt &rpt = rpt_buffer_[cursor];
	rpt.LocalOrderID = pReject->m_ClientOrderToken;
	rpt.OrderStatus = SIG_STATUS_CANCEL;
	rpt.ErrorID = pReject->m_ReasonCode;

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
	rpt.SysOrderID = pAccept->m_MarketOrderToken;
	rpt.OrderStatus = SIG_STATUS_ENTRUSTED;

    //clog_info("[%s] OnOrderMarketAccept:%s", module_name_, EESDatatypeFormater::ToString(pAccept));

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
		EESDatatypeFormater::ToString(pReject));

	int32_t cursor = Push();
	struct TunnRpt &rpt = rpt_buffer_[cursor];
	rpt.LocalOrderID = pReject->m_ClientOrderToken;
	rpt.SysOrderID = pReject->m_MarketOrderToken;
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

    //clog_info("[%s] OnOrderExecution:%s", 
	//			module_name_, 
	//			EESDatatypeFormater::ToString(pExec));

	int32_t cursor = Push();
	struct TunnRpt &rpt = rpt_buffer_[cursor];
	rpt.LocalOrderID = pExec->m_ClientOrderToken;
	rpt.SysOrderID = pExec->m_MarketOrderToken;
	rpt.OrderStatus = SIG_STATUS_PARTED;
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

void TunnRptProducer::OnOrderCxled(EES_OrderCxled* pCxled)
{
	if (ended_) return;

	//clog_info("[%s] OnOrderCxled:%s", module_name_,EESDatatypeFormater::ToString(pCxled));

	int32_t cursor = Push();
	struct TunnRpt &rpt = rpt_buffer_[cursor];
	rpt.LocalOrderID = pCxled->m_ClientOrderToken;
	rpt.SysOrderID = pCxled->m_MarketOrderToken;
	rpt.OrderStatus = SIG_STATUS_CANCEL;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = cursor;
	ivalue->data = TUNN_RPT;
	(vrt_producer_publish(producer_));
}

void TunnRptProducer::OnCxlOrderReject(EES_CxlOrderRej* pReject)
{
	if (ended_) return;

	clog_warning("[%s] OnCxlOrderReject:%s",module_name_,
		EESDatatypeFormater::ToString(pReject));
}

void TunnRptProducer::OnMarketSessionStatReport(EES_MarketSessionId MarketSessionId,
			bool ConnectionGood)
{
	clog_warning("[%s] OnMarketSessionStatReport-MarketSessionId:%hhu; ConnectionGood:%d",
		module_name_,MarketSessionId,(int)ConnectionGood);
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

