/**
* Copyright (C) 2015-2017, �����ɴ���Ϣ�������޹�˾
* @file <X1FtdcApiStruct.h>
* @brief <����ӿ���������ݽӿ�>
* @version <1.5.1.2>
* @author <X1��Ŀ��>
* @date <2017��8��16��>
*/

#ifndef X1FTDCAPISTRUCT_H_
#define X1FTDCAPISTRUCT_H_

#include "X1FtdcApiDataType.h"

#define APISTRUCT

///������
struct APISTRUCT CX1FtdcTimeOutField {
    TX1FtdcRequestIDType                  RequestID;                   // ����ID
};

///���󱨵���������(��������)
struct APISTRUCT CX1FtdcInsertOrderField {
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˻�
    TX1FtdcLocalOrderIDType               LocalOrderID;                 // ����ί�к�, ��APIʹ����ά������ͬһ���Ự�в����ظ�
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcPriceType                      InsertPrice;                  // �����۸�, ����������Ϊ�м�ʱ�����ֶβ�������
    TX1FtdcAmountType                     OrderAmount;                  // ��������
    TX1FtdcBuySellTypeType                BuySellType;                  // ������־, ��Ȩִ��ʱ��1Ϊ���Գ壬2Ϊ�Գ�
    TX1FtdcOpenCloseTypeType              OpenCloseType;                // ��ƽ��־
    TX1FtdcSpeculatorType                 Speculator;                   // Ͷ������, ֧��Ͷ�����������ױ�
    TX1FtdcInsertType                     InsertType;                   // �Զ������(Ĭ��Ϊ��ͨ����)
    TX1FtdcOrderTypeType                  OrderType;                    // ��������, ֧���޼� ���мۣ���������Լ��֧���мۣ������޼۽��д���
    TX1FtdcOrderPropertyType              OrderProperty;                // ������������, ֧��None��FAK��FOK
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����, ��ѡֵ���ڻ�����Ȩ
    TX1FtdcAmountType                     MinMatchAmount;               // ��С�ɽ���
    TX1FtdcReservedType                   ReservedType2;                // Ԥ���ֶ�2
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcCustomCategoryType             CustomCategory;               // �Զ������
    TX1FtdcPriceType                      ProfitLossPrice;              // ֹӯֹ��۸�
};

///������������
struct APISTRUCT CX1FtdcCancelOrderField {
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˻�ID(�˰汾����)
    TX1FtdcX1OrderIDType                  X1OrderID;                    // ��̨ί�к�
    TX1FtdcLocalOrderIDType               LocalOrderID;                 // ����ί�к�
    TX1FtdcOrderSysIDType                 OrderSysID;                   // ����ϵͳί�к�(����˫�߱��۵��߳����ʹ�������Ȩִ�е�����)
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcSessionIDType                  SessionID;                    // �ỰID
};

///ί����Ӧ����
struct APISTRUCT CX1FtdcRspOperOrderField {
    TX1FtdcLocalOrderIDType               LocalOrderID;                 // ����ί�к�
    TX1FtdcX1OrderIDType                  X1OrderID;                    // ��̨ί�к�
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  // ί��״̬
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcPriceType                      Fee;                          // ������,���ֶν����µ�ʱʹ��(�˰汾����)
    TX1FtdcPriceType                      Margin;                       // ���ᱣ֤��,���ֶν����µ�ʱʹ��
    TX1FtdcCustomCategoryType             CustomCategory;               // �Զ������
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˻�ID(�˰汾����)
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����(�˰汾����)
    TX1FtdcSessionIDType                  SessionID;                    // �ỰID  
    TX1FtdcExchangeIDType                 ExchangeID;                   // ������(�˰汾����)
    TX1FtdcBuySellTypeType                BuySellType;                  // ���������ֶν����µ�ʱʹ��
    TX1FtdcOpenCloseTypeType              OpenCloseType;                // ��ƽ(����ʱ�˰汾����)
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����
    TX1FtdcSpeculatorType                 Speculator;                   // Ͷ�����(����ʱ�˰汾����)
    TX1FtdcPriceType                      InsertPrice;                  // ί�м�(����ʱ�˰汾����)
    TX1FtdcPriceType                      ProfitLossPrice;              // ֹӯֹ��۸�
    TX1FtdcAmountType                     MinMatchAmount;               // ��С�ɽ���
    TX1FtdcAmountType                     OrderAmount;                  // ί������(����ʱ�˰汾����)
    TX1FtdcInsertType                     InsertType;                   // �Զ������
    TX1FtdcOrderTypeType                  OrderType;                    // ��������(����ʱ�˰汾����)
    TX1FtdcOrderPropertyType              OrderProperty;                // ��������
    TX1FtdcClientIDType                   ClientID;                     // ���ױ���(�˰汾����)
};

///��ѯ�ʽ���������
struct APISTRUCT CX1FtdcQryCapitalField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˻�ID
};

///��ѯ�ֲ���������
struct APISTRUCT CX1FtdcQryPositionField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˻�ID
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����
};

///��������Լ
/**
* @brief ��������Լ
* @details ��ExchangeIDΪ��ʱ����ʾ��ѯ�������������к�Լ���룬Ҳ��ָ����ѯĳ�����������к�Լ������Ϣ�����ýӿڲ��ܲ�ѯ��������Լ����
*
*/
struct APISTRUCT CX1FtdcQryExchangeInstrumentField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˻�ID
    TX1FtdcExchangeIDType                 ExchangeID;                   // ����������
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����
};

/// �û���¼��������
struct APISTRUCT CX1FtdcReqUserLoginField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID [NM]
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˻�ID [M]
    TX1FtdcPasswordType                   Password;                     // ���� [M]
    TX1FtdcCompanyIDType                  CompanyID;                    // ����ID(�˰汾����)[BLK]
    TX1FtdcAppIDType                      AppID;                        // AppID [M]
    TX1FtdcAuthCodeType                   AuthCode;                     // ��Ȩ�� [M]
};

/// �û���¼������Ϣ 
struct APISTRUCT CX1FtdcRspUserLoginField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��ʺ�ID
    TX1FtdcAccountLoginResultType         LoginResult;                  // ��¼���
    TX1FtdcLocalOrderIDType               InitLocalOrderID;             // ��ʼ����ί�к�(�˰汾����)
    TX1FtdcSessionIDType                  SessionID;                    // sessionID(�ỰID)
    TX1FtdcErrorIDType                    ErrorID;                      // ����ID
    TX1FtdcErrorMsgInfoType               ErrorMsg;                     // ������Ϣ
    TX1FtdcTimeType                       DCEtime;                      // ������ʱ��
    TX1FtdcTimeType                       SHFETime;                     // ������ʱ��
    TX1FtdcTimeType                       CFFEXTime;                    // �н���ʱ��
    TX1FtdcTimeType                       CZCETime;                     // ֣����ʱ��
    TX1FtdcTimeType                       INETime;                      // ������ʱ��
};

///�û��˳�����
struct APISTRUCT CX1FtdcReqUserLogoutField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��ʺ�ID [NM] �û����Դ��ջ�����ȷ�ĵ�¼��
    TX1FtdcSessionIDType                  SessionID;                    // �ỰID(�˰汾����)[BLK]
};

///ί�лر�
struct APISTRUCT CX1FtdcRspPriOrderField {
    TX1FtdcLocalOrderIDType               LocalOrderID;                 // ����ί�к� ��ϵͳ�����󣬾����ָ�����ȷ��׼ȷ
    TX1FtdcX1OrderIDType                  X1OrderID;                    // ��̨ί�к�
    TX1FtdcOrderSysIDType                 OrderSysID;                   // �������
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  // ί��״̬  ��FAK FOK������ؽ������Ѿ����ܣ�����δ�ɽ�8��ʧ�ܷ���7���������һ�ɷ���δ�ɽ�3
    TX1FtdcSessionIDType                  SessionID;                    // �ỰID
    TX1FtdcDateType                       SuspendTime;                  // ����ʱ��(�˰汾����)
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcExchangeIDType                 ExchangeID;                   // ������ 
    TX1FtdcBuySellTypeType                BuySellType;                  // ����
    TX1FtdcOpenCloseTypeType              OpenCloseType;                // ��ƽ
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����
    TX1FtdcSpeculatorType                 Speculator;                   // Ͷ�����
    TX1FtdcPriceType                      InsertPrice;                  // ί�м�
    TX1FtdcPriceType                      ProfitLossPrice;              // ֹӯֹ��۸�
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˺�(�˰汾����)
    TX1FtdcAmountType                     CancelAmount;                 // ��������
    TX1FtdcAmountType                     OrderAmount;                  // ί������
    TX1FtdcInsertType                     InsertType;                   // �Զ������
    TX1FtdcOrderTypeType                  OrderType;                    // ��������
    TX1FtdcX1OrderIDType                  ExtX1OrderID;                 // �㷨�����(�˰汾����)
    TX1FtdcReservedType                   ReservedType2;                // Ԥ���ֶ�2(�˰汾����)
    TX1FtdcCustomCategoryType             CustomCategory;               // �Զ������
    TX1FtdcOrderPropertyType              OrderProperty;                // ��������
    TX1FtdcAmountType                     MinMatchAmount;               // ��С�ɽ���
    TX1FtdcClientIDType                   ClientID;                     // ���ױ���
    TX1FtdcErrorIDType                    ErrorID;                      // ����ID
    TX1FtdcErrorMsgInfoType               StatusMsg;                    // ״̬��Ϣ
    TX1FtdcExtOrderType                   ExtOrderType;                 // ����������(�˰汾����)
    TX1FtdcLocalOrderIDType               PriFlowNo;                    // ˽�������
};

