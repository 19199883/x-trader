#include <functional>   // std::bind
#include "md_producer.h"
#include "perfctx.h"

using namespace std::placeholders;
using namespace std;

FullDepthMDProducer::FullDepthMDProducer(struct vrt_queue  *queue)
:module_name_("FullDepthProducer"),seq_no_(0),server_(0)
{
	ended_ = false;

	ParseConfig();
	clog_warning("[%s] FULL_DEPTH_MD_BUFFER_SIZE: %d;", module_name_, FULL_DEPTH_MD_BUFFER_SIZE);

	this->producer_ = vrt_producer_new("fulldepthmd_producer", 1, queue);
	clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0){
		producer_ ->yield = vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		producer_ ->yield = vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		producer_ ->yield = vrt_yield_strategy_hybrid();
	}

	thread_rev_ = thread(FullDepthMDProducer::RevData);
	thread_rev_.detach();
}

void FullDepthMDProducer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-shmd.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    

	// yield strategy
    TiXmlElement *disruptor_node = RootElement->FirstChildElement("Disruptor");
	if (disruptor_node != NULL){
		config_.yield = disruptor_node->Attribute("yield");
	} else { clog_error("[%s] x-shmd.config error: Disruptor node missing.", module_name_); }

	// addr
    TiXmlElement *fdmd_node = RootElement->FirstChildElement("FullDepthMd");
	if (fdmd_node != NULL){
		config_.addr = fdmd_node->Attribute("addr");
	} else { clog_error("[%s] x-shmd.config error: FulldepthMd node missing.", module_name_); }

	size_t ipstr_start = config_.addr.find("//")+2;
	size_t ipstr_end = config_.addr.find(":",ipstr_start);
	config_.ip = config_.addr.substr(ipstr_start,ipstr_end-ipstr_start);
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
    servaddr.sin_addr.s_addr = inet_addr(config_.ip.c_str());
    servaddr.sin_port = htons(config_.port);
    if (bind(udp_client_fd, (sockaddr *) &servaddr, sizeof(servaddr)) != 0){
        clog_error("[%s] UDP - bind failed:%s:%d",module_name_,config_.ip.c_str(),config_.port);
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
    if (udp_fd < 0) {
        clog_error("[%s] MY_SHFE_MD - CreateUdpFD failed.",module_name_);
        return;
    }

    int recv_struct_len = sizeof(MDPack);
    int ary_len = recv_struct_len;
    char *recv_buf = new char[ary_len];
    std::size_t recv_len = 0;

	// 假设最多支持10个全挡数据服务器
	repairer repairers[10];
	for (int i=0; i<10; i++) repairers[i].server_ = i;
    while (!ended_){
        sockaddr_in fromAddr;
        int nFromLen = sizeof(fromAddr);
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

        MDPack *p = (MDPack *)recv_buf;
		int new_svr = p->seqno % 10;
        if (new_svr != server_) { MY_LOG_INFO("server from %d to %d", server_, new_svr); }
		repairers[new_svr].rev(*p);
		
		bool empty = true;
		// TODO:here 一个行情只需一份，不用多分拷贝
		MDPackEx &data = repairers[new_svr].next(empty);
		while (!empty) { 
			struct vrt_value  *vvalue;
			struct vrt_hybrid_value  *ivalue;
			vrt_producer_claim(producer_, &vvalue);
			ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
			ivalue->index = Push(*md);
			ivalue->data = FULL_DEPTH_MD;
			vrt_producer_publish(producer_);
			proc_udp_data(data);

			data = repairers[new_svr].next(empty);
		}

        server_ = new_svr;
    } // while (!ended_)
}

void FullDepthMDProducer::End()
{
	if(!ended_){
		ended_ = true;
		vrt_producer_eof(producer_);
		clog_warning("[%s] End exit", module_name_);
	}
}

int32_t FullDepthMDProducer::Push(const MYShfeMarketData& md){
	static int32_t shfemarketdata_cursor = FULL_DEPTH_MD_BUFFER_SIZE - 1;
	shfemarketdata_cursor++;
	if (shfemarketdata_cursor%FULL_DEPTH_MD_BUFFER_SIZE == 0){
		shfemarketdata_cursor = 0;
	}
	shfemarketdata_buffer_[shfemarketdata_cursor] = md;

	return shfemarketdata_cursor;
}

MYShfeMarketData* FullDepthMDProducer::GetShfeMarketData(int32_t index)
{
	return &shfemarketdata_buffer_[index];
}

std::string FullDepthMDProducer::ToString(const MDPack &d) {
	MY_LOG_DEBUG("server(%d)MDPack Data: \ninstrument: %s\n"
				"islast: %d\nseqno: %d\ndirection: %c\ncount: %d\n",
				this->server_,d.instrument, (int)d.islast, d.seqno,
				d.direction, d.count);
	for(int i = 0; i < d.count; i++) {
		 MY_LOG_DEBUG("server(%d) price%d: %lf, volume%d: %d",
				 this->server_, i, d.data[i].price, i, d.data[i].volume);
	}
  
  return "";
}