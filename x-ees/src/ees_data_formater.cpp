#include "ees_data_formater.h"
#include <iostream>
#include <sstream>

using namespace std;

static std::string indent_string = " ";
static std::string newline_string = " ";

std::string EESDatatypeFormater::ToString(const EES_TradeSvrInfo *pdata)
{
	stringstream ss;
	ss << "structName=EES_TradeSvrInfo" << ";";
	if (!pdata){
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << "m_remoteTradeIp=" << pdata->m_remoteTradeIp << ";";
	ss << "m_remoteTradeTCPPort=" << pdata->m_remoteTradeTCPPort  << ";";
	ss << "m_remoteTradeUDPPort=" << pdata->m_remoteTradeUDPPort << ";";
	ss << "m_remoteQueryIp=" << pdata->m_remoteQueryIp << ";";
	ss << "m_remoteQueryTCPPort=" << pdata->m_remoteQueryTCPPort<< ";";
	ss << "m_LocalTradeIp=" << pdata->m_LocalTradeIp << ";";
	ss << "m_LocalTradeUDPPort=" << pdata->m_LocalTradeUDPPort<< ";";

	return ss.str();
}

std::string EESDatatypeFormater::ToString(const EES_EnterOrderField *pdata)
{
    stringstream ss;
    ss << "structName=EES_EnterOrderField" << ";";
    if (!pdata) {
        ss << "<null>" << ";";
        return ss.str();
    }
    ss << "m_Account=" << pdata->m_Account << ";";
    ss << "m_Side=" << pdata->m_Side << ";";
    ss << "m_Exchange=" << pdata->m_Exchange << ";";
    ss << "m_Symbol=" << pdata->m_Symbol << ";";
    ss << "m_SecType=" << pdata->m_SecType << ";";
    ss << "m_Price=" << pdata->m_Price << ";";
    ss << "m_Qty=" << pdata->m_Qty << ";";
    ss << "m_OptExecFlag=" << pdata->m_OptExecFlag << ";";
    ss << "m_ClientOrderToken=" << pdata->m_ClientOrderToken << ";";
    ss << "m_Tif=" << pdata->m_Tif << ";";
    ss << "m_MinQty=" << pdata->m_MinQty << ";";
    ss << "m_CustomField=" << pdata->m_CustomField << ";";
    ss << "m_MarketSessionId=" << pdata->m_MarketSessionId << ";";
    ss << "m_HedgeFlag=" << pdata->m_HedgeFlag << ";";

    return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const EES_OrderAcceptField *pdata)
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

std::string FEMASDatatypeFormater::ToString(const EES_OrderRejectField *pdata)
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

std::string FEMASDatatypeFormater::ToString(const EES_OrderMarketAcceptField *pdata)
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

std::string FEMASDatatypeFormater::ToString(const EES_OrderMarketRejectField *pdata)
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
	ss << "m_Userid=" << pdata->m_Userid << ";";
	ss << "m_ClientOrderToken=" << pdata->m_ClientOrderToken << ";";

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const EES_OrderExecutionField *pdata)
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

std::string FEMASDatatypeFormater::ToString(const EES_CancelOrder* pdata)
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

std::string FEMASDatatypeFormater::ToString(const EES_OrderCxled* pdata)
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

