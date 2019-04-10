#ifndef __TUNN_RPT_PRODUCER_H__
#define __TUNN_RPT_PRODUCER_H__

#include <array>
#include <string>
#include "vrt_value_obj.h"
#include "ThostFtdcTraderApi.h"

using namespace std;

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
	int					        LocalOrderID;   ///< 本地委托号
	TThostFtdcOrderStatusType   OrderStatus;    ///< 委托状态
	TThostFtdcVolumeType        MatchedAmount;  ///< 成交数量
	TThostFtdcPriceType         MatchedPrice;   ///< 成交价格
	TThostFtdcErrorIDType       ErrorID;        ///< 错误ID
};

class TunnRptProducer: public CThostFtdcTraderSpi
{
	public:
		TunnRptProducer(struct vrt_queue  *queue);
		~TunnRptProducer();

		/*
		 * things relating to ctp Api
		 */
		// 下发指令接口
		int ReqOrderInsert(CThostFtdcInputOrderField *p);
		// 撤单操作请求
		int ReqOrderAction(CThostFtdcInputOrderActionField *p);
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
		 * things relating to ctp API
		 */
		///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
		virtual void OnFrontConnected();
		virtual void OnFrontDisconnected(int nReason);
		virtual void OnHeartBeatWarning(int nTimeLapse);
		//客户请求登录响应
		virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		//客户退出请求响应
		virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		//委托下单响应
		virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		//委托撤单响应
		virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		
			virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
			///报单通知
			virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);
			///成交通知
			virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);
			///报单录入错误回报
			virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
			//报单操作错误回报
			virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);

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

	CThostFtdcTraderApi *api_;
	char TradingDay_[9];
	TThostFtdcFrontIDType	FrontID_;
	///会话编号
	TThostFtdcSessionIDType	SessionID_;
	///最大报单引用
	TThostFtdcOrderRefType	MaxOrderRef_;
};

#endif
