/**
* Copyright (C) 2015-2016, �����ɴ���Ϣ�������޹�˾
* @file <X1FtdcApiStruct.h>
* @brief <����ӿ���������ݽӿ�>
* @version <1.3.2.3>
* @author <X1��Ŀ��>
* @date <2015��12��2��>
*/

#ifndef X1FTDCAPISTRUCT_H_
#define X1FTDCAPISTRUCT_H_

#include "X1FtdcApiDataType.h"

#define APISTRUCT

///������
struct APISTRUCT CX1FtdcTimeOutField {
    TX1FtdcRequestIDType                  lRequestID;                   ///< ����ID
};

/**
* <table border="3">
*      <tr> <th>����</th>                        <th>����</th>                     <th>����</th>                                      <th>�Ƿ����</th>               <th>ȡֵ��Χ˵��</th></tr>
*      <tr> <td>AccountID</td>                   <td>TX1FtdcAccountIDType</td>     <td>�ʽ��˻�ID</td>                                <td>M</td>                      <td></td> </tr>
*      <tr> <td>LocalOrderID</td>                <td>TX1FtdcLocalOrderIDType</td>  <td>����ί�к�</td>                                <td>M</td>                      <td>��APIʹ����ά������ͬһ���Ự�в����ظ������ұ������һ�δ�</td></tr>
*      <tr> <td>InstrumentID</td>                <td>TX1FtdcInstrumentIDType</td>  <td>��Լ����</td>                                  <td>M</td>                      <td>֧��Ŀǰ����4���ڻ������������к�Լ������������/֣������������Լ</td> </tr>
*      <tr> <td>InsertPrice</td>                 <td>TX1FtdcPriceType</td>         <td>�����۸�</td>                                  <td>M</td>                      <td>����������Ϊ�м�ʱ�����ֶβ�������</td></tr>
*      <tr> <td>OrderAmount</td>                 <td>TX1FtdcAmountType</td>        <td>��������</td>                                  <td>M</td>                      <td>�������0������</td></tr>
*      <tr> <td>TX1FtdcBuySellTypeType</td>      <td>BuySellType</td>              <td>������־</td>                                  <td>M</td>                      <td>1��2��</td></tr>
*      <tr> <td>TX1FtdcOpenCloseTypeType</td>    <td>OpenCloseType</td>            <td>��ƽ��־</td>                                  <td>M</td>                      <td>1���֣�2ƽ�֣�4ƽ��</td></tr>
*      <tr> <td>TX1FtdcSpeculatorType</td>       <td>Speculator</td>               <td>Ͷ������</td>                                  <td>M</td>                      <td>֧��Ͷ�����������ױ�</td></tr>
*      <tr> <td>TX1FtdcInsertType</td>           <td>InsertType</td>               <td>�Զ������</td>                                <td>M</td>                      <td>(Ĭ��Ϊ��ͨ����)(�˰汾����)</td></tr>
*      <tr> <td>TX1FtdcOrderTypeType</td>        <td>OrderType</td>                <td>��������</td>                                  <td>M</td>                      <td>֧���޼� ���мۣ���������Լ��֧���мۣ������޼۽��д���</td></tr>
*      <tr> <td>TX1FtdcOrderPropertyType</td>    <td>OrderProperty</td>            <td>������������</td>                              <td>M</td>                      <td>֧��None��FAK��FOK</td></tr>
*      <tr> <td>TX1FtdcInstrumentTypeType</td>   <td>InstrumentType</td>           <td>��Լ����</td>                                  <td>M</td>                      <td>��ѡֵ���ڻ�����Ȩ</td></tr>
*      <tr> <td>TX1FtdcAmountType</td>           <td>MinMatchAmount</td>           <td>��С�ɽ���</td>                                <td>M</td>                      <td></td></tr>
*      <tr> <td>TX1FtdcReservedType</td>         <td>ReservedType2</td>            <td>Ԥ���ֶ�2</td>                                 <td>NM</td>                     <td></td></tr>
*      <tr> <td>TX1FtdcCustomCategoryType</td>   <td>CustomCategory</td>           <td bgcolor="gray">�Զ������(�˰汾����)</td>     <td>BLK</td>                    <td>�Զ������(�˰汾����)</td></tr>
*      <tr> <td>TX1FtdcPriceType</td>            <td>ProfitLossPrice</td>          <td bgcolor="gray">ֹӯֹ��۸�(�˰汾����)</td>   <td>NM</td>                     <td>ֹӯֹ��۸�(�˰汾����)</td></tr>
* </table>
*/
///���󱨵���������(��������)
struct APISTRUCT CX1FtdcInsertOrderField {
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˻�
    TX1FtdcLocalOrderIDType               LocalOrderID;                 ///< ����ί�к�, ��APIʹ����ά������ͬһ���Ự�в����ظ�
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
    TX1FtdcPriceType                      InsertPrice;                  ///< �����۸�, ����������Ϊ�м�ʱ�����ֶβ�������
    TX1FtdcAmountType                     OrderAmount;                  ///< ��������
    TX1FtdcBuySellTypeType                BuySellType;                  ///< ������־
    TX1FtdcOpenCloseTypeType              OpenCloseType;                ///< ��ƽ��־
    TX1FtdcSpeculatorType                 Speculator;                   ///< Ͷ������, ֧��Ͷ�����������ױ�
    TX1FtdcInsertType                     InsertType;                   ///< �Զ������(Ĭ��Ϊ��ͨ����)(�˰汾����)
    TX1FtdcOrderTypeType                  OrderType;                    ///< ��������, ֧���޼� ���мۣ���������Լ��֧���мۣ������޼۽��д���
    TX1FtdcOrderPropertyType              OrderProperty;                ///< ������������, ֧��None��FAK��FOK
    TX1FtdcInstrumentTypeType             InstrumentType;               ///< ��Լ����, ��ѡֵ���ڻ�����Ȩ
    TX1FtdcAmountType                     MinMatchAmount;               ///< ��С�ɽ���
    TX1FtdcReservedType                   ReservedType2;                ///< Ԥ���ֶ�2
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcCustomCategoryType             CustomCategory;               ///< �Զ������(�˰汾����)
    TX1FtdcPriceType                      ProfitLossPrice;              ///< ֹӯֹ��۸�(�˰汾����)   
};


