#ifndef TAP_QUOTE_API_DATA_TYPE_H
#define TAP_QUOTE_API_DATA_TYPE_H
//���ļ�������TapQuoteAPI ʹ�õ��������ͺ����ݽṹ

#include "TapAPICommDef.h"

#pragma pack(push, 1)

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_Q_BASIC	�����������
 *	@{
 */
//=============================================================================

//! ����۸�
typedef TAPIREAL64		TAPIQPRICE;
//! ������
typedef TAPIUINT64		TAPIQVOLUME;
//! �仯ֵ
typedef TAPIINT64		TAPIQDIFF;	

/** @}*/


//=============================================================================
/**
 *	\addtogroup G_STRUCT_Q	����ṹ�嶨��
 *	@{
 */
//=============================================================================

//! ��¼��֤��Ϣ
struct TapAPIQuoteLoginAuth
{
	TAPISTR_20			UserNo;					///< �û���
	TAPIYNFLAG			ISModifyPassword;		///< �Ƿ��޸����룬'Y'��ʾ�ǣ�'N'��ʾ��
	TAPISTR_20			Password;				///< �û�����
	TAPISTR_20			NewPassword;			///< �����룬����������޸���������Ҫ��д���ֶ�
	TAPISTR_20			QuoteTempPassword;		///< ������ʱ����
	TAPIYNFLAG			ISDDA;					///< �Ƿ���ѽ��̬��֤��'Y'��ʾ�ǣ�'N'��ʾ��
	TAPISTR_30			DDASerialNo;			///< ��̬��֤��
};

//! ��¼������Ϣ
struct TapAPIQuotLoginRspInfo
{
	TAPISTR_20					UserNo;							///< �û���
	TAPIINT32					UserType;						///< �û�����
	TAPISTR_20					UserName;						///< �ǳƣ�GBK�����ʽ
	TAPISTR_20					QuoteTempPassword;				///< ������ʱ����
	TAPISTR_50					ReservedInfo;					///< �û��Լ����õ�Ԥ����Ϣ
	TAPISTR_40					LastLoginIP;					///< �ϴε�¼�ĵ�ַ
	TAPIUINT32					LastLoginProt;					///< �ϴε�¼ʹ�õĶ˿�
	TAPIDATETIME				LastLoginTime;					///< �ϴε�¼��ʱ��
	TAPIDATETIME				LastLogoutTime;					///< �ϴ��˳���ʱ��
	TAPIDATE					TradeDate;						///< ��ǰ��������
	TAPIDATETIME				LastSettleTime;					///< �ϴν���ʱ��
	TAPIDATETIME				StartTime;						///< ϵͳ����ʱ��
	TAPIDATETIME				InitTime;						///< ϵͳ��ʼ��ʱ��
};


//! Ʒ����Ϣ
struct TapAPIQuoteCommodityInfo
{
	TapAPICommodity			Commodity;							///< Ʒ��
	TAPISTR_20				CommodityName;						///< Ʒ������,GBK�����ʽ
	TAPISTR_30				CommodityEngName;					///< Ʒ��Ӣ������
	TAPIREAL64				ContractSize;						///< ÿ�ֳ���
	TAPIREAL64				CommodityTickSize;					///< ��С�䶯��λ
	TAPIINT32				CommodityDenominator;				///< ���۷�ĸ
	TAPICHAR                CmbDirect;							///< ��Ϸ���
	TAPIINT32				CommodityContractLen;				///< Ʒ�ֺ�Լ����
	TAPIYNFLAG				IsDST;								///< �Ƿ�����ʱ,'Y'Ϊ��,'N'Ϊ��
	TapAPICommodity			RelateCommodity1;					///< ����Ʒ��1
	TapAPICommodity			RelateCommodity2;					///< ����Ʒ��2
};


//! �����Լ��Ϣ
struct TapAPIQuoteContractInfo
{
	TapAPIContract				Contract;							///< ��Լ
	TAPICHAR					ContractType;                       ///< ��Լ����,'1'��ʾ���������Լ,'2'��ʾ�����Լ
	TAPISTR_10					QuoteUnderlyingContract;			///< ������ʵ��Լ
	TAPISTR_70					ContractName;                       ///< ��Լ����
	TAPIDATE					ContractExpDate;                    ///< ��Լ������	
	TAPIDATE					LastTradeDate;                      ///< �������
	TAPIDATE					FirstNoticeDate;                    ///< �״�֪ͨ��
};

//! ����ȫ��
struct TapAPIQuoteWhole
{
	TapAPIContract				Contract;						///< ��Լ
	TAPISTR_10					CurrencyNo;						///< ���ֱ��
	TAPICHAR					TradingState;					///< ����״̬��1,���Ͼ���;2,���Ͼ��۴��;3,��������;4,������ͣ;5,����
	TAPIDTSTAMP					DateTimeStamp;					///< ʱ���
	TAPIQPRICE					QPreClosingPrice;				///< �����̼�
	TAPIQPRICE					QPreSettlePrice;				///< ������
	TAPIQVOLUME					QPrePositionQty;				///< ��ֲ���
	TAPIQPRICE					QOpeningPrice;					///< ���̼�
	TAPIQPRICE					QLastPrice;						///< ���¼�
	TAPIQPRICE					QHighPrice;						///< ��߼�
	TAPIQPRICE					QLowPrice;						///< ��ͼ�
	TAPIQPRICE					QHisHighPrice;					///< ��ʷ��߼�
	TAPIQPRICE					QHisLowPrice;					///< ��ʷ��ͼ�
	TAPIQPRICE					QLimitUpPrice;					///< ��ͣ��
	TAPIQPRICE					QLimitDownPrice;				///< ��ͣ��
	TAPIQVOLUME					QTotalQty;						///< �����ܳɽ���
	TAPIQPRICE					QTotalTurnover;					///< ���ճɽ����
	TAPIQVOLUME					QPositionQty;					///< �ֲ���
	TAPIQPRICE					QAveragePrice;					///< ����
	TAPIQPRICE					QClosingPrice;					///< ���̼�
	TAPIQPRICE					QSettlePrice;					///< �����
	TAPIQVOLUME					QLastQty;						///< ���³ɽ���
	TAPIQPRICE					QBidPrice[20];					///< ���1-20��
	TAPIQVOLUME					QBidQty[20];					///< ����1-20��
	TAPIQPRICE					QAskPrice[20];					///< ����1-20��
	TAPIQVOLUME					QAskQty[20];					///< ����1-20��
	TAPIQPRICE					QImpliedBidPrice;				///< �������
	TAPIQVOLUME					QImpliedBidQty;					///< ��������
	TAPIQPRICE					QImpliedAskPrice;				///< ��������
	TAPIQVOLUME					QImpliedAskQty;					///< ��������
	TAPIQPRICE					QPreDelta;						///< ����ʵ��
	TAPIQPRICE					QCurrDelta;						///< ����ʵ��
	TAPIQVOLUME					QInsideQty;						///< ������
	TAPIQVOLUME					QOutsideQty;					///< ������
	TAPIQPRICE					QTurnoverRate;					///< ������
	TAPIQVOLUME					Q5DAvgQty;						///< ���վ���
	TAPIQPRICE					QPERatio;						///< ��ӯ��
	TAPIQPRICE					QTotalValue;					///< ����ֵ
	TAPIQPRICE					QNegotiableValue;				///< ��ͨ��ֵ
	TAPIQDIFF					QPositionTrend;					///< �ֲ�����
	TAPIQPRICE					QChangeSpeed;					///< ����
	TAPIQPRICE					QChangeRate;					///< �Ƿ�
	TAPIQPRICE					QChangeValue;					///< �ǵ�ֵ
	TAPIQPRICE					QSwing;							///< ���
	TAPIQVOLUME					QTotalBidQty;					///< ί������
	TAPIQVOLUME					QTotalAskQty;					///< ί������
	TapAPIContract				UnderlyContract;				///< �����Լ��Ӧ����ʵ��Լ
};


/** @}*/





#pragma pack(pop)

#endif