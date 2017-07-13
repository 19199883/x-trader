#include "tunn_rpt_producer.h"
#include <tinyxml.h>
#include <tinystr.h>


TunnRptProducer::TunnRptProducer(struct vrt_queue  *queue)
{
	this->ParseConfig();

	clog_info("[%s] RPT_BUFFER_SIZE: %d;", CLOG_MODULE, RPT_BUFFER_SIZE);
	rip_check(this->procucer_ = vrt_producer_new("tunnrpt_producer", 1, queue));

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
		vrt_producer_free(this->producer_);
		this->producer_ = NULL;
		clog_info("[%s] release tunnrpt_provider.", CLOG_MODULE);
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
	TNL_LOG_DEBUG("ReqInsertOrder - ret=%d - %s", ret, X1DatatypeFormater::ToString(p).c_str());
	return ret;
}

		//报单操作请求
		int ReqOrderAction(CX1FtdcCancelOrderField *p)
		{
			if (!TunnelIsReady())
			{
				TNL_LOG_WARN("ReqOrderAction when tunnel not ready");
				return TUNNEL_ERR_CODE::NO_VALID_CONNECT_AVAILABLE;
			}
			int ret = TUNNEL_ERR_CODE::RESULT_FAIL;

			try
			{
				ret = api_->ReqCancelOrder(p);
				TNL_LOG_DEBUG("ReqCancelOrder - ret=%d - %s", ret, X1DatatypeFormater::ToString(p).c_str());
				if (ret != 0)
				{
					// -2，表示未处理请求超过许可数；
					// -3，表示每秒发送请求数超过许可数。
					if (ret == -2)
					{
						return TUNNEL_ERR_CODE::CFFEX_OVER_REQUEST;
					}
					if (ret == -3)
					{
						return TUNNEL_ERR_CODE::CFFEX_OVER_REQUEST_PER_SECOND;
					}
					return TUNNEL_ERR_CODE::RESULT_FAIL;
				}
			}
			catch (...)
			{
				TNL_LOG_ERROR("unknown exception in ReqCancelOrder.");
			}

			return ret;
		}

		int QryPosition(CX1FtdcQryPositionDetailField *p)
		{
			if (!TunnelIsReady())
			{
				TNL_LOG_WARN("QryPosition when tunnel not ready");
				return TUNNEL_ERR_CODE::NO_VALID_CONNECT_AVAILABLE;
			}

			QryPosReturnHandler_(&query_info_.position_return);
			LogUtil::OnPositionRtn(&query_info_.position_return, tunnel_info_);
			return TUNNEL_ERR_CODE::RESULT_SUCCESS;
		}
		int QryOrderDetail(CX1FtdcQryOrderField *p)
		{
			if (!TunnelIsReady())
			{
				TNL_LOG_WARN("QryOrderDetail when tunnel not ready");
				return TUNNEL_ERR_CODE::NO_VALID_CONNECT_AVAILABLE;
			}

			if (QryOrderDetailReturnHandler_) QryOrderDetailReturnHandler_(&query_info_.order_return);
			LogUtil::OnOrderDetailRtn(&query_info_.order_return, tunnel_info_);
			return TUNNEL_ERR_CODE::RESULT_SUCCESS;
		}
		int QryTradeDetail(CX1FtdcQryMatchField *p)
		{
			if (!TunnelIsReady())
			{
				TNL_LOG_WARN("QryTradeDetail when tunnel not ready");
				return TUNNEL_ERR_CODE::NO_VALID_CONNECT_AVAILABLE;
			}

			if (QryTradeDetailReturnHandler_) QryTradeDetailReturnHandler_(&query_info_.trade_return);
			LogUtil::OnTradeDetailRtn(&query_info_.trade_return, tunnel_info_);
			return TUNNEL_ERR_CODE::RESULT_SUCCESS;
		}

		int ReqForQuoteInsert(CX1FtdcForQuoteField *p)
		{
			return 0;
		}

		///报价录入请求
		int ReqQuoteInsert(CX1FtdcQuoteInsertField *p)
		{
			return 0;
		}
		///报价操作请求
		int ReqQuoteAction(CX1FtdcCancelOrderField *p)
		{
			return 0;
		}


void MYX1Spi::ReqLogin()
{
    std::this_thread::sleep_for(std::chrono::seconds(3));

    CX1FtdcReqUserLoginField login_data;
    memset(&login_data, 0, sizeof(CX1FtdcReqUserLoginField));
    strncpy(login_data.AccountID, tunnel_info_.account.c_str(), sizeof(login_data.AccountID));
    strncpy(login_data.Password, pswd_.c_str(), sizeof(login_data.Password));
    int rtn = api_->ReqUserLogin(&login_data);
    TNL_LOG_INFO("ReqLogin:  err_no,%d",rtn );
    TNL_LOG_INFO("ReqLogin:  \n%s", X1DatatypeFormater::ToString(&login_data).c_str());
}

