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

/*
 #define DEFAULT_AUTHCODE    ("A54F72F7A15C5A1EDFDB924B29CB7DF1946CED32E26C1EAC946CED\
32E26C1EAC946CED32E26C1EAC946CED32E26C1EAC09E90175BD6CC3F288F89D1100BD2D53946CED32E26\
C1EAC63FF7F329FA574E7946CED32E26C1EAC946CED32E26C1EAC733827B0CE85386973BB72B05484FF79\
48F6CF64F96DC561D003BD55352B6B018BBEC4C806CF6824BB6AD6F62253AE4FAD6AB622B0C0104031A7F\
626700CB34F732DAF7623AEEF8CF25A03337613001FC988A488D79115348ED2C59BF83183C664BDAB338C\
C4E3B28684E46D5842053F1BD2FAE78FCCE442EEB0786AD751D37E7AC15727B30815648A850F4110CC13C\
07F1F97C0A676E00D09FE0CA2808949DB")
 */

static EntrustNoType BuildEntrustNo(const char *sys_no)
{
    // exceed range of int64, remove 0 at middle ( 20150403000000068886 )
    EntrustNoType en = atol(sys_no + 8);

    // get head of entrust no
    char buf[9];
    memcpy(buf, sys_no, 8);
    buf[8] = '\0';
    en += (atol(buf) * 100000000);

    return en;
}

static std::string ReadAuthCode()
{
    char l[1024];
    std::string auth_code;
    ifstream auth_cfg("auth_code.dat");
    while (auth_cfg.getline(l, 1023))
    {
        auth_code.append(l);
    }

    return auth_code;
}

void MYEsunnyTradeSpi::ReportErrorState(int api_error_no, const std::string &error_msg)
{
    if (api_error_no == 0)
    {
        return;
    }
    if (!cfg_.IsKnownErrorNo(api_error_no))
    {
    }
}

MYEsunnyTradeSpi::MYEsunnyTradeSpi(const TunnelConfigData &cfg)
    : cfg_(cfg)
{
    logoned_ = false;
    in_init_state_ = true;

    // whether it is need to support cancel all order when init
    have_handled_unterminated_orders_ = true;
    if (cfg_.Initial_policy_param().cancel_orders_at_start)
    {
        have_handled_unterminated_orders_ = false;
    }
    finish_query_canceltimes_ = false;
    if (cfg_.Compliance_check_param().init_cancel_times_at_start == 0)
    {
        finish_query_canceltimes_ = true;
    }

    // check api version
    TNL_LOG_INFO("TapTradeAPIVersion: %s", GetTapTradeAPIVersion());

    // 从配置解析参数
    if (!ParseConfig())
    {
        return;
    }

    char qtm_tmp_name[32];
    memset(qtm_tmp_name, 0, sizeof(qtm_tmp_name));
    sprintf(qtm_tmp_name, "esunny_%s_%u", tunnel_info_.account.c_str(), getpid());
    tunnel_info_.qtm_name = qtm_tmp_name;

    // create
    TapAPIApplicationInfo auth_info;
    std::string auth_code = ReadAuthCode();
    TNL_LOG_INFO("AuthCode: %s", auth_code.c_str());
    strncpy(auth_info.AuthCode, auth_code.c_str(), sizeof(auth_info.AuthCode));
    strcpy(auth_info.KeyOperationLogPath, "");
    TAPIINT32 result;
    api_ = CreateTapTradeAPI(&auth_info, result);
    if (!api_ || result != TAPIERROR_SUCCEED)
    {
        TNL_LOG_WARN("CreateTapTradeAPI result: %d", result);
        return;
    }
    api_->SetAPINotify(this);

    // set front address (only support 1 front address)
    for (const std::string &v : cfg.Logon_config().front_addrs)
    {
        IPAndPortNum ip_port = ParseIPAndPortNum(v);
        api_->SetHostAddress(ip_port.first.c_str(), ip_port.second);
        TNL_LOG_INFO("SetHostAddress, addr: %s:%d", ip_port.first.c_str(), ip_port.second);
        break;
    }
    if (cfg.Logon_config().front_addrs.size() > 1)
    {
        TNL_LOG_WARN("there are %d front address in configure file, only support 1.", cfg.Logon_config().front_addrs.size());
    }

    //登录服务器
    TapAPITradeLoginAuth stLoginAuth;
    memset(&stLoginAuth, 0, sizeof(stLoginAuth));
    strcpy(stLoginAuth.UserNo, tunnel_info_.account.c_str());
    strcpy(stLoginAuth.Password, pswd_.c_str());
    stLoginAuth.ISModifyPassword = APIYNFLAG_NO;
    stLoginAuth.ISDDA = APIYNFLAG_NO;

    result = api_->Login(&stLoginAuth);
    if (TAPIERROR_SUCCEED != result)
    {
        TNL_LOG_ERROR("Login Error, result: %d", result);
    }
}

