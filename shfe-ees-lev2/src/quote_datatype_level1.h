#pragma once

#include <chrono>
#include <ctime>
#include <ratio>
#include <ctime>


using namespace std::chrono;
#include "ThostFtdcMdApi.h"
#include "quote_cmn_utility.h"

#ifndef DLL_PUBLIC
#define DLL_PUBLIC  __attribute__ ((visibility("default")))
#endif

#include <string>

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
			"InstrumentID:%s; "
			"UpdateTime[9]:%s; "
			"UpdateMillisec:%d; "
			"timestamp:%lld"
			"TradingDay:%s; "
			"LastPrice:%.4f; "
			"PreSettlementPrice:%.4f; "
			"PreClosePrice:%.4f; "
			"PreOpenInterest:%.4f; "
			"OpenPrice:%.4f; "
			"HighestPrice:%.4f; "
			"LowestPrice:%.4f; "
			"Volume:%d; "
			"Turnover:%.4f; "
			"OpenInterest:%.4f; "
			"ClosePrice:%.4f; "
			"SettlementPrice:%.4f; "
			"UpperLimitPrice:%.4f; "
			"LowerLimitPrice:%.4f; "
			//"PreDelta:%.4f; "
			//"CurrDelta:%.4f; "
			"BidPrice1:%.4f; "
			"BidVolume1:%d; "
			"AskPrice1:%.4f; "
			"AskVolume1:%d; "
			"BidPrice2:%.4f; "
			"BidVolume2:%d; "
			"AskPrice2:%.4f; "
			"AskVolume2:%d; "
			"BidPrice3:%.4f; "
			"BidVolume3:%d; "
			"AskPrice3:%.4f; "
			"AskVolume3:%d; "
			"BidPrice4:%.4f; "
			"BidVolume4:%d; "
			"AskPrice4:%.4f; "
			"AskVolume4:%d; "
			"BidPrice5:%.4f; "
			"BidVolume5:%d; "
			"AskPrice5:%.4f; "
			"AskVolume5:%d; "
			"ActionDay:%s;",
			source.InstrumentID,
			source.UpdateTime,
			source.UpdateMillisec,
			(int64_t)high_resolution_clock::now().time_since_epoch().count(),
			source.TradingDay,
			InvalidToZeroD(source.LastPrice),
			InvalidToZeroD(source.PreSettlementPrice),
			InvalidToZeroD(source. PreClosePrice),
			InvalidToZeroD(source.PreOpenInterest),
			InvalidToZeroD(source.OpenPrice),
			InvalidToZeroD(source. HighestPrice),
			InvalidToZeroD(source. LowestPrice),
			source.Volume,
			InvalidToZeroD(source.Turnover),
			InvalidToZeroD(source.OpenInterest),
			source.ClosePrice,
			source.SettlementPrice,
			InvalidToZeroD(source.UpperLimitPrice),
			InvalidToZeroD(source.LowerLimitPrice),
			//InvalidToZeroD(source.PreDelta),
			//InvalidToZeroD(source.CurrDelta),
			InvalidToZeroD(source.BidPrice1),
			source.BidVolume1,
			InvalidToZeroD(source.AskPrice1),
			source.AskVolume1,
			InvalidToZeroD(source.BidPrice2),
			source.BidVolume2,
			InvalidToZeroD(source.AskPrice2),
			source.AskVolume2,
			InvalidToZeroD(source.BidPrice3),
			source.BidVolume3,
			InvalidToZeroD(source.AskPrice3),
			source.AskVolume3,
			InvalidToZeroD(source.BidPrice4),
			source.BidVolume4,
			InvalidToZeroD(source.AskPrice4),
			source.AskVolume4,
			InvalidToZeroD(source.BidPrice5),
			source.BidVolume5,
			InvalidToZeroD(source.AskPrice5),
			source.AskVolume5,
			source.ActionDay);

		return dest;
	}
};
