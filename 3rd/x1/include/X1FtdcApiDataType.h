/**
 * Copyright (C) 2015-2016, 大连飞创信息技术有限公司
 * @file <X1FtdcApiDataType.h>
 * @brief <定义接口所需的数据类型>
 * @version <1.3.2.3>
 * @author <X1项目组>
 * @date <2015年12月2日>
 */

#ifndef X1FTDCAPIDATATYPE_H
#define X1FTDCAPIDATATYPE_H

//#define C_ExchangeID_DCE                  1     // 大商所
//#define C_ExchangeID_SHFE                 2     // 上期所
//#define COMPLIE_EXCHANGE_ID_TYPE C_ExchangeID_SHFE


typedef char TX1FtdcUserIDType[41];               /**< 用户ID数据类型*/
typedef char TX1FtdcAccountIDType[13];            /**< 资金账户数据类型*/
typedef int TX1FtdcExecStateType;                 /**< 执行状态数据类型*/

#define X1_FTDC_ERR_CODE_SUCCESS            0     ///< 成功
#define X1_FTDC_ERR_CODE_FAIL              -1     ///< 失败
#define X1_FTDC_ERR_CODE_INVALID           -2

typedef char TX1FtdcClientIDType[13];             /**< 交易编码数据类型 */
typedef int TX1FtdcClientStatusType;              /**< 交易编码状态数据类型*/

#define X1_FTDC_PROHIBIT_OPEN               4     ///< 禁止开新仓
#define X1_FTDC_ALLOW_OPEN                  5     ///< 允许开新仓

typedef char TX1FtdcInstrumentIDType[64];         /**< 合约代码数据类型 */
typedef char TX1FtdcInstrumentPrefixType[31];     /**< 品种名称数据类型 */
typedef char TX1FtdcVarietyNameType[31];          /**< 品种名称数据类型 */
typedef char TX1FtdcInstrumentNameType[64];       /**< 合约名称数据类型 */
typedef char TX1FtdcActiveContractType[1024];     /**< 有效合约数据类型 */
typedef long TX1FtdcLocalOrderIDType;             /**< 本地委托号数据类型 */


typedef double TX1FtdcPriceType;                  /**< 价格数据类型 */
typedef long TX1FtdcAmountType;                   /**< 委托数量数据类型 */
typedef short TX1FtdcBuySellTypeType;             /**< 买卖数据类型 */

#define X1_FTDC_SPD_BUY                     1     ///< 买
#define X1_FTDC_SPD_SELL                    2     ///< 卖

typedef int TX1FtdcOpenCloseTypeType;             /**< 开平标志数据类型 */

#define X1_FTDC_SPD_OPEN                    1     ///< 开仓
#define X1_FTDC_SPD_CLOSE                   2     ///< 平仓
#define X1_FTDC_SPD_CLOSETODAY              4     ///< 平今





typedef short TX1FtdcSpeculationValueType;        /**< 投机保值数据类型 */
typedef char TX1FtdcExchangeIDType[8];            /**< 交易所编码数据类型 */

#define X1_FTDC_EXCHANGE_DCE              "DCE"   ///< 大商所
#define X1_FTDC_EXCHANGE_CZCE             "CZCE"  ///< 郑商所
#define X1_FTDC_EXCHANGE_SHFE             "SHFE"  ///< 上期所
#define X1_FTDC_EXCHANGE_CFFEX            "CFFEX" ///< 中金所
#define X1_FTDC_EXCHANGE_INE              "INE"   ///< 上能所


typedef char TX1FtdcFrontAddrType[32];            /**< 前置机地址数据类型 */
typedef short TX1FtdcCompanyIDType;               /**< 开发商代码数据类型 */


typedef char TX1FtdcPasswdType[41];               /**< 用户密码数据类型 柜台端密码不能为空且有效长度最大为16位 */
typedef char TX1FtdcPasswordType[41];             /**< 用户密码数据类型 柜台端密码不能为空且有效长度最大为16位 */
#define X1FTDC_PASSWORD_LEN 41


typedef long TX1FtdcX1OrderIDType;                /**< 柜台委托号数据类型 */
typedef char TX1FtdcOrderSysIDType[64];           /**< 报单编号数据类型 */
typedef int TX1FtdcOrderTypeType;                 /**< 报单类型 */

