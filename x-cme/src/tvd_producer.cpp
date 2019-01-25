#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <functional>   // std::bind
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "tvd_producer.h"
#include "quote_cmn_utility.h"
#include <tinyxml.h>
#include <tinystr.h>

using namespace std::placeholders;
using namespace std;

tradeVolume* TVDProducerHelper::GetLastDataImp(const char *contract, int32_t last_index,
	tradeVolume *buffer, int32_t buffer_size,int32_t traverse_count) 
{
	tradeVolume* data = NULL;

	// 从缓存最新位置向前查找13个位置（假设有13个主力合约），找到即停
	int i = 0;
	for(; i<traverse_count; i++){
		int data_index = last_index - i;
		if(data_index < 0){
			data_index = data_index + buffer_size;
		}

		tradeVolume &tmp = buffer[data_index];
		if(strcmp(contract, tmp.name)==0){
			data = &tmp; 
			break;
		}
	}

	return data;
}

TVDProducer::TVDProducer(struct vrt_queue  *queue) : module_name_("TVDProducer")
{
	tvd_cursor_ = TVD_BUFFER_SIZE - 1;
	ended_ = false;
	clog_warning("[%s] TVD_BUFFER_SIZE:%d;",module_name_,TVD_BUFFER_SIZE);

	ParseConfig();

	// init dominant contracts
	memset(dominant_contracts_, 0, sizeof(dominant_contracts_));
	contract_count_ = LoadDominantContracts(config_.contracts_file, dominant_contracts_);
	max_traverse_count_ = contract_count_ * 4;

#ifdef PERSISTENCE_ENABLED 
    p_tradeVolumeData_save_ = new QuoteDataSave<tradeVolume>("tradeVolumeData", TRADEVOLUMEDATA_QUOTE_TYPE);
#endif

	memset(&md_buffer_, 0, sizeof(md_buffer_));

	this->producer_ = vrt_producer_new("tvd_producer", 1, queue);
	clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0){
		this->producer_ ->yield	= vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		this->producer_ ->yield	= vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		this->producer_ ->yield	 = vrt_yield_strategy_hybrid();
	}
	thread_rev_ = new std::thread(&TVDProducer::RevData, this);
}

void TVDProducer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    

	// yield strategy
    TiXmlElement *disruptor_node = RootElement->FirstChildElement("Disruptor");
	if (disruptor_node != NULL){
		strcpy(config_.yield, disruptor_node->Attribute("yield"));
	} else { clog_error("[%s] x-shmd.config error: Disruptor node missing.", module_name_); }

    TiXmlElement *l1md_node = RootElement->FirstChildElement("tradeVolumeData");
	if (l1md_node != NULL){
		strcpy(config_.mcIp, l1md_node->Attribute("mcIp"));
		int mcPort = 0;
		 l1md_node->QueryIntAttribute("mcPort", &mcPort);
		this->config_.mcPort = mcPort;
	} else{
		clog_error("[%s] x-trader.config error: tradeVolumeData node missing.", module_name_); 
	}
	
	// contracts file
    TiXmlElement *contracts_file_node = RootElement->FirstChildElement("Subscription");
	if (contracts_file_node != NULL){
		strcpy(config_.contracts_file, contracts_file_node->Attribute("contracts"));
	} else { clog_error("[%s] x-shmd.config error: Subscription node missing.", module_name_); }
}

TVDProducer::~TVDProducer()
{
#ifdef PERSISTENCE_ENABLED 
    if (p_tradeVolumeData_save_) delete p_tradeVolumeData_save_;
#endif
}


int32_t TVDProducer::Push(const tradeVolume& md){
	tvd_cursor_++;
	if (tvd_cursor_ % TVD_BUFFER_SIZE == 0){
		tvd_cursor_ = 0;
	}
	md_buffer_[tvd_cursor_] = md;
	return tvd_cursor_;
}

tradeVolume* TVDProducer::GetData(int32_t index)
{
	return &md_buffer_[index];
}

// lic
tradeVolume* TVDProducer::GetLastDataForIllegaluser(const char *contract)
{
	tradeVolume* data = TVDProducerHelper::GetLastDataImp(
		contract,0,md_buffer_,TVD_BUFFER_SIZE,TVD_BUFFER_SIZE);
	return data;
}

