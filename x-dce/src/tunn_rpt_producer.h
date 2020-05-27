#ifndef __TUNN_RPT_PRODUCER_H__
#define __TUNN_RPT_PRODUCER_H__

#include <array>
#include <string>
#include "vrt_value_obj.h"
#include "X1FtdcTraderApi.h"

using namespace std;
using namespace x1ftdcapi;

#define CANCELLED_FROM_PENDING 99999999

/*
 * 10 power of 2
 */

struct Tunnconfig
{
	string address;
	string brokerid;
	string userid;
	string password;
	char exchange;

	// disruptor yield strategy
	char yield[20];
};

struct TunnRpt
{
	TX1FtdcLocalOrderIDType               LocalOrderID;                 ///< 本地委托号
	// TODO: order id
	TX1FtdcX1OrderIDType				  OrderID;				//柜台委托号	

	TX1FtdcOrderAnswerStatusType          OrderStatus;                  ///< 委托状态
	TX1FtdcAmountType                     MatchedAmount;                ///< 成交数量
	TX1FtdcPriceType                      MatchedPrice;                 ///< 成交价格
	TX1FtdcErrorIDType                    ErrorID;                      ///< 错误ID
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
		int QryOrderDetail(CX1FtdcQryOrderField *p);
		int QryTradeDetail(CX1FtdcQryMatchField *p);
		int ReqForQuoteInsert(CX1FtdcForQuoteField *p);
		///报价录入请求
		int ReqQuoteInsert(CX1FtdcQuoteInsertField *p);
		///报价操作请求
		int ReqQuoteAction(CX1FtdcCancelOrderField *p);

		/*
		 * things relating to x-trader internal logic
		 */
		long NewLocalOrderID(int32_t strategyid);
		const char* GetAccount();
		TunnRpt* GetRpt(int32_t index);
		int32_t GetStrategyID(TunnRpt& rpt);
		int32_t GetCounterByLocalOrderID(long local_ord_id);

		void End();

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
		virtual void OnRspCancelAllOrder(struct CX1FtdcCancelAllOrderRspField*pf, struct CX1FtdcRspErrorField* pe) {};
		/**
		 * 询价响应
		 * @param pRspForQuoteData:询价请求结构地址。
		 * @return 0 - 请求发送成功 -1 - 请求发送失败  -2 -检测异常。
		 */
		virtual void OnRspForQuote(struct CX1FtdcForQuoteRspField* pf, struct CX1FtdcRspErrorField* pe) {};
		/**
		 * 询价回报
		 * @param pRspForQuoteData:询价请求结构地址。
		 * @return 0 - 请求发送成功 -1 - 请求发送失败  -2 -检测异常。
		 */
		virtual void OnRtnForQuote(struct CX1FtdcForQuoteRtnField* pf){} ;

private:
    void ParseConfig();
    void ReqLogin();

	struct vrt_producer  *producer_;
	TunnRpt rpt_buffer_[RPT_BUFFER_SIZE];
	Tunnconfig config_;
	const char * module_name_;  
	bool ended_;

	/*
	 * true: cancel request; false; place request
	 * 请求与在数组存储位置的关系：RequestID与LocalOrderID相同，
	 * RequestID的counter部分作为数组下标
	 */
	bool cancel_requests_[COUNTER_UPPER_LIMIT];

	/*
	 * things relating to counter API
	 */
	int32_t Push();

	x1ftdcapi::CX1FtdcTraderApi *api_;
	char appid_[30];
	char authcode_[40];
};

#endif