// 区分普通、套利、互换
#define X1_FTDC_LIMITORDER                  1     ///< 限价委托
#define X1_FTDC_MKORDER                     2     ///< 市价委托
#define X1_FTDC_ARBITRAGE                   4     ///< 套利委托
#define X1_FTDC_EXTENSION                   8     ///< 展期互换委托
#define X1_FTDC_PROFIT_LIMITORDER           32    ///< 限价止盈委托
#define X1_FTDC_PROFIT_MKORDER              34    ///< 市价止盈委托
#define X1_FTDC_LOSS_LIMITORDER             48    ///< 限价止损委托
#define X1_FTDC_LOSS_MKORDER                50    ///< 市价止损委托


typedef short TX1FtdcOrderAnswerStatusType;       /**< 委托回报类型 */

#define X1_FTDC_SPD_CANCELED                1     ///< 全部撤单
#define X1_FTDC_SPD_FILLED                  2     ///< 全部成交
#define X1_FTDC_SPD_IN_QUEUE                3     ///< 未成交还在队列中
#define X1_FTDC_SPD_PARTIAL                 4     ///< 部分成交还在队列中
#define X1_FTDC_SPD_PARTIAL_CANCELED        5     ///< 部成部撤
#define X1_FTDC_SPD_IN_CANCELING            6     ///< 撤单中
#define X1_FTDC_SPD_ERROR                   7     ///< 错误(废单错误)
#define X1_FTDC_SPD_PLACED                  8     ///< 未成交不在队列中
#define X1_FTDC_SPD_TRIGGERED               10    ///< 柜台已接收，但尚未到交易所
#define X1_FTDC_SPD_IN_CANCELED             11    ///< 柜台已接收，柜台内部直接撤单

/// 基于算法单模块新增 
#define X1_FTDC_EXT_UNTRIGGER               13    ///< 未触发
#define X1_FTDC_EXT_PART_TRIGGER            14    ///< 部分触发
#define X1_FTDC_EXT_ALL_TRIGGER             15    ///< 全部触发
#define X1_FTDC_EXT_CANCELLED               16    ///< 已经撤单
#define	X1_FTDC_EXT_FAILED                  17    ///< 报单失败



typedef char TX1FtdcMatchIDType[32];              /**< 成交编号数据类型 */
typedef char TX1FtdcDateType[13];                 /**< 时间数据类型 */
typedef long TX1FtdcMatchType;                    /**< 成交类型数据类型 */

#define X1_FTDC_BASIC_TRADE                 0     ///< 普通成交

typedef int TX1FtdcSpeculatorType;                /**< 投保类型 */

#define X1_FTDC_SPD_SPECULATOR              0     ///投机
#define X1_FTDC_SPD_HEDGE                   1     ///套保
#define X1_FTDC_SPD_ARBITRAGE               2     ///套利


typedef double TX1FtdcFeeType;                    /**< 手续费数据类型 */
typedef int TX1FtdcErrorIDType;                   /**< 错误数据类型 */
typedef char TX1FtdcErrorMsgInfoType[256];         /**< 错误信息数据类型 */
typedef char TX1FtdcMsgInfoType[1024];            /**< 消息信息数据类型 */
typedef double TX1FtdcEquityType;                 /**< 权益数据类型 */
typedef double TX1FtdcProfitLossType;             /**< 盈亏数据类型 */
typedef int TX1FtdcAccountLoginResultType;        /**< 资金账户登录结果 */

#define X1_FTDC_LOGIN_SUCCESS               0     ///< 登录成功
#define X1_FTDC_LOGIN_FAILED                1     ///< 登录失败
#define X1_FTDC_LOGIN_QUIT                  2     ///< 已退出
#define X1_FTDC_LOGIN_NOT_OPERATE           9     ///< 未操作

typedef long TX1FtdcSessionIDType;                /**< SessionID数据类型 */
typedef int TX1FtdcAccountLogoutResultType;       /**< 资金帐号登出结果 */

#define X1_FTDC_LOGOUT_SUCCESS              0     ///< 登出成功
#define X1_FTDC_LOGOUT_FAILED               1     ///< 登出失败


