#ifndef ESUNNY_TRADE_INTERFACE_H_
#define ESUNNY_TRADE_INTERFACE_H_

#include <string>
#include <sstream>
#include <list>
#include <atomic>
#include <pthread.h>
#include <mutex>
#include <condition_variable>

#include "TapTradeAPI.h"

#include "config_data.h"
#include "trade_data_type.h"
#include "esunny_trade_context.h"
#include "my_protocol_packager.h"
#include "my_tunnel_lib.h"
#include "my_cmn_util_funcs.h"
#include "tunnel_cmn_utility.h"
#include "trade_log_util.h"
#include "field_convert.h"
#include "esunny_data_formater.h"

struct OriginalReqInfo;

class MYEsunnyTradeSpi: public ITapTradeAPINotify
{
public:
    MYEsunnyTradeSpi(const TunnelConfigData &cfg);
    virtual ~MYEsunnyTradeSpi(void);

    void SetCallbackHandler(std::function<void(const T_OrderRespond *)> handler)
    {
        OrderRespond_call_back_handler_ = handler;
    }
    void SetCallbackHandler(std::function<void(const T_CancelRespond *)> handler)
    {
        CancelRespond_call_back_handler_ = handler;
    }
    void SetCallbackHandler(std::function<void(const T_OrderReturn *)> handler)
    {
        OrderReturn_call_back_handler_ = handler;
    }
    void SetCallbackHandler(std::function<void(const T_TradeReturn *)> handler)
    {
        TradeReturn_call_back_handler_ = handler;
    }
    void SetCallbackHandler(std::function<void(const T_PositionReturn *)> handler)
    {
        QryPosReturnHandler_ = handler;
    }
    void SetCallbackHandler(std::function<void(const T_OrderDetailReturn *)> handler)
    {
        QryOrderDetailReturnHandler_ = handler;
    }
    void SetCallbackHandler(std::function<void(const T_TradeDetailReturn *)> handler)
    {
        QryTradeDetailReturnHandler_ = handler;
    }
    /**
     * @brief 连接成功回调通知
     * @ingroup G_T_Login
     */
    virtual void TAP_CDECL OnConnect();
    /**
     * @brief    系统登录过程回调。
     * @details  此函数为Login()登录函数的回调，调用Login()成功后建立了链路连接，然后API将向服务器发送登录认证信息，
     *           登录期间的数据发送情况和登录的回馈信息传递到此回调函数中。
     * @param[in] errorCode 返回错误码,0表示成功。
     * @param[in] loginRspInfo 登陆应答信息，如果errorCode!=0，则loginRspInfo=NULL。
     * @attention    该回调返回成功，说明用户登录成功。但是不代表API准备完毕。
     * @ingroup G_T_Login
     */
    virtual void TAP_CDECL OnRspLogin(TAPIINT32 errorCode, const TapAPITradeLoginRspInfo *loginRspInfo);
    /**
     * @brief    通知用户API准备就绪。
     * @details  只有用户回调收到此就绪通知时才能进行后续的各种行情数据查询操作。\n
     *           此回调函数是API能否正常工作的标志。
     * @attention 就绪后才可以进行后续正常操作
     * @ingroup G_T_Login
     */
    virtual void TAP_CDECL OnAPIReady();
    /**
     * @brief    API和服务失去连接的回调
     * @details  在API使用过程中主动或者被动与服务器服务失去连接后都会触发此回调通知用户与服务器的连接已经断开。
     * @param[in] reasonCode 断开原因代码。
     * @ingroup G_T_Disconnect
     */
    virtual void TAP_CDECL OnDisconnect(TAPIINT32 reasonCode);
    /**
     * @brief 通知用户密码修改结果
     * @param[in] sessionID 修改密码的会话ID,和ChangePassword返回的会话ID对应。
     * @param[in] errorCode 返回错误码，0表示成功。
     * @ingroup G_T_UserInfo
     */
    virtual void TAP_CDECL OnRspChangePassword(TAPIUINT32 sessionID, TAPIINT32 errorCode);
    /**
     * @brief 设置用户预留信息反馈
     * @param[in] sessionID 设置用户预留信息的会话ID
     * @param[in] errorCode 返回错误码，0表示成功。
     * @param[in] info 指向返回的信息结构体。当errorCode不为0时，info为空。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_UserInfo
     */
    virtual void TAP_CDECL OnRspSetReservedInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode, const TAPISTR_50 info);
    /**
     * @brief    返回用户信息
     * @details  此回调接口向用户返回查询的资金账号的详细信息。用户有必要将得到的账号编号保存起来，然后在后续的函数调用中使用。
     * @param[in] sessionID 请求的会话ID；
     * @param[in] errorCode 错误码。0 表示成功。
     * @param[in] isLast 标示是否是最后一批数据；
     * @param[in] info 指向返回的信息结构体。当errorCode不为0时，info为空。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_AccountInfo
     */
    virtual void TAP_CDECL OnRspQryAccount(TAPIUINT32 sessionID, TAPIUINT32 errorCode, TAPIYNFLAG isLast,
        const TapAPIAccountInfo *info);
    /**
     * @brief 返回资金账户的资金信息
     * @param[in] sessionID 请求的会话ID；
     * @param[in] errorCode 错误码。0 表示成功。
     * @param[in] isLast     标示是否是最后一批数据；
     * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_AccountDetails
     */
    virtual void TAP_CDECL OnRspQryFund(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFundData *info);
    /**
     * @brief    用户资金变化通知
     * @details  用户的委托成交后会引起资金数据的变化，因此需要向用户实时反馈。
     * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_AccountDetails
     */
    virtual void TAP_CDECL OnRtnFund(const TapAPIFundData *info);
    /**
     * @brief 返回系统中的交易所信息
     * @param[in] sessionID 请求的会话ID；
     * @param[in] errorCode 错误码。0 表示成功。
     * @param[in] isLast     标示是否是最后一批数据；
     * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_TradeSystem
     */
    virtual void TAP_CDECL OnRspQryExchange(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
        const TapAPIExchangeInfo *info);
    /**
     * @brief    返回系统中品种信息
     * @details  此回调接口用于向用户返回得到的所有品种信息。
     * @param[in] sessionID 请求的会话ID，和GetAllCommodities()函数返回对应；
     * @param[in] errorCode 错误码。0 表示成功。
     * @param[in] isLast     标示是否是最后一批数据；
     * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_Commodity
     */
    virtual void TAP_CDECL OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
        const TapAPICommodityInfo *info);
    /**
     * @brief 返回系统中合约信息
     * @param[in] sessionID 请求的会话ID；
     * @param[in] errorCode 错误码。0 表示成功。
     * @param[in] isLast     标示是否是最后一批数据；
     * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_Contract
     */
    virtual void TAP_CDECL OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
        const TapAPITradeContractInfo *info);
    /**
     * @brief    返回新增合约信息
     * @details  向用户推送新的合约。主要用来处理在交易时间段中服务器添加了新合约时，向用户发送这个合约的信息。
     * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_Contract
     */
    virtual void TAP_CDECL OnRtnContract(const TapAPITradeContractInfo *info);
    /**
     * @brief 返回新委托。新下的或者其他地方下的推送过来的。
     * @details  服务器接收到客户下的委托内容后就会保存起来等待触发，同时向用户回馈一个
     *           新委托信息说明服务器正确处理了用户的请求，返回的信息中包含了全部的委托信息，
     *           同时有一个用来标示此委托的委托号。
     * @param[in] info 指向返回的信息结构体。当errorCode不为0时，info为空。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_TradeActions
     */
    virtual void TAP_CDECL OnRtnOrder(const TapAPIOrderInfoNotice *info);
    /**
     * @brief    返回对报单的主动操作结果
     * @details  如下单，撤单，改单，挂起，激活等操作的结果。
     * @param[in] sessionID 请求的会话ID；
     * @param[in] errorCode 错误码。0 表示成功。
     * @param[in] info 报单的具体信息。当errorCode不为0时，info为空。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_TradeActions
     */
    virtual void TAP_CDECL OnRspOrderAction(TAPIUINT32 sessionID, TAPIUINT32 errorCode, const TapAPIOrderActionRsp *info);
    /**
     * @brief    返回查询的委托信息
     * @details  返回用户查询的委托的具体信息。
     * @param[in] sessionID 请求的会话ID；
     * @param[in] errorCode 错误码。0 表示成功。
     * @param[in] isLast 标示是否是最后一批数据；
     * @param[in] info 指向返回的信息结构体。当errorCode不为0时，info为空。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_TradeInfo
     */
    virtual void TAP_CDECL OnRspQryOrder(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo *info);
    /**
     * @brief 返回查询的委托变化流程信息
     * @param[in] sessionID 请求的会话ID；
     * @param[in] errorCode 错误码，当errorCode==0时，info指向返回的委托变化流程结构体，不然为NULL；
     * @param[in] isLast 标示是否是最后一批数据；
     * @param[in] info 返回的委托变化流程指针。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_TradeInfo
     */
    virtual void TAP_CDECL OnRspQryOrderProcess(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
        const TapAPIOrderInfo *info);
    /**
     * @brief 返回查询的成交信息
     * @param[in] sessionID 请求的会话ID；
     * @param[in] errorCode 错误码。0 表示成功。
     * @param[in] isLast     标示是否是最后一批数据；
     * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_TradeInfo
     */
    virtual void TAP_CDECL OnRspQryFill(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFillInfo *info);
    /**
     * @brief    推送来的成交信息
     * @details  用户的委托成交后将向用户推送成交信息。
     * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_TradeActions
     */
    virtual void TAP_CDECL OnRtnFill(const TapAPIFillInfo *info);
    /**
     * @brief 返回查询的持仓
     * @param[in] sessionID 请求的会话ID；
     * @param[in] errorCode 错误码。0 表示成功。
     * @param[in] isLast     标示是否是最后一批数据；
     * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_TradeInfo
     */
    virtual void TAP_CDECL OnRspQryPosition(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
        const TapAPIPositionInfo *info);
    /**
     * @brief 持仓变化推送通知
     * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_TradeInfo
     */
    virtual void TAP_CDECL OnRtnPosition(const TapAPIPositionInfo *info);
    /**
     * @brief 返回查询的平仓
     * @param[in] sessionID 请求的会话ID；
     * @param[in] errorCode 错误码。0 表示成功。
     * @param[in] isLast     标示是否是最后一批数据；
     * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_TradeInfo
     */
    virtual void TAP_CDECL OnRspQryClose(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICloseInfo *info);
    /**
     * @brief 平仓数据变化推送
     * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_TradeActions
     */
    virtual void TAP_CDECL OnRtnClose(const TapAPICloseInfo *info);
    /**
     * @brief 持仓盈亏通知
     * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_TradeActions
     */
    virtual void TAP_CDECL OnRtnPositionProfit(const TapAPIPositionProfitNotice *info);
    /**
     * @brief 深度行情查询应答
     * @param[in] sessionID 请求的会话ID；
     * @param[in] errorCode 错误码。0 表示成功。
     * @param[in] isLast     标示是否是最后一批数据；
     * @param[in] info     指向返回的深度行情信息结构体。当errorCode不为0时，info为空。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_DeepQuote
     */
    virtual void TAP_CDECL OnRspQryDeepQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
        const TapAPIDeepQuoteQryRsp *info);

    /**
     * @brief 交易所时间状态信息查询应答
     * @param[in] sessionID 请求的会话ID；
     * @param[in] errorCode 错误码。0 表示成功。
     * @param[in] isLast     标示是否是最后一批数据
     * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
     * @attention  不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_TradeSystem
     */
    virtual void TAP_CDECL OnRspQryExchangeStateInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
        const TapAPIExchangeStateInfo * info);
    /**
     * @brief 交易所时间状态信息通知
     * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
     * @attention  不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_T_TradeSystem
     */
    virtual void TAP_CDECL OnRtnExchangeStateInfo(const TapAPIExchangeStateInfoNotice * info);

	virtual void TAP_CDECL OnRtnReqQuoteNotice(const TapAPIReqQuoteNotice *info) {}
	virtual void TAP_CDECL OnRspUpperChannelInfo(TAPIUINT32 sessionID,TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIUpperChannelInfo * info) {}
	virtual void TAP_CDECL OnRspAccountRentInfo(TAPIUINT32 sessionID,TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIAccountRentInfo * info) {};

    bool ParseConfig();

    // 下发指令接口
    int ReqOrderInsert(const T_PlaceOrder *po)
    {
        if (!TunnelIsReady())
        {
            TNL_LOG_WARN("ReqOrderInsert when tunnel not ready");
            return TUNNEL_ERR_CODE::NO_VALID_CONNECT_AVAILABLE;
        }
        int ret = TUNNEL_ERR_CODE::RESULT_FAIL;

        try
        {
            TapAPINewOrder po_req;
            bool convert_res = ESUNNYPacker::OrderRequest(cfg_, po, po_req);
            if (!convert_res)
            {
                TNL_LOG_WARN("PlaceOrder can't find contract info to complete the order");
                return ret;
            }

            TAPIUINT32 session_id;
            {
                std::lock_guard<std::mutex> lock(api_spinlock_);
                esunny_trade_context_.SavePendingOrder(po);
                ret = api_->InsertOrder(&session_id, &po_req);

                // TODO for test
                //usleep(100 * 1000);
                if (ret == TAPIERROR_SUCCEED)
                {
                    esunny_trade_context_.SaveOrderInfo(session_id, new EsunnyOrderInfo(*po));
                }
            }

            TNL_LOG_DEBUG("InsertOrder - return:%d, session_id:%d, serial_no:%ld", ret, session_id, po->serial_no);
            //TNL_LOG_DEBUG("InsertOrder - return:%d, session_id:%d - %s", ret, session_id, ESUNNYDatatypeFormater::ToString(&po_req).c_str());
        }
        catch (...)
        {
            TNL_LOG_ERROR("unknown exception in InsertOrder.");
        }

        return ConvertErrorCode(ret);
    }

    //报单操作请求
    int ReqOrderAction(const T_CancelOrder *pc)
    {
        if (!TunnelIsReady())
        {
            TNL_LOG_WARN("ReqOrderAction when tunnel not ready");
            return TUNNEL_ERR_CODE::NO_VALID_CONNECT_AVAILABLE;
        }
        int ret = TUNNEL_ERR_CODE::RESULT_FAIL;

        try
        {
            // create cancel request object
            const EsunnyOrderInfo * org_order_info = esunny_trade_context_.GetOrderInfoBySN(pc->org_serial_no);
            if (!org_order_info)
            {
                TNL_LOG_WARN("Cancel Order can't get original order by sn");
                return ret;
            }
            TapAPIOrderCancelReq cancel_req;
            ESUNNYPacker::CancelRequest(org_order_info, cancel_req);

            TAPIUINT32 session_id;
            {
                std::lock_guard<std::mutex> lock(api_spinlock_);
                ret = api_->CancelOrder(&session_id, &cancel_req);
            }

            if (ret == TAPIERROR_SUCCEED)
            {
                esunny_trade_context_.SaveCancelSnOfSessionID(session_id, pc->serial_no);
                /*
                T_CancelRespond rsp;
                ESUNNYPacker::CancelRespond(0, pc->serial_no, 0, rsp);

                // 应答
                if (CancelRespond_call_back_handler_) CancelRespond_call_back_handler_(&rsp);
                LogUtil::OnCancelRespond(&rsp, tunnel_info_);
                */
            }


            TNL_LOG_DEBUG("CancelOrder - return:%d, session_id:%d, cancel_sn:%ld, order_sn:%ld", ret, session_id, pc->serial_no, pc->org_serial_no);
            //TNL_LOG_DEBUG("CancelOrder - return:%d, session_id:%d - %s", ret, session_id, ESUNNYDatatypeFormater::ToString(&cancel_req).c_str());
        }
        catch (...)
        {
            TNL_LOG_ERROR("unknown exception in ReqOrderAction.");
        }

        return ConvertErrorCode(ret);
    }

    int QryPosition(TapAPIPositionQryReq *p, int request_id)
    {
        if (!TunnelIsReady())
        {
            TNL_LOG_WARN("QryPosition when tunnel not ready");
            return TUNNEL_ERR_CODE::NO_VALID_CONNECT_AVAILABLE;
        }

        TAPIUINT32 session_id;
        int ret;
        {
            std::lock_guard<std::mutex> lock(api_spinlock_);
            ret = api_->QryPosition(&session_id, p);
        }
        TNL_LOG_INFO("QryPosition - session_id:%d, return:%d", session_id, ret);

        return ConvertErrorCode(ret);
    }
    int QryOrderDetail(TapAPIOrderQryReq *p, int request_id)
    {
        if (!TunnelIsReady())
        {
            TNL_LOG_WARN("QryOrderDetail when tunnel not ready");
            return TUNNEL_ERR_CODE::NO_VALID_CONNECT_AVAILABLE;
        }

        TAPIUINT32 session_id;
        int ret;
        {
            std::lock_guard<std::mutex> lock(api_spinlock_);
            ret = api_->QryOrder(&session_id, p);
        }
        TNL_LOG_INFO("QryOrder - session_id:%d, return:%d", session_id, ret);

        return ConvertErrorCode(ret);
    }
    int QryTradeDetail(TapAPIFillQryReq *p, int request_id)
    {
        if (!TunnelIsReady())
        {
            TNL_LOG_WARN("QryTradeDetail when tunnel not ready");
            return TUNNEL_ERR_CODE::NO_VALID_CONNECT_AVAILABLE;
        }

        TAPIUINT32 session_id;
        int ret;
        {
            std::lock_guard<std::mutex> lock(api_spinlock_);
            ret = api_->QryFill(&session_id, p);
        }
        TNL_LOG_INFO("QryFill - session_id:%d, return:%d", session_id, ret);

        return ConvertErrorCode(ret);
    }


