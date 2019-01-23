#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <functional>   // std::bind
#include "dmd_producer.h"
#include "quote_cmn_utility.h"
#include <tinyxml.h>
#include <tinystr.h>

using namespace std::placeholders;
using namespace std;

depthMarketData* DMDProducerHelper::GetLastDataImp(const char *contract, int32_t last_index,
	depthMarketData *buffer, int32_t buffer_size,int32_t traverse_count) 
{
	depthMarketData* data = NULL;

	// 全息行情需要一档行情时，从缓存最新位置向前查找13个位置（假设有13个主力合约），找到即停
	int i = 0;
	for(; i<traverse_count; i++){
		int data_index = last_index - i;
		if(data_index < 0){
			data_index = data_index + buffer_size;
		}

		depthMarketData &tmp = buffer[data_index];
		if(strcmp(contract, tmp.name)==0){
			data = &tmp; 
			break;
		}
	}

	return data;
}

DMDProducer::DMDProducer(struct vrt_queue  *queue) : module_name_("DMDProducer")
{
	data_cursor_ = DMD_BUFFER_SIZE - 1;
	ended_ = false;
    api_ = NULL;
	clog_warning("[%s] DMD_BUFFER_SIZE:%d;",module_name_,DMD_BUFFER_SIZE);

	ParseConfig();

	// init dominant contracts
	memset(dominant_contracts_, 0, sizeof(dominant_contracts_));
	contract_count_ = LoadDominantContracts(config_.contracts_file, dominant_contracts_);
	max_traverse_count_ = contract_count_ * 4;

#ifdef PERSISTENCE_ENABLED 
    p_depthMarketData_save_ = new QuoteDataSave<depthMarketData>("depthMarketData", DEPTHMARKETDATA_QUOTE_TYPE);
#endif

	memset(&md_buffer_, 0, sizeof(md_buffer_));

	this->producer_ = vrt_producer_new("dmd_producer", 1, queue);
	clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0){
		this->producer_ ->yield	= vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		this->producer_ ->yield	= vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		this->producer_ ->yield	 = vrt_yield_strategy_hybrid();
	}
	thread_rev_ = new std::thread(&FullDepthMDProducer::RevData, this);
}

void DMDProducer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    

	// yield strategy
    TiXmlElement *disruptor_node = RootElement->FirstChildElement("Disruptor");
	if (disruptor_node != NULL){
		strcpy(config_.yield, disruptor_node->Attribute("yield"));
	} else { clog_error("[%s] x-shmd.config error: Disruptor node missing.", module_name_); }

    TiXmlElement *l1md_node = RootElement->FirstChildElement("depthMarketData");
	if (l1md_node != NULL){
		strcpy(config_.mcIp, l1md_node->Attribute("mcIp"));
		int mcPort = 0;
		 l1md_node->QueryIntAttribute("mcPort", &mcPort);
		this->config_.mcPort = mcPort;
	} else{
		clog_error("[%s] x-trader.config error: depthMarkerData node missing.", module_name_); 
	}
	
	// contracts file
    TiXmlElement *contracts_file_node = RootElement->FirstChildElement("Subscription");
	if (contracts_file_node != NULL){
		strcpy(config_.contracts_file, contracts_file_node->Attribute("contracts"));
	} else { clog_error("[%s] x-shmd.config error: Subscription node missing.", module_name_); }
}

DMDProducer::~DMDProducer()
{
#ifdef PERSISTENCE_ENABLED 
    if (p_depthMarketData_save_) delete p_depthMarketData_save_;
#endif
}


int32_t DMDProducer::Push(const depthMarketData& md){
	dmd_cursor_++;
	if (dmd_cursor_ % DMD_BUFFER_SIZE == 0){
		dmd_cursor_ = 0;
	}
	md_buffer_[dmd_cursor_] = md;
	return dmd_cursor_;
}

depthMarketData* DMDProducer::GetData(int32_t index)
{
	return &md_buffer_[index];
}

// lic
depthMarketData* DMDProducer::GetLastDataForIllegaluser(const char *contract)
{
	depthMarketData* data = DMDProducerHelper::GetLastDataImp(
		contract,0,md_buffer_,DMD_BUFFER_SIZE,DMD_BUFFER_SIZE);
	return data;
}

depthMarketData* DMDProducer::GetLastData(const char *contract, int32_t last_index)
{
	depthMarketData* data = DMDProducerHelper::GetLastDataImp(
		contract,last_index,md_buffer_,DMD_BUFFER_SIZE,max_traverse_count_);
	return data;
}

