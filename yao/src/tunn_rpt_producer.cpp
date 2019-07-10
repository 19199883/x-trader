// done
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
#include <fstream>
#include "ctp_data_formater.h"
#include "DataCollect.h"

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

		
	char pSystemInfo[344];
	int len;
	int rtn = CTP_GetSystemInfo(pSystemInfo, len);
	clog_warning("[%s] CTP_GetSystemInfo:%d; systeminfo:%s",
				module_name_, rtn, pSystemInfo);
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
	clog_info("[%s] release tunnrpt_producer.", module_name_);
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
		strcpy(this->appid_, tunn_node->Attribute("appid"));
		strcpy(this->authcode_, tunn_node->Attribute("authcode"));

		clog_warning("[%s] tunn config:address:%s; brokerid:%s; userid:%s; password:%s"
					"appid:%s, authcode:%s",
					module_name_, 
					this->config_.address.c_str(), 
					this->config_.brokerid.c_str(),
					this->config_.userid.c_str(),
					this->config_.password.c_str(),
					this->appid_,
					this->authcode_);
	} else { clog_error("[%s] x-trader.config error: Tunnel node missing.", module_name_); }
}

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

// 撤单操作请求
int TunnRptProducer::ReqOrderAction(CThostFtdcInputOrderActionField *p)
{
#ifdef LATENCY_MEASURE
	high_resolution_clock::time_point t0 = high_resolution_clock::now();
#endif
	int localordid = stoi(p->OrderRef);
	int counter = GetCounterByLocalOrderID(localordid);
	cancel_requests_[counter] = true;
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


void TunnRptProducer::ReqLogin()
{
    CThostFtdcReqUserLoginField login_data;
    memset(&login_data, 0, sizeof(CThostFtdcReqUserLoginField));
	strncpy(login_data.BrokerID, this->config_.brokerid.c_str(), sizeof(login_data.BrokerID));
    strncpy(login_data.UserID, this->config_.userid.c_str(), sizeof(login_data.UserID));
    strncpy(login_data.Password, this->config_.password.c_str(), sizeof(login_data.Password));
    
	int rtn = api_->ReqUserLogin(&login_data, 0);

    clog_warning("[%s] ReqLogin:  err_no,%d",module_name_, rtn );
    clog_warning("[%s] ReqLogin:   %s", 
			module_name_, CtpDatatypeFormater::ToString(&login_data).c_str());
}

void TunnRptProducer::OnFrontConnected()
{
    clog_warning("[%s] OnFrontConnected.", module_name_);

	CThostFtdcReqAuthenticateField a = { 0 };
	strcpy(a.BrokerID, this->config_.brokerid.c_str());
	strcpy(a.UserID, this->config_.userid.c_str());
	strcpy(a.UserProductInfo, this->appid_);
	strcpy(a.AuthCode, this->authcode_); 
	strcpy(a.AppID, this->appid_);
	int ret = api_->ReqAuthenticate(&a, 0);
	clog_warning("[%s] ReqAuthenticate - ret=%d - %s", 
		module_name_, ret, CtpDatatypeFormater::ToString(&a).c_str());
}

void TunnRptProducer::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, 
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo==NULL || 0==pRspInfo->ErrorID) {
		clog_warning("[%s] OnRspAuthenticate successfully.", module_name_);
		
    }else {
		clog_error("[%s] OnRspAuthenticate , error: %d, msg: %s", module_name_, 
			pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
	this->ReqLogin();
}

void TunnRptProducer::OnFrontDisconnected(int nReason)
{
    clog_error("[%s] OnFrontDisconnected, nReason=%d", module_name_, nReason);
}

void TunnRptProducer::OnHeartBeatWarning(int nTimeLapse)
{
	clog_warning("[%s] OnHeartBeatWarning.", module_name_);
}

int TunnRptProducer::GetTradingDay()
{
	return 0; //this->TradingDay_;
}

//bool TunnRptProducer::IsNightTrading()
//{
//	return this->IsNightTrading;
//}

void TunnRptProducer::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, 
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    clog_warning("[%s] OnRspUserLogin:%s %s",
        module_name_,
		CtpDatatypeFormater::ToString(pRspUserLogin).c_str(),
        CtpDatatypeFormater::ToString(pRspInfo).c_str());

	//this->TradingDay_ = stoi(pRspUserLogin->TradingDay);

	char login_hour[3] = {0};
//	strncpy(login_hour, pRspUserLogin->loginTime, 2);
//	int hours = stoi(login_hour);
//	if(hours>=8 && hours<16){
//		this->IsNightTrading = 0;
//	}else{
//		this->IsNightTrading = 1;
//	}
//    clog_warning("[%s] TradingDay:%d; IsNightTrading:%d",
//				module_name_,
//				this->config_.TradingDay,
//				this->config_.IsNightTrading);

	this->frontid_ = pRspUserLogin->FrontID;	
	this->sessionid_ = pRspUserLogin->SessionID;	
	if(strcmp(pRspUserLogin->MaxOrderRef, "")==0){
		this->MaxOrderRef_ = 0;
	}else{
		this->MaxOrderRef_ =  stoi(pRspUserLogin->MaxOrderRef);
		clog_warning("[%s] MaxOrderRef_ :%d", module_name_, MaxOrderRef_);
	}
	this->counter_ = GetCounterByLocalOrderID(this->MaxOrderRef_);
	clog_warning("[%s] counter_:%d", module_name_, this->counter_);
	
    if (pRspInfo==NULL || 0==pRspInfo->ErrorID) {
		clog_warning("[%s] OnRspUserLogin successfully.", module_name_);
		
		CtpFieldConverter::InitNewOrder(this->config_.userid.c_str(), 
			this->config_.brokerid.c_str());
		CtpFieldConverter::InitCancelOrder(this->config_.userid.c_str(), 
			this->config_.brokerid.c_str(), this->frontid_, this->sessionid_);
	
		CThostFtdcSettlementInfoConfirmField req;
        memset(&req, 0, sizeof(req));
        strncpy(req.BrokerID, this->config_.brokerid.c_str(), sizeof(TThostFtdcBrokerIDType));
		// TODO:
        strncpy(req.InvestorID, this->config_.userid.c_str(), sizeof(TThostFtdcInvestorIDType));
        int ret = api_->ReqSettlementInfoConfirm(&req, 0);
        clog_warning("[%s] ReqSettlementInfoConfirm, return: %d", module_name_, ret);
		

    }else {
		clog_error("[%s] OnRspUserLogin, error: %d, msg: %s", module_name_, 
			pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
}

void TunnRptProducer::OnRspSettlementInfoConfirm(
	CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{    
     clog_warning("[%s] OnRspSettlementInfoConfirm: requestid = %d, last_flag=%d \n%s \n%s",
         module_name_,
		 nRequestID,
		 (int)bIsLast,
         CtpDatatypeFormater::ToString(pSettlementInfoConfirm).c_str(),
         CtpDatatypeFormater::ToString(pRspInfo).c_str());    

	// 查询仓位
	CThostFtdcQryInvestorPositionField a;
	memset(&a, 0, sizeof(a));
	strncpy(a.BrokerID, this->config_.brokerid.c_str(), sizeof(a.BrokerID));
	strncpy(a.InvestorID, this->config_.userid.c_str(), sizeof(a.InvestorID));
	strncpy(a.InstrumentID, "", sizeof(a.InstrumentID));//不填写合约则返回所有持仓
	int ret = api_->ReqQryInvestorPosition(&a, 0);
	if (ret != 0){
		clog_warning("[%s] ReqQryInvestorPosition- ret=%d - %s", 
			module_name_, ret, CtpDatatypeFormater::ToString(&a).c_str());
	}else {
		clog_info("[%s] ReqQryInvestorPosition- ret=%d - %s", 
			module_name_, ret, CtpDatatypeFormater::ToString(&a).c_str());
	}
}

bool TunnRptProducer::Ended()
{
	return ended_;
}

void TunnRptProducer::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, 
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    clog_info("[%s] OnRspUserLogout:%s %s",
        module_name_,
		CtpDatatypeFormater::ToString(pUserLogout).c_str(),
        CtpDatatypeFormater::ToString(pRspInfo).c_str());
		
	if (pRspInfo==NULL || 0==pRspInfo->ErrorID) {
		clog_warning("[%s] OnRspUserLogout successfully.", module_name_);
    }
    else {
		clog_error("[%s] OnRspUserLogout, error: %d, msg: %s", module_name_, 
			pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
}

bool TunnRptProducer::IsFinal(TThostFtdcOrderStatusType   OrderStatus)
{
	// TODO: 状态的各种值需要重新核对，因版本不同有变化
	if(THOST_FTDC_OST_AllTraded==OrderStatus ||
		THOST_FTDC_OST_PartTradedNotQueueing==OrderStatus ||
		THOST_FTDC_OST_NoTradeNotQueueing==OrderStatus ||
		THOST_FTDC_OST_Canceled==OrderStatus){
			return true;
		}else{
			return false;
		}
}

void TunnRptProducer::End()
{
	if(!ended_){
		CThostFtdcUserLogoutField logoutinfo;
		memset(&logoutinfo, 0, sizeof(logoutinfo));		 
		strncpy(logoutinfo.BrokerID, this->config_.brokerid.c_str(), 
				sizeof(logoutinfo.BrokerID));
		strncpy(logoutinfo.UserID, this->config_.userid.c_str(), 
				sizeof(logoutinfo.UserID));		    
		int rtn = api_->ReqUserLogout(&logoutinfo, 0);
		
		if (NULL != api_) {
			api_->RegisterSpi(NULL);
			api_->Release();
			//api_ = NULL;
			ended_ = true;		
			clog_warning("[%s]api release.", module_name_);
		}
		(vrt_producer_eof(producer_));
		clog_warning("[%s] End exit", module_name_);
	}
}

void TunnRptProducer::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, 
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (ended_) return;
	
	 clog_info("[%s] OnRspOrderInsert:%s %s; bIsLast: %d",
        module_name_,
		CtpDatatypeFormater::ToString(pInputOrder).c_str(),
        CtpDatatypeFormater::ToString(pRspInfo).c_str(),
		(int)bIsLast);
		
	if (pRspInfo==NULL || 0==pRspInfo->ErrorID) {				
    }else {		
		 clog_error("[%s] OnRspOrderInsert:%s %s",
        module_name_,
		CtpDatatypeFormater::ToString(pInputOrder).c_str(),
        CtpDatatypeFormater::ToString(pRspInfo).c_str());
				
		int32_t cursor = Push();		
		struct TunnRpt &rpt = rpt_buffer_[cursor]; // LocalOrderID也只需要赋值一次
		rpt.LocalOrderID = stoi(pInputOrder->OrderRef);
		rpt.OrderStatus = THOST_FTDC_OST_Canceled;
		rpt.ErrorID = pRspInfo->ErrorID;

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		(vrt_producer_claim(producer_, &vvalue));
		ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
		ivalue->index = cursor;
		ivalue->data = TUNN_RPT;
		
		clog_info("[%s] OnRspInsertOrder: index,%d; data,%d; LocalOrderID:%s",
				  module_name_, 
				  ivalue->index, 
				  ivalue->data, 
				  pInputOrder->OrderRef);
		
		(vrt_producer_publish(producer_));		
	}
}

void TunnRptProducer::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, 
	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	clog_info("[%s] OnRspOrderAction:%s %s; bIsLast: %d",
        module_name_,
		CtpDatatypeFormater::ToString(pInputOrderAction).c_str(),
        CtpDatatypeFormater::ToString(pRspInfo).c_str(),
		(int)bIsLast);
		
	if (pRspInfo==NULL || 0==pRspInfo->ErrorID) {				
    }else {		
		 clog_error("[%s] OnRspOrderAction:%s %s",
					module_name_,
					CtpDatatypeFormater::ToString(pInputOrderAction).c_str(),
					CtpDatatypeFormater::ToString(pRspInfo).c_str());
	}
}
		
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
	
void TunnRptProducer::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	if (ended_) return;

    clog_info("[%s] OnRtnOrder:%s", 
				module_name_, 
				CtpDatatypeFormater::ToString(pOrder).c_str());

	if (pOrder->OrderStatus == THOST_FTDC_OSS_InsertRejected){
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
				module_name_, 
				ivalue->index, 
				ivalue->data, 
				pOrder->OrderRef);

	(vrt_producer_publish(producer_));	
}

// 看CThostFtdcOrderField.VolumeTraded是否有值
void TunnRptProducer::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	if (ended_) return;

    clog_info("[%s] OnRtnTrade:%s", 
				module_name_, 
				CtpDatatypeFormater::ToString(pTrade).c_str());
}

