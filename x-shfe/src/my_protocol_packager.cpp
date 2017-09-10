#include "my_protocol_packager.h"
#include <sstream>
#include <stdlib.h>
#include "field_convert.h"

void FEMASPacker::CancelRespond(int error_no, SerialNoType serial_no, const char* entrust_no, T_CancelRespond &cancel_respond)
{
	memset(&cancel_respond, 0, sizeof(cancel_respond));
	cancel_respond.entrust_no     = atol(entrust_no);
	cancel_respond.serial_no      = serial_no;
	cancel_respond.error_no       = error_no;

    // 需要回报撤单状态，成功为已报，失败为拒绝
    cancel_respond.entrust_status = MY_TNL_OS_REPORDED;
    if (error_no != 0) cancel_respond.entrust_status = MY_TNL_OS_ERROR;
}

CUstpFtdcOrderActionField
MyFemasTradeSpi::CreatCancelParam(const CUstpFtdcOrderField& order_field)
{
    CUstpFtdcOrderActionField cancle_order;
    OrderRefDataType order_ref = femas_trade_context_.GetNewOrderRef();
    memset(&cancle_order, 0, sizeof(cancle_order));

    memcpy(cancle_order.ExchangeID, order_field.ExchangeID, sizeof(TUstpFtdcExchangeIDType));
    memcpy(cancle_order.OrderSysID, order_field.OrderSysID, sizeof(TUstpFtdcOrderSysIDType));
    memcpy(cancle_order.BrokerID, order_field.BrokerID, sizeof(TUstpFtdcBrokerIDType));
    memcpy(cancle_order.InvestorID, order_field.InvestorID, sizeof(TUstpFtdcInvestorIDType));
    memcpy(cancle_order.UserID, order_field.UserID, sizeof(TUstpFtdcUserIDType));
    memcpy(cancle_order.UserOrderLocalID, order_field.UserOrderLocalID, sizeof(TUstpFtdcUserOrderLocalIDType));
    snprintf(cancle_order.UserOrderActionLocalID, sizeof(cancle_order.UserOrderActionLocalID), "%lld", order_ref);

    cancle_order.ActionFlag = USTP_FTDC_AF_Delete;
    cancle_order.LimitPrice = 0;
    cancle_order.VolumeChange = 0;

    return cancle_order;
}
