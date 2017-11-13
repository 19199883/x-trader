#ifndef __TAP_MD_PRODUCER_H__
#define __TAP_MD_PRODUCER_H__

#include <functional>
#include <array>
#include <thread>         
#include <chrono>        
#include "vrt_value_obj.h"
#include "quote_interface_czce_level2.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "TapQuoteAPI.h"
#include "quote_cmn_utility.h"

/*
 * 10 power of 2
 */
#define MD_BUFFER_SIZE 1000 

struct Mdconfig
{
	char user[50];
	char password[250];
	char addr[50];
	char ip[30];
	int port;
	char contracts_file[500];
	char yield[20];
};

class TapMDProducer : public ITapQuoteAPINotify
{
	public:
		TapMDProducer(struct vrt_queue  *queue);
		~TapMDProducer();

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
