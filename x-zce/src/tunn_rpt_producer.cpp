#include <thread>
#include <chrono>
#include <ctime>
#include <ratio>
#include <stdio.h>
#include <fstream>
#include "tunn_rpt_producer.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "esunny_data_formater.h"
#include "my_protocol_packager.h"
#include "TapAPIError.h"
#include "TapTradeAPI.h"
#include "TapDataCollectAPI.h"

using namespace std::chrono;

static std::string ReadAuthCode()
{
    char l[1024];
    std::string auth_code;
    ifstream auth_cfg("auth_code.dat");
    while (auth_cfg.getline(l, 1023)) 
	{
        auth_code.append(l);
    }

    return auth_code;
}

IPAndPortNum TunnRptProducer::ParseIPAndPortNum(const std::string &addr_cfg)
{   
    //format: udp://192.168.60.23:7120   or  tcp://192.168.60.23:7120
    std::string ip_port = addr_cfg.substr(6);
    std::size_t split_pos = ip_port.find(":");
    if ((split_pos == std::string::npos) || (split_pos + 1 >= ip_port.length())) 
	{
        clog_error("parse address failed: %s", addr_cfg.c_str());
        return std::make_pair("", (unsigned short) 0);
    }

    std::string addr_ip = ip_port.substr(0, split_pos);
    std::string addr_port = ip_port.substr(split_pos + 1);
    int port_tmp = atoi(addr_port.c_str());
    if (port_tmp <= 0 || port_tmp > 0xFFFF) 
	{
        clog_error("port in address beyond valid range: %s", addr_cfg.c_str());
        return std::make_pair("", 0);
    }
    
    return std::make_pair(addr_ip, (unsigned short) port_tmp);
}

TunnRptProducer::TunnRptProducer(struct vrt_queue  *queue)
: module_name_("TunnRptProducer")
#ifdef UPD_ORDER_OPERATION
	 , m_UdpCertCode(0),
     m_udpFd(-1)
#endif
{
	ended_ = false;
	memset(rpt_buffer_,0,sizeof(rpt_buffer_));
	
	clog_warning("[%s] RPT_BUFFER_SIZE: %d;", module_name_, RPT_BUFFER_SIZE);
    clog_warning("[%s] TapTradeAPIVersion:%s", module_name_, GetTapTradeAPIVersion());

	this->ParseConfig();

	// TODO: coding for udp
#ifdef UPD_ORDER_OPERATION
	ESUNNYPacker::InitNewUdpOrder(GetAccount(), this->config_.UpperChannel);
	ESUNNYPacker::InitDeleteUdpOrder();
#else
	ESUNNYPacker::InitNewOrder(GetAccount());
    memset(&cancel_req_, 0, sizeof(cancel_req_));
#endif

	struct vrt_producer  *producer = vrt_producer_new("tunnrpt_producer", 1, queue);
	this->producer_ = producer;
	clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0)
	{
		this->producer_ ->yield = vrt_yield_strategy_threaded();
	}
	else if(strcmp(config_.yield, "spin") == 0)
	{
		this->producer_ ->yield = vrt_yield_strategy_spin_wait();
	}
	else if(strcmp(config_.yield, "hybrid") == 0)
	{
		this->producer_ ->yield = vrt_yield_strategy_hybrid();
	}

	char sysInfo[2048];
	int real_size = 0;
	int nver = 0;
	int error = esunny_getsysteminfo(sysInfo,  &real_size, &nver);
	clog_warning("[%s] esunny_getsysteminfo eror:%d; real_size: %d; nver: %d", 
				module_name_, 
				error, 
				real_size,
				nver);

	// create esunny object
    TapAPIApplicationInfo auth_info;
    std::string auth_code = ReadAuthCode();
    clog_warning("[%s] AuthCode: %s",module_name_,auth_code.c_str());
    strncpy(auth_info.AuthCode, auth_code.c_str(), sizeof(auth_info.AuthCode));
    strcpy(auth_info.KeyOperationLogPath, "");
    TAPIINT32 result;
    api_ = CreateTapTradeAPI(&auth_info, result);
    if (!api_ || result != TAPIERROR_SUCCEED) 
	{
        clog_warning("[%s] CreateTapTradeAPI result:%d",module_name_,result);
        return;
    }
    api_->SetAPINotify(this);
	SetTapTradeAPILogLevel(APILOGLEVEL_NONE);
	// address for front machine
	IPAndPortNum ip_port = ParseIPAndPortNum(config_.address);
	api_->SetHostAddress(ip_port.first.c_str(), ip_port.second);
	clog_warning("SetHostAddress, addr: %s:%d", ip_port.first.c_str(), ip_port.second);
    //登录服务器
    TapAPITradeLoginAuth stLoginAuth;
    memset(&stLoginAuth, 0, sizeof(stLoginAuth));
    strcpy(stLoginAuth.UserNo, config_.userid.c_str());
    strcpy(stLoginAuth.Password, config_.password.c_str());
    stLoginAuth.ISModifyPassword = APIYNFLAG_NO;
    stLoginAuth.ISDDA = APIYNFLAG_NO;
	stLoginAuth.NoticeIgnoreFlag = TAPI_NOTICE_IGNORE_FUND | 
									TAPI_NOTICE_IGNORE_POSITIONPROFIT |
									TAPI_NOTICE_IGNORE_FILL | 
									TAPI_NOTICE_IGNORE_POSITION | 
									TAPI_NOTICE_IGNORE_CLOSE | 
									TAPI_NOTICE_IGNORE_POSITIONPROFIT;
	 strcpy(stLoginAuth.AppID, this->appid_);		
	strcpy(stLoginAuth.AuthCode, this->authcode_);
    result = api_->Login(&stLoginAuth);

    clog_warning("[%s] Login: errorCode: %d, %s",
				module_name_,
				result, 
				ESUNNYDatatypeFormater::ToString(&stLoginAuth).c_str());

    if (TAPIERROR_SUCCEED != result) 
	{
        clog_error("[%s] Login Error, result:%d",module_name_,result);
    }
	else
	{
        clog_warning("[%s] Login success", module_name_);
	}
}