typedef int TX1FtdcUserTypeType;                  /**< 用户类型 */
typedef int TX1FtdcCounterIDType;                 /**< 柜台编号数据类型 */
typedef double TX1FtdcRiskDegreeType;             /**< 风险度数据类型 */
typedef int TX1FtdcMilliSecType;                  /**< 微秒数据类型 */
typedef double TX1FtdcDeltaType;                  /**< 虚实度数据类型 */
typedef int TX1FtdcVolumeType;                    /**< 数量数据类型 */
typedef int TX1FtdcFrontIDType;                   /**< 前置机编号数据类型 */
typedef int TX1FtdcOfferPriceLimitType;           /**< 报价数据上限数据类型 */
typedef short TX1FtdcOrderNumType;                /**< 委托号数据类型 */
typedef double TX1FtdcRatioType;                  /**< 比率数据类型 */
typedef double TX1FtdcPremiumType;                /**< 权利金 */
typedef double TX1FtdcMarketValueType;            /**< 期权市值 */
typedef char TX1FtdcTimeType[256];                /**< 交易所时间 */
typedef char TX1FtdcAbiPolicyCodeType[8];         /**< 套利策略代码数据类型 */

#define X1_FTDC_SP                        "SP"    ///< 跨期套利
#define X1_FTDC_SP_SPC                    "SPC"   ///< 两腿跨品种套利
#define X1_FTDC_SP_SPX                    "SPX"   ///< 压榨套利
#define X1_FTDC_SP_CALL                   "CSPR"  ///< Call Spread
#define X1_FTDC_SP_PUT                    "PSPR"  ///< Put Spread
#define X1_FTDC_SP_COMBO                  "COMBO" ///< Combo
#define X1_FTDC_SP_STRADDLE               "STD"   ///< Straddle
#define X1_FTDC_SP_STRANGLE               "STG"   ///< Strangle
#define X1_FTDC_SP_GUTS                   "GUTS"  ///< Guts
#define X1_FTDC_SP_SYNUND                 "SYN"   ///< Synthetic Underlying

typedef char TX1FtdcOrderPropertyType;            /**< TX1FtdcOrderPropertyType:订单属性 */

#define X1_FTDC_SP_NON                     '0'    ///无订单属性
#define X1_FTDC_SP_FAK                     '1'    ///FAK设置
#define X1_FTDC_SP_FOK                     '2'    ///FOK设置
#define X1_FTDC_SP_ANYPRICE                '3'    ///市价任意价
#define X1_FTDC_SP_ANYPRICE_TO_MKORDER     '4'    ///市价任意价转限价
#define X1_FTDC_SP_FIVELEVELPRICE          '5'    ///五档市价
#define X1_FTDC_SP_FIVELEVELPRICE_TO_LIMIT '6'    ///五档市价转限价
#define X1_FTDC_SP_BESTPRICE               '7'    ///最优价
#define X1_FTDC_SP_BESTPRICE_TO_LIMIT      '8'    ///最优价转限价


typedef int TX1FtdcInsertType;                    /**< 委托类别 */
#define X1_FTDC_BASIC_ORDER               0x0001  ///普通委托单
///自动单
#define X1_FTDC_AUTO_ORDER                0x0002  ///自动单

typedef int TX1FtdcOptionTypeType;                /**< 期权类别数据类型 */

#define X1_FTDC_OPT_CALL                   1      ///看涨
#define X1_FTDC_OPT_PUT                    2      ///看跌


typedef int TX1FtdcInstrumentTypeType;            /**< TX1FtdcInstrumentTypeType:合约类型数据类型 */

#define X1FTDC_INSTRUMENT_TYPE_COMM        0      ///期货
#define X1FTDC_INSTRUMENT_TYPE_OPT         1      ///期权


typedef char TX1FtdcCancelTypeType;               /**< TX1FtdcCancelTypeType:撤销标志数据类型 */

#define X1_FTDC_ORDER_BOOK                'O'     ///订单
#define X1_FTDC_ORDER_CANCEL              'W'     ///撤销