void MYX1Spi::QueryAllBeforeReady()
{
    TNL_LOG_INFO("QueryAllBeforeReady");

    int ret;

    while (query_info_.query_position_status != QueryStatus::QUERY_SUCCESS)
    {
        switch (query_info_.query_position_status)
        {
            case QueryStatus::QUERY_INIT:
            {
                query_info_.position_return.error_no = 0;
                query_info_.position_return.datas.clear();

                CX1FtdcQryPositionDetailField qry_position;
                memset(&qry_position, 0, sizeof(CX1FtdcQryPositionDetailField));
                strncpy(qry_position.AccountID, tunnel_info_.account.c_str(), sizeof(TX1FtdcAccountIDType));
                qry_position.InstrumentType = X1FTDC_INSTRUMENT_TYPE_COMM;

                while ((ret = api_->ReqQryPositionDetail(&qry_position)) != 0)
                {
                    TNL_LOG_ERROR("QryPosition fail, ret is %d", ret);
                    sleep(sleep_time);
                }
                gettimeofday(&timer_start_, NULL);
                query_info_.query_position_status = QueryStatus::QUERY_PENDING;
            }
                break;

            case QueryStatus::QUERY_PENDING:
            {
                gettimeofday(&timer_end_, NULL);
                if (timer_end_.tv_sec - timer_start_.tv_sec >= 2)
                {
                    TNL_LOG_ERROR("QryPosition timeout, retry.");
                    query_info_.query_position_status = QueryStatus::QUERY_INIT;
                }
                else
                {
                    usleep(usleep_time);
                }
            }
                break;

            case QueryStatus::QUERY_ERROR:
            {
                sleep(sleep_time);
                TNL_LOG_ERROR("QryPosition return error, retry");
                query_info_.query_position_status = QueryStatus::QUERY_INIT;
            }
                break;
        }
    }
    TNL_LOG_INFO("QryPosition success");
    TunnelUpdateState(tunnel_info_.qtm_name.c_str(), QtmState::QUERY_POSITION_SUCCESS);

    while (query_info_.query_order_status != QueryStatus::QUERY_SUCCESS)
    {
        switch (query_info_.query_order_status)
        {
            case QueryStatus::QUERY_INIT:
            {
                query_info_.order_return.error_no = 0;
                query_info_.order_return.datas.clear();

                CX1FtdcQryOrderField qry_order;
                memset(&qry_order, 0, sizeof(CX1FtdcQryOrderField));
                strncpy(qry_order.AccountID, tunnel_info_.account.c_str(), sizeof(TX1FtdcAccountIDType));
                qry_order.InstrumentType = X1FTDC_INSTRUMENT_TYPE_COMM; 
                {
                    std::lock_guard<std::mutex> lock(cancel_times_sync_);
                    cancel_times_of_contract.clear();
                }

                while ((ret = api_->ReqQryOrderInfo(&qry_order)) != 0)
                {
                    TNL_LOG_ERROR("QryOrderDetail fail, ret is %d", ret);
                    sleep(sleep_time);
                }
                gettimeofday(&timer_start_, NULL);
                query_info_.query_order_status = QueryStatus::QUERY_PENDING;
            }
                break;

            case QueryStatus::QUERY_PENDING:
            {
                gettimeofday(&timer_end_, NULL);
                if (timer_end_.tv_sec - timer_start_.tv_sec >= 2)
                {
                    TNL_LOG_ERROR("QryOrderDetail timeout, retry.");
                    query_info_.query_order_status = QueryStatus::QUERY_INIT;
                }
                else
                {
                    usleep(usleep_time);
                }
            }
                break;

            case QueryStatus::QUERY_ERROR:
            {
                sleep(sleep_time);
                TNL_LOG_ERROR("QryOrderDetail return error, retry");
                query_info_.query_order_status = QueryStatus::QUERY_INIT;
            }
                break;
        }
    }
    TNL_LOG_INFO("QryOrderDetail success");
    TunnelUpdateState(tunnel_info_.qtm_name.c_str(), QtmState::QUERY_ORDER_SUCCESS);

    while (query_info_.query_trade_status != QueryStatus::QUERY_SUCCESS)
    {
        switch (query_info_.query_trade_status)
        {
            case QueryStatus::QUERY_INIT:
            {
                query_info_.trade_return.error_no = 0;
                query_info_.trade_return.datas.clear();

                CX1FtdcQryMatchField qry_trade;
                memset(&qry_trade, 0, sizeof(CX1FtdcQryMatchField));
                strncpy(qry_trade.AccountID, tunnel_info_.account.c_str(), sizeof(TX1FtdcAccountIDType));
                qry_trade.InstrumentType = X1FTDC_INSTRUMENT_TYPE_COMM;

                while ((ret = api_->ReqQryMatchInfo(&qry_trade)) != 0)
                {
                    TNL_LOG_ERROR("QryTradeDetail fail, ret is %d", ret);
                    sleep(sleep_time);
                }
                gettimeofday(&timer_start_, NULL);
                query_info_.query_trade_status = QueryStatus::QUERY_PENDING;
            }
                break;

            case QueryStatus::QUERY_PENDING:
            {
                gettimeofday(&timer_end_, NULL);
                if (timer_end_.tv_sec - timer_start_.tv_sec >= 2)
                {
                    TNL_LOG_ERROR("QryTradeDetail timeout, retry.");
                    query_info_.query_trade_status = QueryStatus::QUERY_INIT;
                }
                else
                {
                    usleep(usleep_time);
                }
            }
                break;

            case QueryStatus::QUERY_ERROR:
            {
                sleep(sleep_time);
                TNL_LOG_ERROR("QryTradeDetail return error, retry");
                query_info_.query_trade_status = QueryStatus::QUERY_INIT;
            }
                break;
        }
    }
    TNL_LOG_INFO("QryTradeDetails success");
    TunnelUpdateState(tunnel_info_.qtm_name.c_str(), QtmState::QUERY_TRADE_SUCCESS);

    query_is_ready_ = true;
    TunnelUpdateState(tunnel_info_.qtm_name.c_str(), QtmState::API_READY);
}

void MYX1Spi::OnFrontConnected()
{
    TNL_LOG_INFO("OnFrontConnected.");
    connected_ = true;

    CX1FtdcReqUserLoginField login_data;
    memset(&login_data, 0, sizeof(CX1FtdcReqUserLoginField));
    strncpy(login_data.AccountID, tunnel_info_.account.c_str(), sizeof(login_data.AccountID));
    strncpy(login_data.Password, pswd_.c_str(), sizeof(login_data.Password));

    // 成功登录后，断开不再重新登录
    if (in_init_state_)
    {
        TunnelUpdateState(tunnel_info_.qtm_name.c_str(), QtmState::CONNECT_SUCCESS);
        int rtn = api_->ReqUserLogin(&login_data);
        TNL_LOG_INFO("ReqLogin:  err_no,%d",rtn );
        TNL_LOG_INFO("ReqLogin:  \n%s", X1DatatypeFormater::ToString(&login_data).c_str());
    }
}

void MYX1Spi::OnFrontDisconnected(int nReason)
{
    connected_ = false;
    logoned_ = false;
    is_ready_ = false;

    TunnelUpdateState(tunnel_info_.qtm_name.c_str(), QtmState::DISCONNECT);
    TNL_LOG_WARN("OnFrontDisconnected, nReason=%d", nReason);
}

