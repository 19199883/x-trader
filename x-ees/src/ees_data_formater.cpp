#include "ees_data_formater.h"
#include <iostream>
#include <sstream>

using namespace std;

static std::string indent_string = " ";
static std::string newline_string = " ";

const char* EESDatatypeFormater::ToString(const EES_TradeSvrInfo *pdata)
{
	if (!pdata) return "";

	sprintf(buffer_,
		"structName=EES_TradeSvrInfo;"
		"m_remoteTradeIp=%s; "
		"m_remoteTradeTCPPort=%u; "
		"m_remoteTradeUDPPort=%u; "
		"m_remoteQueryIp=%s; "
		"m_remoteQueryTCPPort=%u; "
		"m_LocalTradeIp=%s; "
		"m_LocalTradeUDPPort=%u; ",
		pdata->m_remoteTradeIp,
		pdata->m_remoteTradeTCPPort ,
		pdata->m_remoteTradeUDPPort,
		pdata->m_remoteQueryIp,
		pdata->m_remoteQueryTCPPort,
		pdata->m_LocalTradeIp,
		pdata->m_LocalTradeUDPPort);

	return buffer_;
}

const char* EESDatatypeFormater::ToString(const EES_EnterOrderField *pdata)
{
    if (!pdata) return "";

	sprintf(buffer_,
		"structName=EES_EnterOrderField; "
		"m_Account=%s; "
		"m_Side=%hhu; "
		"m_Exchange=%hhu; "
		"m_Symbol=%s; "
		"m_SecType=%hhu; "
		"m_Price=%f; "
		"m_Qty=%u; "
		"m_OptExecFlag=%hhu; "
		"m_ClientOrderToken=%u; "
		"m_Tif=%u; "
		"m_MinQty=%u; "
		"m_MarketSessionId=%u; "
		"m_HedgeFlag=%hhu; ",
		pdata->m_Account,
		pdata->m_Side,
		pdata->m_Exchange,
		pdata->m_Symbol,
		pdata->m_SecType ,
		pdata->m_Price,
		pdata->m_Qty,
		pdata->m_OptExecFlag,
		pdata->m_ClientOrderToken,
		pdata->m_Tif,
		pdata->m_MinQty,
		pdata->m_MarketSessionId,
		pdata->m_HedgeFlag);

    return buffer_;
}

