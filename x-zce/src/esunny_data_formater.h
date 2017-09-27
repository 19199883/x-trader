/*
 * esunny_data_formater.h
 *
 *  Created on: 2013-8-13
 *      Author: oliver
 */

#ifndef ESUNNY_DATATYPEFORMATER_H_
#define ESUNNY_DATATYPEFORMATER_H_

#include <string>

#include "TapTradeAPIDataType.h"

// ESUNNY结构的格式化信息接口，仅有静态函数，无对象实例
class ESUNNYDatatypeFormater
{
public:
	// 所有请求消息的格式化
	static std::string ToString(const TapAPITradeLoginAuth *pdata);
	static std::string ToString(const TapAPICommodity *pdata);  // 查询系统中指定品种的合约信息
	static std::string ToString(const TapAPIAccQryReq *pdata);  // 查询用户下属的资金账号
	static std::string ToString(const TapAPIFundReq *pdata);    // 查询客户资金
	static std::string ToString(const TapAPINewOrder *pdata);
	static std::string ToString(const TapAPIOrderCancelReq *pdata);
	static std::string ToString(const TapAPIOrderQryReq *pdata);
	static std::string ToString(const TapAPIOrderProcessQryReq *pdata);
	// following structures are typedef of TapAPIOrderQryReq
	//static std::string ToString(const TapAPIFillQryReq *pdata);
	//static std::string ToString(const TapAPICloseQryReq *pdata);
	//static std::string ToString(const TapAPIPositionQryReq *pdata);

	// 所有响应消息的格式化
	static std::string ToString(const TapAPITradeLoginRspInfo *pdata);
	static std::string ToString(const TapAPIAccountInfo *pdata);
	static std::string ToString(const TapAPIFundData *pdata);
	static std::string ToString(const TapAPIExchangeInfo *pdata);
	static std::string ToString(const TapAPICommodityInfo *pdata);
	static std::string ToString(const TapAPITradeContractInfo *pdata);
	static std::string ToString(const TapAPIOrderInfoNotice *pdata);
	static std::string ToString(const TapAPIOrderActionRsp *pdata);
	static std::string ToString(const TapAPIOrderInfo *pdata);
	static std::string ToString(const TapAPIFillInfo *pdata);
	static std::string ToString(const TapAPIPositionInfo *pdata);
	static std::string ToString(const TapAPICloseInfo *pdata);
	static std::string ToString(const TapAPIPositionProfitNotice *pdata);
	static std::string ToString(const TapAPIExchangeStateInfo *pdata);
	static std::string ToString(const TapAPIExchangeStateInfoNotice *pdata);

	// deep quote data
    static std::string ToString(const TapAPIDeepQuoteQryRsp *pdata);

private:
	ESUNNYDatatypeFormater()
	{
	}
	~ESUNNYDatatypeFormater()
	{
	}
};

#endif /* ESUNNY_DATATYPEFORMATER_H_ */