void MYX1Spi::OnRspUserLogin(struct CX1FtdcRspUserLoginField* pf, struct CX1FtdcRspErrorField * pe)
{
    TNL_LOG_INFO("OnRspUserLogin:  \n%s \n%s",
        X1DatatypeFormater::ToString(pf).c_str(),
        X1DatatypeFormater::ToString(pe).c_str());

    if (pe == NULL && pf->LoginResult == 0)
    {
        session_id_ = pf->SessionID;
        max_order_ref_ = pf->InitLocalOrderID;
        xspeed_trade_context_.SetOrderRef(max_order_ref_);

        logoned_ = true;
        in_init_state_ = false;

        is_ready_ = true;
        TunnelUpdateState(tunnel_info_.qtm_name.c_str(), QtmState::LOG_ON_SUCCESS);
        std::thread qry_thread(std::bind(&MYX1Spi::QueryAllBeforeReady, this));
        qry_thread.detach();
            /*
        if (cfg_.Compliance_check_param().init_cancel_times_at_start == 1)
        {
        }
        else
        {
        }*/

        // trade day query
		// there is NOT ReqTradingDay method in X1 API
 //       DFITCTradingDayField qry_trade_day;
 //       memset(&qry_trade_day, 0, sizeof(qry_trade_day));
 //       api_->ReqTradingDay(&qry_trade_day);
 
        //期货没有撤掉所有单据接口
        // start thread for cancel unterminated orders
        //if (cancel_t_ == NULL && !have_handled_unterminated_orders_)
        //{
        //    cancel_t_ = new std::thread(std::bind(&MYXSpeedSpi::CancelUnterminatedOrders, this));
        //}
    }
    else
    {
        if (pe)
        {
            int standard_error_no = cfg_.GetStandardErrorNo(pe->ErrorID);
            TNL_LOG_WARN("OnRspUserLogin, external errorid: %d; Internal errorid: %d", pe->ErrorID, standard_error_no);
        }

        // 重新登陆
        std::thread reLogin(std::bind(&MYX1Spi::ReqLogin, this));
        reLogin.detach();
        TunnelUpdateState(tunnel_info_.qtm_name.c_str(), QtmState::LOG_ON_FAIL);
    }
}

void MYX1Spi::OnRspUserLogout(struct CX1FtdcRspUserLogoutInfoField* pf, struct CX1FtdcRspErrorField * pe)
{
    TNL_LOG_INFO("OnRspUserLogout:  \n%s \n%s",
        X1DatatypeFormater::ToString(pf).c_str(),
        X1DatatypeFormater::ToString(pe).c_str());

    try
    {
        logoned_ = false;
        TunnelUpdateState(tunnel_info_.qtm_name.c_str(), QtmState::LOG_OUT);
    }
    catch (...)
    {
        TNL_LOG_ERROR("unknown exception in OnRspUserLogout.");
    }
}

void MYX1Spi::OnRspInsertOrder(struct CX1FtdcRspOperOrderField* pf, struct CX1FtdcRspErrorField* pe)
{
    TNL_LOG_DEBUG("OnRspInsertOrder:  \n%s \n%s",
        X1DatatypeFormater::ToString(pf).c_str(),
        X1DatatypeFormater::ToString(pe).c_str());

    try
    {
        int standard_error_no = 0;
        int api_err_no = 0;
        OrderRefDataType OrderRef = 0;
        const X1OrderInfo *p = NULL;
        char entrust_status = MY_TNL_OS_ERROR;

        if (!pf)
        {
            TNL_LOG_WARN("OnRspOrderInsert, pInputOrder is null.");
            return;
        }

        if (pe)
        {
            ReportErrorState(pe->ErrorID, pe->ErrorMsg);

            api_err_no = pe->ErrorID;
            standard_error_no = cfg_.GetStandardErrorNo(api_err_no);
            OrderRef = pe->LocalOrderID;

            TNL_LOG_WARN("OnRspOrderInsert, api_err_no: %d; my_err: %d; OrderRef: %lld", api_err_no, standard_error_no, OrderRef);
        }
        else
        {
            OrderRef = pf->LocalOrderID;
            entrust_status = X1FieldConvert::EntrustStatusTrans(pf->OrderStatus);
        }

        p = xspeed_trade_context_.GetOrderInfoByLocalID(OrderRef);
        if (p)
        {
            T_OrderRespond rsp;
			// TODO: wangying, fix the bug of cancel order function:inErrorID=114,x1 order id invalid
			// give a dummy Order ID to cheat trader so that it can perform cancel order request immediately
			// without waiting for the returned order id from counter
			// trader won't perform cancel order request untill order id is greater than zero
            X1Packer::OrderRespond(standard_error_no, p->serial_no, 1, entrust_status, rsp);
            if (standard_error_no != TUNNEL_ERR_CODE::RESULT_SUCCESS)
            {
                // 报单失败，报告合规检查
                ComplianceCheck_OnOrderInsertFailed(
                    tunnel_info_.account.c_str(),
                    OrderRef,
                    p->exchange_code,
                    p->stock_code,
                    p->volume,
                    p->speculator,
                    p->open_close,
                    p->order_type);
            }

            // 应答
            if (OrderRespond_call_back_handler_) OrderRespond_call_back_handler_(&rsp);
            LogUtil::OnOrderRespond(&rsp, tunnel_info_);
        }
        else
        {
            TNL_LOG_INFO("can't get original place order info of order ref: %lld", OrderRef);
        }
    }
    catch (...)
    {
        TNL_LOG_ERROR("unknown exception in OnRspOrderInsert.");
    }
}

