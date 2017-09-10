#include "my_protocol_packager.h"

#include <sstream>
#include <stdlib.h>

#include "field_convert.h"
#include "config_data.h"


void FEMASPacker::OrderRequest(const TunnelConfigData &cfg,const T_PlaceOrder *req,
    OrderRefDataType new_order_ref, CUstpFtdcInputOrderField &insert_order)
{
	memset(&insert_order, 0, sizeof(insert_order));

	strncpy(insert_order.BrokerID, cfg.Logon_config().brokerid.c_str(), sizeof(TUstpFtdcBrokerIDType));
	strncpy(insert_order.ExchangeID, MY_TNL_EXID_SHFE, sizeof(TUstpFtdcExchangeIDType));
	strcpy(insert_order.OrderSysID, "");//系统报单编号，填什么内容?
	strncpy(insert_order.InvestorID, cfg.Logon_config().investorid.c_str(), sizeof(TUstpFtdcInvestorIDType));
	strncpy(insert_order.UserID, cfg.Logon_config().clientid.c_str(), sizeof(TUstpFtdcUserIDType));

	strncpy(insert_order.InstrumentID, req->stock_code, sizeof(TUstpFtdcInstrumentIDType));
	// wangying 8位，左填充0，最大支持99999个信号
	snprintf(insert_order.UserOrderLocalID, sizeof(TUstpFtdcUserOrderLocalIDType), "%lld", new_order_ref);
	insert_order.OrderPriceType = FEMASFieldConvert::GetFEMASPriceType(req->order_kind);
	insert_order.Direction = req->direction;
	insert_order.OffsetFlag = FEMASFieldConvert::GetFEMASOCFlag(MY_TNL_EC_SHFE, req->open_close);
	insert_order.HedgeFlag = FEMASFieldConvert::GetFEMASHedgeType(req->speculator);

	insert_order.LimitPrice = req->limit_price;
	insert_order.Volume = req->volume;

	// 有效期类型
	insert_order.TimeCondition = FEMASFieldConvert::GetFEMASTimeCondition(req->order_type);
	// GTD日期
	strcpy(insert_order.GTDDate, "");
	// 成交量类型
	insert_order.VolumeCondition = USTP_FTDC_VC_AV;
	// 最小成交量
	insert_order.MinVolume = 0;
    if (req->order_type == MY_TNL_HF_FOK)
    {
        insert_order.MinVolume = insert_order.Volume;
        insert_order.VolumeCondition = USTP_FTDC_VC_CV;
    }
	// 止损价
	insert_order.StopPrice = 0;
	// 强平原因
	insert_order.ForceCloseReason = USTP_FTDC_FCR_NotForceClose;
	// 自动挂起标志
	insert_order.IsAutoSuspend = 0;
	strcpy(insert_order.BusinessUnit,"");
	strncpy(insert_order.UserCustom, cfg.Logon_config().clientid.c_str(), sizeof(TUstpFtdcCustomType));
}

void FEMASPacker::CancelRequest(const TunnelConfigData &cfg,const T_CancelOrder *req, OrderRefDataType order_ref,
    OrderRefDataType org_order_ref, CUstpFtdcOrderActionField &cancle_order)
{
	memset(&cancle_order, 0, sizeof(cancle_order));

	// 原报单交易所标识
	strncpy(cancle_order.ExchangeID, MY_TNL_EXID_SHFE, sizeof(TUstpFtdcExchangeIDType));
	strncpy(cancle_order.BrokerID, cfg.Logon_config().brokerid.c_str(), sizeof(TUstpFtdcBrokerIDType));
	strncpy(cancle_order.InvestorID, cfg.Logon_config().investorid.c_str(), sizeof(TUstpFtdcInvestorIDType));
	strncpy(cancle_order.UserID, cfg.Logon_config().clientid.c_str(), sizeof(TUstpFtdcUserIDType));
	// wangying 8位，左填充0，最大支持99999个信号
	snprintf(cancle_order.UserOrderActionLocalID, sizeof(cancle_order.UserOrderActionLocalID), "%lld", order_ref);
	// wangying 8位，左填充0，最大支持99999个信号
    snprintf(cancle_order.UserOrderLocalID, sizeof(cancle_order.UserOrderLocalID), "%lld", org_order_ref);
	FEMASFieldConvert::SysOrderIDToCTPFormat(req->entrust_no, cancle_order.OrderSysID);

	cancle_order.ActionFlag = USTP_FTDC_AF_Delete;
	cancle_order.LimitPrice = 0;
	cancle_order.VolumeChange = 0;
}

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
