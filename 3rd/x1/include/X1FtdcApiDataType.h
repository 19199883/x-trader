/**
 * Copyright (C) 2015-2016, �����ɴ���Ϣ�������޹�˾
 * @file <X1FtdcApiDataType.h>
 * @brief <����ӿ��������������>
 * @version <1.3.2.3>
 * @author <X1��Ŀ��>
 * @date <2015��12��2��>
 */

#ifndef X1FTDCAPIDATATYPE_H
#define X1FTDCAPIDATATYPE_H

//#define C_ExchangeID_DCE                  1     // ������
//#define C_ExchangeID_SHFE                 2     // ������
//#define COMPLIE_EXCHANGE_ID_TYPE C_ExchangeID_SHFE


typedef char TX1FtdcUserIDType[41];               /**< �û�ID��������*/
typedef char TX1FtdcAccountIDType[13];            /**< �ʽ��˻���������*/
typedef int TX1FtdcExecStateType;                 /**< ִ��״̬��������*/

#define X1_FTDC_ERR_CODE_SUCCESS            0     ///< �ɹ�
#define X1_FTDC_ERR_CODE_FAIL              -1     ///< ʧ��
#define X1_FTDC_ERR_CODE_INVALID           -2

typedef char TX1FtdcClientIDType[13];             /**< ���ױ����������� */
typedef int TX1FtdcClientStatusType;              /**< ���ױ���״̬��������*/

#define X1_FTDC_PROHIBIT_OPEN               4     ///< ��ֹ���²�
#define X1_FTDC_ALLOW_OPEN                  5     ///< �����²�

typedef char TX1FtdcInstrumentIDType[64];         /**< ��Լ������������ */
typedef char TX1FtdcInstrumentPrefixType[31];     /**< Ʒ�������������� */
typedef char TX1FtdcVarietyNameType[31];          /**< Ʒ�������������� */
typedef char TX1FtdcInstrumentNameType[64];       /**< ��Լ������������ */
typedef char TX1FtdcActiveContractType[1024];     /**< ��Ч��Լ�������� */
typedef long TX1FtdcLocalOrderIDType;             /**< ����ί�к��������� */


typedef double TX1FtdcPriceType;                  /**< �۸��������� */
typedef long TX1FtdcAmountType;                   /**< ί�������������� */
typedef short TX1FtdcBuySellTypeType;             /**< ������������ */

#define X1_FTDC_SPD_BUY                     1     ///< ��
#define X1_FTDC_SPD_SELL                    2     ///< ��

typedef int TX1FtdcOpenCloseTypeType;             /**< ��ƽ��־�������� */

#define X1_FTDC_SPD_OPEN                    1     ///< ����
#define X1_FTDC_SPD_CLOSE                   2     ///< ƽ��
#define X1_FTDC_SPD_CLOSETODAY              4     ///< ƽ��





typedef short TX1FtdcSpeculationValueType;        /**< Ͷ����ֵ�������� */
typedef char TX1FtdcExchangeIDType[8];            /**< ������������������ */

#define X1_FTDC_EXCHANGE_DCE              "DCE"   ///< ������
#define X1_FTDC_EXCHANGE_CZCE             "CZCE"  ///< ֣����
#define X1_FTDC_EXCHANGE_SHFE             "SHFE"  ///< ������
#define X1_FTDC_EXCHANGE_CFFEX            "CFFEX" ///< �н���
#define X1_FTDC_EXCHANGE_INE              "INE"   ///< ������


typedef char TX1FtdcFrontAddrType[32];            /**< ǰ�û���ַ�������� */
typedef short TX1FtdcCompanyIDType;               /**< �����̴����������� */


typedef char TX1FtdcPasswdType[41];               /**< �û������������� ��̨�����벻��Ϊ������Ч�������Ϊ16λ */
typedef char TX1FtdcPasswordType[41];             /**< �û������������� ��̨�����벻��Ϊ������Ч�������Ϊ16λ */
#define X1FTDC_PASSWORD_LEN 41


typedef long TX1FtdcX1OrderIDType;                /**< ��̨ί�к��������� */
typedef char TX1FtdcOrderSysIDType[64];           /**< ��������������� */
typedef int TX1FtdcOrderTypeType;                 /**< �������� */

