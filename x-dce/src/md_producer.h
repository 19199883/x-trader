#ifndef __MD_PRODUCER_H__
#define __MD_PRODUCER_H__

#include <functional>
#include <array>
#include <thread>         
#include <chrono>        
#include <string>        
#include "vrt_value_obj.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "quote_datatype_dce_level2.h"
#include "quote_cmn_save.h"
#include "quote_cmn_utility.h"

/*
 * 10 power of 2
 */
#define MD_BUFFER_SIZE 1000 

using namespace std;

struct Mdconfig
{
	string addr;
	char ip[30];
	int port;
	char contracts_file[500];
	char yield[20]; // disruptor yield strategy
};

enum EDataType
{
    eMDBestAndDeep = 0,
    eArbiBestAndDeep,
    eMDTenEntrust,
    eMDRealTimePrice,
    eMDOrderStatistic,
    eMDMarchPriceQty,
};

class MDProducer
{
	public:
		MDProducer(struct vrt_queue  *queue);
		~MDProducer();

		MDBestAndDeep_MY* GetBestAnddeep(int32_t index);
		MDOrderStatistic_MY* GetOrderStatistic(int32_t index);

		void End();
		/*
		 * check whether the given contract is dominant.
		 */
		bool IsDominant(const char *contract);

		// lic
		CDepthMarketDataField* GetLastDataForIllegaluser(const char *contract);

	private:
		int32_t Push(const MDBestAndDeep_MY& md);
		int32_t Push(const MDOrderStatistic_MY& md);

		const char *module_name_;  
		int udp_fd_;

		bool ended_;
		Mdconfig config_;
		void ParseConfig();

		MDBestAndDeep_MY bestanddeep_;
		MDOrderStatistic_MY orderstatistic_;

		int InitMDApi();

		void RevData();
		std::thread *thread_rev_;
		int32_t dominant_contract_count_;
		char dominant_contracts_[20][10];

		struct vrt_producer  *producer_;
		std::array<MDOrderStatistic_MY, MD_BUFFER_SIZE> orderstatistic_buffer_;
		std::array<MDBestAndDeep_MY, MD_BUFFER_SIZE> bestanddeep_buffer_;

#ifdef PERSISTENCE_ENABLED 
	QuoteDataSave<MDBestAndDeep_MY> *p_save_best_and_deep_;
	QuoteDataSave<MDOrderStatistic_MY> *p_save_order_statistic_;

#endif
};

#endif
