#include <functional>   // std::bind
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "efh_lev2_producer.h"
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

#include "socket_multicast.h"
using std::cout;
using std::endl;
using std::stringstream;
using namespace std::placeholders;
using namespace std;

EfhLev2Producer::EfhLev2Producer(struct vrt_queue  *queue)
:module_name_("EfhLev2Producer")
{
	ended_ = false;
	clog_warning("[%s] FULL_DEPTH_MD_BUFFER_SIZE: %d;", module_name_, FULL_DEPTH_MD_BUFFER_SIZE);

	ParseConfig();
	
	// init dominant contracts
	memset(dominant_contracts_, 0, sizeof(dominant_contracts_));
	dominant_contract_count_ = LoadDominantContracts(config_.contracts_file, dominant_contracts_);

	// disruptor
	this->producer_ = vrt_producer_new("efh_lev2_producer", 1, queue);
	clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0){
		producer_ ->yield = vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		producer_ ->yield = vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		producer_ ->yield = vrt_yield_strategy_hybrid();
	}

	int err = InitMDApi();
	if(!err)
	{
		clog_warning("[%s] efh lev2 init failed.", module_name_);
	}
	else
	{
		clog_warning("[%s] efh lev2 init is successful.", module_name_);
	}
}

void EfhLev2Producer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    

	// yield strategy
    TiXmlElement *disruptor_node = RootElement->FirstChildElement("Disruptor");
	if (disruptor_node != NULL){
		strcpy(config_.yield, disruptor_node->Attribute("yield"));
	} else { clog_error("[%s] x-shmd.config error: Disruptor node missing.", module_name_); }

	// contracts file
    TiXmlElement *contracts_file_node = RootElement->FirstChildElement("Subscription");
	if (contracts_file_node != NULL){
		strcpy(config_.contracts_file, contracts_file_node->Attribute("contracts"));
	} else { clog_error("[%s] x-shmd.config error: Subscription node missing.", module_name_); }

    TiXmlElement *efhLev2  = RootElement->FirstChildElement("EfhLev2");
	strcpy(m_conf_info.m_remote_ip, efhLev2->Attribute("remote_ip"));
	int remote_port = 0;
	efhLev2->QueryIntAttribute("remote_port", &remote_port);
	this->m_conf_info.m_remote_port = remote_port;
	strcpy(m_conf_info.m_local_ip, efhLev2->Attribute("local_ip"));
	int local_port = 0;
	 efhLev2->QueryIntAttribute("local_port", &local_port);
	this->m_conf_info.m_local_port = local_port;
}

EfhLev2Producer::~EfhLev2Producer()
{
}

int EfhLev2Producer::InitMDApi()
{
	m_udp_conf = m_conf_info;
	return sock_init(m_udp_conf.m_remote_ip, 
				m_udp_conf.m_remote_port, 
				m_udp_conf.m_local_ip, 
				m_udp_conf.m_local_port, 
				this);
}

void EfhLev2Producer::on_receive_quote(efh3_lev2* data)
{
	// discard option
	if(strlen(data->m_symbol) > 6)
	{
		return;
	}

	if(!IsDominant(data->m_symbol)) return;

	// TODO: commented for debug
	char buffer[2048];
	//clog_info("[%s] rev efh3_lev2:%s", 
	//			module_name_,
	//			EfhLev2Producer::Format(*data,buffer));

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	vrt_producer_claim(producer_, &vvalue);
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = Push(*data);
	ivalue->data = EFH_LEV2;
	vrt_producer_publish(producer_);
}

void EfhLev2Producer::End()
{
	if(!ended_){
		ended_ = true;

		sock_close();

		vrt_producer_eof(producer_);
		clog_warning("[%s] End exit", module_name_);
	}
	fflush (Log::fp);
}

int32_t EfhLev2Producer::Push(const efh3_lev2& md){
	static int32_t shfemarketdata_cursor = FULL_DEPTH_MD_BUFFER_SIZE - 1;
	shfemarketdata_cursor++;
	if (shfemarketdata_cursor%FULL_DEPTH_MD_BUFFER_SIZE == 0){
		shfemarketdata_cursor = 0;
	}
	shfemarketdata_buffer_[shfemarketdata_cursor] = md;

	return shfemarketdata_cursor;
}

efh3_lev2* EfhLev2Producer::GetData(int32_t index)
{
	return &shfemarketdata_buffer_[index];
}

bool EfhLev2Producer::IsDominant(const char *contract)
{
#ifdef PERSISTENCE_ENABLED 
	// 持久化行情时，需要记录所有合约
	//clog_warning("[%s] return TRUE in IsDominant.",module_name_);
	return true;
#else
	return IsDominantImp((char*)contract, dominant_contracts_, dominant_contract_count_);
#endif
}