TunnRptProducer::~TunnRptProducer()
{
}

void TunnRptProducer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    

	// yield strategy
    TiXmlElement *comp_node = RootElement->FirstChildElement("Disruptor");
	if (comp_node != NULL)
	{
			strcpy(config_.yield, comp_node->Attribute("yield"));
	} 
	else 
	{ 
		clog_error("[%s] x-trader.config error: Disruptor node missing.", module_name_); 
	}

    TiXmlElement *tunn_node = RootElement->FirstChildElement("Tunnel");
	if (tunn_node != NULL)
	{
		this->config_.address = tunn_node->Attribute("address");
		this->config_.brokerid = tunn_node->Attribute("brokerid");
		this->config_.userid = tunn_node->Attribute("userid");
		this->config_.password = tunn_node->Attribute("password");
		strcpy(this->appid_, tunn_node->Attribute("appid"));
		strcpy(this->authcode_, tunn_node->Attribute("authcode"));

		// TODO: coding for udp config
#ifdef UPD_ORDER_OPERATION
		strcpy(this->config_.udpserverip, tunn_node->Attribute("udpserverip"));
		this->config_.udpserverport = atoi(tunn_node->Attribute("udpserverport"));
		strcpy(this->config_.UpperChannel, tunn_node->Attribute("upperchannel"));

		socklen_t len = sizeof (udpserver_);
		udpserver_.sin_family = AF_INET;
		udpserver_.sin_port = htons(this->config_.udpserverport); ///server的监听端口
		udpserver_.sin_addr.s_addr = inet_addr(this->config_.udpserverip); ///server的地址
		clog_warning("[%s] udp server ip: %s; udp server port: %d; upperchannel: %s",
					module_name_, 
					this->config_.udpserverip,
					this->config_.udpserverport,
					this->config_.UpperChannel);
#endif

		clog_warning("[%s] tunn config:address:%s; brokerid:%s; userid:%s; password:%s; " ,
					module_name_, 
					this->config_.address.c_str(), 
					this->config_.brokerid.c_str(),
					this->config_.userid.c_str(),
					this->config_.password.c_str());
	} 
	else 
	{ 
		clog_error("[%s] x-trader.config error: Tunnel node missing.", module_name_); 
	}

}

