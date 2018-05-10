#include <functional>   // std::bind
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "md_producer.h"
#include "quote_cmn_utility.h"
#include "DFITCL2ApiDataType.h"
#include "perfctx.h"

using namespace std;
using namespace std::placeholders;
using namespace DFITC_L2;

static void Convert(const MDBestAndDeep &other, MDBestAndDeep_MY &data)
{
    data.Type = other.Type;                                     
    data.Length = other.Length;                                 //报文长度
    data.Version = other.Version;                               //版本从1开始
    data.Time = other.Time;                                     //预留字段
    memcpy(data.Exchange, other.Exchange, 3);                   //交易所
    memcpy(data.Contract, other.Contract, 80);                  //合约代码
    data.SuspensionSign = other.SuspensionSign;                 //停牌标志
    data.LastClearPrice = InvalidToZeroF(other.LastClearPrice); //昨结算价
    data.ClearPrice = InvalidToZeroF(other.ClearPrice);         //今结算价
    data.AvgPrice = InvalidToZeroF(other.AvgPrice);             //成交均价
    data.LastClose = InvalidToZeroF(other.LastClose);           //昨收盘
    data.Close = InvalidToZeroF(other.Close);                   //今收盘
    data.OpenPrice = InvalidToZeroF(other.OpenPrice);           //今开盘
    data.LastOpenInterest = other.LastOpenInterest;             //昨持仓量
    data.OpenInterest = other.OpenInterest;                     //持仓量
    data.LastPrice = InvalidToZeroF(other.LastPrice);           //最新价
    data.MatchTotQty = other.MatchTotQty;                       //成交数量
    data.Turnover = InvalidToZeroD(other.Turnover);             //成交金额
    data.RiseLimit = InvalidToZeroF(other.RiseLimit);           //最高报价
    data.FallLimit = InvalidToZeroF(other.FallLimit);           //最低报价
    data.HighPrice = InvalidToZeroF(other.HighPrice);           //最高价
    data.LowPrice = InvalidToZeroF(other.LowPrice);             //最低价
    data.PreDelta = InvalidToZeroF(other.PreDelta);             //昨虚实度
    data.CurrDelta = InvalidToZeroF(other.CurrDelta);           //今虚实度
    data.BuyPriceOne = InvalidToZeroF(other.BuyPriceOne);		//买入价格1
    data.BuyQtyOne = other.BuyQtyOne;                           //买入数量1
    data.BuyImplyQtyOne = other.BuyImplyQtyOne;
    data.BuyPriceTwo = InvalidToZeroF(other.BuyPriceTwo);       
    data.BuyQtyTwo = other.BuyQtyTwo;                           
    data.BuyImplyQtyTwo = other.BuyImplyQtyTwo;
    data.BuyPriceThree = InvalidToZeroF(other.BuyPriceThree);   
    data.BuyQtyThree = other.BuyQtyThree;                       
    data.BuyImplyQtyThree = other.BuyImplyQtyThree;
    data.BuyPriceFour = InvalidToZeroF(other.BuyPriceFour);     
    data.BuyQtyFour = other.BuyQtyFour;                         
    data.BuyImplyQtyFour = other.BuyImplyQtyFour;
    data.BuyPriceFive = InvalidToZeroF(other.BuyPriceFive);     
    data.BuyQtyFive = other.BuyQtyFive;                         
    data.BuyImplyQtyFive = other.BuyImplyQtyFive;
    data.SellPriceOne = InvalidToZeroF(other.SellPriceOne);     //卖出价格1
    data.SellQtyOne = other.SellQtyOne;                         //买出数量1
    data.SellImplyQtyOne = other.SellImplyQtyOne;
    data.SellPriceTwo = InvalidToZeroF(other.SellPriceTwo);     
    data.SellQtyTwo = other.SellQtyTwo;                         
    data.SellImplyQtyTwo = other.SellImplyQtyTwo;
    data.SellPriceThree = InvalidToZeroF(other.SellPriceThree); 
    data.SellQtyThree = other.SellQtyThree;                     
    data.SellImplyQtyThree = other.SellImplyQtyThree;
    data.SellPriceFour = InvalidToZeroF(other.SellPriceFour);   
    data.SellQtyFour = other.SellQtyFour;                       
    data.SellImplyQtyFour = other.SellImplyQtyFour;
    data.SellPriceFive = InvalidToZeroF(other.SellPriceFive);   
    data.SellQtyFive = other.SellQtyFive;                       
    data.SellImplyQtyFive = other.SellImplyQtyFive;
    memcpy(data.GenTime, other.GenTime, 13);                    //行情产生时间
    data.LastMatchQty = other.LastMatchQty;                     //最新成交量
    data.InterestChg = other.InterestChg;                       //持仓量变化
    data.LifeLow = InvalidToZeroF(other.LifeLow);               //历史最低价
    data.LifeHigh = InvalidToZeroF(other.LifeHigh);             //历史最高价
    data.Delta = InvalidToZeroD(other.Delta);                   //delta
    data.Gamma = InvalidToZeroD(other.Gamma);                   //gama
    data.Rho = InvalidToZeroD(other.Rho);                       //rho
    data.Theta = InvalidToZeroD(other.Theta);                   //theta
    data.Vega = InvalidToZeroD(other.Vega);                     //vega
    memcpy(data.TradeDate, other.TradeDate, 9);                 //行情日期
    memcpy(data.LocalDate, other.LocalDate, 9);                 //本地日期
}