/**
* <table border="3">
*      <tr> <th>����</th>         <th>����</th>                     <th>����</th>                                   <th>�Ƿ����</th>   <th>ȡֵ��Χ˵��</th></tr>
*      <tr> <td>AccountID</td>    <td>TX1FtdcAccountIDType</td>     <td bgcolor="gray">�ʽ��˻�ID (�˰汾����)</td> <td>NM</td>         <td></td> </tr>
*      <tr> <td>X1OrderID</td>    <td>TX1FtdcX1OrderIDType</td>     <td>��̨ί�к�</td>                             <td>NM</td>         <td>1. X1OrderID��LocalOrderID������������һ��ֵ����<br>2. ���X1OrderID��LocalOrderID����ֵ�������ˣ���������ʹ��X1OrderID������ͬʱС�ڵ���0ʱ�����޷������ɹ���<br>3. ����̨��������Ҫʹ��X1OrderID���г���</td> </tr>
*      <tr> <td>LocalOrderID</td> <td>TX1FtdcLocalOrderIDType</td>  <td>����ί�к�</td>                             <td>NM</td>         <td>���ͻ���ʹ��API�ǳ����ٵ�½��ǰ�ú󣬴�ʱ��Ҫ��LocalOrderID �� SessionID���г���</td></tr>
*      <tr> <td>InstrumentID</td> <td>TX1FtdcInstrumentIDType</td>  <td bgcolor="red">��Լ����(�˰汾����)</td>     <td>NM</td>         <td></td></tr>
*      <tr> <td>RequestID</td>    <td>TX1FtdcRequestIDType</td>     <td>����ID</td>                                 <td>NM</td>         <td></td></tr>
*      <tr> <td>SessionID</td>    <td>TX1FtdcSessionIDType</td>     <td>�ỰID</td>                                 <td>NM</td>         <td></td></tr>
* </table>
*/
///������������
struct APISTRUCT CX1FtdcCancelOrderField {
    TX1FtdcAccountIDType                  AccountID;                   ///< �ʽ��˻�ID(�˰汾����)
    TX1FtdcX1OrderIDType                  X1OrderID;                   ///< ��̨ί�к�
    TX1FtdcLocalOrderIDType               LocalOrderID;                ///< ����ί�к�
    TX1FtdcInstrumentIDType               InstrumentID;                ///< ��Լ����(�˰汾����)
    TX1FtdcRequestIDType                  RequestID;                   ///< ����ID
    TX1FtdcSessionIDType                  SessionID;                   ///< �ỰID
};


///ί����Ӧ����
struct APISTRUCT CX1FtdcRspOperOrderField {
    TX1FtdcLocalOrderIDType               LocalOrderID;                 ///< ����ί�к�
    TX1FtdcX1OrderIDType                  X1OrderID;                    ///< ��̨ί�к�
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  ///< ί��״̬
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcPriceType                      Fee;                          ///< ������,���ֶν����µ�ʱʹ��(�˰汾����)
    TX1FtdcPriceType                      Margin;                       ///< ���ᱣ֤��,���ֶν����µ�ʱʹ��
    TX1FtdcCustomCategoryType             CustomCategory;               ///< �Զ������(�˰汾����)
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˻�ID(�˰汾����)
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����(�˰汾����)
    TX1FtdcSessionIDType                  SessionID;                    ///< �ỰID  
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ������(�˰汾����)
    TX1FtdcBuySellTypeType                BuySellType;                  ///< ����(�˰汾����)
    TX1FtdcOpenCloseTypeType              OpenCloseType;                ///< ��ƽ(�˰汾����)
    TX1FtdcInstrumentTypeType             InstrumentType;               ///< ��Լ����(�˰汾����)
    TX1FtdcSpeculatorType                 Speculator;                   ///< Ͷ�����(�˰汾����)
    TX1FtdcPriceType                      InsertPrice;                  ///< ί�м�(�˰汾����)
    TX1FtdcPriceType                      ProfitLossPrice;              ///< ֹӯֹ��۸�(�˰汾����)
    TX1FtdcAmountType                     MinMatchAmount;               ///< ��С�ɽ���(�˰汾����)
    TX1FtdcAmountType                     OrderAmount;                  ///< ί������(�˰汾����)
    TX1FtdcInsertType                     InsertType;                   ///< �Զ������
    TX1FtdcOrderTypeType                  OrderType;                    ///< ��������(�˰汾����)
    TX1FtdcOrderPropertyType              OrderProperty;                ///< ��������(�˰汾����)
    TX1FtdcClientIDType                   ClientID;                     ///< ���ױ���(�˰汾����)
};



///��ѯ�ʽ���������
struct APISTRUCT CX1FtdcQryCapitalField {
    TX1FtdcRequestIDType                  RequestID;                   ///< ����ID
    TX1FtdcAccountIDType                  AccountID;                   ///< �ʽ��˻�ID
};


///��ѯ�ֲ���������
struct APISTRUCT CX1FtdcQryPositionField {
    TX1FtdcRequestIDType                  RequestID;                   ///< ����ID
    TX1FtdcAccountIDType                  AccountID;                   ///< �ʽ��˻�ID
    TX1FtdcInstrumentIDType               InstrumentID;                ///< ��Լ����
    TX1FtdcInstrumentTypeType             InstrumentType;              ///< ��Լ����
};


///��������Լ
/**
 * @brief ��������Լ
 * @details ��ExchangeIDΪ��ʱ����ʾ��ѯ�������������к�Լ���룬Ҳ��ָ����ѯĳ�����������к�Լ������Ϣ�����ýӿڲ��ܲ�ѯ��������Լ����
 *
 */
struct APISTRUCT CX1FtdcQryExchangeInstrumentField {
    TX1FtdcRequestIDType                  RequestID;                   ///< ����ID
    TX1FtdcAccountIDType                  AccountID;                   ///< �ʽ��˻�ID
    TX1FtdcExchangeIDType                 ExchangeID;                  ///< ����������
    TX1FtdcInstrumentTypeType             InstrumentType;              ///< ��Լ����
};

/// �û���¼��������
struct APISTRUCT CX1FtdcReqUserLoginField {
    TX1FtdcRequestIDType                  RequestID;                   ///< ����ID [NM]
    TX1FtdcAccountIDType                  AccountID;                   ///< �ʽ��˻�ID [M]
    TX1FtdcPasswordType                   Password;                    ///< ���� [M]
    TX1FtdcCompanyIDType                  CompanyID;                   ///< ����ID(�˰汾����)[BLK]
};

/// �û���¼������Ϣ 
struct APISTRUCT CX1FtdcRspUserLoginField {
    TX1FtdcRequestIDType                  RequestID;                   ///< ����ID
    TX1FtdcAccountIDType                  AccountID;                   ///< �ʽ��ʺ�ID
    TX1FtdcAccountLoginResultType         LoginResult;                 ///< ��¼���
    TX1FtdcLocalOrderIDType               InitLocalOrderID;            ///< ��ʼ����ί�к�(�˰汾����)
    TX1FtdcSessionIDType                  SessionID;                   ///< sessionID(�ỰID)
    TX1FtdcErrorIDType                    ErrorID;                     ///< ����ID
    TX1FtdcErrorMsgInfoType               ErrorMsg;                    ///< ������Ϣ
    TX1FtdcTimeType                       DCEtime;                     ///< ������ʱ��(�˰汾����)
    TX1FtdcTimeType                       SHFETime;                    ///< ������ʱ��(�˰汾����)
    TX1FtdcTimeType                       CFFEXTime;                   ///< �н���ʱ��(�˰汾����)
    TX1FtdcTimeType                       CZCETime;                    ///< ֣����ʱ��(�˰汾����)
    TX1FtdcTimeType                       INETime;                     ///< ������ʱ��(�˰汾����)
};

///�û��˳�����
struct APISTRUCT CX1FtdcReqUserLogoutField {

    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID(�˰汾����) [BLK]
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��ʺ�ID [NM] �û����Դ��ջ�����ȷ�ĵ�¼��
    TX1FtdcSessionIDType                  SessionID;                    ///< �ỰID(�˰汾����)[BLK]
};


