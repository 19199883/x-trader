#ifndef TAP_API_ERROR_H
#define TAP_API_ERROR_H

//=============================================================================
/**
 *	\addtogroup G_ERR_INNER_API		TapAPI�ڲ����صĴ����붨�塣
 *	@{
 */
//=============================================================================
//! �ɹ�
const int TAPIERROR_SUCCEED                                            = 0;
//! ���ӷ���ʧ��
const int TAPIERROR_ConnectFail                                        = -1;
//! ��·��֤ʧ��
const int TAPIERROR_LinkAuthFail                                       = -2;
//! ������ַ������
const int TAPIERROR_HostUnavailable                                    = -3;
//! �������ݴ���
const int TAPIERROR_SendDataError                                      = -4;
//! ���Ա�Ų��Ϸ�
const int TAPIERROR_TestIDError                                        = -5;
//! û׼���ò�������
const int TAPIERROR_NotReadyTestNetwork                                = -6;
//! ��ǰ������Ի�û����
const int TAPIERROR_CurTestNotOver                                     = -7;
//! û�ÿ��õĽ���ǰ��
const int TAPIERROR_NOFrontAvailable                                   = -8;
//! ����·��������
const int TAPIERROR_DataPathAvaiable                                   = -9;
//! �ظ���¼
const int TAPIERROR_RepeatLogin                                        = -10;
//! �ڲ�����	
const int TAPIERROR_InnerError                                         = -11;
//! ��һ������û�н���	
const int TAPIERROR_LastReqNotFinish                                   = -12;
//! ��������Ƿ�	
const int TAPIERROR_InputValueError                                    = -13;
//! ��Ȩ�벻�Ϸ�	
const int TAPIERROR_AuthCode_Invalid                                   = -14;
//! ��Ȩ�볬��	
const int TAPIERROR_AuthCode_Expired                                   = -15;
//! ��Ȩ�����Ͳ�ƥ��	
const int TAPIERROR_AuthCode_TypeNotMatch                              = -16;
//! API��û��׼����
const int TAPIERROR_API_NotReady                                       = -17;
//! UDP�˿ڼ���ʧ��
const int TAPIERROR_UDP_LISTEN_FAILED                                  = -18;
//! UDP���ڼ���
const int TAPIERROR_UDP_LISTENING                                      = -19;
//! �ӿ�δʵ��
const int TAPIERROR_NotImplemented                                     = -20;
//! ÿ�ε�½ֻ�������һ��
const int TAPIERROR_CallOneTimeOnly                                    = -21;
//! ��ѯƵ��̫��
const int TAPIERROR_Frequently                                         = -22;

/** @}*/


//=============================================================================
/**
 *	\addtogroup G_ERR_INPUT_CHECK		�������������
 *	@{
 */