void TunnRptProducer::OnConnect()
{
    clog_warning("[%s] OnConnect.", module_name_);
}

void TunnRptProducer::OnRspLogin(TAPIINT32 errorCode, const TapAPITradeLoginRspInfo* loginRspInfo)
{
    clog_warning("[%s] OnRspLogin: errorCode: %d, %s",
        module_name_,
		errorCode, 
		ESUNNYDatatypeFormater::ToString(loginRspInfo).c_str());
	fflush (Log::fp);

	// TODO: coding
	if(0 != errorCode)
	{
		clog_error("[%s] OnRspLogin: errorCode: %d, %s",
			module_name_,
			errorCode, 
			ESUNNYDatatypeFormater::ToString(loginRspInfo).c_str());
		fflush (Log::fp);
	}
	else
	{
#ifdef UPD_ORDER_OPERATION
		m_UdpCertCode = loginRspInfo->UdpCertCode;
		AuthUdpServer();
#endif
	}
}

void TunnRptProducer::OnAPIReady()
{
    clog_warning("[%s] OnAPIReady",module_name_);	
}

void TunnRptProducer::OnDisconnect(TAPIINT32 reasonCode)
{
    clog_error("[%s] OnDisconnect, reasonCode:%d", module_name_, reasonCode);
}

void TunnRptProducer::OnRspChangePassword(TAPIUINT32 sessionID, TAPIINT32 errorCode)
{
    clog_warning("[%s] OnRspChangePassword.", module_name_);
}

void TunnRptProducer::OnRspSetReservedInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			const TAPISTR_50 info)
{
    clog_warning("[%s] OnRspSetReservedInfo.", module_name_);
}

void TunnRptProducer::OnRspQryAccount(TAPIUINT32 sessionID, TAPIUINT32 errorCode,
			TAPIYNFLAG isLast, const TapAPIAccountInfo* info)
{
    clog_info("[%s] OnRspQryAccount.", module_name_);
}

void TunnRptProducer::OnRspQryFund(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			TAPIYNFLAG isLast, const TapAPIFundData* info) { }

void TunnRptProducer::OnRtnFund(const TapAPIFundData* info) { }

void TunnRptProducer::OnRspQryExchange(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			TAPIYNFLAG isLast, const TapAPIExchangeInfo* info) { }

void TunnRptProducer::OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			TAPIYNFLAG isLast, const TapAPICommodityInfo* info) { }

void TunnRptProducer::OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, 
			TAPIYNFLAG isLast, const TapAPITradeContractInfo* info)
{
    clog_warning("[%s] OnRspQryContract: sessionID:%u, errorCode:%d, isLast:%c, %s",
        module_name_,
		sessionID, 
		errorCode, 
		isLast, 
		ESUNNYDatatypeFormater::ToString(info).c_str());
}

void TunnRptProducer::OnRtnContract(const TapAPITradeContractInfo* info)
{
    clog_warning("[%s] OnRtnContract.", module_name_);
}

void TunnRptProducer::End()
{
	if(!ended_)
	{ 
		if (api_) 
		{
			FreeTapTradeAPI(api_);
			api_ = NULL;
		}
		ended_ = true;
		(vrt_producer_eof(producer_));
	}

	fflush (Log::fp);
}

