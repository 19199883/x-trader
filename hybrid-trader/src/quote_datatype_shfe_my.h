#pragma once

#ifndef DLL_PUBLIC
#define DLL_PUBLIC  __attribute__ ((visibility("default")))
#endif

#include <string.h>
#include <string>

#pragma pack(push)
#pragma pack(8)

// 档位数，暂定为30档
#define  MY_SHFE_QUOTE_PRICE_POS_COUNT 30

///MY自定义的上期行情数据结构
struct DLL_PUBLIC MYShfeMarketData
{
	///交易日
	char	TradingDay[9];
	///结算组代码
	char	SettlementGroupID[9];
	///结算编号
	int	SettlementID;
	///最新价
	double	LastPrice;
	///昨结算
	double	PreSettlementPrice;
	///昨收盘
	double	PreClosePrice;
	///昨持仓量
	double	PreOpenInterest;
	///今开盘
	double	OpenPrice;
	///最高价
	double	HighestPrice;
	///最低价
	double	LowestPrice;
	///数量
	int	Volume;
	///成交金额
	double	Turnover;
	///持仓量
	double	OpenInterest;
	///今收盘
	double	ClosePrice;
	///今结算
	double	SettlementPrice;
	///涨停板价
	double	UpperLimitPrice;
	///跌停板价
	double	LowerLimitPrice;
	///昨虚实度
	double	PreDelta;
	///今虚实度
	double	CurrDelta;
	///最后修改时间
	char	UpdateTime[9];
	///最后修改毫秒
	int	UpdateMillisec;
	///合约代码
	char	InstrumentID[31];
	///申买价一
	double	BidPrice1;
	///申买量一
	int	BidVolume1;
	///申卖价一
	double	AskPrice1;
	///申卖量一
	int	AskVolume1;
	///申买价二
	double	BidPrice2;
	///申买量二
	int	BidVolume2;
	///申卖价二
	double	AskPrice2;
	///申卖量二
	int	AskVolume2;
	///申买价三
	double	BidPrice3;
	///申买量三
	int	BidVolume3;
	///申卖价三
	double	AskPrice3;
	///申卖量三
	int	AskVolume3;
	///申买价四
	double	BidPrice4;
	///申买量四
	int	BidVolume4;
	///申卖价四
	double	AskPrice4;
	///申卖量四
	int	AskVolume4;
	///申买价五
	double	BidPrice5;
	///申买量五
	int	BidVolume5;
	///申卖价五
	double	AskPrice5;
	///申卖量五
	int	AskVolume5;
	///业务发生日期
	char	ActionDay[9];

	// 以上为上期交易所的一档数据结构，下面部分为新增的字段
	int   data_flag; // 数据标记，1：市场行情有效；2：委托行情有效；3：市场行情和委托行情都有效

	// 新增的30档委托数据（档数不足时，price、volume 都取值 0）
    unsigned short  start_index;  // 数据的起始索引号，0
    unsigned short  data_count;   // 委托数据档位数，30
	double buy_price[MY_SHFE_QUOTE_PRICE_POS_COUNT];
	int    buy_volume[MY_SHFE_QUOTE_PRICE_POS_COUNT];
	double sell_price[MY_SHFE_QUOTE_PRICE_POS_COUNT];
	int    sell_volume[MY_SHFE_QUOTE_PRICE_POS_COUNT];

	// 加权平均以及委托总量行情
    unsigned int   buy_total_volume;               //买委托总量
    unsigned int   sell_total_volume;              //卖委托总量
    double         buy_weighted_avg_price;   //加权平均委买价格
    double         sell_weighted_avg_price;  //加权平均委卖价格


	MYShfeMarketData()
	{
		data_flag = 2;
		start_index = 0;
		data_count = MY_SHFE_QUOTE_PRICE_POS_COUNT;
		memset(buy_price, 0, sizeof(buy_price));
		memset(buy_volume, 0, sizeof(buy_volume));
		memset(sell_price, 0, sizeof(sell_price));
		memset(sell_volume, 0, sizeof(sell_volume));
	}

    // HH:MM:SS.mmm
    std::string GetQuoteTime() const
    {
        char buf[64];
        sprintf(buf, "%s.%03d", UpdateTime, UpdateMillisec);
        return std::string(buf);
    }
};

#pragma pack(pop)