//=============================================================================
//! ��������ΪNULL
const int TAPIERROR_INPUTERROR_NULL                                    = -10000;
//! ��������:TAPIYNFLAG
const int TAPIERROR_INPUTERROR_TAPIYNFLAG                              = -10001;
//! ��������:TAPILOGLEVEL
const int TAPIERROR_INPUTERROR_TAPILOGLEVEL                            = -10002;
//! ��������:TAPICommodityType
const int TAPIERROR_INPUTERROR_TAPICommodityType                       = -10003;
//! ��������:TAPICallOrPutFlagType
const int TAPIERROR_INPUTERROR_TAPICallOrPutFlagType                   = -10004;
//! ��������:TAPIAccountType
const int TAPIERROR_INPUTERROR_TAPIAccountType                         = -12001;
//! ��������:TAPIAccountState
const int TAPIERROR_INPUTERROR_TAPIAccountState                        = -12003;
//! ��������:TAPIAccountFamilyType
const int TAPIERROR_INPUTERROR_TAPIAccountFamilyType                   = -12004;
//! ��������:TAPIOrderTypeType
const int TAPIERROR_INPUTERROR_TAPIOrderTypeType                       = -12005;
//! ��������:TAPIOrderSourceType
const int TAPIERROR_INPUTERROR_TAPIOrderSourceType                     = -12006;
//! ��������:TAPITimeInForceType
const int TAPIERROR_INPUTERROR_TAPITimeInForceType                     = -12007;
//! ��������:TAPISideType
const int TAPIERROR_INPUTERROR_TAPISideType                            = -12008;
//! ��������:TAPIPositionEffectType
const int TAPIERROR_INPUTERROR_TAPIPositionEffectType                  = -12009;
//! ��������:TAPIHedgeFlagType
const int TAPIERROR_INPUTERROR_TAPIHedgeFlagType                       = -12010;
//! ��������:TAPIOrderStateType
const int TAPIERROR_INPUTERROR_TAPIOrderStateType                      = -12011;
//! ��������:TAPICalculateModeType
const int TAPIERROR_INPUTERROR_TAPICalculateModeType                   = -12012;
//! ��������:TAPIMatchSourceType
const int TAPIERROR_INPUTERROR_TAPIMatchSourceType                     = -12013;
//! ��������:TAPIOpenCloseModeType
const int TAPIERROR_INPUTERROR_TAPIOpenCloseModeType                   = -12014;
//! ��������:TAPIFutureAlgType
const int TAPIERROR_INPUTERROR_TAPIFutureAlgType                       = -12015;
//! ��������:TAPIOptionAlgType
const int TAPIERROR_INPUTERROR_TAPIOptionAlgType                       = -12016;
//! ��������:TAPIBankAccountLWFlagType
const int TAPIERROR_INPUTERROR_TAPIBankAccountLWFlagType               = -12017;
//! ��������:TAPIMarginCalculateModeType
const int TAPIERROR_INPUTERROR_TAPIMarginCalculateModeType             = -12021;
//! ��������:TAPIOptionMarginCalculateModeType
const int TAPIERROR_INPUTERROR_TAPIOptionMarginCalculateModeType       = -12022;
//! ��������:TAPICmbDirectType
const int TAPIERROR_INPUTERROR_TAPICmbDirectType                       = -12023;
//! ��������:TAPIDeliveryModeType
const int TAPIERROR_INPUTERROR_TAPIDeliveryModeType                    = -12024;
//! ��������:TAPIContractTypeType
const int TAPIERROR_INPUTERROR_TAPIContractTypeType                    = -12025;
//! ��������:TAPITacticsTypeType
const int TAPIERROR_INPUTERROR_TAPITacticsTypeType                     = -12035;
//! ��������:TAPIORDERACT
const int TAPIERROR_INPUTERROR_TAPIORDERACT                            = -12036;
//! ��������:TAPITriggerConditionType
const int TAPIERROR_INPUTERROR_TAPITriggerConditionType                = -12041;
//! ��������:TAPITriggerPriceTypeType
const int TAPIERROR_INPUTERROR_TAPITriggerPriceTypeType                = -12042;
//! ��������:TAPITradingStateType 
const int TAPIERROR_INPUTERROR_TAPITradingStateType                    = -12043;
//! ��������:TAPIMarketLevelType 
const int TAPIERROR_INPUTERROR_TAPIMarketLevelType                     = -12044;
//! ��������:TAPIOrderQryTypeType 
const int TAPIERROR_INPUTERROR_TAPIOrderQryTypeType                    = -12045;

/** @}*/

//=============================================================================
/**
 *	\addtogroup G_ERR_DISCONNECT_REASON	����Ͽ�������붨��
 *	@{
 */
