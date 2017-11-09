#ifndef TAP_API_COMM_DEF_H
#define TAP_API_COMM_DEF_H

#pragma pack(push, 1)

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_CHARS	�����ַ������Ͷ���
 *	@{
 */
//=============================================================================

//! ����Ϊ10���ַ���
typedef char	TAPISTR_10[11];
//! ����Ϊ20���ַ���
typedef char	TAPISTR_20[21];
//! ����Ϊ30���ַ���
typedef char	TAPISTR_30[31];
//! ����Ϊ40���ַ���
typedef char	TAPISTR_40[41];
//! ����Ϊ50���ַ���
typedef char	TAPISTR_50[51];
//! ����Ϊ70���ַ���
typedef char	TAPISTR_70[71];
//! ����Ϊ100���ַ���
typedef char	TAPISTR_100[101];
//! ����Ϊ300���ַ���
typedef char	TAPISTR_300[301];
//! ����Ϊ500���ַ���
typedef char	TAPISTR_500[501];
//! ����Ϊ2000���ַ���
typedef char	TAPISTR_2000[2001];

//! Authorization Code
typedef char	TAPIAUTHCODE[513];


//! ���ַ����壬�����ڶ�����������
typedef char				TAPICHAR;
/** @}*/


//=============================================================================
/**
 *	\addtogroup G_DATATYPE_NUMBER	������ֵ���Ͷ���
 *	@{
 */
//=============================================================================

//! int 32
typedef int					TAPIINT32;
//! unsigned 32
typedef unsigned int		TAPIUINT32;
//! int 64
typedef long long			TAPIINT64;
//! unsigned 64
typedef unsigned long long	TAPIUINT64;
//! unsigned 16
typedef unsigned short		TAPIUINT16;
//! unsigned 8
typedef unsigned char		TAPIUINT8;
//! real 64
typedef double				TAPIREAL64;

/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_YNFLAG	�Ƿ��ʾ
 *	@{
 */
//=============================================================================
//! �Ƿ��ʾ
typedef TAPICHAR			TAPIYNFLAG;
//! ��
const TAPIYNFLAG			APIYNFLAG_YES	='Y';
//! ��
const TAPIYNFLAG			APIYNFLAG_NO	='N';

/** @}*/



//=============================================================================
/**
 *	\addtogroup G_DATATYPE_DATETIME	����ʱ�����Ͷ���
 *	@{
 */
//=============================================================================

//! ʱ�������(��ʽ yyyy-MM-dd hh:nn:ss.xxx)
typedef char	TAPIDTSTAMP[24];
//! ���ں�ʱ������(��ʽ yyyy-MM-dd hh:nn:ss)
typedef char	TAPIDATETIME[20];
//! ��������(��ʽ yyyy-MM-dd)
typedef char	TAPIDATE[11];
//! ʱ������(��ʽ hh:nn:ss)
typedef char	TAPITIME[9];
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_LOG_LEVEL	��־����
 *	@{
 */
//=============================================================================
//! ��־����
typedef TAPICHAR			TAPILOGLEVEL;
//! ��
const TAPILOGLEVEL			APILOGLEVEL_NONE	='N';
//! Error
const TAPILOGLEVEL			APILOGLEVEL_ERROR	='E';
//! Warning
const TAPILOGLEVEL			APILOGLEVEL_WARNING	='W';
//! Debug
const TAPILOGLEVEL			APILOGLEVEL_DEBUG	='D';
/** @}*/


//=============================================================================
/**
 *	\addtogroup G_DATATYPE_COMMODITY_TYPE	Ʒ������
 *	@{
 */
//=============================================================================

//! Ʒ������
typedef TAPICHAR               TAPICommodityType;
//! ��
const TAPICommodityType		TAPI_COMMODITY_TYPE_NONE				= 'N';
//! �ֻ�
const TAPICommodityType		TAPI_COMMODITY_TYPE_SPOT				= 'P';
//! �ڻ�
const TAPICommodityType		TAPI_COMMODITY_TYPE_FUTURES				= 'F';
//! ��Ȩ
const TAPICommodityType		TAPI_COMMODITY_TYPE_OPTION				= 'O';
//! ��������
const TAPICommodityType		TAPI_COMMODITY_TYPE_SPREAD_MONTH		= 'S';
//! ��Ʒ������
const TAPICommodityType		TAPI_COMMODITY_TYPE_SPREAD_COMMODITY	= 'M';
//! ���Ǵ�ֱ����
const TAPICommodityType		TAPI_COMMODITY_TYPE_BUL					= 'U';
//! ������ֱ����
const TAPICommodityType		TAPI_COMMODITY_TYPE_BER					= 'E';
//! ��ʽ����
const TAPICommodityType		TAPI_COMMODITY_TYPE_STD					= 'D';
//! ���ʽ����
const TAPICommodityType		TAPI_COMMODITY_TYPE_STG					= 'G';
//! �������
const TAPICommodityType		TAPI_COMMODITY_TYPE_PRT					= 'R';
//! ��㡪��ֱ�ӻ���
const TAPICommodityType		TAPI_COMMODITY_TYPE_DIRECTFOREX			= 'X';
//! ��㡪����ӻ���
const TAPICommodityType		TAPI_COMMODITY_TYPE_INDIRECTFOREX		= 'I';
//! ��㡪���������
const TAPICommodityType		TAPI_COMMODITY_TYPE_CROSSFOREX			= 'C';
//! ָ��
const TAPICommodityType		TAPI_COMMODITY_TYPE_INDEX				= 'Z';
//! ��Ʊ
const TAPICommodityType		TAPI_COMMODITY_TYPE_STOCK				= 'T';
//! �ֻ�����
const TAPICommodityType		TAPI_COMMODITY_TYPE_SPOT_TRADINGDEFER	= 'Y';

/** @}*/


//=============================================================================
/**
 *	\addtogroup G_DATATYPE_CALL_OR_PUT	���ǿ�����ʾ
 *	@{
 */
//=============================================================================

//! ���ǿ�����ʾ
typedef TAPICHAR			TAPICallOrPutFlagType;
//! ��Ȩ
const TAPICallOrPutFlagType		TAPI_CALLPUT_FLAG_CALL				= 'C';
//! ��Ȩ
const TAPICallOrPutFlagType		TAPI_CALLPUT_FLAG_PUT				= 'P';
//! ��
const TAPICallOrPutFlagType		TAPI_CALLPUT_FLAG_NONE				= 'N';

/** @}*/


//! Application��Ϣ
struct TapAPIApplicationInfo
{
	TAPIAUTHCODE            AuthCode;								///< ��Ȩ��
	TAPISTR_300				KeyOperationLogPath;					///< �ؼ�������־·��
};


//! Ʒ�ֱ���ṹ
struct TapAPICommodity
{
	TAPISTR_10              ExchangeNo;                             ///< ����������
	TAPICommodityType		CommodityType;                          ///< Ʒ������
	TAPISTR_10				CommodityNo;                            ///< Ʒ�ֱ��
};

//! ��Լ����ṹ
struct TapAPIContract
{
	TapAPICommodity			Commodity;								///< Ʒ��
	TAPISTR_10				ContractNo1;                            ///< ��Լ����1
	TAPISTR_10				StrikePrice1;                           ///< ִ�м�1
	TAPICallOrPutFlagType	CallOrPutFlag1;                         ///< ���ǿ�����ʾ1
	TAPISTR_10				ContractNo2;                            ///< ��Լ����2
	TAPISTR_10				StrikePrice2;                           ///< ִ�м�2
	TAPICallOrPutFlagType	CallOrPutFlag2;                         ///< ���ǿ�����ʾ2
};

//! ��������Ϣ
struct TapAPIExchangeInfo
{
	TAPISTR_10				ExchangeNo;								///< ����������
	TAPISTR_20				ExchangeName;							///< ����������
};

//! �޸���������
struct TapAPIChangePasswordReq
{
	TAPISTR_20				OldPassword;							///< ������
	TAPISTR_20				NewPassword;							///< ������
};

#pragma pack(pop)

#endif