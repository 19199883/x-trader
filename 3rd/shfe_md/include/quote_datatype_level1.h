﻿#pragma once

#ifndef DLL_PUBLIC
#define DLL_PUBLIC  __attribute__ ((visibility("default")))
#endif

#include <string>

#pragma pack(push)
#pragma pack(8)

///深度市场行情（一档行情结构）
struct DLL_PUBLIC CDepthMarketDataField
{
    ///交易日
    char TradingDay[9];
    ///结算组代码
    char SettlementGroupID[9];
    ///结算编号
    int SettlementID;
    ///最新价
    double LastPrice;
    ///昨结算
    double PreSettlementPrice;
    ///昨收盘
    double PreClosePrice;
    ///昨持仓量
    double PreOpenInterest;
    ///今开盘
    double OpenPrice;
    ///最高价
    double HighestPrice;
    ///最低价
    double LowestPrice;
    ///数量
    int Volume;
    ///成交金额
    double Turnover;
    ///持仓量
    double OpenInterest;
    ///今收盘
    double ClosePrice;
    ///今结算
    double SettlementPrice;
    ///涨停板价
    double UpperLimitPrice;
    ///跌停板价
    double LowerLimitPrice;
    ///昨虚实度
    double PreDelta;
    ///今虚实度
    double CurrDelta;
    ///最后修改时间
    char UpdateTime[9];
    ///最后修改毫秒
    int UpdateMillisec;
    ///合约代码
    char InstrumentID[31];
    ///申买价一
    double BidPrice1;
    ///申买量一
    int BidVolume1;
    ///申卖价一
    double AskPrice1;
    ///申卖量一
    int AskVolume1;
    ///申买价二
    double BidPrice2;
    ///申买量二
    int BidVolume2;
    ///申卖价二
    double AskPrice2;
    ///申卖量二
    int AskVolume2;
    ///申买价三
    double BidPrice3;
    ///申买量三
    int BidVolume3;
    ///申卖价三
    double AskPrice3;
    ///申卖量三
    int AskVolume3;
    ///申买价四
    double BidPrice4;
    ///申买量四
    int BidVolume4;
    ///申卖价四
    double AskPrice4;
    ///申卖量四
    int AskVolume4;
    ///申买价五
    double BidPrice5;
    ///申买量五
    int BidVolume5;
    ///申卖价五
    double AskPrice5;
    ///申卖量五
    int AskVolume5;
    ///业务发生日期
    char ActionDay[9];

    // HH:MM:SS.mmm
    std::string GetQuoteTime() const
    {
        char buf[64];
        sprintf(buf, "%s.%03d", UpdateTime, UpdateMillisec);
        return std::string(buf);
    }
};

#pragma pack(pop)
