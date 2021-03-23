#include <functional>   // std::bind
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "lev1_producer.h"
#include "quote_cmn_utility.h"

#include <sys/socket.h> 
#include <sys/types.h>
#include <bits/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
//#include <asm/errno.h>

#include <iostream>
#include <sstream>
#include <string>
#include <assert.h>
#include <errno.h>

using std::cout;
using std::endl;
using std::stringstream;
using namespace std::placeholders;
using namespace std;

Lev1Producer::Lev1Producer(struct vrt_queue *queue)
: module_name_("Lev1Producer")
{
	this->idxMsgFinshed = false;

	m_sock = MY_SOCKET_DEFAULT;

	ended_ = false;
	clog_warning("[%s] DATA_BUFFER_SIZE: %d;", 
				module_name_, 
				DATA_BUFFER_SIZE);

	ParseConfig();
	
	// init dominant contracts
	memset(dominant_contracts_, 
				0, 
				sizeof(dominant_contracts_));
	dominant_contract_count_ = LoadDominantContracts(
				config_.contracts_file, 
				dominant_contracts_);

	// disruptor
	this->producer_ = vrt_producer_new(
				"efh_lev2_producer", 
				1, 
				queue);
	clog_warning("[%s] yield:%s", 
				module_name_, 
				config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0)
	{
		producer_ ->yield = vrt_yield_strategy_threaded();
	}
	else if(strcmp(config_.yield, "spin") == 0)
	{
		producer_ ->yield = vrt_yield_strategy_spin_wait();
	}
	else if(strcmp(config_.yield, "hybrid") == 0)
	{
		producer_ ->yield = vrt_yield_strategy_hybrid();
	}
}

void Lev1Producer::Start()
{
	int err = InitMDApi();
}

void Lev1Producer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    

	// yield strategy
    TiXmlElement *disruptor_node = 
		RootElement->FirstChildElement("Disruptor");
	if (disruptor_node != NULL)
	{
		strcpy(config_.yield, disruptor_node->Attribute("yield"));
	}
	else 
	{ 
		clog_error("[%s] x-shmd.config error: "
					"Disruptor node missing.", 
					module_name_); 
	}

	// contracts file
    TiXmlElement *contracts_file_node = 
		RootElement->FirstChildElement("Subscription");
	if (contracts_file_node != NULL)
	{
		strcpy(config_.contracts_file, 
					contracts_file_node->Attribute("contracts"));
	}
	else 
	{ 
		clog_error("[%s] x-shmd.config error: Subscription"
					"node missing.", 
					module_name_); 
	}

    TiXmlElement *lev1  = 
		RootElement->FirstChildElement("L1Md");
	strcpy(m_remote_ip, lev1->Attribute("remote_ip"));
	int remote_port = 0;
	lev1->QueryIntAttribute("remote_port", &remote_port);
	this->m_remote_port = remote_port;
	strcpy(m_local_ip, lev1->Attribute("local_ip"));
	int local_port = 0;
	 lev1->QueryIntAttribute("local_port", &local_port);
	this->m_local_port = local_port;
}

Lev1Producer::~Lev1Producer()
{
}

int Lev1Producer::InitMDApi()
{
	bool err = sock_init();
	if(!err)
	{
		clog_error("[%s] Lev1Producer init failed, err: %d", 
					module_name_,
					(int)err);
	}
	else
	{
		clog_warning("[%s] Lev1Producer init is successful, err: %d", 
					module_name_,
					(int)err);
	}

	return err;
}

void Lev1Producer::on_receive_quote(Lev1MarketData* data, int32_t index)
{
	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	vrt_producer_claim(producer_, &vvalue);
	ivalue = cork_container_of (vvalue, 
				struct vrt_hybrid_value, 
				parent);
	ivalue->index = index;
	ivalue->data = L1_MD;
	vrt_producer_publish(producer_);
}

void Lev1Producer::End()
{
	if(!ended_)
	{
		ended_ = true;
		sock_close();

		vrt_producer_eof(producer_);
		clog_warning("[%s] End exit", module_name_);
	}
	fflush (Log::fp);
}

int32_t Lev1Producer::Push()
{
	static int32_t data_cursor = DATA_BUFFER_SIZE - 1;
	data_cursor++;
	if (data_cursor % DATA_BUFFER_SIZE == 0)
	{
		data_cursor = 0;
	}

	return data_cursor;
}

Lev1MarketData* Lev1Producer::GetData(int32_t index)
{
	return &data_buffer_[index];
}


/////////////////////////////////////////////
// the following is market data from multicast 
//////////////////////////////////