void TunnRptProducer::OnRtnOrder(const TapAPIOrderInfoNotice* info)
{
    clog_info("[%s] OnRtnOrder:%s",
				module_name_, 
				ESUNNYDatatypeFormater::ToString(info).c_str());

	if (ended_) return;

	// TODO: udp version need not session
#ifdef UPD_ORDER_OPERATION
	// TODO: note that udp version how to get local order id
	long localorderid = atol(info->OrderInfo->ClientOrderNo);
#else
	bool session_found = false;
	while(!session_found)
	{
		{
			std::lock_guard<std::mutex> lck (mtx_session_localorderid_);
			auto it = session_localorderid_map_.find(info->SessionID);
			if(it == session_localorderid_map_.end())
			{
				// api_->InsertOrder函数返回，有时会后于OnRtnOrder到达
				session_found = false;
			}
			else
			{
				session_found = true;
			}
		}

		if (!session_found)
		{
			clog_error("[%s] can not find localorderid by session:%u,err:%u",module_name_, 
				info->SessionID,info->ErrorCode);
			std::this_thread::sleep_for (std::chrono::milliseconds(5));
		}
	}
	long localorderid = session_localorderid_map_[info->SessionID];
#endif

	int32_t cursor = Push();
	struct TunnRpt &tunnrpt = rpt_buffer_[cursor];
	tunnrpt.SessionID = info->SessionID;
	tunnrpt.LocalOrderID = localorderid;
	tunnrpt.ServerFlag = info->OrderInfo->ServerFlag;
	strcpy(tunnrpt.OrderNo,info->OrderInfo->OrderNo);

	if (info->OrderInfo->OrderState==TAPI_ORDER_STATE_SUBMIT ||
		info->OrderInfo->OrderState==TAPI_ORDER_STATE_QUEUED ||
		info->OrderInfo->OrderState==TAPI_ORDER_STATE_CANCELING
		) 
	{// discard these reports
		return;
	}

	tunnrpt.MatchedAmount = info->OrderInfo->OrderMatchQty;
	tunnrpt.OrderMatchPrice = info->OrderInfo->OrderMatchPrice;
	tunnrpt.ErrorID = info->OrderInfo->ErrorCode;
	tunnrpt.OrderStatus = info->OrderInfo->OrderState;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	vrt_producer_claim(producer_, &vvalue);
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = cursor;
	ivalue->data = TUNN_RPT;
	vrt_producer_publish(producer_);

	if(tunnrpt.OrderStatus==TAPI_ORDER_STATE_FAIL ||
		tunnrpt.ErrorID != TAPIERROR_SUCCEED)
	{
		clog_error("[%s] OnRtnOrder:%s",
					module_name_, 
					ESUNNYDatatypeFormater::ToString(info).c_str());
	}

	clog_info("[%s] cursor:%d,OnRtnOrder:%s",
				module_name_,
				cursor, 
				ESUNNYDatatypeFormater::ToString(info).c_str());
}

// 该接口目前没有用到，所有操作结果通过OnRtnOrder返回.
// log this info only to see whether this method will be invoked.
void TunnRptProducer::OnRspOrderAction(TAPIUINT32 sessionID, TAPIUINT32 errorCode,
			const TapAPIOrderActionRsp* info)
{
    clog_info("[%s] OnRspOrderAction:sessionID:%u,errorCode:%d, %s", 
				module_name_,
				sessionID, 
				errorCode, 
				ESUNNYDatatypeFormater::ToString(info).c_str());
}

void TunnRptProducer::OnRspQryOrder(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			TAPIYNFLAG isLast, const TapAPIOrderInfo* info)
{
}

void TunnRptProducer::OnRspQryOrderProcess(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			TAPIYNFLAG isLast,
    const TapAPIOrderInfo* info)
{
}

void TunnRptProducer::OnRspQryFill(TAPIUINT32 sessionID, TAPIINT32 errorCode, 
			TAPIYNFLAG isLast, const TapAPIFillInfo* info) { }

// discard this info
void TunnRptProducer::OnRtnFill(const TapAPIFillInfo* info) { }

void TunnRptProducer::OnRspQryPosition(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			TAPIYNFLAG isLast,
    const TapAPIPositionInfo* info) { }

void TunnRptProducer::OnRtnPosition(const TapAPIPositionInfo* info) { }

void TunnRptProducer::OnRspQryClose(TAPIUINT32 sessionID, TAPIINT32 errorCode, 
			TAPIYNFLAG isLast, const TapAPICloseInfo* info) { }

void TunnRptProducer::OnRtnClose(const TapAPICloseInfo* info) { }

void TunnRptProducer::OnRtnPositionProfit(const TapAPIPositionProfitNotice* info) { }

void TunnRptProducer::OnRspQryDeepQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			TAPIYNFLAG isLast,
    const TapAPIDeepQuoteQryRsp* info) { }

void TunnRptProducer::OnRspQryExchangeStateInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode,
			TAPIYNFLAG isLast, const TapAPIExchangeStateInfo* info) {
}

void TunnRptProducer::OnRtnExchangeStateInfo(const TapAPIExchangeStateInfoNotice* info)
{
}