typedef char TX1FtdcContentType[501];             /**< TX1FtdcContentType:消息正文数据类型 */
typedef int TX1FtdcInstrumentStatusType;          /**< TX1FtdcInstrumentStatusType:合约交易状态数据类型 */
typedef short TX1FtdcInstStatusEnterReasonType;   /**< TX1FtdcInstStatusEnterReasonType:进入本状态原因数据类型 */
typedef char TX1FtdcCurrencyType[4];              /**< TX1FtdcCurrencyType:币种数据类型 */

typedef double TX1FtdcCurrencyUnitType;
typedef double TX1FtdcExchangeRateType;

typedef int TX1FtdcConfirmMarkType;               /**< TX1FtdcConfirmType:确认标志数据类型 */

#define X1_FTDC_CON_CONFIRM                2      ///< 确认


typedef char TX1FtdcStanAddrType[16];             /**< TX1FtdcStanAddrType:备用地址数据类型 */
typedef long TX1FtdcCapControlModeType;           /**< TX1FtdcCapControlModeType:资金控制方式数据类型 */

#define X1_FTDC_PPL_USABLE                 2      ///< 盯市盈亏可用 
#define  X1_FTDC_OF_USABLEFUND             4      ///< 平仓资金T+1可用 
#define  X1_FTDC_OF_FETCHABLEMARGIN        8      ///< 平仓保证金可取 
#define  X1_FTDC_AG_FETCHBLETODAY          16     ///< 本日盈亏可取 
#define  X1_FTDC_INTEREST_                 32     ///< 取后权益大于本日总入金 
#define  X1_FTDC_OF_FETCHABLE              128    ///< 平仓盈亏可取 
#define  X1_FTDC_PREMIUMOPTION_FETCHBLE    256    ///< 权利金收入可取 


typedef double TX1FtdcArchRatioType;              /**< TX1FtdcArchRatioType:转存比例数据类型 */
typedef int TX1FtdcSettlementBillTradeType;       /**< TX1FtdcSettlementBillTradeType:汇总标志数据类型 */

#define X1_FTDC_MATCHDETAIL                2      ///< 汇总成交明细
#define X1_FTDC_OPGAL                      4      ///< 汇总持仓盈亏
#define X1_FTDC_OFGAL                      8      ///< 汇总平仓盈亏


typedef int TX1FtdcFilesFlagType;                 /**< TX1FtdcFilesFlagType:档案类型数据类型 */

#define X1_FTDC_PRINT_MATCHDETAIL          4      ///成交明细打印
#define X1_FTDC_PRINT_OPGAL                8      ///持仓盈亏打印 
#define X1_FTDC_PRINT_OFGAL                16     ///平仓盈亏打印 
#define X1_FTDC_PRINT_ACCESSFUNDS          32     ///资金出入打印
#define X1_FTDC_PRINT_ADDMARGIN            64     ///追保声明打印


typedef char TX1FtdcSoftwareVendorIDType[33];     /**< TX1FtdcSoftwareVendorIDType:软件供应商编号数据类型 */
typedef long TX1FtdcProductOnlineCountType;       /**< TX1FtdcProductOnlineCountType:产品在线数量数据类型 */
typedef char TX1FtdcBrokerInfoType[20];           /**< TX1FtdcBrokerInfoType:期货公司名称数据类型 */
typedef char TX1FtdcProductIDType[33];            /**< TX1FtdcProductIDType:产品编号数据类型 */
typedef long TX1FtdcRequestIDType;                /**< TX1FtdcRequestIDType:请求ID数据类型 */
typedef char TX1FtdcCustomCategoryType[32];       /**< TX1FtdcCustomCategoryType:自定义类别数据类型 */
typedef int  TX1FtdcReservedType;                 /**< TX1FtdcReservedType:预留字段数据类型 */
typedef short TX1FtdcNoticeType;                  /**< TX1FtdcNoticeType:消息数据类型 */

#define X1_FTDC_SYS_BROADCAST_MSG          1      ///< 系统广播
#define X1_FTDC_ACCOUNT_ID_MSG             2      ///< 指定客户

////////////////////////////////////////////////////////////
///TX1FtdcTradingSegmentSNType:交易阶段编号数据类型
////////////////////////////////////////////////////////////
typedef int TX1FtdcTradingSegmentSNType;


