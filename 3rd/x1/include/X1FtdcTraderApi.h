/**
* Copyright (C) 2015-2017, �����ɴ���Ϣ�������޹�˾
* @file <X1FtdcTraderApi.h>
* @brief <����X1���׽ӿ�>
* @version <1.6.1.2>
* @author <X1��Ŀ��>
* @date <2018��9��5��>
*/

/**
 * @mainpage X1 UserAPI �û��ĵ�
 *
 *  X1��̨�û�API�����ֲ�
 *  Copyright (C) 2015-2017, �����ɴ���Ϣ�������޹�˾
 *
 */


#ifndef X1FTDCTRADERAPI_H_ 
#define X1FTDCTRADERAPI_H_

#include "X1FtdcApiStruct.h"

#ifdef WIN32
#ifdef X1FTDCAPI_EXPORTS
#define X1FTDCTRADERAPI_API __declspec(dllexport)
#else
#define X1FTDCTRADERAPI_API __declspec(dllimport)
#endif//X1FTDCAPI_EXPORTS
#else
#define X1FTDCTRADERAPI_API
#endif//WIN32

namespace x1ftdcapi {
    /**
    * @brief �û���Ӧ�ӿ�
    * @details ʵ�����¼�֪ͨ�ӿڣ��û���Ҫʵ�ִ���ӿڣ���д�¼��������������û�����Ȥ���¼���
    * @see Init
    */
    class CX1FtdcTraderSpi {
    public:
        /**
        * @brief    ��������������Ӧ
        * @details  ���ͻ����뽻�׺�̨�轨����ͨ������ʱ����δ��¼ǰ�����ͻ���API���Զ������ǰ�û�֮������ӣ�
        *           ��������ã����Զ��������ӣ������ø÷���֪ͨ�ͻ��ˣ� �ͻ��˿�����ʵ�ָ÷���ʱ������ʹ���ʽ��˺Ž��е�¼��
        *           ���÷�������Api��ǰ�û��������Ӻ󱻵��ã��õ��ý�����˵��tcp�����Ѿ������ɹ����û���Ҫ���е�¼���ܽ��к�����ҵ�������
        *           ����ʧ����˷������ᱻ���á���
        *
        */
        virtual void OnFrontConnected() {
        };

        /**
        * @brief �������Ӳ�������Ӧ
        * @details ���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ����豣֤���ӳɹ�������Ӧ�ĵ�¼�߼�����
        * @param    nReason ����ԭ��
        *           <ul>
        *               <li> 0x1001 ��ѯ���������ʧ��
        *               <li> 0x1002 ��ѯ��������дʧ��
        *               <li> 0x2002 ��ѯ����������ʧ��
        *               <li> 0x2003 ��ѯ�����յ�������
        *               <li> 0x3001 �������������ʧ��
        *               <li> 0x3002 ������������дʧ��
        *               <li> 0x4001 �����������������ʱ
        *               <li> 0x4002 ��������������ʧ��
        *               <li> 0x4003 ���������յ�������
        *           </ul>
        *
        */
        virtual void OnFrontDisconnected(int nReason) {
        };
        /**
        * @brief   ��½������Ӧ
        * @details ���û�������¼�����ǰ�û�������Ӧʱ�˷����ᱻ���ã�֪ͨ�û���¼�Ƿ�ɹ�����pUserLoginInfoRtn.loginResultΪ0ʱ��ʾ��¼�ɹ����ҵ�¼�ɹ�ʱ��pErrorInfoΪNULL������pErrorInfo�н���������ID�ʹ�����Ϣ���ɹ�ʱ���û�����ȡһ���ỰID,��SessionIDΪ1��ʱ��Ĭ��Ϊ�û���һ�ε�½.
        * @param pUserLoginInfoRtn �û���¼��Ϣ�ṹ��ַ��
        * @param pErrorInfo ����¼ʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        */
        virtual void OnRspUserLogin(struct CX1FtdcRspUserLoginField* pUserLoginInfoRtn, struct CX1FtdcRspErrorField* pErrorInfo) {
        };

        /**
        * @brief   �ǳ�������Ӧ:
        * @details ���û������˳������ǰ�û�������Ӧ�˷����ᱻ���ã�֪ͨ�û��˳�״̬��
        * @param pUserLogoutInfoRtn �����û��˳���Ϣ�ṹ��ַ��
        * @param pErrorInfo ���ǳ�ʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        * @warning ���û���¼���ش�����35,�û���release API �� ����API�����µ�¼
        */
        virtual void OnRspUserLogout(struct CX1FtdcRspUserLogoutInfoField* pUserLogoutInfoRtn, struct CX1FtdcRspErrorField* pErrorInfo) {
        };

        /**
        * @brief   �ڻ�ί�б�����Ӧ
        * @details ��������������ʱ��pErrorInfo��ΪNULL���������а����˴���ID��������Ϣ�������󱨵�ʱ��LocalOrderID�����ڶ�Ӧ�ͻ�����ı����������ɹ�ʱ���Ǳ�ʾX1̨ϵͳȷ���˸ñʱ������ñ���Ҳͬʱ�����˽�����������������δȷ�ϡ�
        * @param pOrderRtn �����û��µ���Ϣ�ṹ��ַ��
        * @param pErrorInfo ������ʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        */
        virtual void OnRspInsertOrder(struct CX1FtdcRspOperOrderField* pOrderRtn, struct CX1FtdcRspErrorField* pErrorInfo) {
        };

        /**
        * @brief   �ڻ�ί�г�����Ӧ
        * @details ���û�������ǰ�÷�����Ӧ�Ǹ÷����ᱻ���á�
        * @param pOrderCanceledRtn ���س�����Ӧ��Ϣ�ṹ��ַ��
        * @param pErrorInfo ������ʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        * @warning ������������ʱ����Ҫ���pErrorInfo�Ƿ�ΪNULL���ҿͻ����յ�����Ӧʱ��ֻ��ʾ��̨ȷ������ʳ�������
        */
        virtual void OnRspCancelOrder(struct CX1FtdcRspOperOrderField* pOrderCanceledRtn, struct CX1FtdcRspErrorField* pErrorInfo) {
        };

