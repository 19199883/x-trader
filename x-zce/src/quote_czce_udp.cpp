#include <functional>   // std::bind
#include "quote_czce_udp.h"
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <ctime>
#include <ratio>
#include <chrono>
#include "quote_cmn_utility.h"

using namespace std;
using namespace std::placeholders;
using std::chrono::system_clock;
using namespace my_cmn;

void CzceUdpMD::Convert(const StdQuote5 &other,
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
	data.ContractIDType = 0;							/*合约类型 0->目前应该为0， 扩充：0:期货,1:期权,2:组合*/
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

CzceUdpMD::CzceUdpMD(const SubscribeContracts *subscribe_contracts, const ConfigData &cfg)
{
	// initialize level1 market data provider
	lvl1_provider_ = new TAP::MYQuoteData(subscribe_contracts,"my_quote_tap.config");
	auto f2 = std::bind(&CzceUdpMD::OnTapAPIQuoteWhole_MY, this,std::placeholders::_1);
	lvl1_provider_->SetQuoteDataHandler(f2);
	MY_LOG_INFO("CZCE_UDP - lvl1_provider initialized ");
	
    // start recv threads
    p_md_handler_ = new std::thread(std::bind(&CzceUdpMD::UdpDataHandler, this));
}

CzceUdpMD::~CzceUdpMD()
{
	if(lvl1_provider_) delete ((TAP::MYQuoteData *)lvl1_provider_);
}

void CzceUdpMD::UdpDataHandler()
{
    if (cfg_.Logon_config().quote_provider_addrs.size() != 1)
    {
        MY_LOG_ERROR("CZCE_UDP - quote front address is wrong, please check");
        return;
    }

    IPAndPortNum ip_and_port = ParseIPAndPortNum(cfg_.Logon_config().quote_provider_addrs.front());

    int udp_fd = CreateUdpFD(ip_and_port.first, ip_and_port.second);
    if (udp_fd < 0)
    {
        MY_LOG_ERROR("CZCE_UDP - CreateUdpFD failed.");
        return;
    }

    MY_LOG_INFO("CZCE_UDP - sizeof(StdQuote5):%u", sizeof(StdQuote5));

    char buf[2048];
    int data_len = 0;
    sockaddr_in src_addr;
    unsigned int addr_len = sizeof(sockaddr_in);

    while (running_flag_)
    {
        data_len = recvfrom(udp_fd, buf, 2048, 0, (sockaddr *) &src_addr, &addr_len);

        if (data_len > 0)
        {
			if (data_len != sizeof(StdQuote5))
			{
				static bool output_md_bd_flag = true;
				if (output_md_bd_flag)
				{
					output_md_bd_flag = false;
					MY_LOG_ERROR("CZCE_UDP - recv data len:%d", data_len);
				}
				continue;
			}
			timeval t;
			gettimeofday(&t, NULL);

			StdQuote5 * p = (StdQuote5 *) (buf);

			// TODO:
			MY_LOG_INFO("CZCE_UDP - StdQuote5 contract:%s", p->instrument);

			string udp_contr = p->instrument;
			TapAPIQuoteWhole_MY *tap_data = get_data_by_udp_contr(udp_contr );

			ZCEL2QuotSnapshotField_MY data_my = Convert(*p,tap_data );

			// 发出去
			if (tap_data != NULL
				&& l2_quote_handler_
				&& (subscribe_contracts_.empty() || subscribe_contracts_.find(p->instrument) != subscribe_contracts_.end()))
			{
				l2_quote_handler_(&data_my);
			}

			// 存起来
			if (tap_data != NULL) { p_save_zcel2_quote_snap_snapshot_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, &data_my); }
			else {
				//MY_LOG_WARN("rev no level one market data:%s", udp_contr.c_str() ); 
			}
        }
    }
}

int CzceUdpMD::CreateUdpFD(const std::string& addr_ip, unsigned short port)
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
    servaddr.sin_addr.s_addr = inet_addr(addr_ip.c_str());
    servaddr.sin_port = htons(port);

    if (bind(udp_client_fd, (sockaddr *) &servaddr, sizeof(servaddr)) != 0)
    {
        MY_LOG_FATAL("UDP - bind failed: %s:%d", addr_ip.c_str(), port);
        return -1;
    }

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
    int ret = setsockopt(udp_client_fd, SOL_SOCKET, SO_RCVBUF, (const void *) &rcvbufsize, sizeof(rcvbufsize));
    if (ret != 0)
    {
        MY_LOG_WARN("UDP - set SO_RCVBUF failed.");
    }

    int broadcast_on = 1;
    ret = setsockopt(udp_client_fd, SOL_SOCKET, SO_BROADCAST, &broadcast_on, sizeof(broadcast_on));
    if (ret != 0)
    {
        MY_LOG_WARN("UDP - set SO_BROADCAST failed.");
    }

    return udp_client_fd;
}

