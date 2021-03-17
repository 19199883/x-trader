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
	 , m_UdpCertCode(0),
     m_udpFd(-1)
{
	ended_ = false;
	memset(rpt_buffer_,0,sizeof(rpt_buffer_));
	
	clog_warning("[%s] RPT_BUFFER_SIZE: %d;", module_name_, RPT_BUFFER_SIZE);
    clog_warning("[%s] TapTradeAPIVersion:%s", module_name_, GetApiVersion());

	this->ParseConfig();

	ESUNNYPacker::InitNewUdpOrder(GetAccount(), this->config_.UpperChannel);
	ESUNNYPacker::InitDeleteUdpOrder();

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

	IPAndPortNum ip_port = ParseIPAndPortNum(config_.address);
	//创建API
	// create esunny object
    TAPIINT32 result;
    api_ = CreateDstarTradeApi();
    if (!api_) 
	{
        clog_error("[%s] CreateTapTradeAPI faild.", module_name_);
        return;
    }
	else
	{
        clog_error("[%s] CreateTapTradeAPI suceeded.", module_name_);
	}

	// address for front machine
	api_->RegisterSpi(this);
	api_->RegisterFrontAddress(ip_port.first.c_str(), ip_port.second);
	char log_path[] = "./";
	api_->SetApiLogPath(log_path);
	
	DstarApiReqLoginField loginReq;
    memset(&loginReq, 0, sizeof(loginReq));
	strncpy(loginReq.AccountNo, 
				config_.userid.c_str(), 
				sizeof(DstarApiAccountNoType) - 1);
	strncpy(loginReq.Password, 
				config_.password.c_str(), 
				sizeof(DstarApiPasswdType) - 1);
	strncpy(loginReq.AppId, 
				this->appid_, 
				sizeof(DstarApiAppIdType) - 1);
	strncpy(loginReq.LicenseNo, 
				this->authcode_, 
				sizeof(DstarApiLicenseNoType) - 1);
	api_->SetLoginInfo(&loginReq);
	api_->SetCpuId(0, -1);
	api_->SetSubscribeStartId(-1);

	char systeminfo[1024] = {0};
	int nLen = 1024;
	unsigned int nVersion = 0;
	int ret = api_->GetSystemInfo(systeminfo, &nLen, &nVersion);
	clog_warning("[%s] GetSystemInfo, ret:%d, len:%d, version:%d\n", 
				module_name_,
				ret, 
				nLen, 
				nVersion);

	DstarApiSubmitInfoField pSubmitInfo = {0};
	strncpy(pSubmitInfo.AccountNo, 
				config_.userid.c_str(), 
				sizeof(DstarApiAccountNoType) - 1);
	pSubmitInfo.AuthType = DSTAR_API_AUTHTYPE_DIRECT;
	pSubmitInfo.AuthKeyVersion = nVersion;
	memcpy(pSubmitInfo.SystemInfo, systeminfo, nLen);
	strncpy(pSubmitInfo.LicenseNo, 
				this->authcode_, 
				sizeof(DstarApiLicenseNoType) - 1);
	strncpy(pSubmitInfo.ClientAppId, 
				this->appid_, 
				sizeof(DstarApiAppIdType) - 1);
	api_->SetSubmitInfo(&pSubmitInfo);

	if (api_->Init(DSTAR_API_INIT_QUERY) < 0)
	{
		clog_error("[%s] udp client init failed.", module_name_);
	}
	else
	{
		clog_warning("[%s] udp client init succeeded.", module_name_);
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

// ok
void TunnRptProducer::OnRspUdpAuth(const DstarApiRspUdpAuthField *p)
{
	clog_warning("[%s] %s",
		module_name_,
		ESUNNYDatatypeFormater::ToString(p).c_str());

	if (p->ErrorCode == 0)
	{
		// m_bUdpAuth = true;
	}
	else
	{
		clog_error("[%s] udpauth failed, %s",
			module_name_,
			ESUNNYDatatypeFormater::ToString(p).c_str());
	}

	fflush (Log::fp);
}

// ok
void TunnRptProducer::OnRspContract(const DstarApiContractField *pContract)
{
	// TODO: 观看合约索引的内容
	clog_warning("[%s] ",
		module_name_,
		ESUNNYDatatypeFormater::ToString(pContract).c_str());
	fflush (Log::fp);

	if(strlen(pContract->ContractNo) > 6)
	{
		clog_warning("[%s] discard contract: %s", 
					module_name_,
					pContract->ContractNo);
	}
	else
	{
		clog_info("[%s] add contract: %s", 
					module_name_,
					pContract->ContractNo);
		contracts_map_[pContract->ContractNo] = pContract->ContractIndex;
	}
}

// ok
void TunnRptProducer::OnRspSeat(const DstarApiSeatField* pSeat)
{
	// TODO: 观看席位
	//
	clog_warning("[%s] ",
		module_name_,
		ESUNNYDatatypeFormater::ToString(pSeat).c_str());
	fflush (Log::fp);
	m_Seat= *pSeat; 
}

// ok
void TunnRptProducer::OnRspOrderDelete(const DstarApiRspOrderDeleteField *pOrderDelete)
{
	clog_warning("[%s] OnRspOrderDelete:%s",
		module_name_,
		ESUNNYDatatypeFormater::ToString(
			(DstarApiRspOrderInsertField*)pOrderDelete).c_str());
}

// ok
void TunnRptProducer::OnRspUserLogin(const DstarApiRspLoginField *pLoginRsp)
{
	m_LoginInfo = *pLoginRsp;
	clog_warning("[%s] OnRspLogin: errorCode: %d, %s",
		module_name_,
		m_LoginInfo.ErrorCode, 
		ESUNNYDatatypeFormater::ToString(m_LoginInfo).c_str());
	fflush (Log::fp);

	if(0 != m_LoginInfo.ErrorCode)
	{
		clog_error("[%s] OnRspLogin: errorCode: %d, %s",
			module_name_,
			m_LoginInfo.ErrorCode, 
			ESUNNYDatatypeFormater::ToString(m_LoginInfo).c_str());
		fflush (Log::fp);
	}
	else
	{
		AuthUdpServer();
	}
}

// ok
// 该接口目前没有用到，所有操作结果通过OnRtnOrder返回.
// log this info only to see whether this method will be invoked.
void TunnRptProducer::OnRspOrderInsert(const DstarApiRspOrderInsertField *pOrderInsert)
{
	clog_info("[%s]  %s",
		module_name_,
		ESUNNYDatatypeFormater::ToString(pOrder).c_str());

	if(0 != pOrder->ErrorCode)
	{
		clog_error("[%s]  %s",
			module_name_,
			ESUNNYDatatypeFormater::ToString(pOrder).c_str());

		int32_t cursor = Push();
		struct TunnRpt &tunnrpt = rpt_buffer_[cursor];
		tunnrpt.LocalOrderID	= pOrder->Reference;
		strcpy(tunnrpt.OrderNo, pOrder->OrderNo);
		tunnrpt.ErrorID			= pOrder->ErrorCode;
		tunnrpt.OrderStatus		= DSTAR_API_STATUS_FAIL;

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		vrt_producer_claim(producer_, &vvalue);
		ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
		ivalue->index = cursor;
		ivalue->data = TUNN_RPT;
		vrt_producer_publish(producer_);
	}
	else { }
}

// ok
void TunnRptProducer::OnApiReady(const DstarApiSerialIdType nSerialId)
{
    clog_warning("[%s] OnAPIReady nSerialId: %llu",module_name_, nSerialId);	
}

// ok
void TunnRptProducer::OnFrontDisconnected()
{
    clog_error("[%s] OnFrontDisconnected. ", module_name_);
}

// ok
void TunnRptProducer::OnRspError(DstarApiErrorCodeType nErrorCode);
{
    clog_error("[%s] OnRspError: %u. ", module_name_, nErrorCode);
}


void TunnRptProducer::End()
{
	if(!ended_)
	{ 
		if (api_) 
		{
			FreeDstarTradeApi(api_);
			api_ = NULL;
		}
		ended_ = true;
		(vrt_producer_eof(producer_));
	}

	fflush (Log::fp);
}

// ok 
void TunnRptProducer::OnRtnOrder(const DstarApiOrderField *pOrder)
{
    clog_info("[%s] OnRtnOrder:%s",
				module_name_, 
				ESUNNYDatatypeFormater::ToString(pOrder).c_str());

	if (ended_) return;

	int32_t cursor = Push();
	struct TunnRpt &tunnrpt = rpt_buffer_[cursor];
	tunnrpt.LocalOrderID = pOrder->Reference;
	strcpy(tunnrpt.OrderNo, pOrder->OrderNo);
	strcpy(tunnrpt.SystemNo, pOrder->SystemNo);

	if (info->OrderInfo->OrderState==DSTAR_API_STATUS_QUEUE ||
		info->OrderInfo->OrderState==DSTAR_API_STATUS_APPLY ||
		info->OrderInfo->OrderState==DSTAR_API_STATUS_SUSPENDED
		) 
	{// discard these reports
		return;
	}

	// TODO: 确认是累计成交量还是当前成交量
	tunnrpt.MatchedAmount = pOrder->MatchQty;
	tunnrpt.OrderMatchPrice = info->OrderInfo->OrderMatchPrice;
	tunnrpt.ErrorID = pOrder->ErrorCode;
	tunnrpt.OrderStatus = pOrder->OrderState;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	vrt_producer_claim(producer_, &vvalue);
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = cursor;
	ivalue->data = TUNN_RPT;
	vrt_producer_publish(producer_);

	if(tunnrpt.OrderState==DSTAR_API_STATUS_FAIL ||
		tunnrpt.ErrorCode != DSTAR_API_ERR_SUCCESS)
	{
		clog_error("[%s] OnRtnOrder:%s",
					module_name_, 
					ESUNNYDatatypeFormater::ToString(pOrder).c_str());
	}
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

// coding for udp version
// ok
void TunnRptProducer::AuthUdpServer()
{
	m_udpFd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    char sendbuf[sizeof(DstarApiHead) + sizeof(DstarApiReqUdpAuthField)];
    DstarApiHead* pHead = (DstarApiHead*)sendbuf;
	pHead->FrameFlag = DSTAR_API_HEAD_FLAG;
	pHead->ProtocolCode = CMD_API_Req_UdpAuth;
	pHead->Version = DSTAR_API_PROTOCOL_VERSION;
	pHead->FieldCount = 1;
	pHead->FieldSize = sizeof(DstarApiReqUdpAuthField);

	DstarApiReqUdpAuthField *req = (DstarApiReqUdpAuthField *) &sendbuf[sizeof(DstarApiHead)];
	req->AccountIndex = m_LoginInfo.AccountIndex;
	req->UdpAuthCode =	m_LoginInfo.UdpAuthCode;
	req->ReqIdMode =	DSTAR_API_REQIDMODE_NOCHECK;

	socklen_t len = sizeof (udpserver_);
    if (sendto(m_udpFd, 
					sendbuf, 
					sizeof(sendbuf), 
					0, 
					(struct sockaddr*)&udpserver_, 
					len) != -1)
    {
    }
	else
	{
		clog_error("[%s] UdpAuth send failed!", module_name_); 
	}
}

// ok
int TunnRptProducer::InsertUdpOrder(char *sendbuf, const char* contract)
{
    DstarApiReqOrderInsertField * req = (DstarApiReqOrderInsertField*)(
				sendbuf + sizeof(DstarApiHead));
	//席位信息中的席位索引,填0不指定席位，由交易服务轮询席位
	req->SeatIndex = m_Seat.SeatIndex;   
	req->AccountIndex = m_LoginInfo.AccountIndex; 
	req->UdpAuthCode = m_LoginInfo.UdpAuthCode;   
	// TODO: 看看合约索引到底是何物
	req->ContractIndex = contracts_map_[contract];
	req->Reference = req->ClientReqId;     //要求 >=0

	clog_info("[%s] %s", 
				module_name_,
				ESUNNYDatatypeFormater::ToString(req).c_str()); 

	socklen_t len = sizeof (udpserver_);
    if (sendto(m_udpFd, 
					udporder, 
					UDP_ORDER_INSERT_LEN, 
					0, 
					(struct sockaddr*)&udpserver_, 
					len) != -1)
    {
    }
	else
	{
		clog_error("[%s] InsertUdpOrder send failed!", module_name_); 
	}

	return 0;
}


// ok
int TunnRptProducer::CancelUdpOrder(char *sendbuf)
{
	DstarApiReqOrderDeleteField *req = (DstarApiReqOrderDeleteField *)(
				sendbuf + sizeof(DstarApiHead));
	req->AccountIndex = m_LoginInfo.AccountIndex;
	req->UdpAuthCode = m_LoginInfo.UdpAuthCode;
	req->SeatIndex = m_Seat.SeatIndex ;     //0从报单席位撤单,非0从指定席位撤单
	req->Reference = req->ClientReqId;

	clog_info("[%s] %s", 
				module_name_,
				ESUNNYDatatypeFormater::ToString(req).c_str()); 

	socklen_t len = sizeof (udpserver_);
    if (sendto(m_udpFd, 
				deleteudporder, 
				UDP_ORDER_DELETE_LEN, 
				0, 
				(struct sockaddr*)&udpserver_, 
				len) != -1)
    {
    }
	else
	{
		clog_error("[%s] CancelUdpOrder send failed!", module_name_); 
	}

	return 0;
}
