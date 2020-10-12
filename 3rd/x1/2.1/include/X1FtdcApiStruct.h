/**
* Copyright (C) 2019-2021, 大连飞创信息技术有限公司
* @brief    定义接口所需的数据接口
* @version  2.1.17.0
* @author   Xone 项目组
* @date     2019年8月16日
* @note     Xone 2.0柜台用户API帮助手册
*/


#ifndef X1FTDCAPISTRUCT_H_
#define X1FTDCAPISTRUCT_H_

#include "X1FtdcApiDataType.h"

#define APISTRUCT

///心跳包
struct APISTRUCT CX1FtdcTimeOutField
{
    TX1FtdcRequestIDType                  RequestID;                   /**< 请求ID*/
};

///请求报单数据类型(基本报单)
struct APISTRUCT CX1FtdcInsertOrderField
{
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账户*/
    TX1FtdcLocalOrderIDType
    LocalOrderID;                 /**< 本地委托号, 由API使用者维护，在同一个会话中不能重复*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcPriceType
    InsertPrice;                  /**< 报单价格, 当报单类型为市价时，该字段不起作用*/
    TX1FtdcAmountType                     OrderAmount;                  /**< 报单数量*/
    TX1FtdcBuySellTypeType
    BuySellType;                  /**< 买卖标志, 期权执行时：1为不对冲，2为对冲*/
    TX1FtdcOpenCloseTypeType              OpenCloseType;                /**< 开平标志*/
    TX1FtdcSpeculatorType                 Speculator;                   /**< 投保类型, 支持投机、套利、套保*/
    TX1FtdcInsertType                     InsertType;                   /**< 自动单类别(默认为普通订单)*/
    TX1FtdcOrderTypeType
    OrderType;                    /**< 报单类型, 支持限价 、市价；上期所合约不支持市价，均按限价进行处理*/
    TX1FtdcOrderPropertyType              OrderProperty;                /**< 报单附加属性, 支持None、FAK、FOK*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型, 可选值：期货、期权*/
    TX1FtdcAmountType                     MinMatchAmount;               /**< 最小成交量*/
    TX1FtdcReservedType                   ReservedType2;                /**< 预留字段2*/
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcCustomCategoryType             CustomCategory;               /**< 自定义类别*/
    TX1FtdcPriceType                      ProfitLossPrice;              /**< 止盈止损价格*/
    TX1FtdcSeatID                         SeatID;                       /**< 交易所席位编号*/
};

///撤单数据类型
struct APISTRUCT CX1FtdcCancelOrderField
{
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账户ID(此版本不用)*/
    TX1FtdcX1OrderIDType                  X1OrderID;                    /**< 柜台委托号*/
    TX1FtdcLocalOrderIDType               LocalOrderID;                 /**< 本地委托号*/
    TX1FtdcOrderSysIDType
    OrderSysID;                   /**< 订单系统委托号(用于双边报价单边撤单)*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcSessionIDType                  SessionID;                    /**< 会话ID*/
};

///委托响应类型
struct APISTRUCT CX1FtdcRspOperOrderField
{
    TX1FtdcLocalOrderIDType               LocalOrderID;                 /**< 本地委托号*/
    TX1FtdcX1OrderIDType                  X1OrderID;                    /**< 柜台委托号*/
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  /**< 委托状态*/
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcPriceType
    Fee;                          /**< 手续费,该字段仅供下单时使用(此版本不用)*/
    TX1FtdcPriceType
    Margin;                       /**< 冻结保证金,该字段仅供下单时使用*/
    TX1FtdcCustomCategoryType             CustomCategory;               /**< 自定义类别*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账户ID(此版本不用)*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码(此版本不用)*/
    TX1FtdcSessionIDType                  SessionID;                    /**< 会话ID*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所(此版本不用)*/
    TX1FtdcBuySellTypeType                BuySellType;                  /**< 买卖，该字段仅供下单时使用*/
    TX1FtdcOpenCloseTypeType              OpenCloseType;                /**< 开平(撤单时此版本不用)*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
    TX1FtdcSpeculatorType                 Speculator;                   /**< 投资类别*/
    TX1FtdcPriceType                      InsertPrice;                  /**< 委托价(撤单时此版本不用)*/
    TX1FtdcPriceType                      ProfitLossPrice;              /**< 止盈止损价格*/
    TX1FtdcAmountType                     MinMatchAmount;               /**< 最小成交量*/
    TX1FtdcAmountType                     OrderAmount;                  /**< 委托数量(撤单时此版本不用)*/
    TX1FtdcInsertType                     InsertType;                   /**< 自动单类别*/
    TX1FtdcOrderTypeType                  OrderType;                    /**< 订单类型(撤单时此版本不用)*/
    TX1FtdcOrderPropertyType              OrderProperty;                /**< 订单属性*/
    TX1FtdcClientIDType                   ClientID;                     /**< 交易编码(此版本不用)*/
};

///查询资金数据类型
struct APISTRUCT CX1FtdcQryCapitalField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账户ID*/
};

///查询持仓数据类型
struct APISTRUCT CX1FtdcQryPositionField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账户ID*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
};

///交易所合约
/**
* @brief 交易所合约
* @details 当ExchangeID为空时，表示查询各交易所的所有合约代码，也可指定查询某交易所的所有合约代码信息，但该接口不能查询到套利合约代码
*
*/
struct APISTRUCT CX1FtdcQryExchangeInstrumentField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账户ID*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所编码*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
};

/// 用户登录数据类型
struct APISTRUCT CX1FtdcReqUserLoginField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID [NM]*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账户ID [M]*/
    TX1FtdcPasswordType                   Password;                     /**< 密码 [M]*/
    TX1FtdcCompanyIDType                  CompanyID;                    /**< 厂商ID(此版本不用)[BLK]*/
    TX1FtdcAppIDType                      AppID;                        /**< AppID [M]*/
    TX1FtdcAuthCodeType                   AuthCode;                     /**< 授权码 [M]*/

};

/// 用户登录返回信息
struct APISTRUCT CX1FtdcRspUserLoginField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金帐号ID*/
    TX1FtdcAccountLoginResultType         LoginResult;                  /**< 登录结果*/
    TX1FtdcLocalOrderIDType               InitLocalOrderID;             /**< 初始本地委托号(此版本不用)*/
    TX1FtdcSessionIDType                  SessionID;                    /**< sessionID(会话ID)*/
    TX1FtdcErrorIDType                    ErrorID;                      /**< 错误ID*/
    TX1FtdcErrorMsgInfoType               ErrorMsg;                     /**< 错误信息*/
    TX1FtdcTimeType                       DCEtime;                      /**< 大商所时间*/
    TX1FtdcTimeType                       SHFETime;                     /**< 上期所时间*/
    TX1FtdcTimeType                       CFFEXTime;                    /**< 中金所时间*/
    TX1FtdcTimeType                       CZCETime;                     /**< 郑商所时间*/
    TX1FtdcTimeType                       INETime;                      /**< 上能所时间*/
};

///用户退出类型
struct APISTRUCT CX1FtdcReqUserLogoutField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType
    AccountID;                    /**< 资金帐号ID [NM] 用户可以传空或传入正确的登录号*/
    TX1FtdcSessionIDType                  SessionID;                    /**< 会话ID(此版本不用)[BLK]*/
};

///委托回报
struct APISTRUCT CX1FtdcRspPriOrderField
{
    TX1FtdcLocalOrderIDType
    LocalOrderID;                 /**< 本地委托号 在系统重启后，尽量恢复，不确保准确*/
    TX1FtdcX1OrderIDType                  X1OrderID;                    /**< 柜台委托号*/
    TX1FtdcOrderSysIDType                 OrderSysID;                   /**< 报单编号*/
    TX1FtdcOrderAnswerStatusType
    OrderStatus;                  /**< 委托状态  在FAK FOK情况返回交易所已经接受，但尚未成交8，失败返回7，其他情况一律返回未成交3*/
    TX1FtdcSessionIDType                  SessionID;                    /**< 会话ID*/
    TX1FtdcDateType                       SuspendTime;                  /**< 挂起时间(此版本不用)*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所*/
    TX1FtdcBuySellTypeType                BuySellType;                  /**< 买卖*/
    TX1FtdcOpenCloseTypeType              OpenCloseType;                /**< 开平*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
    TX1FtdcSpeculatorType                 Speculator;                   /**< 投资类别*/
    TX1FtdcPriceType                      InsertPrice;                  /**< 委托价*/
    TX1FtdcPriceType                      ProfitLossPrice;              /**< 止盈止损价格*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账号(此版本不用)*/
    TX1FtdcAmountType                     CancelAmount;                 /**< 撤单数量*/
    TX1FtdcAmountType                     OrderAmount;                  /**< 委托数量*/
    TX1FtdcInsertType                     InsertType;                   /**< 自动单类别*/
    TX1FtdcOrderTypeType                  OrderType;                    /**< 报单类型*/
    TX1FtdcX1OrderIDType                  ExtX1OrderID;                 /**< 算法单编号(此版本不用)*/
    TX1FtdcReservedType                   ReservedType2;                /**< 预留字段2(此版本不用)*/
    TX1FtdcCustomCategoryType             CustomCategory;               /**< 自定义类别*/
    TX1FtdcOrderPropertyType              OrderProperty;                /**< 订单属性*/
    TX1FtdcAmountType                     MinMatchAmount;               /**< 最小成交量*/
    TX1FtdcClientIDType                   ClientID;                     /**< 交易编码*/
    TX1FtdcErrorIDType                    ErrorID;                      /**< 错误ID*/
    TX1FtdcErrorMsgInfoType               StatusMsg;                    /**< 状态信息*/
    TX1FtdcExtOrderType                   ExtOrderType;                 /**< 条件单类型(此版本不用)*/
    TX1FtdcLocalOrderIDType               PriFlowNo;                    /**< 私有流编号*/
};