////////////////////// the following codes is for SHFE ///////////////

bool Lev1Producer::sock_init()
{
	bool b_ret = false;
	const int CONST_ERROR_SOCK = -1;

	try
	{
		m_sock = socket(PF_INET, SOCK_DGRAM, 0);
		if(MY_SOCKET_ERROR == m_sock) 
		{
			throw CONST_ERROR_SOCK;
		}
		

		int opt_val = 0;
		socklen_t opt_len = sizeof(opt_val);
		getsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, &opt_val, &opt_len);
		clog_warning("[%s] get default SO_RCVBUF option: %d.", 
					module_name_, 
					opt_val);

		int receive_buf_size  = RCV_BUF_SIZE;	
		if (setsockopt(m_sock, 
						SOL_SOCKET, 
						SO_RCVBUF, 
						(const char*)&receive_buf_size, 
						sizeof(receive_buf_size)) != 0)
		{
			clog_error("[%s] it is failed to set "
						"SO_RCVBUF option: %d.", 
						module_name_, 
						receive_buf_size);
			//throw CONST_ERROR_SOCK;
		}
		opt_val = 0;
		opt_len = sizeof(opt_val);
		getsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, &opt_val, &opt_len);
		clog_warning("[%s] get SO_RCVBUF option: %d.", 
					module_name_, 
					opt_val);

		//socket可以重新使用一个本地地址
		int flag=1;
		if(setsockopt(m_sock, 
						SOL_SOCKET, 
						SO_REUSEADDR, 
						(const char*)&flag, 
						sizeof(flag)) != 0)
		{
			clog_error("[%s] it is failed to set "
						"SOL_SOCKEToption: %d.", 
						module_name_);
			//throw CONST_ERROR_SOCK;
		}

#ifdef PERSISTENCE_ENABLED 
#else
//		int options = fcntl(m_sock, F_GETFL);
//		if(options < 0)
//		{
//			throw CONST_ERROR_SOCK;
//		}
//		options = options | O_NONBLOCK;
//		int i_ret = fcntl(m_sock, F_SETFL, options);
//		if(i_ret < 0)
//		{
//			clog_warning("[%s] it is failed to set O_NONBLOCK option.", module_name_);
//			// throw CONST_ERROR_SOCK;
//		}
#endif

		struct sockaddr_in local_addr;
		memset(&local_addr, 0, sizeof(local_addr));
		local_addr.sin_family = AF_INET;
		local_addr.sin_addr.s_addr = htonl(INADDR_ANY);	
		local_addr.sin_port = htons(m_remote_port);	//multicast port
		if (bind(m_sock, 
			(struct sockaddr*)&local_addr, 
			sizeof(local_addr)) < 0)
		{
			throw CONST_ERROR_SOCK;
		}

		struct ip_mreq mreq;
		//multicast group ip
		mreq.imr_multiaddr.s_addr = inet_addr(m_remote_ip);	
		mreq.imr_interface.s_addr = inet_addr(m_local_ip);

		if (setsockopt(m_sock, 
			IPPROTO_IP, 
			IP_ADD_MEMBERSHIP, 
			&mreq, 
			sizeof(mreq)) != 0)
		{
			throw CONST_ERROR_SOCK;
		}

		//启动线程
		b_ret = start_server_event_thread();
	}
	catch(...)
	{
		clog_error("[%s] sock_init throw exception.",
					module_name_);

		close(m_sock);				
		b_ret = false;
	}

	return b_ret;
}

bool Lev1Producer::sock_close()
{
	bool b_ret = false;
	//关闭线程
	b_ret = stop_server_event_thread();

	if (m_sock != MY_SOCKET_DEFAULT)
	{
		close(m_sock);
		m_sock = MY_SOCKET_DEFAULT;
	}

	return b_ret;
}

void* Lev1Producer::socket_server_event_thread(void* ptr_param)	
{
	Lev1Producer* ptr_this = (Lev1Producer*) ptr_param;
	if (NULL == ptr_this)
	{
		return NULL;
	}

	return ptr_this->on_socket_server_event_thread();
}

