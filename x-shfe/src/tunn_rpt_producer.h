#ifndef __TUNN_RPT_PRODUCER_H__
#define __TUNN_RPT_PRODUCER_H__

#include <array>
#include <string>
#include "vrt_value_obj.h"
#include "USTPFtdcTraderApi.h"

using namespace std;

/*
 * 10 power of 2
 */
#define RPT_BUFFER_SIZE 4096

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
	int									LocalOrderID;                 ///< 本地委托号
	TUstpFtdcOrderStatusType			OrderStatus;                  ///< 委托状态
	TUstpFtdcVolumeType					MatchedAmount;                ///< 成交数量
	TUstpFtdcPriceType					MatchedPrice;                 ///< 成交价格
	TUstpFtdcErrorIDType				ErrorID;                      ///< 错误ID
	TUstpFtdcVolumeType					CancelAmount;                 ///< 撤单数量
};

class TunnRptProducer: public CUstpFtdcTraderSpi 
{
	public:
		TunnRptProducer(struct vrt_queue  *queue);
		virtual ~TunnRptProducer();

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

		void End();

	private:
		/*
		 * things relating to femas API
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

		///报文回调开始通知。当API收到一个报文后，首先调用本方法，然后是各数据域的回调，最后是报文回调结束通知。
		///@param nTopicID 主题代码（如私有流、公共流、行情流等）
		///@param nSequenceNo 报文序号
		virtual void OnPackageStart(int nTopicID, int nSequenceNo);

		///报文回调结束通知。当API收到一个报文后，首先调用报文回调开始通知，然后是各数据域的回调，最后调用本方法。
		///@param nTopicID 主题代码（如私有流、公共流、行情流等）
		///@param nSequenceNo 报文序号
		virtual void OnPackageEnd(int nTopicID, int nSequenceNo);

		///错误应答
		virtual void OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

		///风控前置系统用户登录应答
		virtual void OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID,
		bool bIsLast);

		///用户退出应答
		virtual void OnRspUserLogout(CUstpFtdcRspUserLogoutField *pRspUserLogout, CUstpFtdcRspInfoField *pRspInfo, int nRequestID,
		bool bIsLast);

		///用户密码修改应答
		virtual void OnRspUserPasswordUpdate(CUstpFtdcUserPasswordUpdateField *pUserPasswordUpdate, CUstpFtdcRspInfoField *pRspInfo,
			int nRequestID, bool bIsLast)
		{
		}

		///报单录入应答
		virtual void OnRspOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID,
		bool bIsLast);

		///报单操作应答
		virtual void OnRspOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo, int nRequestID,
		bool bIsLast);

		///数据流回退通知
		virtual void OnRtnFlowMessageCancel(CUstpFtdcFlowMessageCancelField *pFlowMessageCancel)
		{
		}

		///成交回报
		virtual void OnRtnTrade(CUstpFtdcTradeField *pTrade);

		///报单回报
		virtual void OnRtnOrder(CUstpFtdcOrderField *pOrder);

		///报单录入错误回报
		virtual void OnErrRtnOrderInsert(CUstpFtdcInputOrderField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo);

		///报单操作错误回报
		virtual void OnErrRtnOrderAction(CUstpFtdcOrderActionField *pOrderAction, CUstpFtdcRspInfoField *pRspInfo);

		///合约交易状态通知
		virtual void OnRtnInstrumentStatus(CUstpFtdcInstrumentStatusField *pInstrumentStatus)
		{
		}

		///报单查询应答
		virtual void OnRspQryOrder(CUstpFtdcOrderField *pOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

		///成交单查询应答
		virtual void OnRspQryTrade(CUstpFtdcTradeField *pTrade, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

		///可用投资者账户查询应答
		virtual void OnRspQryUserInvestor(CUstpFtdcRspUserInvestorField *pRspUserInvestor, CUstpFtdcRspInfoField *pRspInfo,
			int nRequestID, bool bIsLast)
		{
		}

		///交易编码查询应答
		virtual void OnRspQryTradingCode(CUstpFtdcRspTradingCodeField *pRspTradingCode, CUstpFtdcRspInfoField *pRspInfo, int nRequestID,
		bool bIsLast)
		{
		}

		///投资者资金账户查询应答
		virtual void OnRspQryInvestorAccount(CUstpFtdcRspInvestorAccountField *pRspInvestorAccount, CUstpFtdcRspInfoField *pRspInfo,
			int nRequestID, bool bIsLast)
		{
		}

		///合约查询应答
		virtual void OnRspQryInstrument(CUstpFtdcRspInstrumentField *pRspInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID,
		bool bIsLast)
		{
		}

		///交易所查询应答
		virtual void OnRspQryExchange(CUstpFtdcRspExchangeField *pRspExchange, CUstpFtdcRspInfoField *pRspInfo, int nRequestID,
		bool bIsLast)
		{
		}

		///投资者持仓查询应答
		virtual void OnRspQryInvestorPosition(CUstpFtdcRspInvestorPositionField *pRspInvestorPosition, CUstpFtdcRspInfoField *pRspInfo,
			int nRequestID, bool bIsLast);

		///订阅主题应答
		virtual void OnRspSubscribeTopic(CUstpFtdcDisseminationField *pDissemination, CUstpFtdcRspInfoField *pRspInfo, int nRequestID,
		bool bIsLast)
		{
		}

		///合规参数查询应答
		virtual void OnRspQryComplianceParam(CUstpFtdcRspComplianceParamField *pRspComplianceParam, CUstpFtdcRspInfoField *pRspInfo,
			int nRequestID, bool bIsLast)
		{
		}

		///主题查询应答
		virtual void OnRspQryTopic(CUstpFtdcDisseminationField *pDissemination, CUstpFtdcRspInfoField *pRspInfo, int nRequestID,
		bool bIsLast)
		{
		}

		///投资者手续费率查询应答
		virtual void OnRspQryInvestorFee(CUstpFtdcInvestorFeeField *pInvestorFee, CUstpFtdcRspInfoField *pRspInfo, int nRequestID,
		bool bIsLast)
		{
		}

		///投资者保证金率查询应答
		virtual void OnRspQryInvestorMargin(CUstpFtdcInvestorMarginField *pInvestorMargin, CUstpFtdcRspInfoField *pRspInfo,
			int nRequestID, bool bIsLast)
		{
		}


private:
    void ParseConfig();
    void ReqLogin();

	struct vrt_producer  *producer_;
	std::array<TunnRpt, RPT_BUFFER_SIZE> rpt_buffer_;
	Tunnconfig config_;
	const char * module_name_;  
	bool ended_;

	/*
	 * things relating to counter API
	 */
	int32_t Push(const TunnRpt& rpt);

	x1ftdcapi::CX1FtdcTraderApi *api_;
};

#endif