void TunnRptProducer::OnRspSubmitUserLoginInfo(TAPIUINT32 sessionID, 
			TAPIINT32 errorCode, 
			TAPIYNFLAG isLast, 
			const TapAPISubmitUserLoginRspInfo * info)
{
    clog_warning("[%s] OnRspSubmitUserLoginInfo before.", module_name_);
	fflush (Log::fp);


    clog_warning("[%s] OnRspSubmitUserLoginInfo: errorCode: %d, %s",
				module_name_,
				errorCode, 
				ESUNNYDatatypeFormater::ToString(info).c_str());
    clog_warning("[%s] OnRspSubmitUserLoginInfo end.", module_name_);
	fflush (Log::fp);
}

long TunnRptProducer::NewLocalOrderID(int32_t strategyid)
{
	static int32_t counter = 1;
    long localorderid = strategyid+ counter * 1000;		
	counter++;

	return localorderid;
}

const char* TunnRptProducer::GetAccount()
{
	return config_.userid.c_str();
}

TunnRpt* TunnRptProducer::GetRpt(int32_t index)
{
	return &(rpt_buffer_[index]);
}

int32_t TunnRptProducer::GetStrategyID(TunnRpt& rpt)
{
	return rpt.LocalOrderID % 1000;
}

int32_t TunnRptProducer::GetCounterByLocalOrderID(long local_ord_id)
{
	return local_ord_id/1000;
}

int32_t TunnRptProducer::Push()
{
	static int32_t cursor = RPT_BUFFER_SIZE - 1;
	cursor++;
	if (cursor%RPT_BUFFER_SIZE == 0){
		cursor = 0;
	}

	return cursor;
}

#ifdef UPD_ORDER_OPERATION
// TODO: coding for udp version
void TunnRptProducer::AuthUdpServer()
{
	m_udpFd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    char sendbuf[sizeof (TapAPIUdpHead) + sizeof (TapAPIUdpAuthReq)];
    TapAPIUdpHead* pHead = (TapAPIUdpHead*) sendbuf;
    pHead->PackageFlag = UDP_Package_Flag;
    pHead->ProtocolCode = CMD_UDPAuth_Req;
    pHead->Sequence = InitUdpSequence();
    pHead->Version = 1;
    TapAPIUdpAuthReq* pAuth = (TapAPIUdpAuthReq*)(sendbuf + sizeof (TapAPIUdpHead));
    pAuth->AuthCode = m_UdpCertCode;
    strncpy(pAuth->UserNo, config_.userid.c_str(), sizeof (pAuth->UserNo) - 1);

	socklen_t len = sizeof (udpserver_);
    if (sendto(m_udpFd, 
					sendbuf, 
					sizeof(sendbuf), 
					0, 
					(struct sockaddr*)&udpserver_, 
					len) != -1)
    {
        char recvBuf[2048];
        if (recvfrom(m_udpFd, 
						recvBuf, 
						sizeof(recvBuf), 
						0, 
						(struct sockaddr*)&udpserver_, 
						&len) != -1)
        {
            TapAPIUdpHead* pHead = (TapAPIUdpHead*) recvBuf;
			clog_info("[%s] ProtocolCode: %hu;", module_name_, pHead->ProtocolCode);
        }
    }
}

// TODO: coding for udp version
int TunnRptProducer::InitUdpSequence()
{
	udp_sequence_ = 1;
	return udp_sequence_;
}

// TODO: coding for udp version
int TunnRptProducer::NewUdpSequence()
{
	udp_sequence_++;
	return udp_sequence_;
}

// TODO: coding for udp version
int TunnRptProducer::InsertUdpOrder(char *udporder)
{
    TapAPIUdpHead* pHead = (TapAPIUdpHead*)udporder;
    pHead->Sequence = NewUdpSequence();
    
	socklen_t len = sizeof (udpserver_);
    if (sendto(m_udpFd, 
					udporder, 
					UDP_ORDER_INSERT_LEN, 
					0, 
					(struct sockaddr*)&udpserver_, 
					len) != -1)
    {
        char recvBuf[2048];
        if (recvfrom(m_udpFd, 
						recvBuf, 
						sizeof (recvBuf), 
						0, 
						(struct sockaddr*)&udpserver_, 
						&len) != -1)
        {
            TapAPIUdpHead* pHead = (TapAPIUdpHead*) recvBuf;
			clog_info("[%s] ProtocolCode: %hu;", module_name_, pHead->ProtocolCode);
        }
    }

	return 0;
}