//=============================================================================
//! �����Ͽ�
const int TAPIERROR_DISCONNECT_CLOSE_INIT                              = 1;
//! �����Ͽ�
const int TAPIERROR_DISCONNECT_CLOSE_PASS                              = 2;
//! ������
const int TAPIERROR_DISCONNECT_READ_ERROR                              = 3;
//! д����
const int TAPIERROR_DISCONNECT_WRITE_ERROR                             = 4;
//! ��������
const int TAPIERROR_DISCONNECT_BUF_FULL                                = 5;
//! �첽��������
const int TAPIERROR_DISCONNECT_IOCP_ERROR                              = 6;
//! �������ݴ���
const int TAPIERROR_DISCONNECT_PARSE_ERROR                             = 7;
//! ���ӳ�ʱ
const int TAPIERROR_DISCONNECT_CONNECT_TIMEOUT                         = 8;
//! ��ʼ��ʧ��
const int TAPIERROR_DISCONNECT_INIT_ERROR                              = 9;
//! �Ѿ�����
const int TAPIERROR_DISCONNECT_HAS_CONNECTED                           = 10;
//! �����߳��ѽ���
const int TAPIERROR_DISCONNECT_HAS_EXIT                                = 11;
//! �������ڽ��У����Ժ�����
const int TAPIERROR_DISCONNECT_TRY_LATER                               = 12;
//! �������ʧ��
const int TAPIERROR_DISCONNECT_HEARTBEAT_FAILED                        = 13;

/** @}*/

//=============================================================================
/**
 *	\addtogroup G_ERR_LOGIN	��½���̷��صĴ�����붨��
 *	@{
 */
//=============================================================================
//! ��¼����ִ�д���
const int TAPIERROR_LOGIN                                              = 10001;
//! ��¼�û�������
const int TAPIERROR_LOGIN_USER                                         = 10002;
//! ��Ҫ���ж�̬��֤
const int TAPIERROR_LOGIN_DDA                                          = 10003;
//! ��¼�û�δ��Ȩ
const int TAPIERROR_LOGIN_LICENSE                                      = 10004;
//! ��¼ģ�鲻��ȷ
const int TAPIERROR_LOGIN_MODULE                                       = 10005;
//! ��Ҫǿ���޸�����
const int TAPIERROR_LOGIN_FORCE                                        = 10006;
//! ��¼״̬��ֹ��½
const int TAPIERROR_LOGIN_STATE                                        = 10007;
//! ��¼���벻��ȷ
const int TAPIERROR_LOGIN_PASS                                         = 10008;
//! û�и�ģ���¼Ȩ��
const int TAPIERROR_LOGIN_RIGHT                                        = 10009;
//! ��¼��������
const int TAPIERROR_LOGIN_COUNT                                        = 10010;
//! ��¼�û����ڷ�������ʶ�¿ɵ�¼�û��б���
const int TAPIERROR_LOGIN_NOTIN_SERVERFLAGUSRES                        = 10011;
//! ��½�û�������Ч��
const int TAPIERROR_LOGIN_USER_EXPIRED                                 = 10012;
//! ��½�û��������ͻ�
const int TAPIERROR_LOGIN_NO_ACCOUNT                                   = 10013;

/** @}*/

//=============================================================================
/**
 *	\addtogroup G_ERR_MANAGE ����ҵ�����ش�����	
 *	@{
 */
//==============================================================================
//! ��¼�û���Ϣ��ѯʧ��
const int TAPIERROR_USERINFO_QRY                                       = 10101;
//! ��¼�û���������Ȩ�޲�ѯʧ��
const int TAPIERROR_USERALLRIGHT_QRY                                   = 11001;
//! ��¼�û����������ʽ��˺Ų�ѯʧ��
const int TAPIERROR_USERALLACCOUNT_QRY                                 = 11501;
//! ��¼�û������޸�ʧ��
const int TAPIERROR_USERPASSWORD_MOD                                   = 11701;
//! ��¼�û������޸�ʧ�ܡ���ԭʼ�������
const int TAPIERROR_USERPASSWORD_MOD_SOURCE                            = 11702;
//! ��¼�û������޸�ʧ�ܡ���������ǰn��������ͬ
const int TAPIERROR_USERPASSWORD_MOD_SAME                              = 11703;
//! �����벻�������븴�Ӷ�Ҫ��
const int TAPIERROR_USERPASSWORD_MOD_COMPLEXITY                        = 11704;
//! �ʽ��˺���Ϣ��ѯʧ��
const int TAPIERROR_ACCOUNTINFO_QRY                                    = 20201;
//! �ͻ����ױ����ѯʧ��
const int TAPIERROR_TRADENO_QRY                                        = 20701;
//! ��Լ��Ϣ��ѯʧ��
const int TAPIERROR_CONTRACTINFO_QRY                                   = 22801;
//! ������Ȩ��Ĳ�ѯʧ��
const int TAPIERROR_SPECIALOPTIONFUTURE_QRY                            = 22901;
//! Ʒ��ί�����Ͳ�ѯʧ��
const int TAPIERROR_COMMODITYORDERTYPE_QRY                             = 25501;
//! Ʒ��ί��ʱ����Ч�Բ�ѯʧ��
const int TAPIERROR_ORDERTIMEINFORCE_QRY                               = 25601;
//! �û��µ�Ƶ�ʲ�ѯʧ��
const int TAPIERROR_USER_ORDER_FREQUENCE_QRY                           = 28901;

