#ifndef __TUNN_RPT_PRODUCER_H__
#define __TUNN_RPT_PRODUCER_H__

#include <array>
#include <string>
#include <unordered_map>
#include "vrt_value_obj.h"
#include "TapTradeAPI.h"

using namespace std;


typedef std::pair<std::string, unsigned short> IPAndPortNum;
IPAndPortNum ParseIPAndPortNum(const std::string &addr_cfg);

typedef std::pair<std::string, std::string> IPAndPortStr;
IPAndPortStr ParseIPAndPortStr(const std::string &addr_cfg);

IPAndPortNum ParseIPAndPortNum(const std::string &addr_cfg)
{   
    //format: udp://192.168.60.23:7120   or  tcp://192.168.60.23:7120
    std:::string ip_port = addr_cfg.substr(6);
    std::size_t split_pos = ip_port.find(":");
    if ((split_pos == std::string::npos) || (split_pos + 1 >= ip_port.length())) {
        clog_error("parse address failed: %s", addr_cfg.c_str());
        return std::make_pair("", (unsigned short) 0);
    }

    std::string addr_ip = ip_port.substr(0, split_pos);
    std::string addr_port = ip_port.substr(split_pos + 1);
    int port_tmp = atoi(addr_port.c_str());
    if (port_tmp <= 0 || port_tmp > 0xFFFF) {
        clog_error("port in address beyond valid range: %s", addr_cfg.c_str());
        return std::make_pair("", 0);
    }
    
    return std::make_pair(addr_ip, (unsigned short) port_tmp);
}

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
	TAPIUINT32			    SessionID;
	long				    LocalOrderID;       ///< 本地委托号
	TAPIOrderStateType	    OrderStatus;        ///< 委托状态
	TAPIUINT32		        MatchedAmount;      ///< 成交数量
	TAPIUINT32			    ErrorID;            ///< 错误ID
	TAPICHAR				ServerFlag;			///< 服务器标识
	TAPISTR_20				OrderNo;			///< 委托编码
};

class TunnRptProducer: public ITapTradeAPINotify
{
	public:
		TunnRptProducer(struct vrt_queue  *queue);
		~TunnRptProducer();

		/*
		 * things relating to esunny Api
		 */
		/*
		 * localorderid:新委托单的LocalOrderID
		 */
		int ReqOrderInsert(int32_t localorderid,TAPIUINT32 *session_id, TapAPINewOrder *p);
		/*
		 * counter:要撤单的委托单的counter of LocalOrderID
		 */
		int ReqOrderAction(int32_t counter);

		/**
		 * @brief 连接成功回调通知
		 * @ingroup G_T_Login
		 */
		virtual void TAP_CDECL OnConnect();

		/**
		 * @brief    系统登录过程回调。
		 * @details  此函数为Login()登录函数的回调，调用Login()成功后建立了链路连接，
		 * 然后API将向服务器发送登录认证信息，
		 *           登录期间的数据发送情况和登录的回馈信息传递到此回调函数中。
		 * @param[in] errorCode 返回错误码,0表示成功。
		 * @param[in] loginRspInfo 登陆应答信息，如果errorCode!=0，则loginRspInfo=NULL。
		 * @attention    该回调返回成功，说明用户登录成功。但是不代表API准备完毕。
		 * @ingroup G_T_Login
		 */
		virtual void TAP_CDECL OnRspLogin(TAPIINT32 errorCode, 
					const TapAPITradeLoginRspInfo *loginRspInfo);

		/**
		 * @brief    通知用户API准备就绪。
		 * @details  只有用户回调收到此就绪通知时才能进行后续的各种行情数据查询操作。\n
		 *           此回调函数是API能否正常工作的标志。
		 * @attention 就绪后才可以进行后续正常操作
		 * @ingroup G_T_Login
		 */
		virtual void TAP_CDECL OnAPIReady();

		/**
		 * @brief    API和服务失去连接的回调
		 * @details  在API使用过程中主动或者被动与服务器服务失去连接后都会触发此回调通知用户与服务器的连接已经断开。
		 * @param[in] reasonCode 断开原因代码。
		 * @ingroup G_T_Disconnect
		 */
		virtual void TAP_CDECL OnDisconnect(TAPIINT32 reasonCode);

