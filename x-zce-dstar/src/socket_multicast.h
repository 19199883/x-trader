/*!****************************************************************************
 @note   Copyright (coffee), 2005-2015, Shengli Tech. Co., Ltd.
 @file   socket_multicast.h
 @date   2015/4/29   17:17
 @author zhou.hu
 
 @brief     本类主要实现组播数据的接收， 在原来的基础上略作改进，不再支持windows平台。
		用户在具体实现时可以采用自己的UDP收发工具来替代此类的代码。

 @note 
******************************************************************************/

#pragma once


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef SHENGLI_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 
#endif

#include <WS2tcpip.h>
#include <WinSock2.h>


#else

#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>

#endif

#include <string>
#include <map>

using std::string;
using std::map;



#ifdef SHENGLI_WINDOWS

#define MY_SOCKET							SOCKET				//符合windows标准	

#else

#define MY_SOCKET							int		

#endif

#define SL_SOCK_DEFAULT						0
#define SL_SOCK_SEND						1
#define SL_SOCK_RECV						2
#define SL_SOCK_ERROR						3

///socket文件描述符缺省值
#define MY_SOCKET_DEFAULT					-1
///socket错误信息
#define MY_SOCKET_ERROR						-1	
///最大的接收缓冲区最
#define	RCV_BUF_SIZE						65535
///服务器端最大的支持的客户端的连接数
#define MAX_SOCKET_CONNECT					1024



///-----------------------------------------------------------------------------
///回调参数事件
///-----------------------------------------------------------------------------
enum SOCKET_EVENT
{
	EVENT_CONNECT,				//连接成功事件
	EVENT_REMOTE_DISCONNECT,	//联接端断开事件
	EVENT_LOCALE_DISCONNECT,	//主动断开事件
	EVENT_NETWORK_ERROR,		//网络错误
	EVENT_RECEIVE,				//数据接收事件
	EVENT_SEND,					//数据发送结束事件
	EVENT_RECEIVE_BUFF_FULL,	//接收缓冲区满
	EVENT_UNKNOW,				//未定义状态
};

class socket_event
{
public:
	virtual ~socket_event() {}
	/// \brief 接收到组播数据的回詷事件
	virtual void on_receive_message(int id, const char* buff, unsigned int len) = 0;
};


class socket_multicast
{
public:
	socket_multicast();
	virtual ~socket_multicast(void);
	/// \brief 组播实例初始化
	bool sock_init(const string& remote_ip, unsigned short remote_port,const string& local_ip, unsigned short local_port, int id, socket_event* ptr_event);
	/// \brief 组播实例关闭
	bool sock_close();

protected:
	//----------------------------------------------------------------------------
	//保护类成员函数
	//----------------------------------------------------------------------------

#ifdef SHENGLI_WINDOWS
	/// \brief 组播数收发信号的线程函数(windows 版)
	static  DWORD WINAPI socket_server_event_thread(void* ptr_param);	
#else
	/// \brief 组播数收发信号的线程函数(linux 版)
	static void* socket_server_event_thread(void* ptr_param);			
#endif

	/// \brief 组播数收发信号的处理函数
	void* on_socket_server_event_thread();

	/// \brief 启动组播信号处理线程
	bool start_server_event_thread();										
	/// \brief 停止组播信号处理线程
	bool stop_server_event_thread();	
	
	/// \brief 向客户报告的回调事件
	bool report_user(SOCKET_EVENT eventType, int id, const char *buff, unsigned int size);
	/// \brief 日志记录接口
	void log_msg(const string& msg);

protected:
	socket_event*			m_event;				///< 回调接口
	bool					m_thrade_quit_flag;		///< 信号检测线程退出标志		

	string					m_remote_ip;			///< 组播IP
	unsigned short			m_remote_port;			///< 组播端口
	string					m_local_ip;				///< 本地IP
	unsigned short			m_local_port;			///< 本地端口
	int						m_id;					///< 连接编号
	MY_SOCKET				m_sock;					///< 套接口
};