bool DMDProducer::IsDominant(const char *contract)
{
#ifdef PERSISTENCE_ENABLED 
	// 持久化行情时，需要记录所有合约
	clog_warning("[%s] return TRUE in IsDominant.",module_name_);
	return true;
#else
	return IsDominantImp(contract, dominant_contracts_, contract_count_);
#endif
}

void DMDProducer::ToString(depthMarketData &data)
{
	clog_info("[%s] depthMarketData\n"
		"name:%s \n"
		"transactTime:%lu \n"
		"bid1(price:%f, size:%d, numberOfOrders:%d)\n"
		"bid2(price:%f, size:%d, numberOfOrders:%d)\n"
		"bid3(price:%f, size:%d, numberOfOrders:%d)\n"
		"bid9(price:%f, size:%d, numberOfOrders:%d)\n"
		"bid10(price:%f, size:%d, numberOfOrders:%d)\n"
		"ask1(price:%f, size:%d, numberOfOrders:%d)\n"
		"ask2(price:%f, size:%d, numberOfOrders:%d)\n"
		"ask3(price:%f, size:%d, numberOfOrders:%d)\n"
		"ask9(price:%f, size:%d, numberOfOrders:%d)\n"
		"ask10(price:%f, size:%d, numberOfOrders:%d)\n",			
		module_name_,
		data.name,
		data.transactTime,
		data.bid[0].price, data.bid[0].size, data.bid[0].numberOfOrders,
		data.bid[1].price, data.bid[1].size, data.bid[1].numberOfOrders,
		data.bid[2].price, data.bid[2].size, data.bid[2].numberOfOrders,
		data.bid[8].price, data.bid[8].size, data.bid[8].numberOfOrders,
		data.bid[9].price, data.bid[9].size, data.bid[9].numberOfOrders,		
		data.ask[0].price, data.ask[0].size, data.ask[0].numberOfOrders,
		data.ask[1].price, data.ask[1].size, data.ask[1].numberOfOrders,
		data.ask[2].price, data.ask[2].size, data.ask[2].numberOfOrders,
		data.ask[8].price, data.ask[8].size, data.ask[8].numberOfOrders,
		data.ask[9].price, data.ask[9].size, data.ask[9].numberOfOrders
		);
}

void DMDProducer::InitMDApi()
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
    servaddr.sin_addr.s_addr = inet_addr(config_.mcIp);
    servaddr.sin_port = htons(config_.mcPort);
	clog_warning("[%s] UDP - bind:%s:%d",module_name_,config_.mcIp,config_.mcPort);
    if (bind(udp_client_fd, (sockaddr *) &servaddr, sizeof(servaddr)) != 0){
        clog_error("[%s] UDP - bind failed:%s:%d",module_name_,config_.mcIp,config_.mcPort);
        return -1;
    }

// 查哪种模式最时时
    // set nonblock flag
//    int socket_ctl_flag = fcntl(udp_client_fd, F_GETFL);
//    if (socket_ctl_flag < 0)
//    {
//        MY_LOG_WARN("UDP - get socket control flag failed.");
//    }
//    if (fcntl(udp_client_fd, F_SETFL, socket_ctl_flag | O_NONBLOCK) < 0)
//    {
//        MY_LOG_WARN("UDP - set socket control flag with nonblock failed.");
//    }

    // set buffer length
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

void DMDProducer::RevData()
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

		depthMarketData* md = (depthMarketData*)(buf);
		RelaceInvalidValue(*md);
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
		ivalue->data = Depth_MD;
		vrt_producer_publish(producer_);


#ifdef PERSISTENCE_ENABLED 
		timeval t;
		gettimeofday(&t, NULL);
		p_depthMarketData_save_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, md);
#endif
		// debug
		ToString(*data);

    } // end while (!ended_) 
	clog_warning("[%s] RevData exit.",module_name_);

}

void DMDProducer::RelaceInvalidValue(depthMarketData &d)
{
    //d.Turnover = InvalidToZeroD(d.Turnover);
    //d.LastPrice = InvalidToZeroD(d.LastPrice);
}


void DMDProducer::End()
{
	if(!ended_){
		ended_ = true;

		shutdown(udp_fd_, SHUT_RDWR);
		int err = close(udp_fd_);
		clog_warning("close udp:%d.", err); 
		thread_rev_->join();

		vrt_producer_eof(producer_);
		clog_warning("[%s] End exit", module_name_);
	}
	fflush (Log::fp);
}


