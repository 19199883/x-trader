#include <functional>   // std::bind
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "fulldepthmd_producer.h"
#include "quote_cmn_utility.h"

using namespace std;
using namespace std::placeholders;
using std::chrono::system_clock;
using namespace my_cmn;

void L2MDProducer::Convert(const StdQuote5 &other,
	TapAPIQuoteWhole_MY *tap_data, ZCEL2QuotSnapshotField_MY &data)
{
	if(tap_data != NULL){ // contents from level1 
		data.PreSettle = InvalidToZeroD(tap_data->QPreSettlePrice);	/*前结算价格*/
		data.PreClose = InvalidToZeroD(tap_data->QPreClosingPrice);	/*前收盘价格*/
		data.PreOpenInterest = (int)tap_data->QPrePositionQty;		/*previous days's positions */
		data.OpenPrice = InvalidToZeroD(tap_data->QOpeningPrice);	/*开盘价*/
		data.HighPrice = InvalidToZeroD(tap_data->QHighPrice);	    /*最高价*/
		data.LowPrice = InvalidToZeroD(tap_data->QLowPrice);	        /*最低价*/
		data.ClosePrice = InvalidToZeroD(tap_data->QClosingPrice);	    /*收盘价*/
		data.SettlePrice = InvalidToZeroD(tap_data->QSettlePrice);	/*结算价*/
		data.HighLimit = InvalidToZeroD(tap_data->QLimitUpPrice);	/*涨停板*/
		data.LowLimit = InvalidToZeroD(tap_data->QLimitDownPrice);	/*跌停板*/
		data.LifeHigh = InvalidToZeroD(tap_data->QHisHighPrice);	/*历史最高成交价格*/
		data.LifeLow = InvalidToZeroD(tap_data->QHisLowPrice);	/*历史最低成交价格*/
		data.AveragePrice = InvalidToZeroD(tap_data->QAveragePrice);	/*均价*/
		data.OpenInterest = (int)tap_data->QPositionQty;	/*持仓量*/
		strcpy(data.ContractID,tap_data->ContractNo1);		/*合约编码*/
	}
	 
	//时间：如2014-02-03 13:23:45   
	system_clock::time_point today = system_clock::now();
	std::time_t tt = system_clock::to_time_t ( today );
	strftime(data.TimeStamp, sizeof(data.TimeStamp), "%Y-%m-%d %H:%M:%S",localtime(&tt));
	strcpy(data.TimeStamp+11,other.updateTime);
	strcpy(data.TimeStamp+19,".");
	sprintf(data.TimeStamp+20,"%03d",other.updateMS);

	data.TotalBidLot = (int)other.totalbid;	/*委买总量*/
	data.TotalAskLot = (int)other.totalask;	/*委卖总量*/

	data.TotalVolume = other.volume;
	data.ContractIDType = 0;			/*合约类型 0->目前应该为0， 扩充：0:期货,1:期权,2:组合*/
	data.LastPrice = InvalidToZeroD(other.price);		/*最新价*/
	data.BidPrice[0] = InvalidToZeroD(other.bidPrice1);     /*买入价格 下标从0开始*/
	data.BidPrice[1] = InvalidToZeroD(other.bidPrice2);     /*买入价格 下标从0开始*/
	data.BidPrice[2] = InvalidToZeroD(other.bidPrice3);     /*买入价格 下标从0开始*/
	data.BidPrice[3] = InvalidToZeroD(other.bidPrice4);     /*买入价格 下标从0开始*/	
	data.BidPrice[4] = InvalidToZeroD(other.bidPrice5);     /*买入价格 下标从0开始*/

	data.AskPrice[0] = InvalidToZeroD(other.askPrice1);     /*卖出价 下标从0开始*/
	data.AskPrice[1] = InvalidToZeroD(other.askPrice2);     /*卖出价 下标从0开始*/
	data.AskPrice[2] = InvalidToZeroD(other.askPrice3);     /*卖出价 下标从0开始*/
	data.AskPrice[3] = InvalidToZeroD(other.askPrice4);     /*卖出价 下标从0开始*/
	data.AskPrice[4] = InvalidToZeroD(other.askPrice5);     /*卖出价 下标从0开始*/
	data.BidLot[0] = other.bidVolume1;          /*买入数量 下标从0开始*/
	data.BidLot[1] = other.bidVolume2;          /*买入数量 下标从0开始*/
	data.BidLot[2] = other.bidVolume3;          /*买入数量 下标从0开始*/
	data.BidLot[3] = other.bidVolume4;          /*买入数量 下标从0开始*/
	data.BidLot[4] = other.bidVolume5;          /*买入数量 下标从0开始*/

	data.AskLot[0] = other.askVolume1;          /*卖出数量 下标从0开始*/
	data.AskLot[1] = other.askVolume2;          /*卖出数量 下标从0开始*/
	data.AskLot[2] = other.askVolume3;          /*卖出数量 下标从0开始*/
	data.AskLot[3] = other.askVolume4;          /*卖出数量 下标从0开始*/
	data.AskLot[4] = other.askVolume5;          /*卖出数量 下标从0开始*/
}

L2MDProducer::L2MDProducer(struct vrt_queue  *queue)
	: module_name_("FullDepthProducer")
{
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

	thread_rev_ = new std::thread(&L2MDProducer::RevData, this);
	thread_rev_->detach();
}

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

L2MDProducer::~L2MDProducer()
{
}

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

		StdQuote5* md = (StdQuote5 *)(buf);
		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		vrt_producer_claim(producer_, &vvalue);
		ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
		ivalue->index = Push(*md);
		ivalue->data = L2_MD;
		vrt_producer_publish(producer_);

		// TODO: the following need to move to consumer
		string udp_contr = p->instrument;
		TapAPIQuoteWhole_MY *tap_data = get_data_by_udp_contr(udp_contr );
		ZCEL2QuotSnapshotField_MY data_my = Convert(*p,tap_data );
    } // end while (!ended_) 
}

void L2MDProducer::End()
{
	if(!ended_){
		ended_ = true;
		vrt_producer_eof(producer_);
		clog_warning("[%s] End exit", module_name_);
	}
}

int32_t L2MDProducer::Push(const MDPackEx& md){
	static int32_t shfemarketdata_cursor = FULL_DEPTH_MD_BUFFER_SIZE - 1;
	shfemarketdata_cursor++;
	if (shfemarketdata_cursor%FULL_DEPTH_MD_BUFFER_SIZE == 0){
		shfemarketdata_cursor = 0;
	}
	shfemarketdata_buffer_[shfemarketdata_cursor] = md;

	return shfemarketdata_cursor;
}

MDPackEx* L2MDProducer::GetData(int32_t index)
{
	return &shfemarketdata_buffer_[index];
}

bool L2MDProducer::IsDominant(const char *contract)
{
	return IsDominantImp(contract, dominant_contracts_, dominant_contract_count_);
}

std::string L2MDProducer::ToString(const MDPack &d) {
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

std::string L2MDProducer::ToString(const MDPackEx &d) 
{
	clog_info("MDPackEx Data: instrument:%s; damaged:%d;"
		"islast:%d seqno:%d direction:%c count:%d",
		d.content.instrument, d.damaged, (int)d.content.islast, d.content.seqno,
		d.content.direction, d.content.count);
	for(int i = 0; i < d.content.count; i++) {
		 clog_info("price%d: %lf, volume%d: %d",
			 i, d.content.data[i].price, i, d.content.data[i].volume);
	}
  
  return "";
}
