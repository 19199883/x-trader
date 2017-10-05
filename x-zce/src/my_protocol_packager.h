#ifndef MY_PROTOCOL_PACKAGER_H_
#define MY_PROTOCOL_PACKAGER_H_

#include "TapTradeAPIDataType.h"
#include "signal.h"

#define EXCHANGE_NO "ZCE"

class ESUNNYPacker
{
public:
    static void InitNewOrder(const char *account);

	static TapAPINewOrder* OrderRequest(const signal_t& sig,const char *account,
			long localorderid,int32_t vol);

private:
	static TapAPINewOrder new_order_;
};

#endif // MY_PROTOCOL_Packer_H_
