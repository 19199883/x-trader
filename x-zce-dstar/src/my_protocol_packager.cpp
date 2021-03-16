#include "my_protocol_packager.h"
#include <sstream>
#include <stdlib.h>
#include "vrt_value_obj.h"


char ESUNNYPacker::new_udporder_[UDP_ORDER_INSERT_LEN] = {};
char ESUNNYPacker::delete_udporder_[UDP_ORDER_DELETE_LEN] = {};

// ok
void ESUNNYPacker::InitDeleteUdpOrder()
{
    DstarApiHead * head = (DstarApiHead *)ESUNNYPacker::delete_udporder_;
	head->FrameFlag = DSTAR_API_HEAD_FLAG;
	head->FieldCount = 1;
	head->FieldSize = sizeof(DstarApiReqOrderDeleteField);
	head->ProtocolCode = CMD_API_Req_OrderDelete;
	head->Version = DSTAR_API_PROTOCOL_VERSION;				

}

// ok
void ESUNNYPacker::InitNewUdpOrder(const char *account, char *upperchannel)
{
    DstarApiHead * head = (DstarApiHead *)ESUNNYPacker::new_udporder_;
	head->FrameFlag = DSTAR_API_HEAD_FLAG;
	head->FieldCount = 1;
	head->FieldSize = sizeof(DstarApiReqOrderInsertField);
	head->ProtocolCode = CMD_API_Req_OrderInsert;
	head->Version = DSTAR_API_PROTOCOL_VERSION;

    DstarApiReqOrderInsertField* req = (DstarApiReqOrderInsertField *)(
				ESUNNYPacker::new_udporder_ + sizeof(DstarApiHead));
	req->Hedge = DSTAR_API_HEDGE_SPECULATE;
	req->OrderType = DSTAR_API_ORDERTYPE_LIMIT;
	req->ValidType = DSTAR_API_VALID_GFD;
	req->MinQty = 0;
}

// TODO: to here
char* ESUNNYPacker::DeleteUdpOrderRequest(
			long localorderid, 
			const char *orderNo)
{
    DstarApiReqOrderDeleteField* req = (DstarApiReqOrderDeleteField*)(
				ESUNNYPacker::delete_udporder_ + sizeof(DstarApiHead));
	req->ClientReqId = localorderid;
	strncpy(req->OrderNo, orderNo, sizeof(DstarApiOrderNoType) - 1);

	// TODO: here
	strcpy(pDeleteOrder->OrderNo, orderNo);

	return ESUNNYPacker::delete_udporder_;
}

	// ok
char* ESUNNYPacker::UdpOrderRequest(
			const signal_t& sig,
			long localorderid,
			int32_t vol)
{
    DstarApiReqOrderInsertField* req = (DstarApiReqOrderInsertField*)(
				ESUNNYPacker::new_udporder_ + sizeof(DstarApiHead));

	sprintf(req->ClientReqId, "%u", localorderid);
	strcpy(req->ContractNo, sig.symbol);
	if (sig.sig_act==signal_act_t::buy)
	{
		req->OrderPrice  = sig.buy_price;
		req->Direct = DSTAR_API_DIRECT_BUY;
	} 
	else if (sig.sig_act==signal_act_t::sell)
	{
		req->OrderPrice = sig.sell_price;
		req->Direct = DSTAR_API_DIRECT_SELL;
	} 
	else
	{
		 clog_error("do support BuySellType value:%d; sig id:%d",
			sig.sig_act, 
			sig.sig_id); 
	}
	if (sig.sig_openclose == alloc_position_effect_t::open_)
	{
		req->Offset = DSTAR_API_OFFSET_OPEN;
	} 
	else if (sig.sig_openclose == alloc_position_effect_t::close_)
	{
		req->Offset = DSTAR_API_OFFSET_CLOSE;
	}
	else
	{
		 clog_error("do support PositionEffect value:%d; sig id:%d",
			sig.sig_openclose, 
			sig.sig_id); 
	}
    req->OrderQty = vol;
	
	return ESUNNYPacker::new_udporder_;
}
#else
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
    new_order_.FutureAutoCloseFlag = APIYNFLAG_NO;
    new_order_.HedgeFlag2 = TAPI_HEDGEFLAG_NONE;
    new_order_.MarketLevel= TAPI_MARKET_LEVEL_0;
    //new_order_.OrderDeleteByDisConnFlag = APIYNFLAG_NO;
}

	
TapAPINewOrder* ESUNNYPacker::OrderRequest(
			const signal_t& sig,
			const char *account,
			long localorderid,
			int32_t vol)
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
		 clog_error("do support BuySellType value:%d; sig id:%d",
			sig.sig_act, sig.sig_id); 
	}
	// position effect
	if (sig.sig_openclose == alloc_position_effect_t::open_){
		new_order_.PositionEffect = TAPI_PositionEffect_OPEN;
	} else if (sig.sig_openclose == alloc_position_effect_t::close_){
		new_order_.PositionEffect = TAPI_PositionEffect_COVER;
	} else{
		 clog_error("do support PositionEffect value:%d; sig id:%d",
			sig.sig_openclose, sig.sig_id); 
	}
	// volume
    new_order_.OrderQty = vol;

	return &new_order_;
}

#endif
