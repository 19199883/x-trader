#include <functional>   // std::bind
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "fulldepthmd_producer.h"
#include "quote_cmn_utility.h"

using namespace std::placeholders;
using namespace std;

FullDepthMDProducer::FullDepthMDProducer(struct vrt_queue  *queue)
:module_name_("FullDepthProducer")
{
	udp_fd_ = 0;
	ended_ = false;
	clog_warning("[%s] FULL_DEPTH_MD_BUFFER_SIZE: %d;", module_name_, FULL_DEPTH_MD_BUFFER_SIZE);

	ParseConfig();
	
	// init dominant contracts
	memset(dominant_contracts_, 0, sizeof(dominant_contracts_));
	dominant_contract_count_ = LoadDominantContracts(config_.contracts_file, dominant_contracts_);

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

	thread_rev_ = new std::thread(&FullDepthMDProducer::RevData, this);
}

void FullDepthMDProducer::ParseConfig()
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
    TiXmlElement *fdmd_node = RootElement->FirstChildElement("FullDepthMd");
	if (fdmd_node != NULL){
		config_.addr = fdmd_node->Attribute("addr");
	} else { clog_error("[%s] x-shmd.config error: FulldepthMd node missing.", module_name_); }

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

FullDepthMDProducer::~FullDepthMDProducer()
{
}

int FullDepthMDProducer::InitMDApi()
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

// TODO:查哪种模式最时时
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

void FullDepthMDProducer::RevData()
{
	int udp_fd = InitMDApi();
	udp_fd_ = udp_fd; 
    if (udp_fd < 0) {
        clog_error("[%s] MY_SHFE_MD - CreateUdpFD failed.",module_name_);
        return;
    }else{
        clog_warning("[%s] MY_SHFE_MD - CreateUdpFD succeeded.",module_name_);
	}

    int recv_struct_len = sizeof(MDPack);
    int ary_len = recv_struct_len;
    char *recv_buf = new char[ary_len];
    std::size_t recv_len = 0;
	sockaddr_in fromAddr;
	int nFromLen = sizeof(fromAddr);
    while (!ended_){
        recv_len = recvfrom(udp_fd, recv_buf, ary_len, 0, 
			(sockaddr *)&fromAddr, (socklen_t *)&nFromLen);
        if (recv_len == -1) {
            int error_no = errno;
            if (error_no == 0 || error_no == 251 || 
				error_no == EWOULDBLOCK) {/*251 for PARisk */ //20060224 IA64 add 0
                continue;
            }else{
                clog_error("[%s] UDP-recvfrom failed, error_no=%d.",module_name_,error_no);
                continue;
            }
        }

        MDPack *md = (MDPack *)recv_buf;

		// 解决原油(SC)因序号与上期其它品种的序号是独立的，从而造成数据问题。
		// 解决方法：将sc与其它品种行情分成2种独立行情
		if(md->instrument[0]=='s' && md->instrument[1]=='c'){
			clog_info("[%s] sc, sn=%d.",module_name_,md->seqno);
			struct vrt_value  *vvalue;
			struct vrt_hybrid_value  *ivalue;
			vrt_producer_claim(producer_, &vvalue);
			ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
			ivalue->index = Push(*md);
			ivalue->data = INE_FULL_DEPTH_MD;
			vrt_producer_publish(producer_);
		}else{
			clog_info("[%s] no sc, sn=%d.",module_name_,md->seqno);
			struct vrt_value  *vvalue;
			struct vrt_hybrid_value  *ivalue;
			vrt_producer_claim(producer_, &vvalue);
			ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
			ivalue->index = Push(*md);
			ivalue->data = FULL_DEPTH_MD;
			vrt_producer_publish(producer_);
		}
		// TODO:
		//ToString(*md);
    } // while (!ended_)

	clog_warning("[%s] RevData exit.",module_name_);
	fflush (Log::fp);
}

void FullDepthMDProducer::End()
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

int32_t FullDepthMDProducer::Push(const MDPackEx& md){
	static int32_t shfemarketdata_cursor = FULL_DEPTH_MD_BUFFER_SIZE - 1;
	shfemarketdata_cursor++;
	if (shfemarketdata_cursor%FULL_DEPTH_MD_BUFFER_SIZE == 0){
		shfemarketdata_cursor = 0;
	}
	shfemarketdata_buffer_[shfemarketdata_cursor] = md;

	return shfemarketdata_cursor;
}

MDPackEx* FullDepthMDProducer::GetData(int32_t index)
{
	return &shfemarketdata_buffer_[index];
}

bool FullDepthMDProducer::IsDominant(const char *contract)
{
#ifdef PERSISTENCE_ENABLED 
	// 持久化行情时，需要记录所有合约
	clog_warning("[%s] return TRUE in IsDominant.",module_name_);
	return true;
#else
	return IsDominantImp(contract, dominant_contracts_, dominant_contract_count_);
#endif
}

std::string FullDepthMDProducer::ToString(const MDPack &d) {
	clog_info("MDPack Data:instrument:%s;"
		"islast:%d seqno:%d direction:%c count:%d",
		d.instrument, (int)d.islast, d.seqno,
		d.direction, d.count);
	for(int i = 0; i < d.count; i++) {
		 clog_info("price%d: %lf, volume%d: %d",
			 i, d.data[i].price, i, d.data[i].volume);
	}
  
  return "";
}

std::string FullDepthMDProducer::ToString(const MDPackEx &d) 
{
	clog_debug("MDPackEx Data: instrument:%s; damaged:%d;"
		"islast:%d seqno:%d direction:%c count:%d",
		d.content.instrument, d.damaged, (int)d.content.islast, d.content.seqno,
		d.content.direction, d.content.count);
	for(int i = 0; i < d.content.count; i++) {
		 clog_debug("price%d: %lf, volume%d: %d",
			 i, d.content.data[i].price, i, d.content.data[i].volume);
	}
  
  return "";
}