tradeVolume* TVDProducer::GetLastData(const char *contract, int32_t last_index)
{
	tradeVolume* data = TVDProducerHelper::GetLastDataImp(
		contract,last_index,md_buffer_,TVD_BUFFER_SIZE,max_traverse_count_);
	return data;
}

bool TVDProducer::IsDominant(const char *contract)
{
#ifdef PERSISTENCE_ENABLED 
	// 持久化行情时，需要记录所有合约
	clog_warning("[%s] return TRUE in IsDominant.",module_name_);
	return true;
#else
	return IsDominantImp(contract, dominant_contracts_, contract_count_);
#endif
}

void TVDProducer::ToString(tradeVolume &data)
{
	clog_info("[%s] tradeVolume\n"
		"name:%s \n"
		"transactTime:%lu \n"
		"(volume:%d)\n",			
		module_name_,
		data.name,
		data.transactTime,
		data.volume);
}

int TVDProducer::InitMDApi()
{
    int udp_client_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_client_fd  < 0){   
        clog_error("[%s] UDP - socket error:%d",module_name_,udp_client_fd);
	}else{
        clog_error("[%s] UDP - socket successful:%d",module_name_,udp_client_fd);
	}

	// 端口复用
	int opt = 1; 
	setsockopt(udp_client_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));

//    int rcvbufsize = 1 * 1024 * 1024;
//    int ret = setsockopt(udp_client_fd,SOL_SOCKET,SO_RCVBUF, (const void *)&rcvbufsize,
//				sizeof(rcvbufsize));
//    if (ret != 0){
//        clog_error("[%s] UDP - set SO_RCVBUF failed.",module_name_);
//    }

    // bind address and port
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; //IPv4协议
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(config_.mcPort);
	clog_warning("[%s] UDP - bind:%s:%d",module_name_,config_.mcIp,config_.mcPort);
    if (bind(udp_client_fd, (sockaddr *) &servaddr, sizeof(servaddr)) < 0){
        clog_error("[%s] UDP - bind failed:%s:%d",module_name_,config_.mcIp,config_.mcPort);
        return -1;
    }

	opt = 1;
	int nb = 0;
	nb = setsockopt(udp_client_fd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&opt, sizeof(opt));
	if(nb < 0){
        clog_error("[%s] UDP - set socket error:%d",module_name_, nb);
	}
	     
	/*加入多播组*/
	mreq_;   
	mreq_.imr_multiaddr.s_addr = inet_addr(config_.mcIp); /*多播地址*/
	mreq_.imr_interface.s_addr = htonl(INADDR_ANY); /*网络接口为默认*/
	/*将本机加入多播组*/
	int err = setsockopt(udp_client_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreq_, sizeof(mreq_));//ASM模型
	if (err < 0){
        clog_error("[%s] UDP - IP_ADD_MEMBERSHIP error:%d",module_name_, nb);
	}

    return udp_client_fd;
}

void TVDProducer::RevData()
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
				clog_warning("[%s] RevData error.",module_name_);
                continue;
            }else{
				clog_warning("[%s] RevData error.",module_name_);
                continue;
            }
        }

		tradeVolume* md = (tradeVolume*)(buf);
		RelaceInvalidValue(*md);
		//
		// debug
		ToString(*md);

		bool dominant = IsDominant(md->name);
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
		ivalue->data = TradeVolume_MD;
		vrt_producer_publish(producer_);


#ifdef PERSISTENCE_ENABLED 
		timeval t;
		gettimeofday(&t, NULL);
		p_tradeVolumeData_save_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, md);
#endif

    } // end while (!ended_) 
	clog_warning("[%s] RevData exit.",module_name_);

}

void TVDProducer::RelaceInvalidValue(tradeVolume &d)
{
    //d.Turnover = InvalidToZeroD(d.Turnover);
    //d.LastPrice = InvalidToZeroD(d.LastPrice);
}


void TVDProducer::End()
{
	if(!ended_){
		ended_ = true;

		int err = setsockopt(udp_client_fd_, IPPROTO_IP, IP_DROP_MEMBERSHIP,&mreq_, sizeof(mreq_));		
		shutdown(udp_client_fd_, SHUT_RDWR);
		err = close(udp_client_fd_);
		clog_warning("[%s] close udp:%d.", module_name_, err); 
		thread_rev_->join();

		vrt_producer_eof(producer_);
		clog_warning("[%s] End exit", module_name_);
	}
	fflush (Log::fp);
}


