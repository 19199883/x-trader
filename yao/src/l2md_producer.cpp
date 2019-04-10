// done
#include <functional>   // std::bind
#include<unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "l2md_producer.h"
#include "quote_cmn_utility.h"
#include "perfctx.h"

using namespace std;
using namespace std::placeholders;
using std::chrono::system_clock;

// done
L2MDProducer::L2MDProducer(struct vrt_queue *queue)
	: module_name_("L2MDProducer")
{
	ended_ = false;
	clog_warning("[%s] L2_MD_BUFFER_SIZE: %d;", module_name_, L2MD_BUFFER_SIZE);

	ParseConfig();
	udp_client_fd_ = 0;

	// init dominant contracts
	memset(dominant_contracts_, 0, sizeof(dominant_contracts_));
	dominant_contract_count_ = LoadDominantContracts(config_.contracts_file, dominant_contracts_);
	clog_warning("[%s] dominant_contract_count_:%d;", module_name_, dominant_contract_count_);

	// disruptor
	this->producer_ = vrt_producer_new("fulldepthmd_producer", 1, queue);
	clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0){
		producer_ ->yield = vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		producer_ ->yield = vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		producer_ ->yield = vrt_yield_strategy_hybrid();
	}

	thread_rev_ = new std::thread(&L2MDProducer::RevData, this);
	//thread_rev_->detach();
}

// done
void L2MDProducer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    

	// yield strategy
    TiXmlElement *disruptor_node = RootElement->FirstChildElement("Disruptor");
	if (disruptor_node != NULL){
		strcpy(config_.yield, disruptor_node->Attribute("yield"));
	} else { clog_error("[%s] x-shmd.config error: Disruptor node missing.", module_name_); }

	// addr
    TiXmlElement *fdmd_node = RootElement->FirstChildElement("L2Md");
	if (fdmd_node != NULL){
		config_.addr = fdmd_node->Attribute("addr");
	} else { clog_error("[%s] x-shmd.config error: L2Md node missing.", module_name_); }

	// contracts file
    TiXmlElement *contracts_file_node = RootElement->FirstChildElement("Subscription");
	if (contracts_file_node != NULL){
		strcpy(config_.contracts_file, contracts_file_node->Attribute("contracts"));
	} else { clog_error("[%s] x-shmd.config error: Subscription node missing.", module_name_); }

	size_t ipstr_start = config_.addr.find("//")+2;
	size_t ipstr_end = config_.addr.find(":",ipstr_start);
	strcpy(config_.ip, config_.addr.substr(ipstr_start,ipstr_end-ipstr_start).c_str());
	config_.port = stoi(config_.addr.substr(ipstr_end+1));
}

// done
L2MDProducer::~L2MDProducer()
{
}

// done
int L2MDProducer::InitMDApi()
{
    // init udp socket
    int udp_client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    /* set reuse and non block for this socket */
    int son = 1;
    setsockopt(udp_client_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&son, sizeof(son));

    // bind address and port
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; //IPv4协议
    servaddr.sin_addr.s_addr = inet_addr(config_.ip);
    servaddr.sin_port = htons(config_.port);
	clog_warning("[%s] UDP - bind:%s:%d",module_name_,config_.ip,config_.port);
    if (bind(udp_client_fd, (sockaddr *) &servaddr, sizeof(servaddr)) != 0){
        clog_error("[%s] UDP - bind failed:%s:%d",module_name_,config_.ip,config_.port);
        return -1;
    }

#ifdef MD_UDP_NOBLOCK
    // set nonblock flag
    int socket_ctl_flag = fcntl(udp_client_fd, F_GETFL);
    if (socket_ctl_flag < 0){
        MY_LOG_WARN("UDP - get socket control flag failed.");
    }
    if (fcntl(udp_client_fd, F_SETFL, socket_ctl_flag | O_NONBLOCK) < 0){
        MY_LOG_WARN("UDP - set socket control flag with nonblock failed.");
    }
#endif

    int rcvbufsize = 1 * 1024 * 1024;
    int ret = setsockopt(udp_client_fd,SOL_SOCKET,SO_RCVBUF,
				(const void *)&rcvbufsize,sizeof(rcvbufsize));
    if (ret != 0){
        clog_error("[%a] UDP - set SO_RCVBUF failed.",module_name_);
    }

    int broadcast_on = 1;
    ret = setsockopt(udp_client_fd, SOL_SOCKET, SO_BROADCAST, &broadcast_on, sizeof(broadcast_on));
    if (ret != 0){
        clog_error("[%s] UDP - set SO_RCVBUF failed.",module_name_);
    }

    return udp_client_fd;
}