//  看《CTP问题列表》，该回调可以忽略
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
	}
}

void TunnRptProducer::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, 
	CThostFtdcRspInfoField *pRspInfo)
{
	if (pRspInfo==NULL || 0==pRspInfo->ErrorID) {				
		 clog_info("[%s] OnErrRtnOrderAction:%s",
					 module_name_,		
					 CtpDatatypeFormater::ToString(pRspInfo).c_str());		
    }else {		
		 clog_error("[%s] OnErrRtnOrderAction:%s",
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
	// 如果没有仓位，则pInvestorPosition,pRspInfo为NULL
	clog_warning("[%s] OnRspQryInvestorPosition:%s %s,isLast:%d",
				module_name_,
				CtpDatatypeFormater::ToString(pInvestorPosition).c_str(),
				CtpDatatypeFormater::ToString(pRspInfo).c_str(),
				(int)bIsLast);	

	
	if(NULL != pRspInfo && 0 != pRspInfo->ErrorID ){
		clog_error("[%s] OnRspQryInvestorPosition:%s %s,isLast:%d",
					module_name_,
					CtpDatatypeFormater::ToString(pInvestorPosition).c_str(),
					CtpDatatypeFormater::ToString(pRspInfo).c_str(),
					(int)bIsLast);	
		
		exit (EXIT_FAILURE);
	}else{
		FillInitPosition(pInvestorPosition);
	}
	
	if(bIsLast){
		SavePosition();

		// TODO: yao. pos_redist.py代码在实盘时需要修改
		// 为每个策略重新分配仓位
		char cmd[200];
		sprintf(cmd, "sh pos_redis.sh");
		system(cmd);

		position_ready_ = true;
	}
}

void TunnRptProducer::FillInitPosition(CThostFtdcInvestorPositionField *posiField)
{
	int index = 0;

	// 同一合约，保证存储在_cur_pos，_yesterday_pos中存储在同一个索引位置，方便查找
//	if (posiField != NULL) {		
//		int i=0;
//		for(; i<init_positions._cur_pos.symbol_cnt_; i++){
//			if(strcmp(init_positions._cur_pos[i].symbol,posField->InstrumentID)==0){
//				break;
//			}
//		}
//		index = i;
//		strncpy(init_positions._cur_pos[i].symbol, posField->InstrumentID,
//				sizeof(init_positions._cur_pos[i].symbol);
//		strncpy(init_positions._yesterday_pos[i].symbol, posField->InstrumentID,
//				sizeof(init_positions._yesterday_pos[i].symbol);
//
//		if(posField->PosiDirection==THOST_FTDC_PD_Long){
//			init_positions._cur_pos[i].long_volume = posField->TodayPosition;
//			init_positions._yesterday_pos[i].long_volume = 
//				posField->Position - posField->TodayPosition;
//		}
//		else if(posField->PosiDirection==THOST_FTDC_PD_Short){
//			init_positions._cur_pos[i].short_volume = posField->TodayPosition;
//			init_positions._yesterday_pos[i].short_volume = 
//				posField->Position - posField->TodayPosition;
//		}
//		else{
//			clog_warning("[%s] CThostFtdcInvestorPositionField.PosiDirection is net position!", 
//				module_name_);
//		}
	//}
}

void TunnRptProducer::SavePosition()
{
//	std::ofstream of;
//	of.open("pos_sum.pos",std::ofstream::trunc);
//	if (of.good()) {
//		for(int i=0; i<init_positions._cur_pos.symbol_cnt_; i++){
//			char buffer[10];
//			const char *contract = init_positions._cur_pos[i].symbol;
//			// contract
//			of.write(contract, strlen(contract));	
//			of.write(";", 1);	
//			// yesterday long position
//			snprintf (buffer, sizeof(buffer), "%d", init_positions._yesterday_pos[i].long_volume);
//			of.write(buffer, strlen(buffer));
//			of.write(";", 1);	
//			// yesterday short position
//			snprintf (buffer, sizeof(buffer), "%d", init_positions._yesterday_pos[i].short_volume);
//			of.write(buffer, strlen(buffer));
//			of.write(";", 1);	
//			// today long position
//			snprintf (buffer, sizeof(buffer), "%d", init_positions._cur_pos[i].long_volume);
//			of.write(buffer, strlen(buffer));
//			of.write(";", 1);	
//			// today short position
//			snprintf (buffer, sizeof(buffer), "%d", init_positions._cur_pos[i].short_volume);
//			of.write(buffer, strlen(buffer));
//			of.write(";", 1);	
//			of.write("\n", 1);	
//		} // end for(int i=0; i<init_positions._cur_pos.symbol_cnt_; i++){
//	}
//	else{
//		clog_error("[%s] SavePosition failed due to failure to open file.", module_name_);
//	}
//
//	of.close();
}

bool TunnRptProducer::IsReady()
{
	return position_ready_;
}

int TunnRptProducer::NewLocalOrderID(int32_t strategyid)
{	
	counter_++;
    int localorderid = strategyid+ counter_ * 1000;			
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

int32_t TunnRptProducer::GetCounterByLocalOrderID(int local_ord_id)
{
	return local_ord_id/1000;
}