///ί�лر�
struct APISTRUCT CX1FtdcRspPriOrderField {
    TX1FtdcLocalOrderIDType               LocalOrderID;                 ///< ����ί�к� ��ϵͳ�����󣬾����ָ�����ȷ��׼ȷ
    TX1FtdcX1OrderIDType                  X1OrderID;                    ///< ��̨ί�к�
    TX1FtdcOrderSysIDType                 OrderSysID;                   ///< �������
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  ///< ί��״̬  ��FAK FOK������ؽ������Ѿ����ܣ�����δ�ɽ�8��ʧ�ܷ���7���������һ�ɷ���δ�ɽ�3
    TX1FtdcSessionIDType                  SessionID;                    ///< �ỰID
    TX1FtdcDateType                       SuspendTime;                  ///< ����ʱ��(�˰汾����)
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ������ 
    TX1FtdcBuySellTypeType                BuySellType;                  ///< ����
    TX1FtdcOpenCloseTypeType              OpenCloseType;                ///< ��ƽ
    TX1FtdcInstrumentTypeType             InstrumentType;               ///< ��Լ����
    TX1FtdcSpeculatorType                 Speculator;                   ///< Ͷ�����
    TX1FtdcPriceType                      InsertPrice;                  ///< ί�м�
    TX1FtdcPriceType                      ProfitLossPrice;              ///< ֹӯֹ��۸�(�˰汾����)
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˺�(�˰汾����)
    TX1FtdcAmountType                     CancelAmount;                 ///< ��������
    TX1FtdcAmountType                     OrderAmount;                  ///< ί������
    TX1FtdcInsertType                     InsertType;                   ///< �Զ������(�˰汾����)
    TX1FtdcOrderTypeType                  OrderType;                    ///< ��������(�˰汾����)
    TX1FtdcX1OrderIDType                  ExtX1OrderID;                 ///< �㷨�����(�˰汾����)
    TX1FtdcReservedType                   ReservedType2;                ///< Ԥ���ֶ�2(�˰汾����)
    TX1FtdcCustomCategoryType             CustomCategory;               ///< �Զ������(�˰汾����)
    TX1FtdcOrderPropertyType              OrderProperty;                ///< ��������(�˰汾����)
    TX1FtdcAmountType                     MinMatchAmount;               ///< ��С�ɽ���(�˰汾����)
    TX1FtdcClientIDType                   ClientID;                     ///< ���ױ���
    TX1FtdcErrorIDType                    ErrorID;                      ///< ����ID
    TX1FtdcErrorMsgInfoType               StatusMsg;                    ///< ״̬��Ϣ
    TX1FtdcExtOrderType                   ExtOrderType;                 ///< ����������(�˰汾����)
    TX1FtdcLocalOrderIDType               PriFlowNo;                    ///< ˽�������
};


///�ɽ��ر�
struct APISTRUCT CX1FtdcRspPriMatchInfoField {
    TX1FtdcLocalOrderIDType               LocalOrderID;                 ///< ����ί�к�
    TX1FtdcOrderSysIDType                 OrderSysID;                   ///< �������(�������������)
    TX1FtdcMatchIDType                    MatchID;                      ///< �ɽ����
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
    TX1FtdcBuySellTypeType                BuySellType;                  ///< ����
    TX1FtdcOpenCloseTypeType              OpenCloseType;                ///< ��ƽ��־
    TX1FtdcPriceType                      MatchedPrice;                 ///< �ɽ��۸�
    TX1FtdcAmountType                     OrderAmount;                  ///< ί������(�˰汾����)
    TX1FtdcAmountType                     MatchedAmount;                ///< �ɽ�����
    TX1FtdcDateType                       MatchedTime;                  ///< �ɽ�ʱ��
    TX1FtdcPriceType                      InsertPrice;                  ///< ����(�˰汾����)
    TX1FtdcX1OrderIDType                  X1OrderID;                    ///< ��̨ί�к�
    TX1FtdcMatchType                      MatchType;                    ///< �ɽ�����
    TX1FtdcSpeculatorType                 Speculator;                   ///< Ͷ��
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ������ID
    TX1FtdcFeeType                        Fee;                          ///< ������(�˰汾����)
    TX1FtdcSessionIDType                  SessionID;                    ///< �Ự��ʶ
    TX1FtdcInstrumentTypeType             InstrumentType;               ///< ��Լ����(�˰汾����)
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˺�(�˰汾����)
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  ///< �걨���(�˰汾����)
    TX1FtdcPriceType                      Margin;                       ///< ����Ϊ��֤��,ƽ��Ϊ�ⶳ��֤��(�˰汾����)
    TX1FtdcPriceType                      FrozenCapita;                 ///< �ɽ��ⶳί�ж�����ʽ�(�˰汾����)
    TX1FtdcAdjustmentInfoType             AdjustmentInfo;               ///< ��ϻ�����ı�֤�������Ϣ,��ʽ:[��Լ����,������־,Ͷ�����,�������;](�˰汾����) 
    TX1FtdcCustomCategoryType             CustomCategory;               ///< �Զ������(�˰汾����)
    TX1FtdcPriceType                      Turnover;                     ///< �ɽ����(�˰汾����)
    TX1FtdcOrderTypeType                  OrderType;                    ///< ��������(�˰汾����)
    TX1FtdcInsertType                     InsertType;                   ///< �Զ������(�˰汾����)
    TX1FtdcClientIDType                   ClientID;                     ///< ���ױ���(�˰汾����)
    TX1FtdcLocalOrderIDType               PriFlowNo;                    ///< ˽�������
};


///�����ر�
struct APISTRUCT CX1FtdcRspPriCancelOrderField {
    TX1FtdcLocalOrderIDType               LocalOrderID;                 ///< ����ί�к�
    TX1FtdcOrderSysIDType                 OrderSysID;                   ///< �������  
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����(�˰汾����)  
    TX1FtdcPriceType                      InsertPrice;                  ///< �����۸�(�˰汾����)
    TX1FtdcBuySellTypeType                BuySellType;                  ///< ��������(�˰汾����)
    TX1FtdcOpenCloseTypeType              OpenCloseType;                ///< ��ƽ��־(�˰汾����)
    TX1FtdcAmountType                     CancelAmount;                 ///< ��������
    TX1FtdcX1OrderIDType                  X1OrderID;                    ///< ��̨ί�к�
    TX1FtdcSpeculatorType                 Speculator;                   ///< Ͷ��(�˰汾����)
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ������ID(�˰汾����)
    TX1FtdcDateType                       CanceledTime;                 ///< ����ʱ��
    TX1FtdcSessionIDType                  SessionID;                    ///< �Ự��ʶ
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  ///< �걨���
    TX1FtdcInstrumentTypeType             InstrumentType;               ///< ��Լ����(�˰汾����)
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˺�(�˰汾����)
    TX1FtdcAmountType                     OrderAmount;                  ///< ί������(�˰汾����)   
    TX1FtdcPriceType                      Margin;                       ///< ��֤��(�˰汾����)
    TX1FtdcPriceType                      Fee;                          ///< ������(�˰汾����)
    TX1FtdcCustomCategoryType             CustomCategory;               ///< �Զ������(�˰汾����)
    TX1FtdcPriceType                      ProfitLossPrice;              ///< ֹӯֹ��۸�(�˰汾����)
    TX1FtdcAmountType                     MinMatchAmount;               ///< ��С�ɽ���(�˰汾����)
    TX1FtdcInsertType                     InsertType;                   ///< �Զ������(�˰汾����)
    TX1FtdcClientIDType                   ClientID;                     ///< ���ױ���(�˰汾����)
    TX1FtdcErrorIDType	                  ErrorID;                      ///< ����ID  2�µ�֮ǰ���
    TX1FtdcErrorMsgInfoType               StatusMsg;                    ///< ״̬��Ϣ
    TX1FtdcOrderPropertyType              OrderProperty;                ///< ������������(�˰汾����)
    TX1FtdcLocalOrderIDType               PriFlowNo;                    ///< ˽�������
};


