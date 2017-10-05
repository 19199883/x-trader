#ifndef TAP_TRADE_API_H
#define TAP_TRADE_API_H

#include "TapTradeAPIDataType.h"
//#include "TapProtocol.h"

#ifdef _WIN32
#define TAP_CDECL __cdecl
#define TAP_DLLEXPORT __declspec(dllexport)
#else
#define TAP_CDECL
#define TAP_DLLEXPORT
#endif

//TapTradeAPI.h
//�ļ�������TapTradeAPI�ṩ�������ߵĶ���ӿڡ������ͻص��ӿڡ�

//TapTradeAPI �Ļص�֪ͨ�ӿڡ�
class ITapTradeAPINotify
{
public:
	/**
	* @brief ���ӳɹ��ص�֪ͨ
	* @ingroup G_T_Login
	*/
	virtual void TAP_CDECL OnConnect() = 0;
	/**
	* @brief	ϵͳ��¼���̻ص���
	* @details	�˺���ΪLogin()��¼�����Ļص�������Login()�ɹ���������·���ӣ�Ȼ��API������������͵�¼��֤��Ϣ��
	*			��¼�ڼ�����ݷ�������͵�¼�Ļ�����Ϣ���ݵ��˻ص������С�
	* @param[in] errorCode ���ش�����,0��ʾ�ɹ���
	* @param[in] loginRspInfo ��½Ӧ����Ϣ�����errorCode!=0����loginRspInfo=NULL��
	* @attention	�ûص����سɹ���˵���û���¼�ɹ������ǲ�����API׼����ϡ�
	* @ingroup G_T_Login
	*/
	virtual void TAP_CDECL OnRspLogin(TAPIINT32 errorCode, const TapAPITradeLoginRspInfo *loginRspInfo) = 0;
	/**
	* @brief	֪ͨ�û�API׼��������
	* @details	ֻ���û��ص��յ��˾���֪ͨʱ���ܽ��к����ĸ����������ݲ�ѯ������\n
	*			�˻ص�������API�ܷ����������ı�־��
	* @attention ������ſ��Խ��к�����������
	* @ingroup G_T_Login
	*/
	virtual void TAP_CDECL OnAPIReady() = 0;
	/**
	* @brief	API�ͷ���ʧȥ���ӵĻص�
	* @details	��APIʹ�ù������������߱��������������ʧȥ���Ӻ󶼻ᴥ���˻ص�֪ͨ�û���������������Ѿ��Ͽ���
	* @param[in] reasonCode �Ͽ�ԭ����롣
	* @ingroup G_T_Disconnect
	*/
	virtual void TAP_CDECL OnDisconnect(TAPIINT32 reasonCode) = 0;
	/**
	* @brief ֪ͨ�û������޸Ľ��
	* @param[in] sessionID �޸�����ĻỰID,��ChangePassword���صĻỰID��Ӧ��
	* @param[in] errorCode ���ش����룬0��ʾ�ɹ���
	* @ingroup G_T_UserInfo
	*/
	virtual void TAP_CDECL OnRspChangePassword(TAPIUINT32 sessionID, TAPIINT32 errorCode) = 0;
	/**
	* @brief �����û�Ԥ����Ϣ����
	* @param[in] sessionID �����û�Ԥ����Ϣ�ĻỰID
	* @param[in] errorCode ���ش����룬0��ʾ�ɹ���
	* @param[in] info ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @note �ýӿ���δʵ��
	* @ingroup G_T_UserInfo
	*/
	virtual void TAP_CDECL OnRspSetReservedInfo(TAPIUINT32 sessionID, TAPIINT32 errorCode, const TAPISTR_50 info) = 0;
	/**
	* @brief	�����û���Ϣ
	* @details	�˻ص��ӿ����û����ز�ѯ���ʽ��˺ŵ���ϸ��Ϣ���û��б�Ҫ���õ����˺ű�ű���������Ȼ���ں����ĺ���������ʹ�á�
	* @param[in] sessionID ����ĻỰID��
	* @param[in] errorCode �����롣0 ��ʾ�ɹ���
	* @param[in] isLast ��ʾ�Ƿ������һ�����ݣ�
	* @param[in] info ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_AccountInfo
	*/
	virtual void TAP_CDECL OnRspQryAccount(TAPIUINT32 sessionID, TAPIUINT32 errorCode, TAPIYNFLAG isLast, const TapAPIAccountInfo *info) = 0;
	/**
	* @brief �����ʽ��˻����ʽ���Ϣ
	* @param[in] sessionID ����ĻỰID��
	* @param[in] errorCode �����롣0 ��ʾ�ɹ���
	* @param[in] isLast 	��ʾ�Ƿ������һ�����ݣ�
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_AccountDetails
	*/
	virtual void TAP_CDECL OnRspQryFund(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFundData *info) = 0;
	/**
	* @brief	�û��ʽ�仯֪ͨ
	* @details	�û���ί�гɽ���������ʽ����ݵı仯�������Ҫ���û�ʵʱ������
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @note �������ע�������ݣ������趨Loginʱ��NoticeIgnoreFlag�����Ρ�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_AccountDetails
	*/
	virtual void TAP_CDECL OnRtnFund(const TapAPIFundData *info) = 0;
	/**
	* @brief ����ϵͳ�еĽ�������Ϣ
	* @param[in] sessionID ����ĻỰID��
	* @param[in] errorCode �����롣0 ��ʾ�ɹ���
	* @param[in] isLast 	��ʾ�Ƿ������һ�����ݣ�
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_TradeSystem
	*/
	virtual void TAP_CDECL OnRspQryExchange(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIExchangeInfo *info) = 0;
	/**
	* @brief	����ϵͳ��Ʒ����Ϣ
	* @details	�˻ص��ӿ��������û����صõ�������Ʒ����Ϣ��
	* @param[in] sessionID ����ĻỰID����GetAllCommodities()�������ض�Ӧ��
	* @param[in] errorCode �����롣0 ��ʾ�ɹ���
	* @param[in] isLast 	��ʾ�Ƿ������һ�����ݣ�
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_Commodity
	*/
	virtual void TAP_CDECL OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICommodityInfo *info) = 0;
	/**
	* @brief ����ϵͳ�к�Լ��Ϣ
	* @param[in] sessionID ����ĻỰID��
	* @param[in] errorCode �����롣0 ��ʾ�ɹ���
	* @param[in] isLast 	��ʾ�Ƿ������һ�����ݣ�
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_Contract
	*/
	virtual void TAP_CDECL OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPITradeContractInfo *info) = 0;
	/**
	* @brief	����������Լ��Ϣ
	* @details	���û������µĺ�Լ����Ҫ���������ڽ���ʱ����з�����������º�Լʱ�����û����������Լ����Ϣ��
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_Contract
	*/
	virtual void TAP_CDECL OnRtnContract(const TapAPITradeContractInfo *info) = 0;
	/**
	* @brief ������ί�С����µĻ��������ط��µ����͹����ġ�
	* @details	���������յ��ͻ��µ�ί�����ݺ�ͻᱣ�������ȴ�������ͬʱ���û�����һ��
	*			��ί����Ϣ˵����������ȷ�������û������󣬷��ص���Ϣ�а�����ȫ����ί����Ϣ��
	*			ͬʱ��һ��������ʾ��ί�е�ί�кš�
	* @param[in] info ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @note �������ע�������ݣ������趨Loginʱ��NoticeIgnoreFlag�����Ρ�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_TradeActions
	*/
	virtual void TAP_CDECL OnRtnOrder(const TapAPIOrderInfoNotice *info) = 0;
	/**
	* @brief	���ضԱ����������������
	* @details	���µ��������Ȳ����Ľ����
	* @param[in] sessionID ����ĻỰID��
	* @param[in] errorCode �����롣0 ��ʾ�ɹ���
	* @param[in] info �����ľ�����Ϣ����errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @note �ýӿ�Ŀǰû���õ������в������ͨ��OnRtnOrder����
	* @ingroup G_T_TradeActions
	*/
	virtual void TAP_CDECL OnRspOrderAction(TAPIUINT32 sessionID, TAPIUINT32 errorCode, const TapAPIOrderActionRsp *info) = 0;
	/**
	* @brief	���ز�ѯ��ί����Ϣ
	* @details	�����û���ѯ��ί�еľ�����Ϣ��
	* @param[in] sessionID ����ĻỰID��
	* @param[in] errorCode �����롣0 ��ʾ�ɹ���
	* @param[in] isLast ��ʾ�Ƿ������һ�����ݣ�
	* @param[in] info ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_TradeInfo
	*/
	virtual void TAP_CDECL OnRspQryOrder(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo *info) = 0;
	/**
	* @brief ���ز�ѯ��ί�б仯������Ϣ
	* @param[in] sessionID ����ĻỰID��
	* @param[in] errorCode �����룬��errorCode==0ʱ��infoָ�򷵻ص�ί�б仯���̽ṹ�壬��ȻΪNULL��
	* @param[in] isLast ��ʾ�Ƿ������һ�����ݣ�
	* @param[in] info ���ص�ί�б仯����ָ�롣
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_TradeInfo
	*/
	virtual void TAP_CDECL OnRspQryOrderProcess(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIOrderInfo *info) = 0;
	/**
	* @brief ���ز�ѯ�ĳɽ���Ϣ
	* @param[in] sessionID ����ĻỰID��
	* @param[in] errorCode �����롣0 ��ʾ�ɹ���
	* @param[in] isLast 	��ʾ�Ƿ������һ�����ݣ�
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_TradeInfo
	*/
	virtual void TAP_CDECL OnRspQryFill(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIFillInfo *info) = 0;
	/**
	* @brief	�������ĳɽ���Ϣ
	* @details	�û���ί�гɽ������û����ͳɽ���Ϣ��
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @note �������ע�������ݣ������趨Loginʱ��NoticeIgnoreFlag�����Ρ�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_TradeActions
	*/
	virtual void TAP_CDECL OnRtnFill(const TapAPIFillInfo *info) = 0;
	/**
	* @brief ���ز�ѯ�ĳֲ�
	* @param[in] sessionID ����ĻỰID��
	* @param[in] errorCode �����롣0 ��ʾ�ɹ���
	* @param[in] isLast 	��ʾ�Ƿ������һ�����ݣ�
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_TradeInfo
	*/
	virtual void TAP_CDECL OnRspQryPosition(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIPositionInfo *info) = 0;
	/**
	* @brief �ֱֲ仯����֪ͨ
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @note �������ע�������ݣ������趨Loginʱ��NoticeIgnoreFlag�����Ρ�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_TradeActions
	*/
	virtual void TAP_CDECL OnRtnPosition(const TapAPIPositionInfo *info) = 0;
	/**
	* @brief ���ز�ѯ��ƽ��
	* @param[in] sessionID ����ĻỰID��
	* @param[in] errorCode �����롣0 ��ʾ�ɹ���
	* @param[in] isLast 	��ʾ�Ƿ������һ�����ݣ�
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_TradeInfo
	*/
	virtual void TAP_CDECL OnRspQryClose(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPICloseInfo *info) = 0;
	/**
	* @brief ƽ�����ݱ仯����
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @note �������ע�������ݣ������趨Loginʱ��NoticeIgnoreFlag�����Ρ�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_TradeActions
	*/
	virtual void TAP_CDECL OnRtnClose(const TapAPICloseInfo *info) = 0;
	/**
	* @brief �ֲ�ӯ��֪ͨ
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @note �������ע�������ݣ������趨Loginʱ��NoticeIgnoreFlag�����Ρ�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_TradeActions
	*/
	virtual void TAP_CDECL OnRtnPositionProfit(const TapAPIPositionProfitNotice *info) = 0;
	/**
	* @brief ��������ѯӦ��
	* @param[in] sessionID ����ĻỰID��
	* @param[in] errorCode �����롣0 ��ʾ�ɹ���
	* @param[in] isLast 	��ʾ�Ƿ������һ�����ݣ�
	* @param[in] info	  ָ�򷵻ص����������Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_DeepQuote
	*/
	virtual void TAP_CDECL OnRspQryDeepQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIDeepQuoteQryRsp *info) = 0;
	/**
	* @brief ������ʱ��״̬��Ϣ��ѯӦ��
	* @param[in] sessionID ����ĻỰID��
	* @param[in] errorCode �����롣0 ��ʾ�ɹ���
	* @param[in] isLast 	��ʾ�Ƿ������һ������
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention  ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_TradeSystem
	*/
	virtual void TAP_CDECL OnRspQryExchangeStateInfo(TAPIUINT32 sessionID,TAPIINT32 errorCode, TAPIYNFLAG isLast,const TapAPIExchangeStateInfo * info)=0;
	/**
	* @brief ������ʱ��״̬��Ϣ֪ͨ	
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention  ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_TradeSystem
	*/
	virtual void TAP_CDECL OnRtnExchangeStateInfo(const TapAPIExchangeStateInfoNotice * info)=0;
	/**
	* @brief ѯ��֪ͨ
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_TradeActions
	*/
	virtual void TAP_CDECL OnRtnReqQuoteNotice(const TapAPIReqQuoteNotice *info) = 0;

	/**
	* @brief ������Ϣ��ѯӦ��
	* @param[in] sessionID ����ĻỰID��
	* @param[in] errorCode �����롣0 ��ʾ�ɹ���
	* @param[in] isLast 	��ʾ�Ƿ������һ������
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention  ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_UpperChannelInfo
	*/
	virtual void TAP_CDECL OnRspUpperChannelInfo(TAPIUINT32 sessionID,TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIUpperChannelInfo * info)=0;
        /**
	* @brief �ͻ����շ���Ӧ��
        * @details   ��֤��������㷽ʽ������*ÿ�ֳ���*�������*�۸�\n
        *             ��֤�𶨶���㷽ʽ������*�������\n
        *             �����Ѿ��Է�ʽ���㷽ʽ������*�������������+����*ÿ�ֳ���*�۸�*�����������
	* @param[in] sessionID ����ĻỰID��
	* @param[in] errorCode �����롣0 ��ʾ�ɹ���
	* @param[in] isLast 	��ʾ�Ƿ������һ������
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention  ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_T_AccountRentInfo
	*/
	virtual void TAP_CDECL OnRspAccountRentInfo(TAPIUINT32 sessionID,TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIAccountRentInfo * info)=0;
};

