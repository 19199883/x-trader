#ifndef __FULLDEPTHMD_PRODUCER_H__
#define __FULLDEPTHMD_PRODUCER_H__

#include <functional>
#include <array>
#include <string>
#include <thread>         
#include <chrono>        
#include "vrt_value_obj.h"
#include "quote_datatype_shfe_deep.h"
#include <tinyxml.h>
#include <tinystr.h>
#include <chrono>
#include <ctime>
#include <ratio>
#include <ctime>


using namespace std::chrono;

/////////////////////////
// market data by socket udp multicast
//////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <string>
#include <map>

using std::string;
using std::map;

#define MY_SOCKET							int		

#define SL_SOCK_DEFAULT						0
#define SL_SOCK_SEND						1
#define SL_SOCK_RECV						2
#define SL_SOCK_ERROR						3

///socket文件描述符缺省值
#define MY_SOCKET_DEFAULT					-1
///socket错误信息
#define MY_SOCKET_ERROR						-1	
///最大的接收缓冲区最
///服务器端最大的支持的客户端的连接数
#define MAX_SOCKET_CONNECT					1024
#define RCV_BUF_SIZE						1 * 1024 * 1024

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


#define MAX_IP_LEN				32

#define QUOTE_FLAG_SUMMARY		4


#pragma pack(push, 1)

struct efh3_lev2
{
	unsigned int	m_sequence;				///<会话编号
	char			m_exchange_id;			///<市场  0 表示中金  1表示上期
	char			m_channel_id;			///<通道编号
	char			m_symbol_type_flag;		///<合约标志
	int				m_symbol_code;			///<合约编号
	char			m_symbol[31];			///<合约
	char			m_update_time[9];		///<最后更新时间(秒)
	int				m_millisecond;			///<最后更新时间(毫秒)
	char			m_quote_flag;			///<行情标志		

	double			m_last_px;				///<最新价
	int				m_last_share;			///<最新总成交量
	double			m_turnover;				///<成交金额
	double			m_open_interest;		///<持仓量
	double			m_bid_1_px;				///<最新买价1档
	int				m_bid_1_share;			///<最新买量1档
	double			m_bid_2_px;				///<最新买价2档
	int				m_bid_2_share;			///<最新买量2档
	double			m_bid_3_px;				///<最新买价3档
	int				m_bid_3_share;			///<最新买量3档
	double			m_bid_4_px;				///<最新买价4档
	int				m_bid_4_share;			///<最新买量4档
	double			m_bid_5_px;				///<最新买价5档
	int				m_bid_5_share;			///<最新买量5档

	double			m_ask_1_px;				///<最新卖价1档
	int				m_ask_1_share;			///<最新卖量1档
	double			m_ask_2_px;				///<最新卖价2档
	int				m_ask_2_share;			///<最新卖量2档
	double			m_ask_3_px;				///<最新卖价3档
	int				m_ask_3_share;			///<最新卖量3档
	double			m_ask_4_px;				///<最新卖价4档
	int				m_ask_4_share;			///<最新卖量4档
	double			m_ask_5_px;				///<最新卖价5档
	int				m_ask_5_share;			///<最新卖量5档
};


#pragma pack(pop)
////////////////////////////////end market data by socket udp multicast

/*
 * 缓存的最大的行情数量
 */
#define FULL_DEPTH_MD_BUFFER_SIZE 32768 

using namespace std;

struct EfhLev2Config 
{
	char	m_remote_ip[MAX_IP_LEN];		///< 组播行情远端地址
	int		m_remote_port;					///< 组播行情远端端口
	char	m_local_ip[MAX_IP_LEN];			///< 组播本机地址
	int		m_local_port;					///< 组播本机端口

	char contracts_file[500];
	char yield[20]; // disruptor yield strategy
};

class EfhLev2Producer 
{
	public:
		EfhLev2Producer(struct vrt_queue  *queue);
		~EfhLev2Producer();

		efh3_lev2* GetData(int32_t index);
		void End();
		/*
		 * check whether the given contract is dominant.
		 */
		bool IsDominant(const char *contract);

		void on_receive_quote(efh3_lev2* data, int32_t index);

