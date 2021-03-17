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
#include "DstarTradeApiError.h"
#include "DstarTradeApiDataType.h"
#include "DstarTradeApiStruct.h"
#include "DstarTradeApi.h"
#include "UdpClient.h"

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
	long						LocalOrderID;       ///< 本地委托号
	DstarApiOrderStateType	    OrderStatus;        ///< 委托状态
	DstarApiQuantityType        MatchedAmount;      ///< 成交数量
	DstarApiPriceType			OrderMatchPrice;				///< 成交价1
	DstarApiErrorCodeType	    ErrorID;            ///< 错误ID
	DstarApiOrderNoType			OrderNo;			///< 委托编码
	DstarApiSystemNoType		SystemNo;       // 系统号
};

class TunnRptProducer: public IDstarTradeSpi
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


		///登录请求响应,错误码为0说明用户登录成功。
		virtual void OnRspUserLogin(const DstarApiRspLoginField *pLoginRsp);
		///UDP认证请求响应,错误码为0说明认证成功。
		 virtual void OnRspUdpAuth(const DstarApiRspUdpAuthField *pRspUdpAuth);
		 ///席位信息响应
		 virtual void OnRspSeat(const DstarApiSeatField* pSeat);

		 ///合约信息响应
		 virtual void OnRspContract(const DstarApiContractField *pContract);

		 ///报单应答
		 virtual void OnRspOrderInsert(const DstarApiRspOrderInsertField *pOrderInsert);

		 ///委托通知 (撤单失败时返回委托通知,委托状态不变,包含撤单失败的错误码)
		virtual void OnRtnOrder(const DstarApiOrderField *pOrder);

		///客户端与通知接口通信连接断开
		virtual void OnFrontDisconnected();

		///错误应答
		virtual void OnRspError(DstarApiErrorCodeType nErrorCode);

	   ///持仓快照响应
	   virtual void OnRspPosition(const DstarApiPositionField *pPosition){};

	   ///资金快照响应
	   virtual void OnRspFund(const DstarApiFundField *pFund){};

	   ///报价响应
	   virtual void OnRspOffer(const DstarApiOfferField *pOffer){};

	   ///成交响应
	   virtual void OnRspMatch(const DstarApiMatchField *pTrade){};

	   ///出入金响应
	   virtual void OnRspCashInOut(const DstarApiCashInOutField *pCashInOut){};

	   ///API准备就绪,只有用户收到此就绪通知时才能进行后续的操作
	   virtual void OnApiReady(const DstarApiSerialIdType nSerialId);

	   ///撤单应答
	   virtual void OnRspOrderDelete(const DstarApiRspOrderDeleteField *pOrderDelete);

	   ///报价应答
	   virtual void OnRspOfferInsert(const DstarApiRspOfferInsertField *pOfferInsert)
	   {};

	   ///最新请求号应答
	   virtual void OnRspLastReqId(const DstaApiRspLastReqIdField *pLastReqId){};

	   ///成交通知
	   virtual void OnRtnMatch(const DstarApiMatchField *pTrade){};

	   ///出入金通知
	   virtual void OnRtnCashInOut(const DstarApiCashInOutField *pCashInOut){};

	   ///报价通知
	   virtual void OnRtnOffer(const DstarApiOfferField *pOffer){};
	           
	   ///询价通知
	   virtual void OnRtnEnquiry(const DstarApiEnquiryField *pEnquiry){};

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
};

#endif
