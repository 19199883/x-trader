#ifndef MY_PROTOCOL_PACKAGER_H_
#define MY_PROTOCOL_PACKAGER_H_

#include "TapTradeAPIDataType.h"
#include "signal.h"
#include "EsUdpDataType.h"

#define EXCHANGE_NO "ZCE"

	#define UDP_ORDER_INSERT_LEN sizeof(DstarApiHead) + sizeof(DstarApiReqOrderInsertField)
	#define UDP_ORDER_DELETE_LEN sizeof(DstarApiHead) + sizeof(DstarApiReqOrderDeleteField)

class ESUNNYPacker
{
public:
	// ok
	static char* UdpOrderRequest(
			const signal_t& sig,
			long localorderid,
			int32_t vol);
	static char* DeleteUdpOrderRequest(const char *orderNo);

	// TODO: coding for udp version
	static void InitDeleteUdpOrder();
	static void InitNewUdpOrder(const char *account, char *upperchannel);
    static void InitNewOrder(const char *account);

	static TapAPINewOrder* OrderRequest(const signal_t& sig,const char *account,
			long localorderid,int32_t vol);

private:
	
	static char new_udporder_[UDP_ORDER_INSERT_LEN];
	static char delete_udporder_[UDP_ORDER_DELETE_LEN];
};

#endif // MY_PROTOCOL_Packer_H_
