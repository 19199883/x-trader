/*
 * esunny_data_formater.cpp
 *
 *  Created on: 2013-8-13
 *      Author: oliver
 */

#include "vrt_value_obj.h"
#include "esunny_data_formater.h"

using namespace std;


// ok
std::string ESUNNYDatatypeFormater::ToString(const DstarApiRspLoginField *p)
{
    char buf[1024];
    if (p) 
	{
        snprintf(buf, 
				sizeof(buf), 
				"structName=DstarApiRspLoginField "
				"AccountNo:%s "
				"AccountIndex:%hu "
				"ErrorCode:%u "
				"UdpAuthCode:%u "
				"TradeDate=%s\n",
				p->AccountNo, 
				p->AccountIndex, 
				p->ErrorCode, 
				p->UdpAuthCode,
				p->TradeDate);
    } 
	else 
	{
        snprintf(buf, sizeof(buf), "structName=TradeDate <null>");
    }

    return buf;
}

// ok
std::string ESUNNYDatatypeFormater::ToString(const DstarApiRspUdpAuthField *p)
{
    char buf[1024];
    if (p) 
	{
        snprintf(buf, 
				sizeof(buf), 
				"structName=DstarApiRspUdpAuthField "
				"AccountIndex:%hu "
				"UdpAuthCode:%u "
				"ErrorCode:%u " 
				"ReqIdMode:%hhu\n",
				p->AccountIndex, 
				p->UdpAuthCode, 
				p->ErrorCode,
				p->ReqIdMode);
    } 
	else 
	{
        snprintf(buf, 
				sizeof(buf), 
				"structName=DstarApiRspUdpAuthField <null>");
    }

    return buf;
}


// ok
std::string ESUNNYDatatypeFormater::ToString(const DstarApiContractField *p)
{
    char buf[1024];
    if (p) 
	{
        snprintf(buf, 
				sizeof(buf), 
				"structName=DstarApiContractField "
				"ExchangeId:%c "
				"CommodityType:%c "
				"ContractIndex:%u " 
				"ContractSize:%d"
				"ContractNo:%s \n" ,
				p->ExchangeId, 
				p->CommodityType, 
				p->ContractIndex,
				p->ContractSize,
				p->ContractNo);
    } 
	else 
	{
        snprintf(buf, 
					sizeof(buf), 
					"structName=DstarApiContractField <null>");
    }

    return buf;
}


// ok
std::string ESUNNYDatatypeFormater::ToString(const DstarApiSeatField *p)
{
    char buf[1024];
    if (p) 
	{
        snprintf(buf, 
				sizeof(buf), 
				"structName=DstarApiSeatField "
				"SeatIndex:%hhu "
				"SeatNo:%s "
				"Exchange:%c \n" ,
				p->SeatIndex, 
				p->SeatNo, 
				p->Exchange);
    } 
	else 
	{
        snprintf(buf, 
				sizeof(buf), 
				"structName=DstarApiSeatField <null>");
    }

    return buf;
}


// ok
std::string ESUNNYDatatypeFormater::ToString(const DstarApiRspOrderInsertField *p)
{
    char buf[1024];
    if (p) 
	{
        snprintf(buf, 
				sizeof(buf), 
				"structName=DstarApiSeatField "
				"SeatIndex:%hhu "
				"AccountNo:%s "
				"ClientReqId:%u " 
				"Reference:%d "
				"MaxClientReqId:%u "
				"OrderNo:%s "
				"ErrCode:%u \n" ,
				p->SeatIndex, 
				p->AccountNo, 
				p->ClientReqId,
				p->Reference,
				p->MaxClientReqId,
				p->OrderNo,
				p->ErrCode);
    } 
	else 
	{
        snprintf(buf, 
				sizeof(buf), 
				"structName=DstarApiSeatField <null>");
    }

    return buf;
}


// ok
std::string ESUNNYDatatypeFormater::ToString(const DstarApiOrderField *p)
{
    char buf[1024];
    if (p) 
	{
        snprintf(buf, 
				sizeof(buf), 
				"structName=DstarApiSeatField "
				"MatchQty:%u "
				"ErrCode:%u "
				"OrderLocalNo:%s "
				"SystemNo:%s "
				"Reference:%d "
				"OrderState:%c "
				"SeatIndex:%hhu "
				"AccountNo:%s "
				"OrderNo:%s \n" ,
				p->MatchQty, 
				p->ErrCode,
				p->OrderLocalNo,
				p->SystemNo,
				p->Reference,
				p->OrderState,
				p->SeatIndex, 
				p->AccountNo, 
				p->OrderNo);
    } 
	else 
	{
        snprintf(buf, 
				sizeof(buf), 
				"structName=DstarApiOrderField <null>");
    }

    return buf;
}

// ok 
std::string ESUNNYDatatypeFormater::ToString(const DstarApiReqUdpAuthField *p)
{
    char buf[1024];
    if (p) 
	{
        snprintf(buf, 
				sizeof(buf), 
				"structName=DstarApiReqUdpAuthField "
				"AccountIndex:%hu "
				"UdpAuthCode:%u "
				"ReqIdMode:%hhu \n" ,
				p->AccountIndex, 
				p->UdpAuthCode,
				p->ReqIdMode);
    } 
	else 
	{
        snprintf(buf, 
					sizeof(buf), 
					"structName=DstarApiReqUdpAuthField <null>");
    }

    return buf;
}

// ok
std::string ESUNNYDatatypeFormater::ToString(const DstarApiReqOrderInsertField *p)
{
    char buf[1024];
    if (p) 
	{
        snprintf(buf, 
				sizeof(buf), 
				"structName=DstarApiReqOrderInsertField "
				"Direct:%c "
				"Offset:%c "
				"Hedge:%c "
				"OrderType:%c "
				"ValidType:%c "
				"SeatIndex:%hhu "
				"AccountIndex:%hu "
				"ContractIndex:%u "
				"ContractNo:%s "
				"OrderQty:%u "
				"MinQty:%u "
				"OrderPrice:%f "
				"ClientReqId:%u "
				"Reference:%d "
				"UdpAuthCode:%u \n" ,
				p->Direct,
				p->Offset,
				p->Hedge,
				p->OrderType,
				p->ValidType,
				p->SeatIndex, 
				p->AccountIndex, 
				p->ContractIndex, 
				p->ContractNo, 
				p->OrderQty, 
				p->MinQty, 
				p->OrderPrice, 
				p->ClientReqId, 
				p->Reference, 
				p->UdpAuthCode);
    } 
	else 
	{
        snprintf(buf, 
				sizeof(buf), 
				"structName=DstarApiReqOrderInsertField <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const DstarApiReqOrderDeleteField *p)
{
    char buf[1024];
    if (p) 
	{
        snprintf(buf, 
				sizeof(buf), 
				"structName=DstarApiReqOrderDeleteField "
				"AccountIndex:%hu "
				"ClientReqId:%u "
				"SeatIndex:%hhu "
				"Reference:%d "
				"UdpAuthCode:%u "
				"OrderNo:%s \n" ,
				p->AccountIndex, 
				p->ClientReqId, 
				p->SeatIndex, 
				p->Reference, 
				p->UdpAuthCode,
				p->OrderNo);
    } 
	else 
	{
        snprintf(buf, 
				sizeof(buf), 
				"structName=DstarApiReqOrderDeleteField <null>");
    }

    return buf;
}