//TapTradeAPI ���⹦�ܽӿڡ��������û����Ե��õĹ��ܺ�����
class ITapTradeAPI
{
public:
	/**
	* @brief	����API�Ļص��ӿڶ���
	* @details	ϵͳ��API��֪ͨ��ͨ�����õĻص�����֪ͨ��ʹ���ߡ�
	*			ITapTradeAPINotify��API�Ļص��ӿڣ��û���Ҫ�̳�ʵ�ִ˽ӿ������������û���Ҫ�Ĺ��ܡ�
	*			����û�û�����ûص��ӿڣ���API�������û������κ����õ���Ϣ��\n
	*			ITapTradeAPINotify�����ϸ������鿴TapTradeAPI.h �ļ���
	* @param[in] apiNotify ʵ����ITapTradeAPINotify�ӿڵĶ���ָ�롣
	* @operationtype ͬ������
	* @ingroup G_T_Login
	*/
	virtual TAPIINT32 TAP_CDECL SetAPINotify(ITapTradeAPINotify *apiNotify) = 0;
	/**
	* @brief ���÷�������IP��ַ�Ͷ˿ڡ�
	* @param[in] IP   IP��ַ
	* @param[in] port �˿ں�
	* @operationtype ͬ������
	* @ingroup G_T_Login
	*/
	virtual TAPIINT32 TAP_CDECL SetHostAddress(const TAPICHAR *IP, TAPIUINT16 port) = 0;
	/**
	* @brief	�����¼����API�������ӷ��񣬽�����·�������¼��֤��
	* @details	��ʹ�ú���ǰ�û���Ҫ��ɷ�����������SetHostAddress()�����Ҵ���TapAPITradeLoginAuth���͵��û���Ϣ��
	*			������Ҫ���úûص��ӿڡ�\n
	*			��¼�����н������ӵķ�����Ϣͨ���ص�OnConnect���ظ��û���\n
	*			���ӽ�������û���֤������Ϣͨ���ص�OnLogin()���ظ��û���\n
	*			��¼�ɹ���API���Զ�����API�ĳ�ʼ����API�����������������ݣ���ѯ���Ժ��ͨ���ص�OnAPIReady()
	*			ָʾ�û�API��ʼ����ɣ����Խ��к����Ĳ����ˡ�
	* @param[in] loginAuth ��¼��֤��Ϣ�ṹָ�롣������¼��Ҫ����֤��Ϣ��
	* @retval 0 ��¼�ɹ���API��ʼ׼����̨����
	* @retval ��0 ������
	* @operationtype �첽����
	* @note TapAPITradeLoginAuth�е�NoticeIgnoreFlag���ڱ���Ƿ�����ض���֪ͨ�ص���\n
	*       ���磺����Ҫ����OnRtnFund��OnRtnPositionProfit,������ô�趨��\n 
	*       NoticeIgnoreFlag = TAPI_NOTICE_IGNORE_FUND | TAPI_NOTICE_IGNORE_POSITIONPROFIT;
	* @attention Login()�������óɹ�ֻ�Ǵ����ڷ�������������·���ӣ�ֻ�лص�OnLogin()�ķ�����ָʾ�û��Ƿ�ɹ���¼�ˡ�
	* @ingroup G_T_Login
	*/
	virtual TAPIINT32 TAP_CDECL Login(const TapAPITradeLoginAuth *loginAuth) = 0;
	/**
	* @brief	�Ͽ��ͷ���������·���ӡ�
	* @details	���ú�����API���ǳ����Ͽ�������������ӡ�
	* @operationtype ͬ������
	* @ingroup G_T_Disconnect
	*/
	virtual TAPIINT32 TAP_CDECL Disconnect() = 0;
	/**
	* @brief	�޸����롣
	* @details	�ɹ����û����뽫�����ó�newPassword��
	* @param[out]	sessionID ���ش˴��޸�����ĻỰID;
	* @param[in]	req �����޸�����Ľṹ��ָ��
	* @retval 0 �ɹ�
	* @retval ��0 ������
	* @operationtype �첽����
	* @ingroup G_T_UserInfo
	*/
	virtual TAPIINT32 TAP_CDECL ChangePassword(TAPIUINT32 *sessionID, const TapAPIChangePasswordReq *req) = 0;
	/**
	* @brief	�жϵ�¼�û��Ƿ����ĳȨ�ޡ�
	* @details	�û���Ȩ�����û���¼ʱ�Ѿ����ݸ�API�����Դ˺���ִ�е��Ǳ��صĲ�ѯ��
	* @param[in] rightID Ȩ��ID��
	* @retval 0 ������Ȩ��
	* @retval ��0 ����Ȩ��
	* @operationtype ͬ������
	* @ingroup G_T_UserRight
	*/
	virtual TAPIINT32 TAP_CDECL HaveCertainRight(TAPIRightIDType rightID) = 0;
	/**
	* @brief	�����û�Ԥ����Ϣ��
	* @details	�û���������һ������Ϊ50���ڵ��ַ���Ϣ���´ε�¼����Եõ������Ϣ��
	*			���������Ҫ���������û�ȷ�����Լ����˺ţ���Ҫ���������з�α��
	* @param[out]	sessionID ��������ĻỰID;
	* @param[in] info	Ԥ����Ϣ�ַ���ָ��
	* @retval 0 ����ɹ�
	* @retval ��0 ������
	* @operationtype �첽����
	* @note �ýӿ���δʵ��
	* @ingroup G_T_UserInfo
	*/
	virtual TAPIINT32 TAP_CDECL SetReservedInfo(TAPIUINT32 *sessionID, const TAPISTR_50 info) = 0;
	/**
	* @brief ��ѯ�û��������ʽ��˺�
	* @details	TapAPIAccQryReq�ǰ����ʽ��˺Žṹ�Ľṹ�壬�û���Ҫʵ�����˽ṹ�壬
	*			���ǲ���Ҫ��д������ṹ����Ҫ�����������û������ʽ��˺ţ��ʽ��˺�
	*			�ں����ĺ��������кܶຯ������Ҫʹ�õ���
	* @param[out]	sessionID ��������ĻỰID;
	* @param[in]	qryReq ��ѯ�û��������ʽ��˺ŵĽṹ��ָ��
	* @retval 0 ����ɹ�
	* @retval ��0 ������
	* @operationtype �첽����
	* @ingroup G_T_AccountInfo
	*/
	virtual TAPIINT32 TAP_CDECL QryAccount(TAPIUINT32 *sessionID, const TapAPIAccQryReq *qryReq) = 0;
	/**
	* @brief	��ѯ�ͻ��ʽ�
	* @details	TapAPIFundReq��ҪQryAccount()��ȡ���ʽ��˺š�
	*			�����Ļص��������ʽ��˺ŵ��ʽ���Ϣ��\n
	* @param[out] sessionID ��������ĻỰID;
	* @param[in] qryReq	��ѯ�ͻ��ʽ�����Ľṹ��ָ��
	* @retval 0 ����ɹ�
	* @retval ��0 ������
	* @operationtype �첽����
	* @ingroup G_T_AccountDetails
	* @attention ÿ�ε�½���ýӿ�ֻ�ܽ���һ�ε��á������ʼ��ʱ����һ�Σ�������֪ͨͨ��OnRtnFundȡ�á�
	*/
	virtual TAPIINT32 TAP_CDECL QryFund(TAPIUINT32 *sessionID, const TapAPIFundReq *qryReq) = 0;
	/**
	* @brief ��ѯ����ϵͳ��������Ϣ
	* @param[out] sessionID ��������ĻỰID;
	* @retval 0 ����ɹ�
	* @retval ��0 ������
	* @operationtype �첽����
	* @ingroup G_T_TradeSystem
	*/
	virtual TAPIINT32 TAP_CDECL QryExchange(TAPIUINT32 *sessionID) = 0;
	/**
	* @brief	��ѯϵͳƷ����Ϣ
	* @details	���������ȡ���е�Ʒ����Ϣ��
	* @param[out] sessionID ��������ĻỰID;
	* @retval 0 ����ɹ�
	* @retval ��0 ������
	* @operationtype �첽����
	* @ingroup G_T_Commodity
	*/
	virtual TAPIINT32 TAP_CDECL QryCommodity(TAPIUINT32 *sessionID) = 0;
	/**
	* @brief	��ѯϵͳ��ָ��Ʒ�ֵĺ�Լ��Ϣ
	* @details	ʹ�ô˺���ǰ��Ҫ��QrycommodityInfo()ȡ��Ʒ����Ϣ��
	*			Ȼ��ѡ����Ҫ��Ʒ�ֽ���Ϣ����TapAPICommodity�ṹ������ɲ�ѯ����
	* @param[out] sessionID ��������ĻỰID;
	* @param[in] qryReq ��ѯϵͳ��ָ��Ʒ�ֵĺ�Լ��Ϣ����Ľṹ��ָ��;\n
	*				    �ò������ֶ�Ϊ��ѡ�ֶΣ����������·�����ѯ��\n
	*					1.ȫ�����գ������к�Լ\n
	*					2.��������������Ч����ý�����������Ʒ�ֵĺ�Լ\n
	*					3.�����������Ʒ��������Ч����ý�������ָ��Ʒ�����͵ĺ�Լ\n
	*					4.���������롢Ʒ�����ͺ�Ʒ�ֱ��붼��Ч�����Ʒ���µ����к�Լ
	* @retval 0 ����ɹ�
	* @retval ��0 ������
	* @operationtype �첽����
	* @ingroup G_T_Contract
	*/
	virtual TAPIINT32 TAP_CDECL QryContract(TAPIUINT32 *sessionID, const TapAPICommodity *qryReq) = 0;
	/**
	* @brief	�µ�
	* @details	�û��µ��Ĳ���������
	*			�û����µ�����������û����ʽ𡢳ֲ֡�ƽ�֡��ʽ𡢷�ر�ǵȶ������ݵı仯�������û��µĵ��ɽ���
	*			���ж���ص�֪ͨ�����û�չʾ���ݵı仯��
	* @param[out]	sessionID ��������ĻỰID;
	* @param[in]	order �µ�ί��
	* @retval 0 ����ɹ�
	* @retval ��0 ������
	* @operationtype �첽����
	* @ingroup G_T_TradeActions
	*/
	virtual TAPIINT32 TAP_CDECL InsertOrder(TAPIUINT32 *sessionID, const TapAPINewOrder *order) = 0;
	/**
	* @brief	����
	* @details	�û�ί��û����ȫ�ɽ�֮ǰ����ʣ���ί�С�
	* @param[out]	sessionID ��������ĻỰID;
	* @param[in]	order ������ί��
	* @retval 0 ����ɹ�
	* @retval ��0 ������
	* @operationtype �첽����
	* @ingroup G_T_TradeActions
	*/
	virtual TAPIINT32 TAP_CDECL CancelOrder(TAPIUINT32 *sessionID, const TapAPIOrderCancelReq *order) = 0;
	/**
	* @brief	��ѯί����Ϣ
	* @details	���ص�ǰ��ί����Ϣ�����Բ�ѯ����ί�У�Ҳ���Բ�ѯ����δ������ί�С�\n
	* @retval 0 ����ɹ�
	* @retval ��0 ������
	* @operationtype �첽����
	* @ingroup G_T_TradeInfo
	* @attention �ýӿڿ��Զ�ε��ã������в�ѯƵ�ʵ�Ҫ��ÿ������Բ�ѯһ��
	*/
	virtual TAPIINT32 TAP_CDECL QryOrder(TAPIUINT32 *sessionID, const TapAPIOrderQryReq *qryReq) = 0;
	/**
	* @brief	��ѯί�б仯����
	* @details	��ѯ�û���ί�еı仯���̣���ѯ������ί�е�ÿһ�εı仯��
	* @param[out] sessionID ��������ĻỰID;
	* @param[in] qryReq ί�����̲�ѯ��Ϣ�ṹ��ָ�롣
	* @retval 0 ����ɹ�
	* @retval ��0 ������
	* @operationtype �첽����
	* @ingroup G_T_TradeInfo
	*/
	virtual TAPIINT32 TAP_CDECL QryOrderProcess(TAPIUINT32 *sessionID, const TapAPIOrderProcessQryReq *qryReq) = 0;
	/**
	* @brief ��ѯ�ɽ���Ϣ
	* @details	��ѯϵͳ�гɽ�����Ϣ��
	* @param[out] sessionID ��������ĻỰID;
	* @param[in] qryReq �ɽ���ѯ��Ϣ�ṹ��ָ�롣
	* @retval 0 ����ɹ�
	* @retval ��0 ������
	* @operationtype �첽����
	* @ingroup G_T_TradeInfo
	* @attention ÿ�ε�½���ýӿ�ֻ�ܽ���һ�ε��á������ʼ��ʱ����һ�Σ�������֪ͨͨ��OnRtnFillȡ�á�
	*/
	virtual TAPIINT32 TAP_CDECL QryFill(TAPIUINT32 *sessionID, const TapAPIFillQryReq *qryReq) = 0;
	/**
	* @brief	��ѯ�û��ֲ�
	* @details	��ѯ�û������гֲ���Ϣ��
	* @param[out] sessionID ��������ĻỰID;
	* @param[in] qryReq	��ѯ�û��ֲ�����Ľṹ��ָ��
	* @retval 0 ����ɹ�
	* @retval ��0 ������
	* @operationtype �첽����
	* @ingroup G_T_TradeInfo
	* @attention �ýӿڿ��Զ�ε��ã������в�ѯƵ�ʵ�Ҫ��ÿ������Բ�ѯһ��
	*/
	virtual TAPIINT32 TAP_CDECL QryPosition(TAPIUINT32 *sessionID, const TapAPIPositionQryReq *qryReq) = 0;
	/**
	* @brief ��ѯƽ�ּ�¼
	* @details	��ѯ�û����е�ƽ�ּ�¼��
	* @param[out] sessionID ��������ĻỰID;
	* @param[in] qryReq	��ѯƽ�ּ�¼����Ľṹ��ָ��
	* @retval 0 ����ɹ�
	* @retval ��0 ������
	* @operationtype �첽����
	* @ingroup G_T_TradeInfo
	* @attention ÿ�ε�½���ýӿ�ֻ�ܽ���һ�ε��á������ʼ��ʱ����һ�Σ�������֪ͨͨ��OnRtnCloseȡ�á�
	*/
	virtual TAPIINT32 TAP_CDECL QryClose(TAPIUINT32 *sessionID, const TapAPICloseQryReq *qryReq) = 0;
	/**
	* @brief ��ѯ�������
	* @param[out]	sessionID ��������ĻỰID;
	* @param[in]	qryReq	��ѯ������������ṹ��ָ�룬�ڶ�����Լ���ա�
	* @retval 0 ����ɹ�
	* @retval ��0 ������
	* @operationtype �첽����
	* @ingroup G_T_DeepQuote
	*/
	virtual TAPIINT32 TAP_CDECL QryDeepQuote(TAPIUINT32 *sessionID, const TapAPIContract *qryReq) = 0;
	/**
	* @brief	��ѯ������ʱ��״̬��Ϣ
	* @param[out]	sessionID ��������ĻỰID;
	* @param[in]	qryReq ��ѯ������ʱ��״̬��Ϣ����ṹ��ָ��
	* @retval 0 ����ɹ�
	* @retval ��0 ������
	* @operationtype �첽����
	* @ingroup G_T_TradeSystem
	*/
	virtual TAPIINT32 TAP_CDECL QryExchangeStateInfo(TAPIUINT32 * sessionID,const TapAPIExchangeStateInfoQryReq * qryReq)=0;
	/**
	* @brief   ��ѯ������Ϣ,��Ҫ�����ڿͻ��µ�ʱָ��ͨ���ţ���������ѡ���ӽ��뽻����ǰ�û�
	* @param[out]    sessionID ��������ĻỰID;
	* @param[in]	 qryReq    ����ͨ����Ϣ��ѯ����ṹ��ָ��
	* @retval  0   ����ɹ�
	* @retval  ��0 ������
	* @operationtype �첽����
	* @ingroup G_T_UpperChannelInfo
	*/
	virtual TAPIINT32 TAP_CDECL QryUpperChannel(TAPIUINT32 * sessionID, const TapAPIUpperChannelQryReq * qryReq)=0;
        /**
	* @brief   ��ѯ�ͻ����շ���,��Ҫ�����ڿͻ���ѯ������Լ���շ��ʣ������ڽ����
	* @param[out]    sessionID ��������ĻỰID;
	* @param[in]	 qryReq    �ͻ����շ��ʲ�ѯ����ṹ��ָ��
	* @retval  0   ����ɹ�
	* @retval  ��0 ������
	* @operationtype �첽����
	* @ingroup G_T_AccountRentInfo
	*/
	virtual TAPIINT32 TAP_CDECL QryAccountRent(TAPIUINT32 * sessionID, const TapAPIAccountRentQryReq * qryReq)=0;
	/**
	* @brief	�����
	* @details	�������ı�����ί�н��������¿�ʼ�����ɽ���
	* @param[out]	sessionID ��������ĻỰID;
	* @param[in]	order �����ί��
	* @retval 0 ����ɹ�
	* @retval ��0 ������
	* @operationtype �첽����
	* @ingroup G_T_TradeActions
	*/
	virtual TAPIINT32 TAP_CDECL ActivateOrder(TAPIUINT32 *sessionID, const TapAPIOrderActivateReq *qryReq) = 0;
};

