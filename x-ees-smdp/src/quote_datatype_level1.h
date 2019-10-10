#pragma once

#include "ThostFtdcMdApi.h"

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

class ShfeLev2Formater
{
public:

	/*
	 * 对source指定的行情数据进行格式化后存储到dest
	 */
	static char* Format(CThostFtdcDepthMarketDataField&source,char *dest)
	{
		
		sprintf (dest,
			"CThostFtdcDepthMarketDataField  "
			"TradingDay:%s; "
			"LastPrice:%f; "
			"PreSettlementPrice:%f; "
			"PreClosePrice:%f; "
			"PreOpenInterest:%f; "
			"OpenPrice:%f; "
			"HighestPrice:%f; "
			"LowestPrice:%f; "
			"Volume:%d; "
			"Turnover:%f; "
			"OpenInterest:%f; "
			"ClosePrice:%f; "
			"SettlementPrice:%f; "
			"UpperLimitPrice:%f; "
			"LowerLimitPrice:%f; "
			"PreDelta:%f; "
			"CurrDelta:%f; "
			"UpdateTime[9]:%s; "
			"UpdateMillisec:%d; "
			"InstrumentID:%s; "
			"BidPrice1:%f; "
			"BidVolume1:%d; "
			"AskPrice1:%f; "
			"AskVolume1:%d; "
			"BidPrice2:%f; "
			"BidVolume2:%d; "
			"AskPrice2:%f; "
			"AskVolume2:%d; "
			"BidPrice3:%f; "
			"BidVolume3:%d; "
			"AskPrice3:%f; "
			"AskVolume3:%d; "
			"BidPrice4:%f; "
			"BidVolume4:%d; "
			"AskPrice4:%f; "
			"AskVolume4:%d; "
			"BidPrice5:%f; "
			"BidVolume5:%d; "
			"AskPrice5:%f; "
			"AskVolume5:%d; "
			"ActionDay:%s;",
			source.TradingDay,
			source.LastPrice,
			source.PreSettlementPrice,
			source. PreClosePrice,
			source.PreOpenInterest,
			source.OpenPrice,
			source. HighestPrice,
			source. LowestPrice,
			source.Volume,
			source.Turnover,
			source.OpenInterest,
			source.ClosePrice,
			source.SettlementPrice,
			source.UpperLimitPrice,
			source.LowerLimitPrice,
			source.PreDelta,
			source.CurrDelta,
			source.UpdateTime,
			source.UpdateMillisec,
			source.InstrumentID,
			source.BidPrice1,
			source.BidVolume1,
			source.AskPrice1,
			source.AskVolume1,
			source.BidPrice2,
			source.BidVolume2,
			source.AskPrice2,
			source.AskVolume2,
			source.BidPrice3,
			source.BidVolume3,
			source.AskPrice3,
			source.AskVolume3,
			source.BidPrice4,
			source.BidVolume4,
			source.AskPrice4,
			source.AskVolume4,
			source.BidPrice5,
			source.BidVolume5,
			source.AskPrice5,
			source.AskVolume5,
			source.ActionDay);

		return dest;
	}
	/*
	 * 对source指定的行情数据进行格式化后存储到dest
	 */
	static char* Format(CDepthMarketDataField &source, char *dest)
	{
		
		sprintf (dest,
			"CDepthMarketDataField "
			"TradingDay:%s; "
			"SettlementGroupID:%s; "
			"SettlementID:%d; "
			"LastPrice:%f; "
			"PreSettlementPrice:%f; "
			"PreClosePrice:%f; "
			"PreOpenInterest:%f; "
			"OpenPrice:%f; "
			"HighestPrice:%f; "
			"LowestPrice:%f; "
			"Volume:%d; "
			"Turnover:%f; "
			"OpenInterest:%f; "
			"ClosePrice:%f; "
			"SettlementPrice:%f; "
			"UpperLimitPrice:%f; "
			"LowerLimitPrice:%f; "
			"PreDelta:%f; "
			"CurrDelta:%f; "
			"UpdateTime[9]:%s; "
			"UpdateMillisec:%d; "
			"InstrumentID:%s; "
			"BidPrice1:%f; "
			"BidVolume1:%d; "
			"AskPrice1:%f; "
			"AskVolume1:%d; "
			"BidPrice2:%f; "
			"BidVolume2:%d; "
			"AskPrice2:%f; "
			"AskVolume2:%d; "
			"BidPrice3:%f; "
			"BidVolume3:%d; "
			"AskPrice3:%f; "
			"AskVolume3:%d; "
			"BidPrice4:%f; "
			"BidVolume4:%d; "
			"AskPrice4:%f; "
			"AskVolume4:%d; "
			"BidPrice5:%f; "
			"BidVolume5:%d; "
			"AskPrice5:%f; "
			"AskVolume5:%d; "
			"ActionDay:%s;",
			source.TradingDay,
			source.SettlementGroupID,
			source.SettlementID,
			source.LastPrice,
			source.PreSettlementPrice,
			source. PreClosePrice,
			source.PreOpenInterest,
			source.OpenPrice,
			source. HighestPrice,
			source. LowestPrice,
			source.Volume,
			source.Turnover,
			source.OpenInterest,
			source.ClosePrice,
			source.SettlementPrice,
			source.UpperLimitPrice,
			source.LowerLimitPrice,
			source.PreDelta,
			source.CurrDelta,
			source.UpdateTime,
			source.UpdateMillisec,
			source.InstrumentID,
			source.BidPrice1,
			source.BidVolume1,
			source.AskPrice1,
			source.AskVolume1,
			source.BidPrice2,
			source.BidVolume2,
			source.AskPrice2,
			source.AskVolume2,
			source.BidPrice3,
			source.BidVolume3,
			source.AskPrice3,
			source.AskVolume3,
			source.BidPrice4,
			source.BidVolume4,
			source.AskPrice4,
			source.AskVolume4,
			source.BidPrice5,
			source.BidVolume5,
			source.AskPrice5,
			source.AskVolume5,
			source.ActionDay);

		return dest;
	}
};