/** @}*/

//=============================================================================
/**
 *	\addtogroup G_ERR_TRADE ����ҵ�����ش�����	
 *	@{
 */
//==============================================================================
//! �ʽ��˺Ų�����
const int TAPIERROR_ORDERINSERT_ACCOUNT                                = 60001;
//! �ʽ��˺�״̬����ȷ
const int TAPIERROR_ORDERINSERT_ACCOUNT_STATE		                   = 60002;
//! �µ���Ч�ĺ�Լ
const int TAPIERROR_ORDERINSERT_CONTRACT                               = 60011;
//! �µ���Լ�޽���·��
const int TAPIERROR_ORDERINSERT_TRADEROUTE                             = 60021;
//! �ֲ��������������
const int TAPIERROR_ORDERINSERT_POSITIONMAX                            = 60022;
//! ��ֹ����
const int TAPIERROR_ORDER_NOTRADE                                      = 60023;
//! ֻ��ƽ��
const int TAPIERROR_ORDER_CLOSE                                        = 60024;
//! �µ��ʽ���
const int TAPIERROR_ORDERINSERT_NOTENOUGHFUND                          = 60031;
//! ��֧�ֵĶ�������
const int TAPIERROR_ORDERINSERT_ORDERTYPE                              = 60032;
//! ��֧�ֵ�ʱ����Ч����
const int TAPIERROR_ORDERINSERT_TIMEINFORCE                            = 60033;
//! ��֧�ֵĲ��Ե�����
const int TAPIERROR_ORDERINSERT_NO_TACTICS                             = 60034;
//! ƽ�������������гֲ���
const int TAPIERROR_ORDERINSERT_POSITION_CANNOT_CLOSE                  = 60035;
//! �µ��Զ����ʧ��
const int TAPIERROR_ORDERINSERT_AUTOCHECK_FAIL                         = 60036;
//! LMEδ׼������
const int TAPIERROR_ORDERINSERT_LME_NOTREADY                           = 60037;
//! ƽ�ַ�ʽ����
const int TAPIERROR_ORDERINSERT_CLOSEMODE                              = 60038;
//! �µ���Ӧ�ĸ��˺��ʽ���
const int TAPIERROR_ORDERINSERT_PARENTNOTENOUGHFUND                    = 60039;
//! �������ĺ�Լ��ʽ����
const int TAPIERROR_SWAP_CONTRACT                                      = 60040;
//! ��ǰ�ͻ�����ʹ�ô��˺Ž���
const int TAPIERROR_USERNO_NOTHAS_ACCOUNT                              = 60051;
//! ����ͨ��״̬������
const int TAPIERROR_UPPERCHANNEL_BROKEN                                = 60052;
//! ����ͨ��δ��ͨEXIST
const int TAPIERROR_UPPERCHANNEL_NOT_EXIST                             = 60053;
//! �����޴�ϵͳ��
const int TAPIERROR_ORDERDELETE_NOT_SYSNO                              = 60061;
//! ��״̬��������
const int TAPIERROR_ORDERDELETE_NOT_STATE                              = 60062;
//! ��״̬��������
const int TAPIERROR_ORDERACTIVE_NOT_STATE                              = 60063;
//! ��״̬��ֹ���
const int TAPIERROR_ORDERCHECK_NOT_STATE                               = 60071;
//! �������ʧ��
const int TAPIERROR_ORDERCHECK_FAIL                                    = 60072;
//! ��״̬������ĵ�
const int TAPIERROR_ORDERMODIFY_NOT_STATE                              = 60081;
//! �˹���������ĵ�
const int TAPIERROR_ORDERMODIFY_BACK_INPUT                             = 60082;
//! �����Ѳ�������
const int TAPIERROR_ORDERINSERT_FEE                                    = 60091;
//! ��֤���������
const int TAPIERROR_ORDERINSERT_MARGIN                                 = 60092;
//! �����˺�ֻ�ɲ�ѯ
const int TAPIERROR_ORDER_NO_PERMIT                                    = 60100;
//! �������̲���Ӧ��
const int TAPIERROR_RSPQUOTE_NO_PERMIT                                 = 60101;
//! ��ʹ���ʺ����������̲���Ӧ��
const int TAPIERROR_RSPQUOTE_CHILD_NO_PERMIT                           = 60102;
//! �µ��Ҳ������ױ���
const int TAPIERROR_TRADENO_NOT_FIND                                   = 60103;
//! �����˺�ֻ�ɿ���
const int TAPIERROR_ORDER_NO_CLOSE                                     = 60104;
//! �����˺�û�����ڹҵ���ѯȨ��
const int TAPIERROR_QRY_QUOTE_NO_PERMIT                                = 60105;
//! ������Ч������С�ڵ�ǰ������
const int TAPIERROR_EXPIREDATE_NO_PERMIT                               = 60106;
//! �ñ��벻��������������
const int TAPIERROR_CMB_NO_PERMIT                                      = 60107;
//! �Ǳ�����������µ��˺Ų��������
const int TAPIERROR_ORDERSERVER_NO_PERMIT                              = 60108;
//! ��Ȩ����Ȩ������������
const int TAPIERROR_POSITION_CANNOT_EXEC_OR_ABANDON                    = 60109;
//! û�ж������Ȩ��
const int TAPIERROR_ORDERCHECK_NO_PERMIT                               = 60110;
//! ����������󿪲���
const int TAPIERROR_ORDERMAXOPENVOL_NO_PERMIT                          = 60111;
//! �Ǵ���Ӧ�۵�����������ί������һ��
const int TAPIERROR_ORDER_QTY_NO_PERMIT                                = 60115;
//! ��������Ƶ�ʹ���
const int TAPIERROR_ORDER_FREQUENCY                                    = 61001;
//! ί�в�ѯ����ǰ���ܽ����´β�ѯ
const int TAPIERROR_ORDER_QUERYING                                     = 61002;