///�ɽ��ر�
struct APISTRUCT CX1FtdcRspPriMatchInfoField {
    TX1FtdcLocalOrderIDType               LocalOrderID;                 // ����ί�к�
    TX1FtdcOrderSysIDType                 OrderSysID;                   // �������(�������������)
    TX1FtdcMatchIDType                    MatchID;                      // �ɽ����
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcBuySellTypeType                BuySellType;                  // ����
    TX1FtdcOpenCloseTypeType              OpenCloseType;                // ��ƽ��־
    TX1FtdcPriceType                      MatchedPrice;                 // �ɽ��۸�
    TX1FtdcAmountType                     OrderAmount;                  // ί������
    TX1FtdcAmountType                     MatchedAmount;                // �ɽ�����
    TX1FtdcDateType                       MatchedTime;                  // �ɽ�ʱ��
    TX1FtdcPriceType                      InsertPrice;                  // ����
    TX1FtdcX1OrderIDType                  X1OrderID;                    // ��̨ί�к�
    TX1FtdcMatchType                      MatchType;                    // �ɽ�����
    TX1FtdcSpeculatorType                 Speculator;                   // Ͷ��
    TX1FtdcExchangeIDType                 ExchangeID;                   // ������ID
    TX1FtdcFeeType                        Fee;                          // ������(�˰汾����)
    TX1FtdcSessionIDType                  SessionID;                    // �Ự��ʶ
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˺�(�˰汾����)
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  // �걨���
    TX1FtdcPriceType                      Margin;                       // ����Ϊ��֤��,ƽ��Ϊ�ⶳ��֤��(�˰汾����)
    TX1FtdcPriceType                      FrozenCapita;                 // �ɽ��ⶳί�ж�����ʽ�(�˰汾����)
    TX1FtdcAdjustmentInfoType             AdjustmentInfo;               // ��ϻ�����ı�֤�������Ϣ,��ʽ:[��Լ����,������־,Ͷ�����,�������;](�˰汾����) 
    TX1FtdcCustomCategoryType             CustomCategory;               // �Զ������
    TX1FtdcPriceType                      Turnover;                     // �ɽ����(�˰汾����)
    TX1FtdcOrderTypeType                  OrderType;                    // ��������
    TX1FtdcInsertType                     InsertType;                   // �Զ������
    TX1FtdcClientIDType                   ClientID;                     // ���ױ���(�˰汾����)
    TX1FtdcLocalOrderIDType               PriFlowNo;                    // ˽�������
};

///�����ر�
struct APISTRUCT CX1FtdcRspPriCancelOrderField {
    TX1FtdcLocalOrderIDType               LocalOrderID;                 // ����ί�к�
    TX1FtdcOrderSysIDType                 OrderSysID;                   // �������  
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����(�˰汾����)  
    TX1FtdcPriceType                      InsertPrice;                  // �����۸�
    TX1FtdcBuySellTypeType                BuySellType;                  // ��������
    TX1FtdcOpenCloseTypeType              OpenCloseType;                // ��ƽ��־
    TX1FtdcAmountType                     CancelAmount;                 // ��������
    TX1FtdcX1OrderIDType                  X1OrderID;                    // ��̨ί�к�
    TX1FtdcSpeculatorType                 Speculator;                   // Ͷ��
    TX1FtdcExchangeIDType                 ExchangeID;                   // ������ID
    TX1FtdcDateType                       CanceledTime;                 // ����ʱ��
    TX1FtdcSessionIDType                  SessionID;                    // �Ự��ʶ
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  // �걨���
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˺�(�˰汾����)
    TX1FtdcAmountType                     OrderAmount;                  // ί������
    TX1FtdcPriceType                      Margin;                       // ��֤��(�˰汾����)
    TX1FtdcPriceType                      Fee;                          // ������(�˰汾����)
    TX1FtdcCustomCategoryType             CustomCategory;               // �Զ������
    TX1FtdcPriceType                      ProfitLossPrice;              // ֹӯֹ��۸�
    TX1FtdcAmountType                     MinMatchAmount;               // ��С�ɽ���
    TX1FtdcInsertType                     InsertType;                   // �Զ������
    TX1FtdcClientIDType                   ClientID;                     // ���ױ���(�˰汾����)
    TX1FtdcErrorIDType                    ErrorID;                      // ����ID
    TX1FtdcErrorMsgInfoType               StatusMsg;                    // ״̬��Ϣ
    TX1FtdcOrderPropertyType              OrderProperty;                // ������������
    TX1FtdcLocalOrderIDType               PriFlowNo;                    // ˽�������
};

///������Ϣ
struct APISTRUCT CX1FtdcRspErrorField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcSessionIDType                  SessionID;                    // �Ự��ʶ
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˺�
    TX1FtdcErrorIDType                    ErrorID;                      // ����ID
    TX1FtdcX1OrderIDType                  X1OrderID;                    // ��̨ί�к�
    TX1FtdcLocalOrderIDType               LocalOrderID;                 // ����ί�к�
    TX1FtdcErrorMsgInfoType               ErrorMsg;                     // ������Ϣ
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
};

///�����ʽ���Ϣ
struct APISTRUCT CX1FtdcRspCapitalField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��ʺ�
    TX1FtdcEquityType                     PreEquity;                    // ����Ȩ��
    TX1FtdcEquityType                     TodayEquity;                  // ���տͻ�Ȩ��
    TX1FtdcProfitLossType                 CloseProfitLoss;              // ƽ��ӯ��
    TX1FtdcProfitLossType                 PositionProfitLoss;           // �ֲ�ӯ��
    TX1FtdcProfitLossType                 FrozenMargin;                 // �����ʽ�
    TX1FtdcProfitLossType                 Margin;                       // �ֱֲ�֤��
    TX1FtdcProfitLossType                 Fee;                          // ����������
    TX1FtdcProfitLossType                 Available;                    // �����ʽ�
    TX1FtdcProfitLossType                 Withdraw;                     // ��ȡ�ʽ�
    TX1FtdcRiskDegreeType                 RiskDegree;                   // ���ն�
    TX1FtdcPremiumType                    TodayPremiumIncome;           // ����Ȩ��������
    TX1FtdcPremiumType                    TodayPremiumPay;              // ����Ȩ���𸶳�
    TX1FtdcPremiumType                    YesterdayPremium;             // ��Ȩ�����ո�
    TX1FtdcMarketValueType                OptMarketValue;               // ��Ȩ��ֵ
    TX1FtdcProfitLossType                 ProfitLoss;                   // ����ӯ��
    TX1FtdcProfitLossType                 TotalFundOut;                 // �ܳ���
    TX1FtdcProfitLossType                 TotalFundIn;                  // �����
    TX1FtdcRatioType                      UpFloathingRatio;             // ������������
    TX1FtdcRatioType                      DownFloathingRatio;           // ������������
    TX1FtdcRatioType                      AllocateRatio;                // �������
    TX1FtdcCurrencyType                   CurrencyID;                   //���ִ���
    TX1FtdcProfitLossType                 Mortgage;                     //��Ѻ���(�˰汾����)
    TX1FtdcProfitLossType                 FundMortgageIn;               //����������
    TX1FtdcProfitLossType                 FundMortgageOut;              //�����ʳ����
    TX1FtdcProfitLossType                 FundMortgageAvailable;        //������Ѻ���
    TX1FtdcProfitLossType                 YesterdayMargin;              //��֤��
};

///���سֲ���Ϣ
struct APISTRUCT CX1FtdcRspPositionField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��ʺ�ID
    TX1FtdcExchangeIDType                 ExchangeID;                   // ����������
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ��
    TX1FtdcBuySellTypeType                BuySellType;                  // ����
    TX1FtdcPriceType                      OpenAvgPrice;                 // ���־���
    TX1FtdcPriceType                      PositionAvgPrice;             // �ֲ־���
    TX1FtdcAmountType                     PositionAmount;               // �ֲ���
    TX1FtdcAmountType                     TotalAvaiAmount;              // �ܿ���
    TX1FtdcAmountType                     TodayAvaiAmount;              // �����
    TX1FtdcAmountType                     LastAvaiAmount;               // �����
    TX1FtdcAmountType                     TodayAmount;                  // ���
    TX1FtdcAmountType                     LastAmount;                   // ���
    TX1FtdcAmountType                     TradingAmount;                // ƽ��ҵ���
    TX1FtdcProfitLossType                 DatePositionProfitLoss;       // ���гֲ�ӯ��
    TX1FtdcProfitLossType                 DateCloseProfitLoss;          // ����ƽ��ӯ��
    TX1FtdcProfitLossType                 Premium;                      // Ȩ����
    TX1FtdcProfitLossType                 ProfitLoss;                   // ����ӯ��
    TX1FtdcProfitLossType                 Margin;                       // ռ�ñ�֤��
    TX1FtdcSpeculatorType                 Speculator;                   // Ͷ�����
    TX1FtdcClientIDType                   ClientID;                     // ���ױ���
    TX1FtdcPriceType                      PreSettlementPrice;           // ������
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����
    TX1FtdcAmountType                     YesterdayTradingAmount;       // ƽ��ҵ���
    TX1FtdcProfitLossType                 OptionValue;                  // ��Ȩ��ֵ
};

