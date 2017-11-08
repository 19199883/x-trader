/**
 * ��Ȩ����(C)2012-2016, �����ɴ���Ϣ�������޹�˾
 * �ļ����ƣ�DFITCL2Api.h
 * �ļ�˵��������level 2����API�ӿ�
 * ��ǰ�汾��1.1.2
 * ���ߣ�Datafeed��Ŀ��
 * �������ڣ�2017��2��28��
 */
#ifndef _DFITC_L2_API_H_
#define _DFITC_L2_API_H_

#ifdef WIN32 

#ifdef LEVEL2API_EXPORTS
#define DFITC_L2API_API __declspec(dllexport)
#else
#define DFITC_L2API_API __declspec(dllimport)
#endif

#else
#define DFITC_L2API_API

#endif

#include "DFITCL2ApiDataType.h"

namespace DFITC_L2 {
    class DFITCL2Spi
    {
    public:
        DFITCL2Spi() {}
        /**
        * ���ӳɹ���Ӧ:���û����ӳɹ��󣬴˷����ᱻ���á�
        */
        virtual void OnConnected() { }

        /**
        * ���ӶϿ���Ӧ:��api��level 2 serverʧȥ����ʱ�˷����ᱻ���á�api���Զ��������ӣ��ͻ��˿ɲ�������
        * /@param nReason ����ԭ��
        *        1 ������ʱ
        *        2 ����Ͽ�
        *        3 �յ�������
        *        4 �ಥ����ģʽ�£�����ʵ��ʱ֪ͨ�ͻ�
        *        5 ���ͱ��Ĺ���
        */
        virtual void OnDisconnected(int pReason) { }

        /**
        * ��¼������Ӧ:���û�������¼�����level 2 server������Ӧʱ�˷����ᱻ���ã�֪ͨ�û���¼�Ƿ�ɹ���
        * @param pErrorField:�û���¼������Ϣ��
        */
        virtual void OnRspUserLogin(struct ErrorRtnField * pErrorField) { }

        /**
        * �ǳ�������Ӧ:���û������ǳ������level 2 server������Ӧʱ�˷����ᱻ���ã�֪ͨ�û��ǳ��Ƿ�ɹ���
        * @param pErrorField:�û��ǳ�������Ϣ��
        */
        virtual void OnRspUserLogout(struct ErrorRtnField * pErrorField) { }

        /**
        * ����������Ӧ:���û��������������level 2 server������Ӧʱ�˷����ᱻ���ã�֪ͨ�û������Ƿ�ɹ���
        * @param pErrorField:�û����ķ�����Ϣ��
        */
        virtual void OnRspSubscribeMarketData(struct ErrorRtnField * pErrorField) { }

        /**
        * ȡ������������Ӧ:���û�����ȡ�����������level 2 server������Ӧʱ�˷����ᱻ���ã�֪ͨ�û�ȡ�������Ƿ�ɹ���
        * @param pErrorField:ȡ�����ķ�����Ϣ��
        */
        virtual void OnRspUnSubscribeMarketData(struct ErrorRtnField * pErrorField) { }

        /**
        * ȫ������������Ӧ:���û�����ȫ�����������level 2 server������Ӧʱ�˷����ᱻ���ã�֪ͨ�û�ȫ�������Ƿ�ɹ���
        * @param pErrorField:�û�ȫ�����ķ�����Ϣ��
        */
        virtual void OnRspSubscribeAll(struct ErrorRtnField * pErrorField) { }

        /**
        * ȡ��ȫ������������Ӧ:���û�����ȡ��ȫ�����������level 2 server������Ӧʱ�˷����ᱻ���ã�֪ͨ�û�ȡ��ȫ�������Ƿ�ɹ���
        * @param pErrorField:ȡ��ȫ�����ķ�����Ϣ��
        */
        virtual void OnRspUnSubscribeAll(struct ErrorRtnField * pErrorField) { }

        /**
        * �޸�����������Ӧ:���û������޸����������level 2 server������Ӧʱ�˷����ᱻ���ã�֪ͨ�û��޸������Ƿ�ɹ���
        * @param pErrorField:�޸����뷵����Ϣ��
        */
        virtual void OnRspModifyPassword(struct ErrorRtnField * pErrorField) { }

        /**
        * �������嵵���������ϢӦ��:�����������ɹ��������鷵��ʱ���÷����ᱻ���á�
        * @param pQuote:ָ���������嵵���������Ϣ�ṹ��ָ�룬�ṹ���а��������������Ϣ��
        */
        virtual void OnBestAndDeep(struct MDBestAndDeep * const pQuote) { }

        /**
        * ����������ϢӦ��:�����������ɹ��������鷵��ʱ���÷����ᱻ���á�
        * @param pQuote:����������Ϣ�ṹ��ָ�룬�ṹ���а��������������Ϣ��
        */
        virtual void OnArbi(struct MDBestAndDeep * const pQuote) { }

        /**
        * ���ż�λ��ʮ��ί��������ϢӦ��:�����������ɹ��������鷵��ʱ���÷����ᱻ���á�
        * @param pQuote:���ż�λ��ʮ��ί��������Ϣ�ṹ��ָ�룬�ṹ���а��������������Ϣ��
        */
        virtual void OnTenEntrust(struct MDTenEntrust * const pQuote) { }