void CzceUdpMD::OnTapAPIQuoteWhole_MY(const TapAPIQuoteWhole_MY *data)	
{
	string tap_contr = data->CommodityNo;
	tap_contr += data->ContractNo1;

	// TODO:
	MY_LOG_INFO("CZCE_UDP - TapAPIQuoteWhole_MY comm:%s; contract:%s", data->CommodityNo, data->ContractNo1);

	TapAPIQuoteWhole_MY *tap_data = NULL;
	{
		tap_data =  get_data_by_tap_contr(tap_contr);
	}

	if(tap_data != NULL)
	{
		return;
	}
	else
	{
		lock_guard<std::mutex> lck (first_data_each_contract_lock_);
		first_data_each_contract_[tap_contr] = *data; 
		memcpy(first_data_each_contract_[tap_contr].ContractNo1,tap_contr.c_str(),sizeof(tap_contr.c_str()));	
	}
}

std::string CzceUdpMD::ToString(const ZCEL2QuotSnapshotField_MY * p)
{
	char buf[10240];
	if (p)
	{
		snprintf(buf, sizeof(buf), "structName=ZCEL2QuotSnapshotField_MY\n"
			"\tContractID=%s\n"
			"\tTimeStamp=%s\n"
			"\tPreSettle=%f\n"
			"\tPreClose=%f\n"
			"\tPreOpenInterest=%i\n"
			"\tOpenPrice=%f\n"
			"\tHighPrice=%f\n"
			"\tLowPrice=%f\n"
			"\tLastPrice=%f\n"
			"\tTotalVolume=%i\n"
			"\tOpenInterest=%i\n"
			"\tClosePrice=%f\n"
			"\tSettlePrice=%f\n"
			"\tAveragePrice=%f\n"
			"\tLifeHigh=%f\n"
			"\tLifeLow=%f\n"
			"\tHighLimit=%f\n"
			"\tLowLimit=%f\n"
			"\tTotalBidLot=%i\n"
			"\tTotalAskLot=%i\n"



			"\tBidLot1=%i\n"
			"\tBidLot2=%i\n"
			"\tBidLot3=%i\n"
			"\tBidLot4=%i\n"
			"\tBidLot5=%i\n"

			"\tAskLot1=%i\n"
			"\tAskLot2=%i\n"
			"\tAskLot3=%i\n"
			"\tAskLot4=%i\n"
			"\tAskLot5=%i\n"

			"\tBidPrice1=%f\n"
			"\tBidPrice2=%f\n"
			"\tBidPrice3=%f\n"
			"\tBidPrice4=%f\n"
			"\tBidPrice5=%f\n"

			"\tAskPrice1=%f\n"
			"\tAskPrice2=%f\n"
			"\tAskPrice3=%f\n"
			"\tAskPrice4=%f\n"
			"\tAskPrice5=%f\n",
			p->ContractID,                     ///< 合约编码         
			p->TimeStamp,
			p->PreSettle,                   ///< 前结算价格
			p->PreClose,                 ///< 前收盘价格
			p->PreOpenInterest,                    ///昨日空盘量
			p->OpenPrice,                   ///< 开盘价
			p->HighPrice,                 ///< 最高价
			p->LowPrice,                   ///< 最低价
			p->LastPrice,        ///< 最新价
			p->TotalVolume,                   ///
			p->OpenInterest,                ///< 持仓量
			p->ClosePrice,                  ///< 收盘价
			p->SettlePrice,                ///< 结算价
			p->AveragePrice,                ///< 均价
			p->LifeHigh,                ///< 历史最高成交价格
			p->LifeLow,                ///< 历史最低成交价格
			p->HighLimit,                ///< 涨停板
			p->LowLimit,                ///< 跌停板
			p->TotalBidLot,                ///< 委卖总量
			p->TotalAskLot,                ///< 委卖总量

			p->BidLot[0],                
			p->BidLot[1],                
			p->BidLot[2],                
			p->BidLot[3],                
			p->BidLot[4],                

			p->AskLot[0],                
			p->AskLot[1],                
			p->AskLot[2],                
			p->AskLot[3],                
			p->AskLot[4],                

			p->BidPrice[0],              
			p->BidPrice[1],              
			p->BidPrice[2],              
			p->BidPrice[3],              
			p->BidPrice[4],              

			p->AskPrice[0],              
			p->AskPrice[1],              
			p->AskPrice[2],              
			p->AskPrice[3],              
			p->AskPrice[4]              

		);
	}
	return buf;
}

TapAPIQuoteWhole_MY *CzceUdpMD::get_data_by_tap_contr(string &tap_fmt_contract)
{
	lock_guard<std::mutex> lck (first_data_each_contract_lock_);
	map<string,TapAPIQuoteWhole_MY>::iterator it = first_data_each_contract_.find(tap_fmt_contract);
	if(it==first_data_each_contract_.end())
	{
		return NULL;
	}
	else
	{
		return &it->second;
	}
}

TapAPIQuoteWhole_MY *CzceUdpMD::get_data_by_udp_contr(string &contract)
{
	string tap_fmt_contract = contract;

	// TODO:
	MY_LOG_INFO("CZCE_UDP -before get_data_by_udp_contr contract:%s", tap_fmt_contract.c_str());

	tap_fmt_contract.erase(2,1); 
	
	// TODO:
	MY_LOG_INFO("CZCE_UDP - get_data_by_udp_contr contract:%s", tap_fmt_contract.c_str());

	return this->get_data_by_tap_contr(tap_fmt_contract);
}

