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

        CShfeFtdcMBLMarketDataField* md = (CShfeFtdcMBLMarketDataField*)recv_buf;
	//	clog_info("[%s] FullDepthMDProducer::RevData InstrumentID=%s; dir=%c; price=%f; vol=%d",
	//		module_name_,
	//		md->InstrumentID,
	//		md->Direction,
	//		md->Price,
	//		md->Volume);
		
		// 掉期权行情
		if(strlen(md->InstrumentID)>6){
			// clog_info("[%s] discard contract: %s ", module_name_, md->InstrumentID);
			continue;
		}

		if(strlen(md->InstrumentID)!=4 && !IsDominant(md->InstrumentID)){
			// clog_info("[%s] discard NOT dominant contract: %s ", module_name_, md->InstrumentID);
			continue;
		}

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		vrt_producer_claim(producer_, &vvalue);
		ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
		ivalue->index = Push(*md);
		ivalue->data = FULL_DEPTH_MD;
		vrt_producer_publish(producer_);
		 
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

int32_t FullDepthMDProducer::Push(const CShfeFtdcMBLMarketDataField& md){
	static int32_t shfemarketdata_cursor = FULL_DEPTH_MD_BUFFER_SIZE - 1;
	shfemarketdata_cursor++;
	if (shfemarketdata_cursor%FULL_DEPTH_MD_BUFFER_SIZE == 0){
		shfemarketdata_cursor = 0;
	}
	shfemarketdata_buffer_[shfemarketdata_cursor] = md;

	return shfemarketdata_cursor;
}

CShfeFtdcMBLMarketDataField* FullDepthMDProducer::GetData(int32_t index)
{
	return &shfemarketdata_buffer_[index];
}

bool FullDepthMDProducer::IsDominant(const char *contract)
{
	return IsDominantImp(contract, dominant_contracts_, dominant_contract_count_);
}

std::string FullDepthMDProducer::ToString(const CShfeFtdcMBLMarketDataField &md) {
	clog_info("[%s] FullDepthMDProducer::RevData InstrumentID=%s; dir=%c; price=%f; vol=%d",
		module_name_,
		md.InstrumentID,
		md.Direction,
		md.Price,
		md.Volume);
  
  return "";
}

