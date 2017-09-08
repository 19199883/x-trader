

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcReqUserLoginField *pdata)
{
	stringstream ss;
	ss << newline_string << indent_string << "structName=CUstpFtdcReqUserLoginField		///Login Request" << endl;
	if (!pdata)
	{
		ss << "<null>" << endl;
		return ss.str();
	}

	ss << indent_string << "TradingDay=" << pdata->TradingDay << endl;
	ss << indent_string << "UserID=" << pdata->UserID  << endl;
	ss << indent_string << "BrokerID=" << pdata->BrokerID << endl;
	ss << indent_string << "UserProductInfo=" << pdata->UserProductInfo << endl;
	ss << indent_string << "InterfaceProductInfo=" << pdata->InterfaceProductInfo<< endl;
	ss << indent_string << "ProtocolInfo=" << pdata->ProtocolInfo << endl;
	ss << indent_string << "IPAddress=" << pdata->IPAddress<< endl;
	ss << indent_string << "MacAddress=" << pdata->MacAddress  << endl;
	ss << indent_string << "DataCenterID=" << pdata->DataCenterID << endl;

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcRspUserLoginField* pdata)
{
	stringstream ss;
	ss << newline_string << indent_string << "structName=CUstpFtdcRspUserLoginField		///Login Respond" << endl;
	if (!pdata)
	{
		ss << "<null>" << endl;
		return ss.str();
	}
	ss << indent_string << "TradingDay=" << pdata->TradingDay << endl;
	ss << indent_string << "BrokerID=" << pdata->BrokerID << endl;
	ss << indent_string << "UserID=" << pdata->UserID  << endl;
	ss << indent_string << "LoginTime=" << pdata->LoginTime << endl;
	ss << indent_string << "MaxOrderLocalID=" << pdata->MaxOrderLocalID << endl;
	ss << indent_string << "TradingSystemName=" << pdata->TradingSystemName << endl;
	ss << indent_string << "DataCenterID=" << pdata->DataCenterID << endl;
	ss << indent_string << "PrivateFlowSize=" << pdata->PrivateFlowSize << endl;
	ss << indent_string << "UserFlowSize=" << pdata->UserFlowSize << endl;

	return ss.str();
}



std::string FEMASDatatypeFormater::ToString(const CUstpFtdcInputOrderField* pdata)
{
	stringstream ss;
	ss << newline_string << indent_string << "structName=CUstpFtdcInputOrderField		///Input Order" << endl;
	if (!pdata)
	{
		ss << "<null>" << endl;
		return ss.str();
	}
	ss << indent_string << "BrokerID=" << pdata->BrokerID << endl;
	ss << indent_string << "ExchangeID=" << pdata->ExchangeID << endl;
	ss << indent_string << "OrderSysID=" << pdata->OrderSysID << endl;
	ss << indent_string << "InvestorID=" << pdata->InvestorID << endl;
	ss << indent_string << "UserID=" << pdata->UserID << endl;
	ss << indent_string << "InstrumentID=" << pdata->InstrumentID << endl;
	ss << indent_string << "UserOrderLocalID=" << pdata->UserOrderLocalID << endl;
	ss << indent_string << "OrderPriceType=" << pdata->OrderPriceType << endl;
	ss << indent_string << "Direction=" << pdata->Direction << endl;
	ss << indent_string << "OffsetFlag=" << pdata->OffsetFlag << endl;
	ss << indent_string << "HedgeFlag=" << pdata->HedgeFlag << endl;
	ss << indent_string << "LimitPrice=" << pdata->LimitPrice << endl;
	ss << indent_string << "Volume=" << pdata->Volume << endl;
	ss << indent_string << "TimeCondition=" << pdata->TimeCondition << endl;
	ss << indent_string << "GTDDate=" << pdata->GTDDate << endl;
	ss << indent_string << "VolumeCondition=" << pdata->VolumeCondition << endl;
	ss << indent_string << "MinVolume=" << pdata->MinVolume << endl;
	ss << indent_string << "StopPrice=" << pdata->StopPrice << endl;
	ss << indent_string << "ForceCloseReason=" << pdata->ForceCloseReason << endl;
	ss << indent_string << "IsAutoSuspend=" << pdata->IsAutoSuspend  << endl;
	ss << indent_string << "BusinessUnit=" << pdata->BusinessUnit  << endl;
	ss << indent_string << "UserCustom=" << pdata->UserCustom  << endl;

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcOrderActionField* pdata)
{
	stringstream ss;
	ss << newline_string << indent_string << "structName=CUstpFtdcOrderActionField		///Order Action" << endl;
	if (!pdata)
	{
		ss << "<null>" << endl;
		return ss.str();
	}

	ss << indent_string << "ExchangeID=" << pdata->ExchangeID << endl;
	ss << indent_string << "OrderSysID=" << pdata->OrderSysID << endl;
	ss << indent_string << "BrokerID=" << pdata->BrokerID << endl;
	ss << indent_string << "InvestorID=" << pdata->InvestorID << endl;
	ss << indent_string << "UserID=" << pdata->UserID << endl;
	ss << indent_string << "UserOrderActionLocalID=" << pdata->UserOrderActionLocalID << endl;
	ss << indent_string << "UserOrderLocalID=" << pdata->UserOrderLocalID << endl;
	ss << indent_string << "ActionFlag=" << pdata->ActionFlag << endl;
	ss << indent_string << "LimitPrice=" << pdata->LimitPrice << endl;
	ss << indent_string << "VolumeChange=" << pdata->VolumeChange << endl;

	return ss.str();
}


