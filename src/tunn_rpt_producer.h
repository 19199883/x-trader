#ifndef __TUNN_RPT_PRODUCER_H__
#define __TUNN_RPT_PRODUCER_H__

#include <array>
#include <string>
#include <libcork/core.h>
#include <libcork/helpers/errors.h>
#include <vrt.h>
#include "vrt_value_obj.h"
#include "X1FtdcTraderApi.h"

using namespace std;
/*
 * 10 power of 2
 */
#define RPT_BUFFER_SIZE 32768 
#define CLOG_MODULE "TunnRptProducer" 

struct Tunnconfig
{
	string address;
	string brokerid;
	string userid;
	string password;
	char exchange;
};

struct TunnRpt
{
	TX1FtdcLocalOrderIDType               LocalOrderID;                 ///< 本地委托号
	TX1FtdcOrderAnswerStatusType          OrderStatus;                  ///< 委托状态
	TX1FtdcAmountType                     MatchedAmount;                ///< 成交数量
	TX1FtdcPriceType                      MatchedPrice;                 ///< 成交价格
	TX1FtdcErrorIDType                    ErrorID;                      ///< 错误ID
	TX1FtdcAmountType                     CancelAmount;                 ///< 撤单数量
};

class TunnRptProducer: public x1ftdcapi::CX1FtdcTraderSpi
{
	public:
		TunnRptProducer(struct vrt_queue  *queue);
		~TunnRptProducer();

		/*
		 * things relating to X1 Api
		 */
		// 下发指令接口
		int ReqOrderInsert(CX1FtdcInsertOrderField *p);
		// 撤单操作请求
		int ReqOrderAction(CX1FtdcCancelOrderField *p);
		int QryPosition(CX1FtdcQryPositionDetailField *p);
		int QryOrderDetail(CX1FtdcQryOrderField *p)
		int QryTradeDetail(CX1FtdcQryMatchField *p);
		int ReqForQuoteInsert(CX1FtdcForQuoteField *p);
		///报价录入请求
		int ReqQuoteInsert(CX1FtdcQuoteInsertField *p);
		///报价操作请求
		int ReqQuoteAction(CX1FtdcCancelOrderField *p);


	private:
		/*
		 * things relating to X1 API
		 */
		///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
		virtual void OnFrontConnected();
		///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
		///@param nReason 错误原因
		///        0x1001 网络读失败
		///        0x1002 网络写失败
		///        0x2001 接收心跳超时
		///        0x2002 发送心跳失败
		///        0x2003 收到错误报文
		virtual void OnFrontDisconnected(int nReason);
		//客户请求登录响应
		virtual void OnRspUserLogin(struct CX1FtdcRspUserLoginField *pf, struct CX1FtdcRspErrorField *pe);
		//客户退出请求响应
		virtual void OnRspUserLogout(struct CX1FtdcRspUserLogoutInfoField *pf, struct CX1FtdcRspErrorField *pe);
		//委托下单响应
		virtual void OnRspInsertOrder(struct CX1FtdcRspOperOrderField *pf, struct CX1FtdcRspErrorField *pe);
		//委托撤单响应
		virtual void OnRspCancelOrder(struct CX1FtdcRspOperOrderField *pf, struct CX1FtdcRspErrorField *pe);
		//持仓查询响应
		virtual void OnRspQryPosition(struct CX1FtdcRspPositionField *pf, struct CX1FtdcRspErrorField *pe, bool bIsLast);
		virtual void OnRspQryPositionDetail(struct CX1FtdcRspPositionDetailField * pf, struct CX1FtdcRspErrorField * pe, bool bIsLast);
		//客户资金查询响应
		virtual void OnRspCustomerCapital(struct CX1FtdcRspCapitalField*pf, struct CX1FtdcRspErrorField *pe, bool bIsLast);
		//交易所合约查询响应
		virtual void OnRspQryExchangeInstrument(struct CX1FtdcRspExchangeInstrumentField *pf, struct CX1FtdcRspErrorField *pe, bool bIsLast);
		//错误回报
		virtual void OnRtnErrorMsg(struct CX1FtdcRspErrorField *pf);
		//成交回报
		virtual void OnRtnMatchedInfo(struct CX1FtdcRspPriMatchInfoField *pf);
		//委托回报
		virtual void OnRtnOrder(struct CX1FtdcRspPriOrderField *pf);
		//撤单回报
		virtual void OnRtnCancelOrder(struct CX1FtdcRspPriCancelOrderField *pf);

