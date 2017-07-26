#include "tunn_rpt_producer.h"
#include <tinyxml.h>
#include <tinystr.h>

TunnRptProducer::TunnRptProducer(struct vrt_queue  *queue)
{
	this->ParseConfig();

	clog_info("[%s] RPT_BUFFER_SIZE: %d;", CLOG_MODULE, RPT_BUFFER_SIZE);
	rip_check(this->procucer_ = vrt_producer_new("tunnrpt_producer", 1, queue));
	this->procucer_ ->yield = vrt_yield_strategy_threaded();

	// create X1 object
	char addr[2048];
	strcpy_s(addr, this->config_.addr.c_str());
    api_ = CX1FtdcTraderApi::CreateCX1FtdcTraderApi(); 
	while (1) {
		if (-1 == api_->Init(addr, this)) {
			clog_error("[%s] X1 Api init failed.", CLOG_MODULE);
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		else break;
	}

	clog_info("[%s] X1 Api: connection to front machine succeeds.", CLOG_MODULE);
}

TunnRptProducer::~TunnRptProducer()
{
	if (this->producer_ != NULL){
		vrt_producer_.free(this->producer_);
		this->producer_ = NULL;
		clog_info("[%s] release tunnrpt_producer.", CLOG_MODULE);
	}

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
	while (tunn_node != NULL){
		this->config_.address = tunn_node->Attribute("address");
		this->config_.brokerid = tunn_node->Attribute("brokerid");
		this->config_.userid = tunn_node->Attribute("userid");
		this->config_.password = tunn_node->Attribute("password");

		clog_info("[%s] tunn config:address:%s; brokerid:%s; userid:%s; password:%s",
					CLOG_MODULE, this->config_.address, 
					this->config_.brokerid,
					this->config_.userid,
					this->config_.password);
	}
	else{
		clog_error("[%s] x-trader.config error: Tunnel node missing.", CLOG_MODULE);
	}
}

int TunnRptProducer::ReqOrderInsert(CX1FtdcInsertOrderField *p)
{
	ret = api_->ReqInsertOrder(p);
	clog_degug("[%s] ReqInsertOrder - ret=%d - %s", 
				CLOG_MODULE, ret, X1DatatypeFormater::ToString(p).c_str());
	return ret;
}

// 撤单操作请求
int TunnRptProducer::ReqOrderAction(CX1FtdcCancelOrderField *p)
{
	ret = api_->ReqCancelOrder(p);
	clog_degug("[%s] ReqCancelOrder - ret=%d - %s", 
					CLOG_MODULE, ret, X1DatatypeFormater::ToString(p).c_str());
	return ret;
}

int TunnRptProducer::QryPosition(CX1FtdcQryPositionDetailField *p)
{
	clog_info("[%s] QryPosition- do NOT support this function.", CLOG_MODULE);
	return 0;
}

int TunnRptProducer::QryOrderDetail(CX1FtdcQryOrderField *p)
{
	clog_info("[%s] QryOrderDetail- do NOT support this function.", CLOG_MODULE);
	return 0;
}

int TunnRptProducer::QryTradeDetail(CX1FtdcQryMatchField *p)
{
	clog_info("[%s] QryTradeDetail- do NOT support this function.", CLOG_MODULE);
	return 0;
}

int TunnRptProducer::ReqForQuoteInsert(CX1FtdcForQuoteField *p)
{
	return 0;
}

///报价录入请求
int TunnRptProducer::ReqQuoteInsert(CX1FtdcQuoteInsertField *p)
{
	return 0;
}

///报价操作请求
int TunnRptProducer::ReqQuoteAction(CX1FtdcCancelOrderField *p)
{
	return 0;
}

void TunnRptProducer::ReqLogin()
{
    CX1FtdcReqUserLoginField login_data;
    memset(&login_data, 0, sizeof(CX1FtdcReqUserLoginField));
    strncpy(login_data.AccountID, this->config_.userid.c_str(), sizeof(login_data.AccountID));
    strncpy(login_data.Password, this->config_.password, sizeof(login_data.Password));
    
	int rtn = api_->ReqUserLogin(&login_data);

    clog_info("[%s] ReqLogin:  err_no,%d",CLOG_MODULE, rtn );
    clog_info("[%s] ReqLogin:  \n%s", 
			CLOG_MODULE, X1DatatypeFormater::ToString(&login_data).c_str());
}

void TunnRptProducer::OnFrontConnected()
{
    clog_info("[%s] OnFrontConnected.", CLOG_MODULE);
	this->ReqLogin();
}

void TunnRptProducer::OnFrontDisconnected(int nReason)
{
    clog_info("[%s] OnFrontDisconnected, nReason=%d", CLOG_MODULE, nReason);
}

void TunnRptProducer::OnRspUserLogin(struct CX1FtdcRspUserLoginField* pfield, struct CX1FtdcRspErrorField * perror)
{
    clog_info("[%s] OnRspUserLogin:  \n%s \n%s",
        CLOG_MODULE,
		X1DatatypeFormater::ToString(pf).c_str(),
        X1DatatypeFormater::ToString(pe).c_str());

    if (perror == NULL) {
		clog_info("[%s] OnRspUserLogin,error: %d", CLOG_MODULE, pfield->LoginResult);
    }
    else {
		clog_info("[%s] OnRspUserLogin, error: %d", CLOG_MODULE, perror->ErrorID);
    }
}

void TunnRptProducer::OnRspUserLogout(struct CX1FtdcRspUserLogoutInfoField* pf, struct CX1FtdcRspErrorField * pe)
{
    clog_info("[%s] OnRspUserLogout:  \n%s \n%s",
        CLOG_MODULE,
		X1DatatypeFormater::ToString(pf).c_str(),
        X1DatatypeFormater::ToString(pe).c_str());
}

void TunnRptProducer::OnRspInsertOrder(struct CX1FtdcRspOperOrderField* pfield, struct CX1FtdcRspErrorField* peror)
{
    clog_debug("[%s] OnRspInsertOrder:  \n%s \n%s",
        CLOG_MODULE,
		X1DatatypeFormater::ToString(pf).c_str(),
        X1DatatypeFormater::ToString(pe).c_str());

	struct TunnRpt rpt;
    memset(&rpt, 0, sizeof(rpt));
	if (perror != NULL) {
		rpt.LocalOrderID = LocalOrderID;
		rpt.OrderStatus = X1_FTDC_SPD_ERROR;
		rpt.ErrorID = ErrorID;
	}
	else {
		rpt.LocalOrderID = LocalOrderID;
		rpt.OrderStatus = OrderStatus;
	}

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	rpi_check(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = push(rpt);
	ivalue->data = TUNN_RPT;

	clog_debug("[%s] OnRspInsertOrder: index,%d; data,%d; LocalOrderID:%d",
				CLOG_MODULE, ivalue->index, ivalue->data, pfield->LocalOrderID);

	rpi_check(vrt_producer_publish(producer_));
}

int32_t MDProducer::push(const TunnRpt& rpt){
	static int32_t cursor = RPT_BUFFER_SIZE - 1;
	cursor++;
	if (cursor%RPT_BUFFER_SIZE == 0){
		cursor = 0;
	}
	rpt_buffer_[cursor] = rpt;

	clog_debug("[%s] push TunnRpt: cursor,%d; LocalOrderID:%d;",
				CLOG_MODULE, cursor, rpt->LocalOrderID);

	return cursor;
}

void TunnRptProducer::OnRspCancelOrder(struct CX1FtdcRspOperOrderField* pfield, struct CX1FtdcRspErrorField* perror)
{
    clog_debug("[%s] OnRspCancelOrder:  \n%s \n%s",
        CLOG_MODULE,
		X1DatatypeFormater::ToString(pf).c_str(),
        X1DatatypeFormater::ToString(pe).c_str());

	if (pfield->OrderStatus == X1_FTDC_SPD_IN_CANCELED){
		struct TunnRpt rpt;
		memset(&rpt, 0, sizeof(rpt));
		rpt.LocalOrderID = LocalOrderID;
		rpt.OrderStatus = OrderStatus;

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		rpi_check(vrt_producer_claim(producer_, &vvalue));
		ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
		ivalue->index = push(rpt);
		ivalue->data = TUNN_RPT;

		clog_debug("[%s] OnRspCancelOrder: index,%d; data,%d; LocalOrderID:%d",
					CLOG_MODULE, ivalue->index, ivalue->data, pfield->LocalOrderID);

		rpi_check(vrt_producer_publish(producer_));
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


void TunnRptProducer::OnRtnErrorMsg(struct CX1FtdcRspErrorField* pe)
{
    clog_error("[%s] OnRtnErrorMsg:  \n%s", CLOG_MODULE, X1DatatypeFormater::ToString(pe).c_str());
}

void TunnRptProducer::OnRtnMatchedInfo(struct CX1FtdcRspPriMatchInfoField* pfield)
{
    clog_error("[%s] OnRtnMatchedInfo:  \n%s", CLOG_MODULE, X1DatatypeFormater::ToString(pf).c_str());

	struct TunnRpt rpt;
    memset(&rpt, 0, sizeof(rpt));
	rpt.LocalOrderID = LocalOrderID;
	rpt.OrderStatus = OrderStatus;
	rpt.MatchedAmount = MatchedAmount;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	rpi_check(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = push(rpt);
	ivalue->data = TUNN_RPT;

	clog_debug("[%s] OnRtnMatchedInfo: index,%d; data,%d; LocalOrderID:%d",
				CLOG_MODULE, ivalue->index, ivalue->data, pfield->LocalOrderID);

	rpi_check(vrt_producer_publish(producer_));
}

void TunnRptProducer::OnRtnOrder(struct CX1FtdcRspPriOrderField* pfield)
{
    clog_info("[%s] OnRtnOrder:  \n%s", CLOG_MODULE, X1DatatypeFormater::ToString(pf).c_str());

	struct TunnRpt rpt;
    memset(&rpt, 0, sizeof(rpt));
	rpt.LocalOrderID = LocalOrderID;
	rpt.OrderStatus = OrderStatus;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	rpi_check(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = push(rpt);
	ivalue->data = TUNN_RPT;

	clog_debug("[%s] OnRtnOrder: index,%d; data,%d; LocalOrderID:%d",
				CLOG_MODULE, ivalue->index, ivalue->data, pfield->LocalOrderID);

	rpi_check(vrt_producer_publish(producer_));
}

void TunnRptProducer::OnRtnCancelOrder(struct CX1FtdcRspPriCancelOrderField* pfield)
{
    clog_info("[%s] OnRtnCancelOrder:  \n%s", CLOG_MODULE, X1DatatypeFormater::ToString(pf).c_str());

	if (pfield->OrderStatus == X1_FTDC_SPD_CANCELED ||
		pfield->OrderStatus == X1_FTDC_SPD_PARTIAL_CANCELED ||
		pfield->OrderStatus == X1_FTDC_SPD_IN_CANCELED){
		struct TunnRpt rpt;
		memset(&rpt, 0, sizeof(rpt));
		rpt.LocalOrderID = LocalOrderID;
		rpt.OrderStatus = OrderStatus;

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		rpi_check(vrt_producer_claim(producer_, &vvalue));
		ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
		ivalue->index = push(rpt);
		ivalue->data = TUNN_RPT;

		clog_debug("[%s] OnRtnCancelOrder: index,%d; data,%d; LocalOrderID:%d",
					CLOG_MODULE, ivalue->index, ivalue->data, pfield->LocalOrderID);

		rpi_check(vrt_producer_publish(producer_));
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
    clog_info("[%s] OnRtnExchangeStatus:  \n%s", CLOG_MODULE, X1DatatypeFormater::ToString(pf).c_str());
}

long TunnRptProducer::NewLocalOrderID()
{
	// TODO:
}

void TunnRptProducer::CancelOrder(CX1FtdcCancelOrderField &field)
{
	// TODO:
}

const char* TunnRptProducer::GetAccount()
{
	return config_.userid.c_str();
}