void* Lev1Producer::on_socket_server_event_thread()
{
	int n_rcved = -1;
	Lev1MarketData* line = NULL;
	socklen_t len = sizeof(sockaddr_in);
	char rev_buffer[102400]; // 100K
	int buffer_size = sizeof(rev_buffer);

	struct sockaddr_in muticast_addr;
	memset(&muticast_addr, 0, sizeof(muticast_addr));
	muticast_addr.sin_family = AF_INET;
	muticast_addr.sin_addr.s_addr = inet_addr(m_remote_ip);	
	muticast_addr.sin_port = htons(m_remote_port);

	while(true)
	{
		n_rcved = recvfrom(m_sock, 
					&rev_buffer, 
					buffer_size, 
					0, 
					(struct sockaddr*)&muticast_addr, 
					&len);
		if(n_rcved < 0) 
		{
			continue;
		} 
		else if (0 == n_rcved)
		{
			continue;
		}					
		else
		{
			PackageHead packageHead;
			ProcPackageHead(rev_buffer, &packageHead);

			char *packageBody = rev_buffer + PACKAGE_HEAD_LEN;
			switch (packageHead.MsgType)
			{
				case MsgType::MESSAGE_INSTRUMENT_INDEX:
					ProcIdxMsgs(&packageHead, packageBody);
					break;

				case MsgType::MESSAGE_SINGLE_COMMODITY:
					ProcSCMsgs(&packageHead, packageBody);
					break;

				case MsgType::MESSAGE_COMBINE_TYPE:
					clog_info("[%s] rev MESSAGE_COMBINE_TYPE message.",
								module_name_);
					break;

				case MsgType::MESSAGE_BULLETINE:
					clog_info("[%s] rev MESSAGE_BULLETINE message.", 
								module_name_);
					break;

				case MsgType::MESSAGE_MARKET_MAKER_QUOT:
					clog_info("[%s] rev MESSAGE_MARKET_MAKER_QUOT"
								"message.",
								module_name_);
					break;

				case MsgType::MESSAGE_TESTATUS:
					clog_info("[%s] rev MESSAGE_TESTATUS message.",
								module_name_);
					break;

				default:
					clog_error("[%s] undefined message type %hhu", 
								module_name_, 
								packageHead.MsgType);
					break;
			}

		}	

		//检测线程退出信号
		if (m_thrade_quit_flag)
		{
			//此时已关闭完所有的客户端
			return NULL;
		}		
	}

	return NULL;
}


bool Lev1Producer::start_server_event_thread()
{
	m_thrade_quit_flag = false;

	pthread_t thread_id;
	pthread_attr_t thread_attr;
	pthread_attr_init(&thread_attr);
	///<设置线程可分离
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);		

	///<设置线程的继承策略和参数来自于schedpolicy 
	//与 schedparam中属性中显示设置
	//pthread_attr_setinheritsched(&thread_attr,PTHREAD_EXPLICIT_SCHED);		
	///<设置线程的与系统中所有线程进行竞争
	//pthread_attr_setscope(&thread_attr, PTHREAD_SCOPE_SYSTEM);				

	///<设置线程的调试策略
	//pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO);					
	///<取得最大的优先级
	//int max_priority = sched_get_priority_max(SCHED_FIFO);					
	///<取得最小的优先级
	////int min_priority = sched_get_priority_min(SCHED_FIFO);				

	//struct sched_param sched_value;
	//sched_value.sched_priority = max_priority;
	///<设置优先级
	//pthread_attr_setschedparam(&thread_attr, &sched_value);					

	int ret = pthread_create(&thread_id, 
				&thread_attr, 
				socket_server_event_thread, 
				this);
	pthread_attr_destroy(&thread_attr);

	if (ret != 0)
	{
		return false;
	}
	
	return true;
}

bool Lev1Producer::stop_server_event_thread()
{
	m_thrade_quit_flag = true;

	return true;
}

/////////////////////////////////
//////////// process package ////
/////////////////////////////////

// ok ok
void Lev1Producer::ProcPackageHead(char *packageBuf, 
			PackageHead *packageHead)
{
	packageHead->MsgType = (uint8_t)packageBuf[0];
	packageHead->MsgCnt  = (uint8_t)packageBuf[1];

	uint16_t* pPkgLen = (uint16_t*)(packageBuf+2);
	packageHead->PkgLen  = ntohs(*pPkgLen ); 

	packageHead->Print();
}

// ok ok
void Lev1Producer::ProcComIdxMsgBody(char* msgBodyBuf, 
			IndexMsgType *idxMsg,
			int instrumentIdLen) 
{
	char *fieldBuf = msgBodyBuf;

	// Type
	uint8_t* pType	= (uint8_t*)fieldBuf;
	idxMsg->Type	= *pType; 
	fieldBuf += sizeof(idxMsg->Type);

	// Index 
	uint16_t* pIndex = (uint16_t*)fieldBuf;
	idxMsg->Index = ntohs(*pIndex); 
	fieldBuf += sizeof(idxMsg->Index);

	// InstrumentId 
	memset(idxMsg->InstrumentId, 
				0, 
				sizeof(idxMsg->InstrumentId));
	char* pInstrumentId = (char*)fieldBuf;
	strncpy(idxMsg->InstrumentId, 
				pInstrumentId, 
				instrumentIdLen); 

	idxMsg->Print();
}

