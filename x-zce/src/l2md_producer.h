#ifndef __L2MD_PRODUCER_H__
#define __L2MD_PRODUCER_H__

#include <functional>
#include <array>
#include <string>
#include <thread>         
#include <chrono>        
#include "vrt_value_obj.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "quote_cmn_utility.h"
#include "quote_interface_tap.h"
#include "ZceLevel2ApiStruct.h"

/*
 * 缓存的最大的行情数量
 */
#define L2MD_BUFFER_SIZE 8192 

using namespace std;

struct L2MDConfig 
{
	string addr;
	char ip[30];
	int port;
	char contracts_file[500];
	char yield[20]; // disruptor yield strategy
};

class L2MDProducer
{
	public:
		L2MDProducer(struct vrt_queue  *queue);
		~L2MDProducer();

		MDPackEx* GetData(int32_t index);
		void End();
		/*
		 * check whether the given contract is dominant.
		 */
		bool IsDominant(const char *contract);

		std::string ToString(const MDPackEx &d);
		std::string ToString(const MDPack &d);

	private:
		/*
		 * 与API相关
		 */
		int InitMDApi();

		/*
		 * 逻辑相关
		 */
		void RevData();
		ZCEL2QuotSnapshotField_MY Convert(const StdQuote5 &other,TapAPIQuoteWhole_MY *tap_data,
			ZCEL2QuotSnapshotField_MY &data);
		std::thread *thread_rev_;
		int32_t dominant_contract_count_;
		char dominant_contracts_[20][10];

		/*
		 *disruptor相关
		 */
		int32_t Push(const MDPackEx& md);
		struct vrt_producer  *producer_;
		std::array<MDPackEx, FULL_DEPTH_MD_BUFFER_SIZE> shfemarketdata_buffer_;
		bool ended_;

		/*
		 *日志相关
		 */
		const char *module_name_;  

		/*
		 * 配置相关
		 */
		FulldepthMDConfig config_;
		void ParseConfig();
};

#endif
