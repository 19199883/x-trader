#include "my_protocol_packager.h"
#include <sstream>
#include <stdlib.h>
#include "field_convert.h"

TapAPINewOrder ESUNNYPacker::new_order_;

bool ESUNNYPacker::InitNewOrder(const TunnelConfigData& cfg, const T_PlaceOrder* req,
			TapAPINewOrder& new_or)
{
    memset(&new_order_, 0, sizeof(new_order_));
    strcpy(new_order_.AccountNo, cfg.Logon_config().clientid.c_str());
    strcpy(new_order_.ExchangeNo, ci->ExchangeNo);
    new_order_.CommodityType = ci->CommodityType;
    strcpy(new_order_.StrikePrice, "");
    new_order_.CallOrPutFlag = ci->CallOrPutFlag1;
    strcpy(new_order_.ContractNo2, "");
    strcpy(new_order_.StrikePrice2, "");
    new_order_.CallOrPutFlag2 = ci->CallOrPutFlag2;
    new_order_.OrderType = ESUNNYFieldConvert::GetESUNNYPriceType(req->order_kind);
    new_order_.OrderSource = TAPI_ORDER_SOURCE_ESUNNY_API;
    new_order_.TimeInForce = ESUNNYFieldConvert::GetESUNNYTimeCondition(req->order_type);
    strcpy(new_order_.ExpireTime, "");
    new_order_.IsRiskOrder = APIYNFLAG_NO;
    new_order_.PositionEffect2 = TAPI_PositionEffect_NONE;
    strcpy(new_order_.InquiryNo, "");
    new_order_.HedgeFlag = ESUNNYFieldConvert::GetESUNNYHedgeType(req->speculator);
    new_order_.RefInt = 0;
    new_order_.TacticsType = TAPI_TACTICS_TYPE_NONE;
    new_order_.TriggerCondition = TAPI_TRIGGER_CONDITION_NONE;
    new_order_.TriggerPriceType = TAPI_TRIGGER_PRICE_NONE;
    new_order_.AddOneIsValid = APIYNFLAG_NO;
    new_order-.HedgeFlag2 = new_or.HedgeFlag;
    new_order-.MarketLevel= TAPI_MARKET_LEVEL_0;
    new_order_.OrderDeleteByDisConnFlag = APIYNFLAG_NO;
}

// TODO:创建行情缓存，填充固定字段，之后至填充变更的字段
void ESUNNYPacker::OrderRequest(const TunnelConfigData& cfg, const T_PlaceOrder* req,
			TapAPINewOrder& new_or)
{
    const TapAPITradeContractInfo * ci = ESUNNYFieldConvert::GetContractInfo(req->stock_code);
    strcpy(new_or.CommodityNo, ci->CommodityNo);
    strcpy(new_or.ContractNo, ci->ContractNo1);
    new_or.OrderSide = ESUNNYFieldConvert::GetESUNNYSide(req->direction);
    new_or.PositionEffect = ESUNNYFieldConvert::GetESUNNYOCFlag(req->open_close);
    new_or.OrderPrice = req->limit_price;
    new_or.OrderQty = req->volume;
}

void ESUNNYPacker::OrderRespond(int error_no, long serial_no, long entrust_no, short entrust_status, T_OrderRespond& rsp)
{
    memset(&rsp, 0, sizeof(T_OrderRespond));
    rsp.serial_no = serial_no;
    rsp.error_no = error_no;
    rsp.entrust_no = entrust_no;
    rsp.entrust_status = entrust_status;
}

void ESUNNYPacker::CancelRequest(const EsunnyOrderInfo* org_order_info, TapAPIOrderCancelReq& co)
{
    memset(&co, 0, sizeof(co));
    co.ServerFlag = org_order_info->server_flag;                             ///< 服务器标识
    memcpy(co.OrderNo, org_order_info->order_no, sizeof(co.OrderNo));        ///< 委托编码
}

void ESUNNYPacker::CancelRespond(int error_no, long serial_no, long entrust_no, T_CancelRespond& cancel_respond)
{
    memset(&cancel_respond, 0, sizeof(T_CancelRespond));
    cancel_respond.serial_no = serial_no;
    cancel_respond.error_no = error_no;
    cancel_respond.entrust_no = entrust_no;

    // 需要回报撤单状态，成功为已报，失败为拒绝
    cancel_respond.entrust_status = MY_TNL_OS_REPORDED;
    if (error_no != 0) cancel_respond.entrust_status = MY_TNL_OS_ERROR;
}

void ESUNNYPacker::OrderReturn(const TapAPIOrderInfo* rsp, const EsunnyOrderInfo* p_req, T_OrderReturn& order_return)
{
    memset(&order_return, 0, sizeof(order_return));

    order_return.serial_no = p_req->po.serial_no;
    memcpy(order_return.stock_code, p_req->po.stock_code, sizeof(order_return.stock_code));
    order_return.entrust_no = p_req->entrust_no;
    order_return.entrust_status = p_req->entrust_status;
    order_return.direction = p_req->po.direction;
    order_return.open_close = p_req->po.open_close;
    order_return.speculator = p_req->po.speculator;
    order_return.volume = p_req->po.volume;
    order_return.limit_price = p_req->po.limit_price;
    order_return.volume_remain = p_req->volume_remain;
}