///成交回报
struct APISTRUCT CX1FtdcRspPriMatchInfoField
{
    TX1FtdcLocalOrderIDType               LocalOrderID;                 /**< 本地委托号*/
    TX1FtdcOrderSysIDType                 OrderSysID;                   /**< 报单编号(交易所报单编号)*/
    TX1FtdcMatchIDType                    MatchID;                      /**< 成交编号*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcBuySellTypeType                BuySellType;                  /**< 买卖*/
    TX1FtdcOpenCloseTypeType              OpenCloseType;                /**< 开平标志*/
    TX1FtdcPriceType                      MatchedPrice;                 /**< 成交价格*/
    TX1FtdcAmountType                     OrderAmount;                  /**< 委托数量*/
    TX1FtdcAmountType                     MatchedAmount;                /**< 成交数量*/
    TX1FtdcDateType                       MatchedTime;                  /**< 成交时间*/
    TX1FtdcPriceType                      InsertPrice;                  /**< 报价*/
    TX1FtdcX1OrderIDType                  X1OrderID;                    /**< 柜台委托号*/
    TX1FtdcMatchType                      MatchType;                    /**< 成交类型*/
    TX1FtdcSpeculatorType                 Speculator;                   /**< 投保*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所ID*/
    TX1FtdcFeeType                        Fee;                          /**< 手续费(此版本不用)*/
    TX1FtdcSessionIDType                  SessionID;                    /**< 会话标识*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账号(此版本不用)*/
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  /**< 申报结果*/
    TX1FtdcPriceType
    Margin;                       /**< 开仓为保证金,平仓为解冻保证金(此版本不用)*/
    TX1FtdcPriceType
    FrozenCapita;                 /**< 成交解冻委托冻结的资金(此版本不用)*/
    TX1FtdcAdjustmentInfoType
    AdjustmentInfo;               /**< 组合或对锁的保证金调整信息,格式:[合约代码,买卖标志,投资类别,调整金额;](此版本不用)*/
    TX1FtdcCustomCategoryType             CustomCategory;               /**< 自定义类别*/
    TX1FtdcPriceType                      Turnover;                     /**< 成交金额(此版本不用)*/
    TX1FtdcOrderTypeType                  OrderType;                    /**< 报单类型*/
    TX1FtdcInsertType                     InsertType;                   /**< 自动单类别*/
    TX1FtdcClientIDType                   ClientID;                     /**< 交易编码(此版本不用)*/
    TX1FtdcLocalOrderIDType               PriFlowNo;                    /**< 私有流编号*/
};

///撤单回报
struct APISTRUCT CX1FtdcRspPriCancelOrderField
{
    TX1FtdcLocalOrderIDType               LocalOrderID;                 /**< 本地委托号*/
    TX1FtdcOrderSysIDType                 OrderSysID;                   /**< 报单编号*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码(此版本不用)*/
    TX1FtdcPriceType                      InsertPrice;                  /**< 报单价格*/
    TX1FtdcBuySellTypeType                BuySellType;                  /**< 买卖类型*/
    TX1FtdcOpenCloseTypeType              OpenCloseType;                /**< 开平标志*/
    TX1FtdcAmountType                     CancelAmount;                 /**< 撤单数量*/
    TX1FtdcX1OrderIDType                  X1OrderID;                    /**< 柜台委托号*/
    TX1FtdcSpeculatorType                 Speculator;                   /**< 投保*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所ID*/
    TX1FtdcDateType                       CanceledTime;                 /**< 撤单时间*/
    TX1FtdcSessionIDType                  SessionID;                    /**< 会话标识*/
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  /**< 申报结果*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账号(此版本不用)*/
    TX1FtdcAmountType                     OrderAmount;                  /**< 委托数量*/
    TX1FtdcPriceType                      Margin;                       /**< 保证金(此版本不用)*/
    TX1FtdcPriceType                      Fee;                          /**< 手续费(此版本不用)*/
    TX1FtdcCustomCategoryType             CustomCategory;               /**< 自定义类别*/
    TX1FtdcPriceType                      ProfitLossPrice;              /**< 止盈止损价格*/
    TX1FtdcAmountType                     MinMatchAmount;               /**< 最小成交量*/
    TX1FtdcInsertType                     InsertType;                   /**< 自动单类别*/
    TX1FtdcClientIDType                   ClientID;                     /**< 交易编码(此版本不用)*/
    TX1FtdcErrorIDType                    ErrorID;                      /**< 错误ID*/
    TX1FtdcErrorMsgInfoType               StatusMsg;                    /**< 状态信息*/
    TX1FtdcOrderPropertyType              OrderProperty;                /**< 报单附加属性*/
    TX1FtdcLocalOrderIDType               PriFlowNo;                    /**< 私有流编号*/
};

///错误信息
struct APISTRUCT CX1FtdcRspErrorField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcSessionIDType                  SessionID;                    /**< 会话标识*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账号*/
    TX1FtdcErrorIDType                    ErrorID;                      /**< 错误ID*/
    TX1FtdcX1OrderIDType                  X1OrderID;                    /**< 柜台委托号*/
    TX1FtdcLocalOrderIDType               LocalOrderID;                 /**< 本地委托号*/
    TX1FtdcErrorMsgInfoType               ErrorMsg;                     /**< 错误信息*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
};

///返回资金信息
struct APISTRUCT CX1FtdcRspCapitalField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金帐号*/
    TX1FtdcEquityType                     PreEquity;                    /**< 上日权益*/
    TX1FtdcEquityType                     TodayEquity;                  /**< 当日客户权益*/
    TX1FtdcProfitLossType                 CloseProfitLoss;              /**< 平仓盈亏*/
    TX1FtdcProfitLossType                 PositionProfitLoss;           /**< 持仓盈亏*/
    TX1FtdcProfitLossType                 FrozenMargin;                 /**< 冻结资金*/
    TX1FtdcProfitLossType                 Margin;                       /**< 持仓保证金*/
    TX1FtdcProfitLossType                 Fee;                          /**< 当日手续费*/
    TX1FtdcProfitLossType                 Available;                    /**< 可用资金*/
    TX1FtdcProfitLossType                 Withdraw;                     /**< 可取资金*/
    TX1FtdcRiskDegreeType                 RiskDegree;                   /**< 风险度*/
    TX1FtdcPremiumType                    TodayPremiumIncome;           /**< 本日权利金收入*/
    TX1FtdcPremiumType                    TodayPremiumPay;              /**< 本日权利金付出*/
    TX1FtdcPremiumType                    YesterdayPremium;             /**< 昨权利金收付*/
    TX1FtdcMarketValueType                OptMarketValue;               /**< 期权市值*/
    TX1FtdcProfitLossType                 ProfitLoss;                   /**< 浮动盈亏*/
    TX1FtdcProfitLossType                 TotalFundOut;                 /**< 总出金*/
    TX1FtdcProfitLossType                 TotalFundIn;                  /**< 总入金*/
    TX1FtdcRatioType                      UpFloathingRatio;             /**< 浮动比例上限*/
    TX1FtdcRatioType                      DownFloathingRatio;           /**< 浮动比例下限*/
    TX1FtdcRatioType                      AllocateRatio;                /**< 分配比例*/
    TX1FtdcCurrencyType                   CurrencyID;                   /**<币种代码*/
    TX1FtdcProfitLossType                 Mortgage;                     /**<质押金额(此版本不用)*/
    TX1FtdcProfitLossType                 FundMortgageIn;               /**<货币质入金额*/
    TX1FtdcProfitLossType                 FundMortgageOut;              /**<货币质出金额*/
    TX1FtdcProfitLossType                 FundMortgageAvailable;        /**<货币质押余额*/
    TX1FtdcProfitLossType                 YesterdayMargin;              /**<昨保证金*/
};

///返回持仓信息
struct APISTRUCT CX1FtdcRspPositionField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金帐号ID*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所代码*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约号*/
    TX1FtdcBuySellTypeType                BuySellType;                  /**< 买卖*/
    TX1FtdcPriceType                      OpenAvgPrice;                 /**< 开仓均价*/
    TX1FtdcPriceType                      PositionAvgPrice;             /**< 持仓均价*/
    TX1FtdcAmountType                     PositionAmount;               /**< 持仓量*/
    TX1FtdcAmountType                     TotalAvaiAmount;              /**< 总可用*/
    TX1FtdcAmountType                     TodayAvaiAmount;              /**< 今可用*/
    TX1FtdcAmountType                     LastAvaiAmount;               /**< 昨可用*/
    TX1FtdcAmountType                     TodayAmount;                  /**< 今仓*/
    TX1FtdcAmountType                     LastAmount;                   /**< 昨仓*/
    TX1FtdcAmountType                     TradingAmount;                /**< 平今挂单量*/
    TX1FtdcProfitLossType                 DatePositionProfitLoss;       /**< 盯市持仓盈亏*/
    TX1FtdcProfitLossType                 DateCloseProfitLoss;          /**< 盯市平仓盈亏*/
    TX1FtdcProfitLossType                 Premium;                      /**< 权利金*/
    TX1FtdcProfitLossType                 ProfitLoss;                   /**< 浮动盈亏*/
    TX1FtdcProfitLossType                 Margin;                       /**< 占用保证金*/
    TX1FtdcSpeculatorType                 Speculator;                   /**< 投保类别*/
    TX1FtdcClientIDType                   ClientID;                     /**< 交易编码*/
    TX1FtdcPriceType                      PreSettlementPrice;           /**< 昨结算价*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
    TX1FtdcAmountType                     YesterdayTradingAmount;       /**< 平昨挂单量*/
    TX1FtdcProfitLossType                 OptionValue;                  /**< 期权市值*/
};