void MYX1Spi::OnRspCancelOrder(struct CX1FtdcRspOperOrderField* pf, struct CX1FtdcRspErrorField* pe)
{
    TNL_LOG_DEBUG("OnRspCancelOrder:  \n%s \n%s",
        X1DatatypeFormater::ToString(pf).c_str(),
        X1DatatypeFormater::ToString(pe).c_str());

    try
    {
        int standard_error_no = 0;
        int api_err_no = 0;
        OrderRefDataType OrderRef = 0;
        const X1OrderInfo *p = NULL;

        if (!pf)
        {
            TNL_LOG_WARN("OnRspCancelOrder, pInputOrder is null.");
            return;
        }

        if (pe)
        {
            ReportErrorState(pe->ErrorID, pe->ErrorMsg);
            api_err_no = pe->ErrorID;
            standard_error_no = cfg_.GetStandardErrorNo(api_err_no);
            OrderRef = pe->LocalOrderID;

            TNL_LOG_WARN("OnRspCancelOrder, api_err_no: %d; my_err: %d; OrderRef: %lld", api_err_no, standard_error_no, OrderRef);
        }
        else
        {
            OrderRef = pf->LocalOrderID;
        }

        p = xspeed_trade_context_.GetOrderInfoByLocalID(OrderRef);
        if (p)
        {
            long cancel_serial_no = p->Pop_cancel_serial_no();
            if (cancel_serial_no != 0)
            {
                T_CancelRespond rsp;
                X1Packer::CancelRespond(standard_error_no, cancel_serial_no, p->entrust_no, rsp);

                if (standard_error_no != TUNNEL_ERR_CODE::RESULT_SUCCESS)
                {
                    // 撤单拒绝，报告合规检查
                    ComplianceCheck_OnOrderCancelFailed(
                        tunnel_info_.account.c_str(),
                        p->stock_code,
                        OrderRef);
                }

                // 应答
                if (CancelRespond_call_back_handler_) CancelRespond_call_back_handler_(&rsp);
                LogUtil::OnCancelRespond(&rsp, tunnel_info_);
            }
            else
            {
                TNL_LOG_WARN("cancel order: %ld from outside.", p->serial_no);
            }
        }
        else
        {
            TNL_LOG_INFO("can't get original place order info of order ref: %lld", OrderRef);
        }
    }
    catch (...)
    {
        TNL_LOG_ERROR("unknown exception in OnRspCancelOrder.");
    }
}

// obsolete: can't get cost of yesterday position, so use interface ReqQryPositionDetail
void MYX1Spi::OnRspQryPosition(struct CX1FtdcRspPositionField* pf, struct CX1FtdcRspErrorField* pe, bool bIsLast)
{
    TNL_LOG_DEBUG("OnRspQryPosition: bIsLast=%d \n%s \n%s", bIsLast,
        X1DatatypeFormater::ToString(pf).c_str(),
        X1DatatypeFormater::ToString(pe).c_str());

//    T_PositionReturn ret;
//
//    // respond error
//    if (pe && pe->nErrorID != 0)
//    {
//    	   ReportErrorState(pe->nErrorID, pe->errorMsg);
//        ret.error_no = -1;
//        QryPosReturnHandler_(&ret);
//        LogUtil::OnPositionRtn(&ret, user_);
//        pos_qry_count_ = 0;
//        return;
//    }
//
//    if (bIsLast)
//    {
//        ++pos_rtn_count_;
//        --pos_qry_count_;
//    }
//
//    if (pf)
//    {
//        PositionDetail pos;
//        strncpy(pos.stock_code, pf->instrumentID, sizeof(pf->instrumentID));
//
//        pos.position = pf->positionAmount;
//        if (pf->buySellType == DFITC_SPD_BUY)
//        {
//            pos.direction = FUNC_CONST_DEF::CONST_ENTRUST_BUY;
//        }
//        else
//        {
//            pos.direction = FUNC_CONST_DEF::CONST_ENTRUST_SELL;
//        }
//
//        pos.position_cost = pf->positionAvgPrice * pos.position;
//        pos.yestoday_position = pf->lastAmount;
//        pos.yestoday_position_cost = pf->positionAvgPrice * pf->positionAmount - pf->openAvgPrice * pf->todayAmount;
//        pos_buffer_.push_back(pos);
//    }
//
//    if (pos_rtn_count_ == 2)
//    {
//        TNL_LOG_INFO("receive 2 pos qry result, send to client");
//        ret.datas.swap(pos_buffer_);
//        ret.error_no = 0;
//        QryPosReturnHandler_(&ret);
//        LogUtil::OnPositionRtn(&ret, user_);
//    }
}

struct PositionDetailPred
{
    PositionDetailPred(const PositionDetail &v)
        : v_(v)
    {
    }
    bool operator()(const PositionDetail &l)
    {
        return v_.direction == l.direction && strcmp(v_.stock_code, l.stock_code) == 0;
    }

private:
    const PositionDetail v_;
};

void MYX1Spi::OnRspQryPositionDetail(struct CX1FtdcRspPositionDetailField* pf, struct CX1FtdcRspErrorField* pe, bool bIsLast)
{
    TNL_LOG_DEBUG("OnRspQryPositionDetail: bIsLast=%d \n%s \n%s", bIsLast,
        X1DatatypeFormater::ToString(pf).c_str(),
        X1DatatypeFormater::ToString(pe).c_str());
    T_PositionReturn ret;

    gettimeofday(&timer_start_, NULL);

    // respond error
    if (pe && pe->ErrorID != 0)
    {
        ReportErrorState(pe->ErrorID, pe->ErrorMsg);
        query_info_.query_position_status = QueryStatus::QUERY_ERROR;
        return;
    }

    if (pf && pf->Volume > 0)
    {
        PositionDetail pos;
        memset(&pos, 0, sizeof(pos));
        strncpy(pos.stock_code, pf->InstrumentID, sizeof(pf->InstrumentID));

        if (pf->BuySellType == X1_FTDC_SPD_BUY)
        {
            pos.direction = MY_TNL_D_BUY;
        }
        else
        {
            pos.direction = MY_TNL_D_SELL;
        }

        // 今仓，当前的仓位
        pos.position = pf->Volume;
        pos.position_avg_price = pf->Volume * pf->OpenPrice; // now is total cost
        if (memcmp(trade_day, pf->MatchedDate, sizeof(trade_day)) != 0)
        {
            pos.yestoday_position = pf->Volume;
            pos.yd_position_avg_price = pf->Volume * pf->OpenPrice; // now is total cost
        }
        std::vector<PositionDetail>::iterator it = std::find_if(query_info_.position_return.datas.begin(), query_info_.position_return.datas.end(), PositionDetailPred(pos));
        if (it == query_info_.position_return.datas.end())
        {
            query_info_.position_return.datas.push_back(pos);
        }
        else
        {
            it->position += pos.position;
            it->position_avg_price += pos.position_avg_price; // now is total cost
            it->yestoday_position += pos.yestoday_position;
            it->yd_position_avg_price += pos.yd_position_avg_price; // now is total cost
        }
    }

    if (bIsLast)
    {
        CheckAndSaveYestodayPosition();
        query_info_.query_position_status = QueryStatus::QUERY_SUCCESS;
    }
}

