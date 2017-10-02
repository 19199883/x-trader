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

static std::string ReadAuthCode()
{
    char l[1024];
    std::string auth_code;
    ifstream auth_cfg("auth_code.dat");
    while (auth_cfg.getline(l, 1023)) {
        auth_code.append(l);
    }

    return auth_code;
}

MYEsunnyTradeSpi::MYEsunnyTradeSpi(const TunnelConfigData &cfg)
    : cfg_(cfg)
{
    logoned_ = false;
    in_init_state_ = true;

    // whether it is need to support cancel all order when init
    have_handled_unterminated_orders_ = true;
    if (cfg_.Initial_policy_param().cancel_orders_at_start) {
        have_handled_unterminated_orders_ = false;
    }

    // check api version
    TNL_LOG_INFO("TapTradeAPIVersion: %s", GetTapTradeAPIVersion());

    // 从配置解析参数
    if (!ParseConfig()) {
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
    if (!api_ || result != TAPIERROR_SUCCEED) {
        TNL_LOG_WARN("CreateTapTradeAPI result: %d", result);
        return;
    }
    api_->SetAPINotify(this);

    // set front address (only support 1 front address)
    for (const std::string &v : cfg.Logon_config().front_addrs) {
        IPAndPortNum ip_port = ParseIPAndPortNum(v);
        api_->SetHostAddress(ip_port.first.c_str(), ip_port.second);
        TNL_LOG_INFO("SetHostAddress, addr: %s:%d", ip_port.first.c_str(), ip_port.second);
        break;
    }
    if (cfg.Logon_config().front_addrs.size() > 1) {
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
    if (TAPIERROR_SUCCEED != result) {
        TNL_LOG_ERROR("Login Error, result: %d", result);
    }
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

void MYEsunnyTradeSpi::OnConnect()
{
    TNL_LOG_INFO("OnConnect");
}

void MYEsunnyTradeSpi::OnRspLogin(TAPIINT32 errorCode, const TapAPITradeLoginRspInfo* loginRspInfo)
{
    TNL_LOG_INFO("OnRspLogin: errorCode: %d, \n%s", errorCode, ESUNNYDatatypeFormater::ToString(loginRspInfo).c_str());

    if (errorCode == TAPIERROR_SUCCEED) {
        logoned_ = true;
        in_init_state_ = false;
    }
}

void MYEsunnyTradeSpi::OnAPIReady()
{
    TNL_LOG_INFO("OnAPIReady");

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

void MYEsunnyTradeSpi::OnRtnFund(const TapAPIFundData* info) { }

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

// reserve
void MYEsunnyTradeSpi::OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
    const TapAPITradeContractInfo* info)
{
    TNL_LOG_INFO("OnRspQryContract: sessionID:%u, errorCode: %d, isLast: %c, \n%s",
        sessionID, errorCode, isLast, ESUNNYDatatypeFormater::ToString(info).c_str());

    if (errorCode == TAPIERROR_SUCCEED) {
        if (info) {
            ESUNNYFieldConvert::AddContractInfo(*info);
        }

        if (isLast == APIYNFLAG_YES) { // start thread for cancel unterminated orders }
    }
}

void MYEsunnyTradeSpi::OnRtnContract(const TapAPITradeContractInfo* info)
{
    TNL_LOG_INFO("OnRtnContract: \n%s", ESUNNYDatatypeFormater::ToString(info).c_str());
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