std::string FEMASDatatypeFormater::ToString(const CUstpFtdcOrderField* pdata)
{
	stringstream ss;
	ss << newline_string << indent_string << "structName=CUstpFtdcOrderField" << endl;
	if (!pdata)
	{
		ss << "<null>" << endl;
		return ss.str();
	}
	ss << indent_string << "BrokerID=" << pdata->BrokerID << endl;
	ss << indent_string << "ExchangeID=" << pdata->ExchangeID << endl;
	ss << indent_string << "OrderSysID=" << pdata->OrderSysID << endl;
	ss << indent_string << "InvestorID=" << pdata->InvestorID << endl;
	ss << indent_string << "UserID=" << pdata->UserID << endl;
	ss << indent_string << "InstrumentID=" << pdata->InstrumentID << endl;
	ss << indent_string << "UserOrderLocalID=" << pdata->UserOrderLocalID << endl;
	ss << indent_string << "OrderPriceType=" << pdata->OrderPriceType << endl;
	ss << indent_string << "Direction=" << pdata->Direction << endl;
	ss << indent_string << "OffsetFlag=" << pdata->OffsetFlag << endl;
	ss << indent_string << "HedgeFlag=" << pdata->HedgeFlag << endl;
	ss << indent_string << "LimitPrice=" << pdata->LimitPrice << endl;
	ss << indent_string << "Volume=" << pdata->Volume << endl;
	ss << indent_string << "TimeCondition=" << pdata->TimeCondition << endl;
	ss << indent_string << "GTDDate=" << pdata->GTDDate << endl;
	ss << indent_string << "VolumeCondition=" << pdata->VolumeCondition << endl;
	ss << indent_string << "MinVolume=" << pdata->MinVolume << endl;
	ss << indent_string << "StopPrice=" << pdata->StopPrice << endl;
	ss << indent_string << "ForceCloseReason=" << pdata->ForceCloseReason << endl;
	ss << indent_string << "IsAutoSuspend=" << pdata->IsAutoSuspend << endl;
	ss << indent_string << "BusinessUnit=" << pdata->BusinessUnit << endl;
	ss << indent_string << "UserCustom=" << pdata->UserCustom << endl;
	ss << indent_string << "TradingDay=" << pdata->TradingDay << endl;
	ss << indent_string << "ParticipantID=" << pdata->ParticipantID << endl;
	ss << indent_string << "ClientID=" << pdata->ClientID  << endl;
	ss << indent_string << "SeatID=" << pdata->SeatID << endl;
	ss << indent_string << "InsertTime=" << pdata->InsertTime << endl;
	ss << indent_string << "OrderLocalID=" << pdata->OrderLocalID << endl;

	ss << indent_string << "OrderSource=";
	pdata->OrderSource == '\0' ? ss << "<null>" : ss << pdata->OrderSource << endl;

	ss << indent_string << "OrderStatus=";
	pdata->OrderStatus == '\0' ? ss << "<null>" : ss << pdata->OrderStatus << endl;

	ss << indent_string << "CancelTime=" << pdata->CancelTime << endl;
	ss << indent_string << "CancelUserID=" << pdata->CancelUserID << endl;
	ss << indent_string << "VolumeTraded=" << pdata->VolumeTraded << endl;
	ss << indent_string << "VolumeRemain=" << pdata->VolumeRemain << endl;

	return ss.str();
}

std::string FEMASDatatypeFormater::ToString(const CUstpFtdcTradeField* pdata)
{
	stringstream ss;
	ss << newline_string << indent_string << "structName=CUstpFtdcTradeField" << endl;
	if (!pdata)
	{
		ss << "<null>" << endl;
		return ss.str();
	}

	ss << indent_string << "BrokerID=" << pdata->BrokerID << endl;
	ss << indent_string << "ExchangeID=" << pdata->ExchangeID << endl;
	ss << indent_string << "TradingDay=" << pdata->TradingDay << endl;
	ss << indent_string << "ParticipantID=" << pdata->ParticipantID << endl;
	ss << indent_string << "SeatID=" << pdata->SeatID << endl;
	ss << indent_string << "InvestorID=" << pdata->InvestorID << endl;
	ss << indent_string << "ClientID=" << pdata->ClientID << endl;
	ss << indent_string << "UserID=" << pdata->UserID << endl;
	ss << indent_string << "TradeID=" << pdata->TradeID << endl;
	ss << indent_string << "OrderSysID=" << pdata->OrderSysID << endl;
	ss << indent_string << "UserOrderLocalID=" << pdata->UserOrderLocalID << endl;
	ss << indent_string << "InstrumentID=" << pdata->InstrumentID << endl;
	ss << indent_string << "Direction=" << pdata->Direction << endl;
	ss << indent_string << "OffsetFlag=" << pdata->OffsetFlag << endl;
	ss << indent_string << "HedgeFlag=" << pdata->HedgeFlag << endl;
	ss << indent_string << "TradePrice=" << pdata->TradePrice << endl;
	ss << indent_string << "TradeVolume=" << pdata->TradeVolume << endl;
	ss << indent_string << "TradeTime=" << pdata->TradeTime << endl;
	ss << indent_string << "ClearingPartID=" << pdata->ClearingPartID << endl;

	return ss.str();
}


std::string FEMASDatatypeFormater::ToString(const CUstpFtdcRspInfoField* pdata)
{
	stringstream ss;
	ss << newline_string << indent_string << "structName=CUstpFtdcRspInfoField" << endl;
	if (!pdata)
	{
		ss << "<null>" << endl;
		return ss.str();
	}

	ss << indent_string << "ErrorID=" << pdata->ErrorID << endl;
	ss << indent_string << "ErrorMsg=" << pdata->ErrorMsg << endl;

	return ss.str();
}


