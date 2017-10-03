#include "esunny_trade_interface.h"
#include <stdlib.h>
#include <iomanip>
#include <fstream>
#include <thread>
#include <algorithm>
#include "my_protocol_packager.h"
#include "check_schedule.h"
#include "esunny_data_formater.h"
#include "TapAPIError.h"

using namespace std;

MYEsunnyTradeSpi::MYEsunnyTradeSpi(const TunnelConfigData &cfg)
    : cfg_(cfg)
{
}

MYEsunnyTradeSpi::~MYEsunnyTradeSpi(void)
{
    if (api_) {
        api_->Disconnect();
        api_ = NULL;
    }
}

bool MYEsunnyTradeSpi::ParseConfig()
{
    // 用户密码
    tunnel_info_.account = cfg_.Logon_config().investorid;
    pswd_ = cfg_.Logon_config().password;
    return true;
}


void MYEsunnyTradeSpi::OnRtnOrder(const TapAPIOrderInfoNotice* info)
{
    TNL_LOG_DEBUG("OnRtnOrder: \n%s", ESUNNYDatatypeFormater::ToString(info).c_str());

    // check whether placed in this session
    if (info == NULL || info->SessionID == 0 || info->OrderInfo == NULL) {
        return;
    }

    if (!p) {
        TNL_LOG_INFO("can't get original place order info of SessionID: %u", info->SessionID);
        return;
    }
    bool should_rsp = true;
    if (p->IsTerminated()) {
        should_rsp = false;
        TNL_LOG_ERROR("SessionID: %u. OnRtnOrder after order terminated.", info->SessionID);
    }

    if (info->ErrorCode != TAPIERROR_SUCCEED) {
        int standard_error_no = cfg_.GetStandardErrorNo(info->ErrorCode);
        // 报单失败，报告合规检查
        T_OrderRespond rsp;
        ESUNNYPacker::OrderRespond(standard_error_no, p->po.serial_no, 0, MY_TNL_OS_ERROR, rsp);

        // 应答
        if (should_rsp && OrderRespond_call_back_handler_) OrderRespond_call_back_handler_(&rsp);
        LogUtil::OnOrderRespond(&rsp, tunnel_info_);
        return;
    }

    if (info->OrderInfo->OrderSystemNo[0] != '\0') {
        p->entrust_no = BuildEntrustNo(info->OrderInfo->OrderSystemNo);
    }

    //如果新报文转换成内部状态为已撤，并且该单状态已经为已撤时，消息不推送到交易服务
    char inner_status = ESUNNYFieldConvert::GetMYEntrustStatus(info->OrderInfo->OrderState);
    if ((inner_status == MY_TNL_OS_WITHDRAWED) && (inner_status == p->entrust_status)) {
        TNL_LOG_INFO("Order canceled. SessionID:%u, OrderNo:%s, entrust_no=%lld",
            info->SessionID, info->OrderInfo->OrderNo, p->entrust_no);
        return;
    }

    if (info->OrderInfo->OrderState != TAPI_ORDER_STATE_CANCELING) {
        p->entrust_status = inner_status;
    }

    p->volume_remain = p->po.volume - info->OrderInfo->OrderMatchQty;

    if (p->entrust_status == MY_TNL_OS_COMPLETED) {
        // 全成，报告合规检查
    }
    else if (p->entrust_status == MY_TNL_OS_ERROR) {
        // 报单失败，报告合规检查
    } else if (p->entrust_status == MY_TNL_OS_WITHDRAWED)
    {
    }

    if (info->OrderInfo->OrderState == TAPI_ORDER_STATE_ACCEPT) {
        // 应答
        T_OrderRespond rsp;
        ESUNNYPacker::OrderRespond(0, p->po.serial_no, 0, p->entrust_status, rsp);
        if (should_rsp && OrderRespond_call_back_handler_) OrderRespond_call_back_handler_(&rsp);
        LogUtil::OnOrderRespond(&rsp, tunnel_info_);

    } else {
        // 委托回报
        T_OrderReturn order_return;
        ESUNNYPacker::OrderReturn(info->OrderInfo, p, order_return);
        if (should_rsp && OrderReturn_call_back_handler_) OrderReturn_call_back_handler_(&order_return);
        LogUtil::OnOrderReturn(&order_return, tunnel_info_);
    }
}

// 该接口目前没有用到，所有操作结果通过OnRtnOrder返回.
// log this info only to see whether this method will be invoked.
void MYEsunnyTradeSpi::OnRspOrderAction(TAPIUINT32 sessionID, TAPIUINT32 errorCode, const TapAPIOrderActionRsp* info)
{
    TNL_LOG_INFO("OnRspOrderAction: sessionID:%u, errorCode: %d, \n%s",
        sessionID, errorCode, ESUNNYDatatypeFormater::ToString(info).c_str());
}

void MYEsunnyTradeSpi::OnRspQryOrder(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo* info)
{
    TNL_LOG_INFO("OnRspQryOrder: sessionID:%u, errorCode: %d, isLast: %c, \n%s",
        sessionID, errorCode, isLast, ESUNNYDatatypeFormater::ToString(info).c_str());
}

void MYEsunnyTradeSpi::OnRspQryOrderProcess(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
    const TapAPIOrderInfo* info)
{
    TNL_LOG_INFO("OnRspQryOrderProcess: sessionID:%u, errorCode: %d, isLast: %c, \n%s",
        sessionID, errorCode, isLast, ESUNNYDatatypeFormater::ToString(info).c_str());
}

void MYEsunnyTradeSpi::OnRspQryFill(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFillInfo* info) { }

// discard this info
void MYEsunnyTradeSpi::OnRtnFill(const TapAPIFillInfo* info) { }

void MYEsunnyTradeSpi::OnRspQryPosition(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
    const TapAPIPositionInfo* info) { }

void MYEsunnyTradeSpi::OnRtnPosition(const TapAPIPositionInfo* info) { }

void MYEsunnyTradeSpi::OnRspQryClose(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICloseInfo* info) { }

void MYEsunnyTradeSpi::OnRtnClose(const TapAPICloseInfo* info) { }

void MYEsunnyTradeSpi::OnRtnPositionProfit(const TapAPIPositionProfitNotice* info) { }

void MYEsunnyTradeSpi::OnRspQryDeepQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
    const TapAPIDeepQuoteQryRsp* info) { }

void MYEsunnyTradeSpi::OnRspQryExchangeStateInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIExchangeStateInfo* info) {
}

void MYEsunnyTradeSpi::OnRtnExchangeStateInfo(const TapAPIExchangeStateInfoNotice* info)
{
    TNL_LOG_INFO("OnRtnExchangeStateInfo: \n%s", ESUNNYDatatypeFormater::ToString(info).c_str());
}

void MYEsunnyTradeSpi::ReqLogin() { }

bool MYEsunnyTradeSpi::IsOrderTerminate(const TapAPIOrderInfo& order_field)
{
    return order_field.OrderState == TAPI_ORDER_STATE_FINISHED
        || order_field.OrderState == TAPI_ORDER_STATE_CANCELED
        || order_field.OrderState == TAPI_ORDER_STATE_LEFTDELETED
        || order_field.OrderState == TAPI_ORDER_STATE_FAIL
        || order_field.OrderState == TAPI_ORDER_STATE_DELETED
        || order_field.OrderState == TAPI_ORDER_STATE_DELETEDFOREXPIRE;
}

