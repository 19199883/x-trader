/////////////////////////////////////////////////////////////////////////
///@system ���ǰ��ϵͳ
///@company CFFEX
///@file USTPFtdcUserApiStruct.h
///@brief �����˿ͻ��˽ӿ�ʹ�õ�ҵ�����ݽṹ
///@history 
/////////////////////////////////////////////////////////////////////////

#if !defined(_USTP_FTDCSTRUCT_H)
#define _USTP_FTDCSTRUCT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "USTPFtdcUserApiDataType.h"

///ϵͳ�û���¼����
struct CUstpFtdcReqUserLoginField
{
	///������
	TUstpFtdcDateType	TradingDay;
	///�����û�����
	TUstpFtdcUserIDType	UserID;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///����
	TUstpFtdcPasswordType	Password;
	///�û��˲�Ʒ��Ϣ
	TUstpFtdcProductInfoType	UserProductInfo;
	///�ӿڶ˲�Ʒ��Ϣ
	TUstpFtdcProductInfoType	InterfaceProductInfo;
	///Э����Ϣ
	TUstpFtdcProtocolInfoType	ProtocolInfo;
	///IP��ַ
	TUstpFtdcIPAddressType	IPAddress;
	///Mac��ַ
	TUstpFtdcMacAddressType	MacAddress;
	///�������Ĵ���
	TUstpFtdcDataCenterIDType	DataCenterID;
	///�ͻ��������ļ���С
	TUstpFtdcFileSizeType	UserProductFileSize;
};
///ϵͳ�û���¼Ӧ��
struct CUstpFtdcRspUserLoginField
{
	///������
	TUstpFtdcDateType	TradingDay;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�����û�����
	TUstpFtdcUserIDType	UserID;
	///��¼�ɹ�ʱ��
	TUstpFtdcTimeType	LoginTime;
	///��¼�ɹ�ʱ�Ľ�����ʱ��
	TUstpFtdcTimeType	ExchangeTime;
	///�û���󱾵ر�����
	TUstpFtdcUserOrderLocalIDType	MaxOrderLocalID;
	///����ϵͳ����
	TUstpFtdcTradingSystemNameType	TradingSystemName;
	///�������Ĵ���
	TUstpFtdcDataCenterIDType	DataCenterID;
	///��Ա˽������ǰ����
	TUstpFtdcSequenceNoType	PrivateFlowSize;
	///����Ա˽������ǰ����
	TUstpFtdcSequenceNoType	UserFlowSize;
	///ҵ��������
	TUstpFtdcDateType	ActionDay;
	///����汾��
	TUstpFtdcFemasVersionType	FemasVersion;
	///�����������ں�
	TUstpFtdcFemasLifeCycleType	FemasLifeCycle;
};
///�û��ǳ�����
struct CUstpFtdcReqUserLogoutField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�����û�����
	TUstpFtdcUserIDType	UserID;
};
///�û��ǳ���Ӧ
struct CUstpFtdcRspUserLogoutField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�����û�����
	TUstpFtdcUserIDType	UserID;
};
///ǿ���û��˳�
struct CUstpFtdcForceUserExitField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�����û�����
	TUstpFtdcUserIDType	UserID;
};
///�û������޸�
struct CUstpFtdcUserPasswordUpdateField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�����û�����
	TUstpFtdcUserIDType	UserID;
	///������
	TUstpFtdcPasswordType	OldPassword;
	///������
	TUstpFtdcPasswordType	NewPassword;
};
///���뱨��
struct CUstpFtdcInputOrderField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///ϵͳ�������
	TUstpFtdcOrderSysIDType	OrderSysID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///ָ��ͨ����ϯλ����µ�
	TUstpFtdcSeatNoType	SeatNo;
	///��Լ����/������Ϻ�Լ��
	TUstpFtdcInstrumentIDType	InstrumentID;
	///�û����ر�����
	TUstpFtdcUserOrderLocalIDType	UserOrderLocalID;
	///��������
	TUstpFtdcOrderPriceTypeType	OrderPriceType;
	///��������
	TUstpFtdcDirectionType	Direction;
	///��ƽ��־
	TUstpFtdcOffsetFlagType	OffsetFlag;
	///Ͷ���ױ���־
	TUstpFtdcHedgeFlagType	HedgeFlag;
	///�۸�
	TUstpFtdcPriceType	LimitPrice;
	///����
	TUstpFtdcVolumeType	Volume;
	///��Ч������
	TUstpFtdcTimeConditionType	TimeCondition;
	///GTD����
	TUstpFtdcDateType	GTDDate;
	///�ɽ�������
	TUstpFtdcVolumeConditionType	VolumeCondition;
	///��С�ɽ���
	TUstpFtdcVolumeType	MinVolume;
	///ֹ���ֹӮ��
	TUstpFtdcPriceType	StopPrice;
	///ǿƽԭ��
	TUstpFtdcForceCloseReasonType	ForceCloseReason;
	///�Զ������־
	TUstpFtdcBoolType	IsAutoSuspend;
	///ҵ��Ԫ
	TUstpFtdcBusinessUnitType	BusinessUnit;
	///�û��Զ�����
	TUstpFtdcCustomType	UserCustom;
	///����ҵ���ʶ
	TUstpFtdcBusinessLocalIDType	BusinessLocalID;
	///ҵ��������
	TUstpFtdcDateType	ActionDay;
	///�������
	TUstpFtdcArbiTypeType	ArbiType;
};
///��������
struct CUstpFtdcOrderActionField
{
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///�������
	TUstpFtdcOrderSysIDType	OrderSysID;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///���γ��������ı��ر��
	TUstpFtdcUserOrderLocalIDType	UserOrderActionLocalID;
	///���������ı��ر������
	TUstpFtdcUserOrderLocalIDType	UserOrderLocalID;
	///����������־
	TUstpFtdcActionFlagType	ActionFlag;
	///�۸�
	TUstpFtdcPriceType	LimitPrice;
	///�����仯
	TUstpFtdcVolumeType	VolumeChange;
	///����ҵ���ʶ
	TUstpFtdcBusinessLocalIDType	BusinessLocalID;
};
///�ڴ����
struct CUstpFtdcMemDbField
{
	///�ڴ����
	TUstpFtdcMemTableNameType	MemTableName;
};
///�û���������
struct CUstpFtdcstpUserDepositField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///���
	TUstpFtdcMoneyType	Amount;
	///�������
	TUstpFtdcAccountDirectionType	AmountDirection;
	///�û����ر�����
	TUstpFtdcUserOrderLocalIDType	UserOrderLocalID;
};
///�û�����ϯ�����
struct CUstpFtdcstpTransferMoneyField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///���
	TUstpFtdcMoneyType	Amount;
	///�������
	TUstpFtdcAccountDirectionType	AmountDirection;
	///�û����ر�����
	TUstpFtdcUserOrderLocalIDType	UserOrderLocalID;
	///���л�������
	TUstpFtdcBankIDType	BankID;
	///����ת������
	TUstpFtdcBankPasswdType	BankPasswd;
	///��ϯת������
	TUstpFtdcAccountPasswdType	AccountPasswd;
};
///��Ӧ��Ϣ
struct CUstpFtdcRspInfoField
{
	///�������
	TUstpFtdcErrorIDType	ErrorID;
	///������Ϣ
	TUstpFtdcErrorMsgType	ErrorMsg;
};
///������ѯ
struct CUstpFtdcQryOrderField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///�������
	TUstpFtdcOrderSysIDType	OrderSysID;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
	///����״̬
	TUstpFtdcOrderStatusType	OrderStatus;
	///ί������
	TUstpFtdcOrderTypeType	OrderType;
};
///�ɽ���ѯ
struct CUstpFtdcQryTradeField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///�ɽ����
	TUstpFtdcTradeIDType	TradeID;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
};
///��Լ��ѯ
struct CUstpFtdcQryInstrumentField
{
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///��Ʒ����
	TUstpFtdcProductIDType	ProductID;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
};
///��Լ��ѯӦ��
struct CUstpFtdcRspInstrumentField
{
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///Ʒ�ִ���
	TUstpFtdcProductIDType	ProductID;
	///Ʒ������
	TUstpFtdcProductNameType	ProductName;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
	///��Լ����
	TUstpFtdcInstrumentNameType	InstrumentName;
	///�������
	TUstpFtdcYearType	DeliveryYear;
	///������
	TUstpFtdcMonthType	DeliveryMonth;
	///�޼۵�����µ���
	TUstpFtdcVolumeType	MaxLimitOrderVolume;
	///�޼۵���С�µ���
	TUstpFtdcVolumeType	MinLimitOrderVolume;
	///�м۵�����µ���
	TUstpFtdcVolumeType	MaxMarketOrderVolume;
	///�м۵���С�µ���
	TUstpFtdcVolumeType	MinMarketOrderVolume;
	///��������
	TUstpFtdcVolumeMultipleType	VolumeMultiple;
	///���۵�λ
	TUstpFtdcPriceTickType	PriceTick;
	///����
	TUstpFtdcCurrencyType	Currency;
	///��ͷ�޲�
	TUstpFtdcVolumeType	LongPosLimit;
	///��ͷ�޲�
	TUstpFtdcVolumeType	ShortPosLimit;
	///��ͣ���
	TUstpFtdcPriceType	LowerLimitPrice;
	///��ͣ���
	TUstpFtdcPriceType	UpperLimitPrice;
	///�����
	TUstpFtdcPriceType	PreSettlementPrice;
	///��Լ����״̬
	TUstpFtdcInstrumentStatusType	InstrumentStatus;
	///������
	TUstpFtdcDateType	CreateDate;
	///������
	TUstpFtdcDateType	OpenDate;
	///������
	TUstpFtdcDateType	ExpireDate;
	///��ʼ������
	TUstpFtdcDateType	StartDelivDate;
	///��󽻸���
	TUstpFtdcDateType	EndDelivDate;
	///���ƻ�׼��
	TUstpFtdcPriceType	BasisPrice;
	///��ǰ�Ƿ���
	TUstpFtdcBoolType	IsTrading;
	///������Ʒ����
	TUstpFtdcInstrumentIDType	UnderlyingInstrID;
	///������Ʒ����
	TUstpFtdcUnderlyingMultipleType	UnderlyingMultiple;
	///�ֲ�����
	TUstpFtdcPositionTypeType	PositionType;
	///ִ�м�
	TUstpFtdcPriceType	StrikePrice;
	///��Ȩ����
	TUstpFtdcOptionsTypeType	OptionsType;
	///���ִ���
	TUstpFtdcCurrencyIDType	CurrencyID;
	///�������
	TUstpFtdcArbiTypeType	ArbiType;
	///��һ�Ⱥ�Լ����
	TUstpFtdcInstrumentIDType	InstrumentID_1;
	///��һ����������
	TUstpFtdcDirectionType	Direction_1;
	///��һ����������
	TUstpFtdcRatioType	Ratio_1;
	///�ڶ��Ⱥ�Լ����
	TUstpFtdcInstrumentIDType	InstrumentID_2;
	///�ڶ�����������
	TUstpFtdcDirectionType	Direction_2;
	///�ڶ�����������
	TUstpFtdcRatioType	Ratio_2;
};
///��Լ״̬
struct CUstpFtdcInstrumentStatusField
{
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///Ʒ�ִ���
	TUstpFtdcProductIDType	ProductID;
	///Ʒ������
	TUstpFtdcProductNameType	ProductName;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
	///��Լ����
	TUstpFtdcInstrumentNameType	InstrumentName;
	///�������
	TUstpFtdcYearType	DeliveryYear;
	///������
	TUstpFtdcMonthType	DeliveryMonth;
	///�޼۵�����µ���
	TUstpFtdcVolumeType	MaxLimitOrderVolume;
	///�޼۵���С�µ���
	TUstpFtdcVolumeType	MinLimitOrderVolume;
	///�м۵�����µ���
	TUstpFtdcVolumeType	MaxMarketOrderVolume;
	///�м۵���С�µ���
	TUstpFtdcVolumeType	MinMarketOrderVolume;
	///��������
	TUstpFtdcVolumeMultipleType	VolumeMultiple;
	///���۵�λ
	TUstpFtdcPriceTickType	PriceTick;
	///����
	TUstpFtdcCurrencyType	Currency;
	///��ͷ�޲�
	TUstpFtdcVolumeType	LongPosLimit;
	///��ͷ�޲�
	TUstpFtdcVolumeType	ShortPosLimit;
	///��ͣ���
	TUstpFtdcPriceType	LowerLimitPrice;
	///��ͣ���
	TUstpFtdcPriceType	UpperLimitPrice;
	///�����
	TUstpFtdcPriceType	PreSettlementPrice;
	///��Լ����״̬
	TUstpFtdcInstrumentStatusType	InstrumentStatus;
	///������
	TUstpFtdcDateType	CreateDate;
	///������
	TUstpFtdcDateType	OpenDate;
	///������
	TUstpFtdcDateType	ExpireDate;
	///��ʼ������
	TUstpFtdcDateType	StartDelivDate;
	///��󽻸���
	TUstpFtdcDateType	EndDelivDate;
	///���ƻ�׼��
	TUstpFtdcPriceType	BasisPrice;
	///��ǰ�Ƿ���
	TUstpFtdcBoolType	IsTrading;
	///������Ʒ����
	TUstpFtdcInstrumentIDType	UnderlyingInstrID;
	///������Ʒ����
	TUstpFtdcUnderlyingMultipleType	UnderlyingMultiple;
	///�ֲ�����
	TUstpFtdcPositionTypeType	PositionType;
	///ִ�м�
	TUstpFtdcPriceType	StrikePrice;
	///��Ȩ����
	TUstpFtdcOptionsTypeType	OptionsType;
	///���ִ���
	TUstpFtdcCurrencyIDType	CurrencyID;
	///�������
	TUstpFtdcArbiTypeType	ArbiType;
	///��һ�Ⱥ�Լ����
	TUstpFtdcInstrumentIDType	InstrumentID_1;
	///��һ����������
	TUstpFtdcDirectionType	Direction_1;
	///��һ����������
	TUstpFtdcRatioType	Ratio_1;
	///�ڶ��Ⱥ�Լ����
	TUstpFtdcInstrumentIDType	InstrumentID_2;
	///�ڶ�����������
	TUstpFtdcDirectionType	Direction_2;
	///�ڶ�����������
	TUstpFtdcRatioType	Ratio_2;
	///���뱾״̬����
	TUstpFtdcDateType	EnterDate;
};
///Ͷ�����ʽ��ѯ
struct CUstpFtdcQryInvestorAccountField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
};
///Ͷ�����ʽ�Ӧ��
struct CUstpFtdcRspInvestorAccountField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///�ʽ��ʺ�
	TUstpFtdcAccountIDType	AccountID;
	///�ϴν���׼����
	TUstpFtdcMoneyType	PreBalance;
	///�����
	TUstpFtdcMoneyType	Deposit;
	///������
	TUstpFtdcMoneyType	Withdraw;
	///����ı�֤��
	TUstpFtdcMoneyType	FrozenMargin;
	///����������
	TUstpFtdcMoneyType	FrozenFee;
	///����Ȩ����
	TUstpFtdcMoneyType	FrozenPremium;
	///������
	TUstpFtdcMoneyType	Fee;
	///ƽ��ӯ��
	TUstpFtdcMoneyType	CloseProfit;
	///�ֲ�ӯ��
	TUstpFtdcMoneyType	PositionProfit;
	///�����ʽ�
	TUstpFtdcMoneyType	Available;
	///��ͷ����ı�֤��
	TUstpFtdcMoneyType	LongFrozenMargin;
	///��ͷ����ı�֤��
	TUstpFtdcMoneyType	ShortFrozenMargin;
	///��ͷռ�ñ�֤��
	TUstpFtdcMoneyType	LongMargin;
	///��ͷռ�ñ�֤��
	TUstpFtdcMoneyType	ShortMargin;
	///�����ͷű�֤��
	TUstpFtdcMoneyType	ReleaseMargin;
	///��̬Ȩ��
	TUstpFtdcMoneyType	DynamicRights;
	///���ճ����
	TUstpFtdcMoneyType	TodayInOut;
	///ռ�ñ�֤��
	TUstpFtdcMoneyType	Margin;
	///��ȨȨ������֧
	TUstpFtdcMoneyType	Premium;
	///���ն�
	TUstpFtdcMoneyType	Risk;
};
///����Ͷ���߲�ѯ
struct CUstpFtdcQryUserInvestorField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
};
///����Ͷ����
struct CUstpFtdcRspUserInvestorField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�����û�����
	TUstpFtdcUserIDType	UserID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
};
///���ױ����ѯ
struct CUstpFtdcQryTradingCodeField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
};
///���ױ����ѯ
struct CUstpFtdcRspTradingCodeField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///�ͻ�����
	TUstpFtdcClientIDType	ClientID;
	///�ͻ�����Ȩ��
	TUstpFtdcTradingRightType	ClientRight;
	///�ͻ���ֵ����
	TUstpFtdcHedgeFlagType	ClientHedgeFlag;
	///�Ƿ��Ծ
	TUstpFtdcIsActiveType	IsActive;
};
///��������ѯ����
struct CUstpFtdcQryExchangeField
{
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
};
///��������ѯӦ��
struct CUstpFtdcRspExchangeField
{
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///����������
	TUstpFtdcExchangeNameType	ExchangeName;
};
///Ͷ���ֲֲ߳�ѯ����
struct CUstpFtdcQryInvestorPositionField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
};
///Ͷ���ֲֲ߳�ѯӦ��
struct CUstpFtdcRspInvestorPositionField
{
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///�ͻ�����
	TUstpFtdcClientIDType	ClientID;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
	///��������
	TUstpFtdcDirectionType	Direction;
	///Ͷ���ױ���־
	TUstpFtdcHedgeFlagType	HedgeFlag;
	///ռ�ñ�֤��
	TUstpFtdcMoneyType	UsedMargin;
	///���ֲܳ���
	TUstpFtdcVolumeType	Position;
	///���ճֲֳɱ�
	TUstpFtdcPriceType	PositionCost;
	///��ֲ���
	TUstpFtdcVolumeType	YdPosition;
	///���ճֲֳɱ�
	TUstpFtdcMoneyType	YdPositionCost;
	///����ı�֤��
	TUstpFtdcMoneyType	FrozenMargin;
	///���ֶ���ֲ�
	TUstpFtdcVolumeType	FrozenPosition;
	///ƽ�ֶ���ֲ�
	TUstpFtdcVolumeType	FrozenClosing;
	///ƽ��ֶ���ֲ�
	TUstpFtdcVolumeType	YdFrozenClosing;
	///�����Ȩ����
	TUstpFtdcMoneyType	FrozenPremium;
	///���һ�ʳɽ����
	TUstpFtdcTradeIDType	LastTradeID;
	///���һ�ʱ��ر������
	TUstpFtdcOrderLocalIDType	LastOrderLocalID;
	///Ͷ���ֲ���
	TUstpFtdcVolumeType	SpeculationPosition;
	///�����ֲ���
	TUstpFtdcVolumeType	ArbitragePosition;
	///�ױ��ֲ���
	TUstpFtdcVolumeType	HedgePosition;
	///Ͷ��ƽ�ֶ�����
	TUstpFtdcVolumeType	SpecFrozenClosing;
	///�ױ�ƽ�ֶ�����
	TUstpFtdcVolumeType	HedgeFrozenClosing;
	///����
	TUstpFtdcCurrencyIDType	Currency;
};
///�Ϲ������ѯ����
struct CUstpFtdcQryComplianceParamField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
};
///�Ϲ������ѯӦ��
struct CUstpFtdcRspComplianceParamField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///�ͻ���
	TUstpFtdcClientIDType	ClientID;
	///ÿ����󱨵���
	TUstpFtdcVolumeType	DailyMaxOrder;
	///ÿ����󳷵���
	TUstpFtdcVolumeType	DailyMaxOrderAction;
	///ÿ��������
	TUstpFtdcVolumeType	DailyMaxErrorOrder;
	///ÿ����󱨵���
	TUstpFtdcVolumeType	DailyMaxOrderVolume;
	///ÿ����󳷵���
	TUstpFtdcVolumeType	DailyMaxOrderActionVolume;
};
///�û���ѯ
struct CUstpFtdcQryUserField
{
	///�����û�����
	TUstpFtdcUserIDType	StartUserID;
	///�����û�����
	TUstpFtdcUserIDType	EndUserID;
};
///�û�
struct CUstpFtdcUserField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///�û���¼����
	TUstpFtdcPasswordType	Password;
	///�Ƿ��Ծ
	TUstpFtdcIsActiveType	IsActive;
	///�û�����
	TUstpFtdcUserNameType	UserName;
	///�û�����
	TUstpFtdcUserTypeType	UserType;
	///Ӫҵ��
	TUstpFtdcDepartmentType	Department;
	///��Ȩ���ܼ�
	TUstpFtdcGrantFuncSetType	GrantFuncSet;
	///�޸��û����
	TUstpFtdcUserIDType	SetUserID;
	///��������
	TUstpFtdcDateType	CommandDate;
	///����ʱ��
	TUstpFtdcTimeType	CommandTime;
};
///Ͷ�����������ʲ�ѯ
struct CUstpFtdcQryInvestorFeeField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
};
///Ͷ������������
struct CUstpFtdcInvestorFeeField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�ͻ���
	TUstpFtdcClientIDType	ClientID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
	///Ʒ�ִ���
	TUstpFtdcProductIDType	ProductID;
	///���������Ѱ�����
	TUstpFtdcRatioType	OpenFeeRate;
	///���������Ѱ�����
	TUstpFtdcRatioType	OpenFeeAmt;
	///ƽ�������Ѱ�����
	TUstpFtdcRatioType	OffsetFeeRate;
	///ƽ�������Ѱ�����
	TUstpFtdcRatioType	OffsetFeeAmt;
	///ƽ��������Ѱ�����
	TUstpFtdcRatioType	OTFeeRate;
	///ƽ��������Ѱ�����
	TUstpFtdcRatioType	OTFeeAmt;
	///��Ȩ�����Ѱ�����
	TUstpFtdcRatioType	ExecFeeRate;
	///��Ȩ�����Ѱ�����
	TUstpFtdcRatioType	ExecFeeAmt;
	///ÿ��ί���걨��
	TUstpFtdcRatioType	PerOrderAmt;
	///ÿ�ʳ����걨��
	TUstpFtdcRatioType	PerCancelAmt;
	///Ͷ���ױ���־
	TUstpFtdcHedgeFlagType	HedgeFlag;
};
///Ͷ���߱�֤���ʲ�ѯ
struct CUstpFtdcQryInvestorMarginField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
};
///Ͷ���߱�֤����
struct CUstpFtdcInvestorMarginField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�ͻ���
	TUstpFtdcClientIDType	ClientID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
	///Ʒ�ִ���
	TUstpFtdcProductIDType	ProductID;
	///��ͷռ�ñ�֤�𰴱���
	TUstpFtdcRatioType	LongMarginRate;
	///��ͷ��֤������
	TUstpFtdcRatioType	LongMarginAmt;
	///��ͷռ�ñ�֤�𰴱���
	TUstpFtdcRatioType	ShortMarginRate;
	///��ͷ��֤������
	TUstpFtdcRatioType	ShortMarginAmt;
	///Ͷ���ױ���־
	TUstpFtdcHedgeFlagType	HedgeFlag;
};
///ȷ�϶��˵�
struct CUstpFtdcInputSettlementInfoConfirmField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///ȷ������
	TUstpFtdcDateType	ConfirmDate;
	///ȷ��ʱ��
	TUstpFtdcTimeType	ConfirmTime;
};
///ȷ�϶��˵���Ϣ
struct CUstpFtdcSettlementInfoConfirmField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///ȷ������
	TUstpFtdcDateType	ConfirmDate;
	///ȷ��ʱ��
	TUstpFtdcTimeType	ConfirmTime;
};
///��ѯȷ�϶��˵���Ϣ
struct CUstpFtdcQrySettlementInfoConfirmField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
};
///�ɽ�
struct CUstpFtdcTradeField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///������
	TUstpFtdcTradingDayType	TradingDay;
	///��Ա���
	TUstpFtdcParticipantIDType	ParticipantID;
	///�µ�ϯλ��
	TUstpFtdcSeatIDType	SeatID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///�ͻ���
	TUstpFtdcClientIDType	ClientID;
	///�û����
	TUstpFtdcUserIDType	UserID;
	///�µ��û����
	TUstpFtdcUserIDType	OrderUserID;
	///�ɽ����
	TUstpFtdcTradeIDType	TradeID;
	///�������
	TUstpFtdcOrderSysIDType	OrderSysID;
	///���ر������
	TUstpFtdcUserOrderLocalIDType	UserOrderLocalID;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
	///��������
	TUstpFtdcDirectionType	Direction;
	///��ƽ��־
	TUstpFtdcOffsetFlagType	OffsetFlag;
	///Ͷ���ױ���־
	TUstpFtdcHedgeFlagType	HedgeFlag;
	///�ɽ��۸�
	TUstpFtdcPriceType	TradePrice;
	///�ɽ�����
	TUstpFtdcVolumeType	TradeVolume;
	///�ɽ�ʱ��
	TUstpFtdcTimeType	TradeTime;
	///�����Ա���
	TUstpFtdcParticipantIDType	ClearingPartID;
	///���γɽ�������
	TUstpFtdcMoneyType	UsedFee;
	///���γɽ�ռ�ñ�֤��
	TUstpFtdcMoneyType	UsedMargin;
	///���γɽ�ռ��Ȩ����
	TUstpFtdcMoneyType	Premium;
	///�ֱֲ��ֲ���
	TUstpFtdcVolumeType	Position;
	///�ֱֲ���ճֲֳɱ�
	TUstpFtdcPriceType	PositionCost;
	///�ʽ������ʽ�
	TUstpFtdcMoneyType	Available;
	///�ʽ��ռ�ñ�֤��
	TUstpFtdcMoneyType	Margin;
	///�ʽ����ı�֤��
	TUstpFtdcMoneyType	FrozenMargin;
	///����ҵ���ʶ
	TUstpFtdcBusinessLocalIDType	BusinessLocalID;
	///ҵ��������
	TUstpFtdcDateType	ActionDay;
	///�������
	TUstpFtdcArbiTypeType	ArbiType;
	///��Լ����
	TUstpFtdcInstrumentIDType	ArbiInstrumentID;
};
///����
struct CUstpFtdcOrderField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///ϵͳ�������
	TUstpFtdcOrderSysIDType	OrderSysID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///ָ��ͨ����ϯλ����µ�
	TUstpFtdcSeatNoType	SeatNo;
	///��Լ����/������Ϻ�Լ��
	TUstpFtdcInstrumentIDType	InstrumentID;
	///�û����ر�����
	TUstpFtdcUserOrderLocalIDType	UserOrderLocalID;
	///��������
	TUstpFtdcOrderPriceTypeType	OrderPriceType;
	///��������
	TUstpFtdcDirectionType	Direction;
	///��ƽ��־
	TUstpFtdcOffsetFlagType	OffsetFlag;
	///Ͷ���ױ���־
	TUstpFtdcHedgeFlagType	HedgeFlag;
	///�۸�
	TUstpFtdcPriceType	LimitPrice;
	///����
	TUstpFtdcVolumeType	Volume;
	///��Ч������
	TUstpFtdcTimeConditionType	TimeCondition;
	///GTD����
	TUstpFtdcDateType	GTDDate;
	///�ɽ�������
	TUstpFtdcVolumeConditionType	VolumeCondition;
	///��С�ɽ���
	TUstpFtdcVolumeType	MinVolume;
	///ֹ���ֹӮ��
	TUstpFtdcPriceType	StopPrice;
	///ǿƽԭ��
	TUstpFtdcForceCloseReasonType	ForceCloseReason;
	///�Զ������־
	TUstpFtdcBoolType	IsAutoSuspend;
	///ҵ��Ԫ
	TUstpFtdcBusinessUnitType	BusinessUnit;
	///�û��Զ�����
	TUstpFtdcCustomType	UserCustom;
	///����ҵ���ʶ
	TUstpFtdcBusinessLocalIDType	BusinessLocalID;
	///ҵ��������
	TUstpFtdcDateType	ActionDay;
	///�������
	TUstpFtdcArbiTypeType	ArbiType;
	///������
	TUstpFtdcTradingDayType	TradingDay;
	///��Ա���
	TUstpFtdcParticipantIDType	ParticipantID;
	///����µ��û����
	TUstpFtdcUserIDType	OrderUserID;
	///�ͻ���
	TUstpFtdcClientIDType	ClientID;
	///�µ�ϯλ��
	TUstpFtdcSeatIDType	SeatID;
	///����ʱ��
	TUstpFtdcTimeType	InsertTime;
	///���ر������
	TUstpFtdcOrderLocalIDType	OrderLocalID;
	///������Դ
	TUstpFtdcOrderSourceType	OrderSource;
	///����״̬
	TUstpFtdcOrderStatusType	OrderStatus;
	///����ʱ��
	TUstpFtdcTimeType	CancelTime;
	///�����û����
	TUstpFtdcUserIDType	CancelUserID;
	///��ɽ�����
	TUstpFtdcVolumeType	VolumeTraded;
	///ʣ������
	TUstpFtdcVolumeType	VolumeRemain;
	///ί������
	TUstpFtdcOrderTypeType	OrderType;
	///��Ȩ�Գ��ʶ
	TUstpFtdcDeliveryFlagType	DeliveryFlag;
};
///����������
struct CUstpFtdcFlowMessageCancelField
{
	///����ϵ�к�
	TUstpFtdcSequenceSeriesType	SequenceSeries;
	///������
	TUstpFtdcDateType	TradingDay;
	///�������Ĵ���
	TUstpFtdcDataCenterIDType	DataCenterID;
	///������ʼ���к�
	TUstpFtdcSequenceNoType	StartSequenceNo;
	///���˽������к�
	TUstpFtdcSequenceNoType	EndSequenceNo;
};
///��Ϣ�ַ�
struct CUstpFtdcDisseminationField
{
	///����ϵ�к�
	TUstpFtdcSequenceSeriesType	SequenceSeries;
	///���к�
	TUstpFtdcSequenceNoType	SequenceNo;
};
///�������
struct CUstpFtdcInvestorAccountDepositResField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///�ʽ��ʺ�
	TUstpFtdcAccountIDType	AccountID;
	///�ʽ���ˮ��
	TUstpFtdcAccountSeqNoType	AccountSeqNo;
	///���
	TUstpFtdcMoneyType	Amount;
	///�������
	TUstpFtdcAccountDirectionType	AmountDirection;
	///�����ʽ�
	TUstpFtdcMoneyType	Available;
	///����׼����
	TUstpFtdcMoneyType	Balance;
};
///����¼��
struct CUstpFtdcInputQuoteField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
	///��������
	TUstpFtdcDirectionType	Direction;
	///����ϵͳ���ص�ϵͳ���۱��
	TUstpFtdcQuoteSysIDType	QuoteSysID;
	///�û��趨�ı��ر��۱��
	TUstpFtdcUserQuoteLocalIDType	UserQuoteLocalID;
	///��������ϵͳ���ı��ر��۱��
	TUstpFtdcQuoteLocalIDType	QuoteLocalID;
	///����������
	TUstpFtdcVolumeType	BidVolume;
	///�򷽿�ƽ��־
	TUstpFtdcOffsetFlagType	BidOffsetFlag;
	///��Ͷ���ױ���־
	TUstpFtdcHedgeFlagType	BidHedgeFlag;
	///������۸�
	TUstpFtdcPriceType	BidPrice;
	///������������
	TUstpFtdcVolumeType	AskVolume;
	///������ƽ��־
	TUstpFtdcOffsetFlagType	AskOffsetFlag;
	///����Ͷ���ױ���־
	TUstpFtdcHedgeFlagType	AskHedgeFlag;
	///���������۸�
	TUstpFtdcPriceType	AskPrice;
	///ҵ��Ԫ
	TUstpFtdcBusinessUnitType	BusinessUnit;
	///�û��Զ�����
	TUstpFtdcCustomType	UserCustom;
	///��ֳ��������û����ر������
	TUstpFtdcUserOrderLocalIDType	BidUserOrderLocalID;
	///��ֳ����������û����ر������
	TUstpFtdcUserOrderLocalIDType	AskUserOrderLocalID;
	///��ֳ������򷽱��ر������
	TUstpFtdcOrderLocalIDType	BidOrderLocalID;
	///��ֳ������������ر������
	TUstpFtdcOrderLocalIDType	AskOrderLocalID;
	///ѯ�۱��
	TUstpFtdcQuoteSysIDType	ReqForQuoteID;
	///����ͣ��ʱ��(��)
	TUstpFtdcMeasureSecType	StandByTime;
};
///����֪ͨ
struct CUstpFtdcRtnQuoteField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
	///��������
	TUstpFtdcDirectionType	Direction;
	///����ϵͳ���ص�ϵͳ���۱��
	TUstpFtdcQuoteSysIDType	QuoteSysID;
	///�û��趨�ı��ر��۱��
	TUstpFtdcUserQuoteLocalIDType	UserQuoteLocalID;
	///��������ϵͳ���ı��ر��۱��
	TUstpFtdcQuoteLocalIDType	QuoteLocalID;
	///����������
	TUstpFtdcVolumeType	BidVolume;
	///�򷽿�ƽ��־
	TUstpFtdcOffsetFlagType	BidOffsetFlag;
	///��Ͷ���ױ���־
	TUstpFtdcHedgeFlagType	BidHedgeFlag;
	///������۸�
	TUstpFtdcPriceType	BidPrice;
	///������������
	TUstpFtdcVolumeType	AskVolume;
	///������ƽ��־
	TUstpFtdcOffsetFlagType	AskOffsetFlag;
	///����Ͷ���ױ���־
	TUstpFtdcHedgeFlagType	AskHedgeFlag;
	///���������۸�
	TUstpFtdcPriceType	AskPrice;
	///ҵ��Ԫ
	TUstpFtdcBusinessUnitType	BusinessUnit;
	///�û��Զ�����
	TUstpFtdcCustomType	UserCustom;
	///��ֳ��������û����ر������
	TUstpFtdcUserOrderLocalIDType	BidUserOrderLocalID;
	///��ֳ����������û����ر������
	TUstpFtdcUserOrderLocalIDType	AskUserOrderLocalID;
	///��ֳ������򷽱��ر������
	TUstpFtdcOrderLocalIDType	BidOrderLocalID;
	///��ֳ������������ر������
	TUstpFtdcOrderLocalIDType	AskOrderLocalID;
	///ѯ�۱��
	TUstpFtdcQuoteSysIDType	ReqForQuoteID;
	///����ͣ��ʱ��(��)
	TUstpFtdcMeasureSecType	StandByTime;
	///��ϵͳ�������
	TUstpFtdcQuoteSysIDType	BidOrderSysID;
	///����ϵͳ�������
	TUstpFtdcQuoteSysIDType	AskOrderSysID;
	///���۵�״̬
	TUstpFtdcQuoteStatusType	QuoteStatus;
	///����ʱ��
	TUstpFtdcTimeType	InsertTime;
	///����ʱ��
	TUstpFtdcTimeType	CancelTime;
	///�ɽ�ʱ��
	TUstpFtdcTimeType	TradeTime;
};
///���۲���
struct CUstpFtdcQuoteActionField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///����ϵͳ���ص�ϵͳ���۱��
	TUstpFtdcQuoteSysIDType	QuoteSysID;
	///�û��趨�ı����ı��ر��۱��
	TUstpFtdcUserQuoteLocalIDType	UserQuoteLocalID;
	///�û�������ı��س������۱��
	TUstpFtdcUserQuoteLocalIDType	UserQuoteActionLocalID;
	///����������־
	TUstpFtdcActionFlagType	ActionFlag;
	///ҵ��Ԫ
	TUstpFtdcBusinessUnitType	BusinessUnit;
	///�û��Զ�����
	TUstpFtdcCustomType	UserCustom;
	///��������
	TUstpFtdcDirectionType	Direction;
};
///ѯ������
struct CUstpFtdcReqForQuoteField
{
	///ѯ�۱��
	TUstpFtdcQuoteSysIDType	ReqForQuoteID;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
	///��������
	TUstpFtdcDirectionType	Direction;
	///������
	TUstpFtdcDateType	TradingDay;
	///ѯ��ʱ��
	TUstpFtdcTimeType	ReqForQuoteTime;
};
///�����������
struct CUstpFtdcMarketDataBaseField
{
	///������
	TUstpFtdcDateType	TradingDay;
	///���������
	TUstpFtdcSettlementGroupIDType	SettlementGroupID;
	///������
	TUstpFtdcSettlementIDType	SettlementID;
	///�����
	TUstpFtdcPriceType	PreSettlementPrice;
	///������
	TUstpFtdcPriceType	PreClosePrice;
	///��ֲ���
	TUstpFtdcLargeVolumeType	PreOpenInterest;
	///����ʵ��
	TUstpFtdcRatioType	PreDelta;
};
///���龲̬����
struct CUstpFtdcMarketDataStaticField
{
	///����
	TUstpFtdcPriceType	OpenPrice;
	///��߼�
	TUstpFtdcPriceType	HighestPrice;
	///��ͼ�
	TUstpFtdcPriceType	LowestPrice;
	///������
	TUstpFtdcPriceType	ClosePrice;
	///��ͣ���
	TUstpFtdcPriceType	UpperLimitPrice;
	///��ͣ���
	TUstpFtdcPriceType	LowerLimitPrice;
	///�����
	TUstpFtdcPriceType	SettlementPrice;
	///����ʵ��
	TUstpFtdcRatioType	CurrDelta;
};
///�������³ɽ�����
struct CUstpFtdcMarketDataLastMatchField
{
	///���¼�
	TUstpFtdcPriceType	LastPrice;
	///����
	TUstpFtdcVolumeType	Volume;
	///�ɽ����
	TUstpFtdcMoneyType	Turnover;
	///�ֲ���
	TUstpFtdcLargeVolumeType	OpenInterest;
};
///�������ż�����
struct CUstpFtdcMarketDataBestPriceField
{
	///�����һ
	TUstpFtdcPriceType	BidPrice1;
	///������һ
	TUstpFtdcVolumeType	BidVolume1;
	///������һ
	TUstpFtdcPriceType	AskPrice1;
	///������һ
	TUstpFtdcVolumeType	AskVolume1;
};
///�����������������
struct CUstpFtdcMarketDataBid23Field
{
	///����۶�
	TUstpFtdcPriceType	BidPrice2;
	///��������
	TUstpFtdcVolumeType	BidVolume2;
	///�������
	TUstpFtdcPriceType	BidPrice3;
	///��������
	TUstpFtdcVolumeType	BidVolume3;
};
///������������������
struct CUstpFtdcMarketDataAsk23Field
{
	///�����۶�
	TUstpFtdcPriceType	AskPrice2;
	///��������
	TUstpFtdcVolumeType	AskVolume2;
	///��������
	TUstpFtdcPriceType	AskPrice3;
	///��������
	TUstpFtdcVolumeType	AskVolume3;
};
///���������ġ�������
struct CUstpFtdcMarketDataBid45Field
{
	///�������
	TUstpFtdcPriceType	BidPrice4;
	///��������
	TUstpFtdcVolumeType	BidVolume4;
	///�������
	TUstpFtdcPriceType	BidPrice5;
	///��������
	TUstpFtdcVolumeType	BidVolume5;
};
///���������ġ�������
struct CUstpFtdcMarketDataAsk45Field
{
	///��������
	TUstpFtdcPriceType	AskPrice4;
	///��������
	TUstpFtdcVolumeType	AskVolume4;
	///��������
	TUstpFtdcPriceType	AskPrice5;
	///��������
	TUstpFtdcVolumeType	AskVolume5;
};
///�������ʱ������
struct CUstpFtdcMarketDataUpdateTimeField
{
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
	///����޸�ʱ��
	TUstpFtdcTimeType	UpdateTime;
	///����޸ĺ���
	TUstpFtdcMillisecType	UpdateMillisec;
	///ҵ��������
	TUstpFtdcDateType	ActionDay;
};
///�������
struct CUstpFtdcDepthMarketDataField
{
	///������
	TUstpFtdcDateType	TradingDay;
	///���������
	TUstpFtdcSettlementGroupIDType	SettlementGroupID;
	///������
	TUstpFtdcSettlementIDType	SettlementID;
	///�����
	TUstpFtdcPriceType	PreSettlementPrice;
	///������
	TUstpFtdcPriceType	PreClosePrice;
	///��ֲ���
	TUstpFtdcLargeVolumeType	PreOpenInterest;
	///����ʵ��
	TUstpFtdcRatioType	PreDelta;
	///����
	TUstpFtdcPriceType	OpenPrice;
	///��߼�
	TUstpFtdcPriceType	HighestPrice;
	///��ͼ�
	TUstpFtdcPriceType	LowestPrice;
	///������
	TUstpFtdcPriceType	ClosePrice;
	///��ͣ���
	TUstpFtdcPriceType	UpperLimitPrice;
	///��ͣ���
	TUstpFtdcPriceType	LowerLimitPrice;
	///�����
	TUstpFtdcPriceType	SettlementPrice;
	///����ʵ��
	TUstpFtdcRatioType	CurrDelta;
	///���¼�
	TUstpFtdcPriceType	LastPrice;
	///����
	TUstpFtdcVolumeType	Volume;
	///�ɽ����
	TUstpFtdcMoneyType	Turnover;
	///�ֲ���
	TUstpFtdcLargeVolumeType	OpenInterest;
	///�����һ
	TUstpFtdcPriceType	BidPrice1;
	///������һ
	TUstpFtdcVolumeType	BidVolume1;
	///������һ
	TUstpFtdcPriceType	AskPrice1;
	///������һ
	TUstpFtdcVolumeType	AskVolume1;
	///����۶�
	TUstpFtdcPriceType	BidPrice2;
	///��������
	TUstpFtdcVolumeType	BidVolume2;
	///�������
	TUstpFtdcPriceType	BidPrice3;
	///��������
	TUstpFtdcVolumeType	BidVolume3;
	///�����۶�
	TUstpFtdcPriceType	AskPrice2;
	///��������
	TUstpFtdcVolumeType	AskVolume2;
	///��������
	TUstpFtdcPriceType	AskPrice3;
	///��������
	TUstpFtdcVolumeType	AskVolume3;
	///�������
	TUstpFtdcPriceType	BidPrice4;
	///��������
	TUstpFtdcVolumeType	BidVolume4;
	///�������
	TUstpFtdcPriceType	BidPrice5;
	///��������
	TUstpFtdcVolumeType	BidVolume5;
	///��������
	TUstpFtdcPriceType	AskPrice4;
	///��������
	TUstpFtdcVolumeType	AskVolume4;
	///��������
	TUstpFtdcPriceType	AskPrice5;
	///��������
	TUstpFtdcVolumeType	AskVolume5;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
	///����޸�ʱ��
	TUstpFtdcTimeType	UpdateTime;
	///����޸ĺ���
	TUstpFtdcMillisecType	UpdateMillisec;
	///ҵ��������
	TUstpFtdcDateType	ActionDay;
	///��ʷ��߼�
	TUstpFtdcPriceType	HisHighestPrice;
	///��ʷ��ͼ�
	TUstpFtdcPriceType	HisLowestPrice;
	///���³ɽ���
	TUstpFtdcVolumeType	LatestVolume;
	///��ʼ�ֲ���
	TUstpFtdcVolumeType	InitVolume;
	///�ֲ����仯
	TUstpFtdcVolumeType	ChangeVolume;
	///�����Ƶ���
	TUstpFtdcVolumeType	BidImplyVolume;
	///�����Ƶ���
	TUstpFtdcVolumeType	AskImplyVolume;
	///���վ���
	TUstpFtdcPriceType	AvgPrice;
	///�������
	TUstpFtdcArbiTypeType	ArbiType;
	///��һ�Ⱥ�Լ����
	TUstpFtdcInstrumentIDType	InstrumentID_1;
	///�ڶ��Ⱥ�Լ����
	TUstpFtdcInstrumentIDType	InstrumentID_2;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentName;
	///��������
	TUstpFtdcVolumeType	TotalBidVolume;
	///��������
	TUstpFtdcVolumeType	TotalAskVolume;
};
///���ĺ�Լ�������Ϣ
struct CUstpFtdcSpecificInstrumentField
{
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
};
///�ಥͨ������
struct CUstpFtdcMultiChannelHeartBeatField
{
	///������ʱʱ�䣨�룩
	TUstpFtdcVolumeType	UstpMultiChannelHeartBeatTimeOut;
};
///��ȡ���鶩�ĺ�����
struct CUstpFtdcReqMarketTopicField
{
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
};
///��ȡ���鶩�ĺ�Ӧ��
struct CUstpFtdcRspMarketTopicField
{
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///���ĺ�
	TUstpFtdcSequenceSeriesType	TopicID;
};
///�������
struct CUstpFtdcInputMarginCombActionField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///�����û�����
	TUstpFtdcUserIDType	UserID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///Ͷ���ױ���־
	TUstpFtdcHedgeFlagType	HedgeFlag;
	///�û����ر��
	TUstpFtdcUserOrderLocalIDType	UserActionLocalID;
	///��Ϻ�Լ����
	TUstpFtdcInstrumentIDType	CombInstrumentID;
	///�������
	TUstpFtdcVolumeType	CombVolume;
	///��϶�������
	TUstpFtdcCombDirectionType	CombDirection;
	///���ر��
	TUstpFtdcOrderLocalIDType	ActionLocalID;
};
///���ױ�����ϳֲֲ�ѯ
struct CUstpFtdcQryInvestorCombPositionField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///Ͷ���ױ���־
	TUstpFtdcHedgeFlagType	HedgeFlag;
	///��Ϻ�Լ����
	TUstpFtdcInstrumentIDType	CombInstrumentID;
};
///���ױ�����ϳֲֲ�ѯӦ��
struct CUstpFtdcRspInvestorCombPositionField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///�ֲַ���
	TUstpFtdcDirectionType	Direction;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///Ͷ���ױ���־
	TUstpFtdcHedgeFlagType	HedgeFlag;
	///�ͻ�����
	TUstpFtdcClientIDType	ClientID;
	///��Ϻ�Լ����
	TUstpFtdcInstrumentIDType	CombInstrumentID;
	///����1��Լ����
	TUstpFtdcInstrumentIDType	Leg1InstrumentID;
	///����2��Լ����
	TUstpFtdcInstrumentIDType	Leg2InstrumentID;
	///��ϳֲ�
	TUstpFtdcVolumeType	CombPosition;
	///������ϳֲ�
	TUstpFtdcVolumeType	CombFrozenPosition;
	///���һ���ͷŵı�֤��
	TUstpFtdcMoneyType	CombFreeMargin;
};
///��Ϲ���
struct CUstpFtdcMarginCombinationLegField
{
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///��Ϻ�Լ����
	TUstpFtdcInstrumentIDType	CombInstrumentID;
	///���ȱ��
	TUstpFtdcLegIDType	LegID;
	///���Ⱥ�Լ����
	TUstpFtdcInstrumentIDType	LegInstrumentID;
	///��������
	TUstpFtdcDirectionType	Direction;
	///���ȳ���
	TUstpFtdcLegMultipleType	LegMultiple;
	///���ȼ�
	TUstpFtdcPriorityType	Priority;
};
///���ױ��뵥�ȳֲֲ�ѯ
struct CUstpFtdcQryInvestorLegPositionField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///Ͷ���ױ���־
	TUstpFtdcHedgeFlagType	HedgeFlag;
	///���Ⱥ�Լ����
	TUstpFtdcInstrumentIDType	LegInstrumentID;
};
///���ױ��뵥�ȳֲֲ�ѯӦ��
struct CUstpFtdcRspInvestorLegPositionField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///Ͷ���ױ���־
	TUstpFtdcHedgeFlagType	HedgeFlag;
	///�ͻ�����
	TUstpFtdcClientIDType	ClientID;
	///���Ⱥ�Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
	///��ͷ�ֲ�
	TUstpFtdcVolumeType	LongPosition;
	///��ͷ�ֲ�
	TUstpFtdcVolumeType	ShortPosition;
	///��ͷռ�ñ�֤��
	TUstpFtdcMoneyType	LongMargin;
	///��ͷռ�ñ�֤��
	TUstpFtdcMoneyType	ShortMargin;
	///��ͷ����ֲ�
	TUstpFtdcVolumeType	LongFrozenPosition;
	///��ͷ����ֲ�
	TUstpFtdcVolumeType	ShortFrozenPosition;
	///��ͷ���ᱣ֤��
	TUstpFtdcMoneyType	LongFrozenMargin;
	///��ͷ���ᱣ֤��
	TUstpFtdcMoneyType	ShortFrozenMargin;
};
///��ѯ��Լ����Ϣ
struct CUstpFtdcQryUstpInstrumentGroupField
{
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
};
///��Լ����Ϣ��ѯӦ��
struct CUstpFtdcRspInstrumentGroupField
{
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
	///��Լ�����
	TUstpFtdcInstrumentGroupIDType	InstrumentGroupID;
};
///��ѯ��ϱ�֤������
struct CUstpFtdcQryClientMarginCombTypeField
{
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///Ͷ���ױ���־
	TUstpFtdcHedgeFlagType	HedgeFlag;
	///��Լ�����
	TUstpFtdcInstrumentGroupIDType	InstrumentGroupID;
};
///��ϱ�֤�����Ͳ�ѯӦ��
struct CUstpFtdcRspClientMarginCombTypeField
{
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///��Ա����
	TUstpFtdcParticipantIDType	ParticipantID;
	///�ͻ�����
	TUstpFtdcClientIDType	ClientID;
	///��Լ�����
	TUstpFtdcInstrumentGroupIDType	InstrumentGroupID;
	///��֤���������
	TUstpFtdcClientMarginCombTypeType	MarginCombType;
};
///ϵͳʱ��
struct CUstpFtdcReqQrySystemTimeField
{
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
};
///ϵͳʱ��
struct CUstpFtdcRspQrySystemTimeField
{
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///ϵͳʱ��
	TUstpFtdcTimeType	SystemTime;
};
///��ص�¼����
struct CUstpFtdcReqRiskLoginField
{
	///��ش���
	TUstpFtdcRiskIDType	RiskID;
	///����
	TUstpFtdcPasswordType	Password;
};
///��ص�¼Ӧ��
struct CUstpFtdcRspRiskLoginField
{
	///��ش���
	TUstpFtdcRiskIDType	RiskID;
	///����
	TUstpFtdcPasswordType	Password;
};
///��صǳ�����
struct CUstpFtdcReqRiskLogoutField
{
	///��ش���
	TUstpFtdcRiskIDType	RiskID;
};
///��صǳ�����
struct CUstpFtdcRspRiskLogoutField
{
	///��ش���
	TUstpFtdcRiskIDType	RiskID;
};
///Ͷ���߽���Ȩ�޲�ѯ
struct CUstpFtdcQryInvestorTradingRightField
{
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
};
///Ͷ���߽���Ȩ��
struct CUstpFtdcInvestorTradingRightField
{
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///����Ȩ��
	TUstpFtdcTradingRightType	TradingRight;
};
///ϯλ�����ʽ��ѯ
struct CUstpFtdcQrySeatAvailableMoneyField
{
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///ϯλ��
	TUstpFtdcSeatIDType	SeatID;
};
///ϯλ�����ʽ�
struct CUstpFtdcSeatAvailableMoneyField
{
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///ϯλ��
	TUstpFtdcSeatIDType	SeatID;
	///ϯλ�����ʽ�
	TUstpFtdcMoneyType	Available;
};
///������ʾ��Ϣ
struct CUstpFtdcRiskWarningField
{
	///��Ϣ���
	TUstpFtdcRiskWarningNoType	RiskWarningNo;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///��Ϣ����
	TUstpFtdcRiskWarningTypeType	RiskWarningType;
	///��ȷ�ϱ�־
	TUstpFtdcBoolType	NeedConfirmation;
	///��ϢժҪ
	TUstpFtdcRiskWarningAbstractType	Abstract;
	///��Ϣ����
	TUstpFtdcRiskWarningContentType	Content;
};
///������ʾ��ϢӦ��
struct CUstpFtdcRspRiskWarningField
{
	///��Ϣ���
	TUstpFtdcRiskWarningNoType	RiskWarningNo;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
};
///������ʾ��Ϣȷ��
struct CUstpFtdcRiskWarningConfirmationField
{
	///��Ϣ���
	TUstpFtdcRiskWarningNoType	RiskWarningNo;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///IP��ַ
	TUstpFtdcIPAddressType	IPAddress;
	///Mac��ַ
	TUstpFtdcMacAddressType	MacAddress;
};
///��֤����ϵͳ���͹�˾�ʽ��˻���Կ����
struct CUstpFtdcCFMMCTradingAccountKeyReqField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
};
///��֤����ϵͳ���͹�˾�ʽ��˻���ԿӦ��
struct CUstpFtdcCFMMCTradingAccountKeyRspField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///��Ա���
	TUstpFtdcParticipantIDType	ParticipantID;
	///�ʽ��ʺ�
	TUstpFtdcAccountIDType	AccountID;
	///��Կ���
	TUstpFtdcSequenceNoTypeType	KeyID;
	///��̬��Կ
	TUstpFtdcCFMMCKeyType	CurrentKey;
};
///��ѯͶ���߽���������
struct CUstpFtdcSettlementQryReqField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///������
	TUstpFtdcDateType	TradingDay;
};
///��ѯͶ���߽�����Ӧ��
struct CUstpFtdcSettlementRspField
{
	///������
	TUstpFtdcDateType	TradingDay;
	///������
	TUstpFtdcSettlementIDType	SettlementID;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///���
	TUstpFtdcSequenceNoType	SequenceNo;
	///��Ϣ����
	TUstpFtdcLogStrType	Content;
};
///�ʽ��˻������������
struct CUstpFtdcTradingAccountPasswordUpdateReqField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///������
	TUstpFtdcPasswordType	OldPassword;
	///������
	TUstpFtdcPasswordType	NewPassword;
};
///�ʽ��˻��������������Ӧ
struct CUstpFtdcTradingAccountPasswordUpdateRspField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///������
	TUstpFtdcPasswordType	OldPassword;
	///������
	TUstpFtdcPasswordType	NewPassword;
};
///��ѯת����������
struct CUstpFtdcTransferBankQryReqField
{
	///���д���
	TUstpFtdcBrokerIDType	BankID;
	///���з����Ĵ���
	TUstpFtdcInvestorIDType	BankBrchID;
};
///��ѯת��������Ӧ
struct CUstpFtdcTransferBankQryRspField
{
	///���д���
	TUstpFtdcBrokerIDType	BankID;
	///���з����Ĵ���
	TUstpFtdcInvestorIDType	BankBrchID;
	///��������
	TUstpFtdcBankNameType	BankName;
	///�Ƿ��Ծ
	TUstpFtdcBoolType	isActive;
};
///��ѯ�ֵ��۵���Ϣ����
struct CUstpFtdcEWarrantOffsetFieldReqField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
};
///��ѯ�ֵ��۵���Ϣ��Ӧ
struct CUstpFtdcEWarrantOffsetFieldRspField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
	///��������
	TUstpFtdcDirectionType	Direction;
	///Ͷ���ױ���־
	TUstpFtdcHedgeFlagType	HedgeFlag;
	///����
	TUstpFtdcVolumeType	Volume;
};
///��ѯת����ˮ����
struct CUstpFtdcTransferSerialFieldReqField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�ʽ��ʺ�
	TUstpFtdcAccountIDType	AccountID;
	///���д���
	TUstpFtdcBrokerIDType	BankID;
};
///��ѯת����ˮ��Ӧ��Ӧ
struct CUstpFtdcTransferSerialFieldRspField
{
	///ƽ̨��ˮ��
	TUstpFtdcPlateSerialType	PlateSerial;
	///���׷�������
	TUstpFtdcDateType	TradeDate;
	///��������
	TUstpFtdcDateType	TradingDay;
	///����ʱ��
	TUstpFtdcTimeType	TradeTime;
	///���״���
	TUstpFtdcTradeCodeType	TradeCode;
	///�Ự���
	TUstpFtdcSessionIDType	SessionID;
	///���д���
	TUstpFtdcBrokerIDType	BankID;
	///���з����Ĵ���
	TUstpFtdcInvestorIDType	BankBrchID;
	///�����ʺ�����
	TUstpFtdcBankAccType	BankAccType;
	///�����ʺ�
	TUstpFtdcBankAccountType	BankAccount;
	///������ˮ��
	TUstpFtdcBankSerialType	BankSerial;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TUstpFtdcFutureBranchIDType	BrokerBranchID;
	///�ڻ���˾�ʺ�����
	TUstpFtdcFutureAccType	FutureAccType;
	///�ʽ��ʺ�
	TUstpFtdcAccountIDType	AccountID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///�ڻ���˾��ˮ��
	TUstpFtdcFutureSerialType	FutureSerial;
	///֤������
	TUstpFtdcIdCardTypeType	IdCardType;
	///֤������
	TUstpFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///���ִ���
	TUstpFtdcCurrencyIDType	CurrencyID;
	///���
	TUstpFtdcMoneyType	TradeAmount;
	///Ӧ�տͻ�����
	TUstpFtdcMoneyType	CustFee;
	///Ӧ���ڻ���˾����
	TUstpFtdcMoneyType	BrokerFee;
	///��Ч��־
	TUstpFtdcAvailabilityFlagTypeType	AvailabilityFlag;
	///����Ա
	TUstpFtdcOperatorCodeType	OperatorCode;
	///�������ʺ�
	TUstpFtdcBankAccountType	BankNewAccount;
};
///��ѯ����ǩԼ��ϵ
struct CUstpFtdcAccountregisterReqField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�ʽ��ʺ�
	TUstpFtdcAccountIDType	AccountID;
	///���д���
	TUstpFtdcBrokerIDType	BankID;
};
///��ѯ����ǩԼ��ϵ��Ӧ
struct CUstpFtdcAccountregisterRspField
{
	///��������
	TUstpFtdcDateType	TradeDate;
	///���д���
	TUstpFtdcBrokerIDType	BankID;
	///���з����Ĵ���
	TUstpFtdcInvestorIDType	BankBrchID;
	///�����ʺ�
	TUstpFtdcBankAccountType	BankAccount;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///�ʽ��ʺ�
	TUstpFtdcAccountIDType	AccountID;
	///֤������
	TUstpFtdcIdCardTypeType	IdCardType;
	///֤������
	TUstpFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�ͻ�����
	TUstpFtdcIndividualNameType	CustomerName;
	///���ִ���
	TUstpFtdcCurrencyIDType	CurrencyID;
	///���������
	TUstpFtdcOpenOrDestroyTypeType	OpenOrDestroy;
	///ǩԼ����
	TUstpFtdcDateType	RegDate;
	///��Լ����
	TUstpFtdcDateType	OutDate;
	///����ID
	TUstpFtdcTIDType	TID;
	///�ͻ�����
	TUstpFtdcCustTypeType	CustType;
	///�����ʺ�����
	TUstpFtdcBankAccType	BankAccType;
};
///��ѯǩԼ��������
struct CUstpFtdcContractBankFieldReqField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///���д���
	TUstpFtdcBrokerIDType	BankID;
	///���з����Ĵ���
	TUstpFtdcInvestorIDType	BankBrchID;
};
///��ѯǩԼ������Ӧ
struct CUstpFtdcContractBankFieldRspField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///���д���
	TUstpFtdcBrokerIDType	BankID;
	///���з����Ĵ���
	TUstpFtdcInvestorIDType	BankBrchID;
	///��������
	TUstpFtdcBankNameType	BankName;
};
///ת������
struct CUstpFtdcTransferFieldReqField
{
	///ҵ������
	TUstpFtdcTradeCodeType	TradeCode;
	///���д���
	TUstpFtdcBrokerIDType	BankID;
	///���з����Ĵ���
	TUstpFtdcInvestorIDType	BankBrchID;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TUstpFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TUstpFtdcDateType	TradeDate;
	///�ɽ�ʱ��
	TUstpFtdcTimeType	TradeTime;
	///������ˮ��
	TUstpFtdcBankSerialType	BankSerial;
	///������
	TUstpFtdcDateType	TradingDay;
	///ƽ̨��ˮ��
	TUstpFtdcPlateSerialType	PlateSerial;
	///����Ƭ��־
	TUstpFtdcBoolType	LastFragment;
	///��¼��SessionID
	TUstpFtdcSessionIDType	SessionID;
	///�ͻ�����
	TUstpFtdcIndividualNameType	CustomerName;
	///֤������
	TUstpFtdcIdCardTypeType	IdCardType;
	///֤������
	TUstpFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�ͻ�����
	TUstpFtdcCustTypeType	CustType;
	///�����ʺ�
	TUstpFtdcBankAccountType	BankAccount;
	///��������
	TUstpFtdcPasswordType	BankPassWord;
	///�ʽ��ʺ�
	TUstpFtdcAccountIDType	AccountID;
	///�ڻ�����
	TUstpFtdcPasswordType	Password;
	///��װ���
	TUstpFtdcInstallIDType	InstallID;
	///�ڻ���˾��ˮ��
	TUstpFtdcFutureSerialType	FutureSerial;
	///�û���ʶ
	TUstpFtdcUserIDType	UserID;
	///�ͻ�֤�������־
	TUstpFtdcBoolType	VerifyCertNoFlag;
	///���ִ���
	TUstpFtdcCurrencyIDType	CurrencyID;
	///ת�ʽ��
	TUstpFtdcMoneyType	TradeAmount;
	///�ڻ���ȡ���
	TUstpFtdcMoneyType	FutureFetchAmount;
	///����֧����־
	TUstpFtdcFeePayFlagType	FeePayFlag;
	///Ӧ�տͻ�����
	TUstpFtdcMoneyType	CustFee;
	///Ӧ���ڻ���˾����
	TUstpFtdcMoneyType	BrokerFee;
	///���ͷ������շ�����Ϣ
	TUstpFtdcMessageType	Message;
	///ժҪ
	TUstpFtdcMessageType	Digest;
	///�����ʺ�����
	TUstpFtdcBankAccType	BankAccType;
	///������־
	TUstpFtdcDeviceIdType	DeviceID;
	///�ڻ���λ�ʺ�����
	TUstpFtdcBankAccType	BankSecuAccType;
	///�ڻ���˾���б���
	TUstpFtdcBankCodingForFutureType	BrokerIDByBank;
	///�ڻ���λ�ʺ�
	TUstpFtdcBankAccountType	BankSecuAcc;
	///���������־
	TUstpFtdcPwdFlagTypeType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TUstpFtdcPwdFlagTypeType	SecuPwdFlag;
	///���׹�Ա
	TUstpFtdcOperNoType	OperNo;
	///������
	TUstpFtdcRequestIDType	RequestID;
	///����ID
	TUstpFtdcTIDType	TID;
	///ת�˽���״̬
	TUstpFtdcTransferStatusType	TransferStatus;
};
///��ѯ�˻���Ϣ����
struct CUstpFtdcAccountFieldReqField
{
	///ҵ������
	TUstpFtdcTradeCodeType	TradeCode;
	///���д���
	TUstpFtdcBrokerIDType	BankID;
	///���з����Ĵ���
	TUstpFtdcInvestorIDType	BankBrchID;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TUstpFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TUstpFtdcDateType	TradeDate;
	///�ɽ�ʱ��
	TUstpFtdcTimeType	TradeTime;
	///������ˮ��
	TUstpFtdcBankSerialType	BankSerial;
	///������
	TUstpFtdcDateType	TradingDay;
	///ƽ̨��ˮ��
	TUstpFtdcPlateSerialType	PlateSerial;
	///����Ƭ��־
	TUstpFtdcBoolType	LastFragment;
	///��¼��SessionID
	TUstpFtdcSessionIDType	SessionID;
	///�ͻ�����
	TUstpFtdcIndividualNameType	CustomerName;
	///֤������
	TUstpFtdcIdCardTypeType	IdCardType;
	///֤������
	TUstpFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�ͻ�����
	TUstpFtdcCustTypeType	CustType;
	///�����ʺ�
	TUstpFtdcBankAccountType	BankAccount;
	///��������
	TUstpFtdcPasswordType	BankPassWord;
	///�ʽ��ʺ�
	TUstpFtdcAccountIDType	AccountID;
	///�ڻ�����
	TUstpFtdcPasswordType	Password;
	///��װ���
	TUstpFtdcInstallIDType	InstallID;
	///�ڻ���˾��ˮ��
	TUstpFtdcFutureSerialType	FutureSerial;
	///�û���ʶ
	TUstpFtdcUserIDType	UserID;
	///�ͻ�֤�������־
	TUstpFtdcBoolType	VerifyCertNoFlag;
	///���ִ���
	TUstpFtdcCurrencyIDType	CurrencyID;
	///ժҪ
	TUstpFtdcMessageType	Digest;
	///�����ʺ�����
	TUstpFtdcBankAccType	BankAccType;
	///������־
	TUstpFtdcDeviceIdType	DeviceID;
	///�ڻ���λ�ʺ�����
	TUstpFtdcBankAccType	BankSecuAccType;
	///�ڻ���˾���б���
	TUstpFtdcBankCodingForFutureType	BrokerIDByBank;
	///�ڻ���λ�ʺ�
	TUstpFtdcBankAccountType	BankSecuAcc;
	///���������־
	TUstpFtdcPwdFlagTypeType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TUstpFtdcPwdFlagTypeType	SecuPwdFlag;
	///���׹�Ա
	TUstpFtdcOperNoType	OperNo;
	///������
	TUstpFtdcRequestIDType	RequestID;
	///����ID
	TUstpFtdcTIDType	TID;
};
///��ѯ�˻���Ϣ֪ͨ
struct CUstpFtdcAccountFieldRtnField
{
	///ҵ������
	TUstpFtdcTradeCodeType	TradeCode;
	///���д���
	TUstpFtdcBrokerIDType	BankID;
	///���з����Ĵ���
	TUstpFtdcInvestorIDType	BankBrchID;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TUstpFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TUstpFtdcDateType	TradeDate;
	///�ɽ�ʱ��
	TUstpFtdcTimeType	TradeTime;
	///������ˮ��
	TUstpFtdcBankSerialType	BankSerial;
	///������
	TUstpFtdcDateType	TradingDay;
	///ƽ̨��ˮ��
	TUstpFtdcPlateSerialType	PlateSerial;
	///����Ƭ��־
	TUstpFtdcBoolType	LastFragment;
	///��¼��SessionID
	TUstpFtdcSessionIDType	SessionID;
	///�ͻ�����
	TUstpFtdcIndividualNameType	CustomerName;
	///֤������
	TUstpFtdcIdCardTypeType	IdCardType;
	///֤������
	TUstpFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�ͻ�����
	TUstpFtdcCustTypeType	CustType;
	///�����ʺ�
	TUstpFtdcBankAccountType	BankAccount;
	///��������
	TUstpFtdcPasswordType	BankPassWord;
	///�ʽ��ʺ�
	TUstpFtdcAccountIDType	AccountID;
	///�ڻ�����
	TUstpFtdcPasswordType	Password;
	///��װ���
	TUstpFtdcInstallIDType	InstallID;
	///�ڻ���˾��ˮ��
	TUstpFtdcFutureSerialType	FutureSerial;
	///�û���ʶ
	TUstpFtdcUserIDType	UserID;
	///�ͻ�֤�������־
	TUstpFtdcBoolType	VerifyCertNoFlag;
	///���ִ���
	TUstpFtdcCurrencyIDType	CurrencyID;
	///ժҪ
	TUstpFtdcMessageType	Digest;
	///�����ʺ�����
	TUstpFtdcBankAccType	BankAccType;
	///������־
	TUstpFtdcDeviceIdType	DeviceID;
	///�ڻ���λ�ʺ�����
	TUstpFtdcBankAccType	BankSecuAccType;
	///�ڻ���˾���б���
	TUstpFtdcBankCodingForFutureType	BrokerIDByBank;
	///�ڻ���λ�ʺ�
	TUstpFtdcBankAccountType	BankSecuAcc;
	///���������־
	TUstpFtdcPwdFlagTypeType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TUstpFtdcPwdFlagTypeType	SecuPwdFlag;
	///���׹�Ա
	TUstpFtdcOperNoType	OperNo;
	///������
	TUstpFtdcRequestIDType	RequestID;
	///����ID
	TUstpFtdcTIDType	TID;
	///���п��ý��
	TUstpFtdcMoneyType	BankUseAmount;
	///���п�ȡ���
	TUstpFtdcMoneyType	BankFetchAmount;
};
///������Ϣ
struct CUstpFtdcRepealFieldRspField
{
	///����ʱ����
	TUstpFtdcRepealTimeIntervalType	RepealTimeInterval;
	///�Ѿ���������
	TUstpFtdcRepealedTimesType	RepealedTimes;
	///���г�����־
	TUstpFtdcBankRepealFlagType	BankRepealFlag;
	///�Ѿ���������
	TUstpFtdcBrokerRepealFlagType	BrokerRepealFlag;
	///������ƽ̨��ˮ��
	TUstpFtdcPlateSerialType	PlateRepealSerial;
	///������������ˮ��
	TUstpFtdcBankSerialType	BankRepealSerial;
	///�������ڻ���˾��ˮ��
	TUstpFtdcFutureSerialType	FutureRepealSerial;
	///ҵ������
	TUstpFtdcTradeCodeType	TradeCode;
	///���д���
	TUstpFtdcBrokerIDType	BankID;
	///���з����Ĵ���
	TUstpFtdcInvestorIDType	BankBrchID;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TUstpFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TUstpFtdcDateType	TradeDate;
	///�ɽ�ʱ��
	TUstpFtdcTimeType	TradeTime;
	///������ˮ��
	TUstpFtdcBankSerialType	BankSerial;
	///������
	TUstpFtdcDateType	TradingDay;
	///ƽ̨��ˮ��
	TUstpFtdcPlateSerialType	PlateSerial;
	///����Ƭ��־
	TUstpFtdcBoolType	LastFragment;
	///��¼��SessionID
	TUstpFtdcSessionIDType	SessionID;
	///�ͻ�����
	TUstpFtdcIndividualNameType	CustomerName;
	///֤������
	TUstpFtdcIdCardTypeType	IdCardType;
	///֤������
	TUstpFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�ͻ�����
	TUstpFtdcCustTypeType	CustType;
	///�����ʺ�
	TUstpFtdcBankAccountType	BankAccount;
	///��������
	TUstpFtdcPasswordType	BankPassWord;
	///�ʽ��ʺ�
	TUstpFtdcAccountIDType	AccountID;
	///�ڻ�����
	TUstpFtdcPasswordType	Password;
	///��װ���
	TUstpFtdcInstallIDType	InstallID;
	///�ڻ���˾��ˮ��
	TUstpFtdcFutureSerialType	FutureSerial;
	///�û���ʶ
	TUstpFtdcUserIDType	UserID;
	///�ͻ�֤�������־
	TUstpFtdcBoolType	VerifyCertNoFlag;
	///���ִ���
	TUstpFtdcCurrencyIDType	CurrencyID;
	///ת�ʽ��
	TUstpFtdcMoneyType	TradeAmount;
	///�ڻ���ȡ���
	TUstpFtdcMoneyType	FutureFetchAmount;
	///����֧����־
	TUstpFtdcFeePayFlagType	FeePayFlag;
	///Ӧ�տͻ�����
	TUstpFtdcMoneyType	CustFee;
	///Ӧ���ڻ���˾����
	TUstpFtdcMoneyType	BrokerFee;
	///���ͷ������շ�����Ϣ
	TUstpFtdcMessageType	Message;
	///ժҪ
	TUstpFtdcMessageType	Digest;
	///�����ʺ�����
	TUstpFtdcBankAccType	BankAccType;
	///������־
	TUstpFtdcDeviceIdType	DeviceID;
	///�ڻ���λ�ʺ�����
	TUstpFtdcBankAccType	BankSecuAccType;
	///�ڻ���˾���б���
	TUstpFtdcBankCodingForFutureType	BrokerIDByBank;
	///�ڻ���λ�ʺ�
	TUstpFtdcBankAccountType	BankSecuAcc;
	///���������־
	TUstpFtdcPwdFlagTypeType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TUstpFtdcPwdFlagTypeType	SecuPwdFlag;
	///���׹�Ա
	TUstpFtdcOperNoType	OperNo;
	///������
	TUstpFtdcRequestIDType	RequestID;
	///����ID
	TUstpFtdcTIDType	TID;
	///ת�˽���״̬
	TUstpFtdcTransferStatusType	TransferStatus;
};
///���ڿ�������Ϣ
struct CUstpFtdcAccountFieldField
{
	///ҵ������
	TUstpFtdcTradeCodeType	TradeCode;
	///���д���
	TUstpFtdcBrokerIDType	BankID;
	///���з����Ĵ���
	TUstpFtdcInvestorIDType	BankBrchID;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TUstpFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TUstpFtdcDateType	TradeDate;
	///�ɽ�ʱ��
	TUstpFtdcTimeType	TradeTime;
	///������ˮ��
	TUstpFtdcBankSerialType	BankSerial;
	///������
	TUstpFtdcDateType	TradingDay;
	///ƽ̨��ˮ��
	TUstpFtdcPlateSerialType	PlateSerial;
	///����Ƭ��־
	TUstpFtdcBoolType	LastFragment;
	///��¼��SessionID
	TUstpFtdcSessionIDType	SessionID;
	///�ͻ�����
	TUstpFtdcIndividualNameType	CustomerName;
	///֤������
	TUstpFtdcIdCardTypeType	IdCardType;
	///֤������
	TUstpFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�Ա�
	TUstpFtdcGenderType	Gender;
	///���Ҵ���
	TUstpFtdcCountryCodeType	CountryCode;
	///�ͻ�����
	TUstpFtdcCustTypeType	CustType;
	///��ַ
	TUstpFtdcAddressType	Address;
	///������������
	TUstpFtdcZipCodeType	ZipCode;
	///�绰����
	TUstpFtdcTelephoneType	Telephone;
	///�ֻ�
	TUstpFtdcMobilePhoneType	MobilePhone;
	///����
	TUstpFtdcFaxType	Fax;
	///�����ʼ�
	TUstpFtdcEMailType	EMail;
	///�ʽ��˻�״̬
	TUstpFtdcMoneyAccountStatusType	MoneyAccountStatus;
	///�����ʺ�
	TUstpFtdcBankAccountType	BankAccount;
	///��������
	TUstpFtdcPasswordType	BankPassWord;
	///�ʽ��ʺ�
	TUstpFtdcAccountIDType	AccountID;
	///�ڻ�����
	TUstpFtdcPasswordType	Password;
	///��װ���
	TUstpFtdcInstallIDType	InstallID;
	///�ͻ�֤�������־
	TUstpFtdcBoolType	VerifyCertNoFlag;
	///���ִ���
	TUstpFtdcCurrencyIDType	CurrencyID;
	///�㳮��־
	TUstpFtdcCashExchangeCodeType	CashExchangeCode;
	///ժҪ
	TUstpFtdcMessageType	Digest;
	///�����ʺ�����
	TUstpFtdcBankAccType	BankAccType;
	///������־
	TUstpFtdcDeviceIdType	DeviceID;
	///�ڻ���λ�ʺ�����
	TUstpFtdcBankAccType	BankSecuAccType;
	///�ڻ���˾���б���
	TUstpFtdcBankCodingForFutureType	BrokerIDByBank;
	///�ڻ���λ�ʺ�
	TUstpFtdcBankAccountType	BankSecuAcc;
	///���������־
	TUstpFtdcPwdFlagTypeType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TUstpFtdcPwdFlagTypeType	SecuPwdFlag;
	///���׹�Ա
	TUstpFtdcOperNoType	OperNo;
	///����ID
	TUstpFtdcTIDType	TID;
	///�������
	TUstpFtdcErrorIDType	ErrorID;
	///������Ϣ
	TUstpFtdcErrorMsgType	ErrorMsg;
};
///���ڱ�������˺���Ϣ
struct CUstpFtdcChangeAccountFieldField
{
	///ҵ������
	TUstpFtdcTradeCodeType	TradeCode;
	///���д���
	TUstpFtdcBrokerIDType	BankID;
	///���з����Ĵ���
	TUstpFtdcInvestorIDType	BankBrchID;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TUstpFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TUstpFtdcDateType	TradeDate;
	///�ɽ�ʱ��
	TUstpFtdcTimeType	TradeTime;
	///������ˮ��
	TUstpFtdcBankSerialType	BankSerial;
	///������
	TUstpFtdcDateType	TradingDay;
	///ƽ̨��ˮ��
	TUstpFtdcPlateSerialType	PlateSerial;
	///����Ƭ��־
	TUstpFtdcBoolType	LastFragment;
	///��¼��SessionID
	TUstpFtdcSessionIDType	SessionID;
	///�ͻ�����
	TUstpFtdcIndividualNameType	CustomerName;
	///֤������
	TUstpFtdcIdCardTypeType	IdCardType;
	///֤������
	TUstpFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�Ա�
	TUstpFtdcGenderType	Gender;
	///���Ҵ���
	TUstpFtdcCountryCodeType	CountryCode;
	///�ͻ�����
	TUstpFtdcCustTypeType	CustType;
	///��ַ
	TUstpFtdcAddressType	Address;
	///������������
	TUstpFtdcZipCodeType	ZipCode;
	///�绰����
	TUstpFtdcTelephoneType	Telephone;
	///�ֻ�
	TUstpFtdcMobilePhoneType	MobilePhone;
	///����
	TUstpFtdcFaxType	Fax;
	///�����ʼ�
	TUstpFtdcEMailType	EMail;
	///�ʽ��˻�״̬
	TUstpFtdcMoneyAccountStatusType	MoneyAccountStatus;
	///�����ʺ�
	TUstpFtdcBankAccountType	BankAccount;
	///��������
	TUstpFtdcPasswordType	BankPassWord;
	///�����ʺ�
	TUstpFtdcBankAccountType	NewBankAccount;
	///��������
	TUstpFtdcPasswordType	NewBankPassWord;
	///�ʽ��ʺ�
	TUstpFtdcAccountIDType	AccountID;
	///�ڻ�����
	TUstpFtdcPasswordType	Password;
	///��װ���
	TUstpFtdcInstallIDType	InstallID;
	///�ͻ�֤�������־
	TUstpFtdcBoolType	VerifyCertNoFlag;
	///���ִ���
	TUstpFtdcCurrencyIDType	CurrencyID;
	///ժҪ
	TUstpFtdcMessageType	Digest;
	///�����ʺ�����
	TUstpFtdcBankAccType	BankAccType;
	///�ڻ���˾���б���
	TUstpFtdcBankCodingForFutureType	BrokerIDByBank;
	///���������־
	TUstpFtdcPwdFlagTypeType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TUstpFtdcPwdFlagTypeType	SecuPwdFlag;
	///����ID
	TUstpFtdcTIDType	TID;
	///�������
	TUstpFtdcErrorIDType	ErrorID;
	///������Ϣ
	TUstpFtdcErrorMsgType	ErrorMsg;
};
///���з��������ʽ�ת�ڻ�֪ͨ
struct CUstpFtdcTransferFieldRtnField
{
	///ҵ������
	TUstpFtdcTradeCodeType	TradeCode;
	///���д���
	TUstpFtdcBrokerIDType	BankID;
	///���з����Ĵ���
	TUstpFtdcInvestorIDType	BankBrchID;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TUstpFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TUstpFtdcDateType	TradeDate;
	///�ɽ�ʱ��
	TUstpFtdcTimeType	TradeTime;
	///������ˮ��
	TUstpFtdcBankSerialType	BankSerial;
	///������
	TUstpFtdcDateType	TradingDay;
	///ƽ̨��ˮ��
	TUstpFtdcPlateSerialType	PlateSerial;
	///����Ƭ��־
	TUstpFtdcBoolType	LastFragment;
	///��¼��SessionID
	TUstpFtdcSessionIDType	SessionID;
	///�ͻ�����
	TUstpFtdcIndividualNameType	CustomerName;
	///֤������
	TUstpFtdcIdCardTypeType	IdCardType;
	///֤������
	TUstpFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�ͻ�����
	TUstpFtdcCustTypeType	CustType;
	///�����ʺ�
	TUstpFtdcBankAccountType	BankAccount;
	///��������
	TUstpFtdcPasswordType	BankPassWord;
	///�ʽ��ʺ�
	TUstpFtdcAccountIDType	AccountID;
	///�ڻ�����
	TUstpFtdcPasswordType	Password;
	///��װ���
	TUstpFtdcInstallIDType	InstallID;
	///�ڻ���˾��ˮ��
	TUstpFtdcFutureSerialType	FutureSerial;
	///�û���ʶ
	TUstpFtdcUserIDType	UserID;
	///�ͻ�֤�������־
	TUstpFtdcBoolType	VerifyCertNoFlag;
	///���ִ���
	TUstpFtdcCurrencyIDType	CurrencyID;
	///ת�ʽ��
	TUstpFtdcMoneyType	TradeAmount;
	///�ڻ���ȡ���
	TUstpFtdcMoneyType	FutureFetchAmount;
	///����֧����־
	TUstpFtdcFeePayFlagType	FeePayFlag;
	///Ӧ�տͻ�����
	TUstpFtdcMoneyType	CustFee;
	///Ӧ���ڻ���˾����
	TUstpFtdcMoneyType	BrokerFee;
	///���ͷ������շ�����Ϣ
	TUstpFtdcMessageType	Message;
	///ժҪ
	TUstpFtdcMessageType	Digest;
	///�����ʺ�����
	TUstpFtdcBankAccType	BankAccType;
	///������־
	TUstpFtdcDeviceIdType	DeviceID;
	///�ڻ���λ�ʺ�����
	TUstpFtdcBankAccType	BankSecuAccType;
	///�ڻ���˾���б���
	TUstpFtdcBankCodingForFutureType	BrokerIDByBank;
	///�ڻ���λ�ʺ�
	TUstpFtdcBankAccountType	BankSecuAcc;
	///���������־
	TUstpFtdcPwdFlagTypeType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TUstpFtdcPwdFlagTypeType	SecuPwdFlag;
	///���׹�Ա
	TUstpFtdcOperNoType	OperNo;
	///������
	TUstpFtdcRequestIDType	RequestID;
	///����ID
	TUstpFtdcTIDType	TID;
	///ת�˽���״̬
	TUstpFtdcTransferStatusType	TransferStatus;
	///�������
	TUstpFtdcErrorIDType	ErrorID;
	///������Ϣ
	TUstpFtdcErrorMsgType	ErrorMsg;
};
///��ѯ�˻���Ϣ����֪ͨ
struct CUstpFtdcAccountFieldErrRtnField
{
	///ҵ������
	TUstpFtdcTradeCodeType	TradeCode;
	///���д���
	TUstpFtdcBrokerIDType	BankID;
	///���з����Ĵ���
	TUstpFtdcInvestorIDType	BankBrchID;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TUstpFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TUstpFtdcDateType	TradeDate;
	///�ɽ�ʱ��
	TUstpFtdcTimeType	TradeTime;
	///������ˮ��
	TUstpFtdcBankSerialType	BankSerial;
	///������
	TUstpFtdcDateType	TradingDay;
	///ƽ̨��ˮ��
	TUstpFtdcPlateSerialType	PlateSerial;
	///����Ƭ��־
	TUstpFtdcBoolType	LastFragment;
	///��¼��SessionID
	TUstpFtdcSessionIDType	SessionID;
	///�ͻ�����
	TUstpFtdcIndividualNameType	CustomerName;
	///֤������
	TUstpFtdcIdCardTypeType	IdCardType;
	///֤������
	TUstpFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�ͻ�����
	TUstpFtdcCustTypeType	CustType;
	///�����ʺ�
	TUstpFtdcBankAccountType	BankAccount;
	///��������
	TUstpFtdcPasswordType	BankPassWord;
	///�ʽ��ʺ�
	TUstpFtdcAccountIDType	AccountID;
	///�ڻ�����
	TUstpFtdcPasswordType	Password;
	///��װ���
	TUstpFtdcInstallIDType	InstallID;
	///�ڻ���˾��ˮ��
	TUstpFtdcFutureSerialType	FutureSerial;
	///�û���ʶ
	TUstpFtdcUserIDType	UserID;
	///�ͻ�֤�������־
	TUstpFtdcBoolType	VerifyCertNoFlag;
	///���ִ���
	TUstpFtdcCurrencyIDType	CurrencyID;
	///ժҪ
	TUstpFtdcMessageType	Digest;
	///�����ʺ�����
	TUstpFtdcBankAccType	BankAccType;
	///������־
	TUstpFtdcDeviceIdType	DeviceID;
	///�ڻ���λ�ʺ�����
	TUstpFtdcBankAccType	BankSecuAccType;
	///�ڻ���˾���б���
	TUstpFtdcBankCodingForFutureType	BrokerIDByBank;
	///�ڻ���λ�ʺ�
	TUstpFtdcBankAccountType	BankSecuAcc;
	///���������־
	TUstpFtdcPwdFlagTypeType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TUstpFtdcPwdFlagTypeType	SecuPwdFlag;
	///���׹�Ա
	TUstpFtdcOperNoType	OperNo;
	///������
	TUstpFtdcRequestIDType	RequestID;
	///����ID
	TUstpFtdcTIDType	TID;
	///�������
	TUstpFtdcErrorIDType	ErrorID;
	///������Ϣ
	TUstpFtdcErrorMsgType	ErrorMsg;
};
///����֪ͨ
struct CUstpFtdcRepealFieldRtnField
{
	///����ʱ����
	TUstpFtdcRepealTimeIntervalType	RepealTimeInterval;
	///�Ѿ���������
	TUstpFtdcRepealedTimesType	RepealedTimes;
	///���г�����־
	TUstpFtdcBankRepealFlagType	BankRepealFlag;
	///�Ѿ���������
	TUstpFtdcBrokerRepealFlagType	BrokerRepealFlag;
	///������ƽ̨��ˮ��
	TUstpFtdcPlateSerialType	PlateRepealSerial;
	///������������ˮ��
	TUstpFtdcBankSerialType	BankRepealSerial;
	///�������ڻ���˾��ˮ��
	TUstpFtdcFutureSerialType	FutureRepealSerial;
	///ҵ������
	TUstpFtdcTradeCodeType	TradeCode;
	///���д���
	TUstpFtdcBrokerIDType	BankID;
	///���з����Ĵ���
	TUstpFtdcInvestorIDType	BankBrchID;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///���̷�֧��������
	TUstpFtdcFutureBranchIDType	BrokerBranchID;
	///��������
	TUstpFtdcDateType	TradeDate;
	///�ɽ�ʱ��
	TUstpFtdcTimeType	TradeTime;
	///������ˮ��
	TUstpFtdcBankSerialType	BankSerial;
	///������
	TUstpFtdcDateType	TradingDay;
	///ƽ̨��ˮ��
	TUstpFtdcPlateSerialType	PlateSerial;
	///����Ƭ��־
	TUstpFtdcBoolType	LastFragment;
	///��¼��SessionID
	TUstpFtdcSessionIDType	SessionID;
	///�ͻ�����
	TUstpFtdcIndividualNameType	CustomerName;
	///֤������
	TUstpFtdcIdCardTypeType	IdCardType;
	///֤������
	TUstpFtdcIdentifiedCardNoType	IdentifiedCardNo;
	///�ͻ�����
	TUstpFtdcCustTypeType	CustType;
	///�����ʺ�
	TUstpFtdcBankAccountType	BankAccount;
	///��������
	TUstpFtdcPasswordType	BankPassWord;
	///�ʽ��ʺ�
	TUstpFtdcAccountIDType	AccountID;
	///�ڻ�����
	TUstpFtdcPasswordType	Password;
	///��װ���
	TUstpFtdcInstallIDType	InstallID;
	///�ڻ���˾��ˮ��
	TUstpFtdcFutureSerialType	FutureSerial;
	///�û���ʶ
	TUstpFtdcUserIDType	UserID;
	///�ͻ�֤�������־
	TUstpFtdcBoolType	VerifyCertNoFlag;
	///���ִ���
	TUstpFtdcCurrencyIDType	CurrencyID;
	///ת�ʽ��
	TUstpFtdcMoneyType	TradeAmount;
	///�ڻ���ȡ���
	TUstpFtdcMoneyType	FutureFetchAmount;
	///����֧����־
	TUstpFtdcFeePayFlagType	FeePayFlag;
	///Ӧ�տͻ�����
	TUstpFtdcMoneyType	CustFee;
	///Ӧ���ڻ���˾����
	TUstpFtdcMoneyType	BrokerFee;
	///���ͷ������շ�����Ϣ
	TUstpFtdcMessageType	Message;
	///ժҪ
	TUstpFtdcMessageType	Digest;
	///�����ʺ�����
	TUstpFtdcBankAccType	BankAccType;
	///������־
	TUstpFtdcDeviceIdType	DeviceID;
	///�ڻ���λ�ʺ�����
	TUstpFtdcBankAccType	BankSecuAccType;
	///�ڻ���˾���б���
	TUstpFtdcBankCodingForFutureType	BrokerIDByBank;
	///�ڻ���λ�ʺ�
	TUstpFtdcBankAccountType	BankSecuAcc;
	///���������־
	TUstpFtdcPwdFlagTypeType	BankPwdFlag;
	///�ڻ��ʽ�����˶Ա�־
	TUstpFtdcPwdFlagTypeType	SecuPwdFlag;
	///���׹�Ա
	TUstpFtdcOperNoType	OperNo;
	///������
	TUstpFtdcRequestIDType	RequestID;
	///����ID
	TUstpFtdcTIDType	TID;
	///ת�˽���״̬
	TUstpFtdcTransferStatusType	TransferStatus;
	///�������
	TUstpFtdcErrorIDType	ErrorID;
	///������Ϣ
	TUstpFtdcErrorMsgType	ErrorMsg;
};
///������Ȩ
struct CUstpFtdcInputExecOrderField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///ϵͳ�������
	TUstpFtdcOrderSysIDType	OrderSysID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
	///�û����ر�����
	TUstpFtdcUserOrderLocalIDType	UserOrderLocalID;
	///ί������
	TUstpFtdcOrderTypeType	OrderType;
	///��Ȩ�Գ��ʶ
	TUstpFtdcDeliveryFlagType	DeliveryFlag;
	///Ͷ���ױ���־
	TUstpFtdcHedgeFlagType	HedgeFlag;
	///����
	TUstpFtdcVolumeType	Volume;
	///�û��Զ�����
	TUstpFtdcCustomType	UserCustom;
	///ҵ��������
	TUstpFtdcDateType	ActionDay;
	///����ҵ���ʶ
	TUstpFtdcBusinessLocalIDType	BusinessLocalID;
	///ҵ��Ԫ
	TUstpFtdcBusinessUnitType	BusinessUnit;
};
///������Ȩ����
struct CUstpFtdcInputExecOrderActionField
{
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///�������
	TUstpFtdcOrderSysIDType	OrderSysID;
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///���γ��������ı��ر��
	TUstpFtdcUserOrderLocalIDType	UserOrderActionLocalID;
	///���������ı��ر������
	TUstpFtdcUserOrderLocalIDType	UserOrderLocalID;
	///����������־
	TUstpFtdcActionFlagType	ActionFlag;
	///�����仯
	TUstpFtdcVolumeType	VolumeChange;
	///����ҵ���ʶ
	TUstpFtdcBusinessLocalIDType	BusinessLocalID;
	///ί������
	TUstpFtdcOrderTypeType	OrderType;
};
///��Ȩ֪ͨ
struct CUstpFtdcExecOrderField
{
	///���͹�˾���
	TUstpFtdcBrokerIDType	BrokerID;
	///����������
	TUstpFtdcExchangeIDType	ExchangeID;
	///ϵͳ�������
	TUstpFtdcOrderSysIDType	OrderSysID;
	///Ͷ���߱��
	TUstpFtdcInvestorIDType	InvestorID;
	///�û�����
	TUstpFtdcUserIDType	UserID;
	///��Լ����
	TUstpFtdcInstrumentIDType	InstrumentID;
	///�û����ر�����
	TUstpFtdcUserOrderLocalIDType	UserOrderLocalID;
	///ί������
	TUstpFtdcOrderTypeType	OrderType;
	///��Ȩ�Գ��ʶ
	TUstpFtdcDeliveryFlagType	DeliveryFlag;
	///Ͷ���ױ���־
	TUstpFtdcHedgeFlagType	HedgeFlag;
	///����
	TUstpFtdcVolumeType	Volume;
	///�û��Զ�����
	TUstpFtdcCustomType	UserCustom;
	///ҵ��������
	TUstpFtdcDateType	ActionDay;
	///����ҵ���ʶ
	TUstpFtdcBusinessLocalIDType	BusinessLocalID;
	///ҵ��Ԫ
	TUstpFtdcBusinessUnitType	BusinessUnit;
	///������
	TUstpFtdcTradingDayType	TradingDay;
	///��Ա���
	TUstpFtdcParticipantIDType	ParticipantID;
	///����µ��û����
	TUstpFtdcUserIDType	OrderUserID;
	///�ͻ���
	TUstpFtdcClientIDType	ClientID;
	///�µ�ϯλ��
	TUstpFtdcSeatIDType	SeatID;
	///����ʱ��
	TUstpFtdcTimeType	InsertTime;
	///���ر������
	TUstpFtdcOrderLocalIDType	OrderLocalID;
	///������Դ
	TUstpFtdcOrderSourceType	OrderSource;
	///����״̬
	TUstpFtdcOrderStatusType	OrderStatus;
	///����ʱ��
	TUstpFtdcTimeType	CancelTime;
	///�����û����
	TUstpFtdcUserIDType	CancelUserID;
};


#endif
