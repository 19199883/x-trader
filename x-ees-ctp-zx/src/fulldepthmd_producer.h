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

/*
 * 缓存的最大的行情数量
 */
#define FULL_DEPTH_MD_BUFFER_SIZE 8192 

using namespace std;

struct FulldepthMDConfig 
{
	string addr;
	char ip[30];
	int port;
	char contracts_file[500];
	char yield[20]; // disruptor yield strategy
};

class FullDepthMDProducer
{
	public:
		FullDepthMDProducer(struct vrt_queue  *queue);
		~FullDepthMDProducer();

		CShfeFtdcMBLMarketDataField* GetData(int32_t index);
		void End();
		/*
		 * check whether the given contract is dominant.
		 */
		bool IsDominant(const char *contract);

		std::string ToString(const CShfeFtdcMBLMarketDataField &d);

	private:
		/*
		 * 与API相关
		 */
		int InitMDApi();

		/*
		 * 逻辑相关
		 */
		void RevData();
		std::thread *thread_rev_;
		int32_t dominant_contract_count_;
		char dominant_contracts_[MAX_CONTRACT_COUNT][10];

		/*
		 *disruptor相关
		 */
		int32_t Push(const CShfeFtdcMBLMarketDataField& md);
		struct vrt_producer  *producer_;
		std::array<CShfeFtdcMBLMarketDataField, FULL_DEPTH_MD_BUFFER_SIZE> shfemarketdata_buffer_;
		bool ended_;

		/*
		 *日志相关
		 */
		const char *module_name_;  
		int udp_fd_;

		/*
		 * 配置相关
		 */
		FulldepthMDConfig config_;
		void ParseConfig();
};

#endif