MYEsunnyTradeSpi::~MYEsunnyTradeSpi(void)
{
    if (api_)
    {
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

void MYEsunnyTradeSpi::OnConnect()
{
    TNL_LOG_INFO("OnConnect");
}

void MYEsunnyTradeSpi::OnRspLogin(TAPIINT32 errorCode, const TapAPITradeLoginRspInfo* loginRspInfo)
{
    TNL_LOG_INFO("OnRspLogin: errorCode: %d, \n%s", errorCode, ESUNNYDatatypeFormater::ToString(loginRspInfo).c_str());

    if (errorCode == TAPIERROR_SUCCEED)
    {
        logoned_ = true;
        in_init_state_ = false;
    }
}

void MYEsunnyTradeSpi::OnAPIReady()
{
    TNL_LOG_INFO("OnAPIReady");

    // query contract info
    TAPIUINT32 session_id;
    api_->QryContract(&session_id, NULL);
}

void MYEsunnyTradeSpi::OnDisconnect(TAPIINT32 reasonCode)
{
    TNL_LOG_ERROR("OnDisconnect, reasonCode:%d", reasonCode);
    logoned_ = false;
}

void MYEsunnyTradeSpi::OnRspChangePassword(TAPIUINT32 sessionID, TAPIINT32 errorCode)
{
    TNL_LOG_INFO("OnRspChangePassword: sessionID:%u, errorCode: %d", sessionID, errorCode);
}

void MYEsunnyTradeSpi::OnRspSetReservedInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode, const TAPISTR_50 info)
{
    TNL_LOG_INFO("OnRspSetReservedInfo: sessionID:%u, errorCode: %d, %s", sessionID, errorCode, info);
}

void MYEsunnyTradeSpi::OnRspQryAccount(TAPIUINT32 sessionID, TAPIUINT32 errorCode, TAPIYNFLAG isLast, const TapAPIAccountInfo* info)
{
    TNL_LOG_INFO("OnRspQryAccount: sessionID:%u, errorCode: %d, isLast: %c, \n%s",
        sessionID, errorCode, isLast, ESUNNYDatatypeFormater::ToString(info).c_str());
}

void MYEsunnyTradeSpi::OnRspQryFund(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFundData* info)
{
    TNL_LOG_INFO("OnRspQryFund: sessionID:%u, errorCode: %d, isLast: %c, \n%s",
        sessionID, errorCode, isLast, ESUNNYDatatypeFormater::ToString(info).c_str());
}

void MYEsunnyTradeSpi::OnRtnFund(const TapAPIFundData* info)
{
    //TNL_LOG_INFO("OnRtnFund: \n%s", ESUNNYDatatypeFormater::ToString(info).c_str());
}

void MYEsunnyTradeSpi::OnRspQryExchange(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
    const TapAPIExchangeInfo* info)
{
    TNL_LOG_INFO("OnRspQryExchange: sessionID:%u, errorCode: %d, isLast: %c, \n%s",
        sessionID, errorCode, isLast, ESUNNYDatatypeFormater::ToString(info).c_str());
}

void MYEsunnyTradeSpi::OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
    const TapAPICommodityInfo* info)
{
    TNL_LOG_INFO("OnRspQryCommodity: sessionID:%u, errorCode: %d, isLast: %c, \n%s",
        sessionID, errorCode, isLast, ESUNNYDatatypeFormater::ToString(info).c_str());
}

