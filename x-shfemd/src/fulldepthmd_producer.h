#ifndef __FULLDEPTHMD_PRODUCER_H__
#define __FULLDEPTHMD_PRODUCER_H__

#include <functional>
#include <array>
#include <string>
#include <thread>         
#include <chrono>        
#include "vrt_value_obj.h"
#include "quote_interface_shfe_my.h"
#include <tinyxml.h>
#include <tinystr.h>

/*
 * 缓存的最大的行情数量
 */
#define FULL_DEPTH_MD_BUFFER_SIZE 8192 

using namespace std;

struct FulldepthMDConfig 
{
	string addr;
	char ip[30];
	int port;
	char contracts_file[500];
	char yield[20]; // disruptor yield strategy
};

class FullDepthMDProducer
{
	public:
		FullDepthMDProducer(struct vrt_queue  *queue);
		~FullDepthMDProducer();

		MDPackEx* GetData(int32_t index);
		void End();

	private:
		/*
		 * 与API相关
		 */
		int InitMDApi();
		void RevData();

		/*
		 * 逻辑相关
		 */
		std::thread thread_rev_;

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
		std::string ToString(const MDPack &d);
		const char *module_name_;  

		/*
		 * 配置相关
		 */
		FulldepthMDConfig config_;
		void ParseConfig();
};

#endif
