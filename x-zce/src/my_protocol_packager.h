#ifndef MY_PROTOCOL_PACKAGER_H_
#define MY_PROTOCOL_PACKAGER_H_

#include "TapTradeAPIDataType.h"

#define EXCHANGE_NO "ZCE"

class ESUNNYPacker
{
public:

    static void InitNewOrder(const char *account);

	static TapAPINewOrder* OrderRequest(const signal_t& sig,const char *account,
			long localorderid,int32_t vol)

    static void OrderRespond(int error_no, long serial_no, long entrust_no, 
				short entrust_status, T_OrderRespond &order_respond);

    static void CancelRequest(const EsunnyOrderInfo *org_order_info, TapAPIOrderCancelReq &co);

    static void CancelRespond(int error_no, long serial_no, long entrust_no, 
				T_CancelRespond &cancle_order);

    static void OrderReturn(const TapAPIOrderInfo *rsp, const EsunnyOrderInfo *p_req, 
				T_OrderReturn &order_return);

private:
	TapAPINewOrde new_order_;
};

#endif // MY_PROTOCOL_Packer_H_
