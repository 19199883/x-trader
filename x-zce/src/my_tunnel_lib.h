#pragma once

#include <vector>
#include <functional>
#include "my_trade_tunnel_api.h"
#include "trade_log_util.h"

// 前向声明
class TunnelConfigData;

class EsunnyTunnel: public MYTunnelInterface
{
public:
    /**
     * @param provider_config_file 配置文件名
     */
    EsunnyTunnel(const std::string &ctp_config_file);
    virtual ~EsunnyTunnel();

    /// 报单请求
    virtual void PlaceOrder(const T_PlaceOrder *pReq);

    /// 撤单请求
    virtual void CancelOrder(const T_CancelOrder *pReq);

    /// 查询持仓
    virtual void QueryPosition(const T_QryPosition *pQryParam);

    /// 查询报单明细
    virtual void QueryOrderDetail(const T_QryOrderDetail *pQryParam);

    /// 查询成交明细
    virtual void QueryTradeDetail(const T_QryTradeDetail *pQryParam);

    /// 查询合约
    virtual void QueryContractInfo(const T_QryContractInfo *pQryParam);

    ///询价录入请求
    virtual void ReqForQuoteInsert(const T_ReqForQuote *pReq);

    ///报价录入请求
    virtual void ReqQuoteInsert(const T_InsertQuote *pReq);

    ///报价操作请求
    virtual void ReqQuoteAction(const T_CancelQuote *pReq);

    /// 交易响应数据处理函数对象的设置接口
    virtual void SetCallbackHandler(std::function<void(const T_OrderRespond *)> handler);
    virtual void SetCallbackHandler(std::function<void(const T_CancelRespond *)> handler);
    virtual void SetCallbackHandler(std::function<void(const T_OrderReturn *)> handler);
    virtual void SetCallbackHandler(std::function<void(const T_TradeReturn *)> handler);

    /// 查询回报数据处理函数对象的设置接口
    virtual void SetCallbackHandler(std::function<void(const T_PositionReturn *)> handler);
    virtual void SetCallbackHandler(std::function<void(const T_OrderDetailReturn *)> handler);
    virtual void SetCallbackHandler(std::function<void(const T_TradeDetailReturn *)> handler);
    virtual void SetCallbackHandler(std::function<void(const T_ContractInfoReturn *)> handler);

    /// 做市商交易相关回报数据处理函数对象的设置接口
    virtual void SetCallbackHandler(std::function<void(const T_RtnForQuote *)> handler);
    virtual void SetCallbackHandler(std::function<void(const T_RspOfReqForQuote *)> handler);
    virtual void SetCallbackHandler(std::function<void(const T_InsertQuoteRespond *)> handler);
    virtual void SetCallbackHandler(std::function<void(const T_CancelQuoteRespond *)> handler);
    virtual void SetCallbackHandler(std::function<void(const T_QuoteReturn *)> handler);
    virtual void SetCallbackHandler(std::function<void(const T_QuoteTrade *)> handler);

    /// 获取投资者客户ID
    virtual std::string GetClientID();

private:
    // 禁止拷贝和赋值
    EsunnyTunnel(const EsunnyTunnel & other);
    EsunnyTunnel operator=(const EsunnyTunnel & other);

    // 内部实现接口
    bool InitInf(const TunnelConfigData &cfg);

    void *trade_inf_;

    char exchange_code_;
    Tunnel_Info tunnel_info_;

    // 配置数据对象
    TunnelConfigData *lib_cfg_;

    // 数据处理函数对象
    std::function<void(const T_OrderRespond *)> OrderRespond_call_back_handler_;
    std::function<void(const T_CancelRespond *)> CancelRespond_call_back_handler_;
    std::function<void(const T_PositionReturn *)> QryPosReturnHandler_;
    std::function<void(const T_OrderDetailReturn *)> QryOrderDetailReturnHandler_;
    std::function<void(const T_TradeDetailReturn *)> QryTradeDetailReturnHandler_;
    std::function<void(const T_ContractInfoReturn *)> QryContractInfoHandler_;

    std::function<void(const T_RspOfReqForQuote *)> RspOfReqForQuoteHandler_;
    std::function<void(const T_InsertQuoteRespond *)> InsertQuoteRespondHandler_;
    std::function<void(const T_CancelQuoteRespond *)> CancelQuoteRespondHandler_;

    // pending respond, should return in another thread
    std::vector<std::pair<int, void *> > pending_rsp_;
    void SendRespondAsync(int rsp_type, void *rsp);
    static void RespondHandleThread(EsunnyTunnel *ptunnel);
};
