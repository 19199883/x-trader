#ifndef __TUNN_RPT_PRODUCER_H__
#define __TUNN_RPT_PRODUCER_H__

#include <array>
#include <string>
#include "vrt_value_obj.h"
#include "signal.h"
#include "EesTraderApi.h"

using namespace std;

#define CANCELLED_FROM_PENDING 99999999

/*
 * 10 power of 2
 */
#define MY_TNL_EXID_SHFE  "SHFE"
#define MY_TNL_EXID_INE  "INE"

struct Tunnconfig
{
	// note diferences between account and user id
	string mac;
	string brokerid;
	string investorid; // account
	string userid;
	string password;
	char exchange;

	// disruptor yield strategy
	char yield[20];
};

struct TunnRpt
{
	char symbol[64];
	int									LocalOrderID;                 ///< 本地委托号
	long long int						SysOrderID;                   ///盛立系统产生的单子号
	if_sig_state_t						OrderStatus;                  ///< 委托状态
	int									MatchedAmount;                ///< 成交数量
	double								TradePrice;					  ///< 成交价格
	int									ErrorID;                      ///< 错误ID
};

class TunnRptProducer: public EESTraderEvent 
{
	public:
		TunnRptProducer(struct vrt_queue  *queue);
		virtual ~TunnRptProducer();

		/*
		 * things relating to X1 Api
		 */
		// 下发指令接口
		int ReqOrderInsert(EES_EnterOrderField *p);
		// 撤单操作请求
		int ReqOrderAction(EES_CancelOrder *p);

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
		 * things relating to femas API
		 */
		/// 连接消息的回调
	
		///	\brief	服务器连接事件
		///	\param  errNo                   连接成功能与否的消息
		///	\param  pErrStr                 错误信息
		///	\return void  
	
	virtual void OnConnection(ERR_NO errNo, const char* pErrStr );
	
	/// 连接断开消息的回调
	
		/// \brief	服务器主动断开，会收到这个消息
		/// \param  ERR_NO errNo         连接成功能与否的消息
		/// \param  const char* pErrStr  错误信息
		/// \return void  
	
	virtual void OnDisConnection(ERR_NO errNo, const char* pErrStr );
	
	/// 登录消息的回调
	
		/// \param  pLogon                  登录成功或是失败的结构
		/// \return void 
	
	virtual void OnUserLogon(EES_LogonResponse* pLogon);
	
	/// 下单被柜台系统接受的事件
	
		/// \brief 表示这个订单已经被柜台系统正式的接受
		/// \param  pAccept	                    订单被接受以后的消息体
		/// \return void 
	
	virtual void OnOrderAccept(EES_OrderAcceptField* pAccept );
	
	/// 下单被市场接受的事件

	    /// \brief 表示这个订单已经被交易所正式的接受
	    /// \param  pAccept	                    订单被接受以后的消息体，里面包含了市场订单ID
	    /// \return void 
	virtual void OnOrderMarketAccept(EES_OrderMarketAcceptField* pAccept);
	
	///	下单被柜台系统拒绝的事件
	
		/// \brief	订单被柜台系统拒绝，可以查看语法检查或是风控检查。 
		/// \param  pReject	                    订单被接受以后的消息体
		/// \return void 
	
	virtual void OnOrderReject(EES_OrderRejectField* pReject );

	///	下单被市场拒绝的事件

	/// \brief	订单被市场拒绝，可以查看语法检查或是风控检查。 
	/// \param  pReject	                    订单被接受以后的消息体，里面包含了市场订单ID
	/// \return void 

	virtual void OnOrderMarketReject(EES_OrderMarketRejectField* pReject);

	///	订单成交的消息事件
	
		/// \brief	成交里面包括了订单市场ID，建议用这个ID查询对应的订单
		/// \param  pExec	                   订单被接受以后的消息体，里面包含了市场订单ID
		/// \return void 
	
	virtual void OnOrderExecution(EES_OrderExecutionField* pExec );
	

	///	订单成功撤销事件
	
		/// \brief	成交里面包括了订单市场ID，建议用这个ID查询对应的订单
		/// \param  pCxled		               订单被接受以后的消息体，里面包含了市场订单ID
		/// \return void 
	
	virtual void OnOrderCxled(EES_OrderCxled* pCxled );

	///	撤单被拒绝的消息事件
	
		/// \brief	一般会在发送撤单以后，收到这个消息，表示撤单被拒绝
		/// \param  pReject	                   撤单被拒绝消息体
		/// \return void 
	
	virtual void OnCxlOrderReject(EES_CxlOrderRej* pReject );

	///	交易所连接状态变化报告，

	/// \brief	当交易所连接发生连接/断开时报告此状态
	/// \param  MarketSessionId: 交易所连接代码
	/// \param  ConnectionGood: true表示交易所连接正常，false表示交易所连接断开了。
	/// \return void 
	virtual void OnMarketSessionStatReport(EES_MarketSessionId MarketSessionId, bool ConnectionGood);

private:
    void ParseConfig();
    void ReqLogin();
	long GetCounterByLocalOrderID(long localorderid);

	struct vrt_producer  *producer_;
	TunnRpt rpt_buffer_[RPT_BUFFER_SIZE];
	const char * module_name_;  
	bool ended_;
	static uint32_t counter_;

	/*
	 * things relating to counter API
	 */
	int32_t Push();

	EESTraderApi *LoadTunnelApi();
	void UnloadTunnelApi();
	void *m_handle;
	funcDestroyEESTraderApi m_distoryFun;
	EESTraderApi *api_;
	EES_TradeSvrInfo api_config_;
};

#endif
