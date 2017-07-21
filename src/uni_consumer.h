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

#define STRA_TABLE_SIZE 512 

class UniConsumer
{
	public:
		UniConsumer(struct vrt_queue  *queue);
		~UniConsumer();

	private:
		const std::string module_name_;  
		struct vrt_consumer *consumer_;

		std::array<Strategy, STRA_TABLE_SIZE> stra_table_;
		// key: contract; value: index of strategy in stra_table_
		std::unordered_map<std::string, int32_t> cont_straidx_map_table_;
		// key: strategy id; value: index of strategy in stra_table_
		std::unordered_map<int32_t, int32_t> straid_straidx_map_table_;

		std::list<StrategySetting> strategy_settings_;
		StrategySetting CreateStrategySetting(const TiXmlElement *ele);
		void ParseConfig();
		void CreateStrategies();
};

#endif
