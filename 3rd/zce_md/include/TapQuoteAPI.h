#ifndef TAP_QUOTE_API_H
#define TAP_QUOTE_API_H

#include "TapQuoteAPIDataType.h"

#ifdef _WIN32
#define TAP_CDECL  __cdecl
#define TAP_DLLEXPORT __declspec(dllexport)
#else
#define TAP_CDECL
#define TAP_DLLEXPORT
#endif

//TapQuoteAPI.h
//�ļ�������TapQuoteAPI�ṩ�������ߵĶ���ӿڡ������ͻص��ӿڡ�

//TapQuoteAPI �Ļص�֪ͨ�ӿڡ�
class ITapQuoteAPINotify
{
public:
	/**
	* @brief	ϵͳ��¼���̻ص���
	* @details	�˺���ΪLogin()��¼�����Ļص�������Login()�ɹ���������·���ӣ�Ȼ��API������������͵�¼��֤��Ϣ��
	*			��¼�ڼ�����ݷ�������͵�¼�Ļ�����Ϣ���ݵ��˻ص������С�
	* @param[in] errorCode ���ش�����,0��ʾ�ɹ���
	* @param[in] info ��½Ӧ����Ϣ�����errorCode!=0����info=NULL��
	* @attention	�ûص����سɹ���˵���û���¼�ɹ������ǲ�����API׼����ϡ���Ҫ�ȵ�OnAPIReady���ܽ��в�ѯ�붩������
	* @ingroup G_Q_Login
	*/
	virtual void TAP_CDECL OnRspLogin(TAPIINT32 errorCode, const TapAPIQuotLoginRspInfo *info) = 0;
	/**
	* @brief	֪ͨ�û�API׼��������
	* @details	ֻ���û��ص��յ��˾���֪ͨʱ���ܽ��к����ĸ����������ݲ�ѯ������\n
	*			�˻ص�������API�ܷ����������ı�־��
	* @attention  ������ſ��Խ��к�����������
	* @ingroup G_Q_Login
	*/
	virtual void TAP_CDECL OnAPIReady() = 0;
	/**
	* @brief	API�ͷ���ʧȥ���ӵĻص�
	* @details	��APIʹ�ù������������߱��������������ʧȥ���Ӻ󶼻ᴥ���˻ص�֪ͨ�û���������������Ѿ��Ͽ���
	* @param[in] reasonCode �Ͽ�ԭ����롣����ԭ����μ��������б� \n
	* @ingroup G_Q_Disconnect
	*/
	virtual void TAP_CDECL OnDisconnect(TAPIINT32 reasonCode) = 0;
	/**
	* @brief	��������Ʒ����Ϣ��
	* @details	�˻ص��ӿ��������û����صõ�������Ʒ����Ϣ��
	* @param[in] sessionID ����ĻỰID
	* @param[in] errorCode �����룬��errorCode!=0ʱ,infoΪNULL��
	* @param[in] isLast ��ʾ�Ƿ������һ�����ݣ�
	* @param[in] info ���ص���Ϣ�������ʼָ�롣
	* @attention  ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_Q_Commodity
	*/
	virtual void TAP_CDECL OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteCommodityInfo *info) = 0;
	/**
	* @brief ����ϵͳ�к�Լ��Ϣ
	* @param[in] sessionID ����ĻỰID��
	* @param[in] errorCode �����룬��errorCode!=0ʱ,infoΪNULL��
	* @param[in] isLast ��ʾ�Ƿ������һ�����ݣ�
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_Q_Contract
	*/
	virtual void TAP_CDECL OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteContractInfo *info) = 0;
	/**
	* @brief	���ض��������ȫ�ġ�
	* @details	�˻ص��ӿ��������ض��������ȫ�ġ�ȫ��Ϊ��ǰʱ���������Ϣ��
	* @param[in] sessionID ����ĻỰID��
	* @param[in] isLast ��ʾ�Ƿ������һ�����ݣ�
	* @param[in] errorCode �����룬��errorCode!=0ʱ,infoΪNULL��
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention  ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_Q_Quote
	*/
	virtual void TAP_CDECL OnRspSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteWhole *info) = 0;
	/**
	* @brief �˶�ָ����Լ������Ľ���ص�
	* @param[in] sessionID ����ĻỰID��
	* @param[in] errorCode �����룬��errorCode!=0ʱ,infoΪNULL��
	* @param[in] isLast ��ʾ�Ƿ������һ�����ݣ�
	* @param[in] info		ָ�򷵻ص���Ϣ�ṹ�塣��errorCode��Ϊ0ʱ��infoΪ�ա�
	* @attention  ��Ҫ�޸ĺ�ɾ��info��ָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_Q_Quote
	*/
	virtual void TAP_CDECL OnRspUnSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIContract *info) = 0;
	/**
	* @brief	���ض�������ı仯���ݡ�
	* @details	�˻ص��ӿ�����֪ͨ�û�������Ϣ�����˱仯�������û��ύ�µ�����ȫ�ġ�
	* @param[in] info ���µ�����ȫ������
	* @attention ��Ҫ�޸ĺ�ɾ��Quoteָʾ�����ݣ��������ý���������������Ч��
	* @ingroup G_Q_Quote
	*/
	virtual void TAP_CDECL OnRtnQuote(const TapAPIQuoteWhole *info) = 0;
};