void MYX1Spi::OnRspCustomerCapital(struct CX1FtdcRspCapitalField* pf, struct CX1FtdcRspErrorField* pe, bool bIsLast)
{
    // no request, shouldn't return
    TNL_LOG_INFO("OnRspCustomerCapital: bIsLast=%d \n%s \n%s", bIsLast,
        X1DatatypeFormater::ToString(pf).c_str(),
        X1DatatypeFormater::ToString(pe).c_str());
}

void MYX1Spi::OnRspQryExchangeInstrument(struct CX1FtdcRspExchangeInstrumentField* pf, struct CX1FtdcRspErrorField* pe,
bool bIsLast)
{
    // no request, shouldn't return
    TNL_LOG_INFO("OnRspQryExchangeInstrument: bIsLast=%d \n%s \n%s", bIsLast,
        X1DatatypeFormater::ToString(pf).c_str(),
        X1DatatypeFormater::ToString(pe).c_str());
}

//void MYX1Spi::OnRspBillConfirm(struct DFITCBillConfirmRspField* pf, struct DFITCErrorRtnField* pe)
//{
//    // no request, shouldn't return
//    TNL_LOG_INFO("OnRspBillConfirm:  \n%s \n%s",
//        XSpeedDatatypeFormater::ToString(pf).c_str(),
//        XSpeedDatatypeFormater::ToString(pe).c_str());
//}

void MYX1Spi::OnRtnErrorMsg(struct CX1FtdcRspErrorField* pe)
{
    TNL_LOG_DEBUG("OnRtnErrorMsg:  \n%s", X1DatatypeFormater::ToString(pe).c_str());

    try
    {
        if (pe && pe->SessionID == session_id_)
        {
            ReportErrorState(pe->ErrorID, pe->ErrorMsg);
            int api_err_no = pe->ErrorID;
            int standard_error_no = cfg_.GetStandardErrorNo(api_err_no);
            OrderRefDataType OrderRef = pe->LocalOrderID;
            const X1OrderInfo *p = xspeed_trade_context_.GetOrderInfoByLocalID(OrderRef);
            if (p)
            {
                long cancel_serial_no = p->Pop_cancel_serial_no();
                if (cancel_serial_no != 0)
                {
                    T_CancelRespond rsp;
                    X1Packer::CancelRespond(standard_error_no, cancel_serial_no, p->entrust_no, rsp);

                    if (standard_error_no != TUNNEL_ERR_CODE::RESULT_SUCCESS)
                    {
                        // 撤单拒绝，报告合规检查
                        ComplianceCheck_OnOrderCancelFailed(
                            tunnel_info_.account.c_str(),
                            p->stock_code,
                            OrderRef);
                    }

                    // 应答
                    if (CancelRespond_call_back_handler_) CancelRespond_call_back_handler_(&rsp);
                    LogUtil::OnCancelRespond(&rsp, tunnel_info_);
                }
                // xspeed 如果单据的状态为全成/失败/已撤/已经报入，那么不向上层相应，需要检测撤单数量
                // 例如 : A报单，A还未成交，B撤A，当B到达交易所前，A已经全成，此时B会返回错误，
                // 如果向交易程序应答，交易程序会coredump，这里做过滤
                else if ((MY_TNL_OS_WITHDRAWED == p->entrust_status)
                    || (MY_TNL_OS_COMPLETED == p->entrust_status)
                    || (MY_TNL_OS_ERROR == p->entrust_status)
                    || (MY_TNL_OS_REPORDED == p->entrust_status))
                {
                    // 撤单拒绝，报告合规检查
                    ComplianceCheck_OnOrderCancelFailed(
                        tunnel_info_.account.c_str(),
                        p->stock_code,
                        OrderRef);
                    TNL_LOG_WARN(" OnRtnErrorMsg : status error. order ref: %lld, entrust status:%c.",
                        OrderRef, p->entrust_status);
                }
                else
                {
                    T_OrderRespond rsp;
					// TODO: wangying, fix the bug of cancel order function:inErrorID=114,x1 order id invalid
					// give a dummy Order ID to cheat trader so that it can perform cancel order request immediately
					// without waiting for the returned order id from counter
					// trader won't perform cancel order request untill order id is greater than zero
                    X1Packer::OrderRespond(standard_error_no, p->serial_no, 1, MY_TNL_OS_ERROR, rsp);

                    if (standard_error_no != TUNNEL_ERR_CODE::RESULT_SUCCESS)
                    {
                        // 报单失败，报告合规检查
                        ComplianceCheck_OnOrderInsertFailed(
                            tunnel_info_.account.c_str(),
                            OrderRef,
                            p->exchange_code,
                            p->stock_code,
                            p->volume,
                            p->speculator,
                            p->open_close,
                            p->order_type);
                    }

                    // 应答
                    if (OrderRespond_call_back_handler_) OrderRespond_call_back_handler_(&rsp);
                    LogUtil::OnOrderRespond(&rsp, tunnel_info_);
                }
            }
            else
            {
                TNL_LOG_INFO("can't get original place order info of order ref: %lld", OrderRef);
            }
        }
    }
    catch (...)
    {
        TNL_LOG_ERROR("unknown exception in OnRtnErrorMsg.");
    }
}

