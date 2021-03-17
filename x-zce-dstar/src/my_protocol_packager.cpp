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

// ok
char* ESUNNYPacker::DeleteUdpOrderRequest(
			long localorderid, 
			const char *orderNo,
			const char *systemNo)
{
    DstarApiReqOrderDeleteField* req = (DstarApiReqOrderDeleteField*)(
				ESUNNYPacker::delete_udporder_ + sizeof(DstarApiHead));
	req->ClientReqId = localorderid;
	strcpy(req->OrderNo, orderNo);
	strcpy(req->SystemNo, systemNo);

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

	req->ClientReqId = localorderid;
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