//TapQuoteAPI ���⹦�ܽӿڡ��������û����Ե��õĹ��ܺ�����
class ITapQuoteAPI
{
public:
	/**
	* @brief	����API�Ļص��ӿڶ���
	* @details	ϵͳ��API��֪ͨ��ͨ�����õĻص�����֪ͨ��ʹ���ߡ�
	*			ITapQuoteAPINofify��API�Ļص��ӿڣ��û���Ҫ�̳�ʵ�ִ˽ӿ������������û���Ҫ�Ĺ��ܡ�
	*			����û�û�����ûص��ӿڣ���API�������û������κ����õ���Ϣ��\n
	*			ITapQuoteAPINotify�����ϸ������鿴TapQuoteAPI.h �ļ���
	* @param[in] apiNotify ʵ����ITapQuoteAPINotify�ӿڵĶ���ָ�롣
	* @operationtype ͬ������
	* @ingroup G_Q_Login
	*/
	virtual TAPIINT32 TAP_CDECL SetAPINotify(ITapQuoteAPINotify *apiNotify) = 0;
	/**
	* @brief ���÷�������IP��ַ�Ͷ˿ڡ��ȵ�����Loginʱ�����������ӡ�
	* @param[in] IP   IP��ַ
	* @param[in] port �˿ں�
	* @operationtype ͬ������
	* @ingroup G_Q_Login
	*/
	virtual TAPIINT32 TAP_CDECL SetHostAddress(const TAPICHAR *IP, TAPIUINT16 port) = 0;
	/**
	* @brief	�����¼����API�������ӷ��񣬽�����·�������¼��֤��
	* @details	��ʹ�ú�������ǰ�û���Ҫ��ɷ�����������SetHostAddress()�����Ҵ���TapAPIQuoteLoginAuth���͵��û���Ϣ��
	*			������Ҫ���úûص��ӿڡ�\n
	*			���ӽ�������û���֤������Ϣͨ���ص�OnLogin()���ظ��û���\n
	*			��¼�ɹ���API���Զ�����API�ĳ�ʼ����API�����������������ݣ���ѯ���Ժ��ͨ���ص�OnAPIReady()
	*			ָʾ�û�API��ʼ����ɣ����Խ��к����Ĳ����ˡ�
	* @param[in] loginAuth ��¼��֤��Ϣ�ṹָ�롣������¼��Ҫ����֤��Ϣ��
	* @retval 0 ��¼�ɹ���API��ʼ׼����̨����
	* @retval ��0 ������
	* @attention ��¼�ɹ����Ҫ����OnLogin�ص��жϡ�
	* @operationtype �첽����
	* @warning	Login()�������óɹ�ֻ�Ǵ����ڷ�������������·���ӣ�ֻ�лص�OnLogin()�ķ�����ָʾ�û��Ƿ�ɹ���¼�ˡ�
	* @ingroup G_Q_Login
	*/
	virtual TAPIINT32 TAP_CDECL Login(const TapAPIQuoteLoginAuth *loginAuth) = 0;
	/**
	* @brief	�Ͽ��ͷ���������·���ӡ�
	* @details	���ú�����API���ǳ����Ͽ�������������ӡ�
	* @operationtype �첽����
	* @ingroup G_Q_Disconnect
	*/
	virtual TAPIINT32 TAP_CDECL Disconnect() = 0;
	/**
	* @brief �õ�����Ʒ��
	* @param[out] sessionID ��������ĻỰID��
	* @retval 0 ����ɹ�
	* @retval ��0 ������
	* @operationtype �첽����
	* @ingroup G_Q_Commodity
	*/
	virtual TAPIINT32 TAP_CDECL QryCommodity(TAPIUINT32 *sessionID) = 0;
	/**
	* @brief	��ѯϵͳ��ָ��Ʒ�ֵĺ�Լ��Ϣ
	* @details	ʹ�ô˺���ǰ��Ҫ��QryCommodity()ȡ��Ʒ����Ϣ��
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
	* @ingroup G_Q_Contract
	*/
	virtual TAPIINT32 TAP_CDECL QryContract(TAPIUINT32 *sessionID, const TapAPICommodity *qryReq) = 0;
	/**
	* @brief	����ָ����Լ������
	* @details	���������������contract�����ĺ�Լ��������Ϣ�����鶩�ĳɹ�����������������û�����������Ϣ��
	*			ֱ���û��˶�������Ϣ���߶Ͽ��ڷ�������ͨ�š�\n
	*			���ô˺���ǰ�Ȼ�ȡ��Լ��Ϣ��Ȼ��Ӻ�Լ��Ϣ��ȡ����Լ����contract��\n
	* @param[out] sessionID ��������ĻỰID��
	* @param[in] contract ָ����Լ��
	* @retval 0 ����ɹ�
	* @retval ��0 ������
	* @operationtype �첽����
	* @ingroup G_Q_Quote
	*/
	virtual TAPIINT32 TAP_CDECL SubscribeQuote(TAPIUINT32 *sessionID, const TapAPIContract *contract) = 0;
	/**
	* @brief	�˶�ָ����Լ������
	* @details	�˶��Ѿ����ĵ�������Ϣ��
	* @param[out] sessionID ��������ĻỰID��
	* @param[in]	contract ָ����Լ��
	* @retval 0 ����ɹ�
	* @retval ��0 ������
	* @operationtype �첽����
	* @ingroup G_Q_Quote
	*/
	virtual TAPIINT32 TAP_CDECL UnSubscribeQuote(TAPIUINT32 *sessionID, const TapAPIContract *contract) = 0;
};