///������Ϣ
struct APISTRUCT CX1FtdcRspErrorField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcSessionIDType                  SessionID;                    ///< �Ự��ʶ
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˺�
    TX1FtdcErrorIDType                    ErrorID;                      ///< ����ID
    TX1FtdcX1OrderIDType                  X1OrderID;                    ///< ��̨ί�к�
    TX1FtdcLocalOrderIDType               LocalOrderID;                 ///< ����ί�к�
    TX1FtdcErrorMsgInfoType               ErrorMsg;                     ///< ������Ϣ
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
};


///�����ʽ���Ϣ
struct APISTRUCT CX1FtdcRspCapitalField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��ʺ�
    TX1FtdcEquityType                     PreEquity;                    ///< ����Ȩ��
    TX1FtdcEquityType                     TodayEquity;                  ///< ���տͻ�Ȩ��
    TX1FtdcProfitLossType                 CloseProfitLoss;              ///< ƽ��ӯ��
    TX1FtdcProfitLossType                 PositionProfitLoss;           ///< �ֲ�ӯ��
    TX1FtdcProfitLossType                 FrozenMargin;                 ///< �����ʽ�
    TX1FtdcProfitLossType                 Margin;                       ///< �ֱֲ�֤��
    TX1FtdcProfitLossType                 Fee;                          ///< ����������
    TX1FtdcProfitLossType                 Available;                    ///< �����ʽ�
    TX1FtdcProfitLossType                 Withdraw;                     ///< ��ȡ�ʽ�
    TX1FtdcRiskDegreeType                 RiskDegree;                   ///< ���ն�
    TX1FtdcPremiumType                    TodayPremiumIncome;           ///< ����Ȩ��������
    TX1FtdcPremiumType                    TodayPremiumPay;              ///< ����Ȩ���𸶳�
    TX1FtdcPremiumType                    YesterdayPremium;             ///< ��Ȩ�����ո�
    TX1FtdcMarketValueType                OptMarketValue;               ///< ��Ȩ��ֵ
    TX1FtdcProfitLossType                 ProfitLoss;                   ///< ����ӯ��
    TX1FtdcProfitLossType                 TotalFundOut;                 ///< �ܳ���
    TX1FtdcProfitLossType                 TotalFundIn;                  ///< �����
    TX1FtdcRatioType                      UpFloathingRatio;             ///< ������������
    TX1FtdcRatioType                      DownFloathingRatio;           ///< ������������
    TX1FtdcRatioType                      AllocateRatio;                ///< �������
};


///���سֲ���Ϣ
struct APISTRUCT CX1FtdcRspPositionField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��ʺ�ID
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ����������
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ��
    TX1FtdcBuySellTypeType                BuySellType;                  ///< ����
    TX1FtdcPriceType                      OpenAvgPrice;                 ///< ���־���
    TX1FtdcPriceType                      PositionAvgPrice;             ///< �ֲ־���
    TX1FtdcAmountType                     PositionAmount;               ///< �ֲ���
    TX1FtdcAmountType                     TotalAvaiAmount;              ///< �ܿ���
    TX1FtdcAmountType                     TodayAvaiAmount;              ///< �����
    TX1FtdcAmountType                     LastAvaiAmount;               ///< �����
    TX1FtdcAmountType                     TodayAmount;                  ///< ���
    TX1FtdcAmountType                     LastAmount;                   ///< ���
    TX1FtdcAmountType                     TradingAmount;                ///< ƽ��ҵ���
    TX1FtdcProfitLossType                 DatePositionProfitLoss;       ///< ���гֲ�ӯ��
    TX1FtdcProfitLossType                 DateCloseProfitLoss;          ///< ����ƽ��ӯ��
    TX1FtdcProfitLossType                 Premium;                      ///< Ȩ����
    TX1FtdcProfitLossType                 ProfitLoss;                   ///< ����ӯ��
    TX1FtdcProfitLossType                 Margin;                       ///< ռ�ñ�֤��
    TX1FtdcSpeculatorType                 Speculator;                   ///< Ͷ�����
    TX1FtdcClientIDType                   ClientID;                     ///< ���ױ���
    TX1FtdcPriceType                      PreSettlementPrice;           ///< ������
    TX1FtdcInstrumentTypeType             InstrumentType;               ///< ��Լ����
    TX1FtdcAmountType                     YesterdayTradingAmount;       ///< ƽ��ҵ���
    TX1FtdcProfitLossType                 OptionValue;                  ///< ��Ȩ��ֵ
};

///�û��˳�������Ϣ
struct APISTRUCT CX1FtdcRspUserLogoutInfoField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˻�ID
    TX1FtdcAccountLogoutResultType        LogoutResult;                 ///< �˳����
    TX1FtdcErrorIDType                    ErrorID;                      ///< ����ID
    TX1FtdcErrorMsgInfoType               ErrorMsg;                     ///< ������Ϣ
};

///ָ���ĺ�Լ
struct APISTRUCT CX1FtdcQrySpecificInstrumentField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˻�ID
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ������ID
    TX1FtdcInstrumentTypeType             InstrumentType;               ///< ��Լ����
};

///��������Լ������Ϣ
struct APISTRUCT CX1FtdcRspExchangeInstrumentField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ����������
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
    TX1FtdcVarietyNameType                VarietyName;                  ///< Ʒ������
    TX1FtdcInstrumentTypeType             InstrumentType;               ///< ��Լ����
    TX1FtdcAmountType                     OrderTopLimit;                ///< �޼�ί������
    TX1FtdcAmountType                     MktOrderTopLimit;             ///< �м�ί������
    TX1FtdcPriceType                      ContractMultiplier;           ///< ��Լ����
    TX1FtdcPriceType                      MinPriceFluctuation;          ///< ��С�䶯��λ
    TX1FtdcInstrumentMaturityType         InstrumentMaturity;           ///< ��Լ�������
    TX1FtdcPriceType                      UpperLimitPrice;              ///< ��ͣ���
    TX1FtdcPriceType                      LowerLimitPrice;              ///< ��ͣ���
    TX1FtdcPriceType                      PreClosePrice;                ///< ������
    TX1FtdcPriceType                      PreSettlementPrice;           ///< ������
    TX1FtdcPriceType                      SettlementPrice;              ///< �����
    TX1FtdcAmountType                     PreOpenInterest;              ///< ��ֲ���
    TX1FtdcInstrumentPrefixType           InstrumentPrefix;             ///< ��Լǰ׺
    TX1FtdcInstrumenExpirationDateType    InstrumentExpiration;         ///< ��Լ������
    TX1FtdcInstrumentIDType               Underlying;                   ///< ��Ȩ��Ӧ�ı�ĺ�Լ����
    TX1FtdcOptionTypeType                 OptionType;                   ///< ��Ȩ����
    TX1FtdcPriceType                      StrikePrice;                  ///< ִ�м۸�
    TX1FtdcRiskDegreeType                 ExchangeRiskDegree;           ///< ��������Ȩ��ͱ��Ϸ���ϵ��
    TX1FtdcPriceType                      MinMargin;                    ///< ��λ(��)��Ȩ��Լ��С��֤��
    TX1FtdcAmountType                     TradeSize;                    ///< ��Ȩ���ֵ�λ
};


