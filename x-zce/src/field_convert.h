#ifndef FIELD_CONVERT_H_
#define FIELD_CONVERT_H_

#include "trade_data_type.h"
#include "TapAPIError.h"
#include "TapTradeAPIDataType.h"

#include <string.h>
#include <iomanip>
#include <string>
#include <map>

class ESUNNYFieldConvert
{
public:

private:
};

//将ESUNNY的委托状态转换为统一的委托状态
char ESUNNYFieldConvert::GetMYEntrustStatus(char OrderStatus)
{
    if (OrderStatus == TAPI_ORDER_STATE_PARTFINISHED)
    {
        return MY_TNL_OS_PARTIALCOM;
    } else if (OrderStatus == TAPI_ORDER_STATE_FINISHED) {
        return MY_TNL_OS_COMPLETED;
    } else if (OrderStatus == TAPI_ORDER_STATE_FAIL) {
        return MY_TNL_OS_ERROR;
    } else if (OrderStatus == TAPI_ORDER_STATE_CANCELED || OrderStatus == TAPI_ORDER_STATE_LEFTDELETED
        || OrderStatus == TAPI_ORDER_STATE_DELETED
        || OrderStatus == TAPI_ORDER_STATE_DELETEDFOREXPIRE) {
        return MY_TNL_OS_WITHDRAWED;
    }

    //已经报入
    return MY_TNL_OS_REPORDED;
}


#endif // FIELD_CONVERT_H_
