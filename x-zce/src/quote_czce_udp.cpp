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