// ������ͨ������������
#define X1_FTDC_LIMITORDER                  1     ///< �޼�ί��
#define X1_FTDC_MKORDER                     2     ///< �м�ί��
#define X1_FTDC_ARBITRAGE                   4     ///< ����ί��
#define X1_FTDC_EXTENSION                   8     ///< չ�ڻ���ί��
#define X1_FTDC_PROFIT_LIMITORDER           32    ///< �޼�ֹӯί��
#define X1_FTDC_PROFIT_MKORDER              34    ///< �м�ֹӯί��
#define X1_FTDC_LOSS_LIMITORDER             48    ///< �޼�ֹ��ί��
#define X1_FTDC_LOSS_MKORDER                50    ///< �м�ֹ��ί��


typedef short TX1FtdcOrderAnswerStatusType;       /**< ί�лر����� */

#define X1_FTDC_SPD_CANCELED                1     ///< ȫ������
#define X1_FTDC_SPD_FILLED                  2     ///< ȫ���ɽ�
#define X1_FTDC_SPD_IN_QUEUE                3     ///< δ�ɽ����ڶ�����
#define X1_FTDC_SPD_PARTIAL                 4     ///< ���ֳɽ����ڶ�����
#define X1_FTDC_SPD_PARTIAL_CANCELED        5     ///< ���ɲ���
#define X1_FTDC_SPD_IN_CANCELING            6     ///< ������
#define X1_FTDC_SPD_ERROR                   7     ///< ����(�ϵ�����)
#define X1_FTDC_SPD_PLACED                  8     ///< δ�ɽ����ڶ�����
#define X1_FTDC_SPD_TRIGGERED               10    ///< ��̨�ѽ��գ�����δ��������
#define X1_FTDC_SPD_IN_CANCELED             11    ///< ��̨�ѽ��գ���̨�ڲ�ֱ�ӳ���

/// �����㷨��ģ������ 
#define X1_FTDC_EXT_UNTRIGGER               13    ///< δ����
#define X1_FTDC_EXT_PART_TRIGGER            14    ///< ���ִ���
#define X1_FTDC_EXT_ALL_TRIGGER             15    ///< ȫ������
#define X1_FTDC_EXT_CANCELLED               16    ///< �Ѿ�����
#define	X1_FTDC_EXT_FAILED                  17    ///< ����ʧ��



typedef char TX1FtdcMatchIDType[32];              /**< �ɽ������������ */
typedef char TX1FtdcDateType[13];                 /**< ʱ���������� */
typedef long TX1FtdcMatchType;                    /**< �ɽ������������� */

#define X1_FTDC_BASIC_TRADE                 0     ///< ��ͨ�ɽ�

typedef int TX1FtdcSpeculatorType;                /**< Ͷ������ */

#define X1_FTDC_SPD_SPECULATOR              0     ///Ͷ��
#define X1_FTDC_SPD_HEDGE                   1     ///�ױ�
#define X1_FTDC_SPD_ARBITRAGE               2     ///����


typedef double TX1FtdcFeeType;                    /**< �������������� */
typedef int TX1FtdcErrorIDType;                   /**< ������������ */
typedef char TX1FtdcErrorMsgInfoType[256];         /**< ������Ϣ�������� */
typedef char TX1FtdcMsgInfoType[1024];            /**< ��Ϣ��Ϣ�������� */
typedef double TX1FtdcEquityType;                 /**< Ȩ���������� */
typedef double TX1FtdcProfitLossType;             /**< ӯ���������� */
typedef int TX1FtdcAccountLoginResultType;        /**< �ʽ��˻���¼��� */

#define X1_FTDC_LOGIN_SUCCESS               0     ///< ��¼�ɹ�
#define X1_FTDC_LOGIN_FAILED                1     ///< ��¼ʧ��
#define X1_FTDC_LOGIN_QUIT                  2     ///< ���˳�
#define X1_FTDC_LOGIN_NOT_OPERATE           9     ///< δ����

typedef long TX1FtdcSessionIDType;                /**< SessionID�������� */
typedef int TX1FtdcAccountLogoutResultType;       /**< �ʽ��ʺŵǳ���� */

#define X1_FTDC_LOGOUT_SUCCESS              0     ///< �ǳ��ɹ�
#define X1_FTDC_LOGOUT_FAILED               1     ///< �ǳ�ʧ��


typedef int TX1FtdcUserTypeType;                  /**< �û����� */
typedef int TX1FtdcCounterIDType;                 /**< ��̨����������� */
typedef double TX1FtdcRiskDegreeType;             /**< ���ն��������� */
typedef int TX1FtdcMilliSecType;                  /**< ΢���������� */
typedef double TX1FtdcDeltaType;                  /**< ��ʵ���������� */
typedef int TX1FtdcVolumeType;                    /**< ������������ */
typedef int TX1FtdcFrontIDType;                   /**< ǰ�û������������ */
typedef int TX1FtdcOfferPriceLimitType;           /**< �������������������� */
typedef short TX1FtdcOrderNumType;                /**< ί�к��������� */
typedef double TX1FtdcRatioType;                  /**< ������������ */
typedef double TX1FtdcPremiumType;                /**< Ȩ���� */
typedef double TX1FtdcMarketValueType;            /**< ��Ȩ��ֵ */
typedef char TX1FtdcTimeType[256];                /**< ������ʱ�� */
typedef char TX1FtdcAbiPolicyCodeType[8];         /**< �������Դ����������� */

#define X1_FTDC_SP                        "SP"    ///< ��������
#define X1_FTDC_SP_SPC                    "SPC"   ///< ���ȿ�Ʒ������
#define X1_FTDC_SP_SPX                    "SPX"   ///< ѹե����
#define X1_FTDC_SP_CALL                   "CSPR"  ///< Call Spread
#define X1_FTDC_SP_PUT                    "PSPR"  ///< Put Spread
#define X1_FTDC_SP_COMBO                  "COMBO" ///< Combo
#define X1_FTDC_SP_STRADDLE               "STD"   ///< Straddle
#define X1_FTDC_SP_STRANGLE               "STG"   ///< Strangle
#define X1_FTDC_SP_GUTS                   "GUTS"  ///< Guts
#define X1_FTDC_SP_SYNUND                 "SYN"   ///< Synthetic Underlying

typedef char TX1FtdcOrderPropertyType;            /**< TX1FtdcOrderPropertyType:�������� */

#define X1_FTDC_SP_NON                     '0'    ///�޶�������
#define X1_FTDC_SP_FAK                     '1'    ///FAK����
#define X1_FTDC_SP_FOK                     '2'    ///FOK����
#define X1_FTDC_SP_ANYPRICE                '3'    ///�м������
#define X1_FTDC_SP_ANYPRICE_TO_MKORDER     '4'    ///�м������ת�޼�
#define X1_FTDC_SP_FIVELEVELPRICE          '5'    ///�嵵�м�
#define X1_FTDC_SP_FIVELEVELPRICE_TO_LIMIT '6'    ///�嵵�м�ת�޼�
#define X1_FTDC_SP_BESTPRICE               '7'    ///���ż�
#define X1_FTDC_SP_BESTPRICE_TO_LIMIT      '8'    ///���ż�ת�޼�


typedef int TX1FtdcInsertType;                    /**< ί����� */
#define X1_FTDC_BASIC_ORDER               0x0001  ///��ͨί�е�
///�Զ���
#define X1_FTDC_AUTO_ORDER                0x0002  ///�Զ���

typedef int TX1FtdcOptionTypeType;                /**< ��Ȩ����������� */

#define X1_FTDC_OPT_CALL                   1      ///����
#define X1_FTDC_OPT_PUT                    2      ///����


typedef int TX1FtdcInstrumentTypeType;            /**< TX1FtdcInstrumentTypeType:��Լ������������ */

#define X1FTDC_INSTRUMENT_TYPE_COMM        0      ///�ڻ�
#define X1FTDC_INSTRUMENT_TYPE_OPT         1      ///��Ȩ


typedef char TX1FtdcCancelTypeType;               /**< TX1FtdcCancelTypeType:������־�������� */

#define X1_FTDC_ORDER_BOOK                'O'     ///����
#define X1_FTDC_ORDER_CANCEL              'W'     ///����


typedef char TX1FtdcContentType[501];             /**< TX1FtdcContentType:��Ϣ������������ */
typedef int TX1FtdcInstrumentStatusType;          /**< TX1FtdcInstrumentStatusType:��Լ����״̬�������� */
typedef short TX1FtdcInstStatusEnterReasonType;   /**< TX1FtdcInstStatusEnterReasonType:���뱾״̬ԭ���������� */
typedef char TX1FtdcCurrencyType[4];              /**< TX1FtdcCurrencyType:������������ */