///�û��˳�������Ϣ
struct APISTRUCT CX1FtdcRspUserLogoutInfoField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˻�ID
    TX1FtdcAccountLogoutResultType        LogoutResult;                 // �˳����
    TX1FtdcErrorIDType                    ErrorID;                      // ����ID
    TX1FtdcErrorMsgInfoType               ErrorMsg;                     // ������Ϣ
};

///ָ���ĺ�Լ
struct APISTRUCT CX1FtdcQrySpecificInstrumentField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˻�ID
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcExchangeIDType                 ExchangeID;                   // ������ID
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����
};

///��������Լ������Ϣ
struct APISTRUCT CX1FtdcRspExchangeInstrumentField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcExchangeIDType                 ExchangeID;                   // ����������
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcVarietyNameType                VarietyName;                  // Ʒ������
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����
    TX1FtdcAmountType                     OrderTopLimit;                // �޼�ί������
    TX1FtdcAmountType                     MktOrderTopLimit;             // �м�ί������
    TX1FtdcPriceType                      ContractMultiplier;           // ��Լ����
    TX1FtdcPriceType                      MinPriceFluctuation;          // ��С�䶯��λ
    TX1FtdcInstrumentMaturityType         InstrumentMaturity;           // ��Լ�������
    TX1FtdcPriceType                      UpperLimitPrice;              // ��ͣ���
    TX1FtdcPriceType                      LowerLimitPrice;              // ��ͣ���
    TX1FtdcPriceType                      PreClosePrice;                // ������
    TX1FtdcPriceType                      PreSettlementPrice;           // ������
    TX1FtdcPriceType                      SettlementPrice;              // �����
    TX1FtdcAmountType                     PreOpenInterest;              // ��ֲ���
    TX1FtdcInstrumentPrefixType           InstrumentPrefix;             // ��Լǰ׺
    TX1FtdcInstrumenExpirationDateType    InstrumentExpiration;         // ��Լ������
    TX1FtdcInstrumentIDType               Underlying;                   // ��Ȩ��Ӧ�ı�ĺ�Լ����
    TX1FtdcOptionTypeType                 OptionType;                   // ��Ȩ����
    TX1FtdcPriceType                      StrikePrice;                  // ִ�м۸�
    TX1FtdcRiskDegreeType                 ExchangeRiskDegree;           // ��������Ȩ��ͱ��Ϸ���ϵ��
    TX1FtdcPriceType                      MinMargin;                    // ��λ(��)��Ȩ��Լ��С��֤��
    TX1FtdcAmountType                     TradeSize;                    // ��Ȩ���ֵ�λ
};

///ί�в�ѯ���ݽṹ
/**
* @brief ί�в�ѯ���ݽṹ
* @details �����зḻ�Ĳ�ѯ�������ã�������ѯ����ί�м�¼����ֻ��Ҫ����AccountID��InstrumentType���ɡ�
*          ��ѯ����ĳ״̬�ı�����������Ҫ��OrderStatus����Ϊ��Ӧ����״̬��ֵ���ɣ���3��ʾδ�ɽ��ڶ��еĵ��ӡ�(ע��OrderStatus��OrderType��ѯ�����ð汾�ݲ�֧��)
*          ��ѯָ���������͵ĵ��ӣ����޼۵����м۵��ȡ�
*          ��Ȩִ��ί�в�ѯʱ����̨ί�к�ʹ�ñ�����š�
*          �ϵ���֧���µ���ί�в�ѯ��
*          �ϵ��ļ۸����ʾΪ0.
*/
struct APISTRUCT CX1FtdcQryOrderField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˻�ID
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����
    TX1FtdcCustomCategoryType             CustomCategory;               // �Զ������
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  // ί��״̬
    TX1FtdcOrderTypeType                  OrderType;                    // ��������
    TX1FtdcX1OrderIDType                  X1OrderID;                    // ��̨ί�к�
    TX1FtdcLocalOrderIDType               LocalOrderID;                 // ����ί�к�
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
};

///�ɽ���ѯ���ݽṹ
/**
* @brief �ɽ���ѯ���ݽṹ
* @details �����зḻ�Ĳ�ѯ�������ã�������ѯ����ί�м�¼����ֻ��Ҫ����AccountID��InstrumentType���ɡ���ע��OrderType��ѯ�����ð汾�ݲ�֧�֣�
*/
struct APISTRUCT CX1FtdcQryMatchField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˻�ID
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����
    TX1FtdcCustomCategoryType             CustomCategory;               // �Զ������
    TX1FtdcOrderTypeType                  OrderType;                    // ��������
    TX1FtdcX1OrderIDType                  X1OrderID;                    // ��̨ί�к�
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
};

///ί�в�ѯ��Ӧ���ݽṹ
struct APISTRUCT CX1FtdcRspOrderField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcX1OrderIDType                  X1OrderID;                    // ��̨ί�к�
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  // ί��״̬
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcBuySellTypeType                BuySellType;                  // ����
    TX1FtdcOpenCloseTypeType              OpenClose;                    // ��ƽ��־
    TX1FtdcPriceType                      InsertPrice;                  // ί�м�
    TX1FtdcAmountType                     OrderAmount;                  // ί������
    TX1FtdcPriceType                      MatchedPrice;                 // �ɽ��۸�
    TX1FtdcAmountType                     MatchedAmount;                // �ɽ�����
    TX1FtdcAmountType                     CancelAmount;                 // ��������
    TX1FtdcInsertType                     InsertType;                   // �Զ������
    TX1FtdcSpeculatorType                 Speculator;                   // Ͷ��
    TX1FtdcDateType                       CommTime;                     // ί��ʱ��
    TX1FtdcDateType                       SubmitTime;                   // �걨ʱ��
    TX1FtdcClientIDType                   ClientID;                     // ���ױ���
    TX1FtdcExchangeIDType                 ExchangeID;                   // ������ID
    TX1FtdcFrontAddrType                  OperStation;                  // ί�е�ַ
    TX1FtdcAccountIDType                  AccountID;                    // �ͻ���
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����
    TX1FtdcSessionIDType                  SessionId;                    // �ỰID
    TX1FtdcReservedType                   ReservedType2;                // Ԥ���ֶ�2
    TX1FtdcOrderSysIDType                 OrderSysID;                   // ��������
    TX1FtdcCustomCategoryType             CustomCategory;               // �Զ������
    TX1FtdcPriceType                      Margin;                       // ��֤��
    TX1FtdcPriceType                      Fee;                          // ������
    TX1FtdcLocalOrderIDType               LocalOrderID;                 // ����ί�к� ���µ�ʱ�õ��ӵı���ί�кţ����ͬһ�˺ŴӶ���ͻ����µ������ѯ���ص�LocalOrderID�������ظ���
    TX1FtdcPriceType                      ProfitLossPrice;              // ֹ��ֹӯ��
    TX1FtdcOrderTypeType                  OrderType;                    // �������
    TX1FtdcOrderPropertyType              OrderProperty;                // ��������
};

///�ɽ���ѯ������Ӧ
struct APISTRUCT CX1FtdcRspMatchField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcX1OrderIDType                  X1OrderID;                    // ��̨ί�к�
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˺�
    TX1FtdcExchangeIDType                 ExchangeID;                   // ������ID
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcBuySellTypeType                BuySellType;                  // ����
    TX1FtdcOpenCloseTypeType              OpenClose;                    // ��ƽ
    TX1FtdcPriceType                      MatchedPrice;                 // �ɽ��۸�
    TX1FtdcAmountType                     MatchedAmount;                // �ɽ�����
    TX1FtdcPriceType                      MatchedMort;                  // �ɽ����
    TX1FtdcSpeculatorType                 Speculator;                   // Ͷ�����
    TX1FtdcDateType                       MatchedTime;                  // �ɽ�ʱ��
    TX1FtdcMatchIDType                    MatchedID;                    // �ɽ����
    TX1FtdcLocalOrderIDType               LocalOrderID;                 // ����ί�к�
    TX1FtdcClientIDType                   ClientID;                     // ���ױ���
    TX1FtdcMatchType                      MatchType;                    // �ɽ�����
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����
    TX1FtdcSessionIDType                  SessionId;                    // �ỰID
    TX1FtdcReservedType                   ReservedType2;                // Ԥ���ֶ�2
    TX1FtdcCustomCategoryType             CustomCategory;               // �Զ������
    TX1FtdcPriceType                      Fee;                          // ������
    TX1FtdcOrderTypeType                  OrderType;                    // ��������
    TX1FtdcOrderSysIDType                 OrderSysID;                   // ��������
};