///ί�в�ѯ���ݽṹ
/**
 * @brief ί�в�ѯ���ݽṹ
 * @details �����зḻ�Ĳ�ѯ�������ã�������ѯ����ί�м�¼����ֻ��Ҫ����AccountID��InstrumentType���ɡ�
 *          ��ѯ����ĳ״̬�ı�����������Ҫ��OrderStatus����Ϊ��Ӧ����״̬��ֵ���ɣ���3��ʾδ�ɽ��ڶ��еĵ��ӡ�(ע��OrderStatus��OrderType��ѯ�����ð汾�ݲ�֧��)
 *          ��ѯָ���������͵ĵ��ӣ����޼۵����м۵��ȡ�
 *          �ð汾��֧�ְ���̨ί�к�X1OrderID �򱾵�ί�к�LocalOrderID��ѯ��
 *          �ϵ���֧���µ���ί�в�ѯ��
 *          �ϵ��ļ۸����ʾΪ0.
 */
struct APISTRUCT CX1FtdcQryOrderField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˻�ID
    TX1FtdcInstrumentTypeType             InstrumentType;               ///< ��Լ����
    TX1FtdcCustomCategoryType             CustomCategory;               ///< �Զ������
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  ///< ί��״̬
    TX1FtdcOrderTypeType                  OrderType;                    ///< ��������
    TX1FtdcX1OrderIDType                  X1OrderID;                    ///< ��̨ί�к�
    TX1FtdcLocalOrderIDType               LocalOrderID;                 ///< ����ί�к�
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
};


///�ɽ���ѯ���ݽṹ
/**
 * @brief �ɽ���ѯ���ݽṹ
 * @details �����зḻ�Ĳ�ѯ�������ã�������ѯ����ί�м�¼����ֻ��Ҫ����AccountID��InstrumentType���ɡ���ע��OrderType��ѯ�����ð汾�ݲ�֧�֣�
 */
struct APISTRUCT CX1FtdcQryMatchField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˻�ID
    TX1FtdcInstrumentTypeType             InstrumentType;               ///< ��Լ����
    TX1FtdcCustomCategoryType             CustomCategory;               ///< �Զ������
    TX1FtdcOrderTypeType                  OrderType;                    ///< ��������
    TX1FtdcX1OrderIDType                  X1OrderID;                    ///< ��̨ί�к�
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
};


///ί�в�ѯ��Ӧ���ݽṹ
struct APISTRUCT CX1FtdcRspOrderField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcX1OrderIDType                  X1OrderID;                    ///< ��̨ί�к�
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  ///< ί��״̬
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
    TX1FtdcBuySellTypeType                BuySellType;                  ///< ����
    TX1FtdcOpenCloseTypeType              OpenClose;                    ///< ��ƽ��־
    TX1FtdcPriceType                      InsertPrice;                  ///< ί�м�
    TX1FtdcAmountType                     OrderAmount;                  ///< ί������
    TX1FtdcPriceType                      MatchedPrice;                 ///< �ɽ��۸�
    TX1FtdcAmountType                     MatchedAmount;                ///< �ɽ�����
    TX1FtdcAmountType                     CancelAmount;                 ///< ��������
    TX1FtdcInsertType                     InsertType;                   ///< �Զ������
    TX1FtdcSpeculatorType                 Speculator;                   ///< Ͷ��
    TX1FtdcDateType                       CommTime;                     ///< ί��ʱ��
    TX1FtdcDateType                       SubmitTime;                   ///< �걨ʱ��
    TX1FtdcClientIDType                   ClientID;                     ///< ���ױ���
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ������ID
    TX1FtdcFrontAddrType                  OperStation;                  ///< ί�е�ַ
    TX1FtdcAccountIDType                  AccountID;                    ///< �ͻ���
    TX1FtdcInstrumentTypeType             InstrumentType;               ///< ��Լ����
    TX1FtdcSessionIDType                  SessionId;                    ///< �ỰID(�˰汾��֧��)
    TX1FtdcReservedType                   ReservedType2;                ///< Ԥ���ֶ�2
    TX1FtdcOrderSysIDType                 OrderSysID;                   ///< �������
    TX1FtdcCustomCategoryType             CustomCategory;               ///< �Զ������
    TX1FtdcPriceType                      Margin;                       ///< ��֤��
    TX1FtdcPriceType                      Fee;                          ///< ������
    TX1FtdcLocalOrderIDType               LocalOrderID;                 ///< ����ί�к� ���µ�ʱ�õ��ӵı���ί�кţ����ͬһ�˺ŴӶ���ͻ����µ������ѯ���ص�LocalOrderID�������ظ���
    TX1FtdcPriceType                      ProfitLossPrice;              ///< ֹ��ֹӯ��
    TX1FtdcOrderTypeType                  OrderType;                    ///< �������
    TX1FtdcOrderPropertyType              OrderProperty;                ///< ��������
};


///�ɽ���ѯ������Ӧ
struct APISTRUCT CX1FtdcRspMatchField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcX1OrderIDType                  X1OrderID;                    ///< ��̨ί�к�
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˺�
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ������ID
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
    TX1FtdcBuySellTypeType                BuySellType;                  ///< ����
    TX1FtdcOpenCloseTypeType              OpenClose;                    ///< ��ƽ
    TX1FtdcPriceType                      MatchedPrice;                 ///< �ɽ��۸�
    TX1FtdcAmountType                     MatchedAmount;                ///< �ɽ�����
    TX1FtdcPriceType                      MatchedMort;                  ///< �ɽ����
    TX1FtdcSpeculatorType                 Speculator;                   ///< Ͷ�����
    TX1FtdcDateType                       MatchedTime;                  ///< �ɽ�ʱ��
    TX1FtdcMatchIDType                    MatchedID;                    ///< �ɽ����
    TX1FtdcLocalOrderIDType               LocalOrderID;                 ///< ����ί�к�
    TX1FtdcClientIDType                   ClientID;                     ///< ���ױ���
    TX1FtdcMatchType                      MatchType;                    ///< �ɽ�����
    TX1FtdcInstrumentTypeType             InstrumentType;               ///< ��Լ����
    TX1FtdcSessionIDType                  SessionId;                    ///< �ỰID(�˰汾��֧��)
    TX1FtdcReservedType                   ReservedType2;                ///< Ԥ���ֶ�2
    TX1FtdcCustomCategoryType             CustomCategory;               ///< �Զ������
    TX1FtdcPriceType                      Fee;                          ///< ������
    TX1FtdcOrderTypeType                  OrderType;                    ///< ��������
    TX1FtdcOrderSysIDType                 OrderSysID;                   ///< �������
};


