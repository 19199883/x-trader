#include "esunny_trade_interface.h"
#include <stdlib.h>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include "my_protocol_packager.h"
#include "esunny_data_formater.h"
#include "TapAPIError.h"

using namespace std;


// discard this info
void MYEsunnyTradeSpi::OnRtnFill(const TapAPIFillInfo* info) { }

void MYEsunnyTradeSpi::OnRspQryPosition(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
    const TapAPIPositionInfo* info) { }

void MYEsunnyTradeSpi::OnRtnPosition(const TapAPIPositionInfo* info) { }

void MYEsunnyTradeSpi::OnRspQryClose(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICloseInfo* info) { }

void MYEsunnyTradeSpi::OnRtnClose(const TapAPICloseInfo* info) { }

void MYEsunnyTradeSpi::OnRtnPositionProfit(const TapAPIPositionProfitNotice* info) { }

void MYEsunnyTradeSpi::OnRspQryDeepQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
    const TapAPIDeepQuoteQryRsp* info) { }

void MYEsunnyTradeSpi::OnRspQryExchangeStateInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIExchangeStateInfo* info) {
}

void MYEsunnyTradeSpi::OnRtnExchangeStateInfo(const TapAPIExchangeStateInfoNotice* info)
{
    TNL_LOG_INFO("OnRtnExchangeStateInfo: \n%s", ESUNNYDatatypeFormater::ToString(info).c_str());
}

bool MYEsunnyTradeSpi::IsOrderTerminate(const TapAPIOrderInfo& order_field)
{
    return order_field.OrderState == TAPI_ORDER_STATE_FINISHED
        || order_field.OrderState == TAPI_ORDER_STATE_CANCELED
        || order_field.OrderState == TAPI_ORDER_STATE_LEFTDELETED
        || order_field.OrderState == TAPI_ORDER_STATE_FAIL
        || order_field.OrderState == TAPI_ORDER_STATE_DELETED
        || order_field.OrderState == TAPI_ORDER_STATE_DELETEDFOREXPIRE;
}

