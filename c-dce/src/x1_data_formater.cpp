#include <stdio.h>
#include "x1_data_formater.h"
#include <iostream>

using namespace std;

std::string X1DatatypeFormater::ToString(const CX1FtdcReqUserLoginField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcReqUserLoginField "
            "    lRequestID=%ld "
            "    accountID=%s "
            "    passwd=%s "
            "    companyID=%d ",
            p->RequestID,                   //请求ID
            p->AccountID,                    //资金账户ID
            p->Password,                       //密码
            p->CompanyID                     //厂商ID
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcReqUserLoginField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcInsertOrderField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcInsertOrderField "
            "    accountID=%s "
            "    localOrderID=%ld "
            "    instrumentID=%s "
            "    insertPrice=%.4f "
            "    orderAmount=%ld "
            "    buySellType=%d "
            "    openCloseType=%d "
            "    speculator=%d "
            "    insertType=%d "
            "    orderType=%d "
            "    orderProperty=%d "
            "    instrumentType=%d "
            "    lRequestID=%ld "
            "    customCategory=%s "
            "    reservedType2=%d "
            "    minMatchAmount=%ld "
            "    profitLossPrice=%.4f ",
            p->AccountID,                    //资金账户
            p->LocalOrderID,                 //本地委托号, 由API使用者维护，在同一个会话中不能重复
            p->InstrumentID,                 //合约代码, 支持目前国内4个期货交易所的所有合约，包括大商所/郑商所的套利合约
            p->InsertPrice,                  //报单价格, 当报单类型为市价时，该字段不起作用
            p->OrderAmount,                  //报单数量
            p->BuySellType,                  //买卖标志
            p->OpenCloseType,                //开平标志
            p->Speculator,                   //投保类型, 支持投机、套利、套保
            p->InsertType,                   //委托类别(默认为普通订单)
            p->OrderType,                    //报单类型, 支持限价 、市价；上期所合约不支持市价，均按限价进行处理
            p->OrderProperty,                //报单附加属性, 支持None、FAK、FOK，目前只有大商所合约支持该报单附加属性 FAK/FOK
            p->InstrumentType,               //合约类型, 可选值：期货、期权
            p->RequestID,                   //请求ID
            p->CustomCategory,               //自定义类别
            p->ReservedType2,                //预留字段2
			p->MinMatchAmount,               //上期FAK,FOK单的最小成交量
			p->ProfitLossPrice               //止盈止损价格
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcInsertOrderField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcCancelOrderField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcCancelOrderField "
            "    accountID=%s "
            "    spdOrderID=%ld "
            "    localOrderID=%ld "
            "    instrumentID=%s "
            "    lRequestID=%ld ",
            p->AccountID,                    //资金账户ID
            p->X1OrderID,                   //柜台委托号
            p->LocalOrderID,                 //本地委托号
            p->InstrumentID,                 //合约代码
            p->RequestID                    //请求ID
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcCancelOrderField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcQryPositionField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcQryPositionField "
            "    lRequestID=%ld "
            "    accountID=%s "
            "    instrumentID=%s "
            "    instrumentType=%d ",
            p->RequestID,                   //请求ID
            p->AccountID,                    //资金账户ID
            p->InstrumentID,                 //合约代码
            p->InstrumentType                //合约类型
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcQryPositionField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcQryPositionDetailField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcQryPositionDetailField "
            "    lRequestID=%ld "
            "    accountID=%s "
            "    instrumentID=%s "
            "    instrumentType=%d ",
            p->RequestID,                   //请求ID
            p->AccountID,                    //资金账户ID
            p->InstrumentID,                 //合约代码
            p->InstrumentType                //合约类型
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcQryPositionDetailField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcQryOrderField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcQryOrderField "
            "    lRequestID=%ld "
            "    accountID=%s "
            "    instrumentType=%d "
            "    customCategory=%s ",
            p->RequestID,                   //请求ID
            p->AccountID,                    //资金账户ID
            p->InstrumentType,               //合约类型
            p->CustomCategory                //自定义类别
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcQryOrderField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcQryMatchField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcQryMatchField "
            "    lRequestID=%ld "
            "    accountID=%s "
            "    instrumentType=%d "
            "    customCategory=%s ",
            p->RequestID,                   //请求ID
            p->AccountID,                    //资金账户ID
            p->InstrumentType,               //合约类型
            p->CustomCategory                //自定义类别
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcQryMatchField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcQuoteInsertField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcQuoteInsertField "
            "    accountID=%s "
            "    lRequestID=%ld "
            "    localOrderID=%ld "
            "    insertType=%d "
            "    instrumentID=%s "
            "    quoteID=%s "
            "    instrumentType=%d "
            "    bOrderAmount=%ld "
            "    sOrderAmount=%ld "
            "    bInsertPrice=%.4f "
            "    sInsertPrice=%.4f "
            "    bOpenCloseType=%d "
            "    sOpenCloseType=%d "
            "    bSpeculator=%d "
            "    sSpeculator=%d "
            "    stayTime=%d ",
            p->AccountID,                    //资金账号
            p->RequestID,                   //请求ID
            p->LocalOrderID,                 //本地委托号
            p->InsertType,                   //委托类别
            p->InstrumentID,                 //合约代码
            p->QuoteID,                      //询价编号
            p->InstrumentType,               //合约类型
            p->BuyOrderAmount,                 //报单数量（买）
            p->SellOrderAmount,                 //报单数量（卖）
            p->BuyInsertPrice,                 //委托价格（买）
            p->SellInsertPrice,                 //委托价格（卖）
            p->BuyOpenCloseType,               //开平标志（买）
            p->SellOpenCloseType,               //开平标志（卖）
            p->BuySpeculator,                  //投资类别（买）
            p->SellSpeculator,                  //投资类别（卖）
            p->StayTime                     //停留时间，仅支持郑州。其它情况可设置为0
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcQuoteInsertField <null>");
    }

    return buf;
}