/////////////////////////////////////////////
// the following is market data from multicast 
//////////////////////////////////
bool socket_multicast::sock_init(const string& remote_ip, 
			unsigned short remote_port,
			const string& local_ip, 
			unsigned short local_port, 
			socket_event* ptr_event)//const string& local_ip,
{
	bool b_ret = false;
	const int CONST_ERROR_SOCK = -1;

	m_remote_ip = remote_ip;
	m_remote_port = remote_port;
	m_local_ip = local_ip;
	m_local_port = local_port;
	m_event = ptr_event;

	try
	{
		m_sock = socket(PF_INET, SOCK_DGRAM, 0);
		if(MY_SOCKET_ERROR == m_sock) 
		{
			throw CONST_ERROR_SOCK;
		}
		
		//socket可以重新使用一个本地地址
		int flag=1;
		if(setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&flag, sizeof(flag)) != 0)
		{
			throw CONST_ERROR_SOCK;
		}

//		int options = fcntl(m_sock, F_GETFL);
//		if(options < 0)
//		{
//			throw CONST_ERROR_SOCK;
//		}
//		options = options | O_NONBLOCK;
//		int i_ret = fcntl(m_sock, F_SETFL, options);
//		if(i_ret < 0)
//		{
//			throw CONST_ERROR_SOCK;
//		}

		struct sockaddr_in local_addr;
		memset(&local_addr, 0, sizeof(local_addr));
		local_addr.sin_family = AF_INET;
		local_addr.sin_addr.s_addr = htonl(INADDR_ANY);	
		local_addr.sin_port = htons(m_remote_port);	//multicast port
		if (bind(m_sock, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0)
		{
			throw CONST_ERROR_SOCK;
		}

		struct ip_mreq mreq;
		mreq.imr_multiaddr.s_addr = inet_addr(m_remote_ip.c_str());	//multicast group ip
		mreq.imr_interface.s_addr = inet_addr(m_local_ip.c_str());

		if (setsockopt(m_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) != 0)
		{
			throw CONST_ERROR_SOCK;
		}

		int receive_buf_size  = RCV_BUF_SIZE;	
		if (setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, (const char*)&receive_buf_size, sizeof(receive_buf_size)) != 0)
		{
			throw CONST_ERROR_SOCK;
		}

		//启动线程
		b_ret = start_server_event_thread();
	}
	catch(...)
	{
		close(m_sock);				
		b_ret = false;
	}

	return b_ret;
}


bool socket_multicast::sock_close()
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

void* socket_multicast::socket_server_event_thread(void* ptr_param)	
{
	socket_multicast* ptr_this = (socket_multicast*) ptr_param;
	if (NULL == ptr_this)
	{
		return NULL;
	}

	return ptr_this->on_socket_server_event_thread();
}

void* socket_multicast::on_socket_server_event_thread()
{
	char line[RCV_BUF_SIZE] = "";

	int n_rcved = -1;

	struct sockaddr_in muticast_addr;

	memset(&muticast_addr, 0, sizeof(muticast_addr));
	muticast_addr.sin_family = AF_INET;
	muticast_addr.sin_addr.s_addr = inet_addr(m_remote_ip.c_str());	
	muticast_addr.sin_port = htons(m_remote_port);

	while (true)
	{
		socklen_t len = sizeof(sockaddr_in);

		n_rcved = recvfrom(m_sock, line, RCV_BUF_SIZE, 0, (struct sockaddr*)&muticast_addr, &len);
		if ( n_rcved < 0) 
		{
			continue;
		} 
		else if (0 == n_rcved)
		{
			continue;
		}					
		else
		{
			m_event->on_receive_message(line, n_rcved);
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


bool socket_multicast::start_server_event_thread()
{
	m_thrade_quit_flag = false;

	pthread_t thread_id;
	pthread_attr_t thread_attr;
	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);		///<设置线程可分离
	//pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED);		///<设置线程的继承策略和参数来自于schedpolicy 与 schedparam中属性中显示设置
	//pthread_attr_setscope(&thread_attr, PTHREAD_SCOPE_SYSTEM);				///<设置线程的与系统中所有线程进行竞争

	//pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO);					///<设置线程的调试策略
	//int max_priority = sched_get_priority_max(SCHED_FIFO);					///<取得最大的优先级
	////int min_priority = sched_get_priority_min(SCHED_FIFO);				///<取得最小的优先级

	//struct sched_param sched_value;
	//sched_value.sched_priority = max_priority;
	//pthread_attr_setschedparam(&thread_attr, &sched_value);					///<设置优先级

	int ret = pthread_create(&thread_id, &thread_attr, socket_server_event_thread, this);
	pthread_attr_destroy(&thread_attr);

	if (ret != 0)
	{
		return false;
	}
	
	return true;
}

bool socket_multicast::stop_server_event_thread()
{
	m_thrade_quit_flag = true;

	return true;
}

