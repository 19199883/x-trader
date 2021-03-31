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
#include "ZceLevel2ApiStruct.h"

/*
 * 缓存的最大的行情数量
 */
#define L2MD_BUFFER_SIZE 8192 

// socket RCVBUF size
#define RCV_BUF_SIZE	212992

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

		StdQuote5* GetData(int32_t index);
		void End();
		/*
		 * check whether the given contract is dominant.
		 */
		bool IsDominant(const char *contract);

	private:
		/*
		 * 与API相关
		 */
		int InitMDApi();
		int udp_client_fd_;

		/*
		 * 逻辑相关
		 */
		void RevData();

		std::thread *thread_rev_;
		int32_t dominant_contract_count_;
		char dominant_contracts_[MAX_CONTRACT_COUNT][10];

		/*
		 *disruptor相关
		 */
		int32_t Push();
		void on_receive_quote(int32_t index);
		struct vrt_producer  *producer_;
		StdQuote5 md_buffer_[L2MD_BUFFER_SIZE];
		bool ended_;

		/*
		 *日志相关
		 */
		const char *module_name_;  

		/*
		 * 配置相关
		 */
		L2MDConfig config_;
		void ParseConfig();
};

#endif
