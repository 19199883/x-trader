#ifndef __FULLDEPTHMD_PRODUCER_H__
#define __FULLDEPTHMD_PRODUCER_H__

#include <functional>
#include <array>
#include <thread>         
#include <chrono>        
#include "vrt_value_obj.h"
#include "quote_interface_shfe_my.h"
#include <tinyxml.h>
#include <tinystr.h>

/*
 * 缓存的最大的行情数量
 */
#define FULL_DEPTH_MD_BUFFER_SIZE 1000 

struct FulldepthMDConfig 
{
	string addr;
	string ip[30];
	int port;
	string contracts_file[500];
	string yield[20]; // disruptor yield strategy
};

class FullDepthMDProducer
{
	public:
		FullDepthMDProducer(struct vrt_queue  *queue);
		~FullDepthMDProducer();

		MYShfeMarketData * GetShfeMarketData(int32_t index);
		void End();

	private:
		/*
		 * 与API相关
		 */
		int CreateUdpFD(const std::string &addr_ip, unsigned short port);
		void ShfeMBLHandler();

		void proc_udp_data(MDPackEx &data);
		
		void OnShfeMarketData(const MYShfeMarketData * md);
		int32_t push(const MYShfeMarketData & md);

		MYQuoteData *md_provider_;
		SubscribeContracts subs_;
		const char *module_name_;  
		bool ended_;
		FulldepthMDConfig config_;
		void ParseConfig();
		std::thread *p_mbl_handler_;
		std::string ToString(const MDPack &d);
		int server_;
		int seq_no_;

		struct vrt_producer  *producer_;
		std::array<MYShfeMarketData, MD_BUFFER_SIZE> shfemarketdata_buffer_;
};

#endif
