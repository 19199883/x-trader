MyFemasTradeSpi::MyFemasTradeSpi(const TunnelConfigData &cfg)
    : cfg_(cfg)
{
    connected_ = false;
    logoned_ = false;
    in_init_state_ = true;
 
    // 从配置解析参数
    if (!ParseConfig())
    {
        return;
    }

    // create
    api_ = CUstpFtdcTraderApi::CreateFtdcTraderApi();
    api_->RegisterSpi(this);

    // subscribe
    api_->SubscribePrivateTopic(USTP_TERT_QUICK);
    api_->SubscribePublicTopic(USTP_TERT_QUICK);

    // set front address list
    for (const std::string &v : cfg_.Logon_config().front_addrs)
    {
        char *addr_tmp = new char[v.size() + 1];
        memcpy(addr_tmp, v.c_str(), v.size() + 1);
        api_->RegisterFront(addr_tmp);
        TNL_LOG_WARN("RegisterFront, addr: %s", addr_tmp);
        delete[] addr_tmp;
    }

    // init
    api_->Init();
}

bool MyFemasTradeSpi::ParseConfig()
{
    // 用户密码
    tunnel_info_.account = cfg_.Logon_config().investorid;
    user_ = cfg_.Logon_config().clientid;
    pswd_ = cfg_.Logon_config().password;
    return true;
}

void MyFemasTradeSpi::OnFrontConnected()
{
    TNL_LOG_WARN("OnFrontConnected.");
    connected_ = true;
    
    CUstpFtdcReqUserLoginField login_data;
    memset(&login_data, 0, sizeof(CUstpFtdcReqUserLoginField));
    strncpy(login_data.BrokerID, cfg_.Logon_config().brokerid.c_str(), sizeof(TUstpFtdcBrokerIDType));
    strncpy(login_data.UserID, user_.c_str(), sizeof(TUstpFtdcUserIDType));
    strncpy(login_data.Password, pswd_.c_str(), sizeof(TUstpFtdcPasswordType));

    // 成功登录后，断开不再重新登录
    if (in_init_state_)
    {
        api_->ReqUserLogin(&login_data, 0);
    }
}

void MyFemasTradeSpi::OnFrontDisconnected(int nReason)
{
    connected_ = false;
    logoned_ = false;    
    TNL_LOG_WARN("OnFrontDisconnected, nReason: %d", nReason);
}

void MyFemasTradeSpi::OnHeartBeatWarning(int nTimeLapse)
{
    TNL_LOG_WARN("OnHeartBeatWarning, nTimeLapse: %d", nTimeLapse);
}