//////////////////////////////////////////////
///TX1FtdcExtOrderType:算法单类型数据类型
//////////////////////////////////////////////
typedef int TX1FtdcExtOrderType;
///行情触发单
#define X1_FTDC_PRICESTRIGGER              1
///成交触发单
#define X1_FTDC_MATCHTRIGGER               2
///止损止盈单
#define X1_FTDC_PROFITLOSS                 3
///套利触发
#define X1_FTDC_ARBITRAGE                  4
///行情触发修改
#define X1_FTDC_PRICESTRIGGER_MODIFY       6
///成交触发修改
#define X1_FTDC_MATCHTRIGGER_MODIFY        7
///止损止盈触发修改
#define X1_FTDC_PROFITLOSS_MODIFY          8
///组合触发修改
#define X1_FTDC_ARBITRAGE_MODIFY           9
///预埋单
#define X1_FTDC_YMORDER                    1
///条件单
#define X1_FTDC_TJORDER                    2
///跨期套利订单
#define X1_FTDC_KQTLDD                     3
///跨品种套利订单
#define X1_FTDC_KPZTLDD                    4
///蝶式套利订单
#define X1_FTDC_DSTLDD                     5
///自定义套利订单(暂不支持)
#define X1_FTDC_ZDYTLDD                    6

//////////////////////////////////////////////
///TX1FtdcTriggerTime:触发时间数据类型
//////////////////////////////////////////////
typedef char TX1FtdcTriggerTime[10];


//////////////////////////////////////////////
///TX1FtdcPriceReference:价格参照数据类型
//////////////////////////////////////////////
typedef int TX1FtdcPriceReference;

///参照最新价
#define X1_FTDC_REF_LASTPRICE             0
///参照买一价
#define X1_FTDC_REF_BIDPRICE              1
///参照卖出价
#define X1_FTDC_REF_ASKPRICE              2

//////////////////////////////////////////////
///TX1FtdcCompareFlag:比较标志数据类型
//////////////////////////////////////////////
typedef int TX1FtdcCompareFlag;

///大于
#define X1_FTDC_CF_GREATER                0
///大于等于
#define X1_FTDC_CF_NOTLESS                1
///小于
#define X1_FTDC_CF_LESS                   2
///小于等于
#define X1_FTDC_CF_NOTGREATER             3

//////////////////////////////////////////////
///TX1FtdcOvernightFlag:隔夜标志数据类型
//////////////////////////////////////////////
typedef int TX1FtdcOvernightFlag;

///隔夜
#define X1_FTDC_OVERNIGHT                 1
///不隔夜
#define X1_FTDC_NOT_OVERNIGHT             2

//////////////////////////////////////////////
///TX1FtdcArbitragePrice:套利价格数据类型
//////////////////////////////////////////////
typedef double TX1FtdcArbitragePrice;


//////////////////////////////////////////////
///TX1FtdcExtTriggerCond:触发条件数据类型
//////////////////////////////////////////////
typedef int TX1FtdcExtTriggerCond;

///价格触发
#define X1_FTDC_TRIGGER_PRICE             0
///时间触发
#define X1_FTDC_TRIGGER_TIME              1


////////////////////////////////////////////////////////////
///TX1FtdcInstrumentMaturityType:合约最后交易日
////////////////////////////////////////////////////////////
typedef char TX1FtdcInstrumentMaturityType[12];


////////////////////////////////////////////////////////////
///TX1FtdcInstrumenExpirationDateType:合约到期日
////////////////////////////////////////////////////////////
typedef char TX1FtdcInstrumenExpirationDateType[12];


////////////////////////////////////////////////////////////
///TX1FtdcAdjustmentInfoType:组合或对锁的保证金调整信息
///格式:[合约代码,买卖标志,投资类别,调整金额;]
////////////////////////////////////////////////////////////
typedef char TX1FtdcAdjustmentInfoType[64];


////////////////////////////////////////////////////////////
///TX1FtdcQuoteIDType:询价编号
////////////////////////////////////////////////////////////
typedef char TX1FtdcQuoteIDType[33];


////////////////////////////////////////////////////////////
///TX1FtdcSourceType:来源
////////////////////////////////////////////////////////////
typedef short TX1FtdcSourceType;

///会员
#define X1_FTDC_SOURCE_MEMBER             0
///交易所
#define X1_FTDC_SOURCE_EXCHANGE           1