///���غ�Լ��Ϣ���ݽṹ 
struct APISTRUCT CX1FtdcRspSpecificInstrumentField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ������
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
    TX1FtdcRatioType                      LongMarginRatio;              ///< ��ͷ��֤����
    TX1FtdcRatioType                      ShortMarginRatio;             ///< ��ͷ��֤����
    TX1FtdcPriceType                      LongMarginRatioByVolume;      ///< ��ͷ��֤���(����)
    TX1FtdcPriceType                      ShortMarginRatioByVolume;     ///< ��ͷ��֤���(����)
    TX1FtdcRatioType                      OpenFeeVolRatio;              ///< ���������� ����������
    TX1FtdcRatioType                      CloseFeeVolRatio;             ///< ƽ�������� ����������
    TX1FtdcRatioType                      CloseTodayFeeVolRatio;        ///< ƽ�������� ����������
    TX1FtdcRatioType                      OpenFeeAmtRatio;              ///< ������������ ��������
    TX1FtdcRatioType                      CloseFeeAmtRatio;             ///< ƽ���������� ��������
    TX1FtdcRatioType                      CloseTodayFeeAmtRatio;        ///< ƽ���������� ��������
    TX1FtdcAmountType                     OrderTopLimit;                ///< �޼�ί������
    TX1FtdcPriceType                      ContractMultiplier;           ///< ��Լ����
    TX1FtdcPriceType                      MinimumPriceChange;           ///< ��С�䶯��λ
    TX1FtdcInstrumentTypeType             InstrumentType;               ///< ��Լ����
    TX1FtdcInstrumentMaturityType         InstrumentMaturity;           ///< ��Լ�������
    TX1FtdcComputeModeType                ComputeMode;                  ///< ���㷽ʽ             
    TX1FtdcPriceType                      AtMoneyNorm;                  ///< ƽֵ������
    TX1FtdcPriceType                      UpperLimitPrice;              ///< ��ͣ���
    TX1FtdcPriceType                      LowerLimitPrice;              ///< ��ͣ���
    TX1FtdcPriceType                      PreClosePrice;                ///< ������
    TX1FtdcPriceType                      PreSettlementPrice;           ///< ������
    TX1FtdcPriceType                      SettlementPrice;              ///< �����
    TX1FtdcAmountType                     PreOpenInterest;              ///< ��ֲ���
    TX1FtdcRatioType                      OptExecRatio;                 ///< ��Ȩ����Ȩ������ �ڻ����������
    TX1FtdcRatioType                      OptExecRatioPerVol;           ///< ��Ȩ����Ȩ������ �ڻ����������
    TX1FtdcCalculateMode                  CalculateMode;                ///< �����ʽ���㷽ʽ
    TX1FtdcPriceType                      ReferencePrice;               ///< �ο��۸�
};

///��ѯ�ֲ���ϸ
struct APISTRUCT CX1FtdcQryPositionDetailField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˻�ID
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
    TX1FtdcInstrumentTypeType             InstrumentType;               ///< ��Լ����
};


///��ѯ�ֲ���ϸ��Ӧ
struct APISTRUCT CX1FtdcRspPositionDetailField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��ʺ�ID
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ����������
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ��
    TX1FtdcBuySellTypeType                BuySellType;                  ///< ����
    TX1FtdcPriceType                      OpenPrice;                    ///< ���ּ�
    TX1FtdcAmountType                     Volume;                       ///< ����
    TX1FtdcMatchIDType                    MatchID;                      ///< �ɽ����
    TX1FtdcDateType                       MatchedDate;                  ///< �ɽ�����
    TX1FtdcProfitLossType                 DatePositionProfitLoss;       ///< ���гֲ�ӯ��
    TX1FtdcProfitLossType                 DateCloseProfitLoss;          ///< ����ƽ��ӯ��
    TX1FtdcProfitLossType                 ProfitLoss;                   ///< ����ӯ��
    TX1FtdcProfitLossType                 Margin;                       ///< ռ�ñ�֤��
    TX1FtdcSpeculatorType                 Speculator;                   ///< Ͷ�����
    TX1FtdcClientIDType                   ClientID;                     ///< ���ױ���
    TX1FtdcPriceType                      PreSettlementPrice;           ///< ������
    TX1FtdcInstrumentTypeType             InstrumentType;               ///< ��Լ����
    TX1FtdcX1OrderIDType                  X1OrderID;                    ///< ��̨ί�к�
    TX1FtdcCustomCategoryType             CustomCategory;               ///< �Զ������
    TX1FtdcAmountType                     CloseOrderVol;                ///< ƽ��ί������
    TX1FtdcAmountType                     CloseMatchVol;                ///< ƽ�ֳɽ�����
    TX1FtdcPositionDateType               PositionDateType;             ///< �ֲ���������
};

///����״̬��ѯ����
struct APISTRUCT CX1FtdcQryExchangeStatusField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ����������
};

///������״̬��ѯ��Ӧ
struct APISTRUCT CX1FtdcRspExchangeStatusField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcExchangeStatusType             ExchangeStatus;               ///< ������״̬
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ����������
};

///������״̬֪ͨ
struct APISTRUCT CX1FtdcExchangeStatusRtnField {
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ������
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
    TX1FtdcExchangeStatusType             ExchangeStatus;               ///< ������״̬
};


///�����̱�������
struct APISTRUCT CX1FtdcQuoteInsertField {
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˺�
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcLocalOrderIDType               LocalOrderID;                 ///< ����ί�к�
    TX1FtdcInsertType                     InsertType;                   ///< �Զ������
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
    TX1FtdcQuoteIDType                    QuoteID;                      ///< ѯ�۱��
    TX1FtdcInstrumentTypeType             InstrumentType;               ///< ��Լ���� 
    TX1FtdcAmountType                     BuyOrderAmount;               ///< ��������(��)
    TX1FtdcAmountType                     SellOrderAmount;              ///< ��������(��)
    TX1FtdcPriceType                      BuyInsertPrice;               ///< ί�м۸�(��)         
    TX1FtdcPriceType                      SellInsertPrice;              ///< ί�м۸�(��)          
    TX1FtdcOpenCloseTypeType              BuyOpenCloseType;             ///< ��ƽ��־(��)  
    TX1FtdcOpenCloseTypeType              SellOpenCloseType;            ///< ��ƽ��־(��)
    TX1FtdcSpeculatorType                 BuySpeculator;                ///< Ͷ�����(��)       
    TX1FtdcSpeculatorType                 SellSpeculator;               ///< Ͷ�����(��)          
    TX1FtdcStayTimeType                   StayTime;                     ///< ͣ��ʱ�䣬��֧��֣�ݡ��������������Ϊ0 
    TX1FtdcCustomCategoryType             CustomCategory;               ///< �Զ������
};

///������������
struct APISTRUCT CX1FtdcCancelAllOrderField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˻�ID(�˰汾������)
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ����������(�˰汾������)
};

///ѯ������
struct APISTRUCT CX1FtdcForQuoteField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˻�ID
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
};

///�����̱�����Ӧ
struct APISTRUCT CX1FtdcQuoteRspField {
    TX1FtdcLocalOrderIDType               LocalOrderID;                 ///< ����ί�к�              
    TX1FtdcX1OrderIDType                  X1OrderID;                    ///< ��̨ί�к�         
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID                    
    TX1FtdcPriceType                      Fee;                          ///< ������(������ʹ��)(�˰汾������)          
    TX1FtdcPriceType                      Margin;                       ///< ��֤��(������ʹ��)
    TX1FtdcDateType                       OrderTime;                    ///< ί��ʱ��(������ʹ��)(�˰汾������)  
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  ///< ί��״̬
    TX1FtdcCustomCategoryType             CustomCategory;               ///< �Զ������(�˰汾������)
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����(�˰汾������)
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˺�(�˰汾������)
    TX1FtdcQuoteIDType                    QuoteID;                      ///< ѯ�۱��
    TX1FtdcSessionIDType                  SessionID;                    ///< �ỰID 
    TX1FtdcClientIDType                   ClientID;                     ///< ���ױ���(�˰汾������)
};

