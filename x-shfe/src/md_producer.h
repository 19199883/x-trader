#ifndef __MD_PRODUCER_H__
#define __MD_PRODUCER_H__

#include <functional>
#include <array>
#include <thread>         
#include <chrono>        
#include "vrt_value_obj.h"
#include "quote_interface_shfe_my.h"
#include <tinyxml.h>
#include <tinystr.h>

/*
 * 10 power of 2
 */
#define MD_BUFFER_SIZE 2048 

struct Mdconfig
{
	// disruptor yield strategy
	char yield[20];
};

class MDProducer
{
	public:
		MDProducer(struct vrt_queue  *queue);
		~MDProducer();

		MYShfeMarketData * GetShfeMarketData(int32_t index);
		void End();

	private:
		MYQuoteData* build_quote_provider(SubscribeContracts &subscription);
		
		void OnShfeMarketData(const MYShfeMarketData * md);
		int32_t push(const MYShfeMarketData & md);

		MYQuoteData *md_provider_;
		SubscribeContracts subs_;
		const char *module_name_;  
		bool ended_;
		Mdconfig config_;
		void ParseConfig();

		struct vrt_producer  *producer_;
		std::array<MYShfeMarketData, MD_BUFFER_SIZE> shfemarketdata_buffer_;
};

#endif