void L2MDProducer::RevData()
{
	int udp_fd = InitMDApi();
	udp_client_fd_ = udp_fd;
    if (udp_fd < 0){
        clog_error("[%s] InitMDApi failed.",module_name_);
        return;
    }
	clog_warning("[%s] InitMDApi succeeded.",module_name_);

    char buf[2048];
    int data_len = 0;
    sockaddr_in src_addr;
    unsigned int addr_len = sizeof(sockaddr_in);
    while (!ended_){
        data_len = recvfrom(udp_fd, buf, 2048, 0, (sockaddr *)&src_addr, &addr_len);
        if (data_len == -1) {
            int error_no = errno;
			clog_error("[%s] UDP-recvfrom failed, error_no=%d.",module_name_,error_no);
			fflush (Log::fp);
            if (error_no == 0 || error_no == 251 || 
				error_no == EWOULDBLOCK) {/*251 for PARisk */ //20060224 IA64 add 0
                continue;
            }else{
                continue;
            }
        }

		StdQuote5* md = (StdQuote5 *)(buf);

		bool dominant = IsDominant(md->instrument);
	//	clog_info("[test] StdQuote5 rev [%s]dominant:%d contract:%s, time:%s %d", module_name_, 
	//		dominant, md->instrument, md->updateTime, md->updateMS);

		// 抛弃非主力合约
		if(!dominant) continue;
#ifdef LATENCY_MEASURE
		 static int cnt = 0;
		 perf_ctx::insert_t0(cnt);
		 cnt++;
#endif

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		vrt_producer_claim(producer_, &vvalue);
		ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
		ivalue->index = Push(*md);
		ivalue->data = L2_MD;
		vrt_producer_publish(producer_);


		clog_info("[test] after push StdQuote5 [%s]dominant:%d contract:%s, time:%s %d,idx:%d", 
					module_name_, dominant, md->instrument, md->updateTime, md->updateMS,ivalue->index);

    } // end while (!ended_) 
	clog_warning("[%s] RevData exit.",module_name_);

}

void L2MDProducer::End()
{
	if(!ended_){
		ended_ = true;
		shutdown(udp_client_fd_, SHUT_RDWR);
		int err = close(udp_client_fd_);
		clog_warning("close udp:%d.", err); 
		fflush (Log::fp);

		thread_rev_->join();
		vrt_producer_eof(producer_);
		clog_warning("[%s] End exit", module_name_);
	}

	fflush (Log::fp);
}

int32_t L2MDProducer::Push(const StdQuote5& md){
	static int32_t md_cursor = L2MD_BUFFER_SIZE - 1;
	md_cursor++;
	if (md_cursor % L2MD_BUFFER_SIZE == 0){
		md_cursor = 0;
	}
	md_buffer_[md_cursor] = md;

	return md_cursor;
}

StdQuote5* L2MDProducer::GetData(int32_t index)
{
	return &md_buffer_[index];
}

bool L2MDProducer::IsDominant(const char *contract)
{
#ifdef PERSISTENCE_ENABLED 
	// 持久化行情时，需要记录所有合约
	//clog_warning("[%s] %s, return TRUE in IsDominant.",module_name_,contract);
	return true;
#else
	return IsDominantImp(contract, dominant_contracts_, dominant_contract_count_);
#endif
}