void MYEsunnyTradeSpi::OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
    const TapAPITradeContractInfo* info)
{
    TNL_LOG_INFO("OnRspQryContract: sessionID:%u, errorCode: %d, isLast: %c, \n%s",
        sessionID, errorCode, isLast, ESUNNYDatatypeFormater::ToString(info).c_str());

    if (errorCode == TAPIERROR_SUCCEED)
    {
        if (info)
        {
            ESUNNYFieldConvert::AddContractInfo(*info);
        }

        if (isLast == APIYNFLAG_YES)
        {
            // start thread for cancel unterminated orders
            if (!HaveFinishQueryOrders())
            {

                std::thread qry_order(&MYEsunnyTradeSpi::QueryAndHandleOrders, this);
                qry_order.detach();
            }
            else
            {
            }
        }
    }
}

void MYEsunnyTradeSpi::OnRtnContract(const TapAPITradeContractInfo* info)
{
    TNL_LOG_INFO("OnRtnContract: \n%s", ESUNNYDatatypeFormater::ToString(info).c_str());
}

void MYEsunnyTradeSpi::OnRtnOrder(const TapAPIOrderInfoNotice* info)
{
    TNL_LOG_DEBUG("OnRtnOrder: \n%s", ESUNNYDatatypeFormater::ToString(info).c_str());

    if (!HaveFinishQueryOrders())
    {
        TNL_LOG_WARN("OnRtnOrder when tunnel not ready.");
        return;
    }

    // check whether placed in this session
    if (info == NULL || info->SessionID == 0 || info->OrderInfo == NULL)
    {
        return;
    }

    if (info->OrderInfo->ErrorCode != TAPIERROR_SUCCEED)
    {
        ReportErrorState(info->OrderInfo->ErrorCode, info->OrderInfo->ErrorText);
    }

    // get original place order info
    const EsunnyOrderInfo *p = esunny_trade_context_.UpdateOrderNoAndGetOrderInfo(info->SessionID, info->OrderInfo->OrderNo,
        info->OrderInfo->ServerFlag);
    if (!p)
    {
        TNL_LOG_INFO("can't get original place order info of SessionID: %u", info->SessionID);
        return;
    }
    bool should_rsp = true;
    if (p->IsTerminated())
    {
        should_rsp = false;
        TNL_LOG_ERROR("SessionID: %u. OnRtnOrder after order terminated.", info->SessionID);
    }

    // order refuse
    if (info->ErrorCode != TAPIERROR_SUCCEED)
    {
        int standard_error_no = cfg_.GetStandardErrorNo(info->ErrorCode);
        // 报单失败，报告合规检查
        ComplianceCheck_OnOrderInsertFailed(
            tunnel_info_.account.c_str(),
            p->po.serial_no,
            MY_TNL_EC_CZCE,
            p->po.stock_code,
            p->po.volume,
            p->po.speculator,
            p->po.open_close,
            p->po.order_type);

        T_OrderRespond rsp;
        ESUNNYPacker::OrderRespond(standard_error_no, p->po.serial_no, 0, MY_TNL_OS_ERROR, rsp);

        // 应答
        if (should_rsp && OrderRespond_call_back_handler_) OrderRespond_call_back_handler_(&rsp);
        LogUtil::OnOrderRespond(&rsp, tunnel_info_);
        return;
    }

    if (info->OrderInfo->OrderSystemNo[0] != '\0')
    {
        p->entrust_no = BuildEntrustNo(info->OrderInfo->OrderSystemNo);
    }

    //如果新报文转换成内部状态为已撤，并且该单状态已经为已撤时，消息不推送到交易服务
    char inner_status = ESUNNYFieldConvert::GetMYEntrustStatus(info->OrderInfo->OrderState);
    if ((inner_status == MY_TNL_OS_WITHDRAWED) && (inner_status == p->entrust_status))
    {
        TNL_LOG_INFO("Order canceled. SessionID:%u, OrderNo:%s, entrust_no=%lld",
            info->SessionID, info->OrderInfo->OrderNo, p->entrust_no);
        return;
    }

    if (info->OrderInfo->OrderState != TAPI_ORDER_STATE_CANCELING)
    {
        p->entrust_status = inner_status;
    }

    p->volume_remain = p->po.volume - info->OrderInfo->OrderMatchQty;

    if (p->entrust_status == MY_TNL_OS_COMPLETED)
    {
        // 全成，报告合规检查
        ComplianceCheck_OnOrderFilled(
            tunnel_info_.account.c_str(),
            p->po.serial_no);
    }
    else if (p->entrust_status == MY_TNL_OS_ERROR)
    {
        // 报单失败，报告合规检查
        ComplianceCheck_OnOrderInsertFailed(
            tunnel_info_.account.c_str(),
            p->po.serial_no,
            MY_TNL_EC_CZCE,
            p->po.stock_code,
            p->po.volume,
            p->po.speculator,
            p->po.open_close,
            p->po.order_type);
    }
    else if (p->entrust_status == MY_TNL_OS_WITHDRAWED)
    {
        ComplianceCheck_OnOrderCanceled(
            tunnel_info_.account.c_str(),
            p->po.stock_code,
            p->po.serial_no,
            MY_TNL_EC_CZCE,
            p->volume_remain,
            p->po.speculator,
            p->po.open_close);
    }

    if (info->OrderInfo->OrderState == TAPI_ORDER_STATE_ACCEPT)
    {
        // 应答
        T_OrderRespond rsp;
        ESUNNYPacker::OrderRespond(0, p->po.serial_no, 0, p->entrust_status, rsp);
        if (should_rsp && OrderRespond_call_back_handler_) OrderRespond_call_back_handler_(&rsp);
        LogUtil::OnOrderRespond(&rsp, tunnel_info_);

    }
    else
    {
        // 委托回报
        T_OrderReturn order_return;
        ESUNNYPacker::OrderReturn(info->OrderInfo, p, order_return);
        if (should_rsp && OrderReturn_call_back_handler_) OrderReturn_call_back_handler_(&order_return);
        LogUtil::OnOrderReturn(&order_return, tunnel_info_);
    }
}

