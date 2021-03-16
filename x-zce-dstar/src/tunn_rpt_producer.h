#ifndef __TUNN_RPT_PRODUCER_H__
#define __TUNN_RPT_PRODUCER_H__

#include <mutex>          // std::mutex, std::lock_guard
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <array>
#include <string>
#include <unordered_map>
#include "vrt_value_obj.h"
#include "TapTradeAPI.h"
#include "TapTradeAPIDataType.h"
#include "EsUdpDataType.h"

#define CANCELLED_FROM_PENDING 99999999

using namespace std;

typedef std::pair<std::string, unsigned short> IPAndPortNum;
typedef std::pair<std::string, std::string> IPAndPortStr;


struct Tunnconfig
{
	string address;
	string brokerid;
	string userid;
	string password;
	char exchange;

	// disruptor yield strategy
	char yield[20];
	char udpserverip[20];
	int udpserverport;
	char UpperChannel[30];
};

struct TunnRpt
{
	TAPIUINT32			    SessionID;
	long				    LocalOrderID;       ///< 本地委托号
	TAPIOrderStateType	    OrderStatus;        ///< 委托状态
	TAPIUINT32		        MatchedAmount;      ///< 成交数量
	TAPIREAL64				OrderMatchPrice;				///< 成交价1
	TAPIUINT32			    ErrorID;            ///< 错误ID
	TAPICHAR				ServerFlag;			///< 服务器标识
	TAPISTR_20				OrderNo;			///< 委托编码
};

class TunnRptProducer: public ITapTradeAPINotify
{
	public:
		TunnRptProducer(struct vrt_queue  *queue);
		~TunnRptProducer();

		// udp order operation
		void AuthUdpServer();
		int InitUdpSequence();
		int NewUdpSequence();
		int InsertUdpOrder(char *udporder);
		int CancelUdpOrder(char *deleteudporder);


		/**
		 * @brief 连接成功回调通知
		 * @ingroup G_T_Login
		 */
		virtual void TAP_CDECL OnConnect();


		///登录请求响应,错误码为0说明用户登录成功。
		virtual void OnRspUserLogin(const DstarApiRspLoginField *pLoginRsp);
		///UDP认证请求响应,错误码为0说明认证成功。
		 virtual void OnRspUdpAuth(const DstarApiRspUdpAuthField *pRspUdpAuth);
		 ///席位信息响应
		 virtual void OnRspSeat(const DstarApiSeatField* pSeat);

		 ///合约信息响应
		 virtual void OnRspContract(const DstarApiContractField *pContract);


		virtual void TAP_CDECL OnAPIReady();

		virtual void TAP_CDECL OnDisconnect(TAPIINT32 reasonCode);

		virtual void TAP_CDECL OnRspChangePassword(TAPIUINT32 sessionID, TAPIINT32 errorCode);

		virtual void TAP_CDECL OnRspSetReservedInfo(TAPIUINT32 sessionID,
					TAPIINT32 errorCode, const TAPISTR_50 info);

		virtual void TAP_CDECL OnRspQryAccount(TAPIUINT32 sessionID, TAPIUINT32 errorCode,
					TAPIYNFLAG isLast, const TapAPIAccountInfo *info);

		virtual void TAP_CDECL OnRspQryFund(TAPIUINT32 sessionID, TAPIINT32 errorCode,
					TAPIYNFLAG isLast, const TapAPIFundData *info);

		virtual void TAP_CDECL OnRtnFund(const TapAPIFundData *info);

		virtual void TAP_CDECL OnRspQryExchange(TAPIUINT32 sessionID,
					TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIExchangeInfo *info);