///���غ�Լ��Ϣ���ݽṹ 
struct APISTRUCT CX1FtdcRspSpecificInstrumentField {
    TX1FtdcRequestIDType                  RequestID;                    // ������
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcRatioType                      LongMarginRatio;              // ��ͷ��֤����
    TX1FtdcRatioType                      ShortMarginRatio;             // ��ͷ��֤����
    TX1FtdcPriceType                      LongMarginRatioByVolume;      // ��ͷ��֤���(����)
    TX1FtdcPriceType                      ShortMarginRatioByVolume;     // ��ͷ��֤���(����)
    TX1FtdcRatioType                      OpenFeeVolRatio;              // ���������� ����������
    TX1FtdcRatioType                      CloseFeeVolRatio;             // ƽ�������� ����������
    TX1FtdcRatioType                      CloseTodayFeeVolRatio;        // ƽ�������� ����������
    TX1FtdcRatioType                      OpenFeeAmtRatio;              // ������������ ��������
    TX1FtdcRatioType                      CloseFeeAmtRatio;             // ƽ���������� ��������
    TX1FtdcRatioType                      CloseTodayFeeAmtRatio;        // ƽ���������� ��������
    TX1FtdcAmountType                     OrderTopLimit;                // �޼�ί������
    TX1FtdcPriceType                      ContractMultiplier;           // ��Լ����
    TX1FtdcPriceType                      MinimumPriceChange;           // ��С�䶯��λ
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����
    TX1FtdcInstrumentMaturityType         InstrumentMaturity;           // ��Լ�������
    TX1FtdcComputeModeType                ComputeMode;                  // ���㷽ʽ             
    TX1FtdcPriceType                      AtMoneyNorm;                  // ƽֵ������
    TX1FtdcPriceType                      UpperLimitPrice;              // ��ͣ���
    TX1FtdcPriceType                      LowerLimitPrice;              // ��ͣ���
    TX1FtdcPriceType                      PreClosePrice;                // ������
    TX1FtdcPriceType                      PreSettlementPrice;           // ������
    TX1FtdcPriceType                      SettlementPrice;              // �����
    TX1FtdcAmountType                     PreOpenInterest;              // ��ֲ���
    TX1FtdcRatioType                      OptExecRatio;                 // ��Ȩ����Ȩ������ �ڻ����������
    TX1FtdcRatioType                      OptExecRatioPerVol;           // ��Ȩ����Ȩ������ �ڻ����������
    TX1FtdcCalculateMode                  CalculateMode;                // �����ʽ���㷽ʽ
    TX1FtdcPriceType                      ReferencePrice;               // �ο��۸�
};

///��ѯ�ֲ���ϸ
struct APISTRUCT CX1FtdcQryPositionDetailField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˻�ID
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����
};

///��ѯ�ֲ���ϸ��Ӧ
struct APISTRUCT CX1FtdcRspPositionDetailField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��ʺ�ID
    TX1FtdcExchangeIDType                 ExchangeID;                   // ����������
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ��
    TX1FtdcBuySellTypeType                BuySellType;                  // ����
    TX1FtdcPriceType                      OpenPrice;                    // ���ּ�
    TX1FtdcAmountType                     Volume;                       // ����
    TX1FtdcMatchIDType                    MatchID;                      // �ɽ����
    TX1FtdcDateType                       MatchedDate;                  // �ɽ�����
    TX1FtdcProfitLossType                 DatePositionProfitLoss;       // ���гֲ�ӯ��
    TX1FtdcProfitLossType                 DateCloseProfitLoss;          // ����ƽ��ӯ��
    TX1FtdcProfitLossType                 ProfitLoss;                   // ����ӯ��
    TX1FtdcProfitLossType                 Margin;                       // ռ�ñ�֤��
    TX1FtdcSpeculatorType                 Speculator;                   // Ͷ�����
    TX1FtdcClientIDType                   ClientID;                     // ���ױ���
    TX1FtdcPriceType                      PreSettlementPrice;           // ������
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����
    TX1FtdcX1OrderIDType                  X1OrderID;                    // ��̨ί�к�
    TX1FtdcCustomCategoryType             CustomCategory;               // �Զ������
    TX1FtdcAmountType                     CloseOrderVol;                // ƽ��ί������
    TX1FtdcAmountType                     CloseMatchVol;                // ƽ�ֳɽ�����
    TX1FtdcPositionDateType               PositionDateType;             // �ֲ���������
};

///����״̬��ѯ����
struct APISTRUCT CX1FtdcQryExchangeStatusField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcExchangeIDType                 ExchangeID;                   // ����������
};

///������״̬��ѯ��Ӧ
struct APISTRUCT CX1FtdcRspExchangeStatusField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcExchangeStatusType             ExchangeStatus;               // ������״̬
    TX1FtdcExchangeIDType                 ExchangeID;                   // ����������
};

///������״̬֪ͨ
struct APISTRUCT CX1FtdcExchangeStatusRtnField {
    TX1FtdcExchangeIDType                 ExchangeID;                   // ������
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcExchangeStatusType             ExchangeStatus;               // ������״̬
};

///�����̱�������
struct APISTRUCT CX1FtdcQuoteInsertField {
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˺�
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcLocalOrderIDType               LocalOrderID;                 // ����ί�к�
    TX1FtdcInsertType                     InsertType;                   // �Զ������
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcQuoteIDType                    QuoteID;                      // ѯ�۱��
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ���� 
    TX1FtdcAmountType                     BuyOrderAmount;               // ��������(��)
    TX1FtdcAmountType                     SellOrderAmount;              // ��������(��)
    TX1FtdcPriceType                      BuyInsertPrice;               // ί�м۸�(��)         
    TX1FtdcPriceType                      SellInsertPrice;              // ί�м۸�(��)          
    TX1FtdcOpenCloseTypeType              BuyOpenCloseType;             // ��ƽ��־(��)  
    TX1FtdcOpenCloseTypeType              SellOpenCloseType;            // ��ƽ��־(��)
    TX1FtdcSpeculatorType                 BuySpeculator;                // Ͷ�����(��)       
    TX1FtdcSpeculatorType                 SellSpeculator;               // Ͷ�����(��)          
    TX1FtdcStayTimeType                   StayTime;                     // ͣ��ʱ�䣬��֧��֣�ݡ��������������Ϊ0 
    TX1FtdcCustomCategoryType             CustomCategory;               // �Զ������
};

///������������
struct APISTRUCT CX1FtdcCancelAllOrderField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˻�ID(�˰汾������)
    TX1FtdcExchangeIDType                 ExchangeID;                   // ����������(�˰汾������)
};

///ѯ������
struct APISTRUCT CX1FtdcForQuoteField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˻�ID
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
};

///�����̱�����Ӧ
struct APISTRUCT CX1FtdcQuoteRspField {
    TX1FtdcLocalOrderIDType               LocalOrderID;                 // ����ί�к�              
    TX1FtdcX1OrderIDType                  X1OrderID;                    // ��̨ί�к�         
    TX1FtdcRequestIDType                  RequestID;                    // ����ID                    
    TX1FtdcPriceType                      Fee;                          // ������(������ʹ��)(�˰汾������)          
    TX1FtdcPriceType                      Margin;                       // ��֤��(������ʹ��)
    TX1FtdcDateType                       OrderTime;                    // ί��ʱ��(������ʹ��)(�˰汾������)  
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  // ί��״̬
    TX1FtdcCustomCategoryType             CustomCategory;               // �Զ������
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����(�˰汾������)
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˺�(�˰汾������)
    TX1FtdcQuoteIDType                    QuoteID;                      // ѯ�۱��(��������Ӧʹ��)
    TX1FtdcSessionIDType                  SessionID;                    // �ỰID 
    TX1FtdcClientIDType                   ClientID;                     // ���ױ���(�˰汾������)
};

///�����̱����ر�
struct APISTRUCT CX1FtdcQuoteRtnField {
    TX1FtdcExchangeIDType                 ExchangeID;                   // ������                   
    TX1FtdcClientIDType                   ClientID;                     // ���ױ���  
    TX1FtdcOrderSysIDType                 OrderSysID;                   // �������
    TX1FtdcOrderSysIDType                 OrderSysIDTwo;                // ˫�ߵڶ���
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����                   
    TX1FtdcLocalOrderIDType               LocalOrderID;                 // ����ί�к�                
    TX1FtdcSeatCodeType                   SeatCode;                     // ϯλ����                  
    TX1FtdcOpenCloseTypeType              BuyOpenCloseType;             // ��ƽ��־(��)          
    TX1FtdcOpenCloseTypeType              SellOpenCloseType;            // ��ƽ��־(��)             
    TX1FtdcSpeculatorType                 Speculator;                   // Ͷ�����                       
    TX1FtdcAmountType                     BuyOrderAmount;               // ί������(��)            
    TX1FtdcAmountType                     SellOrderAmount;              // ί������(��)           
    TX1FtdcPriceType                      BuyInsertPrice;               // ί�м�  (��)               
    TX1FtdcPriceType                      SellInsertPrice;              // ί�м�  (��)               
    TX1FtdcX1OrderIDType                  X1OrderID;                    // ��̨ί�к� ���صĹ�̨ί�к�Ϊ��һ�ߵĹ�̨ί�кţ��ڶ��ߵĹ�̨ί�к�ʵ�����ǵ�һ���̨ί�кż�1������Ҫ�û����ġ�������������ָ�������̨ί�к�������˫�߳�������֧��ֻ��������һ�ߵĲ���                             
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˺�(�˰汾������)                     
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����                  
    TX1FtdcDateType                       SuspendTime;                  // �ҵ�ʱ��(�˰汾������)                               
    TX1FtdcEntrusTellerType               EntrusTeller;                 // ί�й�Ա (�˰汾������)
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  // ί��״̬ 
    TX1FtdcSessionIDType                  SessionID;                    // �ỰID
    TX1FtdcQuoteIDType                    QuoteID;                      // ѯ�۱��
    TX1FtdcErrorIDType                    ErrorID;                      // ����ID   2�µ�֮ǰ���
    TX1FtdcErrorMsgInfoType               StatusMsg;                    // ������Ϣ
    TX1FtdcCustomCategoryType             CustomCategory;               // �Զ������
    TX1FtdcLocalOrderIDType               PriFlowNo;                    //  ˽�������
};