std::string X1DatatypeFormater::ToString(const CX1FtdcQryExchangeInstrumentField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcQryExchangeInstrumentField "
            "    lRequestID=%ld "
            "    accountID=%s "
            "    exchangeID=%s "
            "    instrumentType=%d ",
            p->RequestID,                   //请求ID
            p->AccountID,                    //资金账户ID
            p->ExchangeID,                   //交易所编码
            p->InstrumentType                //合约类型
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcQryExchangeInstrumentField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcRspErrorField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspErrorField "
            "    requestID=%ld "
            "    sessionID=%ld "
            "    accountID=%s "
            "    nErrorID=%d "
            "    spdOrderID=%ld "
            "    localOrderID=%ld "
            "    errorMsg=%s ",
            p->RequestID,                    //请求ID
            p->SessionID,                    //会话标识
            p->AccountID,                    //资金账号
            p->ErrorID,                     //错误ID
            p->X1OrderID,                   //柜台委托号
            p->LocalOrderID,                 //本地委托号
            p->ErrorMsg                      //错误信息
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspErrorField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcRspUserLoginField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspUserLoginField "
            "    lRequestID=%ld "
            "    accountID=%s "
            "    loginResult=%d "
            "    initLocalOrderID=%ld "
            "    sessionID=%ld "
            "    nErrorID=%d "
            "    errorMsg=%s "
            "    DCEtime=%s "
            "    SHFETime=%s "
            "    CFFEXTime=%s "
            "    CZCETime=%s ",
            p->RequestID,                   //请求ID
            p->AccountID,                    //资金帐号ID
            p->LoginResult,                  //登录结果
            p->InitLocalOrderID,             //初始本地委托号
            p->SessionID,                    //sessionID
            p->ErrorID,                     //错误ID
            p->ErrorMsg,                     //错误信息
            p->DCEtime,                      //大商所时间
            p->SHFETime,                     //上期所时间
            p->CFFEXTime,                    //中金所时间
            p->CZCETime                      //郑商所时间
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspUserLoginField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcRspUserLogoutInfoField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspUserLogoutInfoField "
            "    lRequestID=%ld "
            "    accountID=%s "
            "    logoutResult=%d "
            "    nErrorID=%d "
            "    errorMsg=%s ",
            p->RequestID,                   //请求ID
            p->AccountID,                    //资金账户ID
            p->LogoutResult,                 //退出结果
            p->ErrorID,                     //错误ID
            p->ErrorMsg                      //错误信息
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspUserLogoutInfoField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcRspOperOrderField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspOperOrderField "
            "    localOrderID=%ld "
            "    spdOrderID=%ld "
            "    orderStatus=%d "
            "    lRequestID=%ld "
            "    fee=%.4f "
            "    margin=%.4f "
            "    customCategory=%s ",
            p->LocalOrderID,                 //本地委托号
            p->X1OrderID,                   //柜台委托号
            p->OrderStatus,                  //委托状态
            p->RequestID,                   //请求ID
            p->Fee,                          //手续费,该字段仅供下单时使用
            p->Margin,                       //冻结保证金,该字段仅供下单时使用
            p->CustomCategory                //自定义类别
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspOperOrderField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcRspPriOrderField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspPriOrderField "
            "    localOrderID=%ld "
            "    spdOrderID=%ld "
            "    OrderSysID=%s "
            "    orderStatus=%d "
            "    sessionID=%ld "
            "    SuspendTime=%s "
            "    instrumentID=%s "
            "    exchangeID=%s "
            "    buySellType=%d "
            "    OpenCloseType=%d "
            "    instrumentType=%d "
            "    speculator=%d "
            "    insertPrice=%.4f "
            "    accountID=%s "
            "    cancelAmount=%ld "
            "    orderAmount=%ld "
            "    insertType=%d "
            "    extspdOrderID=%ld "
            "    customCategory=%s "
            "    reservedType2=%d "
            "    profitLossPrice=%.4f "
            "    ErrorID=%d "
            "    StatusMsg=%s " ,
            p->LocalOrderID,                 //本地委托号
            p->X1OrderID,                   //柜台委托号
            p->OrderSysID,                   //报单编号
            p->OrderStatus,                  //委托状态
            p->SessionID,                    //会话ID
            p->SuspendTime,                  //挂起时间
            p->InstrumentID,                 //合约代码
            p->ExchangeID,                   //交易所
            p->BuySellType,                  //买卖
            p->OpenCloseType,                //开平
            p->InstrumentType,               //合约类型
            p->Speculator,                   //投资类别
            p->InsertPrice,                  //委托价
            p->AccountID,                    //资金账号
            p->CancelAmount,                 //撤单数量
            p->OrderAmount,                  //委托数量
            p->InsertType,                   //委托类别
            p->ExtX1OrderID,                //算法单编号
            p->CustomCategory,               //自定义类别
            p->ReservedType2,                //预留字段2
			p->ProfitLossPrice,               //止盈止损价格
			p->ErrorID,
			p->StatusMsg);
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=DFITCOrderRtnField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcRspPriMatchInfoField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspPriMatchInfoField "
            "    localOrderID=%ld "
            "    OrderSysID=%s "
            "    matchID=%s "
            "    instrumentID=%s "
            "    buySellType=%d "
            "    OpenCloseType=%d "
            "    matchedPrice=%.4f "
            "    orderAmount=%ld "
            "    matchedAmount=%ld "
            "    matchedTime=%s "
            "    insertPrice=%.4f "
            "    spdOrderID=%ld "
            "    matchType=%ld "
            "    speculator=%d "
            "    exchangeID=%s "
            "    fee=%.4f "
            "    sessionID=%ld "
            "    instrumentType=%d "
            "    accountID=%s "
            "    orderStatus=%d "
            "    margin=%.4f "
            "    frozenCapita=%.4f "
            "    adjustmentInfo=%s "
            "    customCategory=%s "
            "    turnover=%.4f ",
            p->LocalOrderID,                 //本地委托号
            p->OrderSysID,                   //报单编号(交易所报单编号)
            p->MatchID,                      //成交编号
            p->InstrumentID,                 //合约代码
            p->BuySellType,                  //买卖
            p->OpenCloseType,                //开平标志
            p->MatchedPrice,                 //成交价格
            p->OrderAmount,                  //委托数量
            p->MatchedAmount,                //成交数量
            p->MatchedTime,                  //成交时间
            p->InsertPrice,                  //报价
            p->X1OrderID,                   //柜台委托号
            p->MatchType,                    //成交类型
            p->Speculator,                   //投保
            p->ExchangeID,                   //交易所ID
            p->Fee,                          //手续费
            p->SessionID,                    //会话标识
            p->InstrumentType,               //合约类型
            p->AccountID,                    //资金账号
            p->OrderStatus,                  //申报结果
            p->Margin,                       //开仓为保证金,平仓为解冻保证金
            p->FrozenCapita,                 //成交解冻委托冻结的资金
            p->AdjustmentInfo,               //组合或对锁的保证金调整信息,格式:[合约代码,买卖标志,投资类别,调整金额,]
            p->CustomCategory,               //自定义类别
            p->Turnover                      //成交金额
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspPriMatchInfoField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcRspPriCancelOrderField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspPriCancelOrderField "
            "    localOrderID=%ld "
            "    OrderSysID=%s "
            "    instrumentID=%s "
            "    insertPrice=%.4f "
            "    buySellType=%d "
            "    OpenCloseType=%d "
            "    cancelAmount=%ld "
            "    spdOrderID=%ld "
            "    speculator=%d "
            "    exchangeID=%s "
            "    canceledTime=%s "
            "    sessionID=%ld "
            "    orderStatus=%d "
            "    instrumentType=%d "
            "    accountID=%s "
            "    orderAmount=%ld "
            "    margin=%.4f "
            "    fee=%.4f ",
            p->LocalOrderID,                 //本地委托号
            p->OrderSysID,                   //报单编号
            p->InstrumentID,                 //合约代码
            p->InsertPrice,                  //报单价格
            p->BuySellType,                  //买卖类型
            p->OpenCloseType,                //开平标志
            p->CancelAmount,                 //撤单数量
            p->X1OrderID,                   //柜台委托号
            p->Speculator,                   //投保
            p->ExchangeID,                   //交易所ID
            p->CanceledTime,                 //撤单时间
            p->SessionID,                    //会话标识
            p->OrderStatus,                  //申报结果
            p->InstrumentType,               //合约类型
            p->AccountID,                    //资金账号
            p->OrderAmount,                  //委托数量
            p->Margin,                       //保证金
            p->Fee
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspPriCancelOrderField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcRspOrderField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspOrderField "
            "    lRequestID=%ld "
            "    spdOrderID=%ld "
            "    orderStatus=%d "
            "    instrumentID=%s "
            "    buySellType=%d "
            "    openClose=%d "
            "    insertPrice=%.4f "
            "    orderAmount=%ld "
            "    matchedPrice=%.4f "
            "    matchedAmount=%ld "
            "    cancelAmount=%ld "
            "    insertType=%d "
            "    speculator=%d "
            "    commTime=%s "
            "    submitTime=%s "
            "    clientID=%s "
            "    exchangeID=%s "
            "    operStation=%s "
            "    accountID=%s "
            "    instrumentType=%d "
            "    OrderSysID=%s "
            "    customCategory=%s "
            "    margin=%.4f "
            "    fee=%.4f "
            "    localOrderID=%ld "
            "    sessionId=%ld "
            "    reservedType2=%d "
            "    profitLossPrice=%.4f ",
            p->RequestID,                   //请求ID
            p->X1OrderID,                   //柜台委托号
            p->OrderStatus,                  //委托状态
            p->InstrumentID,                 //合约代码
            p->BuySellType,                  //买卖
            p->OpenClose,                    //开平标志
            p->InsertPrice,                  //委托价
            p->OrderAmount,                  //委托数量
            p->MatchedPrice,                 //成交价格
            p->MatchedAmount,                //成交数量
            p->CancelAmount,                 //撤单数量
            p->InsertType,                   //委托类别
            p->Speculator,                   //投保
            p->CommTime,                     //委托时间
            p->SubmitTime,                   //申报时间
            p->ClientID,                     //交易编码
            p->ExchangeID,                   //交易所ID
            p->OperStation,                  //委托地址
            p->AccountID,                    //客户号
            p->InstrumentType,               //合约类型
            p->OrderSysID,                   //报单编号
            p->CustomCategory,               //自定义类别
            p->Margin,                       //保证金
            p->Fee,                          //手续费
            p->LocalOrderID,                 //本地委托号
            p->SessionId,                    //会话ID
            p->ReservedType2,                //预留字段2
			p->ProfitLossPrice               //止盈止损价
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspOrderField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcRspMatchField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspMatchField "
            "    lRequestID=%ld "
            "    spdOrderID=%ld "
            "    exchangeID=%s "
            "    instrumentID=%s "
            "    buySellType=%d "
            "    openClose=%d "
            "    matchedPrice=%.4f "
            "    matchedAmount=%ld "
            "    matchedMort=%.4f "
            "    speculator=%d "
            "    matchedTime=%s "
            "    matchedID=%s "
            "    localOrderID=%ld "
            "    clientID=%s "
            "    matchType=%ld "
            "    instrumentType=%d "
            "    customCategory=%s "
            "    fee=%.4f "
            "    reservedType1=%d "
            "    reservedType2=%d ",
            p->RequestID,                   //请求ID
            p->X1OrderID,                   //柜台委托号
            p->ExchangeID,                   //交易所ID
            p->InstrumentID,                 //合约代码
            p->BuySellType,                  //买卖
            p->OpenClose,                    //开平
            p->MatchedPrice,                 //成交价格
            p->MatchedAmount,                //成交数量
            p->MatchedMort,                  //成交金额
            p->Speculator,                   //投保类别
            p->MatchedTime,                  //成交时间
            p->MatchedID,                    //成交编号
            p->LocalOrderID,                 //本地委托号
            p->ClientID,                     //交易编码
            p->MatchType,                    //成交类型
            p->InstrumentType,               //合约类型
            p->CustomCategory,               //自定义类别
            p->Fee                          //手续费
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspMatchField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcRspPositionField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspPositionField "
            "    lRequestID=%ld "
            "    accountID=%s "
            "    exchangeID=%s "
            "    instrumentID=%s "
            "    buySellType=%d "
            "    openAvgPrice=%.4f "
            "    positionAvgPrice=%.4f "
            "    positionAmount=%ld "
            "    totalAvaiAmount=%ld "
            "    todayAvaiAmount=%ld "
            "    lastAvaiAmount=%ld "
            "    todayAmount=%ld "
            "    lastAmount=%ld "
            "    tradingAmount=%ld "
            "    datePositionProfitLoss=%.4f "
            "    dateCloseProfitLoss=%.4f "
            "    dPremium=%.4f "
            "    floatProfitLoss=%.4f "
            "    dmargin=%.4f "
            "    speculator=%d "
            "    clientID=%s "
            "    preSettlementPrice=%.4f "
            "    instrumentType=%d ",
            p->RequestID,                   //请求ID
            p->AccountID,                    //资金帐号ID
            p->ExchangeID,                   //交易所代码
            p->InstrumentID,                 //合约号
            p->BuySellType,                  //买卖
            p->OpenAvgPrice,                 //开仓均价
            p->PositionAvgPrice,             //持仓均价
            p->PositionAmount,               //持仓量
            p->TotalAvaiAmount,              //总可用
            p->TodayAvaiAmount,              //今可用
            p->LastAvaiAmount,               //昨可用
            p->TodayAmount,                  //今仓
            p->LastAmount,                   //昨仓
            p->TradingAmount,                //挂单量
            p->DatePositionProfitLoss,       //盯市持仓盈亏
            p->DateCloseProfitLoss,          //盯市平仓盈亏
            p->Premium,                     //权利金
            p->ProfitLoss,              //浮动盈亏
            p->Margin,                      //占用保证金
            p->Speculator,                   //投保类别
            p->ClientID,                     //交易编码
            p->PreSettlementPrice,           //昨结算价
            p->InstrumentType                //合约类型
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspPositionField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcRspPositionDetailField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspPositionDetailField "
            "    lRequestID=%ld "
            "    accountID=%s "
            "    exchangeID=%s "
            "    instrumentID=%s "
            "    buySellType=%d "
            "    openPrice=%.4f "
            "    volume=%ld "
            "    matchID=%s "
            "    matchedDate=%s "
            "    datePositionProfitLoss=%.4f "
            "    dateCloseProfitLoss=%.4f "
            "    floatProfitLoss=%.4f "
            "    dMargin=%.4f "
            "    speculator=%d "
            "    clientID=%s "
            "    preSettlementPrice=%.4f "
            "    instrumentType=%d "
            "    spdOrderID=%ld "
            "    customCategory=%s ",
            p->RequestID,                   //请求ID
            p->AccountID,                    //资金帐号ID
            p->ExchangeID,                   //交易所代码
            p->InstrumentID,                 //合约号
            p->BuySellType,                  //买卖
            p->OpenPrice,                    //开仓价
            p->Volume,                       //手数
            p->MatchID,                      //成交编号
            p->MatchedDate,                  //成交日期
            p->DatePositionProfitLoss,       //盯市持仓盈亏
            p->DateCloseProfitLoss,          //盯市平仓盈亏
            p->ProfitLoss,              //浮动盈亏
            p->Margin,                      //占用保证金
            p->Speculator,                   //投保类别
            p->ClientID,                     //交易编码
            p->PreSettlementPrice,           //昨结算价
            p->InstrumentType,               //合约类型
            p->X1OrderID,                   //柜台委托号
            p->CustomCategory                //自定义类别
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspPositionDetailField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcRspExchangeInstrumentField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspExchangeInstrumentField "
            "    lRequestID=%ld "
            "    exchangeID=%s "
            "    instrumentID=%s "
            "    VarietyName=%s "
            "    instrumentType=%d "
            "    orderTopLimit=%ld "
            "    contractMultiplier=%.4f "
            "    minPriceFluctuation=%.4f "
            "    instrumentMaturity=%s "
            "    upperLimitPrice=%.4f "
            "    lowerLimitPrice=%.4f "
            "    preClosePrice=%.4f "
            "    preSettlementPrice=%.4f "
            "    settlementPrice=%.4f "
            "    preOpenInterest=%ld ",
            p->RequestID,                   //请求ID
            p->ExchangeID,                   //交易所编码
            p->InstrumentID,                 //合约代码
            p->VarietyName,                  //品种名称
            p->InstrumentType,               //合约类型
            p->OrderTopLimit,                //委托上限
            p->ContractMultiplier,           //合约乘数
            p->MinPriceFluctuation,          //最小变动价位
            p->InstrumentMaturity,           //合约最后交易日
            p->UpperLimitPrice,              //涨停板价
            p->LowerLimitPrice,              //跌停板价
            p->PreClosePrice,                //昨收盘
            p->PreSettlementPrice,           //昨结算价
            p->SettlementPrice,              //结算价
            p->PreOpenInterest               //昨持仓量
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspExchangeInstrumentField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcExchangeStatusRtnField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcExchangeStatusRtnField "
            "    exchangeID=%s "
            "    exchangeStatus=%d ",
            p->ExchangeID,                   //交易所
            p->ExchangeStatus                //交易所状态
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcExchangeStatusRtnField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcRspExchangeStatusField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspExchangeStatusField "
            "    exchangeID=%s "
            "    exchangeStatus=%d ",
            p->ExchangeID,                   //交易所
            p->ExchangeStatus                //交易所状态
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspExchangeStatusField <null>");
    }

    return buf;
}