////////////////////////////////////////////////////////////
///TX1FtdcSeatCodeType:席位代码
////////////////////////////////////////////////////////////
typedef char TX1FtdcSeatCodeType[33];


////////////////////////////////////////////////////////////
///TX1FtdcCloseIDType:平仓执行单号
////////////////////////////////////////////////////////////
typedef char TX1FtdcCloseIDType[33];


////////////////////////////////////////////////////////////
///TX1FtdcEntrusTellerType:委托柜员
////////////////////////////////////////////////////////////
typedef char TX1FtdcEntrusTellerType[41];


////////////////////////////////////////////////////////////
///TX1FtdcStayTimeType：停留时间数据类型
////////////////////////////////////////////////////////////
typedef int TX1FtdcStayTimeType;


////////////////////////////////////////////////////////////
///TX1FtdcComputeModeType：计算方式数据类型
////////////////////////////////////////////////////////////
typedef int TX1FtdcComputeModeType;
///绝对数值计算
#define X1_FTDC_ABSOLUTE_VALUE_COMPUTE        0
///交易所保证金标准基础上浮动
#define X1_FTDC_EXCHANGE_MARGIN_BASIS_FLOAT   1 
///交易所保证金结果基础上浮动
#define X1_FTDC_EXCHANGE_MARGIN_RESULT_FLOAT  2
///期货保证金标准基础上浮动
#define X1_FTDC_FUTURES_MARGIN_BASIS_FLOAT    3


/////////////////////////////////////////////////////////////////////////
///TX1FtdcPriceNoteType:期权保证金计算方式
/////////////////////////////////////////////////////////////////////////
typedef int TX1FtdcPriceNoteType;
///按照昨结算价计算
#define X1_FTDC_CALC_BY_PRESETTLEMENT        1
///按照最新价计算
#define X1_FTDC_CALC_BY_LASTPRICE            2

/////////////////////////////////////////////////////////////////////////
///TX1FtdcLargeMarginDirectType:大边保证金方向数据类型
/////////////////////////////////////////////////////////////////////////
typedef char TX1FtdcLargeMarginDirectType[128];

/////////////////////////////////////////////////////////////////////////
///TX1FtdcBankIDType:银行代码类型
/////////////////////////////////////////////////////////////////////////
typedef char TX1FtdcBankIDType[8];

/////////////////////////////////////////////////////////////////////////
///TX1FtdcBankNameType:银行名称类型
/////////////////////////////////////////////////////////////////////////
typedef char TX1FtdcBankNameType[101];

/////////////////////////////////////////////////////////////////////////
///TX1FtdcBankSerialType:银行流水号类型
/////////////////////////////////////////////////////////////////////////
typedef char TX1FtdcBankSerialType[13];

/////////////////////////////////////////////////////////////////////////
///TX1FtdcSerialType:流水号类型
/////////////////////////////////////////////////////////////////////////
typedef int TX1FtdcSerialType;

/////////////////////////////////////////////////////////////////////////
///TX1FtdcBankAccountType:银行账户类型
/////////////////////////////////////////////////////////////////////////
typedef char TX1FtdcBankAccountType[41];

/////////////////////////////////////////////////////////////////////////
///TX1FtdcFutureSerialType:期货公司流水号类型
/////////////////////////////////////////////////////////////////////////
typedef int TX1FtdcFutureSerialType;

/////////////////////////////////////////////////////////////////////////
///TX1FtdcDigestType:摘要类型
/////////////////////////////////////////////////////////////////////////
typedef char TX1FtdcDigestType[36];

/////////////////////////////////////////////////////////////////////////
///TX1FtdcBankAccTypeType是一个银行帐号类型类型
/////////////////////////////////////////////////////////////////////////
///银行存折
#define X1_FTDC_BAT_BankBook               '1'
///储蓄卡
#define X1_FTDC_BAT_SavingCard             '2'
///信用卡
#define X1_FTDC_BAT_CreditCard             '3'
typedef char TX1FtdcBankAccTypeType;

