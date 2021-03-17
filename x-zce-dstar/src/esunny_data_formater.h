/*
 * esunny_data_formater.h
 *
 *  Created on: 2013-8-13
 *      Author: oliver
 */

#ifndef ESUNNY_DATATYPEFORMATER_H_
#define ESUNNY_DATATYPEFORMATER_H_

#include <string>
#include "DstarTradeApiDataType.h"
#include "DstarTradeApiStruct.h"

// ESUNNY结构的格式化信息接口，仅有静态函数，无对象实例
class ESUNNYDatatypeFormater
{
public:
	// 所有响应消息的格式化
	static std::string ToString(const DstarApiRspLoginField *pLoginRsp);
	static std::string ToString(const DstarApiRspUdpAuthField *p);
	static std::string ToString(const DstarApiContractField *p);
	static std::string ToString(const DstarApiSeatField *p);
	static std::string ToString(const DstarApiRspOrderInsertField *p);
	static std::string ToString(const DstarApiOrderField *p);
	static std::string ToString(const DstarApiReqOfferInsertField *p);
	static std::string ToString(const DstarApiReqOrderDeleteField *p);
	static std::string ToString(const DstarApiReqUdpAuthField *p);

private:
	ESUNNYDatatypeFormater() { }
	~ESUNNYDatatypeFormater() { }
};

#endif /* ESUNNY_DATATYPEFORMATER_H_ */
