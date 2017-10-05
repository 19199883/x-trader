#include "my_protocol_packager.h"
#include <sstream>
#include <stdlib.h>
#include "vrt_value_obj.h"

// done
void ESUNNYPacker::InitNewOrder(const char *account)
{
    memset(&new_order_, 0, sizeof(new_order_));
    strcpy(new_order_.AccountNo,account);
    strcpy(new_order_.ExchangeNo, EXCHANGE_NO);
    new_order_.CommodityType = TAPI_COMMODITY_TYPE_FUTURES; 
    strcpy(new_order_.StrikePrice, "");
    new_order_.CallOrPutFlag = TAPI_CALLPUT_FLAG_NONE;
    strcpy(new_order_.ContractNo2, "");
    strcpy(new_order_.StrikePrice2, "");
    new_order_.CallOrPutFlag2 = TAPI_CALLPUT_FLAG_NONE;
    new_order_.OrderType = TAPI_ORDER_TYPE_LIMIT;
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
    new_order_.HedgeFlag2 = TAPI_HEDGEFLAG_NONE;
    new_order_.MarketLevel= TAPI_MARKET_LEVEL_0;
    new_order_.OrderDeleteByDisConnFlag = APIYNFLAG_NO;
}

// done
TapAPINewOrder* ESUNNYPacker::OrderRequest(const signal_t& sig,const char *account,
			long localorderid,int32_t vol)
{
	// contract
    strncpy(new_order_.CommodityNo,sig.symbol,2);
	new_order_.CommodityNo[2] = 0;
    strncpy(new_order_.ContractNo, sig.symbol+2,3);
	new_order_.ContractNo[3] = 0;
	// side
	if (sig.sig_act==signal_act_t::buy){
		new_order_.OrderPrice = sig.buy_price;
		new_order_.OrderSide = TAPI_SIDE_BUY;
	} else if (sig.sig_act==signal_act_t::sell){
		new_order_.OrderPrice = sig.sell_price;
		new_order_.OrderSide = TAPI_SIDE_SELL;
	} else{
		 clog_warning("do support BuySellType value:%d; sig id:%d",
			sig.sig_act, sig.sig_id); 
	}
	// position effect
	if (sig.sig_openclose == alloc_position_effect_t::open_){
		new_order_.PositionEffect = TAPI_PositionEffect_OPEN;
	} else if (sig.sig_openclose == alloc_position_effect_t::close_){
		new_order_.PositionEffect = TAPI_PositionEffect_COVER;
	} else{
		 clog_warning("do support PositionEffect value:%d; sig id:%d",
			sig.sig_openclose, sig.sig_id); 
	}
	// volume
    new_order_.OrderQty = vol;

	return &new_order_;
}