//-----------------------------TapQuoteAPI��������------------------------------------


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
* @brief	����TapQuoteAPI�Ľӿڶ���
* @details	������������API�Ľӿ�
* @param[in] appInfo Ӧ�ó��������Ϣ��
* @param[in] iResult �����ӿڵĴ����롣
* @retval NULL	����ʧ�ܡ�
* @retval !NULL	ʵ����ITapQuoteAPI�ӿڵĶ���ָ�롣
* @ingroup G_Q_API
*/
TAP_DLLEXPORT ITapQuoteAPI *TAP_CDECL CreateTapQuoteAPI(const TapAPIApplicationInfo *appInfo, TAPIINT32 &iResult);
/**
* @brief	����ͨ��CreateTapQuoteAPI����������ITapQuoteAPI����
* @param[in] apiObj ITapQuoteAPI����ָ�롣
* @ingroup G_Q_API
*/
TAP_DLLEXPORT void TAP_CDECL FreeTapQuoteAPI(ITapQuoteAPI *apiObj);
/**
* @brief	��ȡTapQuoteAPI�İ汾��Ϣ
* @ingroup G_Q_API
*/
TAP_DLLEXPORT const TAPICHAR *TAP_CDECL GetTapQuoteAPIVersion();
/**
* @brief	����API����������Ŀ¼
* @details	���ú�����ͬʱ����path������Ŀ¼�´��������գ���ʽTapQuoteAPI[YYYYMMDD].log)�������ļ���\n
*			û�д��ļ�������»���ͼ�������ļ���\n
*			�ļ��б��������ΪAPI���յ�����Ҫ���ݺ�API��ʹ�úʹ�����־��
* @param[in] path Ŀ¼��������ã�Ŀ¼����Window��Ϊ��\\�����ߡ�/��, Linux��Ϊ��/����
* @retval 0 �ɹ�
* @retval ��0 ������
* @operationtype ͬ������
* @ingroup G_Q_LogConfig
*/
TAP_DLLEXPORT TAPIINT32 TAP_CDECL SetTapQuoteAPIDataPath(const TAPICHAR *path);
/**
* @brief	����API����־����
* @details	�趨��־���������ֻ�е�ʵ����־������˴��趨�ļ�����ͬ�����ʱ���ŻὫ��־д��SetTapQuoteAPIDataPath()�����򿪵���־�ļ���\n
* @param[in]	level ��־����\n
*					APILOGLEVEL_NONE	������¼��־\n
*					APILOGLEVEL_ERROR	����¼Error��־\n
*					APILOGLEVEL_WARNING	����¼Error��Warning��־\n
*					APILOGLEVEL_DEBUG	����¼Error��Warning��Debug��־\n
* @retval 0 �趨�ɹ�
* @retval ��0 ������
* @operationtype ͬ������
* @ingroup G_Q_LogConfig
*/
TAP_DLLEXPORT TAPIINT32 TAP_CDECL SetTapQuoteAPILogLevel(TAPILOGLEVEL level);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TAP_QUOTE_API_H