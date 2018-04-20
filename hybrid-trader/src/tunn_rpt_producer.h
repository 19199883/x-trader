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
#define MY_TNL_EXID_SHFE  "SHFE"
#define MY_TNL_EXID_INE  "INE"

struct Tunnconfig
{
	string address;
	string brokerid;
	string investorid;
	string userid;
	string password;
	char exchange;

	// disruptor yield strategy
	char yield[20];
};

struct TunnRpt
{
	// TODO: to be tested
	char								contract[10];
	int									LocalOrderID;                 ///< 本地委托号
	TThostFtdcOrderStatusType			OrderStatus;                  ///< 委托状态
	TThostFtdcVolumeType					MatchedAmount;                ///< 成交数量
	TThostFtdcPriceType					TradePrice;					  ///< 成交价格
	TThostFtdcErrorIDType				ErrorID;                      ///< 错误ID
};

class TunnRptProducer: public CThostFtdcTraderSpi 
{
	public:
		TunnRptProducer(struct vrt_queue  *queue);
		virtual ~TunnRptProducer();

		/*
		 * things relating to X1 Api
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
		void End();
		Tunnconfig config_;

	private:
		/*
		 * things relating to CTP API
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

		///心跳超时警告。当长时间未收到报文时，该方法被调用。
		///@param nTimeLapse 距离上次接收报文的时间
		virtual void OnHeartBeatWarning(int nTimeLapse);

		///错误应答
		virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

		///风控前置系统用户登录应答
		virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID,
		bool bIsLast);

		///用户退出应答
		virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

		///报单录入应答
		virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast); 
		///报单操作应答
		virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

		///成交回报
		virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

		///报单回报
		virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

		///报单录入错误回报
		virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);

		///报单操作错误回报
		virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);

private:
    void ParseConfig();
    void ReqLogin();
	long GetCounterByLocalOrderID(long localorderid);

	struct vrt_producer  *producer_;
	TunnRpt rpt_buffer_[RPT_BUFFER_SIZE];
	const char * module_name_;  
	bool ended_;
	static int32_t counter_;

	/*
	 * things relating to counter API
	 */
	int32_t Push();

	CThostFtdcTraderApi *api_;
};

#endif
