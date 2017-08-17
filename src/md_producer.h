#ifndef __MD_PRODUCER_H__
#define __MD_PRODUCER_H__

#include <functional>
#include <array>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include "vrt_value_obj.h"
#include "quote_interface_dce_level2.h"
#include <tinyxml.h>
#include <tinystr.h>

/*
 * 10 power of 2
 */
#define MD_BUFFER_SIZE 2048 

class MDProducer
{
	public:
		MDProducer(struct vrt_queue  *queue);
		~MDProducer();

		MDBestAndDeep_MY* GetBestAnddeep(int32_t index);
		MDOrderStatistic_MY* GetOrderStatistic(int32_t index);

		void End();

		// TODO: test
		void SendMd()
		{
			MDOrderStatistic_MY md;
			strcpy(md.ContractID, "pp1801");
			for (int i = 0; i < 100; i++){
				std::this_thread::sleep_for (std::chrono::milliseconds(1));
				md.Len = i;
				this->OnMDOrderStatistic(&md);
				clog_debug("[md test] send i:%d; contract:%s", md.Len, md.ContractID);
			}
		}

	private:
		MYQuoteData* build_quote_provider(SubscribeContracts &subscription);
		
		void OnMDBestAndDeep(const MDBestAndDeep_MY* md);
		int32_t push(const MDBestAndDeep_MY& md);

		void OnMDOrderStatistic(const MDOrderStatistic_MY* md);
		int32_t push(const MDOrderStatistic_MY& md);

		MYQuoteData *md_provider_;
		SubscribeContracts subs_;
		const char *module_name_;  

		struct vrt_producer  *producer_;
		std::array<MDOrderStatistic_MY, MD_BUFFER_SIZE> orderstatistic_buffer_;
		std::array<MDBestAndDeep_MY, MD_BUFFER_SIZE> bestanddeep_buffer_;
};

#endif