        /**
        * ʵʱ�����������ϢӦ��:�����������ɹ��������鷵��ʱ���÷����ᱻ���á�
        * @param pQuote:ʵʱ�����������Ϣ�ṹ��ָ�룬�ṹ���а��������������Ϣ��
        */
        virtual void OnRealtime(struct MDRealTimePrice * const pQuote) { }

        /**
        * ��Ȩƽ����ί��������ϢӦ��:�����������ɹ��������鷵��ʱ���÷����ᱻ���á�
        * @param pQuote:��Ȩƽ����ί��������Ϣ�ṹ��ָ�룬�ṹ���а��������������Ϣ��
        */
        virtual void OnOrderStatistic(struct MDOrderStatistic * const pQuote) { }

        /**
        * �ּ�λ�ɽ�������ϢӦ��:�����������ɹ��������鷵��ʱ���÷����ᱻ���á�
        * @param pQuote:�ּ�λ�ɽ�������Ϣ�ṹ��ָ�룬�ṹ���а��������������Ϣ��
        */
        virtual void OnMarchPrice(struct MDMarchPriceQty * const pQuote) { }

        /**
        * ������ʧ��ϢӦ��:�����level 2 server������ʧ������������⣬�÷����ᱻ���á�
        */
        virtual void OnHeartBeatLost() { }

        virtual ~DFITCL2Spi(){}
    };

    class DFITC_L2API_API DFITCL2Api
    {
    public:
        /**
        * ��������APIʵ��
        * @return ��������UserApi
        */
        static DFITCL2Api *CreateDFITCMdApi();

        /**
        * ����һ��apiʵ����
        * @return 0 - �ɹ�; ��0 - ʧ��
        */
        static int DestoryDFITCMdApi(DFITCL2Api * & p);

        /**
        * �û�������½����
        * @param pReqUserLoginField:ָ���û���¼����ṹ�ĵ�ַ��
        * @return 0 - ��¼�ɹ�; ��0 - ʧ�ܡ�
        */
        virtual int ReqUserLogin(struct DFITCUserLoginField * pReqUserLoginField);

        /**
        * ����������������:ѡ��������鷽ʽ0Ϊudp��
        * @param pszSvrAddr:level 2 server�����ַ��
        *                  ��"tcp://127.0.0.1:10915"
        *                  127.0.0.1Ϊlevel 2 server��ַ
        *                  10915Ϊlevel 2 server����������tcp���ӵĶ˿�
        * @param pSpi:DFITCL2Spi����ʵ����ַ
        * @param RecvQuoteType:0Ϊudp�������飬1Ϊtcp�������飬2Ϊ�ಥ��������
        * @return 0 - �ɹ�; ��0 - ʧ�ܡ�
        */
        virtual int Connect(char * pszSvrAddr, DFITCL2Spi * pSpi, unsigned int RecvQuoteType = 0);

        /**
        * ��������:�÷�����������ĳ������ĳЩ��Լ��������
        * @param ppInstrumentID[]:ָ�����飬ÿ��ָ��ָ��һ����Լ.  
        * @param nCount:��Լ����
        * @return 0 - �ɹ�; ��0 - ʧ�ܡ�
        */
        virtual int SubscribeMarketData(char * ppInstrumentID[], int nCount);

        /**
        * �˶�����:�÷��������˶�ĳ��/ĳЩ��Լ��������
        * @param ppInstrumentID[]:ָ�����飬ÿ��ָ��ָ��һ����Լ.  
        * @param nCount:��Լ����
        * @return 0 - �ɹ�; ��0 - ʧ�ܡ�
        */
        virtual int UnSubscribeMarketData(char * ppInstrumentID[], int nCount);

        /**
        * ����ȫ����Լ����:�÷������Ͷ���ȫ����Լ����
        * @return 0 - �ɹ�; ��0 - ʧ�ܡ�
        */
        virtual int SubscribeAll();

        /**
        * ȡ������ȫ����Լ����:�÷�������ȡ������ȫ����Լ����
        * @return 0 - �ɹ�; ��0 - ʧ�ܡ�
        */
        virtual int UnSubscribeAll();

        /**
        * �û������ǳ�����
        * @param pReqUserLogoutField:ָ���û���¼����ṹ�ĵ�ַ��
        * @return 0 - �ǳ��ɹ�; ��0 - ʧ�ܡ�
        */
        virtual int ReqUserLogout(struct DFITCUserLogoutField * pReqUserLogoutField);

        /**
        * �û����������޸�����
        * @param pReqUserPasswdChangeField:ָ���û������޸���ṹ�ĵ�ַ��
        * @return 0 - �ǳ��ɹ�; ��0 - ʧ�ܡ�
        */
        virtual int ReqChangePassword(struct DFITCPasswdChangeField * pReqUserPasswdChangeField);

        virtual ~DFITCL2Api();
    protected:
        DFITCL2Api();
    };
}
// ��������������������api����
#define NEW_CONNECTOR()     DFITC_L2::DFITCL2Api::CreateDFITCMdApi()
// ���ٶ���
#define DELETE_CONNECTOR(p) DFITC_L2::DFITCL2Api::DestoryDFITCMdApi(p)


#endif
