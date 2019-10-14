#pragma once

#include <functional>
#include <array>
#include <string>
#include <thread>         
#include <chrono>        
#include "vrt_value_obj.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "mirp_message_engine.h"

/*
 * 缓存的最大的行情数量
 */
#define FULL_DEPTH_MD_BUFFER_SIZE 8192 

using namespace std;
using namespace mirp;

struct Lev2Config 
{
	char	m_remote_ip[128];		///< 组播行情远端地址
	int		m_remote_port;					///< 组播行情远端端口
	char	m_local_ip[128];			///< 组播本机地址
	int		m_local_port;					///< 组播本机端口

	char contracts_file[500];
	char yield[20]; // disruptor yield strategy
};

class Lev2Producer : public mirp_message_engine_event
{
	public:
		Lev2Producer(struct vrt_queue  *queue);
		~Lev2Producer();

		CDepthMarketDataField* GetData(int32_t index);
		void End();
		/*
		 * check whether the given contract is dominant.
		 */
		bool IsDominant(const char *contract);

	private:

		virtual void on_receive_quote(CDepthMarketDataField* data);

		/*
		 * 与API相关
		 */
		int InitMDApi();

		int32_t dominant_contract_count_;
		char dominant_contracts_[MAX_CONTRACT_COUNT][10];

		/*
		 *disruptor相关
		 */
		int32_t Push(const CDepthMarketDataField& md);
		struct vrt_producer  *producer_;
		std::array<CDepthMarketDataField, FULL_DEPTH_MD_BUFFER_SIZE> shfemarketdata_buffer_;
		bool ended_;

		/*
		 *日志相关
		 */
		const char *module_name_;  

		/*
		 * 配置相关
		 */
		Lev2Config config_;
		void ParseConfig();

	private:
		mirp_message_engine m_message_engine_;				///< 行情接收对象
		multicast_info	conf_info_;		///< UDP信息
};

