#include "femas_data_formater.h"
#include <iostream>
#include <sstream>

using namespace std;

static std::string indent_string = "    ";
static std::string newline_string = "\n";

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcReqUserLoginField *pdata)
{
	stringstream ss;
	ss << newline_string << indent_string << "structName=CUstpFtdcReqUserLoginField		///Login Request" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << indent_string << "TradingDay=" << pdata->TradingDay << ";";
	ss << indent_string << "UserID=" << pdata->UserID  << ";";
	ss << indent_string << "BrokerID=" << pdata->BrokerID << ";";
	ss << indent_string << "UserProductInfo=" << pdata->UserProductInfo << ";";
	ss << indent_string << "InterfaceProductInfo=" << pdata->InterfaceProductInfo<< ";";
	ss << indent_string << "ProtocolInfo=" << pdata->ProtocolInfo << ";";
	ss << indent_string << "IPAddress=" << pdata->IPAddress<< ";";
	ss << indent_string << "MacAddress=" << pdata->MacAddress  << ";";
	ss << indent_string << "DataCenterID=" << pdata->DataCenterID << ";";

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcReqUserLogoutField *pdata)
{
    stringstream ss;
    ss << newline_string << indent_string << "structName=CUstpFtdcReqUserLogoutField        ///Logout Request" << ";";
    if (!pdata)
    {
        ss << "<null>" << ";";
        return ss.str();
    }
    ss << indent_string << "BrokerID=" << pdata->BrokerID << ";";
    ss << indent_string << "UserID=" << pdata->UserID << ";";

    return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcQryInvestorAccountField *pdata)
{
	stringstream ss;
	ss << newline_string << indent_string << "structName=CUstpFtdcQryInvestorAccountField		///Capital account query" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << indent_string << "BrokerID=" << pdata->BrokerID  << ";";
	ss << indent_string << "UserID=" << pdata->UserID << ";";
	ss << indent_string << "InvestorID=" << pdata->InvestorID << ";";

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcQryOrderField *pdata)
{
	stringstream ss;
	ss << newline_string << indent_string << "structName=CUstpFtdcQryOrderField		///Order Query" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << indent_string << "BrokerID=" << pdata->BrokerID << ";";
	ss << indent_string << "UserID=" << pdata->UserID << ";";
	ss << indent_string << "ExchangeID=" << pdata->ExchangeID << ";";
	ss << indent_string << "InvestorID=" << pdata->InvestorID << ";";
	ss << indent_string << "OrderSysID=" << pdata->OrderSysID << ";";
	ss << indent_string << "InstrumentID=" << pdata->InstrumentID << ";";

	return ss.str();

}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcQryInvestorPositionField *pdata)
{
	stringstream ss;
	ss << newline_string << indent_string << "structName=CUstpFtdcQryInvestorPositionField		///Investor position query" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << indent_string << "BrokerID=" << pdata->BrokerID << ";";
	ss << indent_string << "UserID=" << pdata->UserID  << ";";
	ss << indent_string << "ExchangeID=" << pdata->ExchangeID << ";";
	ss << indent_string << "InvestorID=" << pdata->InvestorID << ";";
	ss << indent_string << "InstrumentID=" << pdata->InstrumentID << ";";

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcRspUserLoginField* pdata)
{
	stringstream ss;
	ss << newline_string << indent_string << "structName=CUstpFtdcRspUserLoginField		///Login Respond" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}
	ss << indent_string << "TradingDay=" << pdata->TradingDay << ";";
	ss << indent_string << "BrokerID=" << pdata->BrokerID << ";";
	ss << indent_string << "UserID=" << pdata->UserID  << ";";
	ss << indent_string << "LoginTime=" << pdata->LoginTime << ";";
	ss << indent_string << "MaxOrderLocalID=" << pdata->MaxOrderLocalID << ";";
	ss << indent_string << "TradingSystemName=" << pdata->TradingSystemName << ";";
	ss << indent_string << "DataCenterID=" << pdata->DataCenterID << ";";
	ss << indent_string << "PrivateFlowSize=" << pdata->PrivateFlowSize << ";";
	ss << indent_string << "UserFlowSize=" << pdata->UserFlowSize << ";";

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcRspUserLogoutField* pdata)
{
    stringstream ss;
    ss << newline_string << indent_string << "structName=CUstpFtdcRspUserLogoutField  // Logout Respond" << ";";
    if (!pdata)
    {
        ss << "<null>" << ";";
        return ss.str();
    }
    ss << indent_string << "BrokerID=" << pdata->BrokerID << ";";
    ss << indent_string << "UserID=" << pdata->UserID  << ";";

    return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcUserPasswordUpdateField* pdata)
{
	stringstream ss;
	ss <<  newline_string << indent_string << "structName=CUstpFtdcUserPasswordUpdateField		///Password change" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}
	ss << indent_string << "BrokerID=" << pdata->BrokerID  << ";";
	ss << indent_string << "UserID=" << pdata->UserID << ";";
	ss << indent_string << "OldPassword=" << pdata->OldPassword  << ";";
	ss << indent_string << "NewPassword=" << pdata->NewPassword << ";";

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcInputOrderField* pdata)
{
	stringstream ss;
	ss << newline_string << indent_string << "structName=CUstpFtdcInputOrderField		///Input Order";
	if (!pdata)
	{
		ss << "<null>";
		return ss.str();
	}
	ss << indent_string << "BrokerID=" << pdata->BrokerID << ";";
	ss << indent_string << "ExchangeID=" << pdata->ExchangeID << ";";
	ss << indent_string << "OrderSysID=" << pdata->OrderSysID << ";";
	ss << indent_string << "InvestorID=" << pdata->InvestorID << ";";
	ss << indent_string << "UserID=" << pdata->UserID << ";";
	ss << indent_string << "InstrumentID=" << pdata->InstrumentID << ";";
	ss << indent_string << "UserOrderLocalID=" << pdata->UserOrderLocalID << ";";
	ss << indent_string << "OrderPriceType=" << pdata->OrderPriceType << ";";
	ss << indent_string << "Direction=" << pdata->Direction << ";";
	ss << indent_string << "OffsetFlag=" << pdata->OffsetFlag << ";";
	ss << indent_string << "HedgeFlag=" << pdata->HedgeFlag << ";";
	ss << indent_string << "LimitPrice=" << pdata->LimitPrice << ";";
	ss << indent_string << "Volume=" << pdata->Volume << ";";
	ss << indent_string << "TimeCondition=" << pdata->TimeCondition << ";";
	ss << indent_string << "GTDDate=" << pdata->GTDDate << ";";
	ss << indent_string << "VolumeCondition=" << pdata->VolumeCondition << ";";
	ss << indent_string << "MinVolume=" << pdata->MinVolume << ";";
	ss << indent_string << "StopPrice=" << pdata->StopPrice << ";";
	ss << indent_string << "ForceCloseReason=" << pdata->ForceCloseReason << ";";
	ss << indent_string << "IsAutoSuspend=" << pdata->IsAutoSuspend  << ";";
	ss << indent_string << "BusinessUnit=" << pdata->BusinessUnit  << ";";
	ss << indent_string << "UserCustom=" << pdata->UserCustom  << ";";

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcOrderActionField* pdata)
{
	stringstream ss;
	ss << newline_string << indent_string << "structName=CUstpFtdcOrderActionField		///Order Action" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << indent_string << "ExchangeID=" << pdata->ExchangeID << ";";
	ss << indent_string << "OrderSysID=" << pdata->OrderSysID << ";";
	ss << indent_string << "BrokerID=" << pdata->BrokerID << ";";
	ss << indent_string << "InvestorID=" << pdata->InvestorID << ";";
	ss << indent_string << "UserID=" << pdata->UserID << ";";
	ss << indent_string << "UserOrderActionLocalID=" << pdata->UserOrderActionLocalID << ";";
	ss << indent_string << "UserOrderLocalID=" << pdata->UserOrderLocalID << ";";
	ss << indent_string << "ActionFlag=" << pdata->ActionFlag << ";";
	ss << indent_string << "LimitPrice=" << pdata->LimitPrice << ";";
	ss << indent_string << "VolumeChange=" << pdata->VolumeChange << ";";

	return ss.str();
}


