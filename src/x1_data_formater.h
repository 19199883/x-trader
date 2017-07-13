//  TODO: wangying x1 status: done

#ifndef X1_DATATYPEFORMATER_H_
#define X1_DATATYPEFORMATER_H_

#include <string>

#include "X1FtdcApiDataType.h"
#include "X1FtdcApiStruct.h"

// X1结构的格式化信息接口，仅有静态函数，无对象实例
class X1DatatypeFormater
{
public:
	// 所有请求消息的格式化
    static std::string ToString(const CX1FtdcReqUserLoginField *pdata);
	static std::string ToString(const CX1FtdcInsertOrderField *pdata);
	static std::string ToString(const CX1FtdcCancelOrderField *pdata);
	static std::string ToString(const CX1FtdcQryPositionField *pdata);
	static std::string ToString(const CX1FtdcQryPositionDetailField *pdata);
	static std::string ToString(const CX1FtdcQryOrderField  *pdata);
	static std::string ToString(const CX1FtdcQryMatchField  *pdata);
	static std::string ToString(const CX1FtdcQuoteInsertField *pdata);
	static std::string ToString(const CX1FtdcQryExchangeInstrumentField *pdata);

	// 所有响应消息的格式化
	static std::string ToString(const CX1FtdcRspErrorField *pdata);
	static std::string ToString(const CX1FtdcRspUserLoginField *pdata);
	static std::string ToString(const CX1FtdcRspUserLogoutInfoField *pdata);
	static std::string ToString(const CX1FtdcRspOperOrderField *pdata);
	static std::string ToString(const CX1FtdcRspPriOrderField *pdata);
	static std::string ToString(const CX1FtdcRspPriMatchInfoField *pdata);
	static std::string ToString(const CX1FtdcRspPriCancelOrderField *pdata);
	static std::string ToString(const CX1FtdcRspOrderField *pdata);
	static std::string ToString(const CX1FtdcRspMatchField *pdata);
	static std::string ToString(const CX1FtdcRspPositionField *pdata);
	static std::string ToString(const CX1FtdcRspPositionDetailField *pdata);
	static std::string ToString(const CX1FtdcRspExchangeInstrumentField *pdata);
	static std::string ToString(const CX1FtdcRspExchangeStatusField *pdata);
	static	std::string ToString(const CX1FtdcExchangeStatusRtnField* p);

    static std::string ToString(const CX1FtdcRspCapitalField *pdata);

private:
	X1DatatypeFormater()
	{
	}
	~X1DatatypeFormater()
	{
	}
};

#endif /* X1_DATATYPEFORMATER_H_ */
