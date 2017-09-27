#ifndef MY_PROTOCOL_PACKAGER_H_
#define MY_PROTOCOL_PACKAGER_H_

#include "TapTradeAPIDataType.h"

#include "trade_data_type.h"
#include "my_tunnel_lib.h"
#include "esunny_trade_context.h"

class ESUNNYPacker
{
public:

    static bool OrderRequest(const TunnelConfigData &cfg, const T_PlaceOrder *req, TapAPINewOrder &new_or);

    static void OrderRespond(int error_no, long serial_no, long entrust_no, short entrust_status, T_OrderRespond &order_respond);

    static void CancelRequest(const EsunnyOrderInfo *org_order_info, TapAPIOrderCancelReq &co);

    static void CancelRespond(int error_no, long serial_no, long entrust_no, T_CancelRespond &cancle_order);

    static void OrderReturn(const TapAPIOrderInfo *rsp, const EsunnyOrderInfo *p_req, T_OrderReturn &order_return);

    static void TradeReturn(const TapAPIFillInfo *rsp, const EsunnyOrderInfo *p_req, T_TradeReturn &trade_return);
};

#endif // MY_PROTOCOL_Packer_H_