static void Convert(const MDOrderStatistic &other, MDOrderStatistic_MY &data)
{
    data.Type = other.Type;                                     //行情域标识
    data.Len = other.Len;                                       
    memcpy(data.ContractID, other.ContractID, 80);              //合约号
    data.TotalBuyOrderNum = other.TotalBuyOrderNum;             //买委托总量
    data.TotalSellOrderNum = other.TotalSellOrderNum;           //卖委托总量
    data.WeightedAverageBuyOrderPrice = InvalidToZeroD(
		other.WeightedAverageBuyOrderPrice);   //加权平均委买价格
    data.WeightedAverageSellOrderPrice = InvalidToZeroD(
		other.WeightedAverageSellOrderPrice); //加权平均委卖价格
}

MDProducer::MDProducer(struct vrt_queue  *queue)
:module_name_("MDProducer")
{
#ifdef PERSISTENCE_ENABLED 
	 p_save_best_and_deep_ = new QuoteDataSave<MDBestAndDeep_MY>(
		 "bestanddeepquote", DCE_MDBESTANDDEEP_QUOTE_TYPE);
	 p_save_order_statistic_ = new QuoteDataSave<MDOrderStatistic_MY>(
		 "orderstatistic", DCE_MDORDERSTATISTIC_QUOTE_TYPE, false);	 
#endif
	udp_fd_ = 0;

	ended_ = false;
	clog_warning("[%s] MD_BUFFER_SIZE: %d;", module_name_, MD_BUFFER_SIZE);

	ParseConfig();

	// init dominant contracts
	memset(dominant_contracts_, 0, sizeof(dominant_contracts_));
	dominant_contract_count_ = LoadDominantContracts(config_.contracts_file, dominant_contracts_);

	this->producer_ = vrt_producer_new("md_producer", 1, queue);
	clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0){
		this->producer_ ->yield = vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		this->producer_ ->yield = vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		this->producer_ ->yield = vrt_yield_strategy_hybrid();
	}

    thread_rev_ = new std::thread(&MDProducer::RevData, this);
}

void MDProducer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    

	// yield strategy
    TiXmlElement *comp_node = RootElement->FirstChildElement("Disruptor");
	if (comp_node != NULL){
		strcpy(config_.yield, comp_node->Attribute("yield"));
	} else { clog_error("[%s] x-trader.config error: Disruptor node missing.", module_name_); }

	// addr
    TiXmlElement *fdmd_node = RootElement->FirstChildElement("Md");
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

MDProducer::~MDProducer()
{
#ifdef PERSISTENCE_ENABLED 
    if (p_save_best_and_deep_) delete p_save_best_and_deep_;
    if (p_save_order_statistic_) delete p_save_order_statistic_;
#endif
}

int MDProducer::InitMDApi()
{
    // init udp socket
    int udp_client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    /* set reuse and non block for this socket */
    int son = 1;
    setsockopt(udp_client_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &son, sizeof(son));

    // bind address and port
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; //IPv4协议
    servaddr.sin_addr.s_addr = inet_addr(config_.ip);
    servaddr.sin_port = htons(config_.port);
    if (bind(udp_client_fd, (sockaddr *) &servaddr, sizeof(servaddr)) != 0){
        clog_error("[%s] UDP - bind failed: %s:%d", module_name_, config_.ip,config_.port);
        return -1;
    }

    // set nonblock flag
    int socket_ctl_flag = fcntl(udp_client_fd, F_GETFL);
    if (socket_ctl_flag < 0){
        clog_error("UDP - get socket control flag failed.");
    }
    if (fcntl(udp_client_fd, F_SETFL, socket_ctl_flag | O_NONBLOCK) < 0){
        clog_error("UDP - set socket control flag with nonblock failed.");
    }

    // set buffer length
    int rcvbufsize = 1 * 1024 * 1024;
    int ret = setsockopt(udp_client_fd, SOL_SOCKET, SO_RCVBUF, 
			(const void *) &rcvbufsize, sizeof(rcvbufsize));
    if (ret != 0){
        clog_error("UDP - set SO_RCVBUF failed.");
    }

    int broadcast_on = 1;
    ret = setsockopt(udp_client_fd, SOL_SOCKET, SO_BROADCAST, 
			&broadcast_on, sizeof(broadcast_on));
    if (ret != 0){
        clog_error("UDP - set SO_BROADCAST failed.");
    }

    return udp_client_fd;
}

