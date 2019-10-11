#include "ees_data_formater.h"
#include <iostream>
#include <sstream>

using namespace std;

char EESDatatypeFormater::buffer_[2048];

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

const char* EESDatatypeFormater::ToString(const EES_OrderAcceptField *pdata)
{
	if (!pdata) return "";

	sprintf(buffer_,
		"structName=EES_OrderAcceptField; "
		"m_ClientOrderToken=%u; "
		"m_MarketOrderToken=%lld; "
		"m_OrderState=%hhu; "
		"m_UserID=%d; "
		"m_Account=%s; "
		"m_Side=%hhu; "
		"m_Exchange=%hhu; "
		"m_Symbol=%s; "
		"m_SecType=%hhu; "
		"m_Price=%f; "
		"m_Qty=%u; "
		"m_OptExecFlag=%hhu; "
		"m_Tif=%u; "
		"m_MinQty=%u; "
		"m_MarketSessionId=%hhu; "
		"m_HedgeFlag=%hhu; ",
		pdata->m_ClientOrderToken,
		pdata->m_MarketOrderToken,
		pdata->m_OrderState,
		pdata->m_UserID,
		pdata->m_Account,
		pdata->m_Side,
		pdata->m_Exchange,
		pdata->m_Symbol,
		pdata->m_SecType,
		pdata->m_Price,
		pdata->m_Qty,
		pdata->m_OptExecFlag,
		pdata->m_Tif,
		pdata->m_MinQty,
		pdata->m_MarketSessionId,
		pdata->m_HedgeFlag);

	return buffer_;
}

const char* EESDatatypeFormater::ToString(const EES_OrderRejectField *pdata)
{
	if (!pdata) return "";

	sprintf(buffer_,
		"structName=EES_OrderRejectField; "
		"m_Userid=%d; "
		"m_ClientOrderToken=%u; "
		"m_RejectedMan=%hhu; "
		"m_ReasonCode=%hhu; "
		"m_GrammerResult=%s; "
		"m_RiskResult=%s; "
		"m_GrammerText=%s; "
		"m_RiskText=%s; ",
		pdata->m_Userid,
		pdata->m_ClientOrderToken,
		pdata->m_RejectedMan,
		pdata->m_ReasonCode,
		pdata->m_GrammerResult,
		pdata->m_RiskResult,
		pdata->m_GrammerText,
		pdata->m_RiskText);

	return buffer_;
}

const char * EESDatatypeFormater::ToString(const EES_OrderMarketAcceptField *pdata)
{
	if (!pdata) return "";

	sprintf(buffer_,
		"structName=EES_OrderMarketAcceptField; "
		"m_Account=%s; "
		"m_MarketOrderToken=%lld"
		"m_MarketOrderId=%s; "
		"m_UserID=%d; "
		"m_ClientOrderToken=%u; ",
		pdata->m_Account,
		pdata->m_MarketOrderToken,
		pdata->m_MarketOrderId,
		pdata->m_UserID,
		pdata->m_ClientOrderToken);

	return buffer_;
}

const char* EESDatatypeFormater::ToString(const EES_OrderMarketRejectField *pdata)
{
	if (!pdata) return "";

	sprintf(buffer_,
		"structName=EES_OrderRejectField; "
		"m_Account=%s; " 
		"m_MarketOrderToken=%lld"
		"m_ReasonText=%s; " 
		"m_Userid=%d; "  
		"m_ClientOrderToken=%u; ",
		pdata->m_Account,
		pdata->m_MarketOrderToken,
		pdata->m_ReasonText,
		pdata->m_UserID,
		pdata->m_ClientOrderToken);

	return buffer_;
}

const char* EESDatatypeFormater::ToString(const EES_OrderExecutionField *pdata)
{
	if (!pdata) return "";

	sprintf(buffer_,
		"structName=EES_OrderExecutionField; "
		"m_Userid=%d; "
		"m_ClientOrderToken=%u; "
		"m_MarketOrderToken=%lld"
		"m_Quantity=%u; "
		"m_Price=%f; "
		"m_ExecutionID=%lld; "
		"m_MarketExecID=%c; ",
		 pdata->m_Userid,
		 pdata->m_ClientOrderToken,
		 pdata->m_MarketOrderToken,
		 pdata->m_Quantity,
		 pdata->m_Price,
		 pdata->m_ExecutionID,
		 pdata->m_MarketExecID);

	return buffer_;
}

const char* EESDatatypeFormater::ToString(const EES_CancelOrder* pdata)
{
	if (!pdata) return "";

	sprintf(buffer_,
		"structName=EES_CancelOrder; "
		"m_MarketOrderToken=%lld; "
		"m_Quantity=%u; "
		"m_Account=%s; ",
		pdata->m_MarketOrderToken,
		pdata->m_Quantity,
		pdata->m_Account);

	return buffer_;
}

const char* EESDatatypeFormater::ToString(const EES_OrderCxled* pdata)
{
    if (!pdata) return "";
    
	sprintf(buffer_,
		"structName=EES_OrderCxled; "
		"m_Userid=%d; "
		"m_ClientOrderToken=%u; "
		"m_MarketOrderToken=%lld; "
		"m_Decrement=%u; "
		"m_Reason=%hhu; ",
		pdata->m_Userid,
		pdata->m_ClientOrderToken,
		pdata->m_MarketOrderToken,
		pdata->m_Decrement,
		pdata->m_Reason);
		

    return buffer_;
}

const char* EESDatatypeFormater::ToString(const EES_CxlOrderRej* pdata)
{
	if (!pdata) return "";

	sprintf(buffer_,
		"structName=EES_CxlOrderRej; "
		"m_account=%s; "
		"m_MarketOrderToken=%lld; " 
		"m_ReasonCode=%u; "
		"m_ReasonText=%s; ",
		pdata->m_account,
		pdata->m_MarketOrderToken,
		pdata->m_ReasonCode,
		pdata->m_ReasonText);

	return buffer_;
}

