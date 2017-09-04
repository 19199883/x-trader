// 茂源交易通道，对外接口

#ifndef _MY_TRADE_TUNNEL_API_H_
#define _MY_TRADE_TUNNEL_API_H_

#include "my_trade_tunnel_data_type.h"
#include "my_trade_tunnel_struct.h"

#include <functional>
#include <string>
#include <vector>

class DLL_PUBLIC MYTunnelInterface
{
public:
	MYTunnelInterface(){}

	virtual ~MYTunnelInterface(){}

	/// 报单请求
	virtual void PlaceOrder(const T_PlaceOrder *pReq) = 0;

	/// 撤单请求
	virtual void CancelOrder(const T_CancelOrder *pReq) = 0;

	/// 查询持仓
	virtual void QueryPosition(const T_QryPosition *pQryParam) = 0;

	/// 查询报单明细
	virtual void QueryOrderDetail(const T_QryOrderDetail *pQryParam) = 0;

	/// 查询成交明细
	virtual void QueryTradeDetail(const T_QryTradeDetail *pQryParam) = 0;

	/// 查询合约
	virtual void QueryContractInfo(const T_QryContractInfo *pQryParam) = 0;

	///询价录入请求
	virtual void ReqForQuoteInsert(const T_ReqForQuote *pReq) = 0;

	///报价录入请求
	virtual void ReqQuoteInsert(const T_InsertQuote *pReq) = 0;

	///报价操作请求
	virtual void ReqQuoteAction(const T_CancelQuote *pReq) = 0;

	/// 交易响应数据处理函数对象的设置接口
	virtual void SetCallbackHandler(std::function<void(const T_OrderRespond *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_CancelRespond *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_OrderReturn *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_TradeReturn *)> handler) = 0;

	/// 查询回报数据处理函数对象的设置接口
	virtual void SetCallbackHandler(std::function<void(const T_PositionReturn *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_OrderDetailReturn *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_TradeDetailReturn *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_ContractInfoReturn *)> handler) = 0;

	/// 做市商交易相关回报数据处理函数对象的设置接口
	virtual void SetCallbackHandler(std::function<void(const T_RtnForQuote *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_RspOfReqForQuote *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_InsertQuoteRespond *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_CancelQuoteRespond *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_QuoteReturn *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_QuoteTrade *)> handler) = 0;

	/// 获取投资者客户ID
	virtual std::string GetClientID() = 0;
};



extern "C"
{
    DLL_PUBLIC MYTunnelInterface *CreateTradeTunnel(const std::string &tunnel_config_file);
    DLL_PUBLIC void DestroyTradeTunnel(MYTunnelInterface *);
}

class DLL_PUBLIC MYExchangeInterface
{
public:
	/**
	* @param provider_config_file 通道配置文件名
	*/
	MYExchangeInterface(){}

	virtual ~MYExchangeInterface(){}

	/// 报单请求
	virtual void PlaceOrder(const T_PlaceOrder *pReq) = 0;

	/// 撤单请求
	virtual void CancelOrder(const T_CancelOrder *pReq) = 0;

	/// 查询持仓
	virtual void QueryPosition(const T_QryPosition *pQryParam) = 0;

	/// 查询报单明细
	virtual void QueryOrderDetail(const T_QryOrderDetail *pQryParam) = 0;

	/// 查询成交明细
	virtual void QueryTradeDetail(const T_QryTradeDetail *pQryParam) = 0;

	/// 查询合约
	virtual void QueryContractInfo(const T_QryContractInfo *pQryParam) = 0;

	///询价录入请求
	virtual void ReqForQuoteInsert(const T_ReqForQuote *pReq) = 0;

	///报价录入请求
	virtual void ReqQuoteInsert(const T_InsertQuote *pReq) = 0;

	///报价操作请求
	virtual void ReqQuoteAction(const T_CancelQuote *pReq) = 0;

	/// 交易响应数据处理函数对象的设置接口
	virtual void SetCallbackHandler(std::function<void(const T_OrderRespond *, const T_PositionData *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_CancelRespond *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_OrderReturn *, const T_PositionData *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_TradeReturn *, const T_PositionData *)> handler) = 0;

	/// 查询回报数据处理函数对象的设置接口
	virtual void SetCallbackHandler(std::function<void(const T_PositionReturn *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_OrderDetailReturn *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_TradeDetailReturn *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_ContractInfoReturn *)> handler) = 0;

	/// 做市商交易相关回报数据处理函数对象的设置接口
	virtual void SetCallbackHandler(std::function<void(const T_RtnForQuote *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_RspOfReqForQuote *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_InsertQuoteRespond *, const T_PositionData *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_CancelQuoteRespond *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_QuoteReturn *, const T_PositionData *)> handler) = 0;
	virtual void SetCallbackHandler(std::function<void(const T_QuoteTrade *, const T_PositionData *)> handler) = 0;

	/// 获取模型合约仓位
	virtual T_PositionData GetPosition(SerialNoType sn, const std::string &contract) = 0;

	/// 获取投资者客户ID
	virtual std::string GetClientID() = 0;
};

struct my_xchg_cfg
{
    /* data of tunnel */
    char tunnel_cfg_path[1024];
    char tunnel_so_path[1024];

    /* data of position control */
    int init_pos_at_start;
    int st_ctrl_oc;
    int change_oc_flag;

    // added at 20160318
    int init_pos_from_ev; // 0:no; 1:yes
};

extern "C" {
DLL_PUBLIC MYExchangeInterface *CreateExchange(struct my_xchg_cfg& cfg/*const std::string &future_exchange_config_file*/);
DLL_PUBLIC void DestroyExchange(MYExchangeInterface *);
}

#endif
