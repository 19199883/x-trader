
// TODO: here
// 报单拒绝
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