///用户退出返回信息
struct APISTRUCT CX1FtdcRspUserLogoutInfoField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账户ID*/
    TX1FtdcAccountLogoutResultType        LogoutResult;                 /**< 退出结果*/
    TX1FtdcErrorIDType                    ErrorID;                      /**< 错误ID*/
    TX1FtdcErrorMsgInfoType               ErrorMsg;                     /**< 错误信息*/
};

///指定的合约
struct APISTRUCT CX1FtdcQrySpecificInstrumentField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账户ID*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所ID*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
};

///交易所合约返回信息
struct APISTRUCT CX1FtdcRspExchangeInstrumentField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所编码*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcVarietyNameType                VarietyName;                  /**< 品种名称*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
    TX1FtdcAmountType                     OrderTopLimit;                /**< 限价委托上限*/
    TX1FtdcAmountType                     MktOrderTopLimit;             /**< 市价委托上限*/
    TX1FtdcPriceType                      ContractMultiplier;           /**< 合约乘数*/
    TX1FtdcPriceType                      MinPriceFluctuation;          /**< 最小变动价位*/
    TX1FtdcInstrumentMaturityType         InstrumentMaturity;           /**< 合约最后交易日*/
    TX1FtdcPriceType                      UpperLimitPrice;              /**< 涨停板价*/
    TX1FtdcPriceType                      LowerLimitPrice;              /**< 跌停板价*/
    TX1FtdcPriceType                      PreClosePrice;                /**< 昨收盘*/
    TX1FtdcPriceType                      PreSettlementPrice;           /**< 昨结算价*/
    TX1FtdcPriceType                      SettlementPrice;              /**< 结算价*/
    TX1FtdcAmountType                     PreOpenInterest;              /**< 昨持仓量*/
    TX1FtdcInstrumentPrefixType           InstrumentPrefix;             /**< 合约前缀*/
    TX1FtdcInstrumenExpirationDateType    InstrumentExpiration;         /**< 合约到期日*/
    TX1FtdcInstrumentIDType               Underlying;                   /**< 期权对应的标的合约代码*/
    TX1FtdcOptionTypeType                 OptionType;                   /**< 期权类型*/
    TX1FtdcPriceType                      StrikePrice;                  /**< 执行价格*/
    TX1FtdcRiskDegreeType                 ExchangeRiskDegree;           /**< 交易所期权最低保障风险系数*/
    TX1FtdcPriceType                      MinMargin;                    /**< 单位(手)期权合约最小保证金*/
    TX1FtdcAmountType                     TradeSize;                    /**< 期权开仓单位*/
};

///委托查询数据结构
/**
* @brief 委托查询数据结构
* @details 这里有丰富的查询条件设置，如果想查询所有委托记录，则只需要输入AccountID和InstrumentType即可。
*          查询处于某状态的报单，则中需要将OrderStatus设置为相应报单状态的值即可，如3表示未成交在队列的单子。(注意OrderStatus和OrderType查询条件该版本暂不支持)
*          查询指定报单类型的单子，如限价单或市价单等。
*          该版本不支持按柜台委托号X1OrderID 或本地委托号LocalOrderID查询。
*          废单仅支持下单的委托查询。
*          废单的价格均显示为0.
*/
struct APISTRUCT CX1FtdcQryOrderField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账户ID*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
    TX1FtdcCustomCategoryType             CustomCategory;               /**< 自定义类别*/
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  /**< 委托状态*/
    TX1FtdcOrderTypeType                  OrderType;                    /**< 报单类型*/
    TX1FtdcX1OrderIDType                  X1OrderID;                    /**< 柜台委托号*/
    TX1FtdcLocalOrderIDType               LocalOrderID;                 /**< 本地委托号*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
};

///成交查询数据结构
/**
* @brief 成交查询数据结构
* @details 这里有丰富的查询条件设置，如果想查询所有委托记录，则只需要输入AccountID和InstrumentType即可。（注意OrderType查询条件该版本暂不支持）
*/
struct APISTRUCT CX1FtdcQryMatchField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账户ID*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
    TX1FtdcCustomCategoryType             CustomCategory;               /**< 自定义类别*/
    TX1FtdcOrderTypeType                  OrderType;                    /**< 报单类型*/
    TX1FtdcX1OrderIDType                  X1OrderID;                    /**< 柜台委托号*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
};

///委托查询响应数据结构
struct APISTRUCT CX1FtdcRspOrderField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcX1OrderIDType                  X1OrderID;                    /**< 柜台委托号*/
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  /**< 委托状态*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcBuySellTypeType                BuySellType;                  /**< 买卖*/
    TX1FtdcOpenCloseTypeType              OpenClose;                    /**< 开平标志*/
    TX1FtdcPriceType                      InsertPrice;                  /**< 委托价*/
    TX1FtdcAmountType                     OrderAmount;                  /**< 委托数量*/
    TX1FtdcPriceType                      MatchedPrice;                 /**< 成交价格*/
    TX1FtdcAmountType                     MatchedAmount;                /**< 成交数量*/
    TX1FtdcAmountType                     CancelAmount;                 /**< 撤单数量*/
    TX1FtdcInsertType                     InsertType;                   /**< 自动单类别*/
    TX1FtdcSpeculatorType                 Speculator;                   /**< 投保*/
    TX1FtdcDateType                       CommTime;                     /**< 委托时间*/
    TX1FtdcDateType                       SubmitTime;                   /**< 申报时间*/
    TX1FtdcClientIDType                   ClientID;                     /**< 交易编码*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所ID*/
    TX1FtdcFrontAddrType                  OperStation;                  /**< 委托地址  ip + mac*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 客户号*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
    TX1FtdcSessionIDType                  SessionId;                    /**< 会话ID*/
    TX1FtdcReservedType                   ReservedType2;                /**< 预留字段2*/
    TX1FtdcOrderSysIDType                 OrderSysID;                   /**< 主场单号*/
    TX1FtdcCustomCategoryType             CustomCategory;               /**< 自定义类别*/
    TX1FtdcPriceType                      Margin;                       /**< 保证金*/
    TX1FtdcPriceType                      Fee;                          /**< 手续费*/
    TX1FtdcLocalOrderIDType
    LocalOrderID;                 /**< 本地委托号 是下单时该单子的本地委托号，如果同一账号从多个客户端下单，则查询返回的LocalOrderID可能是重复的*/
    TX1FtdcPriceType                      ProfitLossPrice;              /**< 止损止盈价*/
    TX1FtdcOrderTypeType                  OrderType;                    /**< 报单类别*/
    TX1FtdcOrderPropertyType              OrderProperty;                /**< 订单属性*/
};

///成交查询数据响应
struct APISTRUCT CX1FtdcRspMatchField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcX1OrderIDType                  X1OrderID;                    /**< 柜台委托号*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账号*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所ID*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcBuySellTypeType                BuySellType;                  /**< 买卖*/
    TX1FtdcOpenCloseTypeType              OpenClose;                    /**< 开平*/
    TX1FtdcPriceType                      MatchedPrice;                 /**< 成交价格*/
    TX1FtdcAmountType                     MatchedAmount;                /**< 成交数量*/
    TX1FtdcPriceType                      MatchedMort;                  /**< 成交金额*/
    TX1FtdcSpeculatorType                 Speculator;                   /**< 投保类别*/
    TX1FtdcDateType                       MatchedTime;                  /**< 成交时间*/
    TX1FtdcMatchIDType                    MatchedID;                    /**< 成交编号*/
    TX1FtdcLocalOrderIDType               LocalOrderID;                 /**< 本地委托号*/
    TX1FtdcClientIDType                   ClientID;                     /**< 交易编码*/
    TX1FtdcMatchType                      MatchType;                    /**< 成交类型*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
    TX1FtdcSessionIDType                  SessionId;                    /**< 会话ID*/
    TX1FtdcReservedType                   ReservedType2;                /**< 预留字段2*/
    TX1FtdcCustomCategoryType             CustomCategory;               /**< 自定义类别*/
    TX1FtdcPriceType                      Fee;                          /**< 手续费*/
    TX1FtdcOrderTypeType                  OrderType;                    /**< 报单类型*/
    TX1FtdcOrderSysIDType                 OrderSysID;                   /**< 主场单号*/
};