void MYEsunnyTradeSpi::OnRspOrderAction(TAPIUINT32 sessionID, TAPIUINT32 errorCode, const TapAPIOrderActionRsp* info)
{
    TNL_LOG_INFO("OnRspOrderAction: sessionID:%u, errorCode: %d, \n%s",
        sessionID, errorCode, ESUNNYDatatypeFormater::ToString(info).c_str());
    if (sessionID == 0 || info == NULL)
    {
        return;
    }

    if (info->OrderInfo->ErrorCode != TAPIERROR_SUCCEED)
    {
        ReportErrorState(info->OrderInfo->ErrorCode, info->OrderInfo->ErrorText);
    }

    SerialNoType c_sn = esunny_trade_context_.GetSnBySessionID(sessionID);
    if (c_sn == 0)
    {
        TNL_LOG_INFO("can't get cancel serial no of SessionID: %u", sessionID);
        return;
    }
    const EsunnyOrderInfo *p = esunny_trade_context_.GetOrderInfoByOrderNo(info->OrderInfo->OrderNo);
    if (!p)
    {
        TNL_LOG_INFO("can't get original place order info of SessionID: %s", info->OrderInfo->OrderNo);
        return;
    }

    int standard_error_no = cfg_.GetStandardErrorNo(errorCode);
    T_CancelRespond rsp;
    ESUNNYPacker::CancelRespond(standard_error_no, c_sn, 0, rsp);

    if (standard_error_no != TUNNEL_ERR_CODE::RESULT_SUCCESS)
    {
        // 撤单拒绝，报告合规检查
        ComplianceCheck_OnOrderCancelFailed(
            tunnel_info_.account.c_str(),
            p->po.stock_code,
            p->po.serial_no);
    }

    // 应答
    if (CancelRespond_call_back_handler_) CancelRespond_call_back_handler_(&rsp);
    LogUtil::OnCancelRespond(&rsp, tunnel_info_);
}