void MDProducer::RevData()
{
	int udp_fd = InitMDApi();
	udp_fd_ = udp_fd; 
    if (udp_fd < 0) {
        clog_error("[%s] MY_SHFE_MD - CreateUdpFD failed.",module_name_);
        return;
    }else{
        clog_warning("[%s] MY_SHFE_MD - CreateUdpFD succeeded.",module_name_);
	}

    clog_debug("[%s] DCE_UDP-sizeof(MDBestAndDeep):%u", module_name_, sizeof(MDBestAndDeep));
    clog_debug("[%s] DCE_UDP-sizeof(MDOrderStatistic):%u", module_name_, sizeof(MDOrderStatistic));

    char buf[2048];
    int data_len = 0;
    sockaddr_in src_addr;
    unsigned int addr_len = sizeof(sockaddr_in);
    while (!ended_){
        data_len = recvfrom(udp_fd, buf, 2048, 0, (sockaddr *) &src_addr, &addr_len);
        if (data_len > 0){
            int type = (int) buf[0];
            if (type == EDataType::eMDBestAndDeep){
                MDBestAndDeep * p = (MDBestAndDeep *) (buf + 1);

				if(!(IsDominant(p->Contract))) continue; // 抛弃非主力合约

                Convert(*p, bestanddeep_);

#ifdef PERSISTENCE_ENABLED 
    timeval t;
    gettimeofday(&t, NULL);
    p_save_best_and_deep_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, &bestanddeep_);
#endif
				struct vrt_value  *vvalue;
				struct vrt_hybrid_value  *ivalue;
				vrt_producer_claim(producer_, &vvalue);
				ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
				ivalue->index = Push(bestanddeep_);
				ivalue->data = BESTANDDEEP;
				vrt_producer_publish(producer_);
            }else if (type == EDataType::eMDOrderStatistic){
                MDOrderStatistic * p = (MDOrderStatistic *) (buf + 1);

				if(!(IsDominant(p->ContractID))) continue; // 抛弃非主力合约

                Convert(*p, orderstatistic_);

#ifdef PERSISTENCE_ENABLED 
    timeval t;
    gettimeofday(&t, NULL);
    p_save_order_statistic_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, &orderstatistic_);
#endif
				struct vrt_value  *vvalue;
				struct vrt_hybrid_value  *ivalue;
				vrt_producer_claim(producer_, &vvalue);
				ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
				ivalue->index = Push(orderstatistic_);
				ivalue->data = ORDERSTATISTIC_DATA;
				vrt_producer_publish(producer_);
            }
        }
    } // while (running_flag_)

	clog_warning("[%s] RevData exit.",module_name_);
}


void MDProducer::End()
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
}

int32_t MDProducer::Push(const MDBestAndDeep_MY& md){
	static int32_t bestanddeep_cursor = MD_BUFFER_SIZE - 1;
	bestanddeep_cursor++;
	if (bestanddeep_cursor%MD_BUFFER_SIZE == 0){
		bestanddeep_cursor = 0;
	}
	bestanddeep_buffer_[bestanddeep_cursor] = md;

	clog_debug("[%s] push MDBestAndDeep: cursor,%d; contract:%s; time: %s",
				module_name_, bestanddeep_cursor, md.Contract, md.GenTime);

	return bestanddeep_cursor;
}

MDBestAndDeep_MY* MDProducer::GetBestAnddeep(int32_t index)
{
	return &bestanddeep_buffer_[index];
}

int32_t MDProducer::Push(const MDOrderStatistic_MY& md){
	static int32_t orderstatics_cursor = MD_BUFFER_SIZE - 1;
	orderstatics_cursor++;
	if (orderstatics_cursor%MD_BUFFER_SIZE == 0){
		orderstatics_cursor = 0;
	}
	orderstatistic_buffer_[orderstatics_cursor] = md;


	clog_debug("[%s] push MDOrderStatistic: cursor,%d; contract:%s; time: %s",
				module_name_, orderstatics_cursor, md.ContractID, "");

	return orderstatics_cursor;
}

MDOrderStatistic_MY* MDProducer::GetOrderStatistic(int32_t index)
{
	return &orderstatistic_buffer_[index];
}

bool MDProducer::IsDominant(const char *contract)
{
	return IsDominantImp(contract, dominant_contracts_, dominant_contract_count_);
}

// lic
CDepthMarketDataField* MDProducer::GetLastDataForIllegaluser(const char *contract)
{
	CDepthMarketDataField* data = L1MDProducerHelper::GetLastDataImp(
		contract,0,md_buffer_,L1MD_BUFFER_SIZE,L1MD_BUFFER_SIZE);
	return data;
}
