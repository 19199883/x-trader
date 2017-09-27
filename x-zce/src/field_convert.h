#ifndef FIELD_CONVERT_H_
#define FIELD_CONVERT_H_

#include "trade_data_type.h"
#include "TapAPIError.h"
#include "TapTradeAPIDataType.h"

#include <string.h>
#include <iomanip>
#include <string>
#include <map>
#include <unordered_map>
#include <mutex>
#include "my_cmn_log.h"

//typedef std::unordered_map<std::string, EsunnyContractInfo> ContractInfoMap;
typedef std::map<std::string, TapAPITradeContractInfo> ContractInfoMap;

class ESUNNYFieldConvert
{
public:
    // 转换成ESUNNY字段取值
    inline static char GetESUNNYSide(char my_side);
    inline static char GetESUNNYOCFlag(char open_close);
    inline static char GetESUNNYHedgeType(char hedge_flag);
    inline static char GetESUNNYPriceType(char price_type);
    inline static char GetESUNNYTimeCondition(char price_type);
    inline static const char * ExchCodeToExchName(char cExchCode);

    // 从ESUNNY转换会MY协议取值
    inline static char GetMYEntrustStatus(char OrderStatus);
    inline static char GetMYHedgeFlag(char HedgeFlag);
    inline static char GetMYPriceType(char PriceType);
    inline static char GetMYSide(char esunny_side);
    inline static char GetMYOCFlag(char esunny_oc);
    inline static char GetMYOrderKind(char time_force);

    // contract info cache
    inline static const TapAPITradeContractInfo *GetContractInfo(const std::string &contract);
    inline static void AddContractInfo(const TapAPITradeContractInfo &info);

private:
    static ContractInfoMap contract_in_market;
    static std::mutex contract_mutex;
};

char ESUNNYFieldConvert::GetESUNNYSide(char my_side)
{
    if (my_side == MY_TNL_D_BUY)
    {
        return TAPI_SIDE_BUY;
    }

    return TAPI_SIDE_SELL;
}

char ESUNNYFieldConvert::GetESUNNYOCFlag(char open_close)
{
    if (open_close == MY_TNL_D_OPEN)
    {
        return TAPI_PositionEffect_OPEN;
    }
    else if (open_close == MY_TNL_D_CLOSE || open_close == MY_TNL_D_CLOSEYESTERDAY)
    {
        return TAPI_PositionEffect_COVER;
    }
    else if (open_close == MY_TNL_D_CLOSETODAY)
    {
        return TAPI_PositionEffect_COVER_TODAY;
    }

    return TAPI_PositionEffect_NONE;
}

char ESUNNYFieldConvert::GetESUNNYHedgeType(char hedge_flag)
{
    if (hedge_flag == MY_TNL_HF_SPECULATION)
    {
        return TAPI_HEDGEFLAG_T;
    }
    else if (hedge_flag == MY_TNL_HF_HEDGE)
    {
        return TAPI_HEDGEFLAG_B;
    }

    return TAPI_HEDGEFLAG_NONE;
}

char ESUNNYFieldConvert::GetESUNNYPriceType(char price_type)
{
    if (price_type == MY_TNL_OPT_ANY_PRICE)
    {
        return TAPI_ORDER_TYPE_MARKET;
    }
    return TAPI_ORDER_TYPE_LIMIT;
}

char ESUNNYFieldConvert::GetESUNNYTimeCondition(char order_type)
{
    if (order_type == MY_TNL_HF_FAK)
    {
        return TAPI_ORDER_TIMEINFORCE_FAK;
    }
    else if (order_type == MY_TNL_HF_FOK)
    {
        return TAPI_ORDER_TIMEINFORCE_FOK;
    }
    return TAPI_ORDER_TIMEINFORCE_GFD;
}

const char * ESUNNYFieldConvert::ExchCodeToExchName(char cExchCode)
{
    if (cExchCode == MY_TNL_EC_SHFE)
    {
        return MY_TNL_EXID_SHFE;
    }
    else if (cExchCode == MY_TNL_EC_DCE)
    {
        return MY_TNL_EXID_DCE;
    }
    else if (cExchCode == MY_TNL_EC_CZCE)
    {
        return MY_TNL_EXID_CZCE;
    }
    else if (cExchCode == MY_TNL_EC_CFFEX)
    {
        return MY_TNL_EXID_CFFEX;
    }
    else
    {
        return "";
    }
}