void MYX1Spi::OnRtnMatchedInfo(struct CX1FtdcRspPriMatchInfoField* pf)
{
    TNL_LOG_DEBUG("OnRtnMatchedInfo:  \n%s", X1DatatypeFormater::ToString(pf).c_str());

    try
    {
        if (pf && pf->SessionID == session_id_)
        {
            OrderRefDataType OrderRef = pf->LocalOrderID;
            const X1OrderInfo *p = xspeed_trade_context_.GetOrderInfoByLocalID(OrderRef);
            if (p)
            {
				// TODO: wangying, fix the bug of cancel order function:inErrorID=114,x1 order id invalid
                p->entrust_no = pf->X1OrderID;

                /*
                 xspeed的状态有问题：
                 先收到全成状态（状态是 c），但是还有剩余4手。
                 然后收到部成状态（状态是 p），剩余0手。
                 导致我们系统的最后状态为部成，后面一直下发撤单。
                 规避解决：我们的通道检查单据的剩余手数，如果剩余手数为0，发送全成状态给交易系统，否则发送部分成交状态给交易系统。
                 */
                p->entrust_status = MY_TNL_OS_PARTIALCOM;
                p->volume_remain -= pf->MatchedAmount;
                if (0 == p->volume_remain)
                {
                    p->entrust_status = MY_TNL_OS_COMPLETED;
                    //全成，报告合规检查
                    ComplianceCheck_OnOrderFilled(
                        tunnel_info_.account.c_str(),
                        OrderRef);
                }

                // 委托回报
                T_OrderReturn order_return;
                X1Packer::OrderReturn(pf, p, order_return);
                OrderReturn_call_back_handler_(&order_return);
                LogUtil::OnOrderReturn(&order_return, tunnel_info_);

                // match回报
                T_TradeReturn trade_return;
                X1Packer::TradeReturn(pf, p, trade_return);
                TradeReturn_call_back_handler_(&trade_return);
                LogUtil::OnTradeReturn(&trade_return, tunnel_info_);
            }
            else
            {
                TNL_LOG_INFO("can't get original place order info of order ref: %lld", OrderRef);
            }
        }
    }
    catch (...)
    {
        TNL_LOG_ERROR("unknown exception in OnRtnMatchedInfo.");
    }
}

void MYX1Spi::OnRtnOrder(struct CX1FtdcRspPriOrderField* pf)
{
#ifdef PROBE_COST
    char tag_name[32];
    memset(tag_name, 0, sizeof(tag_name));
    sprintf(tag_name, "OrderReturn%d", order_return_count-5000);
    TIME_PROBER_SET_TAG_NAME(order_return_count*10, tag_name);
    TIME_PROBER_BEGIN(order_return_count*10);
    sprintf(tag_name, "ToString%d", order_return_count-5000);
    TIME_PROBER_SET_TAG_NAME(order_return_count*10 + 1, tag_name);
    TIME_PROBER_BEGIN(order_return_count*10 + 1);
#endif

    TNL_LOG_DEBUG("OnRtnOrder:  \n%s", X1DatatypeFormater::ToString(pf).c_str());
#ifdef PROBE_COST
    TIME_PROBER_END(order_return_count*10+1);
#endif

    try
    {
        if (pf && pf->SessionID == session_id_)
        {
#ifdef PROBE_COST
    sprintf(tag_name, "FindOrder%d", order_return_count - 5000);
    TIME_PROBER_SET_TAG_NAME(order_return_count*10 + 2, tag_name);
    TIME_PROBER_BEGIN(order_return_count*10 + 2);
#endif
            OrderRefDataType OrderRef = pf->LocalOrderID;
            const X1OrderInfo *p = xspeed_trade_context_.GetOrderInfoByLocalID(OrderRef);
#ifdef PROBE_COST
    TIME_PROBER_END(order_return_count*10+2);
#endif
            if (p)
            {
#ifdef PROBE_COST
    sprintf(tag_name, "ComplianceCheck%d", order_return_count - 5000);
    TIME_PROBER_SET_TAG_NAME(order_return_count*10 + 3, tag_name);
    TIME_PROBER_BEGIN(order_return_count*10 + 3);
#endif
                // xspeed版本存在消息乱序情况，这里进行过滤
                // 例如 : 一个报单先收到全成状态，然后收到已经进入队列状态
                int entrust_status = p->entrust_status;
                if ((MY_TNL_OS_WITHDRAWED == entrust_status)
                    || (MY_TNL_OS_COMPLETED == entrust_status)
                    || (MY_TNL_OS_ERROR == entrust_status))
                {
                    TNL_LOG_WARN("entrust status error. order ref: %lld, entrust status:%d, order status:%d ",
                        OrderRef, entrust_status, pf->OrderStatus);
                    return;
                }

				// TODO: wangying, fix the bug of cancel order function:inErrorID=114,x1 order id invalid
                p->entrust_no = pf->X1OrderID; //柜台委托号
                p->entrust_status = X1FieldConvert::EntrustStatusTrans(pf->OrderStatus);

                //返回废单是进行回滚
                if (p->entrust_status == MY_TNL_OS_ERROR)
                {
                    ComplianceCheck_OnOrderInsertFailed(
                        tunnel_info_.account.c_str(),
                        OrderRef,
                        p->exchange_code,
                        p->stock_code,
                        p->volume,
                        p->speculator,
                        p->open_close,
                        p->order_type);
                }

                // 已撤，报告合规检查
                if (p->entrust_status == MY_TNL_OS_WITHDRAWED)
                {
                    ComplianceCheck_OnOrderCanceled(
                        tunnel_info_.account.c_str(),
                        pf->InstrumentID,
                        OrderRef,
                        p->exchange_code,
                        pf->CancelAmount,
                        p->speculator,
                        p->open_close);
                }

                // 全成，报告合规检查
                if (p->entrust_status == MY_TNL_OS_COMPLETED)
                {
                    ComplianceCheck_OnOrderFilled(
                        tunnel_info_.account.c_str(),
                        OrderRef);
                }
#ifdef PROBE_COST
    TIME_PROBER_END(order_return_count*10+3);
    sprintf(tag_name, "Handler%d", order_return_count - 5000);
    TIME_PROBER_SET_TAG_NAME(order_return_count*10 + 4, tag_name);
    TIME_PROBER_BEGIN(order_return_count*10 + 4);
#endif
                // 委托回报
                T_OrderReturn order_return;
                X1Packer::OrderReturn(pf, p, order_return);
                OrderReturn_call_back_handler_(&order_return);
#ifdef PROBE_COST
                TIME_PROBER_END(order_return_count*10+4);
                sprintf(tag_name, "LogUtil%d", order_return_count - 5000);
                TIME_PROBER_SET_TAG_NAME(order_return_count*10 + 5, tag_name);
                TIME_PROBER_BEGIN(order_return_count*10 + 5);
#endif
                LogUtil::OnOrderReturn(&order_return, tunnel_info_);
            }
            else
            {
                TNL_LOG_INFO("can't get original place order info of order ref: %lld", OrderRef);
            }
        }
#ifdef PROBE_COST
    TIME_PROBER_END(order_return_count*10+5);
#endif
    }
    catch (...)
    {
        TNL_LOG_ERROR("unknown exception in OnRtnOrder.");
    }

#ifdef PROBE_COST
    TIME_PROBER_END(order_return_count*10);
    order_return_count++;
#endif

}