///�����̱����ر�
struct APISTRUCT CX1FtdcQuoteRtnField {
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ������                   
    TX1FtdcClientIDType                   ClientID;                     ///< ���ױ���  
    TX1FtdcOrderSysIDType                 OrderSysID;                   ///< �������
    TX1FtdcOrderSysIDType                 OrderSysIDTwo;                ///< ˫�ߵڶ���
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����                   
    TX1FtdcLocalOrderIDType               LocalOrderID;                 ///< ����ί�к�                
    TX1FtdcSeatCodeType                   SeatCode;                     ///< ϯλ����                  
    TX1FtdcOpenCloseTypeType              BuyOpenCloseType;             ///< ��ƽ��־(��)          
    TX1FtdcOpenCloseTypeType              SellOpenCloseType;            ///< ��ƽ��־(��)             
    TX1FtdcSpeculatorType                 Speculator;                   ///< Ͷ�����                       
    TX1FtdcAmountType                     BuyOrderAmount;               ///< ί������(��)            
    TX1FtdcAmountType                     SellOrderAmount;              ///< ί������(��)           
    TX1FtdcPriceType                      BuyInsertPrice;               ///< ί�м�  (��)               
    TX1FtdcPriceType                      SellInsertPrice;              ///< ί�м�  (��)               
    TX1FtdcX1OrderIDType                  X1OrderID;                    ///< ��̨ί�к� ���صĹ�̨ί�к�Ϊ��һ�ߵĹ�̨ί�кţ��ڶ��ߵĹ�̨ί�к�ʵ�����ǵ�һ���̨ί�кż�1������Ҫ�û����ġ�������������ָ�������̨ί�к�������˫�߳�������֧��ֻ��������һ�ߵĲ���                             
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˺�(�˰汾������)                     
    TX1FtdcInstrumentTypeType             InstrumentType;               ///< ��Լ����                  
    TX1FtdcDateType                       SuspendTime;                  ///< �ҵ�ʱ��(�˰汾������)                               
    TX1FtdcEntrusTellerType               EntrusTeller;                 ///< ί�й�Ա  
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  ///< ί��״̬ 
    TX1FtdcSessionIDType                  SessionID;                    ///< �ỰID
    TX1FtdcQuoteIDType                    QuoteID;                      ///< ѯ�۱��(�˰汾�����ã�Ԥ��)
    TX1FtdcErrorIDType                    ErrorID;                      ///< ����ID	2�µ�֮ǰ���
    TX1FtdcErrorMsgInfoType               StatusMsg;                    ///< ������Ϣ
    TX1FtdcCustomCategoryType             CustomCategory;               ///< �Զ������(�˰汾������)
    TX1FtdcLocalOrderIDType               PriFlowNo;                    ///<  ˽�������
};

///�����̳����ر�
struct APISTRUCT CX1FtdcQuoteCanceledRtnField {
    TX1FtdcExchangeIDType                 ExchangeID;                    ///< ������(�˰汾������)                   
    TX1FtdcClientIDType                   ClientID;                      ///< ���ױ���(�˰汾������)  
    TX1FtdcOrderSysIDType                 OrderSysID;                    ///< �������
    TX1FtdcInstrumentIDType               InstrumentID;                  ///< ��Լ����(�˰汾������)                   
    TX1FtdcLocalOrderIDType               LocalOrderID;                  ///< ����ί�к�                
    TX1FtdcSeatCodeType                   SeatCode;                      ///< ϯλ����                  
    TX1FtdcOpenCloseTypeType              BuyOpenCloseType;              ///< ��ƽ��־(��) (�˰汾������)         
    TX1FtdcOpenCloseTypeType              SellOpenCloseType;             ///< ��ƽ��־(��)(�˰汾������)             
    TX1FtdcSpeculatorType                 Speculator;                    ///< Ͷ�����(�˰汾������)                       
    TX1FtdcX1OrderIDType                  X1OrderID;                     ///< ��̨ί�к�                               
    TX1FtdcAccountIDType                  AccountID;                     ///< �ʽ��˺�(�˰汾������)                     
    TX1FtdcEntrusTellerType               EntrusTeller;                  ///< ί�й�Ա(�˰汾������)       
    TX1FtdcOrderAnswerStatusType          OrderStatus;                   ///< ί��״̬ 
    TX1FtdcAmountType                     CancelAmount;                  ///< ��������                              
    TX1FtdcPriceType                      Fee;                           ///< �ⶳ������(�˰汾������)                  
    TX1FtdcPriceType                      Margin;                        ///< �ⶳ��֤��(�˰汾������) 
    TX1FtdcSessionIDType                  SessionID;                     ///< �ỰID
    TX1FtdcBuySellTypeType                BuySellType;                   ///< ������־(�˰汾������)
    TX1FtdcQuoteIDType                    QuoteID;                       ///< ѯ�۱��
    TX1FtdcDateType                       CanceledTime;                  ///< ����ʱ��
    TX1FtdcCustomCategoryType             CustomCategory;                ///< �Զ������(�˰汾������)
    TX1FtdcErrorIDType                    ErrorID;                       ///< ����ID	2�µ�֮ǰ���
    TX1FtdcErrorMsgInfoType               StatusMsg;                     ///< ״̬��Ϣ
    TX1FtdcLocalOrderIDType               PriFlowNo;                     ///< ˽�������
};


///�����̳ɽ��ر�
struct APISTRUCT CX1FtdcQuoteMatchRtnField {
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ������ID
    TX1FtdcClientIDType                   ClientID;                     ///< ���ױ���(�˰汾����)
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
    TX1FtdcSeatCodeType                   SeatCode;                     ///< ϯλ����
    TX1FtdcLocalOrderIDType               LocalOrderID;                 ///< ����ί�к�
    TX1FtdcOpenCloseTypeType              OpenCloseType;                ///< ��ƽ��־
    TX1FtdcSpeculatorType                 Speculator;                   ///< Ͷ�����  
    TX1FtdcX1OrderIDType                  X1OrderID;                    ///< ��̨ί�к�  
    TX1FtdcOrderSysIDType                 OrderSysID;                   ///< �������(�������������)
    TX1FtdcMatchIDType                    MatchID;                      ///< �ɽ����
    TX1FtdcAmountType                     MatchedAmount;                ///< �ɽ�����
    TX1FtdcPriceType                      MatchedPrice;                 ///< �ɽ��۸�
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˺�(�˰汾����)
    TX1FtdcPriceType                      Turnover;                     ///< �ɽ����(�˰汾����)
    TX1FtdcEntrusTellerType               EntrusTeller;                 ///< ί�й�Ա(�˰汾����)
    TX1FtdcDateType                       MatchedTime;                  ///< �ɽ�ʱ��
    TX1FtdcFeeType                        Fee;                          ///< ������(�˰汾����)
    TX1FtdcPriceType                      InsertPrice;                  ///< ί�м۸�(�˰汾����)
    TX1FtdcAmountType                     OrderAmount;                  ///< ί������(�˰汾����)
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  ///< �걨���(�˰汾����) �ɽ��ɹ�12��ȫ���ɽ�����û����м���
    TX1FtdcPriceType                      Margin;                       ///< ����Ϊ��֤��,ƽ��Ϊ�ⶳ��֤��(�˰汾����)
    TX1FtdcBuySellTypeType                BuySellType;                  ///< ����
    TX1FtdcAmountType                     CloseTodayAmount;             ///< ƽ������(�˰汾����)
    TX1FtdcPriceType                      ClosePrice;                   ///< ƽ�ֽ��(�˰汾����)
    TX1FtdcPriceType                      CloseTodayPrice;              ///< ƽ����(�˰汾����)
    TX1FtdcAdjustmentInfoType             AdjustmentInfo;               ///< ��ϻ�����ı�֤�������Ϣ,��ʽ:[��Լ����,������־,Ͷ�����,�������;](�˰汾����) 
    TX1FtdcPriceType                      FrozenCapita;                 ///< �ɽ��ⶳί�ж�����ʽ�(�˰汾����)
    TX1FtdcProfitLossType                 DateCloseProfitLoss;          ///< ����ƽ��ӯ��(�˰汾����)
    TX1FtdcInstrumentTypeType             InstrumentType;               ///< ��Լ����
    TX1FtdcSessionIDType                  SessionID;                    ///< �Ự��ʶ
    TX1FtdcLargeMarginDirectType          LargeMarginDirect;            ///< ��߱�֤����(�˰汾����)
    TX1FtdcQuoteIDType                    QuoteID;                      ///< ѯ�۱��(�˰汾����)
    TX1FtdcCustomCategoryType             CustomCategory;               ///< �Զ������(�˰汾������)
    TX1FtdcLocalOrderIDType               PriFlowNo;                    ///< ˽�������
};


