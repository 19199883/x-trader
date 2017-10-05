#ifndef __MD_PRODUCER_H__
#define __MD_PRODUCER_H__

#include <functional>
#include <array>
#include <thread>         
#include <chrono>        
#include "vrt_value_obj.h"
#include "quote_datatype_czce_level2.h"
#include <tinyxml.h>
#include <tinystr.h>

/*
 * 10 power of 2
 */
#define MD_BUFFER_SIZE 1000 

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

		ZCEL2QuotSnapshotField_MY* GetL2QuoteSnapshot(int32_t index);
		void End();

	private:
		MYQuoteData* build_quote_provider(SubscribeContracts &subscription);
		
		void OnMD(const ZCEL2QuotSnapshotField_MY* md);
		int32_t push(const ZCEL2QuotSnapshotField_MY& md);

		MYQuoteData *md_provider_;
		SubscribeContracts subs_;
		const char *module_name_;  
		bool ended_;
		Mdconfig config_;
		void ParseConfig();

		struct vrt_producer  *producer_;
		std::array<ZCEL2QuotSnapshotField_MY, MD_BUFFER_SIZE> l2quotesnapshot_buffer_;
};

#endif