std::string FEMASDatatypeFormater::ToString(const EES_CxlOrderRej* pdata)
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

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcInputOrderField* pdata)
{
	stringstream ss;
	ss << "structName=CUstpFtdcInputOrderField	///Input Order";
	if (!pdata)
	{
		ss << "<null>";
		return ss.str();
	}
	ss << "BrokerID=" << pdata->BrokerID << ";";
	ss << "ExchangeID=" << pdata->ExchangeID << ";";
	ss << "OrderSysID=" << pdata->OrderSysID << ";";
	ss << "InvestorID=" << pdata->InvestorID << ";";
	ss << "UserID=" << pdata->UserID << ";";
	ss << "InstrumentID=" << pdata->InstrumentID << ";";
	ss << "UserOrderLocalID=" << pdata->UserOrderLocalID << ";";
	ss << "OrderPriceType=" << pdata->OrderPriceType << ";";
	ss << "Direction=" << pdata->Direction << ";";
	ss << "OffsetFlag=" << pdata->OffsetFlag << ";";
	ss << "HedgeFlag=" << pdata->HedgeFlag << ";";
	ss << "LimitPrice=" << pdata->LimitPrice << ";";
	ss << "Volume=" << pdata->Volume << ";";
	ss << "TimeCondition=" << pdata->TimeCondition << ";";
	ss << "GTDDate=" << pdata->GTDDate << ";";
	ss << "VolumeCondition=" << pdata->VolumeCondition << ";";
	ss << "MinVolume=" << pdata->MinVolume << ";";
	ss << "StopPrice=" << pdata->StopPrice << ";";
	ss << "ForceCloseReason=" << pdata->ForceCloseReason << ";";
	ss << "IsAutoSuspend=" << pdata->IsAutoSuspend  << ";";
	ss << "BusinessUnit=" << pdata->BusinessUnit  << ";";
	ss << "UserCustom=" << pdata->UserCustom  << ";";

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcOrderActionField* pdata)
{
	stringstream ss;
	ss << "structName=CUstpFtdcOrderActionField	///Order Action" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << "ExchangeID=" << pdata->ExchangeID << ";";
	ss << "OrderSysID=" << pdata->OrderSysID << ";";
	ss << "BrokerID=" << pdata->BrokerID << ";";
	ss << "InvestorID=" << pdata->InvestorID << ";";
	ss << "UserID=" << pdata->UserID << ";";
	ss << "UserOrderActionLocalID=" << pdata->UserOrderActionLocalID << ";";
	ss << "UserOrderLocalID=" << pdata->UserOrderLocalID << ";";
	ss << "ActionFlag=" << pdata->ActionFlag << ";";
	ss << "LimitPrice=" << pdata->LimitPrice << ";";
	ss << "VolumeChange=" << pdata->VolumeChange << ";";

	return ss.str();
}


std::string FEMASDatatypeFormater::ToString(const CUstpFtdcOrderField* pdata)
{
	stringstream ss;
	ss << "structName=CUstpFtdcOrderField" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}
	ss << "BrokerID=" << pdata->BrokerID << ";";
	ss << "ExchangeID=" << pdata->ExchangeID << ";";
	ss << "OrderSysID=" << pdata->OrderSysID << ";";
	ss << "InvestorID=" << pdata->InvestorID << ";";
	ss << "UserID=" << pdata->UserID << ";";
	ss << "InstrumentID=" << pdata->InstrumentID << ";";
	ss << "UserOrderLocalID=" << pdata->UserOrderLocalID << ";";
	ss << "OrderPriceType=" << pdata->OrderPriceType << ";";
	ss << "Direction=" << pdata->Direction << ";";
	ss << "OffsetFlag=" << pdata->OffsetFlag << ";";
	ss << "HedgeFlag=" << pdata->HedgeFlag << ";";
	ss << "LimitPrice=" << pdata->LimitPrice << ";";
	ss << "Volume=" << pdata->Volume << ";";
	ss << "TimeCondition=" << pdata->TimeCondition << ";";
	ss << "GTDDate=" << pdata->GTDDate << ";";
	ss << "VolumeCondition=" << pdata->VolumeCondition << ";";
	ss << "MinVolume=" << pdata->MinVolume << ";";
	ss << "StopPrice=" << pdata->StopPrice << ";";
	ss << "ForceCloseReason=" << pdata->ForceCloseReason << ";";
	ss << "IsAutoSuspend=" << pdata->IsAutoSuspend << ";";
	ss << "BusinessUnit=" << pdata->BusinessUnit << ";";
	ss << "UserCustom=" << pdata->UserCustom << ";";
	ss << "TradingDay=" << pdata->TradingDay << ";";
	ss << "ParticipantID=" << pdata->ParticipantID << ";";
	ss << "ClientID=" << pdata->ClientID  << ";";
	ss << "SeatID=" << pdata->SeatID << ";";
	ss << "InsertTime=" << pdata->InsertTime << ";";
	ss << "OrderLocalID=" << pdata->OrderLocalID << ";";

	ss << "OrderSource=";
	pdata->OrderSource == '\0' ? ss << "<null>" : ss << pdata->OrderSource << ";";

	ss << "OrderStatus=";
	pdata->OrderStatus == '\0' ? ss << "<null>" : ss << pdata->OrderStatus << ";";

	ss << "CancelTime=" << pdata->CancelTime << ";";
	ss << "CancelUserID=" << pdata->CancelUserID << ";";
	ss << "VolumeTraded=" << pdata->VolumeTraded << ";";
	ss << "VolumeRemain=" << pdata->VolumeRemain << ";";

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcTradeField* pdata)
{
	stringstream ss;
	ss << "structName=CUstpFtdcTradeField" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << "BrokerID=" << pdata->BrokerID << ";";
	ss << "ExchangeID=" << pdata->ExchangeID << ";";
	ss << "TradingDay=" << pdata->TradingDay << ";";
	ss << "ParticipantID=" << pdata->ParticipantID << ";";
	ss << "SeatID=" << pdata->SeatID << ";";
	ss << "InvestorID=" << pdata->InvestorID << ";";
	ss << "ClientID=" << pdata->ClientID << ";";
	ss << "UserID=" << pdata->UserID << ";";
	ss << "TradeID=" << pdata->TradeID << ";";
	ss << "OrderSysID=" << pdata->OrderSysID << ";";
	ss << "UserOrderLocalID=" << pdata->UserOrderLocalID << ";";
	ss << "InstrumentID=" << pdata->InstrumentID << ";";
	ss << "Direction=" << pdata->Direction << ";";
	ss << "OffsetFlag=" << pdata->OffsetFlag << ";";
	ss << "HedgeFlag=" << pdata->HedgeFlag << ";";
	ss << "TradePrice=" << pdata->TradePrice << ";";
	ss << "TradeVolume=" << pdata->TradeVolume << ";";
	ss << "TradeTime=" << pdata->TradeTime << ";";
	ss << "ClearingPartID=" << pdata->ClearingPartID << ";";

	return ss.str();
}