std::string FEMASDatatypeFormater::ToString(const CUstpFtdcOrderField* pdata)
{
	stringstream ss;
	ss << newline_string << indent_string << "structName=CUstpFtdcOrderField" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}
	ss << indent_string << "BrokerID=" << pdata->BrokerID << ";";
	ss << indent_string << "ExchangeID=" << pdata->ExchangeID << ";";
	ss << indent_string << "OrderSysID=" << pdata->OrderSysID << ";";
	ss << indent_string << "InvestorID=" << pdata->InvestorID << ";";
	ss << indent_string << "UserID=" << pdata->UserID << ";";
	ss << indent_string << "InstrumentID=" << pdata->InstrumentID << ";";
	ss << indent_string << "UserOrderLocalID=" << pdata->UserOrderLocalID << ";";
	ss << indent_string << "OrderPriceType=" << pdata->OrderPriceType << ";";
	ss << indent_string << "Direction=" << pdata->Direction << ";";
	ss << indent_string << "OffsetFlag=" << pdata->OffsetFlag << ";";
	ss << indent_string << "HedgeFlag=" << pdata->HedgeFlag << ";";
	ss << indent_string << "LimitPrice=" << pdata->LimitPrice << ";";
	ss << indent_string << "Volume=" << pdata->Volume << ";";
	ss << indent_string << "TimeCondition=" << pdata->TimeCondition << ";";
	ss << indent_string << "GTDDate=" << pdata->GTDDate << ";";
	ss << indent_string << "VolumeCondition=" << pdata->VolumeCondition << ";";
	ss << indent_string << "MinVolume=" << pdata->MinVolume << ";";
	ss << indent_string << "StopPrice=" << pdata->StopPrice << ";";
	ss << indent_string << "ForceCloseReason=" << pdata->ForceCloseReason << ";";
	ss << indent_string << "IsAutoSuspend=" << pdata->IsAutoSuspend << ";";
	ss << indent_string << "BusinessUnit=" << pdata->BusinessUnit << ";";
	ss << indent_string << "UserCustom=" << pdata->UserCustom << ";";
	ss << indent_string << "TradingDay=" << pdata->TradingDay << ";";
	ss << indent_string << "ParticipantID=" << pdata->ParticipantID << ";";
	ss << indent_string << "ClientID=" << pdata->ClientID  << ";";
	ss << indent_string << "SeatID=" << pdata->SeatID << ";";
	ss << indent_string << "InsertTime=" << pdata->InsertTime << ";";
	ss << indent_string << "OrderLocalID=" << pdata->OrderLocalID << ";";

	ss << indent_string << "OrderSource=";
	pdata->OrderSource == '\0' ? ss << "<null>" : ss << pdata->OrderSource << ";";

	ss << indent_string << "OrderStatus=";
	pdata->OrderStatus == '\0' ? ss << "<null>" : ss << pdata->OrderStatus << ";";

	ss << indent_string << "CancelTime=" << pdata->CancelTime << ";";
	ss << indent_string << "CancelUserID=" << pdata->CancelUserID << ";";
	ss << indent_string << "VolumeTraded=" << pdata->VolumeTraded << ";";
	ss << indent_string << "VolumeRemain=" << pdata->VolumeRemain << ";";

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcTradeField* pdata)
{
	stringstream ss;
	ss << newline_string << indent_string << "structName=CUstpFtdcTradeField" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << indent_string << "BrokerID=" << pdata->BrokerID << ";";
	ss << indent_string << "ExchangeID=" << pdata->ExchangeID << ";";
	ss << indent_string << "TradingDay=" << pdata->TradingDay << ";";
	ss << indent_string << "ParticipantID=" << pdata->ParticipantID << ";";
	ss << indent_string << "SeatID=" << pdata->SeatID << ";";
	ss << indent_string << "InvestorID=" << pdata->InvestorID << ";";
	ss << indent_string << "ClientID=" << pdata->ClientID << ";";
	ss << indent_string << "UserID=" << pdata->UserID << ";";
	ss << indent_string << "TradeID=" << pdata->TradeID << ";";
	ss << indent_string << "OrderSysID=" << pdata->OrderSysID << ";";
	ss << indent_string << "UserOrderLocalID=" << pdata->UserOrderLocalID << ";";
	ss << indent_string << "InstrumentID=" << pdata->InstrumentID << ";";
	ss << indent_string << "Direction=" << pdata->Direction << ";";
	ss << indent_string << "OffsetFlag=" << pdata->OffsetFlag << ";";
	ss << indent_string << "HedgeFlag=" << pdata->HedgeFlag << ";";
	ss << indent_string << "TradePrice=" << pdata->TradePrice << ";";
	ss << indent_string << "TradeVolume=" << pdata->TradeVolume << ";";
	ss << indent_string << "TradeTime=" << pdata->TradeTime << ";";
	ss << indent_string << "ClearingPartID=" << pdata->ClearingPartID << ";";

	return ss.str();
}