		virtual void OnRspQryOrderInfo(struct CX1FtdcRspOrderField *pf, struct CX1FtdcRspErrorField * pe, bool bIsLast);
		virtual void OnRspQryMatchInfo(struct CX1FtdcRspMatchField *pf, struct CX1FtdcRspErrorField * pe, bool bIsLast);
		/**
		 * 做市商报单响应
		 * @param pRspQuoteData:指向做市商报单响应地址的指针。
		 */
		virtual void OnRspQuoteInsert(struct CX1FtdcQuoteRspField* pf, struct CX1FtdcRspErrorField* pe){};
		/**
		 * 做市商报单回报
		 * @param pRtnQuoteData:指向做市商报单回报地址的指针。
		 */
		virtual void OnRtnQuoteInsert(struct CX1FtdcQuoteRtnField* pf){};
		/**
		 * 做市商成交回报
		 * @param pRtnQuoteMatchedData:指向做市商成交回报地址的指针。
		 */
		virtual void OnRtnQuoteMatchedInfo(struct CX1FtdcQuoteMatchRtnField* pf){};
		/**
		 * 做市商撤单响应
		 * @param pRspQuoteCanceledData:指向做市商撤单响应地址的指针。
		 */
		virtual void OnRspQuoteCancel(struct CX1FtdcQuoteRspField* pf, struct CX1FtdcRspErrorField* pe){};
		/**
		 * 做市商撤单回报
		 * @param pRtnQuoteCanceledData:指向做市商撤单回报地址的指针。
		 */
		virtual void OnRtnQuoteCancel(struct CX1FtdcQuoteCanceledRtnField* pf){};
		/**
		 * 交易所状态通知
		 * @param pRtnExchangeStatusData:指向交易所状态通知地址的指针。
		 */
		virtual void OnRtnExchangeStatus(struct CX1FtdcExchangeStatusRtnField* pf);
		/**
		 * 批量撤单响应
		 * @param pRspStripCancelOrderData:指向批量撤单响应地址的指针。
		 */
		virtual void OnRspCancelAllOrder(struct CX1FtdcCancelAllOrderRspField*pf, struct CX1FtdcRspErrorField* pe);
		/**
		 * 询价响应
		 * @param pRspForQuoteData:询价请求结构地址。
		 * @return 0 - 请求发送成功 -1 - 请求发送失败  -2 -检测异常。
		 */
		virtual void OnRspForQuote(struct CX1FtdcForQuoteRspField* pf, struct CX1FtdcRspErrorField* pe);
		/**
		 * 询价回报
		 * @param pRspForQuoteData:询价请求结构地址。
		 * @return 0 - 请求发送成功 -1 - 请求发送失败  -2 -检测异常。
		 */
		virtual void OnRtnForQuote(struct CX1FtdcForQuoteRtnField* pf);


private:
    bool ParseConfig();
    void ReqLogin();

public:
    X1TradeContext xspeed_trade_context_;
    std::mutex client_sync;
    // 外部接口对象使用，为避免修改接口，新增对象放到此处
    std::mutex rsp_sync;
    std::condition_variable rsp_con;

private:
    x1ftdcapi::CX1FtdcTraderApi *api_;
    long session_id_;
    long max_order_ref_;
    TX1FtdcDateType trade_day;

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
    std::function<void(const T_ContractInfoReturn *)> QryContractInfoHandler_;

    // added for support market making interface
    std::function<void(const T_RspOfReqForQuote *)> RspOfReqForQuoteHandler_;
    std::function<void(const T_RtnForQuote *)> RtnForQuoteHandler_;
    std::function<void(const T_InsertQuoteRespond *)> InsertQuoteRespondHandler_;
    std::function<void(const T_CancelQuoteRespond *)> CancelQuoteRespondHandler_;
    std::function<void(const T_QuoteReturn *)> QuoteReturnHandler_;
    std::function<void(const T_QuoteTrade *)> QuoteTradeHandler_;

    // 配置数据对象
    TunnelConfigData cfg_;
    volatile bool connected_;
    std::atomic_bool logoned_;
    QueryInfo query_info_;

    void CheckAndSaveYestodayPosition();
    void LoadYestodayPositionFromFile(const std::string &file);
    void SaveYestodayPositionToFile(const std::string &file);

    // variables and functions for cancel all unterminated orders automatically
    std::atomic_bool have_handled_unterminated_orders_;
    std::mutex cancel_sync_;
    std::condition_variable qry_order_finish_cond_;
    std::thread *cancel_t_;
    //void CancelUnterminatedOrders(){};

    volatile bool is_ready_;
    volatile bool query_is_ready_;
    struct timeval timer_start_, timer_end_;

    std::mutex cancel_times_sync_;
    std::map<std::string, int> cancel_times_of_contract;

    void CalcCancelTimes(const struct CX1FtdcRspOrderField* const pf, const struct CX1FtdcRspErrorField* const pe, const bool bIsLast);
    void QueryAllBeforeReady();
    void ReportErrorState(int api_error_no, const std::string &error_msg);

    volatile bool in_init_state_; // clear after login
		struct Tunnconfig config_;
		void ParseConfig();


		struct vrt_producer  *producer_;
		std::array<TunnRpt, RPT_BUFFER_SIZE> rpt_buffer_;

		/*
		 * things relating to counter API
		 */
		x1ftdcapi::CX1FtdcTraderApi *api_;
};

#endif