std::string X1DatatypeFormater::ToString(const CX1FtdcRspCapitalField* p)
{
    char buf[2048];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspCapitalField "
            "    requestID=%ld "
            "    accountID=%s "
            "    preEquity=%.4f "
            "    todayEquity=%.4f "
            "    closeProfitLoss=%.4f "
            "    positionProfitLoss=%.4f "
            "    frozenMargin=%.4f "
            "    margin=%.4f "
            "    fee=%.4f "
            "    available=%.4f "
            "    withdraw=%.4f "
            "    riskDegree=%.4f "
            "    todayPremiumIncome=%.4f "
            "    todayPremiumPay=%.4f "
            "    yesterdayPremium=%.4f "
            "    optMarketValue=%.4f "
            "    floatProfitLoss=%.4f "
            "    totFundOut=%.4f "
            "    totFundIn=%.4f ",
            p->RequestID,                    //请求ID
            p->AccountID,                    //资金帐号
            p->PreEquity,                    //上日权益
            p->TodayEquity,                  //当日客户权益
            p->CloseProfitLoss,              //平仓盈亏
            p->PositionProfitLoss,           //持仓盈亏
            p->FrozenMargin,                 //冻结资金
            p->Margin,                       //持仓保证金
            p->Fee,                          //当日手续费
            p->Available,                    //可用资金
            p->Withdraw,                     //可取资金
            p->RiskDegree,                   //风险度
            p->TodayPremiumIncome,           //本日权利金收入
            p->TodayPremiumPay,              //本日权利金付出
            p->YesterdayPremium,             //昨权利金收付
            p->OptMarketValue,               //期权市值
            p->ProfitLoss,              //浮动盈亏
            p->TotalFundOut,                   //总出金
            p->TotalFundIn                    //总入金
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=CX1FtdcRspCapitalField <null>");
    }

    return buf;
}