        /**
        * @brief   ����ر�
        * @details API�ڲ����߹�̨�ڲ������쳣����ͨ���˷���֪ͨ�û���
        *           API�ڲ����󣺰���API�ڴ������쳣��ϵͳ��Դ�����쳣�ȡ�
        *           ��̨�ڴ���󣺰���������API�ڲ��׳��쳣����̨�ڲ������쳣��ϵͳ��Դ�����쳣�ȡ�
        * @param pErrorInfo ������Ϣ�Ľṹ��ַ��
        */
        virtual void OnRtnErrorMsg(struct CX1FtdcRspErrorField* pErrorInfo) {
        };

        /**
        * @brief   �ɽ��ر�
        * @details ��ί�гɹ����׺�η����ᱻ���á�
        * @param pRtnMatchData ָ��ɽ��ر��Ľṹ��ָ�롣
        */
        virtual void OnRtnMatchedInfo(struct CX1FtdcRspPriMatchInfoField* pRtnMatchData) {
        };

        /**
        * @brief   ί�лر�
        * @details ���������յ�ί�����󲢷��ػر�ʱ���÷����ᱻ����
        * @param pRtnOrderData ָ��ί�лر���ַ��ָ�롣
        */
        virtual void OnRtnOrder(struct CX1FtdcRspPriOrderField* pRtnOrderData) {
        };

        /**
        * @brief  �����ر�
        * @details �������ɹ���÷����ᱻ���á�
        * @param pCancelOrderData ָ�򳷵��ر��ṹ�ĵ�ַ���ýṹ�������������Լ�������Ϣ��
        */
        virtual void OnRtnCancelOrder(struct CX1FtdcRspPriCancelOrderField* pCancelOrderData) {
        };