		/**
		 * @brief 通知用户密码修改结果
		 * @param[in] sessionID 修改密码的会话ID,和ChangePassword返回的会话ID对应。
		 * @param[in] errorCode 返回错误码，0表示成功。
		 * @ingroup G_T_UserInfo
		 */
		virtual void TAP_CDECL OnRspChangePassword(TAPIUINT32 sessionID, TAPIINT32 errorCode);

		/**
		 * @brief 设置用户预留信息反馈
		 * @param[in] sessionID 设置用户预留信息的会话ID
		 * @param[in] errorCode 返回错误码，0表示成功。
		 * @param[in] info 指向返回的信息结构体。当errorCode不为0时，info为空。
		 * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
		 * @ingroup G_T_UserInfo
		 */
		virtual void TAP_CDECL OnRspSetReservedInfo(TAPIUINT32 sessionID,
					TAPIINT32 errorCode, const TAPISTR_50 info);

		/**
		 * @brief    返回用户信息
		 * @details  此回调接口向用户返回查询的资金账号的详细信息。用户有必要将得到的账
		 * 号编号保存起来，然后在后续的函数调用中使用。
		 * @param[in] sessionID 请求的会话ID；
		 * @param[in] errorCode 错误码。0 表示成功。
		 * @param[in] isLast 标示是否是最后一批数据；
		 * @param[in] info 指向返回的信息结构体。当errorCode不为0时，info为空。
		 * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
		 * @ingroup G_T_AccountInfo
		 */
		virtual void TAP_CDECL OnRspQryAccount(TAPIUINT32 sessionID, TAPIUINT32 errorCode,
					TAPIYNFLAG isLast, const TapAPIAccountInfo *info);

		/**
		 * @brief 返回资金账户的资金信息
		 * @param[in] sessionID 请求的会话ID；
		 * @param[in] errorCode 错误码。0 表示成功。
		 * @param[in] isLast     标示是否是最后一批数据；
		 * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
		 * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
		 * @ingroup G_T_AccountDetails
		 */
		virtual void TAP_CDECL OnRspQryFund(TAPIUINT32 sessionID, TAPIINT32 errorCode,
					TAPIYNFLAG isLast, const TapAPIFundData *info);

		/**
		 * @brief    用户资金变化通知
		 * @details  用户的委托成交后会引起资金数据的变化，因此需要向用户实时反馈。
		 * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
		 * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
		 * @ingroup G_T_AccountDetails
		 */
		virtual void TAP_CDECL OnRtnFund(const TapAPIFundData *info);

		/**
		 * @brief 返回系统中的交易所信息
		 * @param[in] sessionID 请求的会话ID；
		 * @param[in] errorCode 错误码。0 表示成功。
		 * @param[in] isLast     标示是否是最后一批数据；
		 * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
		 * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
		 * @ingroup G_T_TradeSystem
		 */
		virtual void TAP_CDECL OnRspQryExchange(TAPIUINT32 sessionID,
					TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIExchangeInfo *info);