void MYX1Spi::OnRtnCancelOrder(struct CX1FtdcRspPriCancelOrderField* pf)
{
    TNL_LOG_DEBUG("OnRtnCancelOrder:  \n%s", X1DatatypeFormater::ToString(pf).c_str());
    try
    {
        if (pf && pf->SessionID == session_id_)
        {
            OrderRefDataType OrderRef = pf->LocalOrderID;
            const X1OrderInfo *p = xspeed_trade_context_.GetOrderInfoByLocalID(OrderRef);
            if (p)
            {
                p->entrust_status = X1FieldConvert::EntrustStatusTrans(pf->OrderStatus);

                // 已撤，报告合规检查
                if (p->entrust_status == MY_TNL_OS_WITHDRAWED)
                {
                    ComplianceCheck_OnOrderCanceled(
                        tunnel_info_.account.c_str(),
                        pf->InstrumentID,
                        OrderRef,
                        p->exchange_code,
                        pf->CancelAmount,
                        p->speculator,
                        p->open_close);
                }

                // 委托回报
                T_OrderReturn order_return;
                X1Packer::OrderReturn(pf, p, order_return);
                OrderReturn_call_back_handler_(&order_return);
                LogUtil::OnOrderReturn(&order_return, tunnel_info_);
            }
            else
            {
                TNL_LOG_INFO("can't get original place order info of order ref: %lld", OrderRef);
            }
        }
    }
    catch (...)
    {
        TNL_LOG_ERROR("unknown exception in OnRtnCancelOrder.");
    }
}

void MYX1Spi::OnRspQryOrderInfo(struct CX1FtdcRspOrderField* pf, struct CX1FtdcRspErrorField* pe, bool bIsLast)
{
    TNL_LOG_DEBUG("OnRspQryOrderInfo: bIsLast=%d \n%s \n%s", bIsLast,
        X1DatatypeFormater::ToString(pf).c_str(),
        X1DatatypeFormater::ToString(pe).c_str());

    gettimeofday(&timer_start_, NULL);

    if (cfg_.Compliance_check_param().init_cancel_times_at_start == 1)
    {
        CalcCancelTimes(pf, pe, bIsLast);
    }

    T_OrderDetailReturn ret;

    // respond error
    if (pe && pe->ErrorID != 0)
    {
        ReportErrorState(pe->ErrorID, pe->ErrorMsg);
        query_info_.query_order_status = QueryStatus::QUERY_ERROR;
        return;
    }

    if (pf && pf->OrderAmount > 0)
    {
        OrderDetail od;
        strncpy(od.stock_code, pf->InstrumentID, sizeof(od.stock_code));
		// TODO: wangying, fix the bug of cancel order function:inErrorID=114,x1 order id invalid
        od.entrust_no = pf->X1OrderID;

        od.order_kind = MY_TNL_OPT_LIMIT_PRICE;
        od.direction = X1FieldConvert::GetMYBuySell(pf->BuySellType);
        od.open_close = X1FieldConvert::GetMYOpenClose(pf->OpenClose);
        od.speculator = X1FieldConvert::EntrustTbFlagTrans(pf->Speculator);
        od.entrust_status = X1FieldConvert::EntrustStatusTrans(pf->OrderStatus);
        od.limit_price = pf->InsertPrice;
        od.volume = pf->OrderAmount;
        od.volume_traded = pf->MatchedAmount;
        od.volume_remain = pf->OrderAmount - pf->MatchedAmount;
        query_info_.order_return.datas.push_back(od);
    }

    if (bIsLast)
    {
        query_info_.query_order_status = QueryStatus::QUERY_SUCCESS;
    }
}

void MYX1Spi::CalcCancelTimes(const struct CX1FtdcRspOrderField* const pf,
    const struct CX1FtdcRspErrorField* const pe, const bool bIsLast)
{
    if (pf && (X1_FTDC_SPD_CANCELED == pf->OrderStatus       ///全部撤单
    || X1_FTDC_SPD_PARTIAL_CANCELED == pf->OrderStatus   ///部成部撤
    || X1_FTDC_SPD_IN_CANCELING == pf->OrderStatus       ///撤单中
    )) 
    {
        std::lock_guard<std::mutex> lock(cancel_times_sync_);
        std::map<std::string, int>::iterator it = cancel_times_of_contract.find(pf->InstrumentID);
        if (it == cancel_times_of_contract.end())
        {
            cancel_times_of_contract.insert(std::make_pair(pf->InstrumentID, 1));
            TNL_LOG_DEBUG("CalcCancelTimes. [%s-%s]: 1. OrderSysID:%s. ",
                pf->AccountID, pf->InstrumentID, pf->OrderSysID);
        }
        else
        {
            ++it->second;
            TNL_LOG_DEBUG("CalcCancelTimes. [%s-%s]: %d. OrderSysID:%s. ",
                pf->AccountID, pf->InstrumentID, it->second, pf->OrderSysID);
        }
    }

    if (bIsLast)
    {
        {
            std::lock_guard<std::mutex> lock(cancel_times_sync_);
            for (std::map<std::string, int>::iterator it = cancel_times_of_contract.begin();
                it != cancel_times_of_contract.end(); ++it)
            {
                ComplianceCheck_SetCancelTimes(tunnel_info_.account.c_str(), it->first.c_str(), it->second);
            }
            cancel_times_of_contract.clear();
        }

        is_ready_ = true;

    }
}