std::string EESDatatypeFormater::ToString(const EES_OrderAcceptField *pdata)
{
	stringstream ss;
	ss << "structName=EES_OrderAcceptField" << ";";
	if (!pdata) {
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << "m_ClientOrderToken=" << pdata->m_ClientOrderToken  << ";";
	ss << "m_MarketOrderToken=" << pdata->m_MarketOrderToken << ";";
	ss << "m_OrderState=" << pdata->m_OrderState << ";";
	ss << "m_UserID=" << pdata->m_UserID << ";";
	ss << "m_AcceptTime=" << pdata->m_AcceptTime << ";";
	ss << "m_Account=" << pdata->m_Account << ";";
	ss << "m_Side=" << pdata->m_Side << ";";
	ss << "m_Exchange=" << pdata->m_Exchange << ";";
	ss << "m_Symbol=" << pdata->m_Symbol << ";";
	ss << "m_SecType=" << pdata->m_SecType << ";";
	ss << "m_Price=" << pdata->m_Price << ";";
	ss << "m_Qty=" << pdata->m_Qty << ";";
	ss << "m_OptExecFlag=" << pdata->m_OptExecFlag << ";";
	ss << "m_Tif=" << pdata->m_Tif << ";";
	ss << "m_MinQty=" << pdata->m_MinQty << ";";
	ss << "m_CustomField=" << pdata->m_CustomField << ";";
	ss << "m_MarketSessionId=" << pdata->m_MarketSessionId << ";";
	ss << "m_HedgeFlag=" << pdata->m_HedgeFlag << ";";

	return ss.str();
}

std::string EESDatatypeFormater::ToString(const EES_OrderRejectField *pdata)
{
	stringstream ss;
	ss << "structName=EES_OrderRejectField" << ";";
	if (!pdata) {
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << "m_Userid=" << pdata->m_Userid << ";";
	ss << "m_Timestamp=" << pdata->m_Timestamp << ";";
	ss << "m_ClientOrderToken=" << pdata->m_ClientOrderToken << ";";
	ss << "m_RejectedMan=" << pdata->m_RejectedMan << ";";
	ss << "m_ReasonCode=" << pdata->m_ReasonCode << ";";
	ss << "m_GrammerResult=" << pdata->m_GrammerResult << ";";
	ss << "m_RiskResult=" << pdata->m_RiskResult << ";";
	ss << "m_GrammerText=" << pdata->m_GrammerText << ";";
	ss << "m_RiskText=" << pdata->m_RiskText << ";";

	return ss.str();
}

std::string EESDatatypeFormater::ToString(const EES_OrderMarketAcceptField *pdata)
{
	stringstream ss;
	ss << "structName=EES_OrderMarketAcceptField" << ";";
	if (!pdata) {
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << "m_Account=" << pdata->m_Account << ";";
	ss << "m_MarketOrderToken=" << pdata->m_MarketOrderToken << ";";
	ss << "m_MarketOrderId=" << pdata->m_MarketOrderId << ";";
	ss << "m_MarketTime=" << pdata->m_MarketTime << ";";
	ss << "m_UserID=" << pdata->m_UserID << ";";
	ss << "m_ClientOrderToken=" << pdata->m_ClientOrderToken  << ";";

	return ss.str();
}

std::string EESDatatypeFormater::ToString(const EES_OrderMarketRejectField *pdata)
{
	stringstream ss;
	ss << "structName=EES_OrderRejectField" << ";";
	if (!pdata) {
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << "m_Account=" << pdata->m_Account << ";";
	ss << "m_MarketOrderToken=" << pdata->m_MarketOrderToken << ";";
	ss << "m_MarketTimestamp=" << pdata->m_MarketTimestamp << ";";
	ss << "m_ReasonText=" << pdata->m_ReasonText << ";";
	ss << "m_Userid=" << pdata->m_UserID << ";";
	ss << "m_ClientOrderToken=" << pdata->m_ClientOrderToken << ";";

	return ss.str();
}

std::string EESDatatypeFormater::ToString(const EES_OrderExecutionField *pdata)
{
	stringstream ss;
	ss << "structName=EES_OrderExecutionField" << ";";
	if (!pdata) {
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << "m_Userid=" << pdata->m_Userid << ";";
	ss << "m_Timestamp=" << pdata->m_Timestamp  << ";";
	ss << "m_ClientOrderToken=" << pdata->m_ClientOrderToken << ";";
	ss << "m_MarketOrderToken=" << pdata->m_MarketOrderToken << ";";
	ss << "m_Quantity=" << pdata->m_Quantity << ";";
	ss << "m_Price=" << pdata->m_Price << ";";
	ss << "m_ExecutionID=" << pdata->m_ExecutionID << ";";
	ss << "m_MarketExecID=" << pdata->m_MarketExecID << ";";

	return ss.str();
}

std::string EESDatatypeFormater::ToString(const EES_CancelOrder* pdata)
{
	stringstream ss;
	ss << "structName=EES_CancelOrder" << ";";
	if (!pdata) {
		ss << "<null>" << ";";
		return ss.str();
	}
	ss << "m_MarketOrderToken=" << pdata->m_MarketOrderToken << ";";
	ss << "m_Quantity=" << pdata->m_Quantity << ";";
	ss << "m_Account=" << pdata->m_Account  << ";";

	return ss.str();
}

std::string EESDatatypeFormater::ToString(const EES_OrderCxled* pdata)
{
    stringstream ss;
    ss << "structName=EES_OrderCxled" << ";";
    if (!pdata) {
        ss << "<null>" << ";";
        return ss.str();
    }
    ss << "m_Userid=" << pdata->m_Userid << ";";
    ss << "m_Timestamp=" << pdata->m_Timestamp  << ";";
    ss << "m_ClientOrderToken=" << pdata->m_ClientOrderToken  << ";";
    ss << "m_MarketOrderToken=" << pdata->m_MarketOrderToken  << ";";
    ss << "m_Decrement=" << pdata->m_Decrement  << ";";
    ss << "m_Reason=" << pdata->m_Reason  << ";";

    return ss.str();
}

std::string EESDatatypeFormater::ToString(const EES_CxlOrderRej* pdata)
{
	stringstream ss;
	ss <<  "structName=EES_CxlOrderRej" << ";";
	if (!pdata) {
		ss << "<null>" << ";";
		return ss.str();
	}
	ss << "m_account=" << pdata->m_account  << ";";
	ss << "m_MarketOrderToken=" << pdata->m_MarketOrderToken << ";";
	ss << "m_ReasonCode=" << pdata->m_ReasonCode  << ";";
	ss << "m_ReasonText=" << pdata->m_ReasonText << ";";

	return ss.str();
}

