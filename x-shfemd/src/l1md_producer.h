#ifndef __L1MD_PRODUCER_H__
#define __L1MD_PRODUCER_H__

#include <functional>
#include <array>
#include <thread>         
#include <chrono>        
#include "vrt_value_obj.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "mdclient.h"
#include "quote_datatype_level1.h"

/*
 * 10 power of 2
 */
#define MD_BUFFER_SIZE 1000 

struct Mdconfig
{
	// disruptor yield strategy
	char yield[20];
};

class MDProducer : public CMdclientSpi
{
	public:
		MDProducer(struct vrt_queue  *queue);
		~MDProducer();

		/*
		 * 与逻辑处理相关
		 */
		MYShfeMarketData * GetShfeMarketData(int32_t index);
		void End();

		/*
		 * 与API相关
		 */
		virtual void OnRtnDepthMarketData(CDepthMarketDataField *pDepthMarketData);

	private:
		/*
		 * 与API相关
		 */
		void InitMDApi(const ConfigData &cfg);
		CMdclientApi *api_;

		/*
		 * 与逻辑处理相关
		 */
		void ParseConfig();
		void RalaceInvalidValue_Femas(CDepthMarketDataField &d);
		int32_t push(const MYShfeMarketData & md);

		SubscribeContracts subs_;
		const char *module_name_;  
		bool ended_;
		Mdconfig config_;
		ConfigData cfg_;
		struct vrt_producer  *producer_;
		std::array<MYShfeMarketData, MD_BUFFER_SIZE> shfemarketdata_buffer_;
};

#endif