void MYEsunnyTradeSpi::OnRspQryOrder(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo* info)
{
    TNL_LOG_INFO("OnRspQryOrder: sessionID:%u, errorCode: %d, isLast: %c, \n%s",
        sessionID, errorCode, isLast, ESUNNYDatatypeFormater::ToString(info).c_str());

    if (!HaveFinishQueryOrders())
    {
        if (!finish_query_canceltimes_)
        {
            std::unique_lock<std::mutex> lock(stats_canceltimes_sync_);
            if (info && info->OrderState == TAPI_ORDER_STATE_CANCELED)
            {
                std::string contract_no(info->CommodityNo);
                contract_no.append(info->ContractNo);
                std::map<std::string, int>::iterator it = cancel_times_of_contract.find(contract_no);
                if (it == cancel_times_of_contract.end())
                {
                    cancel_times_of_contract.insert(std::make_pair(contract_no, 1));
                }
                else
                {
                    ++it->second;
                }
            }

            if (isLast == APIYNFLAG_YES)
            {
                for (std::map<std::string, int>::iterator it = cancel_times_of_contract.begin();
                    it != cancel_times_of_contract.end(); ++it)
                {
                    ComplianceCheck_SetCancelTimes(tunnel_info_.account.c_str(), it->first.c_str(), it->second);
                }
                cancel_times_of_contract.clear();
                finish_query_canceltimes_ = true;
            }
        }

        if (!have_handled_unterminated_orders_)
        {
            std::unique_lock<std::mutex> lock(cancel_sync_);
            if (info && !IsOrderTerminate(*info))
            {
                unterminated_orders_.push_back(*info);
            }

            if (isLast == APIYNFLAG_YES && unterminated_orders_.empty())
            {
                have_handled_unterminated_orders_ = true;
            }

            if (isLast == APIYNFLAG_YES)
            {
                qry_order_finish_cond_.notify_one();
            }
        }

        TNL_LOG_INFO("OnRspQryOrder when cancel unterminated orders or stats cancel times, order: 0X%X, last: %c", info, isLast);
        return;
    }

    T_OrderDetailReturn ret;

    // respond error
    if (errorCode != TAPIERROR_SUCCEED)
    {
        ret.error_no = -1;
        if (QryOrderDetailReturnHandler_) QryOrderDetailReturnHandler_(&ret);
        LogUtil::OnOrderDetailRtn(&ret, tunnel_info_);
        return;
    }

    // empty Order detail
    if (!info)
    {
        ret.error_no = 0;
        if (QryOrderDetailReturnHandler_) QryOrderDetailReturnHandler_(&ret);
        LogUtil::OnOrderDetailRtn(&ret, tunnel_info_);
        return;
    }

    OrderDetail od;
    std::string contract_no(info->CommodityNo);
    contract_no.append(info->ContractNo);
    strncpy(od.stock_code, contract_no.c_str(), sizeof(od.stock_code));
    od.entrust_no = BuildEntrustNo(info->OrderSystemNo);
    ;
    od.order_kind = ESUNNYFieldConvert::GetMYOrderKind(info->TimeInForce);
    od.direction = ESUNNYFieldConvert::GetMYSide(info->OrderSide);
    od.open_close = ESUNNYFieldConvert::GetMYOCFlag(info->PositionEffect);
    od.speculator = ESUNNYFieldConvert::GetMYHedgeFlag(info->HedgeFlag);
    od.entrust_status = ESUNNYFieldConvert::GetMYEntrustStatus(info->OrderState);
    od.limit_price = info->OrderPrice;
    od.volume = info->OrderQty;
    od.volume_traded = info->OrderMatchQty;
    od.volume_remain = od.volume - od.volume_traded;
    od_buffer_.push_back(od);

    if (isLast == APIYNFLAG_YES)
    {
        ret.datas.swap(od_buffer_);
        ret.error_no = 0;
        if (QryOrderDetailReturnHandler_) QryOrderDetailReturnHandler_(&ret);
        LogUtil::OnOrderDetailRtn(&ret, tunnel_info_);
    }
}

void MYEsunnyTradeSpi::OnRspQryOrderProcess(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
    const TapAPIOrderInfo* info)
{
    TNL_LOG_INFO("OnRspQryOrderProcess: sessionID:%u, errorCode: %d, isLast: %c, \n%s",
        sessionID, errorCode, isLast, ESUNNYDatatypeFormater::ToString(info).c_str());
}

