#ifndef __DMD_PRODUCER_H__
#define __DMD_PRODUCER_H__

#include <functional>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <array>
#include <string>
#include <thread>         
#include <chrono>        
#include "vrt_value_obj.h"
#include "quote_datatype_cme.h"
#include "quote_cmn_utility.h"
#include "quote_cmn_save.h"

/*
 * 10 power of 2
 */
#define DMD_BUFFER_SIZE 1000 

/*
 * 识depthMarketData行情处于未接收数居前的未知位置
 */
#define DMD_NPOS -1 

using namespace std;

struct DMDConfig 
{
	char mcIp[30];
	int mcPort;
	char contracts_file[500];
	char yield[20];				// disruptor yield strategy
};

class DMDProducerHelper
{
	public:
		/*
		 * 获取指定合约的最新行情。
		 * 从行情缓存的最新位置向前查找最多查找主力合约个数Deep位置，中途找到则立即返回
		 */
		static depthMarketData* GetLastDataImp(const char *contract, int32_t last_index,
			depthMarketData *buffer, int32_t buffer_size,int32_t traverse_count);
};

class DMDProducer
{
	public:
		DMDProducer(struct vrt_queue  *queue);
		~DMDProducer();

		/*
		 * 与逻辑处理相关
		 */
		depthMarketData* GetData(int32_t index);

		/*
		 * contract: 要获取行情的合约
		 * last_index;最新行情在缓存的位置
		 * 获取指定合约最新的一档行情。
		 * 从最新存储的行情位置向前查找，最远向前查找到前边n（主力合约个数）个元素
		 */
		depthMarketData* GetLastData(const char *contract, int32_t last_index);
		void End();


		void ToString(depthMarketData &data);

		// lic
		depthMarketData* GetLastDataForIllegaluser(const char *contract);

	private:
		/*
		 * 与API相关
		 */
		int InitMDApi();
		int udp_client_fd_;
		struct ip_mreq mreq_;   

		/*
		 * 与逻辑处理相关
		 */
		void RelaceInvalidValue(depthMarketData &d);
		int32_t Push(const depthMarketData& md);
		struct vrt_producer  *producer_;
		depthMarketData md_buffer_[DMD_BUFFER_SIZE];
		int32_t dmd_cursor_;
		bool ended_;

		void RevData();
		std::thread *thread_rev_;

		/*
		 * check whether the given contract is dominant.
		 */
		bool IsDominant(const char *contract);
		char dominant_contracts_[MAX_CONTRACT_COUNT][10];
		int max_traverse_count_;
		int  contract_count_;

		QuoteDataSave<depthMarketData> *p_depthMarketData_save_;

		/*
		 *日志相关
		 */
		const char *module_name_;  

		/*
		 * 配置相关
		 */
		void ParseConfig();
		DMDConfig config_;
};

#endif // __DMD_PRODUCER_H__