std::string FEMASDatatypeFormater::ToString(const CUstpFtdcRspInvestorPositionField* pdata)
{
	stringstream ss;
	ss << newline_string << indent_string << "structName=CUstpFtdcRspInvestorPositionField" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << indent_string << "InvestorID=" << pdata->InvestorID << ";";
	ss << indent_string << "BrokerID=" << pdata->BrokerID << ";";
	ss << indent_string << "ExchangeID=" << pdata->ExchangeID << ";";
	ss << indent_string << "ClientID=" << pdata->ClientID << ";";
	ss << indent_string << "InstrumentID=" << pdata->InstrumentID << ";";
	ss << indent_string << "Direction=" << pdata->Direction << ";";
	ss << indent_string << "HedgeFlag=" << pdata->HedgeFlag << ";";
	ss << indent_string << "UsedMargin=" << pdata->UsedMargin << ";";
	ss << indent_string << "Position=" << pdata->Position << ";";
	ss << indent_string << "PositionCost=" << pdata->PositionCost << ";";
	ss << indent_string << "YdPosition=" << pdata->YdPosition << ";";
	ss << indent_string << "YdPositionCost=" << pdata->YdPositionCost << ";";
	ss << indent_string << "FrozenMargin=" << pdata->FrozenMargin << ";";
	ss << indent_string << "FrozenPosition=" << pdata->FrozenPosition << ";";
	ss << indent_string << "FrozenClosing=" << pdata->FrozenClosing << ";";
	ss << indent_string << "FrozenPremium=" << pdata->FrozenPremium << ";";
	ss << indent_string << "LastTradeID=" << pdata->LastTradeID << ";";
	ss << indent_string << "LastOrderLocalID=" << pdata->LastOrderLocalID << ";";
	ss << indent_string << "Currency=" << pdata->Currency << ";";

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcRspInfoField* pdata)
{
	stringstream ss;
	ss << newline_string << indent_string << "structName=CUstpFtdcRspInfoField" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}

	ss << indent_string << "ErrorID=" << pdata->ErrorID << ";";
	ss << indent_string << "ErrorMsg=" << pdata->ErrorMsg << ";";

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcRspInvestorAccountField *pdata)
{
	stringstream ss;
	ss << std::fixed <<  newline_string << "structName=CUstpFtdcRspInvestorAccountField" << ";";
	if (!pdata)
	{
		ss << "<null>" << ";";
		return ss.str();
	}
	ss << indent_string << "BrokerID=" << pdata->BrokerID << ";";
	ss << indent_string << "InvestorID=" << pdata->InvestorID << ";";
	ss << indent_string << "AccountID=" << pdata->AccountID << ";";
	ss << indent_string << "PreBalance=" << pdata->PreBalance << ";";
	ss << indent_string << "Deposit=" << pdata->Deposit << ";";
	ss << indent_string << "Withdraw=" << pdata->Withdraw << ";";
	ss << indent_string << "FrozenMargin=" << pdata->FrozenMargin << ";";
	ss << indent_string << "FrozenFee=" << pdata->FrozenFee << ";";
	ss << indent_string << "FrozenPremium=" << pdata->FrozenPremium << ";";
	ss << indent_string << "Fee=" << pdata->Fee << ";";
	ss << indent_string << "CloseProfit=" << pdata->CloseProfit << ";";
	ss << indent_string << "PositionProfit=" << pdata->PositionProfit << ";";
	ss << indent_string << "Available=" << pdata->Available << ";";
	ss << indent_string << "LongFrozenMargin=" << pdata->LongFrozenMargin << ";";
	ss << indent_string << "ShortFrozenMargin=" << pdata->ShortFrozenMargin << ";";
	ss << indent_string << "LongMargin=" << pdata->LongMargin << ";";
	ss << indent_string << "ShortMargin=" << pdata->ShortMargin << ";";
	ss << indent_string << "ReleaseMargin=" << pdata->ReleaseMargin << ";";
	ss << indent_string << "DynamicRights=" << pdata->DynamicRights << ";";
	ss << indent_string << "TodayInOut=" << pdata->TodayInOut << ";";
	ss << indent_string << "Margin=" << pdata->Margin << ";";
	ss << indent_string << "Premium=" << pdata->Premium << ";";
	ss << indent_string << "Risk=" << pdata->Risk << ";";

	return ss.str();
}


