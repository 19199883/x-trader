#include "my_protocol_packager.h"
#include <sstream>
#include <stdlib.h>
#include "field_convert.h"

TapAPINewOrder ESUNNYPacker::new_order_;

// TODO: invoke this method somewhere
bool ESUNNYPacker::InitNewOrder(const char *account)
{
	// done
    memset(&new_order_, 0, sizeof(new_order_));
    strcpy(new_order_.AccountNo,account);
    strcpy(new_order_.ExchangeNo, EXCHANGE_NO);
    new_order_.CommodityType = TAPI_COMMODITY_TYPE_FUTURES; 
    strcpy(new_order_.StrikePrice, "");
    new_order_.CallOrPutFlag = TAPI_CALLPUT_FLAG_NONE;
    strcpy(new_order_.ContractNo2, "");
    strcpy(new_order_.StrikePrice2, "");
    new_order_.CallOrPutFlag2 = TAPI_CALLPUT_FLAG_NONE;
    new_order_.OrderType = TAPI_ORDER_TYPE_LIMIT
    new_order_.OrderSource = TAPI_ORDER_SOURCE_ESUNNY_API;
    new_order_.TimeInForce = TAPI_ORDER_TIMEINFORCE_GFD;
    strcpy(new_order_.ExpireTime, "");
    new_order_.IsRiskOrder = APIYNFLAG_NO;
    new_order_.PositionEffect2 = TAPI_PositionEffect_NONE;
    strcpy(new_order_.InquiryNo, "");
    new_order_.HedgeFlag = TAPI_HEDGEFLAG_T;
    new_order_.RefInt = 0;
    new_order_.TacticsType = TAPI_TACTICS_TYPE_NONE;
    new_order_.TriggerCondition = TAPI_TRIGGER_CONDITION_NONE;
    new_order_.TriggerPriceType = TAPI_TRIGGER_PRICE_NONE;
    new_order_.AddOneIsValid = APIYNFLAG_NO;
    new_order-.HedgeFlag2 = TAPI_HEDGEFLAG_NONE;
    new_order-.MarketLevel= TAPI_MARKET_LEVEL_0;
    new_order_.OrderDeleteByDisConnFlag = APIYNFLAG_NO;
}

// done
TapAPINewOrder* ESUNNYPacker::OrderRequest(const signal_t& sig,const char *account,
			long localorderid,int32_t vol)
{
	// contract
    strncpy(new_or.CommodityNo,sig.symbol,2);
	new_or.CommodityNo[2] = 0;
    strncpy(new_or.ContractNo, sig.symbol+2,3);
	new_or.ContractNo[3] = 0;
	// side
	if (sig.sig_act==signal_act_t::buy){
		new_or.OrderPrice = sig.buy_price;
		new_or.OrderSide = TAPI_SIDE_BUY;
	} else if (sig.sig_act==signal_act_t::sell){
		new_or.OrderPrice = sig.sell_price;
		new_or.OrderSide = TAPI_SIDE_SELL;
	} else{
		 clog_warning("do support BuySellType value:%d; sig id:%d",
			sig.sig_act, sig.sig_id); 
	}
	// position effect
	if (sig.sig_openclose == alloc_position_effect_t::open_){
		new_or.PositionEffect = TAPI_PositionEffect_OPEN;
	} else if (sig.sig_openclose == alloc_position_effect_t::close_){
		new_or.PositionEffect = TAPI_PositionEffect_COVER;
	} else{
		 clog_warning("do support PositionEffect value:%d; sig id:%d",
			sig.sig_openclose, sig.sig_id); 
	}
	// volume
    new_or.OrderQty = vol;
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