///返回合约信息数据结构
struct APISTRUCT CX1FtdcRspSpecificInstrumentField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求编号*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcRatioType                      LongMarginRatio;              /**< 多头保证金率*/
    TX1FtdcRatioType                      ShortMarginRatio;             /**< 空头保证金率*/
    TX1FtdcPriceType                      LongMarginRatioByVolume;      /**< 多头保证金费(定额)*/
    TX1FtdcPriceType                      ShortMarginRatioByVolume;     /**< 空头保证金费(定额)*/
    TX1FtdcRatioType                      OpenFeeVolRatio;              /**< 开仓手续费 按手数计算*/
    TX1FtdcRatioType                      CloseFeeVolRatio;             /**< 平仓手续费 按手数计算*/
    TX1FtdcRatioType                      CloseTodayFeeVolRatio;        /**< 平今手续费 按手数计算*/
    TX1FtdcRatioType                      OpenFeeAmtRatio;              /**< 开仓手续费率 按金额计算*/
    TX1FtdcRatioType                      CloseFeeAmtRatio;             /**< 平仓手续费率 按金额计算*/
    TX1FtdcRatioType                      CloseTodayFeeAmtRatio;        /**< 平今手续费率 按金额计算*/
    TX1FtdcAmountType                     OrderTopLimit;                /**< 限价委托上限*/
    TX1FtdcPriceType                      ContractMultiplier;           /**< 合约乘数*/
    TX1FtdcPriceType                      MinimumPriceChange;           /**< 最小变动价位*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
    TX1FtdcInstrumentMaturityType         InstrumentMaturity;           /**< 合约最后交易日*/
    TX1FtdcComputeModeType                ComputeMode;                  /**< 计算方式*/
    TX1FtdcPriceType                      AtMoneyNorm;                  /**< 平值按定额*/
    TX1FtdcPriceType                      UpperLimitPrice;              /**< 涨停板价*/
    TX1FtdcPriceType                      LowerLimitPrice;              /**< 跌停板价*/
    TX1FtdcPriceType                      PreClosePrice;                /**< 昨收盘*/
    TX1FtdcPriceType                      PreSettlementPrice;           /**< 昨结算价*/
    TX1FtdcPriceType                      SettlementPrice;              /**< 结算价*/
    TX1FtdcAmountType                     PreOpenInterest;              /**< 昨持仓量*/
    TX1FtdcRatioType
    OptExecRatio;                 /**< 期权：行权按比例 期货：交割按比例*/
    TX1FtdcRatioType
    OptExecRatioPerVol;           /**< 期权：行权按定额 期货：交割按定额*/
    TX1FtdcCalculateMode                  CalculateMode;                /**< 冻结资金计算方式*/
    TX1FtdcPriceType                      ReferencePrice;               /**< 参考价格*/
};

///查询持仓明细
struct APISTRUCT CX1FtdcQryPositionDetailField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账户ID*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
};

///查询持仓明细响应
struct APISTRUCT CX1FtdcRspPositionDetailField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金帐号ID*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所代码*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约号*/
    TX1FtdcBuySellTypeType                BuySellType;                  /**< 买卖*/
    TX1FtdcPriceType                      OpenPrice;                    /**< 开仓价*/
    TX1FtdcAmountType                     Volume;                       /**< 手数*/
    TX1FtdcMatchIDType                    MatchID;                      /**< 成交编号*/
    TX1FtdcDateType                       MatchedDate;                  /**< 成交日期*/
    TX1FtdcProfitLossType                 DatePositionProfitLoss;       /**< 盯市持仓盈亏*/
    TX1FtdcProfitLossType                 DateCloseProfitLoss;          /**< 盯市平仓盈亏*/
    TX1FtdcProfitLossType                 ProfitLoss;                   /**< 浮动盈亏*/
    TX1FtdcProfitLossType                 Margin;                       /**< 占用保证金*/
    TX1FtdcSpeculatorType                 Speculator;                   /**< 投保类别*/
    TX1FtdcClientIDType                   ClientID;                     /**< 交易编码*/
    TX1FtdcPriceType                      PreSettlementPrice;           /**< 昨结算价*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
    TX1FtdcX1OrderIDType                  X1OrderID;                    /**< 柜台委托号*/
    TX1FtdcCustomCategoryType             CustomCategory;               /**< 自定义类别*/
    TX1FtdcAmountType                     CloseOrderVol;                /**< 平仓委托数量*/
    TX1FtdcAmountType                     CloseMatchVol;                /**< 平仓成交数量*/
    TX1FtdcPositionDateType               PositionDateType;             /**< 持仓日期类型*/
};

///交易状态查询请求
struct APISTRUCT CX1FtdcQryExchangeStatusField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所编码*/
};

///交易所状态查询响应
struct APISTRUCT CX1FtdcRspExchangeStatusField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcExchangeStatusType             ExchangeStatus;               /**< 交易所状态*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所编码*/
};

///交易所状态通知
struct APISTRUCT CX1FtdcExchangeStatusRtnField
{
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcExchangeStatusType             ExchangeStatus;               /**< 交易所状态*/
};

///做市商报单请求
struct APISTRUCT CX1FtdcQuoteInsertField
{
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账号*/
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcLocalOrderIDType               LocalOrderID;                 /**< 本地委托号*/
    TX1FtdcInsertType                     InsertType;                   /**< 自动单类别*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcQuoteIDType                    QuoteID;                      /**< 询价编号*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
    TX1FtdcAmountType                     BuyOrderAmount;               /**< 报单数量(买)*/
    TX1FtdcAmountType                     SellOrderAmount;              /**< 报单数量(卖)*/
    TX1FtdcPriceType                      BuyInsertPrice;               /**< 委托价格(买)*/
    TX1FtdcPriceType                      SellInsertPrice;              /**< 委托价格(卖)*/
    TX1FtdcOpenCloseTypeType              BuyOpenCloseType;             /**< 开平标志(买)*/
    TX1FtdcOpenCloseTypeType              SellOpenCloseType;            /**< 开平标志(卖)*/
    TX1FtdcSpeculatorType                 BuySpeculator;                /**< 投资类别(买)*/
    TX1FtdcSpeculatorType                 SellSpeculator;               /**< 投资类别(卖)*/
    TX1FtdcStayTimeType
    StayTime;                     /**< 停留时间，仅支持郑州。其它情况可设置为0*/
    TX1FtdcCustomCategoryType             CustomCategory;               /**< 自定义类别*/
};

///批量撤单请求
struct APISTRUCT CX1FtdcCancelAllOrderField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账户ID(此版本不适用)*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所编码(此版本不适用)*/
};

///询价请求
struct APISTRUCT CX1FtdcForQuoteField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账户ID*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
};

///做市商报单响应
struct APISTRUCT CX1FtdcQuoteRspField
{
    TX1FtdcLocalOrderIDType               LocalOrderID;                 /**< 本地委托号*/
    TX1FtdcX1OrderIDType                  X1OrderID;                    /**< 柜台委托号*/
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcPriceType
    Fee;                          /**< 手续费(仅报价使用)(此版本不适用)*/
    TX1FtdcPriceType                      Margin;                       /**< 保证金(仅报价使用)*/
    TX1FtdcDateType
    OrderTime;                    /**< 委托时间(仅报价使用)(此版本不适用)*/
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  /**< 委托状态*/
    TX1FtdcCustomCategoryType             CustomCategory;               /**< 自定义类别*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码(此版本不适用)*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账号(此版本不适用)*/
    TX1FtdcQuoteIDType                    QuoteID;                      /**< 询价编号(仅报单响应使用)*/
    TX1FtdcSessionIDType                  SessionID;                    /**< 会话ID*/
    TX1FtdcClientIDType                   ClientID;                     /**< 交易编码(此版本不适用)*/
};

///做市商报单回报
struct APISTRUCT CX1FtdcQuoteRtnField
{
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所*/
    TX1FtdcClientIDType                   ClientID;                     /**< 交易编码*/
    TX1FtdcOrderSysIDType                 OrderSysID;                   /**< 报单编号*/
    TX1FtdcOrderSysIDType                 OrderSysIDTwo;                /**< 双边第二边*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcLocalOrderIDType               LocalOrderID;                 /**< 本地委托号*/
    TX1FtdcSeatCodeType                   SeatCode;                     /**< 席位代码*/
    TX1FtdcOpenCloseTypeType              BuyOpenCloseType;             /**< 开平标志(买)*/
    TX1FtdcOpenCloseTypeType              SellOpenCloseType;            /**< 开平标志(卖)*/
    TX1FtdcSpeculatorType                 Speculator;                   /**< 投资类别*/
    TX1FtdcAmountType                     BuyOrderAmount;               /**< 委托数量(买)*/
    TX1FtdcAmountType                     SellOrderAmount;              /**< 委托数量(卖)*/
    TX1FtdcPriceType                      BuyInsertPrice;               /**< 委托价  (买)*/
    TX1FtdcPriceType                      SellInsertPrice;              /**< 委托价  (卖)*/
    TX1FtdcX1OrderIDType
    X1OrderID;                    /**< 柜台委托号 返回的柜台委托号为第一边的柜台委托号，第二边的柜台委托号实际上是第一遍柜台委托号加1，不需要用户关心。后续撤单操作指定这个柜台委托号来进行双边撤单，不支持只撤销其中一边的操作*/
    TX1FtdcAccountIDType
    AccountID;                    /**< 资金账号(此版本不适用)*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
    TX1FtdcDateType
    SuspendTime;                  /**< 挂单时间(此版本不适用)*/
    TX1FtdcEntrusTellerType               EntrusTeller;                 /**< 委托柜员 (此版本不适用)*/
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  /**< 委托状态*/
    TX1FtdcSessionIDType                  SessionID;                    /**< 会话ID*/
    TX1FtdcQuoteIDType                    QuoteID;                      /**< 询价编号*/
    TX1FtdcErrorIDType                    ErrorID;                      /**< 错误ID   2月底之前完成*/
    TX1FtdcErrorMsgInfoType               StatusMsg;                    /**< 错误信息*/
    TX1FtdcCustomCategoryType             CustomCategory;               /**< 自定义类别*/
    TX1FtdcLocalOrderIDType               PriFlowNo;                    /**<  私有流编号*/
};

