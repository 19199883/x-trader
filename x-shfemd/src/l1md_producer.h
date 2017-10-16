#ifndef __L1MD_PRODUCER_H__
#define __L1MD_PRODUCER_H__

#include <functional>
#include <array>
#include <string>
#include <thread>         
#include <chrono>        
#include "vrt_value_obj.h"
#include "mdclient.h"
#include "quote_datatype_level1.h"

/*
 * 10 power of 2
 */
#define L1MD_BUFFER_SIZE 1000 

using namespace std;

struct L1MDConfig 
{
	string addr;
	char ip[30];
	int port;
	char contracts_file[500];
	char yield[20]; // disruptor yield strategy
};

class L1MDProducer : public CMdclientSpi
{
	public:
		L1MDProducer(struct vrt_queue  *queue);
		~L1MDProducer();

		/*
		 * 与逻辑处理相关
		 */
		CDepthMarketDataField* GetData(int32_t index);

		/*
		 * contract: 要获取行情的合约
		 * last_index;最新行情在缓存的位置
		 * 获取指定合约最新的一档行情。
		 * 从最新存储的行情位置向前查找，最远向前查找到前边n（主力合约个数）个元素
		 */
		CDepthMarketDataField* GetLastData(const char *contract, int32_t last_index);
		CDepthMarketDataField* GetLastDataImp(const char *contract, int32_t last_index,
			CDepthMarketDataField *buffer, int32_t buffer_size);
		void End();

		/*
		 * 与API相关
		 */
		virtual void OnRtnDepthMarketData(CDepthMarketDataField *pDepthMarketData);

	private:
		/*
		 * 与API相关
		 */
		void InitMDApi();
		CMdclientApi *api_;

		/*
		 * 与逻辑处理相关
		 */
		void RalaceInvalidValue_Femas(CDepthMarketDataField &d);
		int32_t Push(const CDepthMarketDataField& md);
		bool ended_;
		struct vrt_producer  *producer_;
		CDepthMarketDataField md_buffer_[L1MD_BUFFER_SIZE];
		int32_t l1data_cursor_;
		// TODO:
		int32_t dominant_contract_count_;

		/*
		 *日志相关
		 */
		const char *module_name_;  

		/*
		 * 配置相关
		 */
		void ParseConfig();
		L1MDConfig config_;
};

#endif