private:
    ITapTradeAPI *api_;
    ESUNNYTradeContext esunny_trade_context_;

    Tunnel_Info tunnel_info_;
    std::string pswd_;
    std::string quote_addr_;
    std::string exchange_code_;

    std::function<void(const T_OrderRespond *)> OrderRespond_call_back_handler_;
    std::function<void(const T_CancelRespond *)> CancelRespond_call_back_handler_;
    std::function<void(const T_OrderReturn *)> OrderReturn_call_back_handler_;
    std::function<void(const T_TradeReturn *)> TradeReturn_call_back_handler_;

    std::function<void(const T_PositionReturn *)> QryPosReturnHandler_;
    std::function<void(const T_OrderDetailReturn *)> QryOrderDetailReturnHandler_;
    std::function<void(const T_TradeDetailReturn *)> QryTradeDetailReturnHandler_;

    // 配置数据对象
    TunnelConfigData cfg_;
    std::atomic_bool logoned_;
    std::atomic_bool in_init_state_; // clear after first success login

    // query position control variables
    std::vector<PositionDetail> pos_buffer_;

    // query order detail control variables
    std::vector<OrderDetail> od_buffer_;

    // query trade detail control variables
    std::vector<TradeDetail> td_buffer_;

    // variables and functions for cancel all unterminated orders automatically
    std::atomic_bool have_handled_unterminated_orders_;
    std::vector<TapAPIOrderInfo> unterminated_orders_;
    std::mutex cancel_sync_;
    std::condition_variable qry_order_finish_cond_;

    void ReqLogin();

    bool TunnelIsReady()
    {
        return logoned_ && HaveFinishQueryOrders();
    }
    bool HaveFinishQueryOrders()
    {
        return have_handled_unterminated_orders_ && finish_query_canceltimes_;
    }

    int ConvertErrorCode(int esunny_err_code);

    // 查询报单（用于撤盘口单，或统计撤单数）
    void QueryAndHandleOrders();

    void ReportErrorState(int api_error_no, const std::string &error_msg);

    TapAPIOrderCancelReq CreatCancelParam(const TapAPIOrderInfo & order_field);
    bool IsOrderTerminate(const TapAPIOrderInfo & order_field);

    std::mutex stats_canceltimes_sync_;
    std::atomic_bool finish_query_canceltimes_;
    std::map<std::string, int> cancel_times_of_contract;

    std::mutex api_spinlock_;

public:
    // 外部接口对象使用，为避免修改接口，新增对象放到此处
    std::mutex rsp_sync;
    std::condition_variable rsp_con;
};

#endif //
