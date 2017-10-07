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
		MDProducer(const ConfigData &cfg,struct vrt_queue  *queue);
		~MDProducer();

		MYShfeMarketData * GetShfeMarketData(int32_t index);
		void End();

		virtual void OnRtnDepthMarketData(CDepthMarketDataField *pDepthMarketData);

	private:
		void InitMDApi(const ConfigData &cfg);
		void RalaceInvalidValue_Femas(CDepthMarketDataField &d);

		void OnShfeMarketData(const MYShfeMarketData * md);
		int32_t push(const MYShfeMarketData & md);

		SubscribeContracts subs_;
		const char *module_name_;  
		bool ended_;
		Mdconfig config_;
		void ParseConfig();
		ConfigData cfg_;
	CMdclientApi *api_;

		struct vrt_producer  *producer_;
		std::array<MYShfeMarketData, MD_BUFFER_SIZE> shfemarketdata_buffer_;
};

#endif