typedef double TX1FtdcCurrencyUnitType;
typedef double TX1FtdcExchangeRateType;

typedef int TX1FtdcConfirmMarkType;               /**< TX1FtdcConfirmType:ȷ�ϱ�־�������� */

#define X1_FTDC_CON_CONFIRM                2      ///< ȷ��


typedef char TX1FtdcStanAddrType[16];             /**< TX1FtdcStanAddrType:���õ�ַ�������� */
typedef long TX1FtdcCapControlModeType;           /**< TX1FtdcCapControlModeType:�ʽ���Ʒ�ʽ�������� */

#define X1_FTDC_PPL_USABLE                 2      ///< ����ӯ������ 
#define  X1_FTDC_OF_USABLEFUND             4      ///< ƽ���ʽ�T+1���� 
#define  X1_FTDC_OF_FETCHABLEMARGIN        8      ///< ƽ�ֱ�֤���ȡ 
#define  X1_FTDC_AG_FETCHBLETODAY          16     ///< ����ӯ����ȡ 
#define  X1_FTDC_INTEREST_                 32     ///< ȡ��Ȩ����ڱ�������� 
#define  X1_FTDC_OF_FETCHABLE              128    ///< ƽ��ӯ����ȡ 
#define  X1_FTDC_PREMIUMOPTION_FETCHBLE    256    ///< Ȩ���������ȡ 


typedef double TX1FtdcArchRatioType;              /**< TX1FtdcArchRatioType:ת������������� */
typedef int TX1FtdcSettlementBillTradeType;       /**< TX1FtdcSettlementBillTradeType:���ܱ�־�������� */

#define X1_FTDC_MATCHDETAIL                2      ///< ���ܳɽ���ϸ
#define X1_FTDC_OPGAL                      4      ///< ���ֲܳ�ӯ��
#define X1_FTDC_OFGAL                      8      ///< ����ƽ��ӯ��


typedef int TX1FtdcFilesFlagType;                 /**< TX1FtdcFilesFlagType:���������������� */

#define X1_FTDC_PRINT_MATCHDETAIL          4      ///�ɽ���ϸ��ӡ
#define X1_FTDC_PRINT_OPGAL                8      ///�ֲ�ӯ����ӡ 
#define X1_FTDC_PRINT_OFGAL                16     ///ƽ��ӯ����ӡ 
#define X1_FTDC_PRINT_ACCESSFUNDS          32     ///�ʽ�����ӡ
#define X1_FTDC_PRINT_ADDMARGIN            64     ///׷��������ӡ


typedef char TX1FtdcSoftwareVendorIDType[33];     /**< TX1FtdcSoftwareVendorIDType:�����Ӧ�̱���������� */
typedef long TX1FtdcProductOnlineCountType;       /**< TX1FtdcProductOnlineCountType:��Ʒ���������������� */
typedef char TX1FtdcBrokerInfoType[20];           /**< TX1FtdcBrokerInfoType:�ڻ���˾������������ */
typedef char TX1FtdcProductIDType[33];            /**< TX1FtdcProductIDType:��Ʒ����������� */
typedef long TX1FtdcRequestIDType;                /**< TX1FtdcRequestIDType:����ID�������� */
typedef char TX1FtdcCustomCategoryType[32];       /**< TX1FtdcCustomCategoryType:�Զ�������������� */
typedef int  TX1FtdcReservedType;                 /**< TX1FtdcReservedType:Ԥ���ֶ��������� */
typedef short TX1FtdcNoticeType;                  /**< TX1FtdcNoticeType:��Ϣ�������� */

#define X1_FTDC_SYS_BROADCAST_MSG          1      ///< ϵͳ�㲥
#define X1_FTDC_ACCOUNT_ID_MSG             2      ///< ָ���ͻ�

////////////////////////////////////////////////////////////
///TX1FtdcTradingSegmentSNType:���׽׶α����������
////////////////////////////////////////////////////////////
typedef int TX1FtdcTradingSegmentSNType;


