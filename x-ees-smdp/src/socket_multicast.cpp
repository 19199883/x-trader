#ifdef SHENGLI_WINDOWS

#else

#include <sys/socket.h> 
#include <sys/types.h>
#include <bits/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <asm/errno.h>

#endif

#include <iostream>
#include <sstream>
#include <string>
#include <assert.h>
#include <errno.h>

#include "socket_multicast.h"
using std::cout;
using std::endl;
using std::stringstream;



#ifdef SHENGLI_WINDOWS


socket_multicast::socket_multicast()
{
	WORD version_requested;
	WSADATA wsa_data;

	version_requested = MAKEWORD( 2, 2 );

	int ret = WSAStartup( version_requested, &wsa_data );

	m_thrade_quit_flag = false;

	m_id = 0;
	m_local_port = 0;
	m_remote_port = 0;

	m_event = NULL;
	m_sock = MY_SOCKET_DEFAULT;

}

socket_multicast::~socket_multicast(void)
{
	WSACleanup();
}

bool socket_multicast::sock_init(const string& remote_ip, unsigned short remote_port,const string& local_ip, unsigned short local_port, int id, socket_event* ptr_event)//const string& local_ip,
{
	bool b_ret = false;
	const int CONST_ERROR_SOCK = -1;

	m_remote_ip = remote_ip;
	m_remote_port = remote_port;
	m_local_ip = local_ip;
	m_local_port = local_port;
	m_id = id;
	m_event = ptr_event;

	m_sock = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, 0, 0, WSA_FLAG_OVERLAPPED|WSA_FLAG_MULTIPOINT_C_LEAF|WSA_FLAG_MULTIPOINT_D_LEAF);
	if(MY_SOCKET_ERROR == m_sock) 
	{
		return false;
	}
	
	u_long flag = 1;
	if(ioctlsocket(m_sock, FIONBIO, &flag) != 0)
	{
		closesocket(m_sock);
		return false;
	}

	sockaddr_in addrLocal;
	sockaddr_in addrMulticast;
	addrLocal.sin_family = AF_INET;
	//addrLocal.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrLocal.sin_addr.S_un.S_addr = inet_addr(m_local_ip.c_str());
	addrLocal.sin_port = htons(m_remote_port);

	addrMulticast.sin_family = AF_INET;
	addrMulticast.sin_addr.S_un.S_addr = inet_addr(m_remote_ip.c_str());
	addrMulticast.sin_port = htons(m_remote_port);


	int ttl = 64;
	if (setsockopt(m_sock, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ttl, sizeof(ttl)) != 0)	//设置跳数
	{
		closesocket(m_sock);
		return false;
	}


	BOOL bTrue = TRUE;
	if(setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&bTrue, sizeof(BOOL)) != 0)
	{
		closesocket(m_sock);
		return false;
	}

	int ibind = bind(m_sock,(sockaddr*)&addrLocal,sizeof(addrLocal));
	if (ibind != 0)
	{
		closesocket(m_sock);
		return false;
	}

	struct in_addr address;
	address.s_addr = inet_addr(m_local_ip.c_str());
	if(setsockopt(m_sock, IPPROTO_IP, IP_MULTICAST_IF, (char*)&address, sizeof(address)) != 0)
	{
		closesocket(m_sock);
		return false;
	}


	DWORD dw_flag;
	dw_flag = JL_RECEIVER_ONLY;

	SOCKET ret_sock = WSAJoinLeaf(m_sock,(sockaddr*)&addrMulticast,sizeof(addrMulticast),0,0,0,0,dw_flag);
	if(INVALID_SOCKET == ret_sock)
	{
		int ret_error = WSAGetLastError();
		closesocket(m_sock);
		return false;
	}

	int newSize = 5242880;
	if(setsockopt(m_sock, SOL_SOCKET, SO_RCVBUF, (char*)&newSize, sizeof(newSize)) != 0)
	{
		closesocket(m_sock);
		return false;
	}
		
	//启动线程
	b_ret = start_server_event_thread();

	return b_ret;
}


bool socket_multicast::sock_close()
{
	bool b_ret = false;
	//关闭线程
	b_ret = stop_server_event_thread();

	closesocket(m_sock);

	return b_ret;
}




DWORD WINAPI socket_multicast::socket_server_event_thread(void* ptr_param)	
{
	socket_multicast* ptr_this = (socket_multicast*) ptr_param;
	if (NULL == ptr_this)
	{
		return NULL;
	}

	ptr_this->on_socket_server_event_thread();

	return 0L;
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
		int len = sizeof(sockaddr_in);

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
			report_user(EVENT_RECEIVE, m_id, line, n_rcved);				
		}	

		//检测线程退出信号
		if (m_thrade_quit_flag)
		{
			//此时已关闭完所有的客户端
			return NULL;
		}		
	}

	return NULL;;

}


bool socket_multicast::start_server_event_thread()
{
	m_thrade_quit_flag = false;

	DWORD dwThreadId;
	HANDLE hThread; 

	hThread = CreateThread(NULL, 0, socket_server_event_thread, this, 0, &dwThreadId);
	if(hThread != NULL)
	{
		CloseHandle(hThread);
	}

	return true;
}

bool socket_multicast::stop_server_event_thread()
{
	m_thrade_quit_flag = true;

	return true;
}


#else


socket_multicast::socket_multicast()
{

	m_thrade_quit_flag = false;

	m_id = 0;
	m_local_port = 0;
	m_remote_port = 0;

	m_event = NULL;
	m_sock = MY_SOCKET_DEFAULT;

}

socket_multicast::~socket_multicast(void)
{



}
bool socket_multicast::sock_init(const string& remote_ip, unsigned short remote_port,const string& local_ip, unsigned short local_port, int id, socket_event* ptr_event)//const string& local_ip,
{
	bool b_ret = false;
	const int CONST_ERROR_SOCK = -1;

	m_remote_ip = remote_ip;
	m_remote_port = remote_port;
	m_local_ip = local_ip;
	m_local_port = local_port;
	m_id = id;
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

		int options = fcntl(m_sock, F_GETFL);
		if(options < 0)
		{
			throw CONST_ERROR_SOCK;
		}
		options = options | O_NONBLOCK;
		int i_ret = fcntl(m_sock, F_SETFL, options);
		if(i_ret < 0)
		{
			throw CONST_ERROR_SOCK;
		}

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
			report_user(EVENT_RECEIVE, m_id, line, n_rcved);				
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


#endif




bool socket_multicast::report_user( SOCKET_EVENT type, int id, const char *buff, unsigned int size )
{
	if(NULL == m_event)
	{
		return false;
	}

	switch(type)
	{
	case  EVENT_RECEIVE:
		{
			m_event->on_receive_message(id, buff, size);
		}
		break;
	default:
		break;
	}
	return true;
}


void socket_multicast::log_msg( const string& msg )
{
	cout << msg << endl;
}





