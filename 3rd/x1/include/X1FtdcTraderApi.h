/**
* Copyright (C) 2015-2016, 大连飞创信息技术有限公司
* @file <X1FtdcTraderApi.h>
* @brief <定义X1交易接口>
* @version <1.3.2.3>
* @author <X1项目组>
* @date <2015年12月2日>
*/

/**
 * @mainpage X1 UserAPI 用户文档
 *
 *  X1柜台用户API帮助手册
 *  Copyright (C) 2015-2016, 大连飞创信息技术有限公司
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
    * @brief 用户响应接口
    * @details 实现了事件通知接口，用户需要实现此类接口，编写事件处理方法来处理用户感兴趣的事件。
    * @see Init
    */
    class CX1FtdcTraderSpi {
    public:
        /**
        * @brief    网络连接正常响应
        * @details  当客户端与交易后台需建立起通信连接时（还未登录前），客户端API会自动检测与前置机之间的连接，
        *           当网络可用，将自动建立连接，并调用该方法通知客户端， 客户端可以在实现该方法时，重新使用资金账号进行登录。
        *           （该方法是在Api和前置机建立连接后被调用，该调用仅仅是说明tcp连接已经建立成功。用户需要自行登录才能进行后续的业务操作。
        *           连接失败则此方法不会被调用。）
        *
        */
        virtual void OnFrontConnected() {
        };

        /**
        * @brief 网络连接不正常响应
        * @details 当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端需保证连接成功后有相应的登录逻辑处理。
        * @param    nReason 错误原因
        *           <ul>
        *               <li> 0x1001 查询服务网络读失败
        *               <li> 0x1002 查询服务网络写失败
        *               <li> 0x2002 查询服务发送心跳失败
        *               <li> 0x2003 查询服务收到错误报文
        *               <li> 0x3001 报单服务网络读失败
        *               <li> 0x3002 报单服务网络写失败
        *               <li> 0x4001 报单服务接收心跳超时
        *               <li> 0x4002 报单服务发送心跳失败
        *               <li> 0x4003 报单服务收到错误报文
        *           </ul>
        *
        */
        virtual void OnFrontDisconnected(int nReason) {
        };
        /**
        * @brief   登陆请求响应
        * @details 当用户发出登录请求后，前置机返回响应时此方法会被调用，通知用户登录是否成功。当pUserLoginInfoRtn.loginResult为0时表示登录成功，且登录成功时，pErrorInfo为NULL，否则pErrorInfo中将包含错误ID和错误信息。成功时，用户将获取一个会话ID,当SessionID为1的时候，默认为用户第一次登陆.
        * @param pUserLoginInfoRtn 用户登录信息结构地址。
        * @param pErrorInfo 若登录失败，返回错误信息地址，该结构含有错误信息。
        */
        virtual void OnRspUserLogin(struct CX1FtdcRspUserLoginField* pUserLoginInfoRtn, struct CX1FtdcRspErrorField* pErrorInfo) {
        };

        /**
        * @brief   登出请求响应:
        * @details 当用户发出退出请求后，前置机返回响应此方法会被调用，通知用户退出状态。
        * @param pUserLogoutInfoRtn 返回用户退出信息结构地址。
        * @param pErrorInfo 若登出失败，返回错误信息地址，该结构含有错误信息。
        * @warning 当用户登录返回错误码35,用户需release API 或 重启API后重新登录
        */
        virtual void OnRspUserLogout(struct CX1FtdcRspUserLogoutInfoField* pUserLogoutInfoRtn, struct CX1FtdcRspErrorField* pErrorInfo) {
        };

        /**
        * @brief   期货委托报单响应
        * @details 当报单发生错误时，pErrorInfo不为NULL，并在其中包含了错误ID及错误信息，和请求报单时的LocalOrderID，用于对应客户程序的报单。报单成功时，是表示X1台系统确认了该笔报单，该报单也同时报到了交易所，但交易所还未确认。
        * @param pOrderRtn 返回用户下单信息结构地址。
        * @param pErrorInfo 若报单失败，返回错误信息地址，该结构含有错误信息。
        */
        virtual void OnRspInsertOrder(struct CX1FtdcRspOperOrderField* pOrderRtn, struct CX1FtdcRspErrorField* pErrorInfo) {
        };

        /**
        * @brief   期货委托撤单响应
        * @details 当用户撤单后，前置返回响应是该方法会被调用。
        * @param pOrderCanceledRtn 返回撤单响应信息结构地址。
        * @param pErrorInfo 若撤单失败，返回错误信息地址，该结构含有错误信息。
        * @warning 撤单发生错误时，需要检查pErrorInfo是否为NULL。且客户端收到该响应时，只表示柜台确认了这笔撤单请求。
        */
        virtual void OnRspCancelOrder(struct CX1FtdcRspOperOrderField* pOrderCanceledRtn, struct CX1FtdcRspErrorField* pErrorInfo) {
        };

        /**
        * @brief   错误回报
        * @details API内部或者柜台内部发生异常错误，通过此方法通知用户。
        *           API内部错误：包含API内存数据异常、系统资源申请异常等。
        *           柜台内存错误：包含交易所API内部抛出异常，柜台内部数据异常，系统资源申请异常等。
        * @param pErrorInfo 错误信息的结构地址。
        */
        virtual void OnRtnErrorMsg(struct CX1FtdcRspErrorField* pErrorInfo) {
        };

        /**
        * @brief   成交回报
        * @details 当委托成功交易后次方法会被调用。
        * @param pRtnMatchData 指向成交回报的结构的指针。
        */
        virtual void OnRtnMatchedInfo(struct CX1FtdcRspPriMatchInfoField* pRtnMatchData) {
        };

        /**
        * @brief   委托回报
        * @details 当交易所收到委托请求并返回回报时，该方法会被调用
        * @param pRtnOrderData 指向委托回报地址的指针。
        */
        virtual void OnRtnOrder(struct CX1FtdcRspPriOrderField* pRtnOrderData) {
        };

        /**
        * @brief  撤单回报
        * @details 当撤单成功后该方法会被调用。
        * @param pCancelOrderData 指向撤单回报结构的地址，该结构体包含被撤单合约的相关信息。
        */
        virtual void OnRtnCancelOrder(struct CX1FtdcRspPriCancelOrderField* pCancelOrderData) {
        };

        /**
        * @brief  查询当日委托响应
        * @details 当用户发出委托查询后，该方法会被调用。
        * @param pRtnOrderData 指向委托回报结构的地址。
        * @param pErrorInfo 若查询失败，返回错误信息地址，该结构含有错误信息。
        * @param bIsLast    表明是否是最后一条响应信息
        *                   <ul>
        *                       <li> 0 - 否
        *                       <li> 1 - 是
        *                   </ul>
        */
        virtual void OnRspQryOrderInfo(struct CX1FtdcRspOrderField* pRtnOrderData, struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief  查询当日成交响应
        * @details 当用户发出成交查询后该方法会被调用。
        * @param pRtnMatchData 指向成交回报结构的地址。
        * @param pErrorInfo 若查询失败，返回错误信息地址，该结构含有错误信息。
        * @param bIsLast 表明是否是最后一条响应信息
        *                   <ul>
        *                       <li> 0 - 否
        *                       <li> 1 - 是
        *                   </ul>
        */
        virtual void OnRspQryMatchInfo(struct CX1FtdcRspMatchField* pRtnMatchData, struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief  持仓查询响应
        * @details 当用户发出持仓查询指令后，前置返回响应时该方法会被调用。
        * @param pPositionInfoRtn 返回持仓信息结构的地址。
        * @param pErrorInfo 若查询失败，返回错误信息地址，该结构含有错误信息。
        * @param bIsLast 表明是否是最后一条响应信息
        *                   <ul>
        *                       <li> 0 - 否
        *                       <li> 1 - 是
        *                   </ul>
        */
        virtual void OnRspQryPosition(struct CX1FtdcRspPositionField* pPositionInfoRtn, struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief  客户资金查询响应
        * @details 当用户发出资金查询指令后，前置返回响应时该方法会被调用。
        * @param pCapitalInfoRtn 返回资金信息结构的地址。
        * @param pErrorInfo 若查询失败，返回错误信息地址，该结构含有错误信息。
        * @param bIsLast 表明是否是最后一条响应信息
        *                   <ul>
        *                       <li> 0 - 否
        *                       <li> 1 - 是
        *                   </ul>
        */
        virtual void OnRspCustomerCapital(struct CX1FtdcRspCapitalField* pCapitalInfoRtn, struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief  交易所合约查询响应
        * @details 当用户发出合约查询指令后，前置返回响应时该方法会被调用。
        * @param pInstrumentData 返回合约信息结构的地址。
        * @param pErrorInfo 错误信息结构，如果查询发生错误，则返回错误信息。
        * @param bIsLast 表明是否是最后一条响应信息
        *                   <ul>
        *                       <li> 0 - 否
        *                       <li> 1 - 是
        *                   </ul>
        */
        virtual void OnRspQryExchangeInstrument(struct CX1FtdcRspExchangeInstrumentField* pInstrumentData, struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief  查询指定合约响应
        * @details 当用户发出指定合约查询指令后，前置返回响应时该方法会被调用。
        * @param pInstrument 返回指定合约信息结构的地址。
        * @param pErrorInfo 若查询失败，返回错误信息地址，该结构含有错误信息。
        * @param bIsLast 表明是否是最后一条响应信息
        *                   <ul>
        *                       <li> 0 - 否
        *                       <li> 1 - 是
        *                   </ul>
        */
        virtual void OnRspQrySpecifyInstrument(struct CX1FtdcRspSpecificInstrumentField* pInstrument, struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief  查询持仓明细响应
        * @details 当用户发出查询持仓明细后，前置返回响应时该方法会被调用。
        * @param pPositionDetailRtn 返回持仓明细结构的地址。
        * @param pErrorInfo 若查询失败，返回错误信息地址，该结构含有错误信息。
        * @param bIsLast 表明是否是最后一条响应信息
        *                   <ul>
        *                       <li> 0 - 否
        *                       <li> 1 - 是
        *                   </ul>
        */
        virtual void OnRspQryPositionDetail(struct CX1FtdcRspPositionDetailField* pPositionDetailRtn, struct CX1FtdcRspErrorField* pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief 交易所状态查询响应
        * @param pRspExchangeStatusData:指向交易所状态查询响应地址的指针。
        * @warning 若收不到开闭市信号，可先咨询期货公司运维同事，是否在报盘的配置中，打开了<PushJYZT Value=”YES”/>的选项
        */
        virtual void OnRspQryExchangeStatus(struct CX1FtdcRspExchangeStatusField * pRspExchangeStatusData) {
        };
        /**
          * @brief 交易所状态通知
          * @details 支持交易所状态通知和品种状态通知，在x1柜台可配
          * @param pRtnExchangeStatusData 指向交易所状态通知地址的指针。
          */
        virtual void OnRtnExchangeStatus(struct CX1FtdcExchangeStatusRtnField * pRtnExchangeStatusData) {
        };

        /**
        * @brief 做市商报单响应
        * @param pRspQuoteData:指向做市商报单响应地址的指针。
        * @param pErrorInfo:若报价失败，返回错误信息地址，该结构含有错误信息。
        */
        virtual void OnRspQuoteInsert(struct CX1FtdcQuoteRspField * pRspQuoteData, struct CX1FtdcRspErrorField * pErrorInfo) {
        };

        /**
        * @brief 做市商报单回报
        * @param pRtnQuoteData:指向做市商报单回报地址的指针。
        */
        virtual void OnRtnQuoteInsert(struct CX1FtdcQuoteRtnField * pRtnQuoteData) {
        };

        /**
        * @brief 做市商撤单响应
        * @param pRspQuoteCanceledData:指向做市商撤单响应地址的指针。
        * @param pErrorInfo:若撤单失败，返回错误信息地址，该结构含有错误信息。
        */
        virtual void OnRspQuoteCancel(struct CX1FtdcQuoteRspField * pRspQuoteCanceledData, struct CX1FtdcRspErrorField * pErrorInfo) {
        };

        /**
        * @brief 做市商撤单回报
        * @param pRtnQuoteCanceledData:指向做市商撤单回报地址的指针。
        */
        virtual void OnRtnQuoteCancel(struct CX1FtdcQuoteCanceledRtnField * pRtnQuoteCanceledData) {
        };

        /**
        * @brief 做市商成交回报
        * @param pRtnQuoteMatchedData:指向做市商成交回报地址的指针。
        */
        virtual void OnRtnQuoteMatchedInfo(struct CX1FtdcQuoteMatchRtnField * pRtnQuoteMatchedData) {
        };

        /**
        * @brief 批量撤单响应
        * @param pRspStripCancelOrderData:指向批量撤单响应地址的指针。
        * @param pErrorInfo:若批量撤单失败，返回错误信息地址，该结构含有错误信息。
        */
        virtual void OnRspCancelAllOrder(struct CX1FtdcCancelAllOrderRspField *pRspCancelAllOrderData, struct CX1FtdcRspErrorField * pErrorInfo) {
        };


        /**
        * @brief 询价响应
        * @param pRspForQuoteData:询价请求响应结构地址。
        * @param pErrorInfo:若查询失败，返回错误信息地址，该结构含有错误信息。
        */
        virtual void OnRspForQuote(struct CX1FtdcForQuoteRspField * pRspForQuoteData, struct CX1FtdcRspErrorField * pErrorInfo) {
        };

        /**
        * @brief 询价回报
        * @param pRtnForQuoteData:询价回报结构地址。
        */
        virtual void OnRtnForQuote(struct CX1FtdcForQuoteRtnField * pRtnForQuoteData) {
        };


        /**
        * @brief 询价委托查询响应
        * @param pRtnQryForQuoteData:指向询价委托查询响应地址的指针。
        * @param pErrorInfo:若查询失败，返回错误信息地址，该结构含有错误信息。
        * @param bIsLast:表明是否是最后一条响应信息（0 -否   1 -是）。
        */
        virtual void OnRspQryForQuote(struct CX1FtdcQryForQuoteRtnField * pRtnQryForQuoteData, struct CX1FtdcRspErrorField * pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief 查询当日报价委托响应
        * @param pRtnQuoteOrderData:指向报价查询回报结构的地址。
        * @param pErrorInfo:若查询失败，返回错误信息地址，该结构含有错误信息。
        * @param bIsLast:表明是否是最后一条响应信息（0 -否   1 -是）。
        */
        virtual void OnRspQryQuoteOrderInfo(struct CX1FtdcQuoteOrderRtnField * pRtnQuoteOrderData, struct CX1FtdcRspErrorField * pErrorInfo, bool bIsLast) {
        };

        /**
        * @brief 询价通知查询响应
        * @param pRtnQryQuoteNoticeData:查询询价通知回报结构地址。
        * @param pErrorInfo:若查询失败，返回错误信息地址，该结构含有错误信息。
        * @param bIsLast:表明是否是最后一条响应信息（0 -否   1 -是）。
        */
        virtual void OnRspQryQuoteNotice(struct CX1FtdcQryQuoteNoticeRtnField * pRtnQryQuoteNoticeData, struct CX1FtdcRspErrorField * pErrorInfo, bool bIsLast) {
        };


        /**
        * @brief 套利合约查询响应:当用户发出套利合约查询指令后，前置返回响应时该方法会被调用。
        * @param pAbiInstrumentData:返回套利合约信息结构的地址。
        * @param pErrorInfo:若查询失败，返回错误信息地址，该结构含有错误信息。
        * @param bIsLast:表明是否是最后一条响应信息（0 -否   1 -是）。
        */
        virtual void OnRspArbitrageInstrument(struct CX1FtdcAbiInstrumentRtnField * pAbiInstrumentData, struct CX1FtdcRspErrorField * pErrorInfo, bool bIsLast) {
        };


        /**
        * @brief 做市商询价通知，无需订阅，做市商客户自动收到询价通知
        * @param pForQuoteRspData:指向询价通知回报地址的指针。
        */
        virtual void OnRtnForQuoteRsp(struct CX1FtdcQuoteSubscribeRtnField * pForQuoteRspData) {
        };

        virtual void OnNtyExchangeConnectionStatus(CX1FtdcExchangeConnectionStatusRtnField *pExchangeConnectionStatusData) {
        };

    };//end of CX1FtdcTraderSpi

    /**
    * @brief 用户请求接口
    * @details 实现了用户请求接口，用户实例化对象后，可以直接调用接口进行登录、报单、撤单、查询等操作
    * @warning
    *         <ul>
    *           <li> 用户调用接口前，需要进行memset清空结构体内容，再填写需要的字段信息
    *           <li> 接口线程安全
    *           <li> 买卖，开平、投保、订单类型、订单属性这些字段因为使用了位域处理，在API端不做数值有效性检查判断，按照头文件指定数值输入，输入其他值也会被转换为不确定数值
    *           <li> 本节所示接口的委托响应均有可能晚于委托回报返回
    *         </ul>
    * @see Init
    */
    class X1FTDCTRADERAPI_API CX1FtdcTraderApi {
    public:
        CX1FtdcTraderApi();
        virtual ~CX1FtdcTraderApi();
    public:
        /**
        * @brief 静态函数，产生一个api实例
        * @return 返回一个指向CX1FtdcTraderApi实例的指针
        * @see Init
        */
        static CX1FtdcTraderApi * CreateCX1FtdcTraderApi(void);

        /**
        * @brief 释放API实例
        * @warning 调用CreateCX1FtdcTraderApi成功生成一个API实例,init成功后，用户程序退出时必须调用Release接口，否则可能会coredump
        */
        virtual void Release(void) = 0;

        /**
        * @brief 初始化接口
        * @details 和服务器建立socket连接，并启动一个接收线程， 同时该方法注册一个回调函数集
        * @param pszFrontAddr 交易前置网络地址。
        *                     网络地址的格式为:"protocol://ipaddress:port",如"tcp://172.21.200.103:10910"
        *                     其中protocol的值为tcp格式。
        *                     ipaddress表示交易前置的IP,port表示交易前置的端口
        * @param pSpi 指向CX1FtdcTraderSpi子类对象(回调函数集)的指针
        * @param output_core 输出线程绑定的cpu core id. -1表示不绑定
        * @param input_core 输入线程绑定的cpu core id. -1表示不绑定.input_core output_core不支持仅有一个参数为-1的情形
        * @return 初始化结果
        *         <ul>
        *           <li> 0 - 初始化成功
        *           <li> 1 - 初始化失败 此时可能需要检查OnRtnErrorMsg给出的错误信息
        *         </ul>
        *         Init失败原因可能：
        *               填写的addr格式不正确或者addr中的ip地址及端口不正确，如使用交易API连接到了行情前置的端口
        *               网络问题，可telnet连接ip及port，检查是否畅通
        * @code Usage:
        *   x1ftdcapi::CX1FtdcTraderApi* pApi = x1ftdcapi::CX1FtdcTraderApi::CreateCX1FtdcTraderApi();
        *   MyTradeSpi spi(pApi);   // MyTradeSpi是继承CX1FtdcTraderSpi的子类对象，实现了用户响应接口
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
        * @brief 等待接口线程结束运行。
        * @return 线程退出代码。
        *         <ul>
        *           <li> 0  - 线程退出成功
        *           <li> -1 - 线程退出失败
        *         </ul>
        */
        virtual int Join(void) = 0;

        /**
        * @brief 订阅私有流
        * @details 在Init 方法调用之后，ReqUserLogin 方法调用之前调用。若不调用该接口，则收不到私有流
        * @param priflow_req_flag 私有流订阅方式
        *         <ul>
        *           <li> 0 - 只传送登录后私有流的内容
        *           <li> 1 - 从本交易日开始重传
        *           <li> 2 - 从上次收到的续传
        *           <li> 3 - 从指定编号开始续传
        *         </ul>
        * @param pri_no 指定私有流编号(从指定编号处订阅私有流) 当priflow_req_flag取值3时，该参数才有效，其他情况，传入0值即可。注意: 用户收到的私有流编号可能不连续
        */
        virtual void SubscribePrivateTopic(int priflow_req_flag, unsigned int pri_no) = 0;

        virtual void SetMatchInfoAdvanced(bool matchinfo_advanced) = 0;

        /**
        * @brief 用户发出登录请求
        * @param pUserLoginData 指向用户登录请求结构的地址。
        * @return 返回值
        *         <ul>
        *           <li> 0      -  请求发送成功
        *           <li> 非零   -  错误码
        *         </ul>
        */
        virtual int ReqUserLogin(struct CX1FtdcReqUserLoginField * pUserLoginData) = 0;

        /**
        * @brief 用户发出登出请求
        * @param pUserLogoutData 指向用户登录请出结构的地址。
        * @return 返回值
        *         <ul>
        *           <li> 0      -  请求发送成功
        *           <li> 非零   -  错误码
        *         </ul>
        */
        virtual int ReqUserLogout(struct CX1FtdcReqUserLogoutField * pUserLogoutData) = 0;

        /**
        * @brief 期货委托报单请求。
        * @param pInsertOrderData 用户请求报单信息结构地址。
        * @return 返回值
        *         <ul>
        *           <li> 0      -  请求发送成功
        *           <li> 非零   -  错误码
        *         </ul>
        */
        virtual int ReqInsertOrder(struct CX1FtdcInsertOrderField * pInsertOrderData) = 0;

        /**
        * @brief 期货撤单请求。
        * @details 如果提供柜台委托号(柜台委托号大于-1)，则只使用柜台委托号处理；只有当柜台委托号小于0时，才使用本地委托号进行撤单
        * @param pCancelOrderData 用户请求撤单信息结构地址。
        * @return 返回值
        *         <ul>
        *           <li> 0      -  请求发送成功
        *           <li> 非零   -  错误码
        *         </ul>
        */
        virtual int ReqCancelOrder(struct CX1FtdcCancelOrderField * pCancelOrderData) = 0;

        /**
        * @brief 持仓查询请求。
        * @details 如果没有提供合约代码，则查询全部持仓信息。
        * @param pPositionData 用户请求持仓信息结构地址。
        * @return 返回值
        *         <ul>
        *           <li> 0      -  请求发送成功
        *           <li> -1     -  请求发送失败
        *           <li> -2     -  检测异常
        *         </ul>
        */
        virtual int ReqQryPosition(struct CX1FtdcQryPositionField * pPositionData) = 0;

        /**
        * @brief 客户资金查询请求。
        * @details 用户需要填充该结构的各个字段
        * @param pCapitalData 请求资金查询结构地址。
        * @return 返回值
        *         <ul>
        *           <li> 0      -  请求发送成功
        *           <li> 非零   -  错误码
        *         </ul>
        */
        virtual int ReqQryCustomerCapital(struct CX1FtdcQryCapitalField * pCapitalData) = 0;

        /**
        * @brief 查询交易所合约列表（非套利）。
        * @details 当ExchangeID为空时，表示查询各交易所的所有合约代码，也可指定查询某交易所的所有合约代码信息，但该接口不能查询到套利合约代码
        * @param pExchangeInstrumentData 交易所合约查询结构地址。
        * @return 返回值
        *         <ul>
        *           <li> 0      -  请求发送成功
        *           <li> 非零   -  错误码
        *         </ul>
        */
        virtual int ReqQryExchangeInstrument(struct CX1FtdcQryExchangeInstrumentField * pExchangeInstrumentData) = 0;

        /**
        * @brief 当日委托查询请求。
        * @param pOrderData 当日委托查询结构地址。
        * @return 返回值
        *         <ul>
        *           <li> 0      -  请求发送成功
        *           <li> -1     -  请求发送失败
        *           <li> -2     -  检测异常
        *         </ul>
        */
        virtual int ReqQryOrderInfo(struct CX1FtdcQryOrderField * pOrderData) = 0;

        /**
        * @brief 当日成交查询请求。
        * @param pMatchData 当日成交查询结构地址。
        * @return 返回值
        *         <ul>
        *           <li> 0      -  请求发送成功
        *           <li> 非零   -  错误码
        *         </ul>
        */
        virtual int ReqQryMatchInfo(struct CX1FtdcQryMatchField * pMatchData) = 0;

        /**
        * @brief 指定合约信息查询请求。
        * @details ExchangeID并非必须输入选项，因为目前期货市场上，一个合约代码在各交易所是唯一存在的。
        * @param pInstrument 指定合约查询结构地址。
        * @return 返回值
        *         <ul>
        *           <li> 0      -  请求发送成功
        *           <li> 非零   -  错误码
        *         </ul>
        */
        virtual int ReqQrySpecifyInstrument(struct CX1FtdcQrySpecificInstrumentField * pInstrument) = 0;

        /**
        * @brief 持仓明细查询请求。
        * @param pPositionDetailData:持仓明细查询结构地址。
        * @return 返回值
        *         <ul>
        *           <li> 0      -  请求发送成功
        *           <li> 非零   -  错误码
        *         </ul>
        */
        virtual int ReqQryPositionDetail(struct CX1FtdcQryPositionDetailField * pPositionDetailData) = 0;

        /**
        * @brief 交易所状态查询
        * @param pQryExchangeStatusData:交易所状态查询请求结构地址。
        * @return 返回值
        *         <ul>
        *           <li> 0      -  请求发送成功
        *           <li> 非零   -  错误码
        *         </ul>
        */
        virtual int ReqQryExchangeStatus(struct CX1FtdcQryExchangeStatusField *pQryExchangeStatusData) = 0;


        /**
        * @brief 查询交易所套利合约列表。
        * @param pAbtriInstrumentData:交易所套利合约查询结构地址。
        * @return 0 - 请求发送成功 -1 - 请求发送失败  -其它 -检测异常。
        */
        virtual int ReqQryArbitrageInstrument(struct CX1FtdcAbiInstrumentField * pAbtriInstrumentData) = 0;


        ///////以下做市商相关
        /**
        * @brief 做市商报单请求
        * @param pQuoteInsertOrderData:做市商报单请求结构地址。
        * @return 0 - 请求发送成功 -1 - 请求发送失败  -其它 -检测异常。
        */
        virtual int ReqQuoteInsert(struct CX1FtdcQuoteInsertField * pQuoteInsertOrderData) = 0;

        /**
        * @brief 做市商撤单请求
        * @details 对于双边应价，本接口只支持撤双边，不支持只撤销一边的操作
        * @param pQuoteCancelOrderData:做市商撤单请求结构地址。
        * @return 0 - 请求发送成功 -1 - 请求发送失败  -其它 -检测异常。
        */
        virtual int ReqQuoteCancel(struct CX1FtdcCancelOrderField * pQuoteCancelOrderData) = 0;

        /**
        * @brief 批量撤单
        * @details 该指令目前只支持大商所的做市商报价订单，其它交易所暂不支持，普通期货期权订单也不支持
        * @param pCancelAllOrderData:批量撤单请求结构地址。
        * @return 0 - 请求发送成功 -1 - 请求发送失败  -其它 -检测异常。
        */
        virtual int ReqCancelAllOrder(struct CX1FtdcCancelAllOrderField * pCancelAllOrderData) = 0;

        /**
        * @brief 询价请求
        * @param pForQuoteData:询价请求结构地址。
        * @return 0 - 请求发送成功 -1 - 请求发送失败  -其它 -检测异常。
        */
        virtual int ReqForQuote(struct CX1FtdcForQuoteField * pForQuoteData) = 0;

        /**
        * @brief 询价委托查询请求
        * @param pQryForQuoteData:询价委托查询请求结构地址。
        * @return 0 - 请求发送成功 -1 - 请求发送失败  -其它 -检测异常。
        */
        virtual int ReqQryForQuote(struct CX1FtdcQryForQuoteField * pQryForQuoteData) = 0;

        /**
        * @brief 做市商报价委托查询
        * @param pQuoteOrderData:做市商报价委托查询结构地址。
        * @return 0 - 请求发送成功 -1 - 请求发送失败  -其它 -检测异常。
        */
        virtual int ReqQryQuoteOrderInfo(struct CX1FtdcQuoteOrderField * pQuoteOrderData) = 0;

        /**
        * @brief 询价通知查询请求
        * @param pQryQuoteNoticeData:查询询价通知请求结构地址。
        * @return 0 - 请求发送成功 -1 - 请求发送失败  -其它 -检测异常。
        */
        virtual int ReqQryQuoteNotice(struct CX1FtdcQryQuoteNoticeField * pQryQuoteNoticeData) = 0;
        ///////以上做市商相关


    };//end of CX1FtdcTraderApi
}
//end of namespace


#endif//X1FTDCTRADERAPI_H_

