#ifndef MY_PROTOCOL_PACKAGER_H_
#define MY_PROTOCOL_PACKAGER_H_

#include "TapTradeAPIDataType.h"
#include "signal.h"

#define EXCHANGE_NO "ZCE"

class ESUNNYPacker
{
public:
	// TODO: coding for udp version
	static char* ESUNNYPacker::UdpOrderRequest(
			const signal_t& sig,
			const char *account,
			long localorderid,
			int32_t vol);

    static void InitNewOrder(const char *account);

	static TapAPINewOrder* OrderRequest(const signal_t& sig,const char *account,
			long localorderid,int32_t vol);

private:
	static TapAPINewOrder new_order_;
	
	// TODO: coding for udp version
	char new_udporder_[sizeof (TapAPIUdpHead) + sizeof (TapAPIUdpOrderInsertReq)];
};

#endif // MY_PROTOCOL_Packer_H_