		virtual void TAP_CDECL OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode,
					TAPIYNFLAG isLast, const TapAPICommodityInfo *info);

		virtual void TAP_CDECL OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode,
					TAPIYNFLAG isLast, const TapAPITradeContractInfo *info);

		virtual void TAP_CDECL OnRtnContract(const TapAPITradeContractInfo *info);

		virtual void TAP_CDECL OnRtnOrder(const TapAPIOrderInfoNotice *info);

		virtual void TAP_CDECL OnRspOrderAction(TAPIUINT32 sessionID, 
					TAPIUINT32 errorCode, const TapAPIOrderActionRsp *info);
		virtual void TAP_CDECL OnRspQryOrder(TAPIUINT32 sessionID,
					TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo *info);
		virtual void TAP_CDECL OnRspQryOrderProcess(TAPIUINT32 sessionID, 
					TAPIINT32 errorCode, TAPIYNFLAG isLast,
			const TapAPIOrderInfo *info);
		virtual void TAP_CDECL OnRspQryFill(TAPIUINT32 sessionID, 
					TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFillInfo *info);

		virtual void TAP_CDECL OnRtnFill(const TapAPIFillInfo *info);
		virtual void TAP_CDECL OnRspQryPosition(TAPIUINT32 sessionID,
					TAPIINT32 errorCode, TAPIYNFLAG isLast,
			const TapAPIPositionInfo *info);
		virtual void TAP_CDECL OnRtnPosition(const TapAPIPositionInfo *info);
		virtual void TAP_CDECL OnRspQryClose(TAPIUINT32 sessionID,
					TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICloseInfo *info);
		virtual void TAP_CDECL OnRtnClose(const TapAPICloseInfo *info);
		virtual void TAP_CDECL OnRtnPositionProfit(const TapAPIPositionProfitNotice *info);
		virtual void TAP_CDECL OnRspQryDeepQuote(TAPIUINT32 sessionID,
					TAPIINT32 errorCode, TAPIYNFLAG isLast,
			const TapAPIDeepQuoteQryRsp *info);

		virtual void TAP_CDECL OnRspQryExchangeStateInfo(TAPIUINT32 sessionID,
					TAPIINT32 errorCode, TAPIYNFLAG isLast,
			const TapAPIExchangeStateInfo * info);
		virtual void TAP_CDECL OnRtnExchangeStateInfo(const TapAPIExchangeStateInfoNotice * info);

		virtual void TAP_CDECL OnRtnReqQuoteNotice(const TapAPIReqQuoteNotice *info) {}
		virtual void TAP_CDECL OnRspUpperChannelInfo(TAPIUINT32 sessionID,
					TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIUpperChannelInfo * info) {}
		virtual void TAP_CDECL OnRspAccountRentInfo(TAPIUINT32 sessionID,
					TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIAccountRentInfo * info) {};
		virtual void TAP_CDECL OnRspSubmitUserLoginInfo(TAPIUINT32 sessionID, 
					TAPIINT32 errorCode, TAPIYNFLAG isLast, 
					const TapAPISubmitUserLoginRspInfo * info);

	virtual void TAP_CDECL OnRspQryBill(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIBillQryRsp *info){};

    virtual void TAP_CDECL OnRspQryAccountStorage(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIAccountStorageInfo* info) {};

    virtual void TAP_CDECL OnRtnAccountStorage(const TapAPIAccountStorageInfo* info){};

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

	int32_t Push();

	/*
	 * 用于通过disruptor传递数据给消费者
	 */
	TunnRpt rpt_buffer_[RPT_BUFFER_SIZE];

	std::mutex mtx_session_localorderid_;

	Tunnconfig config_;
	const char * module_name_;  
	bool ended_;

	/*
	 * things relating to counter API
	 */
    IDstarTradeApi *api_;
	char appid_[30];
	char authcode_[40];

	IPAndPortNum ParseIPAndPortNum(const std::string &addr_cfg);
	IPAndPortStr ParseIPAndPortStr(const std::string &addr_cfg);

	// udp order operation
	int udp_sequence_;
	DstarApiRspLoginField m_LoginInfo;
	DstarApiSeatField m_Seat;
	unordered_map<std::string,unsigned int> contracts_map_;

	int			m_udpFd;
    struct sockaddr_in udpserver_;
	/*
	 * key:session_id; value:counter of LocalOrderID
	 */
	unordered_map<TAPIUINT32,long > session_localorderid_map_;
};

#endif