///�����̳����ر�
struct APISTRUCT CX1FtdcQuoteCanceledRtnField {
    TX1FtdcExchangeIDType                 ExchangeID;                   // ������               
    TX1FtdcClientIDType                   ClientID;                     // ���ױ���(�˰汾������)  
    TX1FtdcOrderSysIDType                 OrderSysID;                   // �������
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcLocalOrderIDType               LocalOrderID;                 // ����ί�к�                
    TX1FtdcSeatCodeType                   SeatCode;                     // ϯλ����                  
    TX1FtdcOpenCloseTypeType              BuyOpenCloseType;             // ��ƽ��־(��)      
    TX1FtdcOpenCloseTypeType              SellOpenCloseType;            // ��ƽ��־(��)             
    TX1FtdcSpeculatorType                 Speculator;                   // Ͷ�����                       
    TX1FtdcX1OrderIDType                  X1OrderID;                    // ��̨ί�к�                               
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˺�(�˰汾������)                     
    TX1FtdcEntrusTellerType               EntrusTeller;                 // ί�й�Ա(�˰汾������)       
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  // ί��״̬ 
    TX1FtdcAmountType                     CancelAmount;                 // ��������                              
    TX1FtdcPriceType                      Fee;                          // �ⶳ������(�˰汾������)                  
    TX1FtdcPriceType                      Margin;                       // �ⶳ��֤��(�˰汾������) 
    TX1FtdcSessionIDType                  SessionID;                    // �ỰID
    TX1FtdcBuySellTypeType                BuySellType;                  // ������־
    TX1FtdcQuoteIDType                    QuoteID;                      // ѯ�۱��
    TX1FtdcDateType                       CanceledTime;                 // ����ʱ��
    TX1FtdcCustomCategoryType             CustomCategory;               // �Զ������
    TX1FtdcErrorIDType                    ErrorID;                      // ����ID  2�µ�֮ǰ���
    TX1FtdcErrorMsgInfoType               StatusMsg;                    // ״̬��Ϣ
    TX1FtdcLocalOrderIDType               PriFlowNo;                    // ˽�������
};

///�����̳ɽ��ر�
struct APISTRUCT CX1FtdcQuoteMatchRtnField {
    TX1FtdcExchangeIDType                 ExchangeID;                   // ������ID
    TX1FtdcClientIDType                   ClientID;                     // ���ױ���(�˰汾������)
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcSeatCodeType                   SeatCode;                     // ϯλ����
    TX1FtdcLocalOrderIDType               LocalOrderID;                 // ����ί�к�
    TX1FtdcOpenCloseTypeType              OpenCloseType;                // ��ƽ��־
    TX1FtdcSpeculatorType                 Speculator;                   // Ͷ�����  
    TX1FtdcX1OrderIDType                  X1OrderID;                    // ��̨ί�к�  
    TX1FtdcOrderSysIDType                 OrderSysID;                   // �������
    TX1FtdcMatchIDType                    MatchID;                      // �ɽ����
    TX1FtdcAmountType                     MatchedAmount;                // �ɽ�����
    TX1FtdcPriceType                      MatchedPrice;                 // �ɽ��۸�
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˺�(�˰汾������)
    TX1FtdcPriceType                      Turnover;                     // �ɽ����(�˰汾������)
    TX1FtdcEntrusTellerType               EntrusTeller;                 // ί�й�Ա(�˰汾������)
    TX1FtdcDateType                       MatchedTime;                  // �ɽ�ʱ��
    TX1FtdcFeeType                        Fee;                          // ������(�˰汾������)
    TX1FtdcPriceType                      InsertPrice;                  // ί�м۸�(�˰汾������)
    TX1FtdcAmountType                     OrderAmount;                  // ί������
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  // �걨���,�ɽ��ɹ�12��ȫ���ɽ�����û����м���
    TX1FtdcPriceType                      Margin;                       // ����Ϊ��֤��,ƽ��Ϊ�ⶳ��֤��(�˰汾����)
    TX1FtdcBuySellTypeType                BuySellType;                  // ����
    TX1FtdcAmountType                     CloseTodayAmount;             // ƽ������(�˰汾������)
    TX1FtdcPriceType                      ClosePrice;                   // ƽ�ֽ��(�˰汾������)
    TX1FtdcPriceType                      CloseTodayPrice;              // ƽ����(�˰汾������)
    TX1FtdcAdjustmentInfoType             AdjustmentInfo;               // ��ϻ�����ı�֤�������Ϣ,��ʽ:[��Լ����,������־,Ͷ�����,�������;](�˰汾����) 
    TX1FtdcPriceType                      FrozenCapita;                 // �ɽ��ⶳί�ж�����ʽ�(�˰汾������)
    TX1FtdcProfitLossType                 DateCloseProfitLoss;          // ����ƽ��ӯ��(�˰汾������)
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����
    TX1FtdcSessionIDType                  SessionID;                    // �Ự��ʶ
    TX1FtdcLargeMarginDirectType          LargeMarginDirect;            // ��߱�֤����(�˰汾������)
    TX1FtdcQuoteIDType                    QuoteID;                      // ѯ�۱��
    TX1FtdcCustomCategoryType             CustomCategory;               // �Զ������
    TX1FtdcLocalOrderIDType               PriFlowNo;                    // ˽�������
};

///����������Ӧ
struct APISTRUCT CX1FtdcCancelAllOrderRspField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˻�ID(�˰汾������)
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  // ί��״̬
};

///ѯ��������Ӧ
struct APISTRUCT CX1FtdcForQuoteRspField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcX1OrderIDType                  X1OrderID;                    // ��̨ί�к�
    TX1FtdcDateType                       CommTime;                     // ί��ʱ��(�˰汾������)
};

///ѯ�ۻر�
struct APISTRUCT CX1FtdcForQuoteRtnField {
    TX1FtdcX1OrderIDType                  X1OrderID;                    // ��̨ί�к�
    TX1FtdcSessionIDType                  SessionID;                    // �ỰID
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcExchangeIDType                 ExchangeID;                   // ������
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˺�(�˰汾������)
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  // ί��״̬
    TX1FtdcErrorIDType                    ErrorID;                      // ����ID
    TX1FtdcLocalOrderIDType               PriFlowNo;                    // ˽�������
};

///��ѯѯ������
struct APISTRUCT CX1FtdcQryForQuoteField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˻�ID
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcExchangeIDType                 ExchangeID;                   // ������
};

///�����̱���ί�в�ѯ
struct APISTRUCT CX1FtdcQuoteOrderField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcExchangeIDType                 ExchangeID;                   // ������
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˻�ID
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcLocalOrderIDType               LocalOrderID;                 // ����ί�к�
    TX1FtdcX1OrderIDType                  X1OrderID;                    // ��̨ί�к�
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  // ί��״̬
};

///ѯ��֪ͨ��ѯ����
struct APISTRUCT CX1FtdcQryQuoteNoticeField {
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˺�
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcExchangeIDType                 ExchangeID;                   // ������
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
};

///��ѯѯ����Ӧ
struct APISTRUCT CX1FtdcQryForQuoteRtnField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˺�
    TX1FtdcX1OrderIDType                  X1OrderID;                    // ��̨ί�к�
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcExchangeIDType                 ExchangeID;                   // ������
    TX1FtdcDateType                       SuspendTime;                  // ����ʱ��
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  // ί��״̬
};

