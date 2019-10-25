#ifndef __FULLDEPTHMD_PRODUCER_H__
#define __FULLDEPTHMD_PRODUCER_H__

#include <functional>
#include <array>
#include <string>
#include <thread>         
#include <chrono>        
#include "vrt_value_obj.h"
#include "quote_datatype_shfe_deep.h"
#include <tinyxml.h>
#include <tinystr.h>

#ifdef TCPDIRECT

	#include "utils.h"
	
	#include <zf/zf.h>
	#include <etherfabric/vi.h>
	#include <etherfabric/pio.h>
	#include <etherfabric/pd.h>
	#include <etherfabric/memreg.h>
	
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <net/if.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <ifaddrs.h>
	#include <stdbool.h>
	#include <netdb.h>
	#include <pthread.h>
	
	#define MTU                   1500
	#define MAX_ETH_HEADERS       (14/*ETH*/ + 4/*802.1Q*/)
	#define MAX_IP_TCP_HEADERS    (20/*IP*/ + 20/*TCP*/ + 12/*TCP options*/)
	#define MAX_PACKET            (MTU + MAX_ETH_HEADERS)
	#define MAX_MESSAGE           (MTU - MAX_IP_TCP_HEADERS)

		
	static int         cfg_rx_size = 2048;
	static int         cfg_tx_size = 200;
	static const char* cfg_port = "10078";
	static const char* cfg_mcast_addr = "172.18.35.255";


	struct client_state {
	  struct zf_stack*             stack;
	  struct zfur*                 udp_sock;
	  ef_pd                        pd;
	  ef_driver_handle             dh;
	  char*                        msg_buf;
	  int                          msg_len;
	  char                         pkt_buf[MAX_PACKET];
	  char                         recv_buf[MTU];
	};
#endif
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
		int Init();

		/*
		 * 逻辑相关
		 */
#ifdef TCPDIRECT
		int poll_udp_rx(struct client_state* cs);
		void Init( struct client_state* cs, 
				const char* interface,
				const char* port);
#endif

		void Process(MDPack *data);
		void RevData();
		std::thread *thread_rev_;
		int32_t dominant_contract_count_;
		char dominant_contracts_[MAX_CONTRACT_COUNT][10];

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
		int udp_fd_;

		/*
		 * 配置相关
		 */
		FulldepthMDConfig config_;
		void ParseConfig();
};

#endif
