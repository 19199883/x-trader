#ifndef __UNI_CONSUMER_H__
#define __UNI_CONSUMER_H__

#include <functional>
#include <array>
#include <string>
#include <list>
#include <unordered_map>
#include <libcork/core.h>
#include <libcork/helpers/errors.h>
#include <vrt.h>
#include "vrt_value_obj.h"
#include "strategy.h"
#include "md_producer.h"

#define STRA_TABLE_SIZE 512 
#define SIG_BUFFER_SIZE 32 

class UniConsumer
{
	public:
		UniConsumer(struct vrt_queue  *queue, MDProducer *md_producer);
		~UniConsumer();

		void start();

	private:
		bool running_;
		const std::string module_name_;  
		struct vrt_consumer *consumer_;
		MDProducer *md_producer_;

		std::array<Strategy, STRA_TABLE_SIZE> stra_table_;
		// key: contract; value: indices of strategies in stra_table_
		std::unordered_multimap<std::string, int32_t> cont_straidx_map_table_;
		// key: strategy id; value: index of strategy in stra_table_
		std::unordered_map<int32_t, int32_t> straid_straidx_map_table_;

		std::list<StrategySetting> strategy_settings_;
		StrategySetting CreateStrategySetting(const TiXmlElement *ele);
		void ParseConfig();
		void CreateStrategies();

		// business logic
		void ProcBestAndDeep(int32_t index);
		void FeedBestAndDeep(int32_t straidx);
		void ProcOrderStatistic(int32_t index);
		void ProcPendingSig(int32_t index);
		void ProcSigs(int32_t sig_cnt, signal_t *sigs);
		void ProcTunnRpt(int32_t index);
		std::array<signal_t, SIG_BUFFER_SIZE> sig_buffer_;
		
};

#endif