std::string FEMASDatatypeFormater::ToString(const CUstpFtdcRspInvestorPositionField* pdata)
{
	stringstream ss;
	ss << "structName=CUstpFtdcRspInvestorPositionField" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << "InvestorID=" << pdata->InvestorID << ";";
	ss << "BrokerID=" << pdata->BrokerID << ";";
	ss << "ExchangeID=" << pdata->ExchangeID << ";";
	ss << "ClientID=" << pdata->ClientID << ";";
	ss << "InstrumentID=" << pdata->InstrumentID << ";";
	ss << "Direction=" << pdata->Direction << ";";
	ss << "HedgeFlag=" << pdata->HedgeFlag << ";";
	ss << "UsedMargin=" << pdata->UsedMargin << ";";
	ss << "Position=" << pdata->Position << ";";
	ss << "PositionCost=" << pdata->PositionCost << ";";
	ss << "YdPosition=" << pdata->YdPosition << ";";
	ss << "YdPositionCost=" << pdata->YdPositionCost << ";";
	ss << "FrozenMargin=" << pdata->FrozenMargin << ";";
	ss << "FrozenPosition=" << pdata->FrozenPosition << ";";
	ss << "FrozenClosing=" << pdata->FrozenClosing << ";";
	ss << "FrozenPremium=" << pdata->FrozenPremium << ";";
	ss << "LastTradeID=" << pdata->LastTradeID << ";";
	ss << "LastOrderLocalID=" << pdata->LastOrderLocalID << ";";
	ss << "Currency=" << pdata->Currency << ";";

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcRspInfoField* pdata)
{
	stringstream ss;
	ss << "structName=CUstpFtdcRspInfoField" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << "ErrorID=" << pdata->ErrorID << ";";
	ss << "ErrorMsg=" << pdata->ErrorMsg << ";";

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcRspInvestorAccountField *pdata)
{
	stringstream ss;
	ss << std::fixed <<  "structName=CUstpFtdcRspInvestorAccountField" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}
	ss << "BrokerID=" << pdata->BrokerID << ";";
	ss << "InvestorID=" << pdata->InvestorID << ";";
	ss << "AccountID=" << pdata->AccountID << ";";
	ss << "PreBalance=" << pdata->PreBalance << ";";
	ss << "Deposit=" << pdata->Deposit << ";";
	ss << "Withdraw=" << pdata->Withdraw << ";";
	ss << "FrozenMargin=" << pdata->FrozenMargin << ";";
	ss << "FrozenFee=" << pdata->FrozenFee << ";";
	ss << "FrozenPremium=" << pdata->FrozenPremium << ";";
	ss << "Fee=" << pdata->Fee << ";";
	ss << "CloseProfit=" << pdata->CloseProfit << ";";
	ss << "PositionProfit=" << pdata->PositionProfit << ";";
	ss << "Available=" << pdata->Available << ";";
	ss << "LongFrozenMargin=" << pdata->LongFrozenMargin << ";";
	ss << "ShortFrozenMargin=" << pdata->ShortFrozenMargin << ";";
	ss << "LongMargin=" << pdata->LongMargin << ";";
	ss << "ShortMargin=" << pdata->ShortMargin << ";";
	ss << "ReleaseMargin=" << pdata->ReleaseMargin << ";";
	ss << "DynamicRights=" << pdata->DynamicRights << ";";
	ss << "TodayInOut=" << pdata->TodayInOut << ";";
	ss << "Margin=" << pdata->Margin << ";";
	ss << "Premium=" << pdata->Premium << ";";
	ss << "Risk=" << pdata->Risk << ";";

	return ss.str();
}


