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
	static std::string ToString(const TapAPINewOrder *pdata);
	static std::string ToString(const TapAPIOrderCancelReq *pdata);

	// 所有响应消息的格式化
	static std::string ToString(const DstarApiRspLoginField *pLoginRsp);
	static std::string ToString(const DstarApiRspUdpAuthField *p);
	static std::string ToString(const DstarApiContractField *p);
	static std::string ToString(const DstarApiSeatField *p);
	static std::string ToString(const DstarApiRspOrderInsertField *p);
	static std::string ToString(const DstarApiOrderField *p);
	static std::string ToString(const DstarApiReqOfferInsertField *p);
	static std::string ToString(const DstarApiReqOrderDeleteField *p);

private:
	ESUNNYDatatypeFormater() { }
	~ESUNNYDatatypeFormater() { }
};

#endif /* ESUNNY_DATATYPEFORMATER_H_ */
