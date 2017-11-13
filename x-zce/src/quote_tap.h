#ifndef TAP_QUOTE_INTERFACE_H_
#define TAP_QUOTE_INTERFACE_H_

#include <atomic>
#include <string>
#include <sstream>
#include <list>
#include <iomanip>
#include <vector>
#include <functional>
#include <string.h>
#include <string>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <list>
#include <time.h>

class DLL_LOCAL MYTAPDataHandler: 
{
public:
    /**
     * @brief	系统登录过程回调。
     * @details	此函数为Login()登录函数的回调，调用Login()成功后建立了链路连接，然后API将向服务器发送登录认证信息，
     *			登录期间的数据发送情况和登录的回馈信息传递到此回调函数中。
     * @param[in] errorCode 返回错误码,0表示成功。
     * @param[in] info 登陆应答信息，如果errorCode!=0，则info=NULL。
     * @attention	该回调返回成功，说明用户登录成功。但是不代表API准备完毕。
     * @ingroup G_Q_Login
     */
    virtual void TAP_CDECL OnRspLogin(TAPIINT32 errorCode, const TapAPIQuotLoginRspInfo *info);

    /**
     * @brief	通知用户API准备就绪。
     * @details	只有用户回调收到此就绪通知时才能进行后续的各种行情数据查询操作。\n
     *			此回调函数是API能否正常工作的标志。
     * @attention  就绪后才可以进行后续正常操作
     * @ingroup G_Q_Login
     */
    virtual void TAP_CDECL OnAPIReady();

    /**
     * @brief	API和服务失去连接的回调
     * @details	在API使用过程中主动或者被动与服务器服务失去连接后都会触发此回调通知用户与服务器的连接已经断开。
     * @param[in] reasonCode 断开原因代码。 \n
     * @ingroup G_Q_Disconnect
     */
    virtual void TAP_CDECL OnDisconnect(TAPIINT32 reasonCode);

    /**
     * @brief 返回交易所信息
     * @param[in] sessionID 请求的会话ID；
     * @param[in] errorCode 错误码，当errorCode!=0时,info为NULL；
     * @param[in] isLast 标示是否是最后一批数据；
     * @param[in] info 返回的交易所信息。
     * @attention  不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_Q_Exchange
     */
    virtual void TAP_CDECL OnRspQryExchange(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIExchangeInfo *info);

    /**
     * @brief	返回所有品种信息。
     * @details	此回调接口用于向用户返回得到的所有品种信息。
     * @param[in] sessionID 请求的会话ID，和GetAllCommodities()函数返回对应；
     * @param[in] errorCode 错误码，当errorCode!=0时,info为NULL；
     * @param[in] isLast 标示是否是最后一批数据；
     * @param[in] info 返回的信息数组的起始指针。
     * @attention  不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_Q_Commodity
     */
    virtual void TAP_CDECL OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteCommodityInfo *info);

    /**
     * @brief 返回系统中合约信息
     * @param[in] sessionID 请求的会话ID；
     * @param[in] errorCode 错误码，当errorCode!=0时,info为NULL；
     * @param[in] isLast 标示是否是最后一批数据；
     * @param[in] info		指向返回的信息结构体。当errorCode不为0时，info为空。
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_Q_Contract
     */
    virtual void TAP_CDECL OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteContractInfo *info);

    /**
     * @brief	返回订阅行情的全文。
     * @details	此回调接口用来返回订阅行情的全文。全文为当前时间的行情信息。
     * @param[in] sessionID 请求的会话ID；
     * @param[in] isLast 标示是否是最后一批数据；
     * @param[in] errorCode 错误码，当errorCode!=0时,info为NULL；
     * @param[in] info		指向返回的信息结构体。当errorCode不为0时，info为空。
     * @ingroup G_Q_Quote
     */
    virtual void TAP_CDECL OnRspSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteWhole *info);

    /**
     * @brief 退订指定合约的行情的结果回调
     * @param[in] sessionID 请求的会话ID；
     * @param[in] errorCode 错误码，当errorCode!=0时,info为NULL；
     * @param[in] isLast 标示是否是最后一批数据；
     * @param[in] info		指向返回的信息结构体。当errorCode不为0时，info为空。
     * @ingroup G_Q_Quote
     */
    virtual void TAP_CDECL OnRspUnSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIContract *info);

    /**
     * @brief	返回订阅行情的变化内容。
     * @details	此回调接口用来通知用户行情信息产生了变化，并向用户提交新的行情全文。
     * @param[in] info 最新的行情全文内容
     * @attention 不要修改和删除Quote指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_Q_Quote
     */
    virtual void TAP_CDECL OnRtnQuote(const TapAPIQuoteWhole *info);

    /**
     * @brief 通知用户密码修改结果
     * @param[in] sessionID 修改密码的会话ID,和ChangePassword返回的会话ID对应。
     * @param[in] errorCode 返回错误码，0表示成功。
     * @note 暂未实现
     * @ingroup G_Q_Password
     */
    virtual void TAP_CDECL OnRspChangePassword(TAPIUINT32 sessionID, TAPIINT32 errorCode);

    /**
     * @brief 通知合约添加
     * @param[in] info		指向返回的新增合约
     * @attention 不要修改和删除info所指示的数据；函数调用结束，参数不再有效。
     * @ingroup G_Q_Contract
     */
    virtual void TAP_CDECL OnRtnContract(const TapAPIQuoteContractInfo *info);

private:
    ITapQuoteAPI *api_;
    unsigned int *sID;

    /* 包装函数，方便线程调用 */
    void req_login(int wait_seconds);
    void subscribe_by_contract();
    void subscribe_quote(TapAPIContract &contract);

    /* 订阅合约集合 */
    SubscribeContracts subscribe_contracts_;
    bool ParseConfig();

    /* data convert functions */
    TapAPIQuoteWhole_MY Convert(const TapAPIQuoteWhole &other);
};

#endif /*QUOTE_TAP_H_*/