// ok ok
void Lev1Producer::Proc1stIdxMsgBody(char* msgBodyBuf, 
			IndexMsgType *idxMsg,
			int instrumentIdLen)
{
	// TradeData
	uint32_t* pTradeDate = (uint32_t*)msgBodyBuf;
	idxMsg->TradeDate = ntohl(*pTradeDate); 
	
	// other fields
	ProcComIdxMsgBody(msgBodyBuf + sizeof(idxMsg->TradeDate),
				idxMsg,
				instrumentIdLen);
}

// ok ok
// 如何从头到尾完整接收单腿合约的索引信息:
// 方案：
//		从第一个索引单腿合约开始，直到
//		下一次收到该合约开始，表示完成索引接收
void Lev1Producer::ProcIdxMsgs(PackageHead *packageHead, char *packageBodyBuf)
{
	if(this->idxMsgFinshed) return;

	
	int msgCnt = 0;
	int curPackageBodyLen = 0;
	char *msgBuf = packageBodyBuf;
	while(msgCnt<packageHead->MsgCnt ||
		curPackageBodyLen<packageHead->PkgLen)
	{
		IndexMsgType idxMsg;

		// message head		
		MessageHead msgHead;
		uint16_t* pMsgHead = (uint16_t*)msgBuf;
		msgHead.MsgLen = ntohs(*pMsgHead); 
		msgHead.Print();

		if(0 == msgCnt)
		{
			Proc1stIdxMsgBody(msgBuf+MSG_HEAD_LEN, 
						&idxMsg,
						msgHead.MsgLen - 9);
		}
		else
		{
			ProcComIdxMsgBody(msgBuf+MSG_HEAD_LEN, 
						&idxMsg,
						msgHead.MsgLen - 5);
		}

		// 单腿
		if(0 == idxMsg.Type)
		{	
			// 判断接收单腿合约的索引消息，是刚开始，还是已经结束
			if(0 == idxMsg.Index)
			{
				if(contracts_map_.empty())
				{
					contracts_map_[idxMsg.Index] = idxMsg.InstrumentId;
				}
				else
				{
					idxMsgFinshed = true;

					clog_info("[%s] begin to print contracts_map_:", module_name_);
					for ( auto it = contracts_map_.begin(); it != contracts_map_.end(); ++it )
					{
						clog_info("[%s] Index: %d; InstrumentId: %s ",
									module_name_,
									it->first,
									it->second.c_str());
					}
					clog_info("[%s] end printing contracts_map_.", module_name_);

					break;
				}
			}
			else
			{
				if(!contracts_map_.empty())
				{
					contracts_map_[idxMsg.Index] = idxMsg.InstrumentId;
				}
			}
		}

		msgBuf += msgHead.MsgLen;
		msgCnt++;
		curPackageBodyLen += msgHead.MsgLen;
	}
}