///做市商撤单回报
struct APISTRUCT CX1FtdcQuoteCanceledRtnField
{
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所*/
    TX1FtdcClientIDType                   ClientID;                     /**< 交易编码(此版本不适用)*/
    TX1FtdcOrderSysIDType                 OrderSysID;                   /**< 报单编号*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcLocalOrderIDType               LocalOrderID;                 /**< 本地委托号*/
    TX1FtdcSeatCodeType                   SeatCode;                     /**< 席位代码*/
    TX1FtdcOpenCloseTypeType              BuyOpenCloseType;             /**< 开平标志(买)*/
    TX1FtdcOpenCloseTypeType              SellOpenCloseType;            /**< 开平标志(卖)*/
    TX1FtdcSpeculatorType                 Speculator;                   /**< 投资类别*/
    TX1FtdcX1OrderIDType                  X1OrderID;                    /**< 柜台委托号*/
    TX1FtdcAccountIDType
    AccountID;                    /**< 资金账号(此版本不适用)*/
    TX1FtdcEntrusTellerType               EntrusTeller;                 /**< 委托柜员(此版本不适用)*/
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  /**< 委托状态*/
    TX1FtdcAmountType                     CancelAmount;                 /**< 撤单数量*/
    TX1FtdcPriceType
    Fee;                          /**< 解冻手续费(此版本不适用)*/
    TX1FtdcPriceType                      Margin;                       /**< 解冻保证金(此版本不适用)*/
    TX1FtdcSessionIDType                  SessionID;                    /**< 会话ID*/
    TX1FtdcBuySellTypeType                BuySellType;                  /**< 买卖标志*/
    TX1FtdcQuoteIDType                    QuoteID;                      /**< 询价编号*/
    TX1FtdcDateType                       CanceledTime;                 /**< 撤单时间*/
    TX1FtdcCustomCategoryType             CustomCategory;               /**< 自定义类别*/
    TX1FtdcErrorIDType                    ErrorID;                      /**< 错误ID  2月底之前完成*/
    TX1FtdcErrorMsgInfoType               StatusMsg;                    /**< 状态信息*/
    TX1FtdcLocalOrderIDType               PriFlowNo;                    /**< 私有流编号*/
};

///做市商成交回报
struct APISTRUCT CX1FtdcQuoteMatchRtnField
{
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所ID*/
    TX1FtdcClientIDType                   ClientID;                     /**< 交易编码(此版本不适用)*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcSeatCodeType                   SeatCode;                     /**< 席位代码*/
    TX1FtdcLocalOrderIDType               LocalOrderID;                 /**< 本地委托号*/
    TX1FtdcOpenCloseTypeType              OpenCloseType;                /**< 开平标志*/
    TX1FtdcSpeculatorType                 Speculator;                   /**< 投资类别*/
    TX1FtdcX1OrderIDType                  X1OrderID;                    /**< 柜台委托号*/
    TX1FtdcOrderSysIDType                 OrderSysID;                   /**< 报单编号*/
    TX1FtdcMatchIDType                    MatchID;                      /**< 成交编号*/
    TX1FtdcAmountType                     MatchedAmount;                /**< 成交数量*/
    TX1FtdcPriceType                      MatchedPrice;                 /**< 成交价格*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账号(此版本不适用)*/
    TX1FtdcPriceType                      Turnover;                     /**< 成交金额(此版本不适用)*/
    TX1FtdcEntrusTellerType               EntrusTeller;                 /**< 委托柜员(此版本不适用)*/
    TX1FtdcDateType                       MatchedTime;                  /**< 成交时间*/
    TX1FtdcFeeType                        Fee;                          /**< 手续费(此版本不适用)*/
    TX1FtdcPriceType                      InsertPrice;                  /**< 委托价格(此版本不适用)*/
    TX1FtdcAmountType                     OrderAmount;                  /**< 委托数量*/
    TX1FtdcOrderAnswerStatusType
    OrderStatus;                  /**< 申报结果,成交成功12，全部成交与否用户自行计算*/
    TX1FtdcPriceType
    Margin;                       /**< 开仓为保证金,平仓为解冻保证金(此版本不用)*/
    TX1FtdcBuySellTypeType                BuySellType;                  /**< 买卖*/
    TX1FtdcAmountType                     CloseTodayAmount;             /**< 平今数量(此版本不适用)*/
    TX1FtdcPriceType                      ClosePrice;                   /**< 平仓金额(此版本不适用)*/
    TX1FtdcPriceType                      CloseTodayPrice;              /**< 平今金额(此版本不适用)*/
    TX1FtdcAdjustmentInfoType
    AdjustmentInfo;               /**< 组合或对锁的保证金调整信息,格式:[合约代码,买卖标志,投资类别,调整金额;](此版本不用)*/
    TX1FtdcPriceType
    FrozenCapita;                 /**< 成交解冻委托冻结的资金(此版本不适用)*/
    TX1FtdcProfitLossType                 DateCloseProfitLoss;          /**< 盯市平仓盈亏(此版本不适用)*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
    TX1FtdcSessionIDType                  SessionID;                    /**< 会话标识*/
    TX1FtdcLargeMarginDirectType          LargeMarginDirect;            /**< 大边保证金方向(此版本不适用)*/
    TX1FtdcQuoteIDType                    QuoteID;                      /**< 询价编号*/
    TX1FtdcCustomCategoryType             CustomCategory;               /**< 自定义类别*/
    TX1FtdcLocalOrderIDType               PriFlowNo;                    /**< 私有流编号*/
};

///批量撤单响应
struct APISTRUCT CX1FtdcCancelAllOrderRspField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账户ID(此版本不适用)*/
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  /**< 委托状态*/
};

///询价请求响应
struct APISTRUCT CX1FtdcForQuoteRspField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcX1OrderIDType                  X1OrderID;                    /**< 柜台委托号*/
    TX1FtdcDateType                       CommTime;                     /**< 委托时间(此版本不适用)*/
};

///询价回报
struct APISTRUCT CX1FtdcForQuoteRtnField
{
    TX1FtdcX1OrderIDType                  X1OrderID;                    /**< 柜台委托号*/
    TX1FtdcSessionIDType                  SessionID;                    /**< 会话ID*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账号(此版本不适用)*/
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  /**< 委托状态*/
    TX1FtdcErrorIDType                    ErrorID;                      /**< 错误ID*/
    TX1FtdcLocalOrderIDType               PriFlowNo;                    /**< 私有流编号*/
};

///查询询价请求
struct APISTRUCT CX1FtdcQryForQuoteField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账户ID*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所*/
};

///做市商报价委托查询
struct APISTRUCT CX1FtdcQuoteOrderField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账户ID*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcLocalOrderIDType               LocalOrderID;                 /**< 本地委托号*/
    TX1FtdcX1OrderIDType                  X1OrderID;                    /**< 柜台委托号*/
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  /**< 委托状态*/
};

///询价通知查询请求
struct APISTRUCT CX1FtdcQryQuoteNoticeField
{
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账号*/
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
};

///查询询价响应
struct APISTRUCT CX1FtdcQryForQuoteRtnField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账号*/
    TX1FtdcX1OrderIDType                  X1OrderID;                    /**< 柜台委托号*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所*/
    TX1FtdcDateType                       SuspendTime;                  /**< 挂起时间*/
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  /**< 委托状态*/
};

///做市商报价查询响应
struct APISTRUCT CX1FtdcQuoteOrderRtnField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcX1OrderIDType                  X1OrderID;                    /**< 柜台委托号*/
    TX1FtdcOrderAnswerStatusType          OrderStatus;                  /**< 委托状态*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcPriceType                      Margin;                       /**< 保证金*/
    TX1FtdcPriceType                      Fee;                          /**< 手续费*/
    TX1FtdcLocalOrderIDType               LocalOrderID;                 /**< 本地委托号*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 客户号*/
    TX1FtdcDateType                       CommTime;                     /**< 委托时间*/
    TX1FtdcDateType                       SubmitTime;                   /**< 申报时间*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所ID*/
    TX1FtdcAmountType                     BuyOrderAmount;               /**< 委托数量(买)*/
    TX1FtdcAmountType                     BuyMatchedAmount;             /**< 成交数量(买)*/
    TX1FtdcAmountType                     BuyCancelAmount;              /**< 撤单数量(买)*/
    TX1FtdcPriceType                      BuyInsertPrice;               /**< 委托价格(买)*/
    TX1FtdcPriceType                      BuyMatchedPrice;              /**< 成交价格(买)*/
    TX1FtdcOpenCloseTypeType              BuyOpenCloseType;             /**< 开平标志(买)*/
    TX1FtdcAmountType                     SellOrderAmount;              /**< 委托数量(卖)*/
    TX1FtdcAmountType                     SellMatchedAmount;            /**< 成交数量(卖)*/
    TX1FtdcAmountType                     SellCancelAmount;             /**< 撤单数量(卖)*/
    TX1FtdcPriceType                      SellInsertPrice;              /**< 委托价格(卖)*/
    TX1FtdcPriceType                      SellMatchedPrice;             /**< 成交价格(卖)*/
    TX1FtdcOpenCloseTypeType              SellOpenCloseType;            /**< 开平标志(卖)*/
    TX1FtdcFrontAddrType                  OperStation;                  /**< 操作站点*/
    TX1FtdcSessionIDType                  SessionID;                    /**< 会话ID*/
    TX1FtdcQuoteIDType                    QuoteID;                      /**< 询价编号*/
    TX1FtdcCustomCategoryType             CustomCategory;               /**< 自定义类别*/
};

///询价通知查询响应
struct APISTRUCT CX1FtdcQryQuoteNoticeRtnField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcQuoteIDType                    QuoteID;                      /**< 询价编号*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcSourceType                     Source;                       /**< 来源*/
    TX1FtdcDateType                       QuoteTime;                    /**< 询价时间*/
};

///套利合约查询
/**
* @details 当exchangeID为空时，表示查询各交易所的所有合约代码，也可指定查询某交易所的所有合约代码信息。
*/
struct  APISTRUCT CX1FtdcAbiInstrumentField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账户ID*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所代码*/
};

///套利合约返回信息
struct APISTRUCT CX1FtdcAbiInstrumentRtnField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所编码*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcInstrumentNameType             InstrumentName;               /**< 合约名称*/
    TX1FtdcPriceType                      UpperLimitPrice;              /**< 涨停板价*/
    TX1FtdcPriceType                      LowerLimitPrice;              /**< 跌停板价*/
    TX1FtdcPriceType                      PriceTick;                    /**< 最小变动价位*/
};

