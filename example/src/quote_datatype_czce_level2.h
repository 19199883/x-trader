#pragma once

#ifndef DLL_PUBLIC
#define DLL_PUBLIC  __attribute__ ((visibility("default")))
#endif

#include <string>

#pragma pack(push)
#pragma pack(8)

////////////////////////////////////////////////
///行情全文
////////////////////////////////////////////////
struct DLL_PUBLIC ZCEL2QuotSnapshotField_MY
{
    char ContractID[50];    /*合约编码*/
    char ContractIDType;    /*合约类型 0->目前应该为0， 扩充：0:期货,1:期权,2:组合*/
    double PreSettle;       /*前结算价格*/
    double PreClose;        /*前收盘价格*/
    int PreOpenInterest;    /*昨日空盘量*/
    double OpenPrice;       /*开盘价*/
    double HighPrice;       /*最高价*/
    double LowPrice;        /*最低价*/
    double LastPrice;       /*最新价*/
    double BidPrice[5];     /*买入价格 下标从0开始*/
    double AskPrice[5];     /*卖出价 下标从0开始*/
    int BidLot[5];          /*买入数量 下标从0开始*/
    int AskLot[5];          /*卖出数量 下标从0开始*/
    int TotalVolume;        /*总成交量*/
    int OpenInterest;       /*持仓量*/
    double ClosePrice;      /*收盘价*/
    double SettlePrice;     /*结算价*/
    double AveragePrice;    /*均价*/
    double LifeHigh;        /*历史最高成交价格*/
    double LifeLow;         /*历史最低成交价格*/
    double HighLimit;       /*涨停板*/
    double LowLimit;        /*跌停板*/
    int TotalBidLot;        /*委买总量*/
    int TotalAskLot;        /*委卖总量*/
    char TimeStamp[24];		//时间：如2014-02-03 13:23:45

    // HH:MM:SS.mmm
    std::string GetQuoteTime() const
    {
        return std::string(TimeStamp + 11, 12);
    }
};

////////////////////////////////////////////////
//组合行情
struct ZCEQuotCMBQuotField_MY
{
    char CmbType;           /*组合类型 1 -- SPD, 2 -- IPS*/
    char ContractID[50];    /*合约编码*/
    double BidPrice;        /*买入价格*/
    double AskPrice;        /*卖出价*/
    int BidLot;             /*买入数量*/
    int AskLot;             /*卖出数量*/
    int VolBidLot;          /*总买入数量*/
    int VolAskLot;          /*总卖出数量*/
    char TimeStamp[24];		//时间：如2008-02-03 13:23:45

    // HH:MM:SS.mmm
    std::string GetQuoteTime() const
    {
        return std::string(TimeStamp + 11, 12);
    }
};

#pragma pack(pop)