//////////////////////////////////////////////
///TX1FtdcExtOrderType:�㷨��������������
//////////////////////////////////////////////
typedef int TX1FtdcExtOrderType;
///���鴥����
#define X1_FTDC_PRICESTRIGGER              1
///�ɽ�������
#define X1_FTDC_MATCHTRIGGER               2
///ֹ��ֹӯ��
#define X1_FTDC_PROFITLOSS                 3
///��������
#define X1_FTDC_ARBITRAGE                  4
///���鴥���޸�
#define X1_FTDC_PRICESTRIGGER_MODIFY       6
///�ɽ������޸�
#define X1_FTDC_MATCHTRIGGER_MODIFY        7
///ֹ��ֹӯ�����޸�
#define X1_FTDC_PROFITLOSS_MODIFY          8
///��ϴ����޸�
#define X1_FTDC_ARBITRAGE_MODIFY           9
///Ԥ��
#define X1_FTDC_YMORDER                    1
///������
#define X1_FTDC_TJORDER                    2
///������������
#define X1_FTDC_KQTLDD                     3
///��Ʒ����������
#define X1_FTDC_KPZTLDD                    4
///��ʽ��������
#define X1_FTDC_DSTLDD                     5
///�Զ�����������(�ݲ�֧��)
#define X1_FTDC_ZDYTLDD                    6

//////////////////////////////////////////////
///TX1FtdcTriggerTime:����ʱ����������
//////////////////////////////////////////////
typedef char TX1FtdcTriggerTime[10];


//////////////////////////////////////////////
///TX1FtdcPriceReference:�۸������������
//////////////////////////////////////////////
typedef int TX1FtdcPriceReference;

///�������¼�
#define X1_FTDC_REF_LASTPRICE             0
///������һ��
#define X1_FTDC_REF_BIDPRICE              1
///����������
#define X1_FTDC_REF_ASKPRICE              2

//////////////////////////////////////////////
///TX1FtdcCompareFlag:�Ƚϱ�־��������
//////////////////////////////////////////////
typedef int TX1FtdcCompareFlag;

///����
#define X1_FTDC_CF_GREATER                0
///���ڵ���
#define X1_FTDC_CF_NOTLESS                1
///С��
#define X1_FTDC_CF_LESS                   2
///С�ڵ���
#define X1_FTDC_CF_NOTGREATER             3

//////////////////////////////////////////////
///TX1FtdcOvernightFlag:��ҹ��־��������
//////////////////////////////////////////////
typedef int TX1FtdcOvernightFlag;

///��ҹ
#define X1_FTDC_OVERNIGHT                 1
///����ҹ
#define X1_FTDC_NOT_OVERNIGHT             2

//////////////////////////////////////////////
///TX1FtdcArbitragePrice:�����۸���������
//////////////////////////////////////////////
typedef double TX1FtdcArbitragePrice;


//////////////////////////////////////////////
///TX1FtdcExtTriggerCond:����������������
//////////////////////////////////////////////
typedef int TX1FtdcExtTriggerCond;

///�۸񴥷�
#define X1_FTDC_TRIGGER_PRICE             0
///ʱ�䴥��
#define X1_FTDC_TRIGGER_TIME              1


////////////////////////////////////////////////////////////
///TX1FtdcInstrumentMaturityType:��Լ�������
////////////////////////////////////////////////////////////
typedef char TX1FtdcInstrumentMaturityType[12];


////////////////////////////////////////////////////////////
///TX1FtdcInstrumenExpirationDateType:��Լ������
////////////////////////////////////////////////////////////
typedef char TX1FtdcInstrumenExpirationDateType[12];


////////////////////////////////////////////////////////////
///TX1FtdcAdjustmentInfoType:��ϻ�����ı�֤�������Ϣ
///��ʽ:[��Լ����,������־,Ͷ�����,�������;]
////////////////////////////////////////////////////////////
typedef char TX1FtdcAdjustmentInfoType[64];


////////////////////////////////////////////////////////////
///TX1FtdcQuoteIDType:ѯ�۱��
////////////////////////////////////////////////////////////
typedef char TX1FtdcQuoteIDType[33];


////////////////////////////////////////////////////////////
///TX1FtdcSourceType:��Դ
////////////////////////////////////////////////////////////
typedef short TX1FtdcSourceType;