void MYEsunnyTradeSpi::OnRspQryFill(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFillInfo* info)
{
    TNL_LOG_DEBUG("OnRspQryFill: sessionID:%u, errorCode: %d, isLast: %c, \n%s",
        sessionID, errorCode, isLast, ESUNNYDatatypeFormater::ToString(info).c_str());

    T_TradeDetailReturn ret;

    // respond error
    if (errorCode != TAPIERROR_SUCCEED)
    {
        ret.error_no = -1;
        if (QryTradeDetailReturnHandler_) QryTradeDetailReturnHandler_(&ret);
        LogUtil::OnTradeDetailRtn(&ret, tunnel_info_);
        return;
    }

    // empty trade detail
    if (!info)
    {
        ret.error_no = 0;
        if (QryTradeDetailReturnHandler_) QryTradeDetailReturnHandler_(&ret);
        LogUtil::OnTradeDetailRtn(&ret, tunnel_info_);
        return;
    }

    TradeDetail td;
    std::string contract_no(info->CommodityNo);
    contract_no.append(info->ContractNo);
    strncpy(td.stock_code, contract_no.c_str(), sizeof(td.stock_code));

    // don't need get system order no for query result
    td.entrust_no = 0;

    td.direction = ESUNNYFieldConvert::GetMYSide(info->MatchSide);
    td.open_close = ESUNNYFieldConvert::GetMYOCFlag(info->PositionEffect);
    td.speculator = ESUNNYFieldConvert::GetMYHedgeFlag(info->HedgeFlag);
    td.trade_price = info->MatchPrice;
    td.trade_volume = info->MatchQty;
    strncpy(td.trade_time, info->MatchDateTime + 11, sizeof(td.trade_time)); // MatchDateTime=2015-03-26 10:30:57
    td_buffer_.push_back(td);

    if (isLast == APIYNFLAG_YES)
    {
        ret.datas.swap(td_buffer_);
        ret.error_no = 0;
        if (QryTradeDetailReturnHandler_) QryTradeDetailReturnHandler_(&ret);
        LogUtil::OnTradeDetailRtn(&ret, tunnel_info_);
    }
}

void MYEsunnyTradeSpi::OnRtnFill(const TapAPIFillInfo* info)
{
    TNL_LOG_DEBUG("OnRtnFill: \n%s", ESUNNYDatatypeFormater::ToString(info).c_str());

    if (!HaveFinishQueryOrders())
    {
        TNL_LOG_WARN("OnRtnFill when tunnel not ready.");
        return;
    }

    if (info == NULL)
    {
        TNL_LOG_WARN("OnRtnFill return null object.");
        return;
    }

    // get original place order info
    const EsunnyOrderInfo *p = esunny_trade_context_.GetOrderInfoByOrderNo(info->OrderNo);
    if (!p)
    {
        TNL_LOG_INFO("can't get place order info of OrderNo: %s, order not placed by this session", info->OrderNo);
        return;
    }
    bool should_rsp = true;
    if (p->IsTerminated())
    {
        should_rsp = false;
        TNL_LOG_ERROR("OrderNo: %s. OnRtnFill after order terminated.", info->OrderNo);
    }

    // match回报
    T_TradeReturn trade_return;
    ESUNNYPacker::TradeReturn(info, p, trade_return);
    p->volume_total_matched += info->MatchQty;
    if (should_rsp && TradeReturn_call_back_handler_) TradeReturn_call_back_handler_(&trade_return);
    LogUtil::OnTradeReturn(&trade_return, tunnel_info_);
}

namespace
{
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
}

