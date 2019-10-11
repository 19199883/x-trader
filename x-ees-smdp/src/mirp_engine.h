/*!****************************************************************************
 @note   Copyright (coffee), 2005-2014, Shengli Tech. Co., Ltd.
 @file   guava_quote.h
 @date   2014/8/27   12:44
 @author zhou.hu
 
 @brief   本文件是GUAVA行情组播接口的示例程序

 @note 
******************************************************************************/
#pragma once
#include <vector>
#include "socket_multicast.h"
#include "mirp_header.h"

#pragma pack(push, 1)

struct multicast_info
{
	char	m_remote_ip[MAX_IP_LEN];		///< 组播行情远端地址
	int		m_remote_port;					///< 组播行情远端端口
	char	m_local_ip[MAX_IP_LEN];			///< 组播本机地址
	int		m_local_port;					///< 组播本机端口
};


#pragma pack(pop)



class mirp_message_event
{
public:
	virtual ~mirp_message_event() {}
	/// \brief 接收到组播数据的回调事件
	//virtual void on_receive_quote(efh3_lev2* data) = 0;
};

class mirp_message : public socket_event
{
public:
	mirp_message(void);
	~mirp_message(void);

	/// \brief 初始化
	bool init(multicast_info cffex, mirp_message_event* event);

	/// \brief 关闭
	void close();

private:
	/// \brief 组播数据接收回调接口
	virtual void on_receive_message(int id, const char* buff, unsigned int len);


private:
	socket_multicast		m_udp;				///< UDP行情接收接口

	multicast_info			m_udp_conf;			///< 接口信息

	mirp_message_event*		m_ptr_event;		///< 行情回调事件接口
};