///询价通知订阅回报
struct APISTRUCT CX1FtdcQuoteSubscribeRtnField
{
    TX1FtdcQuoteIDType                    QuoteID;                      /**< 询价编号*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcSourceType                     Source;                       /**< 来源*/
    TX1FtdcDateType                       QuoteTime;                    /**< 询价时间*/
};

///修改客户密码
struct APISTRUCT CX1FtdcReqResetPasswordField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID [NM]*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账户ID [M]*/
    TX1FtdcPasswordType                   OldPassword;                  /**< 密码 [M]*/
    TX1FtdcPasswordType                   NewPassword;                  /**< 密码 [M]*/
};

///修改客户密码响应
struct APISTRUCT CX1FtdcRspResetPasswordField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID[BLK]*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金帐号ID [M]*/
    TX1FtdcResetPasswordType              ResetPasswordResult;          /**< 修改密码结果[M]*/
};

///查询套利持仓明细
struct APISTRUCT CX1FtdcArbitrageCombineDetailField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金账户ID*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
};

///查询套利持仓响应
struct APISTRUCT CX1FtdcArbitrageCombineDetailRtnField
{
    TX1FtdcRequestIDType                  RequestID;                    /**< 请求ID*/
    TX1FtdcAccountIDType                  AccountID;                    /**< 资金帐号ID*/
    TX1FtdcExchangeIDType                 ExchangeID;                   /**< 交易所代码*/
    TX1FtdcInstrumentIDType               InstrumentID;                 /**< 合约代码*/
    TX1FtdcBuySellTypeType                BuySellType;                  /**< 买卖*/
    TX1FtdcPriceType                      OpenPrice;                    /**< 开仓价*/
    TX1FtdcInstrumentIDType               ComTradeID;                   /**< 组合编号*/
    TX1FtdcDateType                       OpenDate;                     /**< 开仓日期*/
    TX1FtdcMatchIDType                    MatchID;                      /**< 成交编号/结算编号*/
    TX1FtdcAmountType                     Volume;                       /**< 持仓数量*/
    TX1FtdcAmountType                     TradingAmount;                /**< 挂单数量*/
    TX1FtdcProfitLossType                 Margin;                       /**< 保证金*/
    TX1FtdcSpeculatorType                 Speculator;                   /**< 投保类别*/
    TX1FtdcInstrumentTypeType             InstrumentType;               /**< 合约类型*/
    TX1FtdcX1OrderIDType                  SpdOrderID;                   /**< 柜台委托号*/
    TX1FtdcCustomCategoryType             CustomCategory;               /**< 自定义类别*/
    TX1FtdcClientIDType                   ClientID;                     /**< 交易编码*/
    TX1FtdcAmountType                     YesterdayVolume;              /**< 昨仓数量*/
};

///行情触发请求
struct APISTRUCT CX1FtdcPricesTriggerField
{
    TX1FtdcAccountIDType              AccountID;          /**< 资金账户ID*/
    TX1FtdcLocalOrderIDType           LocalOrderID;       /**< 本地委托号*/
    TX1FtdcInstrumentIDType           InstrumentID;       /**< 合约代码*/
    TX1FtdcPriceType                  InsertPrice;        /**< 委托价格*/
    TX1FtdcAmountType                 OrderAmount;        /**< 委托数量*/
    TX1FtdcSpeculatorType             Speculator;         /**< 投保类型*/
    TX1FtdcOrderTypeType              OrderType;          /**< 条件单报单类型*/
    TX1FtdcBuySellTypeType            BuySellType;        /**< 买卖标志*/
    TX1FtdcOpenCloseTypeType          OpenCloseType;      /**< 开平标志*/
    TX1FtdcRequestIDType              RequestID;          /**< 请求ID*/
    TX1FtdcCompareFlagType            CompareFlag;        /**< 比较标志(价格)*/
    TX1FtdcPriceType
    ComparePrice;       /**< 触发价格(触发条件为行情价格大于或小于等于触发价格)*/
    TX1FtdcPriceReferenceType         PriceReference;     /**< 价格参照*/
    TX1FtdcBreakDownTimesType         BreakDownTimes;     /**< 击穿次数*/
    TX1FtdcDateType                   Validate;           /**< 有效日期(yyyy.mm.dd,暂不支持)*/
    TX1FtdcFrozenTypeType             Frozentype;         /**< 是否冻结资金类型*/
    TX1FtdcInstrumentTypeType         InstrumentType;     /**< 合约类型*/
    TX1FtdcAmountType                 LimitAmount;        /**< 数量限制*/
    TX1FtdcCompareFlagType            QtyCmpFlag;         /**< 比较标志(数量)*/
    TX1FtdcTriggerType                TriggerType;        /**< 触发类型*/
    TX1FtdcBreakDownTypeType          BreakDownType;      /**< 击穿属性*/
};

///条件单查询请求
struct APISTRUCT CX1FtdcQryExtOrderField
{
    TX1FtdcAccountIDType              AccountID;          /**< 资金账户ID*/
    TX1FtdcRequestIDType              RequestID;          /**< 请求ID*/
    TX1FtdcX1OrderIDType              ExtSpdOrderID;      /**< 条件单编号(暂不支持)*/
    TX1FtdcExtOrderType               ExtOrderType;       /**< 条件单类型*/
};

///条件单撤单请求
struct APISTRUCT CX1FtdcCancelExtOrderField
{
    TX1FtdcAccountIDType              AccountID;          /**< 资金账户ID*/
    TX1FtdcRequestIDType              RequestID;          /**< 请求ID*/
    TX1FtdcLocalOrderIDType           LocalOrderID;       /**< 本地委托号*/
    TX1FtdcX1OrderIDType              ExtSpdOrderID;      /**< 条件单编号*/
    TX1FtdcExtOrderType               ExtOrderType;       /**< 条件单类型*/
};

///条件单委托响应
struct APISTRUCT CX1FtdcExtOrderRspDataField
{
    TX1FtdcLocalOrderIDType           LocalOrderID;       /**< 本地委托号*/
    TX1FtdcX1OrderIDType              SpdOrderID;         /**< 柜台委托号*/
    TX1FtdcX1OrderIDType              ExtSpdOrderID;      /**< 条件单编号*/
    TX1FtdcOrderAnswerStatusType      OrderStatus;        /**< 委托状态*/
    TX1FtdcRequestIDType              RequestID;          /**< 请求ID*/
    TX1FtdcPriceType                  FrozenMargin;       /**< 冻结资金(仅下单使用)*/
    TX1FtdcExtOrderType               ExtOrderType;       /**< 条件单类型*/
    TX1FtdcAccountIDType              AccountID;          /**< 资金账户ID*/
};

///条件单委托回报
struct APISTRUCT CX1FtdcPricesTriggerRtnField
{
    TX1FtdcAccountIDType              AccountID;          /**< 资金账户ID*/
    TX1FtdcLocalOrderIDType           LocalOrderID;       /**< 本地委托号*/
    TX1FtdcInstrumentIDType           InstrumentID;       /**< 合约代码*/
    TX1FtdcPriceType                  InsertPrice;        /**< 委托价格*/
    TX1FtdcAmountType                 OrderAmount;        /**< 委托数量*/
    TX1FtdcSpeculatorType             Speculator;         /**< 投保类型*/
    TX1FtdcOrderTypeType              OrderType;          /**< 条件单报单类型*/
    TX1FtdcBuySellTypeType            BuySellType;        /**< 买卖标志*/
    TX1FtdcOpenCloseTypeType          OpenCloseType;      /**< 开平标志*/
    TX1FtdcCompareFlagType            CompareFlag;        /**< 比较标志*/
    TX1FtdcPriceType                  ComparePrice;       /**< 触发价格*/
    TX1FtdcPriceReferenceType         PriceReference;     /**< 价格参照*/
    TX1FtdcBreakDownTimesType         BreakDownTimes;     /**< 击穿次数*/
    TX1FtdcDateType                   Modifiedtime;       /**< 修改时间*/
    TX1FtdcDateType                   CommTime;           /**< 委托时间*/
    TX1FtdcX1OrderIDType              SpdOrderID;         /**< 柜台委托号*/
    TX1FtdcDateType                   CanceledTime;       /**< 撤单时间*/
    TX1FtdcFrozenTypeType             Frozentype;         /**< 冻结类型*/
    TX1FtdcX1OrderIDType              ExtSpdOrderID;      /**< 条件单编号*/
    TX1FtdcProfitLossType             FrozenMargin;       /**< 冻结资金*/
    TX1FtdcAmountType                 FrozenAmount;       /**< 冻结数量*/
    TX1FtdcOrderAnswerStatusType      OrderStatus;        /**< 委托状态*/
    TX1FtdcSessionIDType              SessionID;          /**< 会话ID*/
    TX1FtdcErrorMsgInfoType           StatusMsg;          /**< 状态信息*/
    TX1FtdcAmountType                 LimitAmount;        /**< 数量限制*/
    TX1FtdcCompareFlagType            QtyCmpFlag;         /**< 比较标志(数量)*/
    TX1FtdcTriggerType                TriggerType;        /**< 触发类型*/
    TX1FtdcBreakDownTypeType          BreakDownType;      /**< 击穿属性*/
    TX1FtdcExtOrderTriggerStatusType  OrderTriggerStatus; /**< 条件单触发状态*/
};