//-----------------------------TapTradeAPI��������------------------------------------


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
* @brief	����TapTradeAPI�Ľӿڶ���
* @details	������������API�Ľӿ�
* @param[in] appInfo Ӧ�ó��������Ϣ��KeyOperationLogPath ·��Ϊ��ʱ�������������־
* @param[in] iResult �����ӿڵĴ����롣
* @retval NULL	����ʧ�ܣ�����ԭ���ͨ���������iResult�жϡ�
* @retval !NULL	ʵ����ITapTradeAPI�ӿڵĶ���ָ�롣
* @ingroup G_T_API
*/
TAP_DLLEXPORT ITapTradeAPI *TAP_CDECL CreateTapTradeAPI(const TapAPIApplicationInfo *appInfo, TAPIINT32 &iResult);
/**
* @brief	����ͨ��CreateTapTradeAPI����������ITapTradeAPI����
* @param[in] apiObj ITapTradeAPI����ָ�롣
* @ingroup G_T_API
*/
TAP_DLLEXPORT void TAP_CDECL FreeTapTradeAPI(ITapTradeAPI *apiObj);
/**
* @brief	��ȡTapTradeAPI�İ汾��Ϣ
* @ingroup G_T_API
*/
TAP_DLLEXPORT const TAPICHAR *TAP_CDECL GetTapTradeAPIVersion();
/**
* @brief	����API����������Ŀ¼
* @details	���ú�����ͬʱ����path������Ŀ¼�´��������գ���ʽTapTradeAPI[YYYYMMDD].log)�������ļ���\n
*			û�д��ļ�������»���ͼ�������ļ���\n
*			�ļ��б��������ΪAPI���յ�����Ҫ���ݺ�API��ʹ�úʹ�����־��
* @param[in] path Ŀ¼��������ã�Ŀ¼����Window��Ϊ��\\�����ߡ�/��, Linux��Ϊ��/����
* @retval 0 �ɹ�
* @retval ��0 ������
* @operationtype ͬ������
* @ingroup G_T_LogConfig
*/
TAP_DLLEXPORT TAPIINT32 TAP_CDECL SetTapTradeAPIDataPath(const TAPICHAR *path);
/**
* @brief	����API����־����
* @details	�趨��־���������ֻ�е�ʵ����־������˴��趨�ļ�����ͬ�����ʱ���ŻὫ��־д��SetTapTradeAPIDataPath()�����򿪵���־�ļ���\n
* @param[in]	level ��־����\n
*					APILOGLEVEL_NONE	������¼��־\n
*					APILOGLEVEL_ERROR	����¼Error��־\n
*					APILOGLEVEL_WARNING	����¼Error��Warning��־\n
*					APILOGLEVEL_DEBUG	����¼Error��Warning��Debug��־\n
* @retval 0 �趨�ɹ�
* @retval ��0 ������
* @operationtype ͬ������
* @ingroup G_T_LogConfig
*/
TAP_DLLEXPORT TAPIINT32 TAP_CDECL SetTapTradeAPILogLevel(TAPILOGLEVEL level);

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // TAP_TRADE_API_H