///�����̱��۲�ѯ��Ӧ
struct APISTRUCT CX1FtdcQuoteOrderRtnField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcX1OrderIDType                  X1OrderID;                    // ��̨ί�к�
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  // ί��״̬
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcPriceType                      Margin;                       // ��֤��
    TX1FtdcPriceType                      Fee;                          // ������
    TX1FtdcLocalOrderIDType               LocalOrderID;                 // ����ί�к�
    TX1FtdcAccountIDType                  AccountID;                    // �ͻ���
    TX1FtdcDateType                       CommTime;                     // ί��ʱ��
    TX1FtdcDateType                       SubmitTime;                   // �걨ʱ��
    TX1FtdcExchangeIDType                 ExchangeID;                   // ������ID
    TX1FtdcAmountType                     BuyOrderAmount;               // ί������(��)
    TX1FtdcAmountType                     BuyMatchedAmount;             // �ɽ�����(��)
    TX1FtdcAmountType                     BuyCancelAmount;              // ��������(��)
    TX1FtdcPriceType                      BuyInsertPrice;               // ί�м۸�(��)
    TX1FtdcPriceType                      BuyMatchedPrice;              // �ɽ��۸�(��)
    TX1FtdcOpenCloseTypeType              BuyOpenCloseType;             // ��ƽ��־(��)
    TX1FtdcAmountType                     SellOrderAmount;              // ί������(��)
    TX1FtdcAmountType                     SellMatchedAmount;            // �ɽ�����(��)
    TX1FtdcAmountType                     SellCancelAmount;             // ��������(��)
    TX1FtdcPriceType                      SellInsertPrice;              // ί�м۸�(��)
    TX1FtdcPriceType                      SellMatchedPrice;             // �ɽ��۸�(��)
    TX1FtdcOpenCloseTypeType              SellOpenCloseType;            // ��ƽ��־(��)
    TX1FtdcFrontAddrType                  OperStation;                  // ����վ��
    TX1FtdcSessionIDType                  SessionID;                    // �ỰID
    TX1FtdcQuoteIDType                    QuoteID;                      // ѯ�۱��
    TX1FtdcCustomCategoryType             CustomCategory;               // �Զ������
};

///ѯ��֪ͨ��ѯ��Ӧ
struct APISTRUCT CX1FtdcQryQuoteNoticeRtnField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcQuoteIDType                    QuoteID;                      // ѯ�۱��
    TX1FtdcExchangeIDType                 ExchangeID;                   // ������
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcSourceType                     Source;                       // ��Դ
    TX1FtdcDateType                       QuoteTime;                    // ѯ��ʱ��
};

///������Լ��ѯ
/**
* @details ��exchangeIDΪ��ʱ����ʾ��ѯ�������������к�Լ���룬Ҳ��ָ����ѯĳ�����������к�Լ������Ϣ��
*/
struct  APISTRUCT CX1FtdcAbiInstrumentField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˻�ID
    TX1FtdcExchangeIDType                 ExchangeID;                   // ����������
};

///������Լ������Ϣ
struct APISTRUCT CX1FtdcAbiInstrumentRtnField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcExchangeIDType                 ExchangeID;                   // ����������
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcInstrumentNameType             InstrumentName;               // ��Լ����
    TX1FtdcPriceType                      UpperLimitPrice;              // ��ͣ���
    TX1FtdcPriceType                      LowerLimitPrice;              // ��ͣ���        
    TX1FtdcPriceType                      PriceTick;                    // ��С�䶯��λ
};

///ѯ��֪ͨ���Ļر�
struct APISTRUCT CX1FtdcQuoteSubscribeRtnField {
    TX1FtdcQuoteIDType                    QuoteID;                      // ѯ�۱��
    TX1FtdcExchangeIDType                 ExchangeID;                   // ������
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcSourceType                     Source;                       // ��Դ
    TX1FtdcDateType                       QuoteTime;                    // ѯ��ʱ��
};

///�޸Ŀͻ�����
struct APISTRUCT CX1FtdcReqResetPasswordField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID [NM]
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˻�ID [M]
    TX1FtdcPasswordType                   OldPassword;                  // ���� [M]
    TX1FtdcPasswordType                   NewPassword;                  // ���� [M]
};

///�޸Ŀͻ�������Ӧ
struct APISTRUCT CX1FtdcRspResetPasswordField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID[BLK]
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��ʺ�ID [M]
    TX1FtdcResetPasswordType              ResetPasswordResult;          // �޸�������[M] 
};

///��ѯ�����ֲ���ϸ
struct APISTRUCT CX1FtdcArbitrageCombineDetailField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��˻�ID
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����
};

///��ѯ�����ֲ���Ӧ
struct APISTRUCT CX1FtdcArbitrageCombineDetailRtnField {
    TX1FtdcRequestIDType                  RequestID;                    // ����ID
    TX1FtdcAccountIDType                  AccountID;                    // �ʽ��ʺ�ID
    TX1FtdcExchangeIDType                 ExchangeID;                   // ����������
    TX1FtdcInstrumentIDType               InstrumentID;                 // ��Լ����
    TX1FtdcBuySellTypeType                BuySellType;                  // ����
    TX1FtdcPriceType                      OpenPrice;                    // ���ּ�
    TX1FtdcInstrumentIDType               ComTradeID;                   // ��ϱ��
    TX1FtdcDateType                       OpenDate;                     // ��������
    TX1FtdcMatchIDType                    MatchID;                      // �ɽ����/������
    TX1FtdcAmountType                     Volume;                       // �ֲ�����
    TX1FtdcAmountType                     TradingAmount;                // �ҵ�����
    TX1FtdcProfitLossType                 Margin;                       // ��֤��
    TX1FtdcSpeculatorType                 Speculator;                   // Ͷ�����
    TX1FtdcInstrumentTypeType             InstrumentType;               // ��Լ����
    TX1FtdcX1OrderIDType                  SpdOrderID;                   // ��̨ί�к�
    TX1FtdcCustomCategoryType             CustomCategory;               // �Զ������
    TX1FtdcClientIDType                   ClientID;                     // ���ױ��� 
    TX1FtdcAmountType                     YesterdayVolume;              // �������
};

///���鴥������
struct APISTRUCT CX1FtdcPricesTriggerField {
    TX1FtdcAccountIDType              AccountID;          // �ʽ��˻�ID
    TX1FtdcLocalOrderIDType           LocalOrderID;       // ����ί�к�
    TX1FtdcInstrumentIDType           InstrumentID;       // ��Լ����
    TX1FtdcPriceType                  InsertPrice;        // ί�м۸�
    TX1FtdcAmountType                 OrderAmount;        // ί������
    TX1FtdcSpeculatorType             Speculator;         // Ͷ������
    TX1FtdcOrderTypeType              OrderType;          // ��������������
    TX1FtdcBuySellTypeType            BuySellType;        // ������־
    TX1FtdcOpenCloseTypeType          OpenCloseType;      // ��ƽ��־
    TX1FtdcRequestIDType              RequestID;          // ����ID 
    TX1FtdcCompareFlagType            CompareFlag;        // �Ƚϱ�־(�۸�)
    TX1FtdcPriceType                  ComparePrice;       // �����۸�(��������Ϊ����۸���ڻ�С�ڵ��ڴ����۸�)
    TX1FtdcPriceReferenceType         PriceReference;     // �۸����
    TX1FtdcBreakDownTimesType         BreakDownTimes;     // ��������
    TX1FtdcDateType                   Validate;           // ��Ч����(yyyy.mm.dd,�ݲ�֧��)
    TX1FtdcFrozenTypeType             Frozentype;         // �Ƿ񶳽��ʽ�����
    TX1FtdcInstrumentTypeType         InstrumentType;     // ��Լ����
    TX1FtdcAmountType                 LimitAmount;        // ��������
    TX1FtdcCompareFlagType            QtyCmpFlag;         // �Ƚϱ�־(����)
    TX1FtdcTriggerType                TriggerType;        // ��������
    TX1FtdcBreakDownTypeType          BreakDownType;      // ��������
};

///��������ѯ����
struct APISTRUCT CX1FtdcQryExtOrderField {
    TX1FtdcAccountIDType              AccountID;          // �ʽ��˻�ID
    TX1FtdcRequestIDType              RequestID;          // ����ID
    TX1FtdcX1OrderIDType              ExtSpdOrderID;      // ���������(�ݲ�֧��)
    TX1FtdcExtOrderType               ExtOrderType;       // ����������
};

///��������������
struct APISTRUCT CX1FtdcCancelExtOrderField {
    TX1FtdcAccountIDType              AccountID;          // �ʽ��˻�ID
    TX1FtdcRequestIDType              RequestID;          // ����ID 
    TX1FtdcLocalOrderIDType           LocalOrderID;       // ����ί�к�
    TX1FtdcX1OrderIDType              ExtSpdOrderID;      // ���������
    TX1FtdcExtOrderType               ExtOrderType;       // ����������
};

///������ί����Ӧ
struct APISTRUCT CX1FtdcExtOrderRspDataField {
    TX1FtdcLocalOrderIDType           LocalOrderID;       // ����ί�к�
    TX1FtdcX1OrderIDType              SpdOrderID;         // ��̨ί�к�
    TX1FtdcX1OrderIDType              ExtSpdOrderID;      // ���������
    TX1FtdcOrderAnswerStatusType      OrderStatus;        // ί��״̬
    TX1FtdcRequestIDType              RequestID;          // ����ID
    TX1FtdcPriceType                  FrozenMargin;       // �����ʽ�(���µ�ʹ��)
    TX1FtdcExtOrderType               ExtOrderType;       // ����������
    TX1FtdcAccountIDType              AccountID;          // �ʽ��˻�ID
};