///����������Ӧ
struct APISTRUCT CX1FtdcCancelAllOrderRspField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˻�ID(�˰汾������)
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  ///< ί��״̬
};

///ѯ��������Ӧ
struct APISTRUCT CX1FtdcForQuoteRspField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcX1OrderIDType                  X1OrderID;                    ///< ��̨ί�к�
    TX1FtdcDateType                       CommTime;                     ///< ί��ʱ��(�˰汾������)
};

///ѯ�ۻر�
struct APISTRUCT CX1FtdcForQuoteRtnField {
    TX1FtdcX1OrderIDType                  X1OrderID;                    ///< ��̨ί�к�
    TX1FtdcSessionIDType                  SessionID;                    ///< �ỰID
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ������(�˰汾������)
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˺�(�˰汾������)
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  ///< ί��״̬
    TX1FtdcErrorIDType                    ErrorID;                      ///< ����ID
    TX1FtdcLocalOrderIDType               PriFlowNo;                    ///< ˽�������
};

///��ѯѯ������
struct APISTRUCT CX1FtdcQryForQuoteField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˻�ID
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ������
};

///�����̱���ί�в�ѯ
struct APISTRUCT CX1FtdcQuoteOrderField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ������
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˻�
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
    TX1FtdcLocalOrderIDType               LocalOrderID;                 ///< ����ί�к�
    TX1FtdcX1OrderIDType                  X1OrderID;                    ///< ��̨ί�к�
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  ///< ί��״̬
};

///ѯ��֪ͨ��ѯ����
struct APISTRUCT CX1FtdcQryQuoteNoticeField {
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˺�
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ������
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
};

///��ѯѯ����Ӧ
struct APISTRUCT CX1FtdcQryForQuoteRtnField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˺�
    TX1FtdcX1OrderIDType                  X1OrderID;                    ///< ��̨ί�к�
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ������
    TX1FtdcDateType                       SuspendTime;                  ///< ����ʱ��
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  ///< ί��״̬
};

///�����̱��۲�ѯ��Ӧ
struct APISTRUCT CX1FtdcQuoteOrderRtnField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcX1OrderIDType                  X1OrderID;                    ///< ��̨ί�к�
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  ///< ί��״̬
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
    TX1FtdcPriceType                      Margin;                       ///< ��֤��
    TX1FtdcPriceType                      Fee;                          ///< ������
    TX1FtdcLocalOrderIDType               LocalOrderID;                 ///< ����ί�к�
    TX1FtdcAccountIDType                  AccountID;                    ///< �ͻ���
    TX1FtdcDateType                       CommTime;                     ///< ί��ʱ��
    TX1FtdcDateType                       SubmitTime;                   ///< �걨ʱ��
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ������ID
    TX1FtdcAmountType                     BuyOrderAmount;               ///< ί������(��)
    TX1FtdcAmountType                     BuyMatchedAmount;             ///< �ɽ�����(��)
    TX1FtdcAmountType                     BuyCancelAmount;              ///< ��������(��)
    TX1FtdcPriceType                      BuyInsertPrice;               ///< ί�м۸�(��)
    TX1FtdcPriceType                      BuyMatchedPrice;              ///< �ɽ��۸�(��)
    TX1FtdcOpenCloseTypeType              BuyOpenCloseType;             ///< ��ƽ��־(��)
    TX1FtdcAmountType                     SellOrderAmount;              ///< ί������(��)
    TX1FtdcAmountType                     SellMatchedAmount;            ///< �ɽ�����(��)
    TX1FtdcAmountType                     SellCancelAmount;             ///< ��������(��)
    TX1FtdcPriceType                      SellInsertPrice;              ///< ί�м۸�(��)
    TX1FtdcPriceType                      SellMatchedPrice;             ///< �ɽ��۸�(��)
    TX1FtdcOpenCloseTypeType              SellOpenCloseType;            ///< ��ƽ��־(��)
    TX1FtdcFrontAddrType                  OperStation;                  ///< ����վ��
    TX1FtdcSessionIDType                  SessionID;                    ///< �ỰID
    TX1FtdcQuoteIDType                    QuoteID;                      ///< ѯ�۱��
    TX1FtdcCustomCategoryType             CustomCategory;               ///< �Զ������
};

///ѯ��֪ͨ��ѯ��Ӧ
struct APISTRUCT CX1FtdcQryQuoteNoticeRtnField {
    TX1FtdcRequestIDType                  RequestID;                    ///< ����ID
    TX1FtdcQuoteIDType                    QuoteID;                      ///< ѯ�۱��
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ������
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
    TX1FtdcSourceType                     Source;                       ///< ��Դ
    TX1FtdcDateType                       QuoteTime;                    ///< ѯ��ʱ��
};

///������Լ��ѯ
/**
 * @details ��exchangeIDΪ��ʱ����ʾ��ѯ�������������к�Լ���룬Ҳ��ָ����ѯĳ�����������к�Լ������Ϣ��
 */
struct  APISTRUCT CX1FtdcAbiInstrumentField {
    TX1FtdcRequestIDType                  RequestID;                   ///< ����ID
    TX1FtdcAccountIDType                  AccountID;                    ///< �ʽ��˻�ID
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ����������
};

///������Լ������Ϣ
struct APISTRUCT CX1FtdcAbiInstrumentRtnField {
    TX1FtdcRequestIDType                  RequestID;                   ///< ����ID
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ����������
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
    TX1FtdcInstrumentNameType             InstrumentName;               ///< ��Լ����
    TX1FtdcPriceType                      UpperLimitPrice;              ///< ��ͣ���
    TX1FtdcPriceType                      LowerLimitPrice;              ///< ��ͣ���        
    TX1FtdcPriceType                      PriceTick;                    ///< ��С�䶯��λ
};

///ѯ��֪ͨ���Ļر�
struct APISTRUCT CX1FtdcQuoteSubscribeRtnField {
    TX1FtdcQuoteIDType                    QuoteID;                      ///< ѯ�۱��
    TX1FtdcExchangeIDType                 ExchangeID;                   ///< ������
    TX1FtdcInstrumentIDType               InstrumentID;                 ///< ��Լ����
    TX1FtdcSourceType                     Source;                       ///< ��Դ
    TX1FtdcDateType                       QuoteTime;                    ///< ѯ��ʱ��
};

struct APISTRUCT CX1FtdcExchangeConnectionStatusRtnField {
};

#endif///< TX1FtdcAPISTRUCT_H_
