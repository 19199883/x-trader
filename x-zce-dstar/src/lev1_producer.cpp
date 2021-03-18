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

using std::cout;
using std::endl;
using std::stringstream;
using namespace std::placeholders;
using namespace std;

EfhLev2Producer::EfhLev2Producer(struct vrt_queue  *queue)
:module_name_("EfhLev2Producer")
{
	m_sock = MY_SOCKET_DEFAULT;

	ended_ = false;
	clog_warning("[%s] FULL_DEPTH_MD_BUFFER_SIZE: %d;", module_name_, FULL_DEPTH_MD_BUFFER_SIZE);

	ParseConfig();
	
	// init dominant contracts
	memset(dominant_contracts_, 0, sizeof(dominant_contracts_));
	dominant_contract_count_ = LoadDominantContracts(config_.contracts_file, dominant_contracts_);

	// disruptor
	this->producer_ = vrt_producer_new("efh_lev2_producer", 1, queue);
	clog_warning("[%s] yield:%s", module_name_, config_.yield); 
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

void EfhLev2Producer::Start()
{
	int err = InitMDApi();
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
	strcpy(m_remote_ip, efhLev2->Attribute("remote_ip"));
	int remote_port = 0;
	efhLev2->QueryIntAttribute("remote_port", &remote_port);
	this->m_remote_port = remote_port;
	strcpy(m_local_ip, efhLev2->Attribute("local_ip"));
	int local_port = 0;
	 efhLev2->QueryIntAttribute("local_port", &local_port);
	this->m_local_port = local_port;

    TiXmlElement *ineEfhLev2  = RootElement->FirstChildElement("IneEfhLev2");
	strcpy(m_Ine_remote_ip, ineEfhLev2->Attribute("remote_ip"));
	int ine_remote_port = 0;
	ineEfhLev2->QueryIntAttribute("remote_port", &ine_remote_port);
	this->m_Ine_remote_port = ine_remote_port;
	strcpy(m_Ine_local_ip, ineEfhLev2->Attribute("local_ip"));
	int ine_local_port = 0;
	ineEfhLev2->QueryIntAttribute("local_port", &ine_local_port);
	this->m_Ine_local_port = ine_local_port;
}

EfhLev2Producer::~EfhLev2Producer()
{
}

int EfhLev2Producer::InitMDApi()
{
	int err = sock_init();
	if(!err)
	{
		clog_warning("[%s] efh lev2 init failed.", module_name_);
	}
	else
	{
		clog_warning("[%s] efh lev2 init is successful.", module_name_);
	}

	int errIne = sock_init_Ine();
	if(!errIne)
	{
		clog_warning("[%s] Ine efh lev2 init failed.", module_name_);
	}
	else
	{
		clog_warning("[%s] ine efh lev2 init is successful.", module_name_);
	}

	return errIne;
}

void EfhLev2Producer::on_receive_quote(efh3_lev2* data, int32_t index)
{
	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	vrt_producer_claim(producer_, &vvalue);
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = index;
	ivalue->data = EFH_LEV2;
	vrt_producer_publish(producer_);
}

void EfhLev2Producer::End()
{
	if(!ended_){
		ended_ = true;

		sock_close();
		sock_close_Ine();

		vrt_producer_eof(producer_);
		clog_warning("[%s] End exit", module_name_);
	}
	fflush (Log::fp);
}

int32_t EfhLev2Producer::Push()
{
	static int32_t shfemarketdata_cursor = FULL_DEPTH_MD_BUFFER_SIZE - 1;
	shfemarketdata_cursor++;
	if (shfemarketdata_cursor%FULL_DEPTH_MD_BUFFER_SIZE == 0){
		shfemarketdata_cursor = 0;
	}

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

////////////////////// the following codes is for SHFE ///////////////

bool EfhLev2Producer::sock_init()
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
		clog_warning("[%s] get default SO_RCVBUF option: %d.", module_name_, opt_val);

		int receive_buf_size  = RCV_BUF_SIZE;	
		if (setsockopt(m_sock, 
						SOL_SOCKET, 
						SO_RCVBUF, 
						(const char*)&receive_buf_size, 
						sizeof(receive_buf_size)) != 0)
		{
			clog_warning("[%s] it is failed to set SO_RCVBUF option: %d.", 
						module_name_, 
						receive_buf_size);
			//throw CONST_ERROR_SOCK;
		}
		opt_val = 0;
		opt_len = sizeof(opt_val);
		getsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, &opt_val, &opt_len);
		clog_warning("[%s] get SO_RCVBUF option: %d.", module_name_, opt_val);

		//socket可以重新使用一个本地地址
		int flag=1;
		if(setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&flag, sizeof(flag)) != 0)
		{
			throw CONST_ERROR_SOCK;
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
		if (bind(m_sock, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0)
		{
			throw CONST_ERROR_SOCK;
		}

		struct ip_mreq mreq;
		mreq.imr_multiaddr.s_addr = inet_addr(m_remote_ip);	//multicast group ip
		mreq.imr_interface.s_addr = inet_addr(m_local_ip);

		if (setsockopt(m_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) != 0)
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


bool EfhLev2Producer::sock_close()
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

void* EfhLev2Producer::socket_server_event_thread(void* ptr_param)	
{

	EfhLev2Producer* ptr_this = (EfhLev2Producer*) ptr_param;
	if (NULL == ptr_this)
	{
		return NULL;
	}

	return ptr_this->on_socket_server_event_thread();

}

void* EfhLev2Producer::on_socket_server_event_thread()
{
	int n_rcved = -1;
	int buffer_size = sizeof(efh3_lev2);
	efh3_lev2* line = NULL;
	socklen_t len = sizeof(sockaddr_in);
	efh3_lev2 rev_buffer;

	struct sockaddr_in muticast_addr;

	memset(&muticast_addr, 0, sizeof(muticast_addr));
	muticast_addr.sin_family = AF_INET;
	muticast_addr.sin_addr.s_addr = inet_addr(m_remote_ip);	
	muticast_addr.sin_port = htons(m_remote_port);

	while (true)
	{

		n_rcved = recvfrom(m_sock, &rev_buffer, buffer_size, 0, (struct sockaddr*)&muticast_addr, &len);
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
			// lock
			std::lock_guard<std::mutex> lck (m_mtx);

			int32_t next_index = Push();
			line = shfemarketdata_buffer_ + next_index;
			memcpy(line, &rev_buffer, sizeof(efh3_lev2 ));

			on_receive_quote((efh3_lev2*)line, next_index);
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


bool EfhLev2Producer::start_server_event_thread()
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

bool EfhLev2Producer::stop_server_event_thread()
{
	m_thrade_quit_flag = true;

	return true;
}

////////////////////// the following is codes for INE ////////////////////////

bool EfhLev2Producer::sock_init_Ine()
{
	bool b_ret = false;
	const int CONST_ERROR_SOCK = -1;

	try
	{
		m_sock_Ine = socket(PF_INET, SOCK_DGRAM, 0);
		if(MY_SOCKET_ERROR == m_sock_Ine) 
		{
			throw CONST_ERROR_SOCK;
		}

		int opt_val = 0;
		socklen_t opt_len = sizeof(opt_val);
		getsockopt(m_sock_Ine, SOL_SOCKET, SO_RCVBUF, &opt_val, &opt_len);
		clog_warning("[%s] Ine get default SO_RCVBUF option: %d.", module_name_, opt_val);

		int receive_buf_size  = RCV_BUF_SIZE;	
		if (setsockopt(m_sock_Ine, 
						SOL_SOCKET, 
						SO_RCVBUF, 
						(const char*)&receive_buf_size, 
						sizeof(receive_buf_size)) != 0)
		{
			clog_warning("[%s] Ine it is failed to set SO_RCVBUF option: %d.", 
						module_name_, 
						receive_buf_size);
			//throw CONST_ERROR_SOCK;
		}
		opt_val = 0;
		opt_len = sizeof(opt_val);
		getsockopt(m_sock_Ine, SOL_SOCKET, SO_RCVBUF, &opt_val, &opt_len);
		clog_warning("[%s] Ine get SO_RCVBUF option: %d.", module_name_, opt_val);

		//socket可以重新使用一个本地地址
		int flag=1;
		if(setsockopt(m_sock_Ine, SOL_SOCKET, SO_REUSEADDR, (const char*)&flag, sizeof(flag)) != 0)
		{
			throw CONST_ERROR_SOCK;
		}

#ifdef PERSISTENCE_ENABLED 
#else
//		int options = fcntl(m_sock_Ine, F_GETFL);
//		if(options < 0)
//		{
//			throw CONST_ERROR_SOCK;
//		}
//		options = options | O_NONBLOCK;
//		int i_ret = fcntl(m_sock_Ine, F_SETFL, options);
//		if(i_ret < 0)
//		{
//			clog_warning("[%s] Ine it is failed to set O_NONBLOCK option.", module_name_);
//			// throw CONST_ERROR_SOCK;
//		}
#endif

		struct sockaddr_in local_addr;
		memset(&local_addr, 0, sizeof(local_addr));
		local_addr.sin_family = AF_INET;
		local_addr.sin_addr.s_addr = htonl(INADDR_ANY);	
		local_addr.sin_port = htons(m_Ine_remote_port);	//multicast port
		if (bind(m_sock_Ine, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0)
		{
			throw CONST_ERROR_SOCK;
		}

		struct ip_mreq mreq;
		mreq.imr_multiaddr.s_addr = inet_addr(m_Ine_remote_ip);	//multicast group ip
		mreq.imr_interface.s_addr = inet_addr(m_Ine_local_ip);

		if (setsockopt(m_sock_Ine, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) != 0)
		{
			throw CONST_ERROR_SOCK;
		}

		//启动线程
		b_ret = start_server_event_thread_Ine();
	}
	catch(...)
	{
		close(m_sock_Ine);				
		b_ret = false;
	}

	return b_ret;
}

bool EfhLev2Producer::sock_close_Ine()
{
	bool b_ret = false;
	//关闭线程
	b_ret = stop_server_event_thread_Ine();

	if (m_sock_Ine != MY_SOCKET_DEFAULT)
	{
		close(m_sock_Ine);
		m_sock_Ine = MY_SOCKET_DEFAULT;
	}

	return b_ret;
}

void* EfhLev2Producer::socket_server_event_thread_Ine(void* ptr_param)	
{

	EfhLev2Producer* ptr_this = (EfhLev2Producer*) ptr_param;
	if (NULL == ptr_this)
	{
		return NULL;
	}

	return ptr_this->on_socket_server_event_thread_Ine();

}

void* EfhLev2Producer::on_socket_server_event_thread_Ine()
{
	int n_rcved = -1;
	int buffer_size = sizeof(efh3_lev2);
	efh3_lev2* line = NULL;
	socklen_t len = sizeof(sockaddr_in);
	efh3_lev2 rev_buffer;

	struct sockaddr_in muticast_addr;

	memset(&muticast_addr, 0, sizeof(muticast_addr));
	muticast_addr.sin_family = AF_INET;
	muticast_addr.sin_addr.s_addr = inet_addr(m_Ine_remote_ip);	
	muticast_addr.sin_port = htons(m_Ine_remote_port);

	while (true)
	{

		n_rcved = recvfrom(m_sock_Ine, &rev_buffer, buffer_size, 0, (struct sockaddr*)&muticast_addr, &len);
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
			// lock
			std::lock_guard<std::mutex> lck (m_mtx);

			int32_t next_index = Push();
			line = shfemarketdata_buffer_ + next_index;
			memcpy(line, &rev_buffer, sizeof(efh3_lev2 ));

			on_receive_quote((efh3_lev2*)line, next_index);
		}	

		//检测线程退出信号
		if (m_thrade_quit_flag_Ine)
		{
			//此时已关闭完所有的客户端
			return NULL;
		}		
	}

	return NULL;
}

bool EfhLev2Producer::start_server_event_thread_Ine()
{
	m_thrade_quit_flag_Ine = false;

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

	int ret = pthread_create(&thread_id, &thread_attr, socket_server_event_thread_Ine, this);
	pthread_attr_destroy(&thread_attr);

	if (ret != 0)
	{
		return false;
	}
	
	return true;
}

bool EfhLev2Producer::stop_server_event_thread_Ine()
{
	m_thrade_quit_flag_Ine = true;

	return true;
}