///������ί�лر�
struct APISTRUCT CX1FtdcPricesTriggerRtnField {
    TX1FtdcAccountIDType              AccountID;          // �ʽ��˻�ID
    TX1FtdcLocalOrderIDType           LocalOrderID;       // ����ί�к�
    TX1FtdcInstrumentIDType           InstrumentID;       // ��Լ����
    TX1FtdcPriceType                  InsertPrice;        // ί�м۸�
    TX1FtdcAmountType                 OrderAmount;        // ί������
    TX1FtdcSpeculatorType             Speculator;         // Ͷ������
    TX1FtdcOrderTypeType              OrderType;          // ��������������
    TX1FtdcBuySellTypeType            BuySellType;        // ������־
    TX1FtdcOpenCloseTypeType          OpenCloseType;      // ��ƽ��־
    TX1FtdcCompareFlagType            CompareFlag;        // �Ƚϱ�־
    TX1FtdcPriceType                  ComparePrice;       // �����۸�
    TX1FtdcPriceReferenceType         PriceReference;     // �۸����
    TX1FtdcBreakDownTimesType         BreakDownTimes;     // ��������
    TX1FtdcDateType                   Modifiedtime;       // �޸�ʱ��
    TX1FtdcDateType                   CommTime;           // ί��ʱ��
    TX1FtdcX1OrderIDType              SpdOrderID;         // ��̨ί�к�
    TX1FtdcDateType                   CanceledTime;       // ����ʱ��
    TX1FtdcFrozenTypeType             Frozentype;         // ��������
    TX1FtdcX1OrderIDType              ExtSpdOrderID;      // ���������
    TX1FtdcProfitLossType             FrozenMargin;       // �����ʽ�
    TX1FtdcAmountType                 FrozenAmount;       // ��������
    TX1FtdcOrderAnswerStatusType      OrderStatus;        // ί��״̬
    TX1FtdcSessionIDType              SessionID;          // �ỰID
    TX1FtdcErrorMsgInfoType           StatusMsg;          // ״̬��Ϣ
    TX1FtdcAmountType                 LimitAmount;        // ��������
    TX1FtdcCompareFlagType            QtyCmpFlag;         // �Ƚϱ�־(����)
    TX1FtdcTriggerType                TriggerType;        // ��������
    TX1FtdcBreakDownTypeType          BreakDownType;      // ��������
    TX1FtdcExtOrderTriggerStatusType  OrderTriggerStatus; // ����������״̬
};

///���鴥����ѯ��Ӧ
struct APISTRUCT CX1FtdcQryPricesTriggerField {
    TX1FtdcAccountIDType              AccountID;          // �ʽ��˻�ID
    TX1FtdcLocalOrderIDType           LocalOrderID;       // ����ί�к�
    TX1FtdcInstrumentIDType           InstrumentID;       // ��Լ����
    TX1FtdcPriceType                  InsertPrice;        // ί�м۸�
    TX1FtdcAmountType                 OrderAmount;        // ί������
    TX1FtdcSpeculatorType             Speculator;         // Ͷ������
    TX1FtdcOrderTypeType              OrderType;          // ��������
    TX1FtdcBuySellTypeType            BuySellType;        // ������־
    TX1FtdcOpenCloseTypeType          OpenCloseType;      // ��ƽ��־
    TX1FtdcRequestIDType              RequestID;          // ����ID 
    TX1FtdcCompareFlagType            CompareFlag;        // �Ƚϱ�־
    TX1FtdcPriceType                  ComparePrice;       // �����۸�
    TX1FtdcPriceReferenceType         PriceReference;     // �۸����
    TX1FtdcBreakDownTimesType         BreakDownTimes;     // ��������
    TX1FtdcDateType                   Validate;           // ��Ч����
    TX1FtdcDateType                   Modifiedtime;       // �޸�ʱ��
    TX1FtdcDateType                   CommTime;           // ί��ʱ��
    TX1FtdcDateType                   Commdate;           // ί������
    TX1FtdcX1OrderIDType              SpdOrderID;         // ��̨ί�к�
    TX1FtdcDateType                   CanceledTime;       // ����ʱ��
    TX1FtdcFrozenTypeType             Frozentype;         // ��������
    TX1FtdcX1OrderIDType              ExtSpdOrderID;      // ���������
    TX1FtdcExtOrderType               ExtOrderType;       // ����������
    TX1FtdcProfitLossType             FrozenMargin;       // �����ʽ�
    TX1FtdcAmountType                 FrozenAmount;       // ��������
    TX1FtdcOrderAnswerStatusType      OrderStatus;        // ί��״̬
    TX1FtdcExtOrderTriggerStatusType  OrderTriggerStatus; // ����������״̬
    TX1FtdcAmountType                 LimitAmount;        // ��������
    TX1FtdcCompareFlagType            QtyCmpFlag;         // �Ƚϱ�־(����)
    TX1FtdcTriggerType                TriggerType;        // ��������
    TX1FtdcBreakDownTypeType          BreakDownType;      // ��������
};

///��ѯ����������
struct APISTRUCT CX1FtdcTradingDayField {
    TX1FtdcRequestIDType              RequestID;         // ����ID
};

///������������Ӧ
struct APISTRUCT CX1FtdcTradingDayRtnField {
    TX1FtdcRequestIDType              RequestID;           // ����ID
    TX1FtdcDateType                   date;                // ������
};

///��Ȩ�Գ�����
struct APISTRUCT CX1FtdcOptOffsetField {
    TX1FtdcRequestIDType              RequestID;           // ����ID
    TX1FtdcAccountIDType              AccountID;           // �ʽ��˻�ID
    TX1FtdcSpeculatorType             Speculator;          // Ͷ������
    TX1FtdcExchangeIDType             ExchangeID;          // ����������
    TX1FtdcInstrumentIDType           InstrumentID;        // ��Լ����
    TX1FtdcOffsetFlagType             IfOffset;            // �Ƿ�Գ壨����ʱ���ã�
};

///��Ȩ�Գ��ѯ����
struct APISTRUCT CX1FtdcQryOptOffsetField {
    TX1FtdcRequestIDType              RequestID;           // ����ID
    TX1FtdcAccountIDType              AccountID;           // �ʽ��˻�ID
    TX1FtdcSpeculatorType             Speculator;          // Ͷ������
    TX1FtdcExchangeIDType             ExchangeID;          // ����������
    TX1FtdcInstrumentIDType           InstrumentID;        // ��Լ����
};

///��Ȩ�Գ���Ӧ
struct APISTRUCT CX1FtdcRspOptOffsetField {
    TX1FtdcRequestIDType              RequestID;           // ����ID
    TX1FtdcAccountIDType              AccountID;           // �ʽ��˻�ID
    TX1FtdcExchangeIDType             ExchangeID;          // ����������
    TX1FtdcInstrumentIDType           InstrumentID;        // ��Լ����
    TX1FtdcOffsetFlagType             IfOffset;            // �Ƿ�Գ壨����ʱ���ã�
};

///��Ȩ�Գ�ر�
struct APISTRUCT CX1FtdcRtnOptOffsetField {
    TX1FtdcExchangeIDType             ExchangeID;          // ����������
    TX1FtdcAccountIDType              AccountID;           // �ʽ��˻�ID
    TX1FtdcInstrumentIDType           InstrumentID;        // ��Լ����
    TX1FtdcOffsetFlagType             IfOffset;            // �Ƿ�Գ�
    TX1FtdcErrorIDType                ErrorID;             // ����ID
    TX1FtdcErrorMsgInfoType           StatusMsg;           // ״̬��Ϣ
};

///��Լ�Գ�����
struct APISTRUCT CX1FtdcPerformOffsetField {
    TX1FtdcRequestIDType              RequestID;           // ����ID
    TX1FtdcAccountIDType              AccountID;           // �ʽ��˻�ID
    TX1FtdcSpeculatorType             Speculator;          // Ͷ������
    TX1FtdcExchangeIDType             ExchangeID;          // ����������
    TX1FtdcOffsetFlagType             IfOffset;            // �Ƿ�Գ�(����ʱ����)
};

///��Լ�Գ��ѯ����
struct APISTRUCT CX1FtdcQryPerformOffsetField {
    TX1FtdcRequestIDType              RequestID;           // ����ID
    TX1FtdcExchangeIDType             ExchangeID;          // ����������
    TX1FtdcAccountIDType              AccountID;           // �ʽ��˻�ID
    TX1FtdcSpeculatorType             Speculator;          // Ͷ������
};

///��Լ�Գ���Ӧ
struct APISTRUCT CX1FtdcRspPerformOffsetField {
    TX1FtdcRequestIDType              RequestID;           // ����ID
    TX1FtdcExchangeIDType             ExchangeID;          // ����������
    TX1FtdcAccountIDType              AccountID;           // �ʽ��˻�ID
    TX1FtdcOffsetFlagType             IfOffset;            // �Ƿ�Գ�
};

///��Լ�Գ�ر�
struct APISTRUCT CX1FtdcRtnPerformOffsetField {
    TX1FtdcExchangeIDType             ExchangeID;          // ����������
    TX1FtdcAccountIDType              AccountID;           // �ʽ��˻�ID
    TX1FtdcOffsetFlagType             IfOffset;            // �Ƿ�Գ�
    TX1FtdcErrorIDType                ErrorID;             // ����ID
    TX1FtdcErrorMsgInfoType           StatusMsg;           // ״̬��Ϣ
};

