#pragma once

/*
 * reference doc: zce一档组播行情接入说明.pdf
 *
 *
 */

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
#include <arpa/inet.h> 

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

#pragma pack(push)
#pragma pack(1)

/*
 * 消息类型( ( MsgType) 定义
 */
enum class MsgType
{
	//合约索引信息消息
	MESSAGE_INSTRUMENT_INDEX	=	0x05,	

	//单腿行情消息
	MESSAGE_SINGLE_COMMODITY	=	0x10,	

	//组合行情消息
	MESSAGE_COMBINE_TYPE		=	0x11,	

	//广播消息
	MESSAGE_BULLETINE			=	0x12,	

	// 做市商报价消息
	MESSAGE_MARKET_MAKER_QUOT	=	0x13,	

	//交易系统状态消息
	MESSAGE_TESTATUS			=	0x14,	
};


/*
 *	消息头
 *	MsgLen 包含消息头本身的长度
 */
#define MSG_HEAD_LEN 2
#define MSG_ITEM_LEN 4
struct MessageHead
{
	uint16_t MsgLen;	// 消息长度
};

/*
 * 行情数据传输协议为二进制协议，采用网络字节序的
 * 大端存储方式。
 *
 * TODO:考虑是否需要大小端转换
 * ref: https://www.zhihu.com/question/19953760
 *
 * 每一个数据包(Package)可能包含多个消息(Message)。消息
 * 的个数在包头(Package Head)中定义。包头中的包
 * 长度(Package Length)不包括包头的长度。包头中消息
 * 类型(Message Type)定义了消息的类型。
 * 消息长度包括消息头的长度
 * 
 * 注意：MsgCnt可能比实际的消息个数多1，
 * 所以要同时判断消息个数和报文实际长度。
 *
 */
#define PACKAGE_HEAD_LEN 4
struct PackageHead
{
	public:
		void Print()
		{
			clog_warning("[%s] PackageHead:"
						"MsgType: %hhu;", 
						"MsgCnt: %hhu;", 
						"PkgLen: %hu;", 
						module_name_, 
						this->MsgType,
						this->MsgCnt,
						this->PkgLen);

		}

		uint8_t MsgType;  // 消息类型
		uint8_t MsgCnt;	  //	消息个数
		uint16_t PkgLen;  //	数据包长度

};

/*
 * * TODO：如何确认所有的索引信息都接收完毕
 * * 一种方法：从接收索引是0的单腿合约开始计时，
 *  等待1分30秒，即可认为接收完毕
 *
 * * 合约索引信息消息
 * * 第一个Msg中包含有交易日信息
 * * 后续msg不包含交易日信息  
 * * 单腿合约和组合合约分别从 0 开始编号
 * * 合约索引消息定时发送，一分钟内所有的合约索引发送一次
 * * 合约信息建议更新至集合竞价阶段，确保获取最新的合约信息 
 * */
struct IndexMsgType
{
	uint32_t TradeDate;		//	交易日, 如  20200101
	uint8_t Type;			//	合约类型,0:单腿,1:组合
	uint16_t Index;			//	合约索引

	//	第一个Msg中包含有交易日信息: char MsgLen-9 
	//	合约编码，
	//	后续msg不包含交易日信息: char MsgLen-5 
	//	合约编码，
	//
	//	例如期货 AP005，
	//	期权 CF007C10000，
	//	组合SPD-AP005/AP007 ，IPS-SF010/SM010
	char InstrumentId[50]; 
};


/*
 * 单腿行情报文由多个Item组成，每个Item 
 * 长度4字节。最多包含18个Item。
 * 例如,报价精度=100,OpenPrice=13787,
 * 实际的 OpenPrice=137.87
 * 
 * Single Contract Msg First Item Type
*/
struct SCMsg1stItemType
{
	uint16_t Decimal;	// 报价精度
	uint16_t Index;		// 合约索引
};

/*
 * 单腿合约报文中Item Index 具体含义如下
 */
enum class SCMsgItemIndexType
{
	INSTRUMENTINDEX = 0,  //  合约索引
	OPENPRICE		= 1,  //  开盘价
	HIGHPRICE		= 2,  //  最高价
	LOWPRICE		= 3,  //  最低价
	LASTPRICE		= 4,  //  最新价
	BIDPRICE		= 5,  //  买价
	ASKPRICE		= 6,  //  卖价
	BIDLOT			= 7,  //  买量
	ASKLOT			= 8,  //  卖量
	VOLUME			= 9,  //  成交量
	OPENINTEREST	= 10, //  持仓量
	DERIVEBIDPRICE	= 11, //  组合买入价		
	DERIVEASKPRICE	= 12, //  组合卖出价
	DERIVEBIDLOT	= 13, //  组合买入数量
	DERIVEASKLOT	= 14, //  组合卖出数量
	AVGPRICE		= 15, //  均价
	UPDATETIME		= 16, //  更新时间
	CLEAR			= 17, // 结算价
};

/*
 * 第二个Item开始，需要按照bit位解析，
 * 解析4字节中具体bit位表示的含义，
 * 后续以B(31-0)表示，如 B31表示第31 位，
 * 从0计数。
 * 
 * B31 是符号位，0表示正数，1表示负数。
 * B30-B26 是 Item Index，说明该 Item 代表的具体内容。
 * B25-B0 是对应的具体值。TODO: 观察该值是字符串形式
 * 还是其他的
 */
struct SCMsgContentItemType
{
	uint32_t Content;	
};


#pragma pack(pop)


