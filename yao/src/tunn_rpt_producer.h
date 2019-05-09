#ifndef __TUNN_RPT_PRODUCER_H__
#define __TUNN_RPT_PRODUCER_H__

#include <array>
#include <string>
#include <unordered_map>
#include "vrt_value_obj.h"
#include "ThostFtdcTraderApi.h"
#include "signal.h"

using namespace std;

#define CANCELLED_FROM_PENDING 99999999

#define CTP_EXCHANGE_SHFE "SHFE"
#define CTP_EXCHANGE_DCE "DCE"
#define CTP_EXCHANGE_CZCE "CZCE"

#define OEDERINSERT_REQUESTID 1
#define ORDERCANCEL_REQUESTID 2

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
	char TradingDay[20];
	/*
	 * true:夜盘交易
	 * false：日盘交易
	 */
	bool IsNightTrading;
};

struct TunnRpt
{
	int					        LocalOrderID;   ///< 本地委托号,对应ctp的OrderRef
	TThostFtdcOrderStatusType   OrderStatus;    ///< 委托状态
	TThostFtdcVolumeType        MatchedAmount;  ///< 成交数量
	TThostFtdcPriceType         MatchedPrice;   ///< 成交价格
	TThostFtdcErrorIDType       ErrorID;        ///< 错误ID
	TThostFtdcExchangeIDType	ExchangeID;
	TThostFtdcOrderSysIDType	OrderSysID;
};

class CtpFieldConverter
{
	public:
		/*
			将yao系统内部的交易所定义转换成ctp中的交易所定义
		*/
		static const char* ConvertExchange(exchange_names yExc)
		{
			if(exchange_names::SHFE == yExc){
				return CTP_EXCHANGE_SHFE;
			}else if(exchange_names::XDCE == yExc){
				return CTP_EXCHANGE_DCE;
			}else if(exchange_names::XZCE == yExc){
				return CTP_EXCHANGE_CZCE;
			}else{return "";}			
		}
		
		static void InitNewOrder(const char *userid, const char* brokerid)
		{
			// done
			memset(&new_order_, 0, sizeof(CThostFtdcInputOrderField));

			new_order_.RequestID = 0;
			strncpy(new_order_.BrokerID, brokerid, sizeof(TThostFtdcBrokerIDType));
			strncpy(new_order_.InvestorID, userid, sizeof(TThostFtdcInvestorIDType));
			strncpy(new_order_.UserID, userid, sizeof(TThostFtdcInvestorIDType));
			new_order_.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			new_order_.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
			new_order_.TimeCondition = THOST_FTDC_TC_GFD;
			strcpy(new_order_.GTDDate, "");
			new_order_.VolumeCondition = THOST_FTDC_VC_AV;
			new_order_.MinVolume = 1;
			new_order_.ContingentCondition = THOST_FTDC_CC_Immediately;
			new_order_.StopPrice = 0;
			// 强平原因
			new_order_.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
			// 自动挂起标志
			new_order_.IsAutoSuspend = 0;
			//用户强平标志
			new_order_.UserForceClose = 0;
		}

		/*
			将策略产生的信号等信息生成ctp需要的下单对象
		*/
		static CThostFtdcInputOrderField*  Convert(const signal_t& sig,int localorderid,int32_t vol)
		{	
			strncpy(new_order_.ExchangeID, ConvertExchange(sig.exchange), sizeof(new_order_.ExchangeID));						
			new_order_.RequestID = OEDERINSERT_REQUESTID;
			strncpy(new_order_.InstrumentID, sig.symbol, sizeof(new_order_.InstrumentID));
			snprintf(new_order_.OrderRef, sizeof(TThostFtdcOrderRefType), "%d", localorderid);
 
			if (sig.sig_act == signal_act_t::buy){
				new_order_.LimitPrice = sig.buy_price;
				new_order_.Direction = THOST_FTDC_D_Buy;
			}else if (sig.sig_act == signal_act_t::sell){
				new_order_.LimitPrice = sig.sell_price;
				new_order_.Direction = THOST_FTDC_D_Sell;
			}else{
				 clog_warning("[%s] do support Direction value:%d; sig id:%d", "CtpFieldConverter",
					new_order_.Direction, sig.sig_id); 
			}

			// TODO: 是否平今还是平昨，需要根据市场和昨仓决定
			if (sig.sig_openclose == alloc_position_effect_t::open_){
				new_order_.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
			}else if (sig.sig_openclose == alloc_position_effect_t::close_){
				new_order_.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
			}else{
				clog_warning("[%s] do support sig_openclose value:%d; sig id:%d", "CtpFieldConverter",
				sig.sig_openclose, sig.sig_id); 
			}

			new_order_.VolumeTotalOriginal = vol;

			return &new_order_;
		}

		static void InitCancelOrder(const char *userid, const char* brokerid,
			TThostFtdcFrontIDType frontid, TThostFtdcSessionIDType	sessionid)
		{
			memset(&cancel_order_, 0, sizeof(cancel_order_));
			strncpy(cancel_order_.BrokerID, brokerid, sizeof(TThostFtdcBrokerIDType));
			strncpy(cancel_order_.InvestorID, userid, sizeof(TThostFtdcInvestorIDType));
			cancel_order_.ActionFlag = THOST_FTDC_AF_Delete;	
			strncpy(cancel_order_.UserID, userid, sizeof(TThostFtdcInvestorIDType));
			cancel_order_.FrontID = frontid;
			cancel_order_.SessionID = sessionid;					
		}
		
		static CThostFtdcInputOrderActionField* Convert(exchange_names exchange, const char* symbol, 
				int cancel_localorderid, int orig_localorderid, TThostFtdcOrderSysIDType ordersysid)
		{
			cancel_order_.OrderActionRef = cancel_localorderid;
			snprintf(cancel_order_.OrderRef, sizeof(TThostFtdcOrderRefType), "%d", orig_localorderid);
			strncpy(cancel_order_.ExchangeID, ConvertExchange(exchange), sizeof(cancel_order_.ExchangeID));						
			strncpy(cancel_order_.OrderSysID, ordersysid, sizeof(cancel_order_.OrderSysID));
			strncpy(cancel_order_.InstrumentID, symbol, sizeof(cancel_order_.InstrumentID));
			return &cancel_order_;
	
		}
		
	private:
		static CThostFtdcInputOrderField new_order_;
		static CThostFtdcInputOrderActionField cancel_order_;
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
		int NewLocalOrderID(int32_t strategyid);
		const char* GetAccount();
		TunnRpt* GetRpt(int32_t index);
		int32_t GetStrategyID(TunnRpt& rpt);
		int32_t GetCounterByLocalOrderID(int local_ord_id);

		void End();
		bool IsReady();
		bool IsFinal(TThostFtdcOrderStatusType   OrderStatus);

		bool Ended();

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
		virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

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
		TThostFtdcFrontIDType	frontid_;
		///会话编号
		TThostFtdcSessionIDType	sessionid_;
		///最大报单引用
		int	MaxOrderRef_;
		int counter_;
		// key: 合约；value：仓位对象
		std::unordered_map<std::string,CThostFtdcInvestorPositionField> positions_;
		
		bool position_ready_;
		char appid_[30];
		char authcode_[40];
	
};

#endif