///���/������������
struct APISTRUCT CX1FtdcCombPosiField {
    TX1FtdcRequestIDType              RequestID;           // ����ID
    TX1FtdcLocalOrderIDType           LocalOrderID;        // ����ί�к�
    TX1FtdcExchangeIDType             ExchangeID;          // ����������
    TX1FtdcAccountIDType              AccountID;           // �ʽ��˻�ID
    TX1FtdcSpeculatorType             Speculator;          // Ͷ������(�˰汾�����ã�Ԥ��)
    TX1FtdcInstrumentIDType           ComInstrumentID;     // ��Ϻ�Լ����
    TX1FtdcComSpeculatorType          ComSpeculator;       // ���Ͷ����־
    TX1FtdcComStatusType              ComStatus;           // ���״̬
    TX1FtdcAmountType                 ComAmount;           // �������
};

///���/����������Ӧ
struct APISTRUCT CX1FtdcRspCombPosiField {
    TX1FtdcRequestIDType              RequestID;           // ����ID
    TX1FtdcLocalOrderIDType           LocalOrderID;        // ����ί�к�
    TX1FtdcX1OrderIDType              X1OrderID;           // ��̨ί�к�
    TX1FtdcSessionIDType              SessionID;           // �ỰID
    TX1FtdcOrderAnswerStatusType      OrderStatus;         // ί��״̬
    TX1FtdcExchangeIDType             ExchangeID;          // ����������
    TX1FtdcAccountIDType              AccountID;           // �ʽ��˻�ID
    TX1FtdcInstrumentIDType           ComInstrumentID;     // ��Ϻ�Լ����
    TX1FtdcComSpeculatorType          ComSpeculator;       // ���Ͷ����־
    TX1FtdcComStatusType              ComStatus;           // ���״̬
    TX1FtdcAmountType                 ComAmount;           // �������
};

///���/��������ر�
struct APISTRUCT CX1FtdcRtnCombPosiField {
    TX1FtdcLocalOrderIDType           LocalOrderID;        // ����ί�к�
    TX1FtdcX1OrderIDType              X1OrderID;           // ��̨ί�к�
    TX1FtdcSessionIDType              SessionID;           // �ỰID
    TX1FtdcOrderAnswerStatusType      OrderStatus;         // ί��״̬
    TX1FtdcExchangeIDType             ExchangeID;          // ����������
    TX1FtdcAccountIDType              AccountID;           // �ʽ��˻�ID
    TX1FtdcInstrumentIDType           ComInstrumentID;     // ��Ϻ�Լ����
    TX1FtdcComSpeculatorType          ComSpeculator;       // ���Ͷ����־
    TX1FtdcComStatusType              ComStatus;           // ���״̬
    TX1FtdcAmountType                 ComAmount;           // �������
    TX1FtdcErrorIDType                ErrorID;             // ����ID
    TX1FtdcErrorMsgInfoType           StatusMsg;           // ״̬��Ϣ
};

///��ѯ��ϳֲ�
struct APISTRUCT CX1FtdcQryCombPositionField {
    TX1FtdcRequestIDType              RequestID;           // ����ID
    TX1FtdcExchangeIDType             ExchangeID;          // ����������
    TX1FtdcAccountIDType              AccountID;           // �ʽ��˻�ID
    TX1FtdcInstrumentIDType           ComInstrumentID;     // ��Ϻ�Լ����
    TX1FtdcComSpeculatorType          ComSpeculator;       // ���Ͷ����־
    TX1FtdcInstrumentIDType           InstrumentID;        // ��Լ����
};

///��ѯ��ϳֲ���Ӧ
struct APISTRUCT CX1FtdcRspCombPositionField {
    TX1FtdcRequestIDType              RequestID;           // ����ID
    TX1FtdcAccountIDType              AccountID;           // �ʽ��˻�ID
    TX1FtdcAmountType                 ComAmount;           // �������
    TX1FtdcExchangeIDType             ExchangeID;          // ����������
    TX1FtdcInstrumentIDType           ComInstrumentID;     // ��Ϻ�Լ����
    TX1FtdcComSpeculatorType          ComSpeculator;       // ���Ͷ����־
    TX1FtdcPriceType                  Margin;              // ��ϱ�֤��
};

///��ѯ��Ϻ�Լ
struct APISTRUCT CX1FtdcQryCombInstrumentField {
    TX1FtdcRequestIDType              RequestID;           // ����ID
    TX1FtdcAccountIDType              AccountID;           // �ʽ��˻�ID
    TX1FtdcExchangeIDType             ExchangeID;          // ����������
};

///��ѯ��Ϻ�Լ��Ӧ
struct APISTRUCT CX1FtdcRspCombInstrumentField {
    TX1FtdcRequestIDType              RequestID;           // ����ID
    TX1FtdcAccountIDType              AccountID;           // �ʽ��˻�ID
    TX1FtdcExchangeIDType             ExchangeID;          // ����������
    TX1FtdcInstrumentIDType           ComInstrumentID;     // ��Ϻ�Լ����
    TX1FtdcCombStrategyTypeType       StrategyType;        // ��ϲ�������
    TX1FtdcPriorityType               Priority;            // ������ȼ�
    TX1FtdcComSpeculatorType          ComSpeculator;       // ���Ͷ����־
};

///�����������Զ���Ȩ����
struct APISTRUCT CX1FtdcAutoExecAbandField {
    TX1FtdcRequestIDType              RequestID;           // ����ID
    TX1FtdcAccountIDType              AccountID;           // �ʽ��˻�ID
    TX1FtdcSpeculatorType             Speculator;          // Ͷ������
    TX1FtdcExchangeIDType             ExchangeID;          // ����������
    TX1FtdcInstrumentIDType           InstrumentID;        // ��Լ����
};

///��ѯ�����������Զ���Ȩ����
struct APISTRUCT CX1FtdcQryAutoExecAbandField {
    TX1FtdcRequestIDType              RequestID;           // ����ID
    TX1FtdcAccountIDType              AccountID;           // �ʽ��˻�ID
    TX1FtdcSpeculatorType             Speculator;          // Ͷ������
    TX1FtdcExchangeIDType             ExchangeID;          // ����������
    TX1FtdcInstrumentIDType           InstrumentID;        // ��Լ����
};

///�����������Զ���Ȩ������Ӧ
struct APISTRUCT CX1FtdcRspAutoExecAbandField {
    TX1FtdcRequestIDType              RequestID;           // ����ID
    TX1FtdcAccountIDType              AccountID;           // �ʽ��˻�ID
    TX1FtdcInstrumentIDType           InstrumentID;        // ��Լ����
};

///�����������Զ���Ȩ���ûر�
struct APISTRUCT CX1FtdcRtnAutoExecAbandField {
    TX1FtdcAccountIDType              AccountID;           // �ʽ��˻�ID
    TX1FtdcInstrumentIDType           InstrumentID;        // ��Լ����
    TX1FtdcErrorIDType                ErrorID;             // ����ID
    TX1FtdcErrorMsgInfoType           StatusMsg;           // ״̬��Ϣ
};

struct APISTRUCT CX1FtdcExchangeConnectionStatusRtnField {
};

// �м��ϱ��ն�ϵͳ��Ϣ����
struct APISTRUCT CX1FtdcReqSubmitUserSystemInfoField {
    TX1FtdcRequestIDType             RequestID;            // ����ID [NM]
    TX1FtdcAccountIDType             AccountID;            // �ʽ��˻�ID [M]
    TX1FtdcAppIDType                 AppID;                // AppID���� [M]
    TX1FtdcAppIDType                 RelayAppID;           // RelayAppID���� [M]
    TX1FtdcRemoteIPType              RemoteIP;             // �ն˹���IP [M]����ʽʾ����IPv4:192.168.123.123;IPv6:CDCD:910A:2222:5498:8475:1111:0010:2020
    TX1FtdcRemotePortType            RemotePort;           // �ն˹���port [M]
    TX1FtdcRemoteLoginTimeType       LoginTime;            // ��¼ʱ�� [M]����ʽʾ��"2018-05-01 09:00:00"
    TX1FtdcSystemInfoType            EncryptSystemInfo;    // �ն˲ɼ���Ϣ���� [M]
    TX1FtdcSystemInfoLenType         EncryptSystemInfoLen; // �ն˲ɼ���Ϣ���ĳ��� [M]
};

// �м��ϱ��ն�ϵͳ��Ϣ��Ӧ 
struct APISTRUCT CX1FtdcRspSubmitUserSystemInfoField {
    TX1FtdcRequestIDType             RequestID;            // ����ID
    TX1FtdcAccountIDType             AccountID;            // �ʽ��ʺ�ID
    TX1FtdcAppIDType                 AppID;                // AppID����
    TX1FtdcAppIDType                 RelayAppID;           // RelayAppID����
    TX1FtdcErrorIDType               ErrorID;              // ����ID
};

#endif // TX1FtdcAPISTRUCT_H_
