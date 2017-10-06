#pragma once

#ifndef DLL_PUBLIC
#define DLL_PUBLIC  __attribute__ ((visibility("default")))
#endif

#include <string>

#pragma pack(push)
#pragma pack(8)

//! 行情全文
struct TapAPIQuoteWhole_MY
{
    char ExchangeNo[11];                            ///< 交易所编码
    char CommodityType;                             ///< 品种类型
    char CommodityNo[11];                           ///< 品种编号
    char ContractNo1[11];                           ///< 合约代码1
    char StrikePrice1[11];                          ///< 执行价1
    char CallOrPutFlag1;                            ///< 看涨看跌标示1
    char ContractNo2[11];                           ///< 合约代码2
    char StrikePrice2[11];                          ///< 执行价2
    char CallOrPutFlag2;                            ///< 看涨看跌标示2
    char CurrencyNo[11];                            ///< 币种编号
    char TradingState;                              ///< 交易状态。1,集合竞价;2,集合竞价撮合;3,连续交易;4,交易暂停;5,闭市
    char DateTimeStamp[24];                         ///< 时间戳
    double QPreClosingPrice;                        ///< 昨收盘价
    double QPreSettlePrice;                         ///< 昨结算价
    unsigned long long QPrePositionQty;             ///< 昨持仓量
    double QOpeningPrice;                           ///< 开盘价
    double QLastPrice;                              ///< 最新价
    double QHighPrice;                              ///< 最高价
    double QLowPrice;                               ///< 最低价
    double QHisHighPrice;                           ///< 历史最高价
    double QHisLowPrice;                            ///< 历史最低价
    double QLimitUpPrice;                           ///< 涨停价
    double QLimitDownPrice;                         ///< 跌停价
    unsigned long long QTotalQty;                   ///< 当日总成交量
    double QTotalTurnover;                          ///< 当日成交金额
    unsigned long long QPositionQty;                ///< 持仓量
    double QAveragePrice;                           ///< 均价
    double QClosingPrice;                           ///< 收盘价
    double QSettlePrice;                            ///< 结算价
    unsigned long long QLastQty;                    ///< 最新成交量
    double QBidPrice[20];                           ///< 买价1-20档
    unsigned long long QBidQty[20];                 ///< 买量1-20档
    double QAskPrice[20];                           ///< 卖价1-20档
    unsigned long long QAskQty[20];                 ///< 卖量1-20档
    double QImpliedBidPrice;                        ///< 隐含买价
    unsigned long long QImpliedBidQty;              ///< 隐含买量
    double QImpliedAskPrice;                        ///< 隐含卖价
    unsigned long long QImpliedAskQty;              ///< 隐含卖量
    double QPreDelta;                               ///< 昨虚实度
    double QCurrDelta;                              ///< 今虚实度
    unsigned long long QInsideQty;                  ///< 内盘量
    unsigned long long QOutsideQty;                 ///< 外盘量
    double QTurnoverRate;                           ///< 换手率
    unsigned long long Q5DAvgQty;                   ///< 五日均量
    double QPERatio;                                ///< 市盈率
    double QTotalValue;                             ///< 总市值
    double QNegotiableValue;                        ///< 流通市值
    long long QPositionTrend;                       ///< 持仓走势
    double QChangeSpeed;                            ///< 涨速
    double QChangeRate;                             ///< 涨幅
    double QChangeValue;                            ///< 涨跌值
    double QSwing;                                  ///< 振幅
    unsigned long long QTotalBidQty;                ///< 委买总量
    unsigned long long QTotalAskQty;                ///< 委卖总量

    std::string
    GetQuoteTime() const
    {
        return std::string(DateTimeStamp + 11, 12);
    }
};

#pragma pack(pop)