//将ESUNNY的委托状态转换为统一的委托状态
char ESUNNYFieldConvert::GetMYEntrustStatus(char OrderStatus)
{
    if (OrderStatus == TAPI_ORDER_STATE_PARTFINISHED)
    {
        return MY_TNL_OS_PARTIALCOM;
    }
    else if (OrderStatus == TAPI_ORDER_STATE_FINISHED)
    {
        return MY_TNL_OS_COMPLETED;
    }
    else if (OrderStatus == TAPI_ORDER_STATE_FAIL)
    {
        return MY_TNL_OS_ERROR;
    }
    else if (OrderStatus == TAPI_ORDER_STATE_CANCELED
        || OrderStatus == TAPI_ORDER_STATE_LEFTDELETED
        || OrderStatus == TAPI_ORDER_STATE_DELETED
        || OrderStatus == TAPI_ORDER_STATE_DELETEDFOREXPIRE)
    {
        return MY_TNL_OS_WITHDRAWED;
    }

    //已经报入
    return MY_TNL_OS_REPORDED;
}

//将ESUNNY的投机、套保标志转换为统一的委托状态
char ESUNNYFieldConvert::GetMYHedgeFlag(char HedgeFlag)
{
    //套保
    if (HedgeFlag == TAPI_HEDGEFLAG_B)
    {
        return MY_TNL_HF_HEDGE;
    }
    return MY_TNL_HF_SPECULATION;
}

char ESUNNYFieldConvert::GetMYPriceType(char PriceType)
{
    if (PriceType == TAPI_ORDER_TYPE_MARKET)
    {
        return MY_TNL_OPT_ANY_PRICE;
    }
    return MY_TNL_OPT_LIMIT_PRICE;
}

char ESUNNYFieldConvert::GetMYSide(char esunny_side)
{
    if (esunny_side == TAPI_SIDE_BUY)
    {
        return MY_TNL_D_BUY;
    }
    return MY_TNL_D_SELL;
}

inline char ESUNNYFieldConvert::GetMYOCFlag(char esunny_oc)
{
    if (TAPI_PositionEffect_OPEN == esunny_oc)
    {
        return MY_TNL_D_OPEN;
    }
    else if (TAPI_PositionEffect_COVER == esunny_oc)
    {
        return MY_TNL_D_CLOSE;
    }
    else if (TAPI_PositionEffect_COVER_TODAY == esunny_oc)
    {
        return MY_TNL_D_CLOSETODAY;
    }

    return MY_TNL_D_CLOSE;
}

inline char ESUNNYFieldConvert::GetMYOrderKind(char time_force)
{
    if (TAPI_ORDER_TIMEINFORCE_FAK == time_force)
    {
        return MY_TNL_HF_FAK;
    }
    else if (TAPI_ORDER_TIMEINFORCE_FOK == time_force)
    {
        return MY_TNL_HF_FOK;
    }

    return MY_TNL_HF_NORMAL;
}

inline const TapAPITradeContractInfo* ESUNNYFieldConvert::GetContractInfo(const std::string& contract)
{
    std::lock_guard<std::mutex> lock(contract_mutex);

    ContractInfoMap::const_iterator cit = contract_in_market.find(contract);
    if (cit != contract_in_market.end())
    {
        return &(cit->second);
    }

    return NULL;
}

inline void ESUNNYFieldConvert::AddContractInfo(const TapAPITradeContractInfo& info)
{
    std::lock_guard<std::mutex> lock(contract_mutex);
	if (TAPI_COMMODITY_TYPE_FUTURES != info.CommodityType) return;
	
    std::string contract(info.CommodityNo);
    contract.append(info.ContractNo1);
    if (info.ContractNo2[0] != '\0')
    {
        contract.append("/");
        contract.append(info.CommodityNo);
        contract.append(info.ContractNo2);
    }

    ContractInfoMap::iterator it = contract_in_market.find(contract);
    if (it != contract_in_market.end())
    {
        it->second = info;
    }
    else
    {
        contract_in_market.insert(std::make_pair(contract, info));
    }
}

#endif // FIELD_CONVERT_H_
