//���ļ�������TapTradeAPI ʹ�õ��������ͺ����ݽṹ
#ifndef TAP_TRADE_API_DATA_TYPE_H
#define TAP_TRADE_API_DATA_TYPE_H
#include "TapAPICommDef.h"

#pragma pack(push, 1)


//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPIACCOUNTTYPE	�˺�����
 *	@{
 */
//=============================================================================
//! �˺�����
typedef TAPICHAR			TAPIAccountType;
//! ���˿ͻ�
const TAPIAccountType		TAPI_ACCOUNT_TYPE_PERSON				= 'P';
//! �����ͻ�
const TAPIAccountType		TAPI_ACCOUNT_TYPE_ORGANIZATION		    = 'O';
/** @}*/

//=============================================================================
/**
*	\addtogroup G_DATATYPE_T_TAPIRIGHTIDTYPE	Ȩ�ޱ�������
*	@{
*/
//=============================================================================
//! Ȩ�ޱ�������
typedef TAPIINT32			TAPIRightIDType;
//! ϵͳɾ��
const TAPIRightIDType		TAPI_RIGHT_ORDER_DEL	= 30001;
//! �������
const TAPIRightIDType		TAPI_RIGHT_ORDER_CHECK	= 30002;
//! ֻ�ɲ�ѯ
const TAPIRightIDType		TAPI_RIGHT_ONLY_QRY		= 31000;
//! ֻ�ɿ���
const TAPIRightIDType		TAPI_RIGHT_ONLY_OPEN	= 31001;
//! ���ڲ�ҵ�
const TAPIRightIDType		TAPI_RIGHT_SHFE_QUOTE	= 31002;
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPIUSERTYPETYPE	��¼�û��������
 *	@{
 */
//=============================================================================
//! ��¼�û��������
typedef TAPIINT32			TAPIUserTypeType;
//! Ͷ�����û�
const TAPIUserTypeType		TAPI_USERTYPE_CLIENT					=10000;
//! ������
const TAPIUserTypeType		TAPI_USERTYPE_BROKER					=20000;
//! ����Ա
const TAPIUserTypeType		TAPI_USERTYPE_TRADER					=30000;
//! ���
const TAPIUserTypeType		TAPI_USERTYPE_RISK						=40000;
//! ����Ա
const TAPIUserTypeType		TAPI_USERTYPE_MANAGER					=50000;
//! ����
const TAPIUserTypeType		TAPI_USERTYPE_QUOTE						=60000;
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPIACCOUNTSTATE	�˺�״̬
 *	@{
 */
//=============================================================================
//! �˺�״̬
typedef TAPICHAR			TAPIAccountState;
//! ����
const TAPIAccountState		TAPI_ACCOUNT_STATE_NORMAL				= 'N';
//! ����
const TAPIAccountState		TAPI_ACCOUNT_STATE_CANCEL				= 'C';
//! ����
const TAPIAccountState		TAPI_ACCOUNT_STATE_SLEEP               = 'S';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPIACCOUNTFAMILYTYPE	�����˻�����
 *	@{
 */
//=============================================================================
//! �����˻�����
typedef TAPICHAR			TAPIAccountFamilyType;
//! �����˺�
const	TAPIAccountFamilyType	TAPI_ACCOUNT_FAMILYTYPE_NOMAL	='N';
//! ���˻�
const	TAPIAccountFamilyType	TAPI_ACCOUNT_FAMILYTYPE_CHILD	='C';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPIORDERTYPETYPE	ί������
 *	@{
 */
//=============================================================================
//! ί������
typedef TAPICHAR				TAPIOrderTypeType;
//! �м�
const TAPIOrderTypeType			TAPI_ORDER_TYPE_MARKET				= '1';
//! �޼�
const TAPIOrderTypeType			TAPI_ORDER_TYPE_LIMIT				= '2';
//! �м�ֹ��
const TAPIOrderTypeType			TAPI_ORDER_TYPE_STOP_MARKET			= '3';
//! �޼�ֹ��
const TAPIOrderTypeType			TAPI_ORDER_TYPE_STOP_LIMIT			= '4';
//! ��Ȩ��Ȩ
const TAPIOrderTypeType			TAPI_ORDER_TYPE_OPT_EXEC			= '5';
//! ��Ȩ��Ȩ
const TAPIOrderTypeType			TAPI_ORDER_TYPE_OPT_ABANDON			= '6';
//! ѯ��
const TAPIOrderTypeType			TAPI_ORDER_TYPE_REQQUOT				= '7';
//! Ӧ��
const TAPIOrderTypeType			TAPI_ORDER_TYPE_RSPQUOT				= '8';
//! ����
const TAPIOrderTypeType			TAPI_ORDER_TYPE_SWAP				= '9';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPIORDERSOURCETYPE	ί����Դ
 *	@{
 */
//=============================================================================
//! ί����Դ
typedef TAPICHAR				TAPIOrderSourceType;
//! �������ӵ�
const TAPIOrderSourceType		TAPI_ORDER_SOURCE_SELF_ETRADER			= '1';
//! ������ӵ�
const TAPIOrderSourceType		TAPI_ORDER_SOURCE_PROXY_ETRADER			= '2';
//! �ⲿ���ӵ�(�ⲿ����ϵͳ�µ�����ϵͳ¼��)
const TAPIOrderSourceType		TAPI_ORDER_SOURCE_JTRADER				= '3';
//! �˹�¼�뵥(�ⲿ������ʽ�µ�����ϵͳ¼��)
const TAPIOrderSourceType		TAPI_ORDER_SOURCE_MANUAL				= '4';
//! carry��
const TAPIOrderSourceType		TAPI_ORDER_SOURCE_CARRY					= '5';
//! ��ʽ������
const TAPIOrderSourceType		TAPI_ORDER_SOURCE_PROGRAM				= '6';
//! ������Ȩ
const TAPIOrderSourceType		TAPI_ORDER_SOURCE_DELIVERY				= '7';
//! ��Ȩ����
const TAPIOrderSourceType		TAPI_ORDER_SOURCE_ABANDON				= '8';
//! ͨ����
const TAPIOrderSourceType		TAPI_ORDER_SOURCE_CHANNEL				= '9';
//! ��ʢAPI
const TAPIOrderSourceType		TAPI_ORDER_SOURCE_ESUNNY_API			= 'A';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPITIMEINFORCETYPE	ί����Ч����
 *	@{
 */
//=============================================================================
//! ί����Ч����
typedef TAPICHAR				TAPITimeInForceType;
//! ������Ч
const TAPITimeInForceType		TAPI_ORDER_TIMEINFORCE_GFD					= '0';
//! ȡ��ǰ��Ч
const TAPITimeInForceType		TAPI_ORDER_TIMEINFORCE_GTC					= '1';
//! ָ������ǰ��Ч
const TAPITimeInForceType		TAPI_ORDER_TIMEINFORCE_GTD					= '2';
//! FAK��IOC
const TAPITimeInForceType		TAPI_ORDER_TIMEINFORCE_FAK					= '3';
//! FOK
const TAPITimeInForceType		TAPI_ORDER_TIMEINFORCE_FOK					= '4';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPISIDETYPE	��������
 *	@{
 */
//=============================================================================
//! ��������
typedef TAPICHAR				TAPISideType;
//! ��
const TAPISideType				TAPI_SIDE_NONE							= 'N';
//! ����
const TAPISideType				TAPI_SIDE_BUY							= 'B';
//! ����
const TAPISideType				TAPI_SIDE_SELL							= 'S';
//! ˫��
const TAPISideType				TAPI_SIDE_ALL    						= 'A';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPIPOSITIONEFFECTTYPE	��ƽ����
 *	@{
 */
//=============================================================================
//! ��ƽ����
typedef TAPICHAR				TAPIPositionEffectType;
//! ���ֿ�ƽ
const TAPIPositionEffectType	TAPI_PositionEffect_NONE				= 'N';
//! ����
const TAPIPositionEffectType	TAPI_PositionEffect_OPEN				= 'O';
//! ƽ��
const TAPIPositionEffectType	TAPI_PositionEffect_COVER			= 'C';
//! ƽ����
const TAPIPositionEffectType	TAPI_PositionEffect_COVER_TODAY		= 'T';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPIHEDGEFLAGTYPE	Ͷ����ֵ����
 *	@{
 */
//=============================================================================
//! Ͷ����ֵ����
typedef TAPICHAR				TAPIHedgeFlagType;
//! ��
const TAPIHedgeFlagType			TAPI_HEDGEFLAG_NONE					= 'N';
//! Ͷ��
const TAPIHedgeFlagType			TAPI_HEDGEFLAG_T					= 'T';
//! ��ֵ
const TAPIHedgeFlagType			TAPI_HEDGEFLAG_B					= 'B';
//! ����
const TAPIHedgeFlagType			TAPI_HEDGEFLAG_L					= 'L';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPIORDERSTATETYPE	ί��״̬����
 *	@{
 */
//=============================================================================
//! ί��״̬����
typedef TAPICHAR				TAPIOrderStateType;
//! �ն��ύ
const TAPIOrderStateType		TAPI_ORDER_STATE_SUBMIT				= '0';
//! ������
const TAPIOrderStateType		TAPI_ORDER_STATE_ACCEPT				= '1';
//! ���Դ�����
const TAPIOrderStateType		TAPI_ORDER_STATE_TRIGGERING			= '2';
//! ������������
const TAPIOrderStateType		TAPI_ORDER_STATE_EXCTRIGGERING		= '3';
//! ���Ŷ�
const TAPIOrderStateType		TAPI_ORDER_STATE_QUEUED				= '4';
//! ���ֳɽ�
const TAPIOrderStateType		TAPI_ORDER_STATE_PARTFINISHED		= '5';
//! ��ȫ�ɽ�
const TAPIOrderStateType		TAPI_ORDER_STATE_FINISHED			= '6';
//! ������(�Ŷ���ʱ״̬)
const TAPIOrderStateType		TAPI_ORDER_STATE_CANCELING			= '7';
//! ���޸�(�Ŷ���ʱ״̬)
const TAPIOrderStateType		TAPI_ORDER_STATE_MODIFYING			= '8';
//! ��ȫ����
const TAPIOrderStateType		TAPI_ORDER_STATE_CANCELED			= '9';
//! �ѳ��൥
const TAPIOrderStateType		TAPI_ORDER_STATE_LEFTDELETED		= 'A';
//! ָ��ʧ��
const TAPIOrderStateType		TAPI_ORDER_STATE_FAIL				= 'B';
//! ����ɾ��
const TAPIOrderStateType		TAPI_ORDER_STATE_DELETED			= 'C';
//! �ѹ���
const TAPIOrderStateType		TAPI_ORDER_STATE_SUPPENDED			= 'D';
//! ����ɾ��
const TAPIOrderStateType		TAPI_ORDER_STATE_DELETEDFOREXPIRE	= 'E';
//! ����Ч����ѯ�۳ɹ�
const TAPIOrderStateType		TAPI_ORDER_STATE_EFFECT				= 'F';
//! �����롪����Ȩ����Ȩ������������ɹ�
const TAPIOrderStateType		TAPI_ORDER_STATE_APPLY				= 'G';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPICALCULATEMODETYPE	���㷽ʽ
 *	@{
 */
//=============================================================================
//! ���㷽ʽ
typedef TAPICHAR               TAPICalculateModeType;
//! ����
const TAPICalculateModeType		TAPI_CALCULATE_MODE_PERCENTAGE          = '1';
//! ����
const TAPICalculateModeType		TAPI_CALCULATE_MODE_QUOTA               = '2';
//! ���Է�ʽ
const TAPICalculateModeType		TAPI_CALCULATE_MODE_ABSOLUTE		= '7';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPIMATCHSOURCETYPE	�ɽ���Դ
 *	@{
 */
//=============================================================================
//! �ɽ���Դ
typedef TAPICHAR				TAPIMatchSourceType;
//! ȫ��
const TAPIMatchSourceType		TAPI_MATCH_SOURCE_ALL                   = '0';
//! �������ӵ�
const TAPIMatchSourceType		TAPI_MATCH_SOURCE_SELF_ETRADER          = '1';
//! ������ӵ�
const TAPIMatchSourceType		TAPI_MATCH_SOURCE_PROXY_ETRADER         = '2';
//! �ⲿ���ӵ�
const TAPIMatchSourceType		TAPI_MATCH_SOURCE_JTRADER				= '3';
//! �˹�¼�뵥
const TAPIMatchSourceType		TAPI_MATCH_SOURCE_MANUAL				= '4';
//! carry��
const TAPIMatchSourceType		TAPI_MATCH_SOURCE_CARRY					= '5';
//! ��ʽ����
const TAPIMatchSourceType		TAPI_MATCH_SOURCE_PROGRAM				= '6';
//! ������Ȩ
const TAPIMatchSourceType		TAPI_MATCH_SOURCE_DELIVERY				= '7';
//! ��Ȩ����
const TAPIMatchSourceType		TAPI_MATCH_SOURCE_ABANDON				= '8';
//! ͨ����
const TAPIMatchSourceType		TAPI_MATCH_SOURCE_CHANNEL				= '9';
//! ��ʢAPI
const TAPIMatchSourceType		TAPI_MATCH_SOURCE_ESUNNY_API			= 'A';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPIOPENCLOSEMODETYPE	��ƽ��ʽ
 *	@{
 */
//=============================================================================
//! ��ƽ��ʽ
typedef TAPICHAR				TAPIOpenCloseModeType;
//! �����ֿ�ƽ
const TAPIOpenCloseModeType		TAPI_CLOSE_MODE_NONE				= 'N';
//! ƽ��δ�˽�
const TAPIOpenCloseModeType		TAPI_CLOSE_MODE_UNFINISHED			= 'U';
//! ���ֿ��ֺ�ƽ��
const TAPIOpenCloseModeType		TAPI_CLOSE_MODE_OPENCOVER			= 'C';
//! ���ֿ��֡�ƽ�ֺ�ƽ��
const TAPIOpenCloseModeType		TAPI_CLOSE_MODE_CLOSETODAY			= 'T';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPIFUTUREALGTYPE	�ڻ��㷨
 *	@{
 */
//=============================================================================
//! �ڻ��㷨
typedef TAPICHAR				TAPIFutureAlgType;
//! ���
const TAPIFutureAlgType			TAPI_FUTURES_ALG_ZHUBI                  = '1';
//! ����
const TAPIFutureAlgType			TAPI_FUTURES_ALG_DINGSHI                = '2';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPIOPTIONALGTYPE	��Ȩ�㷨
 *	@{
 */
//=============================================================================
//! ��Ȩ�㷨
typedef TAPICHAR				TAPIOptionAlgType;
//! �ڻ���ʽ
const TAPIOptionAlgType         TAPI_OPTION_ALG_FUTURES                 = '1';
//! ��Ȩ��ʽ
const TAPIOptionAlgType         TAPI_OPTION_ALG_OPTION                  = '2';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPIBANKACCOUNTLWFLAGTYPE	����ұ�ʶ
 *	@{
 */
//=============================================================================
//! ����ұ�ʶ
typedef TAPICHAR				TAPIBankAccountLWFlagType;
//! ����������˻�
const TAPIBankAccountLWFlagType	TAPI_LWFlag_L					= 'L';
//! �ͻ���������˻�
const TAPIBankAccountLWFlagType	TAPI_LWFlag_W					= 'W';
/** @}*/


//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPIMARGINCALCULATEMODETYPE	�ڻ���֤��ʽ
 *	@{
 */
//=============================================================================
//! �ڻ���֤��ʽ
typedef TAPICHAR				TAPIMarginCalculateModeType;
//! �ֱ�
const TAPIMarginCalculateModeType TAPI_DEPOSITCALCULATE_MODE_FEN     = '1';
//! ����
const TAPIMarginCalculateModeType TAPI_DEPOSITCALCULATE_MODE_SUO     = '2';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPIOPTIONMARGINCALCULATEMODETYPE	��Ȩ��֤��ʽ
 *	@{
 */
//=============================================================================
//! ��Ȩ��֤��ʽ,�ݴ��жϸ�Ʒ����Ȩ���ú������ü��㹫ʽ���㱣֤��
typedef TAPICHAR				TAPIOptionMarginCalculateModeType;
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPICMBDIRECTTYPE	��Ϸ���
 *	@{
 */
//=============================================================================
//! ��Ϸ���,Ʒ��������Ϻ�Լ����������͵ڼ�����ͬ
typedef TAPICHAR				TAPICmbDirectType;
//! �͵�һ��һ��
const TAPICmbDirectType         TAPI_CMB_DIRECT_FIRST                    = '1';
//! �͵ڶ���һ��
const TAPICmbDirectType         TAPI_CMB_DIRECT_SECOND                   = '2';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPIDELIVERYMODETYPE	������Ȩ��ʽ
 *	@{
 */
//=============================================================================
//! ������Ȩ��ʽ,�ڻ�����Ȩ�˽�ķ�ʽ
typedef TAPICHAR				TAPIDeliveryModeType;
//! ʵ�ｻ��
const TAPIDeliveryModeType		TAPI_DELIVERY_MODE_GOODS			= 'G';
//! �ֽ𽻸�
const TAPIDeliveryModeType		TAPI_DELIVERY_MODE_CASH				= 'C';
//! ��Ȩ��Ȩ
const TAPIDeliveryModeType		TAPI_DELIVERY_MODE_EXECUTE			= 'E';
//! ��Ȩ����
const TAPIDeliveryModeType		TAPI_DELIVERY_MODE_ABANDON			= 'A';
//! �۽�����Ȩ
const TAPIDeliveryModeType		TAPI_DELIVERY_MODE_HKF				= 'H';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPICONTRACTTYPETYPE	��Լ����
 *	@{
 */
//=============================================================================
//! ��Լ����
typedef TAPICHAR				TAPIContractTypeType;
//! ���������Լ
const TAPIContractTypeType		TAPI_CONTRACT_TYPE_TRADEQUOTE		='1';
//! �����Լ
const TAPIContractTypeType		TAPI_CONTRACT_TYPE_QUOTE			='2';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPITACTICSTYPETYPE	���Ե�����
 *	@{
 */
//=============================================================================
//! ���Ե�����
typedef TAPICHAR					TAPITacticsTypeType;
//! ��
const TAPITacticsTypeType			TAPI_TACTICS_TYPE_NONE				= 'N';
//! Ԥ����(��)
const TAPITacticsTypeType			TAPI_TACTICS_TYPE_READY				= 'M';
//! �Զ���
const TAPITacticsTypeType			TAPI_TACTICS_TYPE_ATUO				= 'A';
//! ������
const TAPITacticsTypeType			TAPI_TACTICS_TYPE_CONDITION			= 'C';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPIORDERACT	������������
 *	@{
 */
//=============================================================================
//! ������������
typedef TAPICHAR		TAPIORDERACT;
//! ����
const TAPIORDERACT APIORDER_INSERT			= '1';
//! �ĵ�
const TAPIORDERACT APIORDER_MODIFY			= '2';
//! ����
const TAPIORDERACT APIORDER_DELETE			= '3';
//! ����
const TAPIORDERACT APIORDER_SUSPEND			= '4';
//! ����
const TAPIORDERACT APIORDER_ACTIVATE		= '5';
//! ɾ��
const TAPIORDERACT APIORDER_SYSTEM_DELETE	= '6';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPITRIGGERCONDITIONTYPE	������������
 *	@{
 */
//=============================================================================
//! ������������
typedef TAPICHAR				TAPITriggerConditionType;
//! ��
const TAPITriggerConditionType	TAPI_TRIGGER_CONDITION_NONE			= 'N';
//! ���ڵ���
const TAPITriggerConditionType	TAPI_TRIGGER_CONDITION_GREAT		= 'G';
//! С�ڵ���
const TAPITriggerConditionType	TAPI_TRIGGER_CONDITION_LITTLE		= 'L';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPITRIGGERPRICETYPETYPE	�����۸�����
 *	@{
 */
//=============================================================================
//! �����۸�����
typedef TAPICHAR				TAPITriggerPriceTypeType;
//! ��
const TAPITriggerPriceTypeType	TAPI_TRIGGER_PRICE_NONE				= 'N';
//! ���
const TAPITriggerPriceTypeType	TAPI_TRIGGER_PRICE_BUY				= 'B';
//! ����
const TAPITriggerPriceTypeType	TAPI_TRIGGER_PRICE_SELL				= 'S';
//! ���¼�
const TAPITriggerPriceTypeType	TAPI_TRIGGER_PRICE_LAST				= 'L';
/** @}*/

//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPIMARKETLEVELTYPE	�м۴�����
 *	@{
 */
//=============================================================================
//! �м۴�����
typedef TAPIUINT8				TAPIMarketLevelType;
//! �����
const TAPIMarketLevelType	TAPI_MARKET_LEVEL_0						= 0;
//! 1�����ż�
const TAPIMarketLevelType	TAPI_MARKET_LEVEL_1						= 1;
//! 5��
const TAPIMarketLevelType	TAPI_MARKET_LEVEL_5						= 5;
//! 10��
const TAPIMarketLevelType	TAPI_MARKET_LEVEL_10					= 10;
/** @}*/


//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPITRADINGSTATETYPE	����״̬
 *	@{
 */
//=============================================================================
//! ����״̬
typedef TAPICHAR               TAPITradingStateType;
//! ���Ͼ���
const TAPITradingStateType      TAPI_TRADE_STATE_BID                = '1';
//! ���Ͼ��۴��
const TAPITradingStateType      TAPI_TRADE_STATE_MATCH              = '2';
//! ��������
const TAPITradingStateType      TAPI_TRADE_STATE_CONTINUOUS         = '3';
//! ������ͣ
const TAPITradingStateType      TAPI_TRADE_STATE_PAUSED             = '4';
//! ����
const TAPITradingStateType      TAPI_TRADE_STATE_CLOSE              = '5';
//! ���д���ʱ��
const TAPITradingStateType      TAPI_TRADE_STATE_DEALLAST           = '6';
//! ����δ��
const TAPITradingStateType		TAPI_TRADE_STATE_GWDISCONNECT		= '0';
//! δ֪״̬
const TAPITradingStateType		TAPI_TRADE_STATE_UNKNOWN			= 'N';
//! ����ʼ��
const TAPITradingStateType		TAPI_TRADE_STATE_INITIALIZE			= 'I';
//! ׼������
const TAPITradingStateType		TAPI_TRADE_STATE_READY				= 'R';
/** @}*/



//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPINOTICEIGNOREFLAGTYPE	���Ժ�̨����֪ͨ���
 *	@{
 */
//=============================================================================
//! ���Ժ�̨����֪ͨ���
typedef TAPIUINT32              TAPINoticeIgnoreFlagType;
//! ����������Ϣ
const TAPINoticeIgnoreFlagType TAPI_NOTICE_IGNORE_NONE				= 0x00000000;
//! ������������
const TAPINoticeIgnoreFlagType TAPI_NOTICE_IGNORE_ALL				= 0xFFFFFFFF;
//! �����ʽ�����:OnRtnFund
const TAPINoticeIgnoreFlagType TAPI_NOTICE_IGNORE_FUND				= 0x00000001;
//! ����ί������:OnRtnOrder
const TAPINoticeIgnoreFlagType TAPI_NOTICE_IGNORE_ORDER				= 0x00000002;
//! ���Գɽ�����:OnRtnFill
const TAPINoticeIgnoreFlagType TAPI_NOTICE_IGNORE_FILL				= 0x00000004;
//! ���Գֲ�����:OnRtnPosition
const TAPINoticeIgnoreFlagType TAPI_NOTICE_IGNORE_POSITION			= 0x00000008;
//! ����ƽ������:OnRtnClose
const TAPINoticeIgnoreFlagType TAPI_NOTICE_IGNORE_CLOSE				= 0x00000010;
//! ���Գֲ�ӯ������:OnRtnPositionProfit
const TAPINoticeIgnoreFlagType TAPI_NOTICE_IGNORE_POSITIONPROFIT	= 0x00000020;
/** @}*/


//=============================================================================
/**
 *	\addtogroup G_DATATYPE_T_TAPIORDERQRYTYPETYPE	ί�в�ѯ����
 *	@{
 */
//=============================================================================
//! ί�в�ѯ����
typedef TAPICHAR              TAPIOrderQryTypeType;
//! ��������ί��
const TAPIOrderQryTypeType TAPI_ORDER_QRY_TYPE_ALL				= 'A';
//! ֻ����δ������ί��
const TAPIOrderQryTypeType TAPI_ORDER_QRY_TYPE_UNENDED			= 'U';
/** @}*/


//! ------------------------------------------------------------------------------------------

//! ��¼��֤��Ϣ
struct TapAPITradeLoginAuth
{
	TAPISTR_20					UserNo;					///< �û���
	TAPIYNFLAG					ISModifyPassword;		///< �Ƿ��޸�����	
	TAPISTR_20					Password;				///< ����
	TAPISTR_20					NewPassword;			///< ������
	TAPIYNFLAG					ISDDA;					///< �Ƿ���Ҫ��̬��֤
	TAPISTR_30					DDASerialNo;			///< ��̬��֤��
	TAPINoticeIgnoreFlagType	NoticeIgnoreFlag;		///< ��������֪ͨ���
};

//! ��¼������Ϣ
struct TapAPITradeLoginRspInfo
{
	TAPISTR_20					UserNo;							///< �û����
	TAPIUserTypeType			UserType;						///< �û�����
	TAPISTR_20					UserName;						///< �û���
	TAPISTR_20					QuoteTempPassword;				///< ������ʱ����
	TAPISTR_50					ReservedInfo;					///< Ԥ����Ϣ
	TAPISTR_40					LastLoginIP;					///< �ϴε�¼IP
	TAPIUINT32					LastLoginProt;					///< �ϴε�¼�˿�
	TAPIDATETIME				LastLoginTime;					///< �ϴε�¼ʱ��
	TAPIDATETIME				LastLogoutTime;					///< �ϴ��˳�ʱ��
	TAPIDATE					TradeDate;						///< ��ǰ��������
	TAPIDATETIME				LastSettleTime;					///< �ϴν���ʱ��
	TAPIDATETIME				StartTime;						///< ϵͳ����ʱ��
	TAPIDATETIME				InitTime;						///< ϵͳ��ʼ��ʱ��
};

//! �˺������Ϣ��ѯ����
struct TapAPIAccQryReq
{
	TAPISTR_20					AccountNo;						///< �ʽ��˺�				
};

//! �ʽ��˺���Ϣ
struct TapAPIAccountInfo
{
	TAPISTR_20              AccountNo;                              ///< �ʽ��˺�
	TAPIAccountType			AccountType;                            ///< �˺�����
	TAPIAccountState		AccountState;                           ///< �˺�״̬
	TAPISTR_20				AccountShortName;                       ///< �˺ż��
	TAPIYNFLAG				AccountIsDocHolder;                     ///< �Ƿ��֤��
	TAPIYNFLAG				IsMarketMaker;							///< �Ƿ���������
	TAPIAccountFamilyType	AccountFamilyType;						///< �����˺�����
};

//! �ͻ��µ�����ṹ
struct TapAPINewOrder
{
	TAPISTR_20					AccountNo;						///< �ͻ��ʽ��ʺ�
	TAPISTR_10					ExchangeNo;						///< ���������
	TAPICommodityType			CommodityType;					///< Ʒ������
	TAPISTR_10					CommodityNo;					///< Ʒ�ֱ�������
	TAPISTR_10					ContractNo;						///< ��Լ1
	TAPISTR_10					StrikePrice;					///< ִ�м۸�1
	TAPICallOrPutFlagType		CallOrPutFlag;					///< ���ſ���1
	TAPISTR_10					ContractNo2;					///< ��Լ2
	TAPISTR_10					StrikePrice2;					///< ִ�м۸�2
	TAPICallOrPutFlagType		CallOrPutFlag2;					///< ���ſ���2
	TAPIOrderTypeType			OrderType;						///< ί������
	TAPIOrderSourceType			OrderSource;					///< ί����Դ
	TAPITimeInForceType			TimeInForce;					///< ί����Ч����
	TAPIDATETIME				ExpireTime;						///< ��Ч����(GTD�����ʹ��)
	TAPIYNFLAG					IsRiskOrder;					///< �Ƿ���ձ���
	TAPISideType				OrderSide;						///< ��������
	TAPIPositionEffectType		PositionEffect;					///< ��ƽ��־1
	TAPIPositionEffectType		PositionEffect2;				///< ��ƽ��־2
	TAPISTR_50					InquiryNo;						///< ѯ�ۺ�
	TAPIHedgeFlagType			HedgeFlag;						///< Ͷ����ֵ
	TAPIREAL64					OrderPrice;						///< ί�м۸�1
	TAPIREAL64					OrderPrice2;					///< ί�м۸�2��������Ӧ��ʹ��
	TAPIREAL64					StopPrice;						///< �����۸�
	TAPIUINT32					OrderQty;						///< ί������
	TAPIUINT32					OrderMinQty;					///< ��С�ɽ���
	TAPIUINT32					MinClipSize;					///< ��ɽ����С�����
	TAPIUINT32					MaxClipSize;					///< ��ɽ����������
	TAPIINT32					RefInt;							///< ���Ͳο�ֵ
	TAPISTR_50					RefString;						///< �ַ����ο�ֵ
	TAPITacticsTypeType			TacticsType;					///< ���Ե�����
	TAPITriggerConditionType	TriggerCondition;				///< ��������
	TAPITriggerPriceTypeType	TriggerPriceType;				///< �����۸�����	
	TAPIYNFLAG					AddOneIsValid;					///< �Ƿ�T+1��Ч
	TAPIUINT32					OrderQty2;						///< ί������2
	TAPIHedgeFlagType			HedgeFlag2;						///< Ͷ����ֵ2
	TAPIMarketLevelType			MarketLevel;					///< �м۴�����
	TAPIYNFLAG					OrderDeleteByDisConnFlag;		///< �������ʧ��ʱ���������Զ�������ʶ
	TAPISTR_10					UpperChannelNo;					///< ����ͨ����
};

//! ί�����Ͳ�ѯӦ��
struct TapAPICommodityOrderTypeQryRsp
{
	TapAPICommodity			Commodity;							///< Ʒ����Ϣ
	TAPIINT32				Count;								///< ί�����͸���
	TAPIOrderTypeType*		OrderTypes;							///< ί�����������׵�ַ
};

//! ί����Ч���Ͳ�ѯӦ��
struct TapAPICommodityOrderTimeInForceQryRsp
{
	TapAPICommodity			Commodity;							///< Ʒ����Ϣ
	TAPIINT32				Count;								///< ί����Ч���͸���
	TAPITimeInForceType*	OrderTimeInForces;					///< ί����Ч���������׵�ַ
};


//! ί��������Ϣ
struct TapAPIOrderInfo
{
	TAPISTR_20					AccountNo;						///< �ͻ��ʽ��ʺ�
	TAPISTR_10					ExchangeNo;						///< ���������
	TAPICommodityType			CommodityType;					///< Ʒ������
	TAPISTR_10					CommodityNo;					///< Ʒ�ֱ�������
	TAPISTR_10					ContractNo;						///< ��Լ1
	TAPISTR_10					StrikePrice;					///< ִ�м۸�1
	TAPICallOrPutFlagType		CallOrPutFlag;					///< ���ſ���1
	TAPISTR_10					ContractNo2;					///< ��Լ2
	TAPISTR_10					StrikePrice2;					///< ִ�м۸�2
	TAPICallOrPutFlagType		CallOrPutFlag2;					///< ���ſ���2

	TAPIOrderTypeType			OrderType;						///< ί������
	TAPIOrderSourceType			OrderSource;					///< ί����Դ
	TAPITimeInForceType			TimeInForce;					///< ί����Ч����
	TAPIDATETIME				ExpireTime;						///< ��Ч����(GTD�����ʹ��)

	TAPIYNFLAG					IsRiskOrder;					///< �Ƿ���ձ���
	TAPISideType				OrderSide;						///< ��������
	TAPIPositionEffectType		PositionEffect;					///< ��ƽ��־1
	TAPIPositionEffectType		PositionEffect2;				///< ��ƽ��־2
	TAPISTR_50					InquiryNo;						///< ѯ�ۺ�
	TAPIHedgeFlagType			HedgeFlag;						///< Ͷ����ֵ
	TAPIREAL64					OrderPrice;						///< ί�м۸�1
	TAPIREAL64					OrderPrice2;					///< ί�м۸�2��������Ӧ��ʹ��
	TAPIREAL64					StopPrice;						///< �����۸�
	TAPIUINT32					OrderQty;						///< ί������
	TAPIUINT32					OrderMinQty;					///< ��С�ɽ���
	TAPIUINT32					OrderCanceledQty;				///< ��������

	TAPIUINT32					MinClipSize;					///< ��ɽ����С�����
	TAPIUINT32					MaxClipSize;					///< ��ɽ����������
	TAPISTR_50					LicenseNo;						///< �����Ȩ��

	TAPIINT32					RefInt;							///< ���Ͳο�ֵ
	TAPISTR_50					RefString;						///< �ַ����ο�ֵ
	TAPISTR_20					ParentAccountNo;				///< �ϼ��ʽ��˺�

	TAPICHAR					ServerFlag;						///< ��������ʶ
	TAPISTR_20					OrderNo;						///< ί�б���
	TAPISTR_50                  ClientOrderNo;					///< �ͻ��˱���ί�б��
	TAPISTR_20					OrderLocalNo;					///< ���غ�
	TAPISTR_50					OrderSystemNo;					///< ϵͳ��
	TAPISTR_50					OrderExchangeSystemNo;			///< ������ϵͳ�� 
    TAPISTR_20					OrderParentNo;					///< ������
	TAPISTR_50					OrderParentSystemNo;			///< ����ϵͳ��
	TAPISTR_10					TradeNo;						///< ���ױ���

	TAPISTR_10					UpperNo;						///< ���ֺ�
	TAPISTR_10					UpperChannelNo;					///< ����ͨ����
	TAPISTR_20					UpperSettleNo;					///< ��Ա�ź������
	TAPISTR_20					UpperUserNo;					///< ���ֵ�¼��
	TAPISTR_20					OrderInsertUserNo;				///< �µ���
	TAPIDATETIME				OrderInsertTime;				///< �µ�ʱ��
	TAPISTR_20					OrderCommandUserNo;				///< ¼��������
	TAPISTR_20					OrderUpdateUserNo;				///< ί�и�����
	TAPIDATETIME				OrderUpdateTime;				///< ί�и���ʱ��
	TAPIOrderStateType			OrderState;						///< ί��״̬
	TAPIREAL64					OrderMatchPrice;				///< �ɽ���1
	TAPIREAL64					OrderMatchPrice2;				///< �ɽ���2
	TAPIUINT32					OrderMatchQty;					///< �ɽ���1
	TAPIUINT32					OrderMatchQty2;					///< �ɽ���2

	TAPIUINT32					ErrorCode;						///< ���һ�β���������Ϣ��
	TAPISTR_50					ErrorText;						///< ������Ϣ

	TAPIYNFLAG					IsBackInput;					///< �Ƿ�Ϊ¼��ί�е�
	TAPIYNFLAG					IsDeleted;						///< ί�гɽ�ɾ����
	TAPIYNFLAG					IsAddOne;						///< �Ƿ�ΪT+1��

	TAPIUINT32					OrderStreamID;					///< ί����ˮ��
	TAPIUINT32					UpperStreamID;					///< ��������

	TAPIREAL64					ContractSize;					///< ÿ�ֳ������������
	TAPIREAL64					ContractSize2;					///< ÿ�ֳ������������
	TAPISTR_10					CommodityCurrencyGroup;			///< Ʒ�ֱ�����
	TAPISTR_10					CommodityCurrency;				///< Ʒ�ֱ���
	TAPICalculateModeType		FeeMode;						///< �����Ѽ��㷽ʽ
	TAPIREAL64					FeeParam;						///< �����Ѳ���ֵ ���������Ѿ����տ��������Ѽ���
	TAPISTR_10					FeeCurrencyGroup;				///< �ͻ������ѱ�����
	TAPISTR_10					FeeCurrency;					///< �ͻ������ѱ���
	TAPICalculateModeType		FeeMode2;						///< �����Ѽ��㷽ʽ
	TAPIREAL64					FeeParam2;						///< �����Ѳ���ֵ ���������Ѿ����տ��������Ѽ���
	TAPISTR_10					FeeCurrencyGroup2;				///< �ͻ������ѱ�����
	TAPISTR_10					FeeCurrency2;					///< �ͻ������ѱ���
	TAPICalculateModeType		MarginMode;						///< ��֤����㷽ʽ
	TAPIREAL64					MarginParam;					///< ��֤�����ֵ
	TAPICalculateModeType		MarginMode2;					///< ��֤����㷽ʽ
	TAPIREAL64					MarginParam2;					///< ��֤�����ֵ
	TAPIREAL64					PreSettlePrice;					///< ������  ������ʽ���м۵�����϶���ʹ��
	TAPIREAL64					PreSettlePrice2;				///< ������  ������ʽ���м۵�����϶���ʹ��
	TAPIUINT32					OpenVol;						///< Ԥ�������� ί�������еĿ��ֲ���
	TAPIUINT32					CoverVol;						///< Ԥƽ������ ί�������е�ƽ�ֲ���
	TAPIUINT32					OpenVol2;						///< Ԥ�������� ί�������еĿ��ֲ���
	TAPIUINT32					CoverVol2;						///< Ԥƽ������ ί�������е�ƽ�ֲ���
	TAPIREAL64					FeeValue;						///< ����������
	TAPIREAL64					MarginValue;					///< ���ᱣ֤�� �ϲ�����Ĵ���Ϊ0
	TAPITacticsTypeType			TacticsType;					///< ���Ե�����
	TAPITriggerConditionType	TriggerCondition;				///< ��������
	TAPITriggerPriceTypeType	TriggerPriceType;				///< �����۸�����
	TAPIYNFLAG					AddOneIsValid;					///< �Ƿ�T+1��Ч
	TAPIUINT32					OrderQty2;						///< ί������2
	TAPIHedgeFlagType			HedgeFlag2;						///< Ͷ����ֵ2
	TAPIMarketLevelType			MarketLevel;					///< �м۴�����
	TAPIYNFLAG					OrderDeleteByDisConnFlag;		///< �������ʧ��ʱ���������Զ�������ʶ
};

//! ����֪ͨ�ṹ
struct TapAPIOrderInfoNotice
{
	TAPIUINT32					SessionID;						///< �ỰID
	TAPIUINT32					ErrorCode; 						///< ������
	TapAPIOrderInfo*			OrderInfo;						///< ί��������Ϣ
};

//! ��������Ӧ��ṹ
struct TapAPIOrderActionRsp
{
	TAPIORDERACT				ActionType;						///< ��������
	TapAPIOrderInfo*			OrderInfo;						///< ί����Ϣ
};


//! �ͻ��ĵ�����
struct TapAPIAmendOrder
{
    TapAPINewOrder              ReqData;                        ///< ������������
    TAPICHAR					ServerFlag;						///< ��������ʶ
    TAPISTR_20                  OrderNo;                        ///< ί�б��
};

//! �ͻ���������ṹ
struct TapAPIOrderCancelReq
{
	TAPIINT32					RefInt;							///< ���Ͳο�ֵ
	TAPISTR_50					RefString;						///< �ַ����ο�ֵ
	TAPICHAR					ServerFlag;						///< ��������ʶ
	TAPISTR_20					OrderNo;						///< ί�б���
};

//! ����ί������ṹ
typedef TapAPIOrderCancelReq TapAPIOrderDeactivateReq;

//! ����ί������ṹ
typedef TapAPIOrderCancelReq TapAPIOrderActivateReq;

//! ɾ��ί������ṹ
typedef TapAPIOrderCancelReq TapAPIOrderDeleteReq;

//! ί�в�ѯ����ṹ
struct TapAPIOrderQryReq
{
	TAPIOrderQryTypeType	OrderQryType;	///< ί�в�ѯ����
};

//! ί�����̲�ѯ
struct TapAPIOrderProcessQryReq
{
	TAPICHAR					ServerFlag;						///< ��������ʶ
	TAPISTR_20					OrderNo;						///< ί�б���
};

//! �ɽ���ѯ����ṹ
struct TapAPIFillQryReq
{
	TAPIUINT32					StreamID;				    	///< ��ˮ��
};

//! �ɽ���Ϣ
struct TapAPIFillInfo
{
	TAPISTR_20					AccountNo;						///< �ͻ��ʽ��ʺ�
	TAPISTR_20					ParentAccountNo;				///< �ϼ��ʽ��˺�
	TAPISTR_10					ExchangeNo;						///< ���������
	TAPICommodityType			CommodityType;					///< Ʒ������
	TAPISTR_10					CommodityNo;					///< Ʒ�ֱ�������
	TAPISTR_10					ContractNo;						///< ��Լ1
	TAPISTR_10					StrikePrice;					///< ִ�м۸�
	TAPICallOrPutFlagType		CallOrPutFlag;					///< ���ſ���
	TAPISTR_10					ExchangeNoRef;					///< ���������
	TAPICommodityType			CommodityTypeRef;				///< Ʒ������
	TAPISTR_10					CommodityNoRef;					///< Ʒ�ֱ�������
	TAPISTR_10					ContractNoRef;					///< ��Լ
	TAPISTR_10					StrikePriceRef;					///< ִ�м۸�
	TAPICallOrPutFlagType		CallOrPutFlagRef;				///< ���ſ���
	TAPISTR_10					OrderExchangeNo;				///< ���������
	TAPICommodityType			OrderCommodityType;				///< Ʒ������
	TAPISTR_10					OrderCommodityNo;				///< Ʒ�ֱ�������
	TAPIOrderTypeType			OrderType;						///< ί������
	TAPIMatchSourceType			MatchSource;					///< ί����Դ
	TAPITimeInForceType			TimeInForce;					///< ί����Ч����
	TAPIDATETIME				ExpireTime;						///< ��Ч����(GTD�����ʹ��)
	TAPIYNFLAG					IsRiskOrder;					///< �Ƿ���ձ���
	TAPISideType				MatchSide;						///< ��������
	TAPIPositionEffectType		PositionEffect;					///< ��ƽ��־1
	TAPIPositionEffectType		PositionEffectRef;				///< ��ƽ��־2
	TAPIHedgeFlagType			HedgeFlag;						///< Ͷ����ֵ
	TAPICHAR					ServerFlag;						///< ��������ʶ
	TAPISTR_20					OrderNo;						///< ί�б���
	TAPISTR_20					OrderLocalNo;					///< ί�б��غ�
	TAPISTR_20					MatchNo;						///< ���سɽ���
	TAPISTR_70					ExchangeMatchNo;				///< �������ɽ���
	TAPIDATETIME				MatchDateTime;					///< �ɽ�ʱ��
	TAPIDATETIME				UpperMatchDateTime;				///< ���ֳɽ�ʱ��
	TAPISTR_10					UpperNo;						///< ���ֺ�
	TAPISTR_10					UpperChannelNo;					///< ����ͨ����
	TAPISTR_20					UpperSettleNo;					///< ��Ա�ź��������
	TAPISTR_20					UpperUserNo;					///< ���ֵ�¼��
	TAPISTR_10					TradeNo;						///< ���ױ���
	TAPIREAL64					MatchPrice;						///< �ɽ���
	TAPIUINT32					MatchQty;						///< �ɽ���
	TAPIYNFLAG					IsBackInput;					///< �Ƿ�Ϊ¼��ί�е�
	TAPIYNFLAG					IsDeleted;						///< ί�гɽ�ɾ����
	TAPIYNFLAG					IsAddOne;						///< �Ƿ�ΪT+1��
	TAPIUINT32					MatchStreamID;					///< ί����ˮ��
	TAPIUINT32					UpperStreamID;					///< ��������
	TAPIOpenCloseModeType		OpenCloseMode;                  ///< ��ƽ��ʽ
	TAPIREAL64					ContractSize;					///< ÿ�ֳ������������
	TAPISTR_10					CommodityCurrencyGroup;			///< Ʒ�ֱ�����
	TAPISTR_10					CommodityCurrency;				///< Ʒ�ֱ���
	TAPICalculateModeType		FeeMode;						///< �����Ѽ��㷽ʽ
	TAPIREAL64					FeeParam;						///< �����Ѳ���ֵ ���������Ѿ����տ��������Ѽ���
	TAPISTR_10					FeeCurrencyGroup;				///< �ͻ������ѱ�����
	TAPISTR_10					FeeCurrency;					///< �ͻ������ѱ���
	TAPIREAL64					PreSettlePrice;					///< ������
	TAPIREAL64					FeeValue;						///< ������
	TAPIYNFLAG					IsManualFee;					///< �˹��ͻ������ѱ��
	TAPIREAL64					Turnover;						///< �ɽ����
	TAPIREAL64					PremiumIncome;					///< Ȩ������ȡ
	TAPIREAL64					PremiumPay;						///< Ȩ����֧��
	TAPISTR_20					OppoMatchNo;					///< ���ֱ��سɽ��ţ����ֺ�ƽ�ֶ�Ӧ��ţ�
	TAPIREAL64					CloseProfit;					///< ƽ��ӯ��
	TAPIREAL64					UnExpProfit;					///< δ����ƽӯ
	TAPIREAL64					UpperMatchPrice;				///< ���ֳɽ���
	TAPIREAL64					QuotePrice;						///< ��ǰ�����
	TAPIREAL64					ClosePL;                        ///< ���ƽӯ
};

//! ƽ�ֲ�ѯ����ṹ
struct TapAPICloseQryReq
{
};

//! �ֲֲ�ѯ����ṹ
struct TapAPIPositionQryReq
{
};

//! �ֲ���Ϣ
struct TapAPIPositionInfo
{
	TAPISTR_20					AccountNo;						///< �ͻ��ʽ��ʺ�
	TAPISTR_20					ParentAccountNo;				///< �ϼ��ʽ��˺�
	TAPISTR_10					ExchangeNo;						///< ���������
	TAPICommodityType			CommodityType;					///< Ʒ������
	TAPISTR_10					CommodityNo;					///< Ʒ�ֱ�������
	TAPISTR_10					ContractNo;						///< ��Լ1
	TAPISTR_10					StrikePrice;					///< ִ�м۸�
	TAPICallOrPutFlagType		CallOrPutFlag;					///< ���ſ���
	TAPISTR_10					ExchangeNoRef;					///< ���������
	TAPICommodityType			CommodityTypeRef;				///< Ʒ������
	TAPISTR_10					CommodityNoRef;					///< Ʒ�ֱ�������
	TAPISTR_10					ContractNoRef;					///< ��Լ
	TAPISTR_10					StrikePriceRef;					///< ִ�м۸�
	TAPICallOrPutFlagType		CallOrPutFlagRef;				///< ���ſ���
	TAPISTR_10					OrderExchangeNo;				///< ���������
	TAPICommodityType			OrderCommodityType;				///< Ʒ������
	TAPISTR_10					OrderCommodityNo;				///< Ʒ�ֱ�������
	TAPIOrderTypeType			OrderType;						///< ί������
	TAPIMatchSourceType			MatchSource;					///< ί����Դ
	TAPITimeInForceType			TimeInForce;					///< ί����Ч����
	TAPIDATETIME				ExpireTime;						///< ��Ч����(GTD�����ʹ��)
	TAPISideType				MatchSide;						///< ��������
	TAPIHedgeFlagType			HedgeFlag;						///< Ͷ����ֵ
	TAPISTR_70					PositionNo;						///< ���سֲֺţ���������д
	TAPICHAR					ServerFlag;						///< ��������ʶ
	TAPISTR_20					OrderNo;						///< ί�б���
	TAPISTR_20					MatchNo;						///< ���سɽ���
	TAPISTR_70					ExchangeMatchNo;				///< �������ɽ���
	TAPIDATE					MatchDate;						///< �ɽ�����
	TAPIDATETIME				MatchTime;						///< �ɽ�ʱ��
	TAPIDATETIME				UpperMatchTime;					///< ���ֳɽ�ʱ��
	TAPISTR_10					UpperNo;						///< ���ֺ�
	TAPISTR_20					UpperSettleNo;					///< ��Ա�ź��������
	TAPISTR_20					UpperUserNo;					///< ���ֵ�¼��
	TAPISTR_10					TradeNo;						///< ���ױ���
	TAPIREAL64					PositionPrice;					///< �ֲּ�
	TAPIUINT32					PositionQty;					///< �ֲ���
	TAPIYNFLAG					IsBackInput;					///< �Ƿ�Ϊ¼��ί�е�
	TAPIYNFLAG					IsAddOne;						///< �Ƿ�ΪT+1��
	TAPIUINT32					MatchStreamID;					///< �ɽ���ˮ��
    TAPIUINT32                  PositionStreamId;				///< �ֲ�����
    TAPIOpenCloseModeType       OpenCloseMode;                  ///< ��ƽ��ʽ
	TAPIREAL64					ContractSize;					///< ÿ�ֳ������������
	TAPISTR_10					CommodityCurrencyGroup;			///< Ʒ�ֱ�����
	TAPISTR_10					CommodityCurrency;				///< Ʒ�ֱ���
	TAPIREAL64					PreSettlePrice;					///< ������
	TAPIREAL64					SettlePrice;					///< ��ǰ����۸�
	TAPIREAL64					Turnover;						///< �ֲֽ��
	TAPICalculateModeType		AccountMarginMode;				///< ��֤����㷽ʽ
	TAPIREAL64					AccountMarginParam;				///< ��֤�����ֵ
	TAPICalculateModeType		UpperMarginMode;				///< ��֤����㷽ʽ
	TAPIREAL64					UpperMarginParam;				///< ��֤�����ֵ
	TAPIREAL64					AccountInitialMargin;			///< �ͻ���ʼ��֤��
	TAPIREAL64					AccountMaintenanceMargin;		///< �ͻ�ά�ֱ�֤��
	TAPIREAL64					UpperInitialMargin;				///< ���ֳ�ʼ��֤��
	TAPIREAL64					UpperMaintenanceMargin;			///< ����ά�ֱ�֤��
	TAPIREAL64					PositionProfit;					///< �ֲ�ӯ��
	TAPIREAL64					LMEPositionProfit;				///< LME�ֲ�ӯ��
	TAPIREAL64					OptionMarketValue;				///< ��Ȩ��ֵ
	TAPISTR_20					MatchCmbNo;						///< ��ϳֲֺ�
	TAPIYNFLAG					IsHistory;						///< �Ƿ���ʷ�ֲ�
	TAPIREAL64					FloatingPL;						///< ��ʸ�ӯ
};

//! �ͻ��ֲ�ӯ��
struct TapAPIPositionProfit
{
	TAPISTR_70					PositionNo;						///< ���سֲֺţ���������д
	TAPIUINT32					PositionStreamId;				///< �ֲ�����
	TAPIREAL64					PositionProfit;					///< �ֲ�ӯ��
	TAPIREAL64					LMEPositionProfit;				///< LME�ֲ�ӯ��
	TAPIREAL64					OptionMarketValue;				///< ��Ȩ��ֵ
	TAPIREAL64					CalculatePrice;					///< ����۸�
	TAPIREAL64					FloatingPL;						///< ��ʸ�ӯ
};

//! �ͻ��ֲ�ӯ��֪ͨ
struct TapAPIPositionProfitNotice
{
	TAPIYNFLAG					IsLast;							///< �Ƿ����һ��
	TapAPIPositionProfit*		Data;							///< �ͻ��ֲ�ӯ����Ϣ
};

//! ƽ����Ϣ
struct TapAPICloseInfo
{
	TAPISTR_20					AccountNo;						///< �ͻ��ʽ��ʺ�
	TAPISTR_20					ParentAccountNo;				///< �ϼ��ʽ��˺�
	TAPISTR_10					ExchangeNo;						///< ���������
	TAPICommodityType			CommodityType;					///< Ʒ������
	TAPISTR_10					CommodityNo;					///< Ʒ�ֱ�������
	TAPISTR_10					ContractNo;						///< ��Լ
	TAPISTR_10					StrikePrice;					///< ִ�м۸�
	TAPICallOrPutFlagType		CallOrPutFlag;					///< ���ſ���
	TAPISTR_10					OpenOrderExchangeNo;			///< ���������
	TAPICommodityType			OpenOrderCommodityType;			///< Ʒ������
	TAPISTR_10					OpenOrderCommodityNo;			///< Ʒ�ֱ�������
	TAPISTR_10					CloseOrderExchangeNo;			///< ���������
	TAPICommodityType			CloseOrderCommodityType;		///< Ʒ������
	TAPISTR_10					CloseOrderCommodityNo;			///< Ʒ�ֱ�������
	TAPIMatchSourceType			OpenMatchSource;
	TAPIMatchSourceType			CloseMatchSource;
	TAPISideType				CloseSide;						///< ƽ��һ�ߵ���������
	TAPIUINT32					CloseQty;						///< ƽ�ֳɽ���
	TAPIREAL64					OpenPrice;						///< ���ֳɽ���
	TAPIREAL64					ClosePrice;						///< ƽ�ֳɽ���
	TAPICHAR					OpenServerFlag;					///< ��������ʶ
	TAPISTR_20					OpenOrderNo;					///< ί�б���
	TAPISTR_20					OpenMatchNo;					///< ���سɽ���
	TAPISTR_70					OpenExchangeMatchNo;			///< �������ɽ���
	TAPIDATETIME				OpenMatchDateTime;				///< �ɽ�ʱ��
	TAPICHAR					CloseServerFlag;				///< ��������ʶ
	TAPISTR_20					CloseOrderNo;					///< ί�б���
	TAPISTR_20					CloseMatchNo;					///< ���سɽ���
	TAPISTR_70					CloseExchangeMatchNo;			///< �������ɽ���
	TAPIDATETIME				CloseMatchDateTime;				///< �ɽ�ʱ��
    TAPIUINT32                  CloseStreamId;					///< ƽ������
    TAPIOpenCloseModeType		OpenCloseMode;                  ///< ��ƽ��ʽ
	TAPIREAL64					ContractSize;					///< ÿ�ֳ������������
	TAPISTR_10					CommodityCurrencyGroup;			///< Ʒ�ֱ�����
	TAPISTR_10					CommodityCurrency;				///< Ʒ�ֱ���
	TAPIREAL64					PreSettlePrice;					///< ������
	TAPIREAL64					PremiumIncome;					///< Ȩ������ȡ
	TAPIREAL64					PremiumPay;						///< Ȩ����֧��
	TAPIREAL64					CloseProfit;					///< ƽ��ӯ��
	TAPIREAL64					UnExpProfit;					///< δ����ƽӯ
	TAPIREAL64					ClosePL;                        ///< ���ƽӯ
};
//! �ʽ��ѯ����
struct TapAPIFundReq
{
	TAPISTR_20					AccountNo;						///< �ͻ��ʽ��ʺ�
};
//! �ʽ��˺��ʽ���Ϣ
struct TapAPIFundData
{
	TAPISTR_20					AccountNo;						///< �ͻ��ʽ��˺�
	TAPISTR_20					ParentAccountNo;				///< �ϼ��ʽ��˺�
	TAPISTR_10					CurrencyGroupNo;				///< �������
	TAPISTR_10					CurrencyNo;						///< ���ֺ�(Ϊ�ձ�ʾ����������ʽ�)
	TAPIREAL64					TradeRate;						///< ���׻���
	TAPIFutureAlgType			FutureAlg;                      ///< �ڻ��㷨
	TAPIOptionAlgType			OptionAlg;                      ///< ��Ȩ�㷨
	TAPIREAL64					PreBalance;						///< ���ս��
	TAPIREAL64					PreUnExpProfit;					///< ����δ����ƽӯ
	TAPIREAL64					PreLMEPositionProfit;			///< ����LME�ֲ�ƽӯ
	TAPIREAL64					PreEquity;						///< ����Ȩ��
	TAPIREAL64					PreAvailable1;					///< ���տ���
	TAPIREAL64					PreMarketEquity;				///< ������ֵȨ��
	TAPIREAL64					CashInValue;					///< ���
	TAPIREAL64					CashOutValue;					///< ����
	TAPIREAL64					CashAdjustValue;				///< �ʽ����
	TAPIREAL64					CashPledged;					///< ��Ѻ�ʽ�
	TAPIREAL64					FrozenFee;						///< ����������
	TAPIREAL64					FrozenDeposit;					///< ���ᱣ֤��
	TAPIREAL64					AccountFee;						///< �ͻ������Ѱ�������������
	TAPIREAL64					ExchangeFee;					///< ���������
	TAPIREAL64					AccountDeliveryFee;				///< �ͻ�����������
	TAPIREAL64					PremiumIncome;					///< Ȩ������ȡ
	TAPIREAL64					PremiumPay;						///< Ȩ����֧��
	TAPIREAL64					CloseProfit;					///< ƽ��ӯ��
	TAPIREAL64					DeliveryProfit;					///< ����ӯ��
	TAPIREAL64					UnExpProfit;					///< δ����ƽӯ
	TAPIREAL64					ExpProfit;						///< ����ƽ��ӯ��
	TAPIREAL64					PositionProfit;					///< ����LME�ֲ�ӯ�� 
	TAPIREAL64					LmePositionProfit;				///< LME�ֲ�ӯ��
	TAPIREAL64					OptionMarketValue;				///< ��Ȩ��ֵ
	TAPIREAL64					AccountIntialMargin;			///< �ͻ���ʼ��֤��
	TAPIREAL64					AccountMaintenanceMargin;		///< �ͻ�ά�ֱ�֤��
	TAPIREAL64					UpperInitalMargin;				///< ���ֳ�ʼ��֤��
	TAPIREAL64					UpperMaintenanceMargin;			///< ����ά�ֱ�֤��
	TAPIREAL64					Discount;						///< LME����
	TAPIREAL64					Balance;						///< ���ս��
	TAPIREAL64					Equity;							///< ����Ȩ��
	TAPIREAL64					Available;						///< ���տ���
	TAPIREAL64					CanDraw;						///< ����ȡ
	TAPIREAL64					MarketEquity;					///< �˻���ֵ
	TAPIREAL64					OriginalCashInOut;				///< ����ԭʼ�����
	TAPIREAL64					FloatingPL;						///< ��ʸ�ӯ
	TAPIREAL64					FrozenRiskFundValue;			///< ���ն����ʽ�
	TAPIREAL64					ClosePL;						///< ���ƽӯ
	TAPIREAL64					NoCurrencyPledgeValue;          ///< �ǻ�����Ѻ
	TAPIREAL64					PrePledgeValue;                 ///< �ڳ���Ѻ
	TAPIREAL64					PledgeIn;                       ///< ����
	TAPIREAL64					PledgeOut;                      ///< �ʳ�
	TAPIREAL64					PledgeValue;                    ///< ��Ѻ���
	TAPIREAL64					BorrowValue;                    ///< ���ý��
	TAPIREAL64					SpecialAccountFrozenMargin;     ///< �����Ʒ���ᱣ֤��
	TAPIREAL64					SpecialAccountMargin;           ///< �����Ʒ��֤��   
	TAPIREAL64					SpecialAccountFrozenFee;        ///< �����Ʒ����������
	TAPIREAL64					SpecialAccountFee;              ///< �����Ʒ������
	TAPIREAL64					SpecialFloatProfit;             ///< �����Ʒ��ӯ
	TAPIREAL64					SpecialCloseProfit;             ///< �����Ʒƽӯ
	TAPIREAL64					SpecialFloatPL;                 ///< �����Ʒ��ʸ�ӯ
	TAPIREAL64					SpecialClosePL;                 ///< �����Ʒ���ƽӯ
};

//! ����Ʒ����Ϣ
struct TapAPICommodityInfo
{
	TAPISTR_10				ExchangeNo;						///< ����������
	TAPICommodityType		CommodityType;					///< Ʒ������
	TAPISTR_10				CommodityNo;					///< Ʒ�ֱ��
	TAPISTR_20				CommodityName;					///< Ʒ������
	TAPISTR_30				CommodityEngName;				///< Ʒ��Ӣ������
	TAPISTR_10				RelateExchangeNo;				///< ����Ʒ�ֽ���������
	TAPICommodityType		RelateCommodityType;			///< ����Ʒ��Ʒ������
	TAPISTR_10				RelateCommodityNo;				///< ����Ʒ��Ʒ�ֱ��
	TAPISTR_10				RelateExchangeNo2;				///< ����Ʒ��2����������
	TAPICommodityType		RelateCommodityType2;			///< ����Ʒ��2Ʒ������
	TAPISTR_10				RelateCommodityNo2;				///< ����Ʒ��2Ʒ�ֱ��
	TAPISTR_10				TradeCurrency;					///< ���ױ���
	TAPISTR_10				SettleCurrency;					///< �������
	TAPIREAL64				ContractSize;					///< ÿ�ֳ���
	TAPIMarginCalculateModeType			MarginCalculateMode;			///< �ڻ���֤��ʽ,�ֱ�'1',����'2'
	TAPIOptionMarginCalculateModeType	OptionMarginCalculateMode;		///< ��Ȩ��֤��ʽ
	TAPIOpenCloseModeType				OpenCloseMode;					///< ��ƽ��ʽ
	TAPIREAL64				StrikePriceTimes;				///< ִ�м۸���
	TAPIYNFLAG				IsOnlyQuoteCommodity;			///< ������Ʒ��
	TAPIREAL64				CommodityTickSize;				///< ��С�䶯��λ
	TAPIINT32				CommodityDenominator;			///< ���۷�ĸ
	TAPICmbDirectType		CmbDirect;						///< ��Ϸ���
	TAPIINT32				OnlyCanCloseDays;				///< ֻ��ƽ����ǰ����
	TAPIDeliveryModeType	DeliveryMode;					///< ������Ȩ��ʽ
	TAPIINT32				DeliveryDays;					///< ������ƫ��
	TAPITIME				AddOneTime;						///< T+1�ָ�ʱ��
	TAPIINT32				CommodityTimeZone;				///< Ʒ��ʱ��
};

//! ���׺�Լ��Ϣ
struct TapAPITradeContractInfo
{
	TAPISTR_10              ExchangeNo;                             ///< ����������
	TAPICommodityType		CommodityType;                          ///< Ʒ������
	TAPISTR_10              CommodityNo;                            ///< Ʒ�ֱ��
	TAPISTR_10              ContractNo1;                            ///< ��Լ����1
	TAPISTR_10              StrikePrice1;                           ///< ִ�м�1
	TAPICallOrPutFlagType	CallOrPutFlag1;                         ///< ���ǿ�����ʾ1
	TAPISTR_10              ContractNo2;                            ///< ��Լ����2
	TAPISTR_10              StrikePrice2;                           ///< ִ�м�2
	TAPICallOrPutFlagType	CallOrPutFlag2;                         ///< ���ǿ�����ʾ2
	TAPIContractTypeType	ContractType;                           ///< ��Լ����
	TAPISTR_10				QuoteUnderlyingContract;				///< ������ʵ��Լ
	TAPISTR_70              ContractName;                           ///< ��Լ����
	TAPIDATE                ContractExpDate;                        ///< ��Լ������	
	TAPIDATE                LastTradeDate;                          ///< �������
	TAPIDATE                FirstNoticeDate;                        ///< �״�֪ͨ��
	TAPISTR_10				FutureContractNo;						///< �ڻ���Լ���(��Ȩ���)
};


//! ���������Ϣ
struct TapAPIDeepQuoteInfo
{
	TAPISideType				Side;							///< ��������
	TAPIREAL64					Price;							///< �ҵ��۸�
	TAPIUINT32					Qty;							///< �ҵ�����
};

//! ��������ѯӦ��
struct TapAPIDeepQuoteQryRsp
{
	TapAPIContract			Contract;							///< ��Լ
	TapAPIDeepQuoteInfo		DeepQuote;							///< �������
};

//! ������ʱ��״̬��Ϣ��ѯ����ṹ
struct TapAPIExchangeStateInfoQryReq
{
};

//! ������ʱ��״̬��Ϣ
struct TapAPIExchangeStateInfo
{
	TAPISTR_10				UpperChannelNo;			///< ����ͨ�����
	TAPISTR_10				ExchangeNo;				///< ���������
	TAPICommodityType		CommodityType;			///< Ʒ������
	TAPISTR_10				CommodityNo;			///< Ʒ�ֱ��
	TAPIDATETIME			ExchangeTime;			///< ������ʱ��
	TAPITradingStateType	TradingState;			///< ������״̬
};

//! ������ʱ��״̬��Ϣ֪ͨ�ṹ
struct TapAPIExchangeStateInfoNotice
{
	TAPIYNFLAG				IsLast;					///< �Ƿ����һ������
	TapAPIExchangeStateInfo ExchangeStateInfo;		///< ������ʱ��״̬��Ϣ
};

//! ѯ��֪ͨ�ṹ
struct TapAPIReqQuoteNotice
{
	TAPISTR_10					ExchangeNo;						///< ���������
	TAPICommodityType			CommodityType;					///< Ʒ������
	TAPISTR_10					CommodityNo;					///< Ʒ�ֱ�������
	TAPISTR_10					ContractNo;						///< ��Լ
	TAPISTR_10					StrikePrice;					///< ִ�м۸�
	TAPICallOrPutFlagType		CallOrPutFlag;					///< ���ſ���
	TAPISTR_50					InquiryNo;						///< ѯ�ۺ�
	TAPIDATETIME				UpdateTime;						///< ����ʱ��                   
};

//! ����ͨ����Ϣ��ѯ����ṹ
struct TapAPIUpperChannelQryReq
{
};

//! ����ͨ����Ϣ�ṹ
struct TapAPIUpperChannelInfo
{
	TAPISTR_10				UpperChannelNo;			///< ����ͨ�����
	TAPISTR_20				UpperChannelName;		///< ����ͨ������
	TAPISTR_10				UpperNo;				///< ����ͨ����
	TAPISTR_20				UpperUserNo;			///< ����ϯλ��
};
//! �ͻ����շ��ʲ�ѯ����ṹ
struct TapAPIAccountRentQryReq
{
    TAPISTR_20                              AccountNo;													//�ʽ��˻�
    TAPISTR_10                              ExchangeNo;													//����������
	TAPICommodityType			            CommodityType;												//Ʒ������
	TAPISTR_10                              CommodityNo;												//Ʒ�ֱ��
	TAPISTR_10                              ContractNo;													//��Լ���� 
};
//! �ͻ����շ�����Ϣ�ṹ
struct	TapAPIAccountRentInfo
{
	TAPISTR_20                              AccountNo;													//�ʽ��˻�
	TAPISTR_10                              ExchangeNo;													//����������
	TAPICommodityType			            CommodityType;												//Ʒ������
	TAPISTR_10                              CommodityNo;												//Ʒ�ֱ��
	TAPISTR_10                              ContractNo;													//��Լ���� 
	TAPIMatchSourceType			            MatchSource;												//�ɽ���Դ  
    TAPISTR_10				                CurrencyNo;													//���ֱ��
	
	TAPICalculateModeType			        FeeMode;                                                    //�����Ѽ��㷽ʽ 
	TAPIREAL64				                OpenTurnover;												//���������Ѱ����
	TAPIREAL64				                ClosedTurnover;												//ƽ�������Ѱ����
	TAPIREAL64				                CloseNewTurnover;											//ƽ�������Ѱ����
	TAPIREAL64				                OpenVolume;													//���������Ѱ�����
	TAPIREAL64				                ClosedVolume;												//ƽ�������Ѱ�����	
    TAPIREAL64				                CloseNewVolume;												//ƽ�������Ѱ�����
	
    TAPICalculateModeType                   MarginMode;                                                 //��֤����㷽ʽ   
	TAPIREAL64				                BuyTInitMargin;												//��Ͷ����ʼ��֤��
	TAPIREAL64				                SellTInitMargin;											//��Ͷ����ʼ��֤��
	TAPIREAL64				                BuyBInitMargin;												//��ֵ��ʼ��֤��
	TAPIREAL64				                SellBInitMargin;											//����ֵ��ʼ��֤��
	TAPIREAL64				                BuyLInitMargin;												//��������ʼ��֤��
	TAPIREAL64				                SellLInitMargin;											//��������ʼ��֤��
	TAPIREAL64				                BuyMInitMargin;												//�������̳�ʼ��֤��
	TAPIREAL64				                SellMInitMargin;											//�������̳�ʼ��֤��
	TAPIREAL64				                BuyTMaintMargin;											//��Ͷ��ά�ֱ�֤��
	TAPIREAL64				                SellTMaintMargin;											//��Ͷ��ά�ֱ�֤��
	TAPIREAL64				                BuyBMaintMargin;											//��ֵά�ֱ�֤��
	TAPIREAL64				                SellBMaintMargin;											//����ֵά�ֱ�֤��
	TAPIREAL64				                BuyLMaintMargin;											//������ά�ֱ�֤��
	TAPIREAL64				                SellLMaintMargin;											//������ά�ֱ�֤��
	TAPIREAL64				                BuyMMaintMargin;											//��������ά�ֱ�֤��
	TAPIREAL64				                SellMMaintMargin;											//��������ά�ֱ�֤��

};

#pragma pack(pop)
#endif //TAP_TRADE_API_DATA_TYPE_H