// ok ok
void Lev1Producer::ProcSCMsg(char* msgBuf, 
			Lev1MarketData *lev1Data,
			MessageHead *msgHead)
{
	// message head		
	uint16_t* pMsgHead = (uint16_t*)msgBuf;
	msgHead->MsgLen = ntohs(*pMsgHead); 
	msgHead->Print();

	char *msgBodyBuf = msgBuf + MSG_HEAD_LEN; 
	// first item
	SCMsg1stItemType firstItem;
	uint16_t* pDecimal = (uint16_t*)(msgBodyBuf);
	firstItem.Decimal = ntohs(*pDecimal); 
	msgBodyBuf += sizeof(firstItem.Decimal);
	uint16_t* pIndex = (uint16_t*)(msgBodyBuf);
	firstItem.Index = ntohs(*pIndex); 
	firstItem.Print();
	msgBodyBuf += sizeof(firstItem.Index);
	
	// others items
	int itemCnt = (msgHead->MsgLen-MSG_HEAD_LEN)/MSG_ITEM_LEN;
	for(int i=0; i<itemCnt-1; i++) // skip first item
	{
		uint32_t itemValueBuf = ntohl(((uint32_t*)msgBodyBuf)[0]);

		// Sign
		int sign = itemValueBuf >> 31;
		int signValue = 1;
		if(0 == sign)
		{
			signValue = +1;
		}
		else if(1 == sign)
		{
			signValue = -1;
		}

		// item index
		uint32_t itemIndex = (itemValueBuf & 0x7FFFFFFF) >> 26;
		// value
		// TODO: need to see define.
		uint32_t value = itemValueBuf & 0x3FFFFFF;

		clog_info("[%s] ProcSCMsg,"
					"item cnt: %d"
					"sign: %d; "
					"itemIndex: %u"
					"value: %u" ,  
					module_name_, 
					i,
					sign,
					itemIndex,
					value);

		switch (itemIndex)
		{
			case SCMsgItemIndexType::INSTRUMENTINDEX:
				// TODO: 按B25-B0取值看起来不对，需继续验证。
				// 暂时用firstItem.Index
				lev1Data->InstrumentIndex = firstItem.Index;
				// TODO:
				strcpy(lev1Data->InstrumentID, 
					contracts_map_[firstItem.Index].c_str());
				break;

			case SCMsgItemIndexType::OPENPRICE:
				lev1Data->OpenPrice = 
					(double)value/(double)firstItem.Decimal*signValue ;
				break;

			case SCMsgItemIndexType::HIGHPRICE:
				lev1Data->HighestPrice = 
					(double)value/(double)firstItem.Decimal*signValue ;
				break;

			case SCMsgItemIndexType::LOWPRICE:
				lev1Data->LowestPrice = 
					(double)value/(double)firstItem.Decimal*signValue ;
				break;

			case SCMsgItemIndexType::LASTPRICE:
				lev1Data->LastPrice = 
					(double)value/(double)firstItem.Decimal*signValue ;
				break;

			case SCMsgItemIndexType::BIDPRICE:
				lev1Data->BidPrice1 = 
					(double)value/(double)firstItem.Decimal*signValue ;
				break;

			case SCMsgItemIndexType::ASKPRICE:
				lev1Data->AskPrice1 = 
					(double)value/(double)firstItem.Decimal*signValue ;
				break;

			case SCMsgItemIndexType::BIDLOT:
				lev1Data->BidVolume1 = value * signValue;
				break;

			case SCMsgItemIndexType::ASKLOT:
				lev1Data->AskVolume1 = value * signValue;
				break;

			case SCMsgItemIndexType::VOLUME:
				lev1Data->Volume = value * signValue;
				break;

			case SCMsgItemIndexType::OPENINTEREST:
				lev1Data->OpenInterest = 
					(double)value/(double)firstItem.Decimal*signValue ;
				break;

			case SCMsgItemIndexType::DERIVEBIDPRICE:
				break;

			case SCMsgItemIndexType::DERIVEASKPRICE:
				break;

			case SCMsgItemIndexType::DERIVEBIDLOT:
				break;

			case SCMsgItemIndexType::DERIVEASKLOT:
				break;

			case SCMsgItemIndexType::AVGPRICE:
				lev1Data->AvgPrice = 
					(double)value/(double)firstItem.Decimal*signValue ;
				break;

			case SCMsgItemIndexType::UPDATETIME:
				// TODO: to see value detail: 135224
				lev1Data->UpdateTime = value * signValue;
				break;

			case SCMsgItemIndexType::CLEAR:
				lev1Data->SettlementPrice = 
					(double)value/(double)firstItem.Decimal*signValue ;
				break;

			default:
				clog_error("[%s] ProcSCMsg undifined itemIndex: %d;", 
							module_name_, 
							(int)itemIndex);
				break;
		}

		msgBodyBuf += MSG_ITEM_LEN;
	}
}

// ok ok
void Lev1Producer::ProcSCMsgs(PackageHead *packageHead,
			char *packageBodyBuf)
{
	if(!this->idxMsgFinshed) return;

	int msgCnt = 0;
	int curPackageBodyLen = 0;
	char *msgBuf = packageBodyBuf;
	while(msgCnt < packageHead->MsgCnt ||
		curPackageBodyLen < packageHead->PkgLen)
	{
		MessageHead msgHead;

		int32_t next_index = Push();
		Lev1MarketData *lev1Data = data_buffer_ + next_index;
		ProcSCMsg(msgBuf, lev1Data, &msgHead);
		lev1Data->Print();
		on_receive_quote(lev1Data, next_index);

		msgBuf += msgHead.MsgLen;
		msgCnt++;
		curPackageBodyLen += msgHead.MsgLen;

			clog_info("[%s] ProcSCMsgs while: msgCnt++: %d; "
						"curPackageBodyLen:%d",  
						module_name_, 
						msgCnt,
					curPackageBodyLen);

	}
}