void MYEsunnyTradeSpi::OnRspQryPosition(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
    const TapAPIPositionInfo* info)
{
    if (sessionID == 0) return;

    TNL_LOG_INFO("OnRspQryPosition: sessionID:%u, errorCode: %d, isLast: %c, \n%s",
        sessionID, errorCode, isLast, ESUNNYDatatypeFormater::ToString(info).c_str());
    T_PositionReturn ret;

    // respond error
    if (errorCode != TAPIERROR_SUCCEED)
    {
        ret.error_no = -1;
        QryPosReturnHandler_(&ret);
        LogUtil::OnPositionRtn(&ret, tunnel_info_);
        return;
    }

    // empty positioon
    if (!info)
    {
        ret.error_no = 0;
        QryPosReturnHandler_(&ret);
        LogUtil::OnPositionRtn(&ret, tunnel_info_);
        return;
    }

    PositionDetail pos;
    std::string contract_no(info->CommodityNo);
    contract_no.append(info->ContractNo);
    strncpy(pos.stock_code, contract_no.c_str(), sizeof(StockCodeType));
    pos.direction = ESUNNYFieldConvert::GetMYSide(info->MatchSide);
    pos.position = info->PositionQty;
    pos.position_avg_price = 0;
    if (pos.position > 0)
    {
        pos.position_avg_price = info->PositionPrice;
    }

    pos.yestoday_position = 0;
    pos.yd_position_avg_price = 0;
    if (info->IsHistory == APIYNFLAG_YES)
    {
        pos.yestoday_position = info->PositionQty;
        pos.yd_position_avg_price = info->PositionPrice;
    }

    if (pos.position > 0 || pos.yestoday_position > 0)
    {
        std::vector<PositionDetail>::iterator it = std::find_if(pos_buffer_.begin(), pos_buffer_.end(), PositionDetailPred(pos));
        if (it == pos_buffer_.end())
        {
            pos_buffer_.push_back(pos);
        }
        else
        {
            if (pos.position > 0)
            {
                it->position_avg_price = ((it->position_avg_price * it->position) + (pos.position_avg_price * pos.position))
                    / (it->position + pos.position);
                it->position += pos.position;
            }
            if (pos.yestoday_position > 0)
            {
                it->yd_position_avg_price = ((it->yd_position_avg_price * it->yestoday_position)
                    + (pos.yd_position_avg_price * pos.yestoday_position))
                    / (it->yestoday_position + pos.yestoday_position);
                it->yestoday_position += pos.yestoday_position;
            }
        }
    }

    if (isLast == APIYNFLAG_YES)
    {
        ret.datas.swap(pos_buffer_);
        ret.error_no = 0;
        QryPosReturnHandler_(&ret);
        LogUtil::OnPositionRtn(&ret, tunnel_info_);
    }
}

void MYEsunnyTradeSpi::OnRtnPosition(const TapAPIPositionInfo* info)
{
    //TNL_LOG_INFO("OnRtnPosition: \n%s", ESUNNYDatatypeFormater::ToString(info).c_str());
}

void MYEsunnyTradeSpi::OnRspQryClose(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICloseInfo* info)
{
    TNL_LOG_INFO("OnRspQryClose: sessionID:%u, errorCode: %d, isLast: %c, \n%s",
        sessionID, errorCode, isLast, ESUNNYDatatypeFormater::ToString(info).c_str());
}

void MYEsunnyTradeSpi::OnRtnClose(const TapAPICloseInfo* info)
{
    //TNL_LOG_INFO("OnRtnClose: \n%s", ESUNNYDatatypeFormater::ToString(info).c_str());
}

void MYEsunnyTradeSpi::OnRtnPositionProfit(const TapAPIPositionProfitNotice* info)
{
    //TNL_LOG_INFO("OnRtnPositionProfit: \n%s", ESUNNYDatatypeFormater::ToString(info).c_str());
}

void MYEsunnyTradeSpi::OnRspQryDeepQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
    const TapAPIDeepQuoteQryRsp* info)
{
    TNL_LOG_INFO("OnRspQryDeepQuote: sessionID:%u, errorCode: %d, isLast: %c, \n%s",
        sessionID, errorCode, isLast, ESUNNYDatatypeFormater::ToString(info).c_str());
}

void MYEsunnyTradeSpi::OnRspQryExchangeStateInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
    const TapAPIExchangeStateInfo* info)
{
    TNL_LOG_INFO("OnRspQryExchangeStateInfo: sessionID:%u, errorCode: %d, isLast: %c, \n%s",
        sessionID, errorCode, isLast, ESUNNYDatatypeFormater::ToString(info).c_str());
}

