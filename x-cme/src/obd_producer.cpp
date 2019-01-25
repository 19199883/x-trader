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
#include "obd_producer.h"
#include "quote_cmn_utility.h"
#include <tinyxml.h>
#include <tinystr.h>

using namespace std::placeholders;
using namespace std;

orderbookData* OBDProducerHelper::GetLastDataImp(const char *contract, int32_t last_index,
	orderbookData *buffer, int32_t buffer_size,int32_t traverse_count) 
{
	orderbookData* data = NULL;

	// 从缓存最新位置向前查找13个位置（假设有13个主力合约），找到即停
	int i = 0;
	for(; i<traverse_count; i++){
		int data_index = last_index - i;
		if(data_index < 0){
			data_index = data_index + buffer_size;
		}

		orderbookData &tmp = buffer[data_index];
		if(strcmp(contract, tmp.name)==0){
			data = &tmp; 
			break;
		}
	}

	return data;
}

OBDProducer::OBDProducer(struct vrt_queue  *queue) : module_name_("OBDProducer")
{
	obd_cursor_ = OBD_BUFFER_SIZE - 1;
	ended_ = false;
	clog_warning("[%s] OBD_BUFFER_SIZE:%d;",module_name_,OBD_BUFFER_SIZE);

	ParseConfig();

	// init dominant contracts
	memset(dominant_contracts_, 0, sizeof(dominant_contracts_));
	contract_count_ = LoadDominantContracts(config_.contracts_file, dominant_contracts_);
	max_traverse_count_ = contract_count_ * 4;

#ifdef PERSISTENCE_ENABLED 
    p_orderbookData_save_ = new QuoteDataSave<orderbookData>("orderbookData", ORDERBOOKDATA_QUOTE_TYPE);
#endif

	memset(&md_buffer_, 0, sizeof(md_buffer_));

	this->producer_ = vrt_producer_new("obd_producer", 1, queue);
	clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0){
		this->producer_ ->yield	= vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		this->producer_ ->yield	= vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		this->producer_ ->yield	 = vrt_yield_strategy_hybrid();
	}
	thread_rev_ = new std::thread(&OBDProducer::RevData, this);
}

void OBDProducer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    

	// yield strategy
    TiXmlElement *disruptor_node = RootElement->FirstChildElement("Disruptor");
	if (disruptor_node != NULL){
		strcpy(config_.yield, disruptor_node->Attribute("yield"));
	} else { clog_error("[%s] x-shmd.config error: Disruptor node missing.", module_name_); }

    TiXmlElement *l1md_node = RootElement->FirstChildElement("orderbookData");
	if (l1md_node != NULL){
		strcpy(config_.mcIp, l1md_node->Attribute("mcIp"));
		int mcPort = 0;
		 l1md_node->QueryIntAttribute("mcPort", &mcPort);
		this->config_.mcPort = mcPort;
	} else{
		clog_error("[%s] x-trader.config error: orderbookData node missing.", module_name_); 
	}
	
	// contracts file
    TiXmlElement *contracts_file_node = RootElement->FirstChildElement("Subscription");
	if (contracts_file_node != NULL){
		strcpy(config_.contracts_file, contracts_file_node->Attribute("contracts"));
	} else { clog_error("[%s] x-shmd.config error: Subscription node missing.", module_name_); }
}

OBDProducer::~OBDProducer()
{
#ifdef PERSISTENCE_ENABLED 
    if (p_orderbookData_save_) delete p_orderbookData_save_;
#endif
}


int32_t OBDProducer::Push(const orderbookData& md){
	obd_cursor_++;
	if (obd_cursor_ % OBD_BUFFER_SIZE == 0){
		obd_cursor_ = 0;
	}
	md_buffer_[obd_cursor_] = md;
	return obd_cursor_;
}

orderbookData* OBDProducer::GetData(int32_t index)
{
	return &md_buffer_[index];
}

// lic
orderbookData* OBDProducer::GetLastDataForIllegaluser(const char *contract)
{
	orderbookData* data = OBDProducerHelper::GetLastDataImp(
		contract,0,md_buffer_,OBD_BUFFER_SIZE,OBD_BUFFER_SIZE);
	return data;
}

orderbookData* OBDProducer::GetLastData(const char *contract, int32_t last_index)
{
	orderbookData* data = OBDProducerHelper::GetLastDataImp(
		contract,last_index,md_buffer_,OBD_BUFFER_SIZE,max_traverse_count_);
	return data;
}

bool OBDProducer::IsDominant(const char *contract)
{
#ifdef PERSISTENCE_ENABLED 
	// 持久化行情时，需要记录所有合约
	clog_warning("[%s] return TRUE in IsDominant.",module_name_);
	return true;
#else
	return IsDominantImp(contract, dominant_contracts_, contract_count_);
#endif
}

void OBDProducer::ToString(orderbookData &data)
{
	clog_info("[%s] orderbookData\n"
		"name:%s \n"
		"transactTime:%lu \n"
		"(price:%f, size:%d)\n",			
		module_name_,
		data.name,
		data.transactTime,
		data.price, data.orderQty);
}

int OBDProducer::InitMDApi()
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

void OBDProducer::RevData()
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

		orderbookData* md = (orderbookData*)(buf);
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
		ivalue->data = Orderbook_MD;
		vrt_producer_publish(producer_);


#ifdef PERSISTENCE_ENABLED 
		timeval t;
		gettimeofday(&t, NULL);
		p_orderbookData_save_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, md);
#endif

    } // end while (!ended_) 
	clog_warning("[%s] RevData exit.",module_name_);

}

void OBDProducer::RelaceInvalidValue(orderbookData &d)
{
    //d.Turnover = InvalidToZeroD(d.Turnover);
    //d.LastPrice = InvalidToZeroD(d.LastPrice);
}


void OBDProducer::End()
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


