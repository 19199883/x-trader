#pragma once

#include "ThostFtdcMdApi.h"

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
			"PreDelta:%.4f; "
			"CurrDelta:%.4f; "
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