        /**
        * @brief  ��ѯ����ί����Ӧ
        * @details ���û�����ί�в�ѯ�󣬸÷����ᱻ���á�
        * @param pRtnOrderData ָ��ί�лر��ṹ�ĵ�ַ��
        * @param pErrorInfo ����ѯʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        * @param bIsLast    �����Ƿ������һ����Ӧ��Ϣ
        *                   <ul>
        *                       <li> 0 - ��
        *                       <li> 1 - ��
        *                   </ul>
        */
        virtual void OnRspQryOrderInfo(struct CX1FtdcRspOrderField* pRtnOrderData, struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief  ��ѯ���ճɽ���Ӧ
        * @details ���û������ɽ���ѯ��÷����ᱻ���á�
        * @param pRtnMatchData ָ��ɽ��ر��ṹ�ĵ�ַ��
        * @param pErrorInfo ����ѯʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        * @param bIsLast �����Ƿ������һ����Ӧ��Ϣ
        *                   <ul>
        *                       <li> 0 - ��
        *                       <li> 1 - ��
        *                   </ul>
        */
        virtual void OnRspQryMatchInfo(struct CX1FtdcRspMatchField* pRtnMatchData, struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief  �ֲֲ�ѯ��Ӧ
        * @details ���û������ֲֲ�ѯָ���ǰ�÷�����Ӧʱ�÷����ᱻ���á�
        * @param pPositionInfoRtn ���سֲ���Ϣ�ṹ�ĵ�ַ��
        * @param pErrorInfo ����ѯʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        * @param bIsLast �����Ƿ������һ����Ӧ��Ϣ
        *                   <ul>
        *                       <li> 0 - ��
        *                       <li> 1 - ��
        *                   </ul>
        */
        virtual void OnRspQryPosition(struct CX1FtdcRspPositionField* pPositionInfoRtn, struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief  �ͻ��ʽ��ѯ��Ӧ
        * @details ���û������ʽ��ѯָ���ǰ�÷�����Ӧʱ�÷����ᱻ���á�
        * @param pCapitalInfoRtn �����ʽ���Ϣ�ṹ�ĵ�ַ��
        * @param pErrorInfo ����ѯʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        * @param bIsLast �����Ƿ������һ����Ӧ��Ϣ
        *                   <ul>
        *                       <li> 0 - ��
        *                       <li> 1 - ��
        *                   </ul>
        */
        virtual void OnRspCustomerCapital(struct CX1FtdcRspCapitalField* pCapitalInfoRtn, struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief  ��������Լ��ѯ��Ӧ
        * @details ���û�������Լ��ѯָ���ǰ�÷�����Ӧʱ�÷����ᱻ���á�
        * @param pInstrumentData ���غ�Լ��Ϣ�ṹ�ĵ�ַ��
        * @param pErrorInfo ������Ϣ�ṹ�������ѯ���������򷵻ش�����Ϣ��
        * @param bIsLast �����Ƿ������һ����Ӧ��Ϣ
        *                   <ul>
        *                       <li> 0 - ��
        *                       <li> 1 - ��
        *                   </ul>
        */
        virtual void OnRspQryExchangeInstrument(struct CX1FtdcRspExchangeInstrumentField* pInstrumentData, struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief  ��ѯָ����Լ��Ӧ
        * @details ���û�����ָ����Լ��ѯָ���ǰ�÷�����Ӧʱ�÷����ᱻ���á�
        * @param pInstrument ����ָ����Լ��Ϣ�ṹ�ĵ�ַ��
        * @param pErrorInfo ����ѯʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        * @param bIsLast �����Ƿ������һ����Ӧ��Ϣ
        *                   <ul>
        *                       <li> 0 - ��
        *                       <li> 1 - ��
        *                   </ul>
        */
        virtual void OnRspQrySpecifyInstrument(struct CX1FtdcRspSpecificInstrumentField* pInstrument, struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief  ��ѯ�ֲ���ϸ��Ӧ
        * @details ���û�������ѯ�ֲ���ϸ��ǰ�÷�����Ӧʱ�÷����ᱻ���á�
        * @param pPositionDetailRtn ���سֲ���ϸ�ṹ�ĵ�ַ��
        * @param pErrorInfo ����ѯʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        * @param bIsLast �����Ƿ������һ����Ӧ��Ϣ
        *                   <ul>
        *                       <li> 0 - ��
        *                       <li> 1 - ��
        *                   </ul>
        */
        virtual void OnRspQryPositionDetail(struct CX1FtdcRspPositionDetailField* pPositionDetailRtn, struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief ������״̬��ѯ��Ӧ
        * @param pRspExchangeStatusData:ָ������״̬��ѯ��Ӧ��ַ��ָ�롣
        * @warning ���ղ����������źţ�������ѯ�ڻ���˾��άͬ�£��Ƿ��ڱ��̵������У�����<PushJYZT Value=��YES��/>��ѡ��
        */
        virtual void OnRspQryExchangeStatus(struct CX1FtdcRspExchangeStatusField * pRspExchangeStatusData) {
        };

        /**
          * @brief ������״̬֪ͨ
          * @details ֧�ֽ�����״̬֪ͨ��Ʒ��״̬֪ͨ����x1��̨����
          * @param pRtnExchangeStatusData ָ������״̬֪ͨ��ַ��ָ�롣
          */
        virtual void OnRtnExchangeStatus(struct CX1FtdcExchangeStatusRtnField * pRtnExchangeStatusData) {
        };

        /**
        * @brief �����̱�����Ӧ
        * @param pRspQuoteData:ָ�������̱�����Ӧ��ַ��ָ�롣
        * @param pErrorInfo:������ʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        */
        virtual void OnRspQuoteInsert(struct CX1FtdcQuoteRspField * pRspQuoteData, struct CX1FtdcRspErrorField * pErrorInfo) {
        };

        /**
        * @brief �����̱����ر�
        * @param pRtnQuoteData:ָ�������̱����ر���ַ��ָ�롣
        */
        virtual void OnRtnQuoteInsert(struct CX1FtdcQuoteRtnField * pRtnQuoteData) {
        };

        /**
        * @brief �����̳�����Ӧ
        * @param pRspQuoteCanceledData:ָ�������̳�����Ӧ��ַ��ָ�롣
        * @param pErrorInfo:������ʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        */
        virtual void OnRspQuoteCancel(struct CX1FtdcQuoteRspField * pRspQuoteCanceledData, struct CX1FtdcRspErrorField * pErrorInfo) {
        };

        /**
        * @brief �����̳����ر�
        * @param pRtnQuoteCanceledData:ָ�������̳����ر���ַ��ָ�롣
        */
        virtual void OnRtnQuoteCancel(struct CX1FtdcQuoteCanceledRtnField * pRtnQuoteCanceledData) {
        };

        /**
        * @brief �����̳ɽ��ر�
        * @param pRtnQuoteMatchedData:ָ�������̳ɽ��ر���ַ��ָ�롣
        */
        virtual void OnRtnQuoteMatchedInfo(struct CX1FtdcQuoteMatchRtnField * pRtnQuoteMatchedData) {
        };

        /**
        * @brief ����������Ӧ
        * @param pRspStripCancelOrderData:ָ������������Ӧ��ַ��ָ�롣
        * @param pErrorInfo:����������ʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        */
        virtual void OnRspCancelAllOrder(struct CX1FtdcCancelAllOrderRspField *pRspCancelAllOrderData, struct CX1FtdcRspErrorField * pErrorInfo) {
        };

        /**
        * @brief ѯ����Ӧ
        * @param pRspForQuoteData:ѯ��������Ӧ�ṹ��ַ��
        * @param pErrorInfo:����ѯʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        */
        virtual void OnRspForQuote(struct CX1FtdcForQuoteRspField * pRspForQuoteData, struct CX1FtdcRspErrorField * pErrorInfo) {
        };

        /**
        * @brief ѯ�ۻر�
        * @param pRtnForQuoteData:ѯ�ۻر��ṹ��ַ��
        */
        virtual void OnRtnForQuote(struct CX1FtdcForQuoteRtnField * pRtnForQuoteData) {
        };


        /**
        * @brief ѯ��ί�в�ѯ��Ӧ
        * @param pRtnQryForQuoteData:ָ��ѯ��ί�в�ѯ��Ӧ��ַ��ָ�롣
        * @param pErrorInfo:����ѯʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        * @param bIsLast:�����Ƿ������һ����Ӧ��Ϣ��0 -��   1 -�ǣ���
        */
        virtual void OnRspQryForQuote(struct CX1FtdcQryForQuoteRtnField * pRtnQryForQuoteData, struct CX1FtdcRspErrorField * pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief ��ѯ���ձ���ί����Ӧ
        * @param pRtnQuoteOrderData:ָ�򱨼۲�ѯ�ر��ṹ�ĵ�ַ��
        * @param pErrorInfo:����ѯʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        * @param bIsLast:�����Ƿ������һ����Ӧ��Ϣ��0 -��   1 -�ǣ���
        */
        virtual void OnRspQryQuoteOrderInfo(struct CX1FtdcQuoteOrderRtnField * pRtnQuoteOrderData, struct CX1FtdcRspErrorField * pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief ѯ��֪ͨ��ѯ��Ӧ
        * @param pRtnQryQuoteNoticeData:��ѯѯ��֪ͨ�ر��ṹ��ַ��
        * @param pErrorInfo:����ѯʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        * @param bIsLast:�����Ƿ������һ����Ӧ��Ϣ��0 -��   1 -�ǣ���
        */
        virtual void OnRspQryQuoteNotice(struct CX1FtdcQryQuoteNoticeRtnField * pRtnQryQuoteNoticeData, struct CX1FtdcRspErrorField * pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief ������Լ��ѯ��Ӧ:���û�����������Լ��ѯָ���ǰ�÷�����Ӧʱ�÷����ᱻ���á�
        * @param pAbiInstrumentData:����������Լ��Ϣ�ṹ�ĵ�ַ��
        * @param pErrorInfo:����ѯʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        * @param bIsLast:�����Ƿ������һ����Ӧ��Ϣ��0 -��   1 -�ǣ���
        */
        virtual void OnRspArbitrageInstrument(struct CX1FtdcAbiInstrumentRtnField * pAbiInstrumentData, struct CX1FtdcRspErrorField * pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief ������ѯ��֪ͨ�����趩�ģ������̿ͻ��Զ��յ�ѯ��֪ͨ
        * @param pForQuoteRspData:ָ��ѯ��֪ͨ�ر���ַ��ָ�롣
        */
        virtual void OnRtnForQuoteRsp(struct CX1FtdcQuoteSubscribeRtnField * pForQuoteRspData) {
        };

        /**
        * @brief ����������״̬֪ͨ
        * @param pExchangeConnectionStatusData:ָ����������״̬֪ͨ��ַ��ָ�롣
        */
        virtual void OnNtyExchangeConnectionStatus(CX1FtdcExchangeConnectionStatusRtnField *pExchangeConnectionStatusData) {
        };

        /**
        * @brief ����������Ӧ:���û�������������ָ���ǰ�÷�����Ӧʱ�÷����ᱻ���á�
        * @param pResetPasswordData:ָ������������Ӧ��ָ�롣
        * @param pErrorInfo:����������ʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        */
        virtual void OnRspResetPassword(struct CX1FtdcRspResetPasswordField * pResetPasswordData, struct CX1FtdcRspErrorField* pErrorInfo) {
        };

        /**
        * @brief ��ѯ�����ֲ���ϸ��Ӧ:���û�������ѯ�ֲ���ϸ��ǰ�÷�����Ӧʱ�÷����ᱻ���á�
        * @param pPositionDetailRtn:���سֲ���ϸ�ṹ�ĵ�ַ��
        * @param pErrorInfo:����ѯʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        * @param bIsLast:�����Ƿ������һ����Ӧ��Ϣ��0 -��   1 -�ǣ���
        */
        virtual void OnRspQryArbitrageCombineDetail(struct CX1FtdcArbitrageCombineDetailRtnField * pPositionDetail, struct CX1FtdcRspErrorField * pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief ������ȷ����Ӧ:���ڽ��ս�������Ϣ��
        * @param pTradingDayRtnData: ���ؽ���������ȷ����Ӧ�ṹ�ĵ�ַ��
        */
        virtual void OnRspTradingDay(struct CX1FtdcTradingDayRtnField * pTradingDayRtnData) {
        };

        /**
        * @brief ��Ȩ�Գ���Ӧ:���û�������Ȩ�Գ������ǰ�÷�����Ӧʱ�÷����ᱻ���á�
        * @param pOptOffsetRtn: ������Ȩ�Գ�����ȷ����Ӧ�ṹ�ĵ�ַ��
        * @param pErrorInfo:������ʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        */
        virtual void OnRspOptOffset(struct CX1FtdcRspOptOffsetField * pOptOffsetRtn, struct CX1FtdcRspErrorField * pErrorInfo) {
        };

        /**
        * @brief ��Ȩ�Գ�����ر�
        * @details ���������յ���Ȩ�Գ����󲢷��ػر�ʱ���÷����ᱻ����
        * @param pRtnOptOffset ָ����Ȩ�Գ�ر���ַ��ָ�롣
        */
        virtual void OnRtnOptOffset(struct CX1FtdcRtnOptOffsetField * pRtnOptOffset) {
        };

        /**
        * @brief ������Ȩ�Գ���Ӧ:���û�����������Ȩ�Գ������ǰ�÷�����Ӧʱ�÷����ᱻ���á�
        * @param pCancelOptOffsetRtn: ���س�����Ȩ�Գ�����ȷ����Ӧ�ṹ�ĵ�ַ��
        * @param pErrorInfo:������ʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        */
        virtual void OnRspCancelOptOffset(struct CX1FtdcRspOptOffsetField* pCancelOptOffsetRtn, struct CX1FtdcRspErrorField*pErrorInfo) {
        };

        /**
        * @brief ������Ȩ�Գ�����ر�
        * @details ���������յ�������Ȩ�Գ����󲢷��ػر�ʱ���÷����ᱻ����
        * @param pRtnCancelOptOffset ָ����Ȩ�Գ�ر���ַ��ָ�롣
        */
        virtual void OnRtnCancelOptOffset(struct CX1FtdcRtnOptOffsetField * pRtnCancelOptOffset) {
        };

        /**
        * @brief ��ѯ��Ȩ�Գ���Ӧ:���û�������ѯ��Ȩ�Գ��ǰ�÷�����Ӧʱ�÷����ᱻ���á�
        * @param pRtnOptOffset:������Ȩ�Գ�ṹ�ĵ�ַ��
        * @param pErrorInfo:����ѯʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        * @param bIsLast:�����Ƿ������һ����Ӧ��Ϣ��0 -��   1 -�ǣ���
        */
        virtual void OnRspQryOptOffset(struct CX1FtdcRspOptOffsetField * pRtnOptOffset, struct CX1FtdcRspErrorField * pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief ��Լ�Գ���Ӧ:���û�������Լ�Գ������ǰ�÷�����Ӧʱ�÷����ᱻ���á�
        * @param pPerformOffsetRtn: ������Լ�Գ�����ȷ����Ӧ�ṹ�ĵ�ַ��
        * @param pErrorInfo:������ʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        */
        virtual void OnRspPerformOffset(struct CX1FtdcRspPerformOffsetField * pPerformOffsetRtn, struct CX1FtdcRspErrorField * pErrorInfo) {
        };

        /**
        * @brief ��Լ�Գ�����ر�
        * @details ���������յ���Լ�Գ����󲢷��ػر�ʱ���÷����ᱻ����
        * @param pRtnPerformOffset ָ����Լ�Գ�ر���ַ��ָ�롣
        */
        virtual void OnRtnPerformOffset(struct CX1FtdcRtnPerformOffsetField * pRtnPerformOffset) {
        };

        /**
        * @brief ������Լ�Գ���Ӧ:���û�����������Լ�Գ������ǰ�÷�����Ӧʱ�÷����ᱻ���á�
        * @param pCancelPerformOffsetRtn: ���س�����Լ�Գ�����ȷ����Ӧ�ṹ�ĵ�ַ��
        * @param pErrorInfo:������ʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        */
        virtual void OnRspCancelPerformOffset(struct CX1FtdcRspPerformOffsetField * pCancelPerformOffsetRtn, struct CX1FtdcRspErrorField * pErrorInfo) {
        };

        /**
        * @brief ������Լ�Գ�����ر�
        * @details ���������յ�������Լ�Գ����󲢷��ػر�ʱ���÷����ᱻ����
        * @param pRtnCancelPerformOffset ָ������Լ�Գ�ر���ַ��ָ�롣
        */
        virtual void OnRtnCancelPerformOffset(struct CX1FtdcRtnPerformOffsetField * pRtnCancelPerformOffset) {
        };

        /**
        * @brief ��ѯ��Լ�Գ���Ӧ:���û�������ѯ��Լ�Գ��ǰ�÷�����Ӧʱ�÷����ᱻ���á�
        * @param pPerformOffsetRtn:������Լ�Գ�ṹ�ĵ�ַ��
        * @param pErrorInfo:����ѯʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        */
        virtual void OnRspQryPerformOffset(struct CX1FtdcRspPerformOffsetField * pPerformOffsetRtn, struct CX1FtdcRspErrorField * pErrorInfo) {
        };

        /**
        * @brief ���/����������Ӧ:���û��������/�������������ǰ�÷�����Ӧʱ�÷����ᱻ���á�
        * @param pCombPosiRtn: �������/����������Ӧ�ṹ�ĵ�ַ��
        * @param pErrorInfo:������ʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        */
        virtual void OnRspCombPosi(struct CX1FtdcRspCombPosiField * pCombPosiRtn, struct CX1FtdcRspErrorField * pErrorInfo) {
        };

        /**
        * @brief ���/��������ر�
        * @details ���������յ����/�������벢���ػر�ʱ���÷����ᱻ����
        * @param pRtnCombPosi ָ�����/�����ر���ַ��ָ�롣
        */
        virtual void OnRtnCombPosi(struct CX1FtdcRtnCombPosiField * pRtnCombPosi) {
        };

        /**
        * @brief ��ѯ��ϳֲ���Ӧ:���û�������ѯ��ϳֲֲ�ѯ�����ǰ�÷�����Ӧʱ�÷����ᱻ���á�
        * @param pQryCombPosiRtn: ���ز�ѯ��ϳֲ���Ӧ�ṹ�ĵ�ַ��
        * @param pErrorInfo:������ʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        */
        virtual void OnRspQryCombPosition(struct CX1FtdcRspCombPositionField * pQryCombPosiRtn, struct CX1FtdcRspErrorField * pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief ��ѯ��Ϻ�Լ��Ӧ
        * @param pQryCombInstrumentRtn:��ѯ��Ϻ�Լ��Ӧ�ṹ��ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ�� -���� -����쳣��
        */
        virtual void OnRspQryCombInstrument(struct CX1FtdcRspCombInstrumentField * pQryCombInstrumentRtn, struct CX1FtdcRspErrorField * pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief �����������Զ���Ȩ��Ӧ:���û����������������Զ���Ȩ�����ǰ�÷�����Ӧʱ�÷����ᱻ���á�
        * @param pAutoExecAbandRtn: ���ط����������Զ���Ȩ��Ӧ�ṹ�ĵ�ַ��
        * @param pErrorInfo:������ʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        */
        virtual void OnRspAutoExecAband(struct CX1FtdcRspAutoExecAbandField * pAutoExecAbandRtn, struct CX1FtdcRspErrorField * pErrorInfo) {
        };

        /**
        * @brief �����������Զ���Ȩ�ر�
        * @details ���������յ������������Զ���Ȩ�����ػر�ʱ���÷����ᱻ���á�
        * @param pRtnAutoExecAband ָ�����������Զ���Ȩ�ر���ַ��ָ�롣
        */
        virtual void OnRtnAutoExecAband(struct CX1FtdcRtnAutoExecAbandField * pRtnAutoExecAband) {
        };

        /**
        * @brief ���������������Զ���Ȩ��Ӧ:���û��������������������Զ���Ȩ�����ǰ�÷�����Ӧʱ�÷����ᱻ���á�
        * @param pCancelAutoExecAbandRtn: ���س��������������Զ���Ȩ����ȷ����Ӧ�ṹ�ĵ�ַ��
        * @param pErrorInfo:������ʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        */
        virtual void OnRspCancelAutoExecAband(struct CX1FtdcRspAutoExecAbandField * pCancelAutoExecAbandRtn, struct CX1FtdcRspErrorField * pErrorInfo) {
        };

        /**
        * @brief ���������������Զ���Ȩ�ر�
        * @details ���������յ����������������Զ���Ȩ���󲢷��ػر�ʱ���÷����ᱻ����
        * @param pRtnCancelAutoExecAbandset ָ���������������Զ���Ȩ�ر���ַ��ָ�롣
        */
        virtual void OnRtnCancelAutoExecAband(struct CX1FtdcRtnAutoExecAbandField * pRtnCancelAutoExecAband) {
        };

        /*
        * @brief ��ѯ�����������Զ���Ȩ��Ӧ:
        * @param pQryAutoExecAbandRtn: ���ز�ѯ�����������Զ���Ȩ��Ӧ�ṹ�ĵ�ַ��
        * @param pErrorInfo:������ʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        */
        virtual void OnRspQryAutoExecAband(struct CX1FtdcRspAutoExecAbandField * pQryAutoExecAbandRtn, struct CX1FtdcRspErrorField * pErrorInfo, bool bIsLast) {
        };


        /**
        * �м��û��ɼ���Ϣ�ϱ���Ӧ:���м��û������ύ�û���Ϣ�����ǰ�û�������Ӧʱ�˷����ᱻ���ã�֪ͨ�û��ϴ��û���Ϣ�Ƿ�ɹ���
        * @param pUserSystemInfoRsp:�û��ɼ���Ϣ������Ϣ�ṹ��ַ��
        * @param pErrorInfo:���ϴ�ʧ�ܣ����ش�����Ϣ��ַ���ýṹ���д�����Ϣ��
        */
        virtual void OnRspSubmitUserSystemInfo(struct CX1FtdcRspSubmitUserSystemInfoField * pUserSystemInfoRsp, struct CX1FtdcRspErrorField * pErrorInfo) {
        };

    };//end of CX1FtdcTraderSpi

    /**
    * @brief �û�����ӿ�
    * @details ʵ�����û�����ӿڣ��û�ʵ��������󣬿���ֱ�ӵ��ýӿڽ��е�¼����������������ѯ�Ȳ���
    * @warning
    *         <ul>
    *           <li> �û����ýӿ�ǰ����Ҫ����memset��սṹ�����ݣ�����д��Ҫ���ֶ���Ϣ
    *           <li> �ӿ��̰߳�ȫ
    *           <li> ��������ƽ��Ͷ�����������͡�����������Щ�ֶ���Ϊʹ����λ������API�˲�����ֵ��Ч�Լ���жϣ�����ͷ�ļ�ָ����ֵ���룬��������ֵҲ�ᱻת��Ϊ��ȷ����ֵ
    *           <li> ������ʾ�ӿڵ�ί����Ӧ���п�������ί�лر�����
    *         </ul>
    * @see Init
    */
    class X1FTDCTRADERAPI_API CX1FtdcTraderApi {
    public:
        CX1FtdcTraderApi();
        virtual ~CX1FtdcTraderApi();
    public:
        /**
        * @brief ��̬����������һ��apiʵ��
        * @return ����һ��ָ��CX1FtdcTraderApiʵ����ָ��
        * @see Init
        */
        static CX1FtdcTraderApi * CreateCX1FtdcTraderApi(void);

        /**
        * @brief �ͷ�APIʵ��
        * @warning ����CreateCX1FtdcTraderApi�ɹ�����һ��APIʵ��,init�ɹ����û������˳�ʱ�������Release�ӿڣ�������ܻ�coredump
        */
        virtual void Release(void) = 0;

        /**
        * @brief ��ʼ���ӿ�
        * @details �ͷ���������socket���ӣ�������һ�������̣߳� ͬʱ�÷���ע��һ���ص�������
        * @param pszFrontAddr ����ǰ�������ַ��
        *                     �����ַ�ĸ�ʽΪ:"protocol://ipaddress:port",��"tcp://172.21.200.103:10910"
        *                     ����protocol��ֵΪtcp��ʽ��
        *                     ipaddress��ʾ����ǰ�õ�IP,port��ʾ����ǰ�õĶ˿�
        * @param pSpi ָ��CX1FtdcTraderSpi�������(�ص�������)��ָ��
        * @param output_core ����̰߳󶨵�cpu core id. -1��ʾ����
        * @param input_core �����̰߳󶨵�cpu core id. -1��ʾ����.input_core output_core��֧�ֽ���һ������Ϊ-1������
        * @return ��ʼ�����
        *         <ul>
        *           <li> 0 - ��ʼ���ɹ�
        *           <li> 1 - ��ʼ��ʧ�� ��ʱ������Ҫ���OnRtnErrorMsg�����Ĵ�����Ϣ
        *         </ul>
        *         Initʧ��ԭ����ܣ�
        *               ��д��addr��ʽ����ȷ����addr�е�ip��ַ���˿ڲ���ȷ����ʹ�ý���API���ӵ�������ǰ�õĶ˿�
        *               �������⣬��telnet����ip��port������Ƿ�ͨ
        * @code Usage:
        *   x1ftdcapi::CX1FtdcTraderApi* pApi = x1ftdcapi::CX1FtdcTraderApi::CreateCX1FtdcTraderApi();
        *   MyTradeSpi spi(pApi);   // MyTradeSpi�Ǽ̳�CX1FtdcTraderSpi���������ʵ�����û���Ӧ�ӿ�
        *   int iret = pApi->Init("tcp://127.0.0.1:10700", &spi, 2, 4);
        *   if (iret != 0) {
        *       printf("init failed! error:%d\n", iret);
        *       return -1;
        *   }
        *
        *   //do something
        *
        *   pApi->Release();
        * @endcode
        */
        virtual int Init(char * pszFrontAddr, CX1FtdcTraderSpi * pSpi, int output_core = -1, int input_core = -1) = 0;

        /**
        * @brief �ȴ��ӿ��߳̽������С�
        * @return �߳��˳����롣
        *         <ul>
        *           <li> 0  - �߳��˳��ɹ�
        *           <li> -1 - �߳��˳�ʧ��
        *         </ul>
        */
        virtual int Join(void) = 0;

        /**
        * @brief ����˽����
        * @details ��Init ��������֮��ReqUserLogin ��������֮ǰ���á��������øýӿڣ����ղ���˽����
        * @param priflow_req_flag ˽�������ķ�ʽ
        *         <ul>
        *           <li> 0 - ֻ���͵�¼��˽����������
        *           <li> 1 - �ӱ������տ�ʼ�ش�
        *           <li> 2 - ���ϴ��յ�������
        *           <li> 3 - ��ָ����ſ�ʼ����
        *         </ul>
        * @param pri_no ָ��˽�������(��ָ����Ŵ�����˽����) ��priflow_req_flagȡֵ3ʱ���ò�������Ч���������������0ֵ���ɡ�ע��: �û��յ���˽������ſ��ܲ�����
        */
        virtual void SubscribePrivateTopic(int priflow_req_flag, unsigned int pri_no) = 0;

        /**
        * @brief ������ȳɽ���Ϣ���ݲ�֧�֣� 
        */
        virtual void SetMatchInfoAdvanced(bool matchinfo_advanced) = 0;

        /**
        * @brief �û�������¼����
        * @param pUserLoginData ָ���û���¼����ṹ�ĵ�ַ��
        * @return ����ֵ
        *         <ul>
        *           <li> 0      -  �����ͳɹ�
        *           <li> ����   -  ������
        *         </ul>
        */
        virtual int ReqUserLogin(struct CX1FtdcReqUserLoginField * pUserLoginData) = 0;

        /**
        * @brief �û������ǳ�����
        * @param pUserLogoutData ָ���û���¼����ṹ�ĵ�ַ��
        * @return ����ֵ
        *         <ul>
        *           <li> 0      -  �����ͳɹ�
        *           <li> ����   -  ������
        *         </ul>
        */
        virtual int ReqUserLogout(struct CX1FtdcReqUserLogoutField * pUserLogoutData) = 0;

        /**
        * @brief �û�����������������
        * @param pResetPasswordData ָ���û�������������ṹ�ĵ�ַ��
        * @return ����ֵ
        *         <ul>
        *           <li> 0      -  �����ͳɹ�
        *           <li> ����   -  ������
        *         </ul>
        */
        virtual int ReqResetPassword(struct CX1FtdcReqResetPasswordField * pResetPasswordData) = 0;

        /**
        * @brief �ڻ�ί�б�������
        * @param pInsertOrderData �û����󱨵���Ϣ�ṹ��ַ��
        * @return ����ֵ
        *         <ul>
        *           <li> 0      -  �����ͳɹ�
        *           <li> ����   -  ������
        *         </ul>
        */
        virtual int ReqInsertOrder(struct CX1FtdcInsertOrderField * pInsertOrderData) = 0;

        /**
        * @brief �ڻ���������
        * @details ����ṩ��̨ί�к�(��̨ί�кŴ���-1)����ֻʹ�ù�̨ί�кŴ���ֻ�е���̨ί�к�С��0ʱ����ʹ�ñ���ί�кŽ��г���
        * @param pCancelOrderData �û����󳷵���Ϣ�ṹ��ַ��
        * @return ����ֵ
        *         <ul>
        *           <li> 0      -  �����ͳɹ�
        *           <li> ����   -  ������
        *         </ul>
        */
        virtual int ReqCancelOrder(struct CX1FtdcCancelOrderField * pCancelOrderData) = 0;

        /**
        * @brief �ֲֲ�ѯ����
        * @details ���û���ṩ��Լ���룬���ѯȫ���ֲ���Ϣ��
        * @param pPositionData �û�����ֲ���Ϣ�ṹ��ַ��
        * @return ����ֵ
        *         <ul>
        *           <li> 0      -  �����ͳɹ�
        *           <li> -1     -  ������ʧ��
        *           <li> -2     -  ����쳣
        *         </ul>
        */
        virtual int ReqQryPosition(struct CX1FtdcQryPositionField * pPositionData) = 0;

        /**
        * @brief �ͻ��ʽ��ѯ����
        * @details �û���Ҫ���ýṹ�ĸ����ֶ�
        * @param pCapitalData �����ʽ��ѯ�ṹ��ַ��
        * @return ����ֵ
        *         <ul>
        *           <li> 0      -  �����ͳɹ�
        *           <li> ����   -  ������
        *         </ul>
        */
        virtual int ReqQryCustomerCapital(struct CX1FtdcQryCapitalField * pCapitalData) = 0;

        /**
        * @brief ��ѯ��������Լ�б�����������
        * @details ��ExchangeIDΪ��ʱ����ʾ��ѯ�������������к�Լ���룬Ҳ��ָ����ѯĳ�����������к�Լ������Ϣ�����ýӿڲ��ܲ�ѯ��������Լ����
        * @param pExchangeInstrumentData ��������Լ��ѯ�ṹ��ַ��
        * @return ����ֵ
        *         <ul>
        *           <li> 0      -  �����ͳɹ�
        *           <li> ����   -  ������
        *         </ul>
        */
        virtual int ReqQryExchangeInstrument(struct CX1FtdcQryExchangeInstrumentField * pExchangeInstrumentData) = 0;

        /**
        * @brief ����ί�в�ѯ����
        * @param pOrderData ����ί�в�ѯ�ṹ��ַ��
        * @return ����ֵ
        *         <ul>
        *           <li> 0      -  �����ͳɹ�
        *           <li> -1     -  ������ʧ��
        *           <li> -2     -  ����쳣
        *         </ul>
        */
        virtual int ReqQryOrderInfo(struct CX1FtdcQryOrderField * pOrderData) = 0;

        /**
        * @brief ���ճɽ���ѯ����
        * @param pMatchData ���ճɽ���ѯ�ṹ��ַ��
        * @return ����ֵ
        *         <ul>
        *           <li> 0      -  �����ͳɹ�
        *           <li> ����   -  ������
        *         </ul>
        */
        virtual int ReqQryMatchInfo(struct CX1FtdcQryMatchField * pMatchData) = 0;

        /**
        * @brief ָ����Լ��Ϣ��ѯ����
        * @details ExchangeID���Ǳ�������ѡ���ΪĿǰ�ڻ��г��ϣ�һ����Լ�����ڸ���������Ψһ���ڵġ�
        * @param pInstrument ָ����Լ��ѯ�ṹ��ַ��
        * @return ����ֵ
        *         <ul>
        *           <li> 0      -  �����ͳɹ�
        *           <li> ����   -  ������
        *         </ul>
        */
        virtual int ReqQrySpecifyInstrument(struct CX1FtdcQrySpecificInstrumentField * pInstrument) = 0;

        /**
        * @brief �ֲ���ϸ��ѯ����
        * @param pPositionDetailData:�ֲ���ϸ��ѯ�ṹ��ַ��
        * @return ����ֵ
        *         <ul>
        *           <li> 0      -  �����ͳɹ�
        *           <li> ����   -  ������
        *         </ul>
        */
        virtual int ReqQryPositionDetail(struct CX1FtdcQryPositionDetailField * pPositionDetailData) = 0;

        /**
        * @brief ������״̬��ѯ
        * @param pQryExchangeStatusData:������״̬��ѯ����ṹ��ַ��
        * @return ����ֵ
        *         <ul>
        *           <li> 0      -  �����ͳɹ�
        *           <li> ����   -  ������
        *         </ul>
        */
        virtual int ReqQryExchangeStatus(struct CX1FtdcQryExchangeStatusField *pQryExchangeStatusData) = 0;

        /**
        * @brief ��ѯ������������Լ�б�
        * @param pAbtriInstrumentData:������������Լ��ѯ�ṹ��ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ��  -���� -����쳣��
        */
        virtual int ReqQryArbitrageInstrument(struct CX1FtdcAbiInstrumentField * pAbtriInstrumentData) = 0;

        /**
        * @brief �����̱�������
        * @param pQuoteInsertOrderData:�����̱�������ṹ��ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ��  -���� -����쳣��
        */
        virtual int ReqQuoteInsert(struct CX1FtdcQuoteInsertField * pQuoteInsertOrderData) = 0;

        /**
        * @brief �����̳�������
        * @details ����˫��Ӧ�ۣ����ӿ�ֻ֧�ֳ�˫�ߣ���֧��ֻ����һ�ߵĲ���
        * @param pQuoteCancelOrderData:�����̳�������ṹ��ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ��  -���� -����쳣��
        */
        virtual int ReqQuoteCancel(struct CX1FtdcCancelOrderField * pQuoteCancelOrderData) = 0;

        /**
        * @brief ��������
        * @details ��ָ��Ŀǰֻ֧�ִ������������̱��۶����������������ݲ�֧�֣���ͨ�ڻ���Ȩ����Ҳ��֧��
        * @param pCancelAllOrderData:������������ṹ��ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ��  -���� -����쳣��
        */
        virtual int ReqCancelAllOrder(struct CX1FtdcCancelAllOrderField * pCancelAllOrderData) = 0;

        /**
        * @brief ѯ������
        * @param pForQuoteData:ѯ������ṹ��ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ��  -���� -����쳣��
        */
        virtual int ReqForQuote(struct CX1FtdcForQuoteField * pForQuoteData) = 0;

        /**
        * @brief ѯ��ί�в�ѯ����
        * @param pQryForQuoteData:ѯ��ί�в�ѯ����ṹ��ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ��  -���� -����쳣��
        */
        virtual int ReqQryForQuote(struct CX1FtdcQryForQuoteField * pQryForQuoteData) = 0;

        /**
        * @brief �����̱���ί�в�ѯ
        * @param pQuoteOrderData:�����̱���ί�в�ѯ�ṹ��ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ��  -���� -����쳣��
        */
        virtual int ReqQryQuoteOrderInfo(struct CX1FtdcQuoteOrderField * pQuoteOrderData) = 0;

        /**
        * @brief ѯ��֪ͨ��ѯ����
        * @param pQryQuoteNoticeData:��ѯѯ��֪ͨ����ṹ��ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ��  -���� -����쳣��
        */
        virtual int ReqQryQuoteNotice(struct CX1FtdcQryQuoteNoticeField * pQryQuoteNoticeData) = 0;

        /**
        * @brief �����ֲ���ϸ��ѯ����
        * @param pQryArbitrageCombineDetailData:��ѯ�����ֲ�����ṹ��ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ��  -���� -����쳣��
        */
        virtual int ReqQryArbitrageCombineDetail(struct CX1FtdcArbitrageCombineDetailField *pQryArbitrageCombineDetailData) = 0;

        /**
        * @brief �����ղ�ѯ����
        * @param pTradingDay:�����ղ�ѯ����ṹ��ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ�� -���� -����쳣��
        */
        virtual int ReqTradingDay(struct CX1FtdcTradingDayField * pTradingDay) = 0;
        
        /**
        * @brief ��Ȩ�Գ���������
        * @param pOptOffsetData:��Ȩ�Գ����ýṹ��ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ�� -���� -����쳣��
        */
        virtual int ReqOptOffset(struct CX1FtdcOptOffsetField * pOptOffsetData) = 0;

        /**
        * @brief ������Ȩ�Գ���������
        * @param pCancelOptOffsetData:������Ȩ�Գ����ýṹ��ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ�� -���� -����쳣��
        */
        virtual int ReqCancelOptOffset(struct CX1FtdcOptOffsetField * pCancelOptOffsetData) = 0;

        /**
        * @brief ��Ȩ�Գ��ѯ����
        * @param pQryOptOffsetData:��Ȩ�Գ��ѯ�ṹ��ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ�� -���� -����쳣��
        */
        virtual int ReqQryOptOffset(struct CX1FtdcQryOptOffsetField * pQryOptOffsetData) = 0;

        /**
        * @brief ��Լ�Գ�����
        * @param pPerformOffsetData:��Լ�Գ�ṹ��ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ�� -���� -����쳣��
        */
        virtual int ReqPerformOffset(struct CX1FtdcPerformOffsetField * pPerformOffsetData) = 0;

        /**
        * @brief ������Լ�Գ�����
        * @param pCancelPerformOffsetData:������Լ�Գ�ṹ��ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ�� -���� -����쳣��
        */
        virtual int ReqCancelPerformOffset(struct CX1FtdcPerformOffsetField * pCancelPerformOffsetData) = 0;

        /**
        * @brief ��Լ�Գ��ѯ����
        * @param pQryPerformOffsetData:��Լ�Գ��ѯ�ṹ��ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ�� -���� -����쳣��
        */
        virtual int ReqQryPerformOffset(struct CX1FtdcQryPerformOffsetField * pQryPerformOffsetData) = 0;

        /**
        * @brief ���/������������
        * @param pCombPosiData:���/��������ṹ��ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ�� -���� -����쳣��
        */
        virtual int ReqCombPosi(struct CX1FtdcCombPosiField * pCombPosiData) = 0;

        /**
        * @brief ��ѯ��ϳֲ�����
        * @param pQryCombPosiData:��ѯ��ϳֲֽṹ��ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ�� -���� -����쳣��
        */
        virtual int ReqQryCombPosition(struct CX1FtdcQryCombPositionField * pQryCombPosiData) = 0;

        /**
        * @brief ��ѯ��Ϻ�Լ����
        * @param pQryCombPosiData:��ѯ��Ϻ�Լ�ṹ��ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ�� -���� -����쳣��
        */
        virtual int ReqQryCombInstrument(struct CX1FtdcQryCombInstrumentField * pQryCombInstrumentData) = 0;

        /**
        * @brief �����������Զ���Ȩ����
        * @param pAutoExecAbandData:�����������Զ���Ȩ�����ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ�� -���� -����쳣��
        */
        virtual int ReqAutoExecAband(struct CX1FtdcAutoExecAbandField * pAutoExecAbandData) = 0;
        
        /**
        * @brief ���������������Զ���Ȩ����
        * @param pCancelAutoExecAbandData:���������������Զ���Ȩ�����ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ�� -���� -����쳣��
        */
        virtual int ReqCancelAutoExecAband(struct CX1FtdcAutoExecAbandField * pCancelAutoExecAbandData) = 0;

        /**
        * @brief ��ѯ�����������Զ���Ȩ����
        * @param pQryAutoExecAbandData:��ѯ�����������Զ���Ȩ�����ַ��
        * @return 0 - �����ͳɹ� -1 - ������ʧ�� -���� -����쳣��
        */
        virtual int ReqQryAutoExecAband(struct CX1FtdcQryAutoExecAbandField * pQryAutoExecAbandData) = 0;
        /**
        * @brief ��ȡ�汾��
        * @return �汾���ַ���
        */
        virtual const char * GetVersion() = 0;

        /**
        * @brief �û������ϱ���Ϣ����
        * @param pUserSystemData ָ���ϱ���Ϣ����ṹ�ĵ�ַ��
        * @return ����ֵ
        *         <ul>
        *           <li> 0      -  �����ͳɹ�
        *           <li> ����   -  ������
        *         </ul>
        */
        virtual int ReqSubmitUserSystemInfo(struct CX1FtdcReqSubmitUserSystemInfoField * pUserSystemData) = 0;

    };//end of CX1FtdcTraderApi
}
//end of namespace


#endif//X1FTDCTRADERAPI_H_

