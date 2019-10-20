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
#include "guava_quote.h"
#include <chrono>
#include <ctime>
#include <ratio>
#include <ctime>


using namespace std::chrono;


/*
 * 缓存的最大的行情数量
 */
#define FULL_DEPTH_MD_BUFFER_SIZE 8192 

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

class EfhLev2Producer : public guava_quote_event
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

		/*
		 * 对source指定的行情数据进行格式化后存储到dest
		 */
		static char* Format(efh3_lev2 &source, char *dest)
		{
			sprintf (dest,
				"efh3_lev2 "				
				"InstrumentID:%s; "
				"UpdateTime[9]:%s; "
				"UpdateMillisec:%d; "
				"m_symbol_code:%d; "
				"LastPrice:%f; "												
				"Volume:%d; "
				"Turnover:%f; "								
				"BidPrice1:%f; "
				"BidVolume1:%d; "
				"AskPrice1:%f; "
				"AskVolume1:%d; "
				"BidPrice2:%f; "
				"BidVolume2:%d; "
				"AskPrice2:%f; "
				"AskVolume2:%d; "
				"BidPrice3:%f; "
				"BidVolume3:%d; "
				"AskPrice3:%f; "
				"AskVolume3:%d; "
				"BidPrice4:%f; "
				"BidVolume4:%d; "
				"AskPrice4:%f; "
				"AskVolume4:%d; "
				"BidPrice5:%f; "
				"BidVolume5:%d; "
				"AskPrice5:%f; "
				"AskVolume5:%d; "
				"m_open_interest:%f;"
				"timestmp:%ld\n ",
				source.m_symbol,
				source.m_update_time,
				source.m_millisecond,
				source.m_symbol_code,
				source.m_last_px,															
				source.m_last_share,
				source.m_turnover,				
				source.m_bid_1_px,
				source.m_bid_1_share,
				source.m_ask_1_px,
				source.m_ask_1_share,
				source.m_bid_2_px,
				source.m_bid_2_share,
				source.m_ask_2_px,
				source.m_ask_2_share,
				source.m_bid_3_px,
				source.m_bid_3_share,
				source.m_ask_3_px,
				source.m_ask_3_share,
				source.m_bid_4_px,
				source.m_bid_4_share,
				source.m_ask_4_px,
				source.m_ask_4_share,
				source.m_bid_5_px,
				source.m_bid_5_share,
				source.m_ask_5_px,
				source.m_ask_5_share,
				source.m_open_interest,
				(long)high_resolution_clock::now().time_since_epoch().count());

			return dest;
		}

	private:

		virtual void on_receive_quote(efh3_lev2* data);

		/*
		 * 与API相关
		 */
		int InitMDApi();

		int32_t dominant_contract_count_;
		char dominant_contracts_[MAX_CONTRACT_COUNT][10];

		/*
		 *disruptor相关
		 */
		int32_t Push(const efh3_lev2& md);
		struct vrt_producer  *producer_;
		std::array<efh3_lev2, FULL_DEPTH_MD_BUFFER_SIZE> shfemarketdata_buffer_;
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
		guava_quote	m_efh3;				///< 行情接收对象
		multicast_info	m_conf_info;		///< UDP信息
};

#endif