///行情触发查询响应
struct APISTRUCT CX1FtdcQryPricesTriggerField
{
    TX1FtdcAccountIDType              AccountID;          /**< 资金账户ID*/
    TX1FtdcLocalOrderIDType           LocalOrderID;       /**< 本地委托号*/
    TX1FtdcInstrumentIDType           InstrumentID;       /**< 合约代码*/
    TX1FtdcPriceType                  InsertPrice;        /**< 委托价格*/
    TX1FtdcAmountType                 OrderAmount;        /**< 委托数量*/
    TX1FtdcSpeculatorType             Speculator;         /**< 投保类型*/
    TX1FtdcOrderTypeType              OrderType;          /**< 报单类型*/
    TX1FtdcBuySellTypeType            BuySellType;        /**< 买卖标志*/
    TX1FtdcOpenCloseTypeType          OpenCloseType;      /**< 开平标志*/
    TX1FtdcRequestIDType              RequestID;          /**< 请求ID*/
    TX1FtdcCompareFlagType            CompareFlag;        /**< 比较标志*/
    TX1FtdcPriceType                  ComparePrice;       /**< 触发价格*/
    TX1FtdcPriceReferenceType         PriceReference;     /**< 价格参照*/
    TX1FtdcBreakDownTimesType         BreakDownTimes;     /**< 击穿次数*/
    TX1FtdcDateType                   Validate;           /**< 有效日期*/
    TX1FtdcDateType                   Modifiedtime;       /**< 修改时间*/
    TX1FtdcDateType                   CommTime;           /**< 委托时间*/
    TX1FtdcDateType                   Commdate;           /**< 委托日期*/
    TX1FtdcX1OrderIDType              SpdOrderID;         /**< 柜台委托号*/
    TX1FtdcDateType                   CanceledTime;       /**< 撤单时间*/
    TX1FtdcFrozenTypeType             Frozentype;         /**< 冻结类型*/
    TX1FtdcX1OrderIDType              ExtSpdOrderID;      /**< 条件单编号*/
    TX1FtdcExtOrderType               ExtOrderType;       /**< 条件单类型*/
    TX1FtdcProfitLossType             FrozenMargin;       /**< 冻结资金*/
    TX1FtdcAmountType                 FrozenAmount;       /**< 冻结数量*/
    TX1FtdcOrderAnswerStatusType      OrderStatus;        /**< 委托状态*/
    TX1FtdcExtOrderTriggerStatusType  OrderTriggerStatus; /**< 条件单触发状态*/
    TX1FtdcAmountType                 LimitAmount;        /**< 数量限制*/
    TX1FtdcCompareFlagType            QtyCmpFlag;         /**< 比较标志(数量)*/
    TX1FtdcTriggerType                TriggerType;        /**< 触发类型*/
    TX1FtdcBreakDownTypeType          BreakDownType;      /**< 击穿属性*/
};

///查询交易日请求
struct APISTRUCT CX1FtdcTradingDayField
{
    TX1FtdcRequestIDType              RequestID;         /**< 请求ID*/
};

///交易日请求响应
struct APISTRUCT CX1FtdcTradingDayRtnField
{
    TX1FtdcRequestIDType              RequestID;           /**< 请求ID*/
    TX1FtdcDateType                   date;                /**< 交易日*/
};

///期权对冲请求
struct APISTRUCT CX1FtdcOptOffsetField
{
    TX1FtdcRequestIDType              RequestID;           /**< 请求ID*/
    TX1FtdcAccountIDType              AccountID;           /**< 资金账户ID*/
    TX1FtdcSpeculatorType             Speculator;          /**< 投保类型*/
    TX1FtdcExchangeIDType             ExchangeID;          /**< 交易所代码*/
    TX1FtdcInstrumentIDType           InstrumentID;        /**< 合约代码*/
    TX1FtdcOffsetFlagType             IfOffset;            /**< 是否对冲（撤销时不用）*/
};

///期权对冲查询请求
struct APISTRUCT CX1FtdcQryOptOffsetField
{
    TX1FtdcRequestIDType              RequestID;           /**< 请求ID*/
    TX1FtdcAccountIDType              AccountID;           /**< 资金账户ID*/
    TX1FtdcSpeculatorType             Speculator;          /**< 投保类型*/
    TX1FtdcExchangeIDType             ExchangeID;          /**< 交易所代码*/
    TX1FtdcInstrumentIDType           InstrumentID;        /**< 合约代码*/
};

///期权对冲响应
struct APISTRUCT CX1FtdcRspOptOffsetField
{
    TX1FtdcRequestIDType              RequestID;           /**< 请求ID*/
    TX1FtdcAccountIDType              AccountID;           /**< 资金账户ID*/
    TX1FtdcExchangeIDType             ExchangeID;          /**< 交易所代码*/
    TX1FtdcInstrumentIDType           InstrumentID;        /**< 合约代码*/
    TX1FtdcOffsetFlagType             IfOffset;            /**< 是否对冲（撤销时不用）*/
};

///期权对冲回报
struct APISTRUCT CX1FtdcRtnOptOffsetField
{
    TX1FtdcExchangeIDType             ExchangeID;          /**< 交易所代码*/
    TX1FtdcAccountIDType              AccountID;           /**< 资金账户ID*/
    TX1FtdcInstrumentIDType           InstrumentID;        /**< 合约代码*/
    TX1FtdcOffsetFlagType             IfOffset;            /**< 是否对冲*/
    TX1FtdcErrorIDType                ErrorID;             /**< 错误ID*/
    TX1FtdcErrorMsgInfoType           StatusMsg;           /**< 状态信息*/
};

///履约对冲请求
struct APISTRUCT CX1FtdcPerformOffsetField
{
    TX1FtdcRequestIDType              RequestID;           /**< 请求ID*/
    TX1FtdcAccountIDType              AccountID;           /**< 资金账户ID*/
    TX1FtdcSpeculatorType             Speculator;          /**< 投保类型*/
    TX1FtdcExchangeIDType             ExchangeID;          /**< 交易所代码*/
    TX1FtdcOffsetFlagType             IfOffset;            /**< 是否对冲(撤销时不用)*/
};

///履约对冲查询请求
struct APISTRUCT CX1FtdcQryPerformOffsetField
{
    TX1FtdcRequestIDType              RequestID;           /**< 请求ID*/
    TX1FtdcExchangeIDType             ExchangeID;          /**< 交易所代码*/
    TX1FtdcAccountIDType              AccountID;           /**< 资金账户ID*/
    TX1FtdcSpeculatorType             Speculator;          /**< 投保类型*/
};

///履约对冲响应
struct APISTRUCT CX1FtdcRspPerformOffsetField
{
    TX1FtdcRequestIDType              RequestID;           /**< 请求ID*/
    TX1FtdcExchangeIDType             ExchangeID;          /**< 交易所代码*/
    TX1FtdcAccountIDType              AccountID;           /**< 资金账户ID*/
    TX1FtdcOffsetFlagType             IfOffset;            /**< 是否对冲*/
};

///履约对冲回报
struct APISTRUCT CX1FtdcRtnPerformOffsetField
{
    TX1FtdcExchangeIDType             ExchangeID;          /**< 交易所代码*/
    TX1FtdcAccountIDType              AccountID;           /**< 资金账户ID*/
    TX1FtdcOffsetFlagType             IfOffset;            /**< 是否对冲*/
    TX1FtdcErrorIDType                ErrorID;             /**< 错误ID*/
    TX1FtdcErrorMsgInfoType           StatusMsg;           /**< 状态信息*/
};

///组合/解锁申请请求
struct APISTRUCT CX1FtdcCombPosiField
{
    TX1FtdcRequestIDType              RequestID;           /**< 请求ID*/
    TX1FtdcLocalOrderIDType           LocalOrderID;        /**< 本地委托号*/
    TX1FtdcExchangeIDType             ExchangeID;          /**< 交易所代码*/
    TX1FtdcAccountIDType              AccountID;           /**< 资金账户ID*/
    TX1FtdcSpeculatorType             Speculator;          /**< 投保类型*/
    TX1FtdcInstrumentIDType           ComInstrumentID;     /**< 组合合约代码*/
    TX1FtdcComSpeculatorType          ComSpeculator;       /**< 组合投保标志*/
    TX1FtdcComStatusType              ComStatus;           /**< 组合状态*/
    TX1FtdcAmountType                 ComAmount;           /**< 组合数量*/
};

///组合/解锁申请响应
struct APISTRUCT CX1FtdcRspCombPosiField
{
    TX1FtdcRequestIDType              RequestID;           /**< 请求ID*/
    TX1FtdcLocalOrderIDType           LocalOrderID;        /**< 本地委托号*/
    TX1FtdcX1OrderIDType              X1OrderID;           /**< 柜台委托号*/
    TX1FtdcSessionIDType              SessionID;           /**< 会话ID*/
    TX1FtdcOrderAnswerStatusType      OrderStatus;         /**< 委托状态*/
    TX1FtdcExchangeIDType             ExchangeID;          /**< 交易所代码*/
    TX1FtdcAccountIDType              AccountID;           /**< 资金账户ID*/
    TX1FtdcInstrumentIDType           ComInstrumentID;     /**< 组合合约代码*/
    TX1FtdcComSpeculatorType          ComSpeculator;       /**< 组合投保标志*/
    TX1FtdcComStatusType              ComStatus;           /**< 组合状态*/
    TX1FtdcAmountType                 ComAmount;           /**< 组合数量*/
};