///��Ա
#define X1_FTDC_SOURCE_MEMBER             0
///������
#define X1_FTDC_SOURCE_EXCHANGE           1


////////////////////////////////////////////////////////////
///TX1FtdcSeatCodeType:ϯλ����
////////////////////////////////////////////////////////////
typedef char TX1FtdcSeatCodeType[33];


////////////////////////////////////////////////////////////
///TX1FtdcCloseIDType:ƽ��ִ�е���
////////////////////////////////////////////////////////////
typedef char TX1FtdcCloseIDType[33];


////////////////////////////////////////////////////////////
///TX1FtdcEntrusTellerType:ί�й�Ա
////////////////////////////////////////////////////////////
typedef char TX1FtdcEntrusTellerType[41];


////////////////////////////////////////////////////////////
///TX1FtdcStayTimeType��ͣ��ʱ����������
////////////////////////////////////////////////////////////
typedef int TX1FtdcStayTimeType;


////////////////////////////////////////////////////////////
///TX1FtdcComputeModeType�����㷽ʽ��������
////////////////////////////////////////////////////////////
typedef int TX1FtdcComputeModeType;
///������ֵ����
#define X1_FTDC_ABSOLUTE_VALUE_COMPUTE        0
///��������֤���׼�����ϸ���
#define X1_FTDC_EXCHANGE_MARGIN_BASIS_FLOAT   1 
///��������֤���������ϸ���
#define X1_FTDC_EXCHANGE_MARGIN_RESULT_FLOAT  2
///�ڻ���֤���׼�����ϸ���
#define X1_FTDC_FUTURES_MARGIN_BASIS_FLOAT    3


/////////////////////////////////////////////////////////////////////////
///TX1FtdcPriceNoteType:��Ȩ��֤����㷽ʽ
/////////////////////////////////////////////////////////////////////////
typedef int TX1FtdcPriceNoteType;
///���������ۼ���
#define X1_FTDC_CALC_BY_PRESETTLEMENT        1
///�������¼ۼ���
#define X1_FTDC_CALC_BY_LASTPRICE            2

/////////////////////////////////////////////////////////////////////////
///TX1FtdcLargeMarginDirectType:��߱�֤������������
/////////////////////////////////////////////////////////////////////////
typedef char TX1FtdcLargeMarginDirectType[128];

/////////////////////////////////////////////////////////////////////////
///TX1FtdcBankIDType:���д�������
/////////////////////////////////////////////////////////////////////////
typedef char TX1FtdcBankIDType[8];

/////////////////////////////////////////////////////////////////////////
///TX1FtdcBankNameType:������������
/////////////////////////////////////////////////////////////////////////
typedef char TX1FtdcBankNameType[101];

/////////////////////////////////////////////////////////////////////////
///TX1FtdcBankSerialType:������ˮ������
/////////////////////////////////////////////////////////////////////////
typedef char TX1FtdcBankSerialType[13];

/////////////////////////////////////////////////////////////////////////
///TX1FtdcSerialType:��ˮ������
/////////////////////////////////////////////////////////////////////////
typedef int TX1FtdcSerialType;

/////////////////////////////////////////////////////////////////////////
///TX1FtdcBankAccountType:�����˻�����
/////////////////////////////////////////////////////////////////////////
typedef char TX1FtdcBankAccountType[41];

/////////////////////////////////////////////////////////////////////////
///TX1FtdcFutureSerialType:�ڻ���˾��ˮ������
/////////////////////////////////////////////////////////////////////////
typedef int TX1FtdcFutureSerialType;

/////////////////////////////////////////////////////////////////////////
///TX1FtdcDigestType:ժҪ����
/////////////////////////////////////////////////////////////////////////
typedef char TX1FtdcDigestType[36];

/////////////////////////////////////////////////////////////////////////
///TX1FtdcBankAccTypeType��һ�������ʺ���������
/////////////////////////////////////////////////////////////////////////
///���д���
#define X1_FTDC_BAT_BankBook               '1'
///���
#define X1_FTDC_BAT_SavingCard             '2'
///���ÿ�
#define X1_FTDC_BAT_CreditCard             '3'
typedef char TX1FtdcBankAccTypeType;