void MYEsunnyTradeSpi::OnRtnExchangeStateInfo(const TapAPIExchangeStateInfoNotice* info)
{
    TNL_LOG_INFO("OnRtnExchangeStateInfo: \n%s", ESUNNYDatatypeFormater::ToString(info).c_str());
}

void MYEsunnyTradeSpi::ReqLogin()
{
}

int MYEsunnyTradeSpi::ConvertErrorCode(int esunny_err_code)
{
    return esunny_err_code;
}

TapAPIOrderCancelReq MYEsunnyTradeSpi::CreatCancelParam(const TapAPIOrderInfo& o)
{
    TapAPIOrderCancelReq req_param;
    memset(&req_param, 0, sizeof(req_param));

    //req_param.RefInt = o.RefInt;                                            ///< 整型参考值
    //memcpy(req_param.RefString, o.RefString, sizeof(req_param.RefString));  ///< 字符串参考值
    req_param.ServerFlag = o.ServerFlag;                                    ///< 服务器标识
    memcpy(req_param.OrderNo, o.OrderNo, sizeof(req_param.OrderNo));        ///< 委托编码

    return req_param;
}

bool MYEsunnyTradeSpi::IsOrderTerminate(const TapAPIOrderInfo& order_field)
{
    return order_field.OrderState == TAPI_ORDER_STATE_FINISHED
        || order_field.OrderState == TAPI_ORDER_STATE_CANCELED
        || order_field.OrderState == TAPI_ORDER_STATE_LEFTDELETED
        || order_field.OrderState == TAPI_ORDER_STATE_FAIL
        || order_field.OrderState == TAPI_ORDER_STATE_DELETED
        || order_field.OrderState == TAPI_ORDER_STATE_DELETEDFOREXPIRE;
}

void MYEsunnyTradeSpi::QueryAndHandleOrders()
{
    // query order detail parameter
    TapAPIOrderQryReq qry_param;
    memset(&qry_param, 0, sizeof(TapAPIOrderQryReq));

    //超时后没有完成查询，重试。为防止委托单太多，10s都回报不了，每次超时加5s
    int wait_seconds = 10;

    std::vector<TapAPIOrderInfo> unterminated_orders_t;
    while (!HaveFinishQueryOrders())
    {
        //主动查询所有报单
        while (true)
        {
            TAPIUINT32 session_id;
			qry_param.OrderQryType = TAPI_ORDER_QRY_TYPE_ALL;
            int qry_result = api_->QryOrder(&session_id, &qry_param);
            TNL_LOG_INFO("QryOrder (for cancel unterminated orders or stats cancel times) - session_id:%d, return:%d", session_id,
                qry_result);

            if (qry_result != 0)
            {
                // retry if failed, wait some seconds
                sleep(2);
                continue;
            }

            break;
        }

        //处理未终结报单（撤未终结报单）
        if (!have_handled_unterminated_orders_)
        {
            {
                std::unique_lock<std::mutex> lock(cancel_sync_);
                qry_order_finish_cond_.wait_for(lock, std::chrono::seconds(10));
                unterminated_orders_t.swap(unterminated_orders_);
            }
            //遍历 unterminated_orders 间隔20ms（流控要求），发送撤单请求
            for (const TapAPIOrderInfo &order_field : unterminated_orders_t)
            {
                TapAPIOrderCancelReq action_field = CreatCancelParam(order_field);
                TAPIUINT32 session_id;
                TAPIINT32 ret = api_->CancelOrder(&session_id, &action_field);
                if (ret != TAPIERROR_SUCCEED)
                {
                    TNL_LOG_WARN("in CancelUnterminatedOrders, CancelOrder return %d", ret);
                }
                usleep(20 * 1000);
            }
            unterminated_orders_t.clear();

            //全部发送完毕后，等待 1s ， 判断 handle_flag , 如没有完成，则retry
            sleep(1);
        }
        else if (!finish_query_canceltimes_)
        {
            // wait order query result return back
            sleep(wait_seconds);
            wait_seconds += 5;
        }
    }
}