///组合/解锁申请回报
struct APISTRUCT CX1FtdcRtnCombPosiField
{
    TX1FtdcLocalOrderIDType           LocalOrderID;        /**< 本地委托号*/
    TX1FtdcX1OrderIDType              X1OrderID;           /**< 柜台委托号*/
    TX1FtdcSessionIDType              SessionID;           /**< 会话ID*/
    TX1FtdcOrderAnswerStatusType      OrderStatus;         /**< 委托状态*/
    TX1FtdcExchangeIDType             ExchangeID;          /**< 交易所代码*/
    TX1FtdcAccountIDType              AccountID;           /**< 资金账户ID*/
    TX1FtdcInstrumentIDType           ComInstrumentID;     /**< 组合合约代码*/
    TX1FtdcComSpeculatorType          ComSpeculator;       /**< 组合投保标志*/
    TX1FtdcComStatusType              ComStatus;           /**< 组合状态*/
    TX1FtdcAmountType                 ComAmount;           /**< 组合数量*/
    TX1FtdcErrorIDType                ErrorID;             /**< 错误ID*/
    TX1FtdcErrorMsgInfoType           StatusMsg;           /**< 状态信息*/
};

///查询组合持仓
struct APISTRUCT CX1FtdcQryCombPositionField
{
    TX1FtdcRequestIDType              RequestID;           /**< 请求ID*/
    TX1FtdcExchangeIDType             ExchangeID;          /**< 交易所代码*/
    TX1FtdcAccountIDType              AccountID;           /**< 资金账户ID*/
    TX1FtdcInstrumentIDType           ComInstrumentID;     /**< 组合合约代码*/
    TX1FtdcComSpeculatorType          ComSpeculator;       /**< 组合投保标志*/
    TX1FtdcInstrumentIDType           InstrumentID;        /**< 合约代码*/
};

///查询组合持仓响应
struct APISTRUCT CX1FtdcRspCombPositionField
{
    TX1FtdcRequestIDType              RequestID;           /**< 请求ID*/
    TX1FtdcAccountIDType              AccountID;           /**< 资金账户ID*/
    TX1FtdcAmountType                 ComAmount;           /**< 组合数量*/
    TX1FtdcExchangeIDType             ExchangeID;          /**< 交易所代码*/
    TX1FtdcInstrumentIDType           ComInstrumentID;     /**< 组合合约代码*/
    TX1FtdcComSpeculatorType          ComSpeculator;       /**< 组合投保标志*/
    TX1FtdcPriceType                  Margin;              /**< 组合保证金*/
};

///查询组合合约
struct APISTRUCT CX1FtdcQryCombInstrumentField
{
    TX1FtdcRequestIDType              RequestID;           /**< 请求ID*/
    TX1FtdcAccountIDType              AccountID;           /**< 资金账户ID*/
    TX1FtdcExchangeIDType             ExchangeID;          /**< 交易所代码*/
};

///查询组合合约响应
struct APISTRUCT CX1FtdcRspCombInstrumentField
{
    TX1FtdcRequestIDType              RequestID;           /**< 请求ID*/
    TX1FtdcAccountIDType              AccountID;           /**< 资金账户ID*/
    TX1FtdcExchangeIDType             ExchangeID;          /**< 交易所代码*/
    TX1FtdcInstrumentIDType           ComInstrumentID;     /**< 组合合约代码*/
    TX1FtdcCombStrategyTypeType       StrategyType;        /**< 组合策略类型*/
    TX1FtdcPriorityType               Priority;            /**< 组合优先级*/
};

///放弃到期日自动行权请求
struct APISTRUCT CX1FtdcAutoExecAbandField
{
    TX1FtdcRequestIDType              RequestID;           /**< 请求ID*/
    TX1FtdcAccountIDType              AccountID;           /**< 资金账户ID*/
    TX1FtdcSpeculatorType             Speculator;          /**< 投保类型*/
    TX1FtdcExchangeIDType             ExchangeID;          /**< 交易所代码*/
    TX1FtdcInstrumentIDType           InstrumentID;        /**< 合约代码*/
};

///查询放弃到期日自动行权请求
struct APISTRUCT CX1FtdcQryAutoExecAbandField
{
    TX1FtdcRequestIDType              RequestID;           /**< 请求ID*/
    TX1FtdcAccountIDType              AccountID;           /**< 资金账户ID*/
    TX1FtdcSpeculatorType             Speculator;          /**< 投保类型*/
    TX1FtdcExchangeIDType             ExchangeID;          /**< 交易所代码*/
    TX1FtdcInstrumentIDType           InstrumentID;        /**< 合约代码*/
};

///放弃到期日自动行权设置响应
struct APISTRUCT CX1FtdcRspAutoExecAbandField
{
    TX1FtdcRequestIDType              RequestID;           /**< 请求ID*/
    TX1FtdcAccountIDType              AccountID;           /**< 资金账户ID*/
    TX1FtdcInstrumentIDType           InstrumentID;        /**< 合约代码*/
};

///放弃到期日自动行权设置回报
struct APISTRUCT CX1FtdcRtnAutoExecAbandField
{
    TX1FtdcAccountIDType              AccountID;           /**< 资金账户ID*/
    TX1FtdcInstrumentIDType           InstrumentID;        /**< 合约代码*/
    TX1FtdcErrorIDType                ErrorID;             /**< 错误ID*/
    TX1FtdcErrorMsgInfoType           StatusMsg;           /**< 状态信息*/
};

struct APISTRUCT CX1FtdcExchangeConnectionStatusRtnField
{
};





///ADD By Moses.20181109

///深度行情
struct APISTRUCT CX1FtdcDepthMarketDataField
{
    TX1FtdcDateType                       tradingDay;                   /**<交易日 yyyy.mm.dd*/
    TX1FtdcInstrumentIDType               instrumentID;                 /**<合约代码*/
    TX1FtdcExchangeIDType                 exchangeID;                   /**<交易所代码*/
    TX1FtdcInstrumentIDType               exchangeInstID;               /**<合约在交易所的代码*/
    TX1FtdcPriceType                      lastPrice;                    /**<最新价*/
    TX1FtdcPriceType                      preSettlementPrice;           /**<上次结算价*/
    TX1FtdcPriceType                      preClosePrice;                /**<昨收盘*/
    TX1FtdcAmountType                     preOpenInterest;              /**<昨持仓量*/
    TX1FtdcPriceType                      openPrice;                    /**<今开盘*/
    TX1FtdcPriceType                      highestPrice;                 /**<最高价*/
    TX1FtdcPriceType                      lowestPrice;                  /**<最低价*/
    TX1FtdcAmountType                     Volume;                       /**<成交数量*/
    TX1FtdcPriceType                      turnover;                     /**<成交金额*/
    TX1FtdcAmountType                     openInterest;                 /**<持仓量*/
    TX1FtdcPriceType                      closePrice;                   /**<今收盘*/
    TX1FtdcPriceType                      settlementPrice;              /**<本次结算价*/
    TX1FtdcPriceType                      upperLimitPrice;              /**<涨停板价*/
    TX1FtdcPriceType                      lowerLimitPrice;              /**<跌停板价*/
    TX1FtdcDeltaType                      preDelta;                     /**<昨虚实度*/
    TX1FtdcDeltaType                      currDelta;                    /**<今虚实度*/
    TX1FtdcDateType                       UpdateTime;                   /**<最后修改时间*/
    TX1FtdcMilliSecType                   UpdateMillisec;               /**<最后修改毫秒*/
    TX1FtdcPriceType                      BidPrice1;                    /**<申买价一*/
    TX1FtdcAmountType                     BidVolume1;                   /**<申买量一*/
    TX1FtdcPriceType                      AskPrice1;                    /**<申卖价一*/
    TX1FtdcAmountType                     AskVolume1;                   /**<申卖量一*/
    TX1FtdcPriceType                      BidPrice2;                    /**<申买价二*/
    TX1FtdcAmountType                     BidVolume2;                   /**<申买量二*/
    TX1FtdcPriceType                      AskPrice2;                    /**<申卖价二*/
    TX1FtdcAmountType                     AskVolume2;                   /**<申卖量二*/
    TX1FtdcPriceType                      BidPrice3;                    /**<申买价三*/
    TX1FtdcAmountType                     BidVolume3;                   /**<申买量三*/
    TX1FtdcPriceType                      AskPrice3;                    /**<申卖价三*/
    TX1FtdcAmountType                     AskVolume3;                   /**<申卖量三*/
    TX1FtdcPriceType                      BidPrice4;                    /**<申买价四*/
    TX1FtdcAmountType                     BidVolume4;                   /**<申买量四*/
    TX1FtdcPriceType                      AskPrice4;                    /**<申卖价四*/
    TX1FtdcAmountType                     AskVolume4;                   /**<申卖量四*/
    TX1FtdcPriceType                      BidPrice5;                    /**<申买价五*/
    TX1FtdcAmountType                     BidVolume5;                   /**<申买量五*/
    TX1FtdcPriceType                      AskPrice5;                    /**<申卖价五*/
    TX1FtdcAmountType                     AskVolume5;                   /**<申卖量五*/
    TX1FtdcPriceType                      AveragePrice;                 /**<当日均价*/
    TX1FtdcDateType                       XSpeedTime;                   /**<柜台系统时间*/
};


//自定义组合行情
struct APISTRUCT CX1FtdcCustomMarketDataField
{
    TX1FtdcInstrumentIDType               instrumentID;                 /**<合约代码*/
    TX1FtdcExchangeIDType                 exchangeID;                   /**<交易所*/
    TX1FtdcAmountType                     bidVolume1;                   /**<买一量*/
    TX1FtdcPriceType                      bidPrice1;                    /**<买一价(挂价价差)*/
    TX1FtdcAmountType                     askVolume1;                   /**<卖一量*/
    TX1FtdcPriceType                      askPrice1;                    /**<卖一价(对价价差)*/
    TX1FtdcPriceType                      lastPrice;                    /**<最新价价差*/
};

#endif // TX1FtdcAPISTRUCT_H_