		/*
		 * 对source指定的行情数据进行格式化后存储到dest
		 */
		static char* Format(efh3_lev2 &source, char *dest)
		{
				//long timestamp = (long)high_resolution_clock::now().time_since_epoch().count();
			
			sprintf (dest,
				"efh3_lev2 "				
				"InstrumentID:%s; "
				"m_sequence:%u; "
				"UpdateTime[9]:%s; "
				"UpdateMillisec:%d; "
				//"timestamp:%ld"
				//"m_symbol_code:%d; "
				//"LastPrice:%.4f; "												
				//"Volume:%d; "
				//"Turnover:%.4f; "								
				//"m_open_interest:%.4f; "
				//"BidPrice1:%.4f; "
				//"BidVolume1:%d; "
				//"AskPrice1:%.4f; "
				//"AskVolume1:%d; "
				//"BidPrice2:%.4f; "
				//"BidVolume2:%d; "
				//"AskPrice2:%.4f; "
				//"AskVolume2:%d; "
				//"BidPrice3:%.4f; "
				//"BidVolume3:%d; "
				//"AskPrice3:%.4f; "
				//"AskVolume3:%d; "
				//"BidPrice4:%.4f; "
				//"BidVolume4:%d; "
				//"AskPrice4:%.4f; "
				//"AskVolume4:%d; "
				//"BidPrice5:%.4f; "
				//"BidVolume5:%d; "
				//"AskPrice5:%.4f; "
				//"AskVolume5:%d; "
				,
				source.m_symbol,
				source.m_sequence,
				source.m_update_time,
				source.m_millisecond
				//timestamp,
				//source.m_symbol_code,
				//source.m_last_px,															
				//source.m_last_share,
				//source.m_turnover,				
				//source.m_open_interest,
				//source.m_bid_1_px,
				//source.m_bid_1_share,
				//source.m_ask_1_px,
				//source.m_ask_1_share,
				//source.m_bid_2_px,
				//source.m_bid_2_share,
				//source.m_ask_2_px,
				//source.m_ask_2_share,
				//source.m_bid_3_px,
				//source.m_bid_3_share,
				//source.m_ask_3_px,
				//source.m_ask_3_share,
				//source.m_bid_4_px,
				//source.m_bid_4_share,
				//source.m_ask_4_px,
				//source.m_ask_4_share,
				//source.m_bid_5_px,
				//source.m_bid_5_share,
				//source.m_ask_5_px,
				//source.m_ask_5_share
					);

			return dest;
		}

		///////////////// market data bu socket udp multicast  //////
		/// \brief 组播实例初始化
		bool sock_init();
		/// \brief 组播实例关闭
		bool sock_close();
		
	private:
		/// \brief 组播数收发信号的线程函数(linux 版)
		static void* socket_server_event_thread(void* ptr_param);			

		/// \brief 组播数收发信号的处理函数
		void* on_socket_server_event_thread();

		/// \brief 启动组播信号处理线程
		bool start_server_event_thread();										
		/// \brief 停止组播信号处理线程
		bool stop_server_event_thread();	

		bool					m_thrade_quit_flag;		///< 信号检测线程退出标志		
		MY_SOCKET				m_sock;					///< 套接口
		///////////////// end market data bu socket udp multicast  //////
		//
		
	private:

		/*
		 * 与API相关
		 */
		int InitMDApi();

		int32_t dominant_contract_count_;
		char dominant_contracts_[MAX_CONTRACT_COUNT][10];

		/*
		 *disruptor相关
		 */
		int32_t Push();
		struct vrt_producer  *producer_;
		efh3_lev2 shfemarketdata_buffer_[FULL_DEPTH_MD_BUFFER_SIZE];
		bool ended_;

		/*
		 *日志相关
		 */
		const char *module_name_;  

		/*
		 * 配置相关
		 */
		EfhLev2Config config_;
		void ParseConfig();

	private:
		char	m_remote_ip[MAX_IP_LEN];		///< 组播行情远端地址
		int		m_remote_port;					///< 组播行情远端端口
		char	m_local_ip[MAX_IP_LEN];			///< 组播本机地址
		int		m_local_port;					///< 组播本机端口

};

#endif
