#ifndef EES_DATATYPEFORMATER_H_
#define EES_DATATYPEFORMATER_H_

#include <string>

#include "EesTraderDefine.h"

class EESDatatypeFormater
{
public:
	// 所有请求消息的格式化
	static const char* ToString(const EES_TradeSvrInfo *pdata);
	static const char* ToString(const EES_EnterOrderField *pdata);
	static const char* ToString(const EES_OrderAcceptField *pdata);
	static const char* ToString(const EES_OrderRejectField *pdata);
	static const char* ToString(const EES_OrderMarketAcceptField *pdata);
	static const char* ToString(const EES_OrderMarketRejectField* pdata);
	static const char* ToString(const EES_OrderExecutionField* pdata);
	static const char* ToString(const EES_CancelOrder* pdata);
	static const char* ToString(const EES_OrderCxled *pdata);
	static const char* ToString(const EES_CxlOrderRej *pdata);

private:
	EESDatatypeFormater()
	{
	}
	~EESDatatypeFormater()
	{
	}

	char static buffer_[2048];
};

#endif /* EES_DATATYPEFORMATER_H_*/