///深度市场行情（一档行情结构）
struct Lev1MarketData
{
	uint16_t InstrumentIndex;
    ///交易日
    char TradingDay[9];
    ///最新价
    double LastPrice;
    ///昨结算
    double PreSettlementPrice;
    ///昨收盘
    double PreClosePrice;
    ///昨持仓量
    double PreOpenInterest;
    ///今开盘
    double OpenPrice;
    ///最高价
    double HighestPrice;
    ///最低价
    double LowestPrice;
    ///数量
    int Volume;
    ///成交金额
    double Turnover;
    ///持仓量
    double OpenInterest;
    ///今收盘
    double ClosePrice;
    ///今结算
    double SettlementPrice;
    ///涨停板价
    double UpperLimitPrice;
    ///跌停板价
    double LowerLimitPrice;
    ///最后修改时间
    uint16_t UpdateTime;
    ///最后修改毫秒
    int UpdateMillisec;
    ///合约代码
    char InstrumentID[31];
    ///申买价一
    double BidPrice1;
    ///申买量一
    int BidVolume1;
    ///申卖价一
    double AskPrice1;
    ///申卖量一
    int AskVolume1;
	// 均价
    double AvgPrice;
};


////////////////////////////////end market data by socket udp multicast

/*
 * 缓存的最大的行情数量
 */
#define DATA_BUFFER_SIZE 32768 

using namespace std;

struct Lev1Config 
{
	char	m_remote_ip[MAX_IP_LEN];		///< 组播行情远端地址
	int		m_remote_port;					///< 组播行情远端端口
	char	m_local_ip[MAX_IP_LEN];			///< 组播本机地址
	int		m_local_port;					///< 组播本机端口

	char contracts_file[500];
	char yield[20]; // disruptor yield strategy
};

class Lev1Producer 
{
	public:
		Lev1Producer(struct vrt_queue  *queue);
		~Lev1Producer();

		Lev1MarketData* GetData(int32_t index);
		void Start();
		void End();

		void ProcPackageHead(
					char *packageBuf, 
					PackageHead *packageHead);

		/*
		 *
		 */
		void ProcIdxMsgs(
					PackageHead *packageHead,
					char *packageBodyBuf);

		/*
		 *
		 */
		void ProcSCMsgs(
					PackageHead *packageHead
					char *packageBodyBuf, 
					Lev1MarketData *lev1Data);

		/*
		 * check whether the given contract is dominant.
		 */
		bool IsDominant(const char *contract);

		void on_receive_quote(Lev1MarketData* data, int32_t index);

		/*
		 * 对source指定的行情数据进行格式化后存储到dest
		 */
		static char* Format(Lev1MarketData *source,char *dest)
		{
				sprintf (dest,
				"Lev1MarketData "
				"InstrumentID:%s; "
				"UpdateTime[9]:%s; "
				"UpdateMillisec:%d; "
				"TradingDay:%s; "
				"LastPrice:%.4f; "
				"PreSettlementPrice:%.4f; "
				"PreClosePrice:%.4f; "
				"PreOpenInterest:%.4f; "
				"OpenPrice:%.4f; "
				"HighestPrice:%.4f; "
				"LowestPrice:%.4f; "
				"Volume:%d; "
				"Turnover:%.4f; "
				"OpenInterest:%.4f; "
				"ClosePrice:%.4f; "
				"SettlementPrice:%.4f; "
				"UpperLimitPrice:%.4f; "
				"LowerLimitPrice:%.4f; "
				"BidPrice1:%.4f; "
				"BidVolume1:%d; "
				"AskPrice1:%.4f; "
				"AskVolume1:%d; ",
				source.InstrumentID,
				source.UpdateTime,
				source.UpdateMillisec,
				source.TradingDay,
				InvalidToZeroD(source.LastPrice),
				InvalidToZeroD(source.PreSettlementPrice),
				InvalidToZeroD(source. PreClosePrice),
				InvalidToZeroD(source.PreOpenInterest),
				InvalidToZeroD(source.OpenPrice),
				InvalidToZeroD(source. HighestPrice),
				InvalidToZeroD(source. LowestPrice),
				source.Volume,
				InvalidToZeroD(source.Turnover),
				InvalidToZeroD(source.OpenInterest),
				source.ClosePrice,
				source.SettlementPrice,
				InvalidToZeroD(source.UpperLimitPrice),
				InvalidToZeroD(source.LowerLimitPrice),
				InvalidToZeroD(source.BidPrice1),
				source.BidVolume1,
				InvalidToZeroD(source.AskPrice1),
				source.AskVolume1);

			return dest;
		
		}

		///////////////// market data bu socket udp multicast  //////
		////////////// the following is for SHFE ////////////
		/// \brief 组播实例初始化
		bool sock_init();
		/// \brief 组播实例关闭
		bool sock_close();

		
	private:
		////////////////// the following is for SHFE ///////
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

		bool					m_thrade_quit_flag_Ine;		///< 信号检测线程退出标志		
		MY_SOCKET				m_sock_Ine;					///< 套接口

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
		Lev1MarketData data_buffer_[DATA_BUFFER_SIZE];
		bool ended_;

		/*
		 *日志相关
		 */
		const char *module_name_;  

		/*
		 * 配置相关
		 */
		Lev1Config config_;
		void ParseConfig();

	private:
		char	m_remote_ip[MAX_IP_LEN];		///< 组播行情远端地址
		int		m_remote_port;					///< 组播行情远端端口
		char	m_local_ip[MAX_IP_LEN];			///< 组播本机地址
		int		m_local_port;					///< 组播本机端口
};

