#ifndef EES_DATATYPEFORMATER_H_
#define EES_DATATYPEFORMATER_H_

#include <string>

#include "EesTraderDefine.h"

class EESDatatypeFormater
{
public:
	// 所有请求消息的格式化
	static std::string ToString(const EES_TradeSvrInfo *pdata);
	static std::string ToString(const EES_EnterOrderField *pdata);
	static std::string ToString(const EES_OrderAcceptField *pdata);
	static std::string ToString(const EES_OrderRejectField *pdata);
	static std::string ToString(const EES_OrderMarketAcceptField *pdata);
	static std::string ToString(const EES_OrderMarketRejectField* pdata);
	static std::string ToString(const EES_OrderExecutionField* pdata);
	static std::string ToString(const EES_CancelOrder* pdata);
	static std::string ToString(const EES_OrderCxled *pdata);
	static std::string ToString(const EES_CxlOrderRej *pdata);

private:
	EESDatatypeFormater()
	{
	}
	~EESDatatypeFormater()
	{
	}
};

#endif /* EES_DATATYPEFORMATER_H_*/