/////////////////////////////////////////////////////////////////////////
///TX1FtdcTransferStatusType:ת�˽���״̬����
/////////////////////////////////////////////////////////////////////////
///����
#define X1_FTDC_TRFS_Normal                '0'
///������
#define X1_FTDC_TRFS_Repealed              '1'
typedef char TX1FtdcTransferStatusType;

/////////////////////////////////////////////////////////////////////////
///TX1FtdcTransferType:����ת��ҵ������
/////////////////////////////////////////////////////////////////////////
typedef int TX1FtdcTransferType;

/////////////////////////////////////////////////////////////////////////
///TX1FtdcTransferType:����ת�˴���������
/////////////////////////////////////////////////////////////////////////
typedef int TX1FtdcProcResultType;
///�ɹ�
#define X1_FTDC_PROC_SUCCESS               0
///ʧ��
#define X1_FTDC_PROC_FAIL                  1
///�ȴ���ִ
#define X1_FTDC_PROC_WAIT_RTN              2


/////////////////////////////////////////////////////////////////////////
///TX1FtdcApplyNumberType:����ת�����������
/////////////////////////////////////////////////////////////////////////
typedef int TX1FtdcApplyNumberType;


//********************��չ��������**********************

/////////////////////////////////////////////////////////////////////////
///TX1FtdcImpliedVolatilityType:��������������
/////////////////////////////////////////////////////////////////////////
typedef double TX1FtdcImpliedVolatilityType;

/////////////////////////////////////////////////////////////////////////
///TX1FtdcOptionComputationType:��Ȩ������������
/////////////////////////////////////////////////////////////////////////
typedef double TX1FtdcOptionComputationType;

////////////////////////////////////////////////////////////
///TX1FtdcFunctionIDType:������չ���ܺ�
////////////////////////////////////////////////////////////
typedef char TX1FtdcFunctionIDType[16];

////////////////////////////////////////////////////////////
///TX1FtdcExtMarketDataType:������չ���ܺ�
////////////////////////////////////////////////////////////
typedef char TX1FtdcExtMarketDataType[128];

//********************��չ��������**********************


/////////////////////////////////////////////////////////////////////////
///TX1FtdcExchangeStatusType:������״̬��������
/////////////////////////////////////////////////////////////////////////
typedef int TX1FtdcExchangeStatusType;
///����ǰ
#define X1_FTDC_IS_BEFORETRADING           0
///�ǽ���
#define X1_FTDC_IS_NOTRADING               1
///��������
#define X1_FTDC_IS_CONTINOUS               2
///���Ͼ��۱���
#define X1_FTDC_IS_AUCTIONORDERING         3
///���Ͼ��ۼ۸�ƽ��
#define X1_FTDC_IS_AUCTIONBALANCE          4
///���Ͼ��۴��
#define X1_FTDC_IS_AUCTIONMATCH            5
///����
#define X1_FTDC_IS_CLOSED                  6


/////////////////////////////////////////////////////////////////////////
///TX1FtdcPositionDateType:�ֲ���������
/////////////////////////////////////////////////////////////////////////
typedef int TX1FtdcPositionDateType;
//���
#define X1_FTDC_PSD_TODAY                  1
//��ʷ��
#define X1_FTDC_PSD_HISTORY                2

/////////////////////////////////////////////////////////////////////////
///TX1FtdcX1PriFlowIDType:˽�����������
/////////////////////////////////////////////////////////////////////////
typedef int TX1FtdcX1PriFlowIDType;

/// ˽���������־
#define X1_PrivateFlow_Req_Quick          0    // ֻ���͵�¼��˽����������
#define X1_PrivateFlow_Req_Restart        1    // �ӱ������տ�ʼ�ش�
#define X1_PrivateFlow_Req_Resume         2    // ���ϴ��յ�������
#define X1_PrivateFlow_Req_Specify        3    // ��ָ����ſ�ʼ����
////////////////////////////////////////////////////////////
///DFITCCalculateMode:�����ʽ���㷽ʽ
////////////////////////////////////////////////////////////
typedef int TX1FtdcCalculateMode;

#define X1_CAlCULATEMODE_UPPPERLIMITPRICE 1    //��ͣ��۸�
#define X1_CAlCULATEMODE_LOWERLIMITPRICE  2    //��ͣ��۸�
#define X1_CAlCULATEMODE_SPECIFIEDPRICE   3    //ָ���۸�

#endif//X1FTDCAPIDATATYPE_H