// TODO: coding for udp version
int TunnRptProducer::CancelUdpOrder(char *deleteudporder)
{
    TapAPIUdpHead* pHead = (TapAPIUdpHead*)deleteudporder;
    pHead->Sequence = NewUdpSequence();
    
	socklen_t len = sizeof (udpserver_);
    if (sendto(m_udpFd, 
					deleteudporder, 
					UDP_ORDER_DELETE_LEN, 
					0, 
					(struct sockaddr*)&udpserver_, 
					len) != -1)
    {
        char recvBuf[2048];
        if (recvfrom(m_udpFd, 
						recvBuf, 
						sizeof (recvBuf), 
						0, 
						(struct sockaddr*)&udpserver_, 
						&len) != -1)
        {
            TapAPIUdpHead* pHead = (TapAPIUdpHead*) recvBuf;
			clog_info("[%s] ProtocolCode: %hu;", module_name_, pHead->ProtocolCode);
        }
    }

	return 0;
}
#else

int TunnRptProducer::ReqOrderInsert(int32_t localorderid,TAPIUINT32 *session, TapAPINewOrder *p)
{
#ifdef LATENCY_MEASURE
	high_resolution_clock::time_point t0 = high_resolution_clock::now();
#endif
	clog_info("[%s] ReqInsertOrder-:%s", module_name_, ESUNNYDatatypeFormater::ToString(p).c_str());
	int ret = api_->InsertOrder(session,p);
	{
		std::lock_guard<std::mutex> lck (mtx_session_localorderid_);
		session_localorderid_map_[*session] = localorderid;
	}
#ifdef LATENCY_MEASURE
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		int latency = (t1.time_since_epoch().count() - t0.time_since_epoch().count()) / 1000;	
		clog_warning("[%s] ReqOrderInsert latency:%d us", 
					module_name_,latency); 
#endif
	if (ret != 0){
		//因为穿透版，权限问题采集信息不全，临时方案 
		if(TAPIERROR_API_NotReady == ret) ret = 0;

		clog_error("[%s] ReqInsertOrder - return:%d, session_id:%u,localorderid:%d",
				module_name_,ret, *session,localorderid);
	}
	else 
	{
		clog_info("[%s] ReqInsertOrder - return:%d, session_id:%u,localorderid:%d",
				module_name_,
				ret, 
				*session,
				localorderid);
	}

	return ret;
}

// TODO: coding for udp version
//

int TunnRptProducer::ReqOrderAction(TAPICHAR serverflag, const char* orderno)
{
#ifdef LATENCY_MEASURE
	high_resolution_clock::time_point t0 = high_resolution_clock::now();
#endif
	TAPIUINT32 sessionID;
    cancel_req_.ServerFlag = serverflag;
    strcpy(cancel_req_.OrderNo, orderno);

	clog_info("[%s] ReqOrderAction-:%s", 
				module_name_, 
				ESUNNYDatatypeFormater::ToString(&cancel_req_).c_str());

	clog_info("[%s]before ReqOrderAction", module_name_);

	int ret = api_->CancelOrder(&sessionID, &cancel_req_);

	clog_info("[%s]after ReqOrderAction", module_name_);

#ifdef LATENCY_MEASURE
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		int latency = (t1.time_since_epoch().count() - t0.time_since_epoch().count()) / 1000;	
		clog_warning("[%s] ReqOrderAction latency:%d us", 
					module_name_,latency); 
#endif

	if (ret != 0)
	{
		clog_error("[%s] CancelOrder - return:%d, session_id:%u, "
			"server flag:%c,order no:%s", 
			module_name_,
			ret,
			sessionID,
			cancel_req_.ServerFlag,
			cancel_req_.OrderNo);
	} 
	else 
	{
		clog_info("[%s] CancelOrder - return:%d, session_id:%u, "
			"server flag:%c,order no:%s", 
			module_name_,
			ret,
			sessionID,
			cancel_req_.ServerFlag,
			cancel_req_.OrderNo);
	}

	return ret;
}
#endif
