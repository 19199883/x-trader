#ifndef __L1MD_PRODUCER_H__
#define __L1MD_PRODUCER_H__

#include <functional>
#include <array>
#include <string>
#include <thread>         
#include <chrono>        
#include "vrt_value_obj.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "mdclient.h"
#include "quote_datatype_level1.h"

// TODO:因消费者需要合并一档与全挡数据，所以需要考虑消费者如何引用缓存在一档生产者的数据，一档数据与全挡数据合并后，需清空



/*
 * 10 power of 2
 */
#define L1MD_BUFFER_SIZE 1000 

using namespace std;

struct L1MDConfig 
{
	string addr;
	string ip[30];
	int port;
	string contracts_file[500];
	string yield[20]; // disruptor yield strategy
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
		int32_t push(const CDepthMarketDataField& md);

		SubscribeContracts subs_;
		const char *module_name_;  
		bool ended_;
		struct vrt_producer  *producer_;
		std::array<CDepthMarketDataField, MD_BUFFER_SIZE> md_buffer_;

		/*
		 * 配置相关
		 */
		void ParseConfig();
		L1MDConfig config_;
};

#endif
