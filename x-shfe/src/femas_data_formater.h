#ifndef FEMAS_DATATYPEFORMATER_H_
#define FEMAS_DATATYPEFORMATER_H_

#include <string>

#include "USTPFtdcUserApiDataType.h"
#include "USTPFtdcUserApiStruct.h"

// CTP结构的格式化信息接口，仅有静态函数，无对象实例
class FEMASDatatypeFormater
{
public:
	// 所有请求消息的格式化
	static std::string ToString(const CUstpFtdcReqUserLoginField *pdata);
	static std::string ToString(const CUstpFtdcReqUserLogoutField *pdata);
	static std::string ToString(const CUstpFtdcQryOrderField *pdata);
	static std::string ToString(const CUstpFtdcQryInvestorPositionField *pdata);
	static std::string ToString(const CUstpFtdcQryInvestorAccountField *pdata);

	static std::string ToString(const CUstpFtdcRspUserLoginField* pdata);
	static std::string ToString(const CUstpFtdcRspUserLogoutField* pdata);
	static std::string ToString(const CUstpFtdcUserPasswordUpdateField* pdata);
	static std::string ToString(const CUstpFtdcInputOrderField *pdata);
	static std::string ToString(const CUstpFtdcOrderActionField *pdata);
	static std::string ToString(const CUstpFtdcTradeField *pdata);
	static std::string ToString(const CUstpFtdcOrderField *pdata);
	static std::string ToString(const CUstpFtdcRspInvestorPositionField *pdata);
	static std::string ToString(const CUstpFtdcRspInfoField *pdata);
	static std::string ToString(const CUstpFtdcRspInvestorAccountField *pdata);

private:
	FEMASDatatypeFormater()
	{
	}
	~FEMASDatatypeFormater()
	{
	}
};

#endif /* CTP_DATATYPEFORMATER_H_ */