void MyFemasTradeSpi::OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID,
bool bIsLast)
{
    TNL_LOG_INFO("OnRspUserLogin: requestid = %d, last_flag=%d \n%s \n%s",
        nRequestID, bIsLast,
        FEMASDatatypeFormater::ToString(pRspUserLogin).c_str(),
        FEMASDatatypeFormater::ToString(pRspInfo).c_str());

    // logon success
    if (pRspInfo == NULL || pRspInfo->ErrorID == 0)
    {       
        logoned_ = true;
        in_init_state_ = false;        
    }
    else
    {
        ReportErrorState(pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        int standard_error_no = cfg_.GetStandardErrorNo(pRspInfo->ErrorID);

        TNL_LOG_WARN("OnRspUserLogin, external errorid: %d; Internal errorid: %d",
            pRspInfo->ErrorID, standard_error_no);        
    }
}

void MyFemasTradeSpi::OnRspUserLogout(CUstpFtdcRspUserLogoutField *pRspUserLogout, CUstpFtdcRspInfoField *pRspInfo, int nRequestID,
bool bIsLast)
{   
        logoned_ = false;       
        TNL_LOG_WARN("OnRspUserLogout: requestid = %d, last_flag=%d \n%s \n%s",
            nRequestID, bIsLast,
            FEMASDatatypeFormater::ToString(pRspUserLogout).c_str(),
            FEMASDatatypeFormater::ToString(pRspInfo).c_str());    
}

void MyFemasTradeSpi::OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{    
        if (pRspInfo)
        {
            ReportErrorState(pRspInfo->ErrorID, pRspInfo->ErrorMsg);
            TNL_LOG_WARN("OnRspError: requestid = %d, last_flag=%d \n%s",
                nRequestID, bIsLast, FEMASDatatypeFormater::ToString(pRspInfo).c_str());
        }
        else
        {
            TNL_LOG_INFO("OnRspError ");
        }   
}

void MyFemasTradeSpi::OnRtnOrder(CUstpFtdcOrderField *pOrder)
{  
        const OriginalReqInfo *p = NULL;
        OrderRefDataType order_ref = atoll(pOrder->UserOrderLocalID);       
            femas_trade_context_.AddNewOrderSysID(atoll(pOrder->OrderSysID));                        
               OnRtnOrderNormal(pOrder, p, order_ref);           
            TNL_LOG_DEBUG("%s", FEMASDatatypeFormater::ToString(pOrder).c_str());         
}

void MyFemasTradeSpi::OnRtnTrade(CUstpFtdcTradeField *pTrade)
{    
        EntrustNoType sysid = atoll(pTrade->OrderSysID);
        OrderRefDataType order_ref = atoll(pTrade->UserOrderLocalID);
        const OriginalReqInfo *p = NULL;        
                T_TradeReturn trade_return;
                FEMASPacker::TradeReturn(p, pTrade, trade_return);                                                      
            TNL_LOG_DEBUG("%s", FEMASDatatypeFormater::ToString(pTrade).c_str());        
}

// 报单拒绝
void MyFemasTradeSpi::OnRspOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID,
bool bIsLast)
{
    int errorid = 0;        
        if (pRspInfo)
        {            
            errorid = pRspInfo->ErrorID;           
        }

        TNL_LOG_WARN("OnRspOrderInsert, api_err_code: %d; my_err_code: %d; OrderRef: %s",
            errorid, standard_error_no, pInputOrder->UserOrderLocalID);

        OrderRefDataType OrderRef = atoll(pInputOrder->UserOrderLocalID);              
            T_OrderRespond pOrderRespond;
            if (0 != errorid)
            {             
            }
            else
            {                
                FEMASPacker::OrderRespond(standard_error_no, p->serial_no, pInputOrder->OrderSysID, '3', pOrderRespond);                                               
            }

            TNL_LOG_DEBUG("OnRspOrderInsert: requestid = %d, last_flag=%d \n%s \n%s",
                nRequestID, bIsLast,
                FEMASDatatypeFormater::ToString(pInputOrder).c_str(),
                FEMASDatatypeFormater::ToString(pRspInfo).c_str());      
}

// 飞马平台撤单响应
void MyFemasTradeSpi::OnRspOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID,
bool bIsLast)
{
    int errorid = 0;           
        if (pRspInfo)
        {
            ReportErrorState(pRspInfo->ErrorID, pRspInfo->ErrorMsg);
            errorid = pRspInfo->ErrorID;            
        }

        TNL_LOG_WARN("OnRspOrderAction, api_err_code: %d; my_err_code: %d; OrderRef: %s",
            errorid, standard_error_no, pOrderAction->UserOrderActionLocalID);
        OrderRefDataType OrderRef = atoll(pOrderAction->UserOrderActionLocalID);        
            T_CancelRespond cancel_respond;
            if (0 != errorid)
            {
                if (femas_trade_context_.CheckAndSetHaveHandleActionErr(p->serial_no))
                {
                    FEMASPacker::CancelRespond(standard_error_no, p->serial_no, "0", cancel_respond);                                       
                }
                else
                {
                    TNL_LOG_WARN("rsp err action have handled, order sn: %ld", p->serial_no);
                }
            }
            else
            {
                FEMASPacker::CancelRespond(standard_error_no, p->serial_no, "0", cancel_respond);
            }
            TNL_LOG_DEBUG("OnRspOrderAction: requestid = %d, last_flag=%d \n%s \n%s",
                nRequestID, bIsLast,
                FEMASDatatypeFormater::ToString(pOrderAction).c_str(),
                FEMASDatatypeFormater::ToString(pRspInfo).c_str());        
}

void MyFemasTradeSpi::OnErrRtnOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo)
{
    int errorid = 0;           
        if (pRspInfo)
        {
            errorid = pRspInfo->ErrorID;
            standard_error_no = cfg_.GetStandardErrorNo(errorid);
        }
        TNL_LOG_WARN("OnErrRtnOrderInsert, api_err_code: %d; my_err_code: %d; OrderRef: %s",
            errorid, standard_error_no, pInputOrder->UserOrderLocalID);

        OrderRefDataType OrderRef = atoll(pInputOrder->UserOrderLocalID);        
            T_OrderRespond pOrderRespond;                       
                FEMASPacker::OrderRespond(standard_error_no, p->serial_no, "0", 'e', pOrderRespond);                                     
            TNL_LOG_DEBUG("OnErrRtnOrderInsert:  \n%s \n%s",
                FEMASDatatypeFormater::ToString(pInputOrder).c_str(),
                FEMASDatatypeFormater::ToString(pRspInfo).c_str());
        }
        else
        {
            TNL_LOG_INFO("%s - place by %s, not mine(%s) in OnErrRtnOrderInsert", pInputOrder->UserOrderLocalID,
                pInputOrder->UserCustom, pInputOrder->UserID);
        }    
}