void MYX1Spi::OnRspQryMatchInfo(struct CX1FtdcRspMatchField* pf, struct CX1FtdcRspErrorField* pe, bool bIsLast)
{
    TNL_LOG_DEBUG("OnRspQryMatchInfo: bIsLast=%d \n%s \n%s", bIsLast,
        X1DatatypeFormater::ToString(pf).c_str(),
        X1DatatypeFormater::ToString(pe).c_str());

    T_TradeDetailReturn ret;

    // respond error
    if (pe && pe->ErrorID != 0)
    {
        ReportErrorState(pe->ErrorID, pe->ErrorMsg);
        query_info_.query_trade_status = QueryStatus::QUERY_ERROR;

        return;
    }

    if (pf && pf->MatchedAmount > 0)
    {
        TradeDetail td;
        strncpy(td.stock_code, pf->InstrumentID, sizeof(td.stock_code));

		// TODO: wangying, fix the bug of cancel order function:inErrorID=114,x1 order id invalid
        td.entrust_no = pf->X1OrderID;

        td.direction = X1FieldConvert::GetMYBuySell(pf->BuySellType);
        td.open_close = X1FieldConvert::GetMYOpenClose(pf->OpenClose);
        td.speculator = X1FieldConvert::EntrustTbFlagTrans(pf->Speculator);
        td.trade_price = pf->MatchedPrice;
        td.trade_volume = pf->MatchedAmount;
        strncpy(td.trade_time, pf->MatchedTime, sizeof(TX1FtdcDateType));
        query_info_.trade_return.datas.push_back(td);
    }

    if (bIsLast)
    {
        query_info_.query_trade_status = QueryStatus::QUERY_SUCCESS;
    }
}

// commented by wangying for there is NOT OnRspTradingDay in X1 API
//void MYX1Spi::OnRspTradingDay(struct DFITCTradingDayRtnField* pf)
//{
//    TNL_LOG_INFO("OnRspTradingDay:  \n%s", XSpeedDatatypeFormater::ToString(pf).c_str());
//    if (pf) memcpy(trade_day, pf->date, sizeof(trade_day));
//}

void MYX1Spi::OnRtnExchangeStatus(struct CX1FtdcExchangeStatusRtnField* pf)
{
    TNL_LOG_DEBUG("OnRtnExchangeStatus:  \n%s", X1DatatypeFormater::ToString(pf).c_str());
}

//如果已经存储昨仓数据，直接从文件中读取，否则，存储到文件中
void MYX1Spi::CheckAndSaveYestodayPosition()
{
    // calc avg price
    for (PositionDetail &v : query_info_.position_return.datas)
    {
        if (v.position > 0) v.position_avg_price = v.position_avg_price / v.position;
        if (v.yestoday_position > 0) v.yd_position_avg_price = v.yd_position_avg_price / v.yestoday_position;
    }

    // 存储文件名：yd_pos_账号_日期.rec
    std::string yd_pos_file_name = "yd_pos_" + tunnel_info_.account + "_" + std::string(trade_day) + ".rec";
    int ret = access(yd_pos_file_name.c_str(), F_OK);

    bool file_exist = (ret == 0);

    if (file_exist)
    {
        LoadYestodayPositionFromFile(yd_pos_file_name);
    }
    else
    {
        SaveYestodayPositionToFile(yd_pos_file_name);
    }
}

void MYX1Spi::LoadYestodayPositionFromFile(const std::string &file)
{
    std::ifstream yd_pos_fs(file.c_str());
    if (!yd_pos_fs)
    {
        TNL_LOG_ERROR("open file failed when LoadYestodayPositionFromFile");
        return;
    }
    char r[255];
    while (yd_pos_fs.getline(r, 255))
    {
        std::string one_line(r);
        std::vector<std::string> fields;
        my_cmn::MYStringSplit(one_line.c_str(), fields, ',');

        // 格式：合约，方向，昨仓量，开仓均价
        if (fields.size() != 4)
        {
            TNL_LOG_ERROR("yestoday position data in wrong format, line: %s, file: %s", one_line.c_str(), file.c_str());
            continue;
        }
        PositionDetail pos;
        memset(&pos, 0, sizeof(pos));
        strncpy(pos.stock_code, fields[0].c_str(), sizeof(pos.stock_code));
        pos.direction = fields[1][0];
        pos.yestoday_position = atoi(fields[2].c_str());
        pos.yd_position_avg_price = atof(fields[3].c_str());

        std::vector<PositionDetail>::iterator it = std::find_if(query_info_.position_return.datas.begin(), query_info_.position_return.datas.end(), PositionDetailPred(pos));
        if (it == query_info_.position_return.datas.end())
        {
            query_info_.position_return.datas.push_back(pos);
        }
        else
        {
            it->yestoday_position = pos.yestoday_position;
            it->yd_position_avg_price = pos.yd_position_avg_price;
        }
    }
}

void MYX1Spi::SaveYestodayPositionToFile(const std::string &file)
{
    std::ofstream yd_pos_fs(file.c_str());
    if (!yd_pos_fs)
    {
        TNL_LOG_ERROR("open file failed when SaveYestodayPositionToFile");
        return;
    }

    for (PositionDetail v : query_info_.position_return.datas)
    {
        if (v.yestoday_position > 0)
        {
            // 格式：合约，方向，昨仓量，开仓均价
            yd_pos_fs << v.stock_code << ","
                << v.direction << ","
                << v.yestoday_position << ","
                << fixed << setprecision(16) << v.yd_position_avg_price << std::endl;
        }
    }
}
