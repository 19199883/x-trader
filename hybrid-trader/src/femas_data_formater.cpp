#include "femas_data_formater.h"
#include <iostream>
#include <sstream>

using namespace std;

static std::string indent_string = " ";
static std::string newline_string = " ";

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcReqUserLoginField *pdata)
{
	stringstream ss;
	ss << "structName=CUstpFtdcReqUserLoginFieldi ///Login Request" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << "TradingDay=" << pdata->TradingDay << ";";
	ss << "UserID=" << pdata->UserID  << ";";
	ss << "BrokerID=" << pdata->BrokerID << ";";
	ss << "UserProductInfo=" << pdata->UserProductInfo << ";";
	ss << "InterfaceProductInfo=" << pdata->InterfaceProductInfo<< ";";
	ss << "ProtocolInfo=" << pdata->ProtocolInfo << ";";
	ss << "IPAddress=" << pdata->IPAddress<< ";";
	ss << "MacAddress=" << pdata->MacAddress  << ";";
	ss << "DataCenterID=" << pdata->DataCenterID << ";";

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcReqUserLogoutField *pdata)
{
    stringstream ss;
    ss << "structName=CUstpFtdcReqUserLogoutField ///Logout Request" << ";";
    if (!pdata)
    {
        ss << "<null>" << ";";
        return ss.str();
    }
    ss << "BrokerID=" << pdata->BrokerID << ";";
    ss << "UserID=" << pdata->UserID << ";";

    return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcQryInvestorAccountField *pdata)
{
	stringstream ss;
	ss << "structName=CUstpFtdcQryInvestorAccountField ///Capital account query" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << "BrokerID=" << pdata->BrokerID  << ";";
	ss << "UserID=" << pdata->UserID << ";";
	ss << "InvestorID=" << pdata->InvestorID << ";";

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcQryOrderField *pdata)
{
	stringstream ss;
	ss << "structName=CUstpFtdcQryOrderField ///Order Query" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << "BrokerID=" << pdata->BrokerID << ";";
	ss << "UserID=" << pdata->UserID << ";";
	ss << "ExchangeID=" << pdata->ExchangeID << ";";
	ss << "InvestorID=" << pdata->InvestorID << ";";
	ss << "OrderSysID=" << pdata->OrderSysID << ";";
	ss << "InstrumentID=" << pdata->InstrumentID << ";";

	return ss.str();

}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcQryInvestorPositionField *pdata)
{
	stringstream ss;
	ss << "structName=CUstpFtdcQryInvestorPositionField	///Investor position query" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << "BrokerID=" << pdata->BrokerID << ";";
	ss << "UserID=" << pdata->UserID  << ";";
	ss << "ExchangeID=" << pdata->ExchangeID << ";";
	ss << "InvestorID=" << pdata->InvestorID << ";";
	ss << "InstrumentID=" << pdata->InstrumentID << ";";

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcRspUserLoginField* pdata)
{
	stringstream ss;
	ss << "structName=CUstpFtdcRspUserLoginField ///Login Respond" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}
	ss << "TradingDay=" << pdata->TradingDay << ";";
	ss << "BrokerID=" << pdata->BrokerID << ";";
	ss << "UserID=" << pdata->UserID  << ";";
	ss << "LoginTime=" << pdata->LoginTime << ";";
	ss << "MaxOrderLocalID=" << pdata->MaxOrderLocalID << ";";
	ss << "TradingSystemName=" << pdata->TradingSystemName << ";";
	ss << "DataCenterID=" << pdata->DataCenterID << ";";
	ss << "PrivateFlowSize=" << pdata->PrivateFlowSize << ";";
	ss << "UserFlowSize=" << pdata->UserFlowSize << ";";

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcRspUserLogoutField* pdata)
{
    stringstream ss;
    ss << "structName=CUstpFtdcRspUserLogoutField // Logout Respond" << ";";
    if (!pdata)
    {
        ss << "<null>" << ";";
        return ss.str();
    }
    ss << "BrokerID=" << pdata->BrokerID << ";";
    ss << "UserID=" << pdata->UserID  << ";";

    return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcUserPasswordUpdateField* pdata)
{
	stringstream ss;
	ss <<  "structName=CUstpFtdcUserPasswordUpdateField	///Password change" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}
	ss << "BrokerID=" << pdata->BrokerID  << ";";
	ss << "UserID=" << pdata->UserID << ";";
	ss << "OldPassword=" << pdata->OldPassword  << ";";
	ss << "NewPassword=" << pdata->NewPassword << ";";

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