/** @}*/

//=============================================================================
/**
 *	\addtogroup G_ERR_QUOTE ����ҵ�����ش�����	
 *	@{
 */
//=============================================================================
//! ������������ܶ�����
const int TAPIERROR_SUBSCRIBEQUOTE_MAX                                 = 72001;
//! �����ý����������������
const int TAPIERROR_SUBSCRIBEQUOTE_EXCHANGE_MAX                        = 72002;
//! û�и�����Ķ���Ȩ��
const int TAPIERROR_SUBSCRIBEQUOTE_NO_RIGHT                            = 72101;
//! û�иý�����������Ķ���Ȩ��
const int TAPIERROR_SUBSCRIBEQUOTE_NO_EXCHANGE_RIGHT                   = 72102;
//! Ʒ�ֲ�����
const int TAPIERROR_SUBSCRIBEQUOTE_COMMODITY_NOT_EXIST                 = 72103;
//! ��Լ���ܲ�����
const int TAPIERROR_SUBSCRIBEQUOTE_CONTRACT_MAY_NOT_EXIST              = 72104;
//! ��֧�ֵ�����Э��
const int TAPIERROR_QUOTEFRONT_UNKNOWN_PROTOCOL                        = 83001;

/** @}*/


#endif //! TAP_API_ERROR_H