		/**
		 * @brief    返回系统中品种信息
		 * @details  此回调接口用于向用户返回得到的所有品种信息。
		 * @param[in] sessionID 请求的会话ID，和GetAllCommodities()函数返回对应；
		 * @param[in] errorCode 错误码。0 表示成功。
		 * @param[in] isLast     标示是否是最后一批数据；
		 * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
		 * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
		 * @ingroup G_T_Commodity
		 */
		virtual void TAP_CDECL OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode,
					TAPIYNFLAG isLast, const TapAPICommodityInfo *info);

		/**
		 * @brief 返回系统中合约信息
		 * @param[in] sessionID 请求的会话ID；
		 * @param[in] errorCode 错误码。0 表示成功。
		 * @param[in] isLast     标示是否是最后一批数据；
		 * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
		 * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
		 * @ingroup G_T_Contract
		 */
		virtual void TAP_CDECL OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode,
					TAPIYNFLAG isLast, const TapAPITradeContractInfo *info);

		/**
		 * @brief    返回新增合约信息
		 * @details  向用户推送新的合约。主要用来处理在交易时间段中服务器添加了新合约时，
		 * 向用户发送这个合约的信息。
		 * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
		 * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
		 * @ingroup G_T_Contract
		 */
		virtual void TAP_CDECL OnRtnContract(const TapAPITradeContractInfo *info);

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
		virtual void TAP_CDECL OnRspOrderAction(TAPIUINT32 sessionID, 
					TAPIUINT32 errorCode, const TapAPIOrderActionRsp *info);

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
		virtual void TAP_CDECL OnRspQryOrder(TAPIUINT32 sessionID,
					TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo *info);
		/**
		 * @brief 返回查询的委托变化流程信息
		 * @param[in] sessionID 请求的会话ID；
		 * @param[in] errorCode 错误码，当errorCode==0时，info指向返回的委托
		 * 变化流程结构体，不然为NULL；
		 * @param[in] isLast 标示是否是最后一批数据；
		 * @param[in] info 返回的委托变化流程指针。
		 * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
		 * @ingroup G_T_TradeInfo
		 */
		virtual void TAP_CDECL OnRspQryOrderProcess(TAPIUINT32 sessionID, 
					TAPIINT32 errorCode, TAPIYNFLAG isLast,
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
		virtual void TAP_CDECL OnRspQryFill(TAPIUINT32 sessionID, 
					TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFillInfo *info);

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
		virtual void TAP_CDECL OnRspQryPosition(TAPIUINT32 sessionID,
					TAPIINT32 errorCode, TAPIYNFLAG isLast,
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
		virtual void TAP_CDECL OnRspQryClose(TAPIUINT32 sessionID,
					TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICloseInfo *info);
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
		virtual void TAP_CDECL OnRspQryDeepQuote(TAPIUINT32 sessionID,
					TAPIINT32 errorCode, TAPIYNFLAG isLast,
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
		virtual void TAP_CDECL OnRspQryExchangeStateInfo(TAPIUINT32 sessionID,
					TAPIINT32 errorCode, TAPIYNFLAG isLast,
			const TapAPIExchangeStateInfo * info);
		/**
		 * @brief 交易所时间状态信息通知
		 * @param[in] info       指向返回的信息结构体。当errorCode不为0时，info为空。
		 * @attention  不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
		 * @ingroup G_T_TradeSystem
		 */
		virtual void TAP_CDECL OnRtnExchangeStateInfo(const TapAPIExchangeStateInfoNotice * info);

		virtual void TAP_CDECL OnRtnReqQuoteNotice(const TapAPIReqQuoteNotice *info) {}
		virtual void TAP_CDECL OnRspUpperChannelInfo(TAPIUINT32 sessionID,
					TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIUpperChannelInfo * info) {}
		virtual void TAP_CDECL OnRspAccountRentInfo(TAPIUINT32 sessionID,
					TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIAccountRentInfo * info) {};


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
    void ParseConfig();

	/*
	 * things relating to x-trader internal logic
	 */
	struct vrt_producer  *producer_;
	/* 有2各用途：
	 * 1.缓存所有的委托报告信息，用于撤单使用。访问一个委托单的报告，
	 * 通过该委托单的LocalOrderID的counter作为数组下标
	 * 2.用于通过disruptor传递数据给消费者
	 */
	TunnRpt rpt_buffer_[RPT_BUFFER_SIZE];

	Tunnconfig config_;
	const char * module_name_;  
	bool ended_;
	/*
	 * key:session_id; value:counter of LocalOrderID
	 */
	unordered_map<TAPIUINT32,long > session_localorderid_map_;

	/*
	 * 撤单请求，避免每次初始化
	 */
	TapAPIOrderCancelReq cancel_req_;

	/*
	 * true: cancel request; false; place request
	 * 请求与在数组存储位置的关系：RequestID与LocalOrderID相同，
	 * RequestID的counter部分作为数组下标
	 */
	bool cancel_requests_[COUNTER_UPPER_LIMIT];

	/*
	 * things relating to counter API
	 */
    ITapTradeAPI *api_;
};

#endif