/////////////////////////////////////////////////////////////////////////
///TX1FtdcTransferStatusType:转账交易状态类型
/////////////////////////////////////////////////////////////////////////
///正常
#define X1_FTDC_TRFS_Normal                '0'
///被冲正
#define X1_FTDC_TRFS_Repealed              '1'
typedef char TX1FtdcTransferStatusType;

/////////////////////////////////////////////////////////////////////////
///TX1FtdcTransferType:银期转账业务类型
/////////////////////////////////////////////////////////////////////////
typedef int TX1FtdcTransferType;

/////////////////////////////////////////////////////////////////////////
///TX1FtdcTransferType:银期转账处理结果类型
/////////////////////////////////////////////////////////////////////////
typedef int TX1FtdcProcResultType;
///成功
#define X1_FTDC_PROC_SUCCESS               0
///失败
#define X1_FTDC_PROC_FAIL                  1
///等待回执
#define X1_FTDC_PROC_WAIT_RTN              2


/////////////////////////////////////////////////////////////////////////
///TX1FtdcApplyNumberType:银期转账申请号类型
/////////////////////////////////////////////////////////////////////////
typedef int TX1FtdcApplyNumberType;


//********************扩展行情扩充**********************

/////////////////////////////////////////////////////////////////////////
///TX1FtdcImpliedVolatilityType:隐含波动率类型
/////////////////////////////////////////////////////////////////////////
typedef double TX1FtdcImpliedVolatilityType;

/////////////////////////////////////////////////////////////////////////
///TX1FtdcOptionComputationType:期权计算数据类型
/////////////////////////////////////////////////////////////////////////
typedef double TX1FtdcOptionComputationType;

////////////////////////////////////////////////////////////
///TX1FtdcFunctionIDType:行情扩展功能号
////////////////////////////////////////////////////////////
typedef char TX1FtdcFunctionIDType[16];

////////////////////////////////////////////////////////////
///TX1FtdcExtMarketDataType:行情扩展功能号
////////////////////////////////////////////////////////////
typedef char TX1FtdcExtMarketDataType[128];

//********************扩展行情扩充**********************


/////////////////////////////////////////////////////////////////////////
///TX1FtdcExchangeStatusType:交易所状态数据类型
/////////////////////////////////////////////////////////////////////////
typedef int TX1FtdcExchangeStatusType;
///开盘前
#define X1_FTDC_IS_BEFORETRADING           0
///非交易
#define X1_FTDC_IS_NOTRADING               1
///连续交易
#define X1_FTDC_IS_CONTINOUS               2
///集合竞价报单
#define X1_FTDC_IS_AUCTIONORDERING         3
///集合竞价价格平衡
#define X1_FTDC_IS_AUCTIONBALANCE          4
///集合竞价撮合
#define X1_FTDC_IS_AUCTIONMATCH            5
///收盘
#define X1_FTDC_IS_CLOSED                  6


/////////////////////////////////////////////////////////////////////////
///TX1FtdcPositionDateType:持仓日期类型
/////////////////////////////////////////////////////////////////////////
typedef int TX1FtdcPositionDateType;
//今仓
#define X1_FTDC_PSD_TODAY                  1
//历史仓
#define X1_FTDC_PSD_HISTORY                2

/////////////////////////////////////////////////////////////////////////
///TX1FtdcX1PriFlowIDType:私有流编号类型
/////////////////////////////////////////////////////////////////////////
typedef int TX1FtdcX1PriFlowIDType;

/// 私有流请求标志
#define X1_PrivateFlow_Req_Quick          0    // 只传送登录后私有流的内容
#define X1_PrivateFlow_Req_Restart        1    // 从本交易日开始重传
#define X1_PrivateFlow_Req_Resume         2    // 从上次收到的续传
#define X1_PrivateFlow_Req_Specify        3    // 从指定编号开始续传
////////////////////////////////////////////////////////////
///DFITCCalculateMode:冻结资金计算方式
////////////////////////////////////////////////////////////
typedef int TX1FtdcCalculateMode;

#define X1_CAlCULATEMODE_UPPPERLIMITPRICE 1    //涨停板价格
#define X1_CAlCULATEMODE_LOWERLIMITPRICE  2    //跌停板价格
#define X1_CAlCULATEMODE_SPECIFIEDPRICE   3    //指定价格

#endif//X1FTDCAPIDATATYPE_H
