#ifndef ESUNNY_TRADE_INTERFACE_H_
#define ESUNNY_TRADE_INTERFACE_H_

#include "config_data.h"
#include "trade_data_type.h"
#include "my_protocol_packager.h"
#include "my_tunnel_lib.h"
#include "my_cmn_util_funcs.h"
#include "tunnel_cmn_utility.h"
#include "field_convert.h"

struct OriginalReqInfo;

class MYEsunnyTradeSpi: public ITapTradeAPINotify
{
public:
    MYEsunnyTradeSpi(const TunnelConfigData &cfg);
    virtual ~MYEsunnyTradeSpi(void);

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
        int ret = TUNNEL_ERR_CODE::RESULT_FAIL;

		TapAPINewOrder po_req;
		bool convert_res = ESUNNYPacker::OrderRequest(cfg_, po, po_req);
		if (!convert_res) {
			TNL_LOG_WARN("PlaceOrder can't find contract info to complete the order");
			return ret;
		}

		TAPIUINT32 session_id;
			esunny_trade_context_.SavePendingOrder(po);
			ret = api_->InsertOrder(&session_id, &po_req);
		TNL_LOG_DEBUG("InsertOrder - return:%d, session_id:%d, serial_no:%ld", ret, session_id, po->serial_no);
    }

    //报单操作请求
    int ReqOrderAction(const T_CancelOrder *pc)
    {
        if (!TunnelIsReady()){
            TNL_LOG_WARN("ReqOrderAction when tunnel not ready");
            return TUNNEL_ERR_CODE::NO_VALID_CONNECT_AVAILABLE;
        }
        int ret = TUNNEL_ERR_CODE::RESULT_FAIL;

		TapAPIOrderCancelReq cancel_req;
		ESUNNYPacker::CancelRequest(org_order_info, cancel_req);
		TAPIUINT32 session_id;
			ret = api_->CancelOrder(&session_id, &cancel_req);
		TNL_LOG_DEBUG("CancelOrder - return:%d, session_id:%d, cancel_sn:%ld, order_sn:%ld", ret, session_id, pc->serial_no, pc->org_serial_no);
    }

private:

    Tunnel_Info tunnel_info_;
    std::string pswd_;

    TunnelConfigData cfg_;
    std::atomic_bool logoned_;
    std::atomic_bool in_init_state_; // clear after first success login

    void ReqLogin();
    bool TunnelIsReady() { return logoned_ ; }
    // 查询报单（用于撤盘口单，或统计撤单数）
    bool IsOrderTerminate(const TapAPIOrderInfo & order_field);
};

#endif //