void MyFemasTradeSpi::OnErrRtnOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo)
{
    int errorid = 0;               
        if (pRspInfo)
        {
            ReportErrorState(pRspInfo->ErrorID, pRspInfo->ErrorMsg);
            errorid = pRspInfo->ErrorID;           
        }
        TNL_LOG_WARN("OnErrRtnOrderAction, api_err_code: %d; my_err_code: %d; OrderRef: %s",
            errorid, standard_error_no, pOrderAction->UserOrderActionLocalID);
        OrderRefDataType OrderRef = atoll(pOrderAction->UserOrderActionLocalID);        
            T_CancelRespond cancel_respond;            
                FEMASPacker::CancelRespond(standard_error_no, p->serial_no, "0", cancel_respond);              
            TNL_LOG_DEBUG("OnErrRtnOrderAction:  \n%s \n%s",
                FEMASDatatypeFormater::ToString(pOrderAction).c_str(),
                FEMASDatatypeFormater::ToString(pRspInfo).c_str());         
}

void MyFemasTradeSpi::OnRtnOrderNormal(CUstpFtdcOrderField * pOrder, const OriginalReqInfo * p, OrderRefDataType order_ref)
{
    //PackagerHelper rsp_package(pOrder, p);
    SerialNoType serial_no = p->serial_no;  
    T_OrderReturn order_return;
    FEMASPacker::OrderReturn(serial_no, pOrder, order_return);
}

CUstpFtdcOrderActionField
MyFemasTradeSpi::CreatCancelParam(const CUstpFtdcOrderField& order_field)
{
    CUstpFtdcOrderActionField cancle_order;
    OrderRefDataType order_ref = femas_trade_context_.GetNewOrderRef();
    memset(&cancle_order, 0, sizeof(cancle_order));

    memcpy(cancle_order.ExchangeID, order_field.ExchangeID, sizeof(TUstpFtdcExchangeIDType));
    memcpy(cancle_order.OrderSysID, order_field.OrderSysID, sizeof(TUstpFtdcOrderSysIDType));
    memcpy(cancle_order.BrokerID, order_field.BrokerID, sizeof(TUstpFtdcBrokerIDType));
    memcpy(cancle_order.InvestorID, order_field.InvestorID, sizeof(TUstpFtdcInvestorIDType));
    memcpy(cancle_order.UserID, order_field.UserID, sizeof(TUstpFtdcUserIDType));
    memcpy(cancle_order.UserOrderLocalID, order_field.UserOrderLocalID, sizeof(TUstpFtdcUserOrderLocalIDType));
    snprintf(cancle_order.UserOrderActionLocalID, sizeof(cancle_order.UserOrderActionLocalID), "%lld", order_ref);

    cancle_order.ActionFlag = USTP_FTDC_AF_Delete;
    cancle_order.LimitPrice = 0;
    cancle_order.VolumeChange = 0;

    return cancle_order;
}

bool MyFemasTradeSpi::IsOrderTerminate(const CUstpFtdcOrderField& order_field)
{
    return order_field.OrderStatus == USTP_FTDC_OS_AllTraded
        || order_field.OrderStatus == USTP_FTDC_OS_Canceled;
}


void MyFemasTradeSpi::OnPackageStart(int nTopicID, int nSequenceNo)
{
    // 不能用于识别初始化时的恢复数据，每单个Rtn消息都有开始结束
    //TNL_LOG_INFO("OnPackageStart, nTopicID:%d, nSequenceNo:%d", nTopicID, nSequenceNo);
}

void MyFemasTradeSpi::OnPackageEnd(int nTopicID, int nSequenceNo)
{
    // 不能用于识别初始化时的恢复数据，每单个Rtn消息都有开始结束
    //TNL_LOG_INFO("OnPackageEnd, nTopicID:%d, nSequenceNo:%d", nTopicID, nSequenceNo);
}
