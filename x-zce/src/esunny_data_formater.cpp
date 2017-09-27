/*
 * esunny_data_formater.cpp
 *
 *  Created on: 2013-8-13
 *      Author: oliver
 */

#include "esunny_data_formater.h"

using namespace std;

std::string ESUNNYDatatypeFormater::ToString(const TapAPITradeLoginAuth* p)
{
    char buf[10240];
    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPITradeLoginAuth\n"
            "\tUserNo=%s\n"
            "\tISModifyPassword=%02X\n"
            "\tPassword=%s\n"
            "\tNewPassword=%s\n"
            "\tISDDA=%02X\n"
            "\tDDASerialNo=%s\n",
            p->UserNo,                   //用户名
            p->ISModifyPassword,         //是否修改密码
            p->Password,                 //密码
            p->NewPassword,              //新密码
            p->ISDDA,                    //是否需要动态认证
            p->DDASerialNo               //动态认证码
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPITradeLoginAuth <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPICommodity* p)
{
    char buf[10240];
    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPICommodity\n"
            "\tExchangeNo=%s\n"
            "\tCommodityType=%02X\n"
            "\tCommodityNo=%s\n",
            p->ExchangeNo,                     ///< 交易所编码
            p->CommodityType,                  ///< 品种类型
            p->CommodityNo                     ///< 品种编号
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPICommodity <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIAccQryReq* p)
{
    char buf[10240];
    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIAccQryReq\n"
            "\tAccountNo=%s\n",
            p->AccountNo                       ///< 资金账号
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIAccQryReq <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIFundReq* p)
{
    char buf[10240];
    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIFundReq\n"
            "\tAccountNo=%s\n",
            p->AccountNo                       ///< 资金账号
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIFundReq <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPINewOrder* p)
{
    char buf[10240];
    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPINewOrder\n"
            "\tAccountNo=%s\n"
            "\tExchangeNo=%s\n"
            "\tCommodityType=%02X\n"
            "\tCommodityNo=%s\n"
            "\tContractNo=%s\n"
            "\tStrikePrice=%s\n"
            "\tCallOrPutFlag=%02X\n"
            "\tContractNo2=%s\n"
            "\tStrikePrice2=%s\n"
            "\tCallOrPutFlag2=%02X\n"
            "\tOrderType=%02X\n"
            "\tOrderSource=%02X\n"
            "\tTimeInForce=%02X\n"
            "\tExpireTime=%s\n"
            "\tIsRiskOrder=%02X\n"
            "\tOrderSide=%02X\n"
            "\tPositionEffect=%02X\n"
            "\tPositionEffect2=%02X\n"
            "\tInquiryNo=%s\n"
            "\tHedgeFlag=%02X\n"
            "\tOrderPrice=%.4f\n"
            "\tOrderPrice2=%.4f\n"
            "\tStopPrice=%.4f\n"
            "\tOrderQty=%u\n"
            "\tOrderMinQty=%u\n"
            "\tMinClipSize=%u\n"
            "\tMaxClipSize=%u\n"
            "\tRefInt=%d\n"
            "\tRefString=%s\n"
            "\tTacticsType=%02X\n"
            "\tTriggerCondition=%02X\n"
            "\tTriggerPriceType=%02X\n"
            "\tAddOneIsValid=%02X\n"
            "\tOrderQty2=%u\n"
            "\tHedgeFlag2=%02X\n"
            "\tMarketLevel=%u\n",
            p->AccountNo,                      ///< 客户资金帐号
            p->ExchangeNo,                     ///< 交易所编号
            p->CommodityType,                  ///< 品种类型
            p->CommodityNo,                    ///< 品种编码类型
            p->ContractNo,                     ///< 合约1
            p->StrikePrice,                    ///< 执行价格1
            p->CallOrPutFlag,                  ///< 看张看跌1
            p->ContractNo2,                    ///< 合约2
            p->StrikePrice2,                   ///< 执行价格2
            p->CallOrPutFlag2,                 ///< 看张看跌2
            p->OrderType,                      ///< 委托类型
            p->OrderSource,                    ///< 委托来源
            p->TimeInForce,                    ///< 委托有效类型
            p->ExpireTime,                     ///< 有效日期(GTD情况下使用)
            p->IsRiskOrder,                    ///< 是否风险报单
            p->OrderSide,                      ///< 买入卖出
            p->PositionEffect,                 ///< 开平标志1
            p->PositionEffect2,                ///< 开平标志2
            p->InquiryNo,                      ///< 询价号
            p->HedgeFlag,                      ///< 投机保值
            p->OrderPrice,                     ///< 委托价格1
            p->OrderPrice2,                    ///< 委托价格2，做市商应价使用
            p->StopPrice,                      ///< 触发价格
            p->OrderQty,                       ///< 委托数量
            p->OrderMinQty,                    ///< 最小成交量
            p->MinClipSize,                    ///< 冰山单最小随机量
            p->MaxClipSize,                    ///< 冰山单最大随机量
            p->RefInt,                         ///< 整型参考值
            p->RefString,                      ///< 字符串参考值
            p->TacticsType,                    ///< 策略单类型
            p->TriggerCondition,               ///< 触发条件
            p->TriggerPriceType,               ///< 触发价格类型
            p->AddOneIsValid,                  ///< 是否T+1有效
            p->OrderQty2,                      ///< 委托数量2
            p->HedgeFlag2,                     ///< 投机保值2
            p->MarketLevel                     ///< 市价撮合深度
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPINewOrder <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIOrderCancelReq* p)
{
    char buf[10240];
    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIOrderCancelReq\n"
            "\tRefInt=%d\n"
            "\tRefString=%s\n"
            "\tServerFlag=%02X\n"
            "\tOrderNo=%s\n",
            p->RefInt,                         ///< 整型参考值
            p->RefString,                      ///< 字符串参考值
            p->ServerFlag,                     ///< 服务器标识
            p->OrderNo                         ///< 委托编码
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIOrderCancelReq <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIOrderQryReq* p)
{
    char buf[10240];
    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIOrderQryReq\n"
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIOrderQryReq <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIOrderProcessQryReq* p)
{
    char buf[10240];
    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIOrderProcessQryReq\n"
            "\tServerFlag=%02X\n"
            "\tOrderNo=%s\n",
            p->ServerFlag,                     ///< 服务器标识
            p->OrderNo                         ///< 委托编码
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIOrderProcessQryReq <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPITradeLoginRspInfo* p)
{
    char buf[10240];
    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPITradeLoginRspInfo\n"
            "\tUserNo=%s\n"
            "\tUserType=%d\n"
            "\tUserName=%s\n"
            "\tQuoteTempPassword=%s\n"
            "\tReservedInfo=%s\n"
            "\tLastLoginIP=%s\n"
            "\tLastLoginProt=%u\n"
            "\tLastLoginTime=%s\n"
            "\tLastLogoutTime=%s\n"
            "\tTradeDate=%s\n"
            "\tLastSettleTime=%s\n"
            "\tStartTime=%s\n"
            "\tInitTime=%s\n",
            p->UserNo,                         ///< 用户编号
            p->UserType,                       ///< 用户类型
            p->UserName,                       ///< 用户名
            p->QuoteTempPassword,              ///< 行情临时密码
            p->ReservedInfo,                   ///< 预留信息
            p->LastLoginIP,                    ///< 上次登录IP
            p->LastLoginProt,                  ///< 上次登录端口
            p->LastLoginTime,                  ///< 上次登录时间
            p->LastLogoutTime,                 ///< 上次退出时间
            p->TradeDate,                      ///< 当前交易日期
            p->LastSettleTime,                 ///< 上次结算时间
            p->StartTime,                      ///< 系统启动时间
            p->InitTime                        ///< 系统初始化时间
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPITradeLoginRspInfo <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIAccountInfo* p)
{
    char buf[10240];
    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIAccountInfo\n"
            "\tAccountNo=%s\n"
            "\tAccountType=%02X\n"
            "\tAccountState=%02X\n"
            "\tAccountShortName=%s\n"
            "\tAccountIsDocHolder=%02X\n"
            "\tIsMarketMaker=%02X\n"
            "\tAccountFamilyType=%02X\n",
            p->AccountNo,                      ///< 资金账号
            p->AccountType,                    ///< 账号类型
            p->AccountState,                   ///< 账号状态
            p->AccountShortName,               ///< 账号简称
            p->AccountIsDocHolder,             ///< 是否持证者
            p->IsMarketMaker,                  ///< 是否是做市商
            p->AccountFamilyType               ///< 父子账号类型
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIAccountInfo <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIFundData* p)
{
    char buf[10240];
    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIFundData\n"
            "\tAccountNo=%s\n"
            "\tParentAccountNo=%s\n"
            "\tCurrencyGroupNo=%s\n"
            "\tCurrencyNo=%s\n"
            "\tTradeRate=%.6f\n"
            "\tFutureAlg=%02X\n"
            "\tOptionAlg=%02X\n"
            "\tPreBalance=%.6f\n"
            "\tPreUnExpProfit=%.6f\n"
            "\tPreLMEPositionProfit=%.6f\n"
            "\tPreEquity=%.6f\n"
            "\tPreAvailable1=%.6f\n"
            "\tPreMarketEquity=%.6f\n"
            "\tCashInValue=%.6f\n"
            "\tCashOutValue=%.6f\n"
            "\tCashAdjustValue=%.6f\n"
            "\tCashPledged=%.6f\n"
            "\tFrozenFee=%.6f\n"
            "\tFrozenDeposit=%.6f\n"
            "\tAccountFee=%.6f\n"
            "\tExchangeFee=%.6f\n"
            "\tAccountDeliveryFee=%.6f\n"
            "\tPremiumIncome=%.6f\n"
            "\tPremiumPay=%.6f\n"
            "\tCloseProfit=%.6f\n"
            "\tDeliveryProfit=%.6f\n"
            "\tUnExpProfit=%.6f\n"
            "\tExpProfit=%.6f\n"
            "\tPositionProfit=%.6f\n"
            "\tLmePositionProfit=%.6f\n"
            "\tOptionMarketValue=%.6f\n"
            "\tAccountIntialMargin=%.6f\n"
            "\tAccountMaintenanceMargin=%.6f\n"
            "\tUpperInitalMargin=%.6f\n"
            "\tUpperMaintenanceMargin=%.6f\n"
            "\tDiscount=%.6f\n"
            "\tBalance=%.6f\n"
            "\tEquity=%.6f\n"
            "\tAvailable=%.6f\n"
            "\tCanDraw=%.6f\n"
            "\tMarketEquity=%.6f\n"
            "\tOriginalCashInOut=%.6f\n"
            "\tFloatingPL=%.6f\n"
            "\tFrozenRiskFundValue=%.6f\n"
            "\tClosePL=%.6f\n"
            "\tNoCurrencyPledgeValue=%.6f\n"
            "\tPrePledgeValue=%.6f\n"
            "\tPledgeIn=%.6f\n"
            "\tPledgeOut=%.6f\n"
            "\tPledgeValue=%.6f\n"
            "\tBorrowValue=%.6f\n"
            "\tSpecialAccountFrozenMargin=%.6f\n"
            "\tSpecialAccountMargin=%.6f\n"
            "\tSpecialAccountFrozenFee=%.6f\n"
            "\tSpecialAccountFee=%.6f\n"
            "\tSpecialFloatProfit=%.6f\n"
            "\tSpecialCloseProfit=%.6f\n"
            "\tSpecialFloatPL=%.6f\n"
            "\tSpecialClosePL=%.6f\n",
            p->AccountNo,                      ///< 客户资金账号
            p->ParentAccountNo,                ///< 上级资金账号
            p->CurrencyGroupNo,                ///< 币种组号
            p->CurrencyNo,                     ///< 币种号(为空表示币种组基币资金)
            p->TradeRate,                      ///< 交易汇率
            p->FutureAlg,                      ///< 期货算法
            p->OptionAlg,                      ///< 期权算法
            p->PreBalance,                     ///< 上日结存
            p->PreUnExpProfit,                 ///< 上日未到期平盈
            p->PreLMEPositionProfit,           ///< 上日LME持仓平盈
            p->PreEquity,                      ///< 上日权益
            p->PreAvailable1,                  ///< 上日可用
            p->PreMarketEquity,                ///< 上日市值权益
            p->CashInValue,                    ///< 入金
            p->CashOutValue,                   ///< 出金
            p->CashAdjustValue,                ///< 资金调整
            p->CashPledged,                    ///< 质押资金
            p->FrozenFee,                      ///< 冻结手续费
            p->FrozenDeposit,                  ///< 冻结保证金
            p->AccountFee,                     ///< 客户手续费包含交割手续费
            p->ExchangeFee,                    ///< 汇兑手续费
            p->AccountDeliveryFee,             ///< 客户交割手续费
            p->PremiumIncome,                  ///< 权利金收取
            p->PremiumPay,                     ///< 权利金支付
            p->CloseProfit,                    ///< 平仓盈亏
            p->DeliveryProfit,                 ///< 交割盈亏
            p->UnExpProfit,                    ///< 未到期平盈
            p->ExpProfit,                      ///< 到期平仓盈亏
            p->PositionProfit,                 ///< 不含LME持仓盈亏
            p->LmePositionProfit,              ///< LME持仓盈亏
            p->OptionMarketValue,              ///< 期权市值
            p->AccountIntialMargin,            ///< 客户初始保证金
            p->AccountMaintenanceMargin,       ///< 客户维持保证金
            p->UpperInitalMargin,              ///< 上手初始保证金
            p->UpperMaintenanceMargin,         ///< 上手维持保证金
            p->Discount,                       ///< LME贴现
            p->Balance,                        ///< 当日结存
            p->Equity,                         ///< 当日权益
            p->Available,                      ///< 当日可用
            p->CanDraw,                        ///< 可提取
            p->MarketEquity,                   ///< 账户市值
            p->OriginalCashInOut,              ///< 币种原始出入金
            p->FloatingPL,                     ///< 逐笔浮盈
            p->FrozenRiskFundValue,            ///< 风险冻结资金
            p->ClosePL,                        ///< 逐笔平盈
            p->NoCurrencyPledgeValue,          ///< 非货币质押
            p->PrePledgeValue,                 ///< 期初质押
            p->PledgeIn,                       ///< 质入
            p->PledgeOut,                      ///< 质出
            p->PledgeValue,                    ///< 质押余额
            p->BorrowValue,                    ///< 借用金额
            p->SpecialAccountFrozenMargin,     ///< 特殊产品冻结保证金
            p->SpecialAccountMargin,           ///< 特殊产品保证金
            p->SpecialAccountFrozenFee,        ///< 特殊产品冻结手续费
            p->SpecialAccountFee,              ///< 特殊产品手续费
            p->SpecialFloatProfit,             ///< 特殊产品浮盈
            p->SpecialCloseProfit,             ///< 特殊产品平盈
            p->SpecialFloatPL,                 ///< 特殊产品逐笔浮盈
            p->SpecialClosePL                  ///< 特殊产品逐笔平盈
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIFundData <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIExchangeInfo* p)
{
    char buf[10240];
    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIExchangeInfo\n"
            "\tExchangeNo=%s\n"
            "\tExchangeName=%s\n",
            p->ExchangeNo,                     ///< 交易所编码
            p->ExchangeName                    ///< 交易所名称
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIExchangeInfo <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPICommodityInfo* p)
{
    char buf[10240];
    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPICommodityInfo\n"
            "\tExchangeNo=%s\n"
            "\tCommodityType=%02X\n"
            "\tCommodityNo=%s\n"
            "\tCommodityName=%s\n"
            "\tCommodityEngName=%s\n"
            "\tRelateExchangeNo=%s\n"
            "\tRelateCommodityType=%02X\n"
            "\tRelateCommodityNo=%s\n"
            "\tRelateExchangeNo2=%s\n"
            "\tRelateCommodityType2=%02X\n"
            "\tRelateCommodityNo2=%s\n"
            "\tTradeCurrency=%s\n"
            "\tSettleCurrency=%s\n"
            "\tContractSize=%.6f\n"
            "\tMarginCalculateMode=%02X\n"
            "\tOptionMarginCalculateMode=%02X\n"
            "\tOpenCloseMode=%02X\n"
            "\tStrikePriceTimes=%.6f\n"
            "\tIsOnlyQuoteCommodity=%02X\n"
            "\tCommodityTickSize=%.6f\n"
            "\tCommodityDenominator=%d\n"
            "\tCmbDirect=%02X\n"
            "\tOnlyCanCloseDays=%d\n"
            "\tDeliveryMode=%02X\n"
            "\tDeliveryDays=%d\n"
            "\tAddOneTime=%s\n"
            "\tCommodityTimeZone=%d\n",
            p->ExchangeNo,                     ///< 交易所编码
            p->CommodityType,                  ///< 品种类型
            p->CommodityNo,                    ///< 品种编号
            p->CommodityName,                  ///< 品种名称
            p->CommodityEngName,               ///< 品种英文名称
            p->RelateExchangeNo,               ///< 关联品种交易所编码
            p->RelateCommodityType,            ///< 关联品种品种类型
            p->RelateCommodityNo,              ///< 关联品种品种编号
            p->RelateExchangeNo2,              ///< 关联品种2交易所编码
            p->RelateCommodityType2,           ///< 关联品种2品种类型
            p->RelateCommodityNo2,             ///< 关联品种2品种编号
            p->TradeCurrency,                  ///< 交易币种
            p->SettleCurrency,                 ///< 结算币种
            p->ContractSize,                   ///< 每手乘数
            p->MarginCalculateMode,            ///< 期货保证金方式,分笔'1',锁仓'2'
            p->OptionMarginCalculateMode,      ///< 期权保证金公式
            p->OpenCloseMode,                  ///< 开平方式
            p->StrikePriceTimes,               ///< 执行价格倍数
            p->IsOnlyQuoteCommodity,           ///< 单行情品种
            p->CommodityTickSize,              ///< 最小变动价位
            p->CommodityDenominator,           ///< 报价分母
            p->CmbDirect,                      ///< 组合方向
            p->OnlyCanCloseDays,               ///< 只可平仓提前天数
            p->DeliveryMode,                   ///< 交割行权方式
            p->DeliveryDays,                   ///< 交割日偏移
            p->AddOneTime,                     ///< T+1分割时间
            p->CommodityTimeZone               ///< 品种时区
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPICommodityInfo <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPITradeContractInfo* p)
{
    char buf[10240];
    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPITradeContractInfo\n"
            "\tExchangeNo=%s\n"
            "\tCommodityType=%02X\n"
            "\tCommodityNo=%s\n"
            "\tContractNo1=%s\n"
            "\tStrikePrice1=%s\n"
            "\tCallOrPutFlag1=%02X\n"
            "\tContractNo2=%s\n"
            "\tStrikePrice2=%s\n"
            "\tCallOrPutFlag2=%02X\n"
            "\tContractType=%02X\n"
            "\tQuoteUnderlyingContract=%s\n"
            "\tContractName=%s\n"
            "\tContractExpDate=%s\n"
            "\tLastTradeDate=%s\n"
            "\tFirstNoticeDate=%s\n"
            "\tFutureContractNo=%s\n",
            p->ExchangeNo,                     ///< 交易所编码
            p->CommodityType,                  ///< 品种类型
            p->CommodityNo,                    ///< 品种编号
            p->ContractNo1,                    ///< 合约代码1
            p->StrikePrice1,                   ///< 执行价1
            p->CallOrPutFlag1,                 ///< 看涨看跌标示1
            p->ContractNo2,                    ///< 合约代码2
            p->StrikePrice2,                   ///< 执行价2
            p->CallOrPutFlag2,                 ///< 看涨看跌标示2
            p->ContractType,                   ///< 合约类型
            p->QuoteUnderlyingContract,        ///< 行情真实合约
            p->ContractName,                   ///< 合约名称
            p->ContractExpDate,                ///< 合约到期日
            p->LastTradeDate,                  ///< 最后交易日
            p->FirstNoticeDate,                ///< 首次通知日
            p->FutureContractNo                ///< 期货合约编号(期权标的)
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPITradeContractInfo <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIOrderInfoNotice* pp)
{
    char buf[10240];
    TapAPIOrderInfo *p = NULL;
    if (pp) p = pp->OrderInfo;

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIOrderInfoNotice\n"
            "\tSessionID=%u\n"
            "\tErrorCode=%u\n"
            "\tAccountNo=%s\n"
            "\tExchangeNo=%s\n"
            "\tCommodityType=%d\n"
            "\tCommodityNo=%s\n"
            "\tContractNo=%s\n"
            "\tStrikePrice=%s\n"
            "\tCallOrPutFlag=%02X\n"
            "\tContractNo2=%s\n"
            "\tStrikePrice2=%s\n"
            "\tCallOrPutFlag2=%02X\n"
            "\tOrderType=%02X\n"
            "\tOrderSource=%02X\n"
            "\tTimeInForce=%02X\n"
            "\tExpireTime=%s\n"
            "\tIsRiskOrder=%02X\n"
            "\tOrderSide=%02X\n"
            "\tPositionEffect=%02X\n"
            "\tPositionEffect2=%02X\n"
            "\tInquiryNo=%s\n"
            "\tHedgeFlag=%02X\n"
            "\tOrderPrice=%.4f\n"
            "\tOrderPrice2=%.4f\n"
            "\tStopPrice=%.4f\n"
            "\tOrderQty=%u\n"
            "\tOrderMinQty=%u\n"
            "\tOrderCanceledQty=%u\n"
            "\tMinClipSize=%u\n"
            "\tMaxClipSize=%u\n"
            "\tLicenseNo=%s\n"
            "\tRefInt=%d\n"
            "\tRefString=%s\n"
            "\tParentAccountNo=%s\n"
            "\tServerFlag=%02X\n"
            "\tOrderNo=%s\n"
            "\tClientOrderNo=%s\n"
            "\tOrderLocalNo=%s\n"
            "\tOrderSystemNo=%s\n"
            "\tOrderExchangeSystemNo=%s\n"
            "\tOrderParentNo=%s\n"
            "\tOrderParentSystemNo=%s\n"
            "\tTradeNo=%s\n"
            "\tUpperNo=%s\n"
            "\tUpperChannelNo=%s\n"
            "\tUpperSettleNo=%s\n"
            "\tUpperUserNo=%s\n"
            "\tOrderInsertUserNo=%s\n"
            "\tOrderInsertTime=%s\n"
            "\tOrderCommandUserNo=%s\n"
            "\tOrderUpdateUserNo=%s\n"
            "\tOrderUpdateTime=%s\n"
            "\tOrderState=%02X\n"
            "\tOrderMatchPrice=%.4f\n"
            "\tOrderMatchPrice2=%.4f\n"
            "\tOrderMatchQty=%u\n"
            "\tOrderMatchQty2=%u\n"
            "\tErrorCode=%u\n"
            "\tErrorText=%s\n"
            "\tIsBackInput=%02X\n"
            "\tIsDeleted=%02X\n"
            "\tIsAddOne=%02X\n"
            "\tOrderStreamID=%u\n"
            "\tUpperStreamID=%u\n"
            "\tContractSize=%.6f\n"
            "\tContractSize2=%.6f\n"
            "\tCommodityCurrencyGroup=%s\n"
            "\tCommodityCurrency=%s\n"
            "\tFeeMode=%02X\n"
            "\tFeeParam=%.6f\n"
            "\tFeeCurrencyGroup=%s\n"
            "\tFeeCurrency=%s\n"
            "\tFeeMode2=%02X\n"
            "\tFeeParam2=%.6f\n"
            "\tFeeCurrencyGroup2=%s\n"
            "\tFeeCurrency2=%s\n"
            "\tMarginMode=%02X\n"
            "\tMarginParam=%.6f\n"
            "\tMarginMode2=%02X\n"
            "\tMarginParam2=%.6f\n"
            "\tPreSettlePrice=%.6f\n"
            "\tPreSettlePrice2=%.6f\n"
            "\tOpenVol=%u\n"
            "\tCoverVol=%u\n"
            "\tOpenVol2=%u\n"
            "\tCoverVol2=%u\n"
            "\tFeeValue=%.6f\n"
            "\tMarginValue=%.6f\n"
            "\tTacticsType=%02X\n"
            "\tTriggerCondition=%02X\n"
            "\tTriggerPriceType=%02X\n"
            "\tAddOneIsValid=%02X\n"
            "\tOrderQty2=%u\n"
            "\tHedgeFlag2=%02X\n"
            "\tMarketLevel=%u\n",
            pp->SessionID,                     ///< 会话ID
            pp->ErrorCode,                     ///< 错误码
            p->AccountNo,                      ///< 客户资金帐号
            p->ExchangeNo,                     ///< 交易所编号
            p->CommodityType,                  ///< 品种类型
            p->CommodityNo,                    ///< 品种编码类型
            p->ContractNo,                     ///< 合约1
            p->StrikePrice,                    ///< 执行价格1
            p->CallOrPutFlag,                  ///< 看张看跌1
            p->ContractNo2,                    ///< 合约2
            p->StrikePrice2,                   ///< 执行价格2
            p->CallOrPutFlag2,                 ///< 看张看跌2
            p->OrderType,                      ///< 委托类型
            p->OrderSource,                    ///< 委托来源
            p->TimeInForce,                    ///< 委托有效类型
            p->ExpireTime,                     ///< 有效日期(GTD情况下使用)
            p->IsRiskOrder,                    ///< 是否风险报单
            p->OrderSide,                      ///< 买入卖出
            p->PositionEffect,                 ///< 开平标志1
            p->PositionEffect2,                ///< 开平标志2
            p->InquiryNo,                      ///< 询价号
            p->HedgeFlag,                      ///< 投机保值
            p->OrderPrice,                     ///< 委托价格1
            p->OrderPrice2,                    ///< 委托价格2，做市商应价使用
            p->StopPrice,                      ///< 触发价格
            p->OrderQty,                       ///< 委托数量
            p->OrderMinQty,                    ///< 最小成交量
            p->OrderCanceledQty,               ///< 撤单数量
            p->MinClipSize,                    ///< 冰山单最小随机量
            p->MaxClipSize,                    ///< 冰山单最大随机量
            p->LicenseNo,                      ///< 软件授权号
            p->RefInt,                         ///< 整型参考值
            p->RefString,                      ///< 字符串参考值
            p->ParentAccountNo,                ///< 上级资金账号
            p->ServerFlag,                     ///< 服务器标识
            p->OrderNo,                        ///< 委托编码
            p->ClientOrderNo,                  ///< 客户端本地委托编号
            p->OrderLocalNo,                   ///< 本地号
            p->OrderSystemNo,                  ///< 系统号
            p->OrderExchangeSystemNo,          ///< 交易所系统号
            p->OrderParentNo,                  ///< 父单号
            p->OrderParentSystemNo,            ///< 父单系统号
            p->TradeNo,                        ///< 交易编码
            p->UpperNo,                        ///< 上手号
            p->UpperChannelNo,                 ///< 上手通道号
            p->UpperSettleNo,                  ///< 会员号和清算号
            p->UpperUserNo,                    ///< 上手登录号
            p->OrderInsertUserNo,              ///< 下单人
            p->OrderInsertTime,                ///< 下单时间
            p->OrderCommandUserNo,             ///< 录单操作人
            p->OrderUpdateUserNo,              ///< 委托更新人
            p->OrderUpdateTime,                ///< 委托更新时间
            p->OrderState,                     ///< 委托状态
            p->OrderMatchPrice,                ///< 成交价1
            p->OrderMatchPrice2,               ///< 成交价2
            p->OrderMatchQty,                  ///< 成交量1
            p->OrderMatchQty2,                 ///< 成交量2
            p->ErrorCode,                      ///< 最后一次操作错误信息码
            p->ErrorText,                      ///< 错误信息
            p->IsBackInput,                    ///< 是否为录入委托单
            p->IsDeleted,                      ///< 委托成交删除标
            p->IsAddOne,                       ///< 是否为T+1单
            p->OrderStreamID,                  ///< 委托流水号
            p->UpperStreamID,                  ///< 上手流号
            p->ContractSize,                   ///< 每手乘数，计算参数
            p->ContractSize2,                  ///< 每手乘数，计算参数
            p->CommodityCurrencyGroup,         ///< 品种币种组
            p->CommodityCurrency,              ///< 品种币种
            p->FeeMode,                        ///< 手续费计算方式
            p->FeeParam,                       ///< 手续费参数值 冻结手续费均按照开仓手续费计算
            p->FeeCurrencyGroup,               ///< 客户手续费币种组
            p->FeeCurrency,                    ///< 客户手续费币种
            p->FeeMode2,                       ///< 手续费计算方式
            p->FeeParam2,                      ///< 手续费参数值 冻结手续费均按照开仓手续费计算
            p->FeeCurrencyGroup2,              ///< 客户手续费币种组
            p->FeeCurrency2,                   ///< 客户手续费币种
            p->MarginMode,                     ///< 保证金计算方式
            p->MarginParam,                    ///< 保证金参数值
            p->MarginMode2,                    ///< 保证金计算方式
            p->MarginParam2,                   ///< 保证金参数值
            p->PreSettlePrice,                 ///< 昨结算价  比例方式的市价单和组合订单使用
            p->PreSettlePrice2,                ///< 昨结算价  比例方式的市价单和组合订单使用
            p->OpenVol,                        ///< 预开仓数量 委托数量中的开仓部分
            p->CoverVol,                       ///< 预平仓数量 委托数量中的平仓部分
            p->OpenVol2,                       ///< 预开仓数量 委托数量中的开仓部分
            p->CoverVol2,                      ///< 预平仓数量 委托数量中的平仓部分
            p->FeeValue,                       ///< 冻结手续费
            p->MarginValue,                    ///< 冻结保证金 合并计算的此项为0
            p->TacticsType,                    ///< 策略单类型
            p->TriggerCondition,               ///< 触发条件
            p->TriggerPriceType,               ///< 触发价格类型
            p->AddOneIsValid,                  ///< 是否T+1有效
            p->OrderQty2,                      ///< 委托数量2
            p->HedgeFlag2,                     ///< 投机保值2
            p->MarketLevel                     ///< 市价撮合深度
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIOrderInfoNotice <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIOrderActionRsp* pp)
{
    char buf[10240];
    TapAPIOrderInfo *p = NULL;
    if (pp) p = pp->OrderInfo;

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIOrderActionRsp\n"
            "\tActionType=%02X\n"
            "\tAccountNo=%s\n"
            "\tExchangeNo=%s\n"
            "\tCommodityType=%d\n"
            "\tCommodityNo=%s\n"
            "\tContractNo=%s\n"
            "\tStrikePrice=%s\n"
            "\tCallOrPutFlag=%02X\n"
            "\tContractNo2=%s\n"
            "\tStrikePrice2=%s\n"
            "\tCallOrPutFlag2=%02X\n"
            "\tOrderType=%02X\n"
            "\tOrderSource=%02X\n"
            "\tTimeInForce=%02X\n"
            "\tExpireTime=%s\n"
            "\tIsRiskOrder=%02X\n"
            "\tOrderSide=%02X\n"
            "\tPositionEffect=%02X\n"
            "\tPositionEffect2=%02X\n"
            "\tInquiryNo=%s\n"
            "\tHedgeFlag=%02X\n"
            "\tOrderPrice=%.4f\n"
            "\tOrderPrice2=%.4f\n"
            "\tStopPrice=%.4f\n"
            "\tOrderQty=%u\n"
            "\tOrderMinQty=%u\n"
            "\tOrderCanceledQty=%u\n"
            "\tMinClipSize=%u\n"
            "\tMaxClipSize=%u\n"
            "\tLicenseNo=%s\n"
            "\tRefInt=%d\n"
            "\tRefString=%s\n"
            "\tParentAccountNo=%s\n"
            "\tServerFlag=%02X\n"
            "\tOrderNo=%s\n"
            "\tClientOrderNo=%s\n"
            "\tOrderLocalNo=%s\n"
            "\tOrderSystemNo=%s\n"
            "\tOrderExchangeSystemNo=%s\n"
            "\tOrderParentNo=%s\n"
            "\tOrderParentSystemNo=%s\n"
            "\tTradeNo=%s\n"
            "\tUpperNo=%s\n"
            "\tUpperChannelNo=%s\n"
            "\tUpperSettleNo=%s\n"
            "\tUpperUserNo=%s\n"
            "\tOrderInsertUserNo=%s\n"
            "\tOrderInsertTime=%s\n"
            "\tOrderCommandUserNo=%s\n"
            "\tOrderUpdateUserNo=%s\n"
            "\tOrderUpdateTime=%s\n"
            "\tOrderState=%02X\n"
            "\tOrderMatchPrice=%.4f\n"
            "\tOrderMatchPrice2=%.4f\n"
            "\tOrderMatchQty=%u\n"
            "\tOrderMatchQty2=%u\n"
            "\tErrorCode=%u\n"
            "\tErrorText=%s\n"
            "\tIsBackInput=%02X\n"
            "\tIsDeleted=%02X\n"
            "\tIsAddOne=%02X\n"
            "\tOrderStreamID=%u\n"
            "\tUpperStreamID=%u\n"
            "\tContractSize=%.6f\n"
            "\tContractSize2=%.6f\n"
            "\tCommodityCurrencyGroup=%s\n"
            "\tCommodityCurrency=%s\n"
            "\tFeeMode=%02X\n"
            "\tFeeParam=%.6f\n"
            "\tFeeCurrencyGroup=%s\n"
            "\tFeeCurrency=%s\n"
            "\tFeeMode2=%02X\n"
            "\tFeeParam2=%.6f\n"
            "\tFeeCurrencyGroup2=%s\n"
            "\tFeeCurrency2=%s\n"
            "\tMarginMode=%02X\n"
            "\tMarginParam=%.6f\n"
            "\tMarginMode2=%02X\n"
            "\tMarginParam2=%.6f\n"
            "\tPreSettlePrice=%.6f\n"
            "\tPreSettlePrice2=%.6f\n"
            "\tOpenVol=%u\n"
            "\tCoverVol=%u\n"
            "\tOpenVol2=%u\n"
            "\tCoverVol2=%u\n"
            "\tFeeValue=%.6f\n"
            "\tMarginValue=%.6f\n"
            "\tTacticsType=%02X\n"
            "\tTriggerCondition=%02X\n"
            "\tTriggerPriceType=%02X\n"
            "\tAddOneIsValid=%02X\n"
            "\tOrderQty2=%u\n"
            "\tHedgeFlag2=%02X\n"
            "\tMarketLevel=%u\n",
            pp->ActionType,                    ///< 操作类型
            p->AccountNo,                      ///< 客户资金帐号
            p->ExchangeNo,                     ///< 交易所编号
            p->CommodityType,                  ///< 品种类型
            p->CommodityNo,                    ///< 品种编码类型
            p->ContractNo,                     ///< 合约1
            p->StrikePrice,                    ///< 执行价格1
            p->CallOrPutFlag,                  ///< 看张看跌1
            p->ContractNo2,                    ///< 合约2
            p->StrikePrice2,                   ///< 执行价格2
            p->CallOrPutFlag2,                 ///< 看张看跌2
            p->OrderType,                      ///< 委托类型
            p->OrderSource,                    ///< 委托来源
            p->TimeInForce,                    ///< 委托有效类型
            p->ExpireTime,                     ///< 有效日期(GTD情况下使用)
            p->IsRiskOrder,                    ///< 是否风险报单
            p->OrderSide,                      ///< 买入卖出
            p->PositionEffect,                 ///< 开平标志1
            p->PositionEffect2,                ///< 开平标志2
            p->InquiryNo,                      ///< 询价号
            p->HedgeFlag,                      ///< 投机保值
            p->OrderPrice,                     ///< 委托价格1
            p->OrderPrice2,                    ///< 委托价格2，做市商应价使用
            p->StopPrice,                      ///< 触发价格
            p->OrderQty,                       ///< 委托数量
            p->OrderMinQty,                    ///< 最小成交量
            p->OrderCanceledQty,               ///< 撤单数量
            p->MinClipSize,                    ///< 冰山单最小随机量
            p->MaxClipSize,                    ///< 冰山单最大随机量
            p->LicenseNo,                      ///< 软件授权号
            p->RefInt,                         ///< 整型参考值
            p->RefString,                      ///< 字符串参考值
            p->ParentAccountNo,                ///< 上级资金账号
            p->ServerFlag,                     ///< 服务器标识
            p->OrderNo,                        ///< 委托编码
            p->ClientOrderNo,                  ///< 客户端本地委托编号
            p->OrderLocalNo,                   ///< 本地号
            p->OrderSystemNo,                  ///< 系统号
            p->OrderExchangeSystemNo,          ///< 交易所系统号
            p->OrderParentNo,                  ///< 父单号
            p->OrderParentSystemNo,            ///< 父单系统号
            p->TradeNo,                        ///< 交易编码
            p->UpperNo,                        ///< 上手号
            p->UpperChannelNo,                 ///< 上手通道号
            p->UpperSettleNo,                  ///< 会员号和清算号
            p->UpperUserNo,                    ///< 上手登录号
            p->OrderInsertUserNo,              ///< 下单人
            p->OrderInsertTime,                ///< 下单时间
            p->OrderCommandUserNo,             ///< 录单操作人
            p->OrderUpdateUserNo,              ///< 委托更新人
            p->OrderUpdateTime,                ///< 委托更新时间
            p->OrderState,                     ///< 委托状态
            p->OrderMatchPrice,                ///< 成交价1
            p->OrderMatchPrice2,               ///< 成交价2
            p->OrderMatchQty,                  ///< 成交量1
            p->OrderMatchQty2,                 ///< 成交量2
            p->ErrorCode,                      ///< 最后一次操作错误信息码
            p->ErrorText,                      ///< 错误信息
            p->IsBackInput,                    ///< 是否为录入委托单
            p->IsDeleted,                      ///< 委托成交删除标
            p->IsAddOne,                       ///< 是否为T+1单
            p->OrderStreamID,                  ///< 委托流水号
            p->UpperStreamID,                  ///< 上手流号
            p->ContractSize,                   ///< 每手乘数，计算参数
            p->ContractSize2,                  ///< 每手乘数，计算参数
            p->CommodityCurrencyGroup,         ///< 品种币种组
            p->CommodityCurrency,              ///< 品种币种
            p->FeeMode,                        ///< 手续费计算方式
            p->FeeParam,                       ///< 手续费参数值 冻结手续费均按照开仓手续费计算
            p->FeeCurrencyGroup,               ///< 客户手续费币种组
            p->FeeCurrency,                    ///< 客户手续费币种
            p->FeeMode2,                       ///< 手续费计算方式
            p->FeeParam2,                      ///< 手续费参数值 冻结手续费均按照开仓手续费计算
            p->FeeCurrencyGroup2,              ///< 客户手续费币种组
            p->FeeCurrency2,                   ///< 客户手续费币种
            p->MarginMode,                     ///< 保证金计算方式
            p->MarginParam,                    ///< 保证金参数值
            p->MarginMode2,                    ///< 保证金计算方式
            p->MarginParam2,                   ///< 保证金参数值
            p->PreSettlePrice,                 ///< 昨结算价  比例方式的市价单和组合订单使用
            p->PreSettlePrice2,                ///< 昨结算价  比例方式的市价单和组合订单使用
            p->OpenVol,                        ///< 预开仓数量 委托数量中的开仓部分
            p->CoverVol,                       ///< 预平仓数量 委托数量中的平仓部分
            p->OpenVol2,                       ///< 预开仓数量 委托数量中的开仓部分
            p->CoverVol2,                      ///< 预平仓数量 委托数量中的平仓部分
            p->FeeValue,                       ///< 冻结手续费
            p->MarginValue,                    ///< 冻结保证金 合并计算的此项为0
            p->TacticsType,                    ///< 策略单类型
            p->TriggerCondition,               ///< 触发条件
            p->TriggerPriceType,               ///< 触发价格类型
            p->AddOneIsValid,                  ///< 是否T+1有效
            p->OrderQty2,                      ///< 委托数量2
            p->HedgeFlag2,                     ///< 投机保值2
            p->MarketLevel                     ///< 市价撮合深度
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIOrderActionRsp <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIOrderInfo* p)
{
    char buf[10240];
    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIOrderInfo\n"
            "\tAccountNo=%s\n"
            "\tExchangeNo=%s\n"
            "\tCommodityType=%d\n"
            "\tCommodityNo=%s\n"
            "\tContractNo=%s\n"
            "\tStrikePrice=%s\n"
            "\tCallOrPutFlag=%02X\n"
            "\tContractNo2=%s\n"
            "\tStrikePrice2=%s\n"
            "\tCallOrPutFlag2=%02X\n"
            "\tOrderType=%02X\n"
            "\tOrderSource=%02X\n"
            "\tTimeInForce=%02X\n"
            "\tExpireTime=%s\n"
            "\tIsRiskOrder=%02X\n"
            "\tOrderSide=%02X\n"
            "\tPositionEffect=%02X\n"
            "\tPositionEffect2=%02X\n"
            "\tInquiryNo=%s\n"
            "\tHedgeFlag=%02X\n"
            "\tOrderPrice=%.4f\n"
            "\tOrderPrice2=%.4f\n"
            "\tStopPrice=%.4f\n"
            "\tOrderQty=%u\n"
            "\tOrderMinQty=%u\n"
            "\tOrderCanceledQty=%u\n"
            "\tMinClipSize=%u\n"
            "\tMaxClipSize=%u\n"
            "\tLicenseNo=%s\n"
            "\tRefInt=%d\n"
            "\tRefString=%s\n"
            "\tParentAccountNo=%s\n"
            "\tServerFlag=%02X\n"
            "\tOrderNo=%s\n"
            "\tClientOrderNo=%s\n"
            "\tOrderLocalNo=%s\n"
            "\tOrderSystemNo=%s\n"
            "\tOrderExchangeSystemNo=%s\n"
            "\tOrderParentNo=%s\n"
            "\tOrderParentSystemNo=%s\n"
            "\tTradeNo=%s\n"
            "\tUpperNo=%s\n"
            "\tUpperChannelNo=%s\n"
            "\tUpperSettleNo=%s\n"
            "\tUpperUserNo=%s\n"
            "\tOrderInsertUserNo=%s\n"
            "\tOrderInsertTime=%s\n"
            "\tOrderCommandUserNo=%s\n"
            "\tOrderUpdateUserNo=%s\n"
            "\tOrderUpdateTime=%s\n"
            "\tOrderState=%02X\n"
            "\tOrderMatchPrice=%.4f\n"
            "\tOrderMatchPrice2=%.4f\n"
            "\tOrderMatchQty=%u\n"
            "\tOrderMatchQty2=%u\n"
            "\tErrorCode=%u\n"
            "\tErrorText=%s\n"
            "\tIsBackInput=%02X\n"
            "\tIsDeleted=%02X\n"
            "\tIsAddOne=%02X\n"
            "\tOrderStreamID=%u\n"
            "\tUpperStreamID=%u\n"
            "\tContractSize=%.6f\n"
            "\tContractSize2=%.6f\n"
            "\tCommodityCurrencyGroup=%s\n"
            "\tCommodityCurrency=%s\n"
            "\tFeeMode=%02X\n"
            "\tFeeParam=%.6f\n"
            "\tFeeCurrencyGroup=%s\n"
            "\tFeeCurrency=%s\n"
            "\tFeeMode2=%02X\n"
            "\tFeeParam2=%.6f\n"
            "\tFeeCurrencyGroup2=%s\n"
            "\tFeeCurrency2=%s\n"
            "\tMarginMode=%02X\n"
            "\tMarginParam=%.6f\n"
            "\tMarginMode2=%02X\n"
            "\tMarginParam2=%.6f\n"
            "\tPreSettlePrice=%.6f\n"
            "\tPreSettlePrice2=%.6f\n"
            "\tOpenVol=%u\n"
            "\tCoverVol=%u\n"
            "\tOpenVol2=%u\n"
            "\tCoverVol2=%u\n"
            "\tFeeValue=%.6f\n"
            "\tMarginValue=%.6f\n"
            "\tTacticsType=%02X\n"
            "\tTriggerCondition=%02X\n"
            "\tTriggerPriceType=%02X\n"
            "\tAddOneIsValid=%02X\n"
            "\tOrderQty2=%u\n"
            "\tHedgeFlag2=%02X\n"
            "\tMarketLevel=%u\n",
            p->AccountNo,                      ///< 客户资金帐号
            p->ExchangeNo,                     ///< 交易所编号
            p->CommodityType,                  ///< 品种类型
            p->CommodityNo,                    ///< 品种编码类型
            p->ContractNo,                     ///< 合约1
            p->StrikePrice,                    ///< 执行价格1
            p->CallOrPutFlag,                  ///< 看张看跌1
            p->ContractNo2,                    ///< 合约2
            p->StrikePrice2,                   ///< 执行价格2
            p->CallOrPutFlag2,                 ///< 看张看跌2
            p->OrderType,                      ///< 委托类型
            p->OrderSource,                    ///< 委托来源
            p->TimeInForce,                    ///< 委托有效类型
            p->ExpireTime,                     ///< 有效日期(GTD情况下使用)
            p->IsRiskOrder,                    ///< 是否风险报单
            p->OrderSide,                      ///< 买入卖出
            p->PositionEffect,                 ///< 开平标志1
            p->PositionEffect2,                ///< 开平标志2
            p->InquiryNo,                      ///< 询价号
            p->HedgeFlag,                      ///< 投机保值
            p->OrderPrice,                     ///< 委托价格1
            p->OrderPrice2,                    ///< 委托价格2，做市商应价使用
            p->StopPrice,                      ///< 触发价格
            p->OrderQty,                       ///< 委托数量
            p->OrderMinQty,                    ///< 最小成交量
            p->OrderCanceledQty,               ///< 撤单数量
            p->MinClipSize,                    ///< 冰山单最小随机量
            p->MaxClipSize,                    ///< 冰山单最大随机量
            p->LicenseNo,                      ///< 软件授权号
            p->RefInt,                         ///< 整型参考值
            p->RefString,                      ///< 字符串参考值
            p->ParentAccountNo,                ///< 上级资金账号
            p->ServerFlag,                     ///< 服务器标识
            p->OrderNo,                        ///< 委托编码
            p->ClientOrderNo,                  ///< 客户端本地委托编号
            p->OrderLocalNo,                   ///< 本地号
            p->OrderSystemNo,                  ///< 系统号
            p->OrderExchangeSystemNo,          ///< 交易所系统号
            p->OrderParentNo,                  ///< 父单号
            p->OrderParentSystemNo,            ///< 父单系统号
            p->TradeNo,                        ///< 交易编码
            p->UpperNo,                        ///< 上手号
            p->UpperChannelNo,                 ///< 上手通道号
            p->UpperSettleNo,                  ///< 会员号和清算号
            p->UpperUserNo,                    ///< 上手登录号
            p->OrderInsertUserNo,              ///< 下单人
            p->OrderInsertTime,                ///< 下单时间
            p->OrderCommandUserNo,             ///< 录单操作人
            p->OrderUpdateUserNo,              ///< 委托更新人
            p->OrderUpdateTime,                ///< 委托更新时间
            p->OrderState,                     ///< 委托状态
            p->OrderMatchPrice,                ///< 成交价1
            p->OrderMatchPrice2,               ///< 成交价2
            p->OrderMatchQty,                  ///< 成交量1
            p->OrderMatchQty2,                 ///< 成交量2
            p->ErrorCode,                      ///< 最后一次操作错误信息码
            p->ErrorText,                      ///< 错误信息
            p->IsBackInput,                    ///< 是否为录入委托单
            p->IsDeleted,                      ///< 委托成交删除标
            p->IsAddOne,                       ///< 是否为T+1单
            p->OrderStreamID,                  ///< 委托流水号
            p->UpperStreamID,                  ///< 上手流号
            p->ContractSize,                   ///< 每手乘数，计算参数
            p->ContractSize2,                  ///< 每手乘数，计算参数
            p->CommodityCurrencyGroup,         ///< 品种币种组
            p->CommodityCurrency,              ///< 品种币种
            p->FeeMode,                        ///< 手续费计算方式
            p->FeeParam,                       ///< 手续费参数值 冻结手续费均按照开仓手续费计算
            p->FeeCurrencyGroup,               ///< 客户手续费币种组
            p->FeeCurrency,                    ///< 客户手续费币种
            p->FeeMode2,                       ///< 手续费计算方式
            p->FeeParam2,                      ///< 手续费参数值 冻结手续费均按照开仓手续费计算
            p->FeeCurrencyGroup2,              ///< 客户手续费币种组
            p->FeeCurrency2,                   ///< 客户手续费币种
            p->MarginMode,                     ///< 保证金计算方式
            p->MarginParam,                    ///< 保证金参数值
            p->MarginMode2,                    ///< 保证金计算方式
            p->MarginParam2,                   ///< 保证金参数值
            p->PreSettlePrice,                 ///< 昨结算价  比例方式的市价单和组合订单使用
            p->PreSettlePrice2,                ///< 昨结算价  比例方式的市价单和组合订单使用
            p->OpenVol,                        ///< 预开仓数量 委托数量中的开仓部分
            p->CoverVol,                       ///< 预平仓数量 委托数量中的平仓部分
            p->OpenVol2,                       ///< 预开仓数量 委托数量中的开仓部分
            p->CoverVol2,                      ///< 预平仓数量 委托数量中的平仓部分
            p->FeeValue,                       ///< 冻结手续费
            p->MarginValue,                    ///< 冻结保证金 合并计算的此项为0
            p->TacticsType,                    ///< 策略单类型
            p->TriggerCondition,               ///< 触发条件
            p->TriggerPriceType,               ///< 触发价格类型
            p->AddOneIsValid,                  ///< 是否T+1有效
            p->OrderQty2,                      ///< 委托数量2
            p->HedgeFlag2,                     ///< 投机保值2
            p->MarketLevel                     ///< 市价撮合深度
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIOrderInfo <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIFillInfo* p)
{
    char buf[10240];
    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIFillInfo\n"
            "\tAccountNo=%s\n"
            "\tParentAccountNo=%s\n"
            "\tExchangeNo=%s\n"
            "\tCommodityType=%02X\n"
            "\tCommodityNo=%s\n"
            "\tContractNo=%s\n"
            "\tStrikePrice=%s\n"
            "\tCallOrPutFlag=%02X\n"
            "\tExchangeNoRef=%s\n"
            "\tCommodityTypeRef=%02X\n"
            "\tCommodityNoRef=%s\n"
            "\tContractNoRef=%s\n"
            "\tStrikePriceRef=%s\n"
            "\tCallOrPutFlagRef=%02X\n"
            "\tOrderExchangeNo=%s\n"
            "\tOrderCommodityType=%02X\n"
            "\tOrderCommodityNo=%s\n"
            "\tOrderType=%02X\n"
            "\tMatchSource=%02X\n"
            "\tTimeInForce=%02X\n"
            "\tExpireTime=%s\n"
            "\tIsRiskOrder=%02X\n"
            "\tMatchSide=%02X\n"
            "\tPositionEffect=%02X\n"
            "\tPositionEffectRef=%02X\n"
            "\tHedgeFlag=%02X\n"
            "\tServerFlag=%02X\n"
            "\tOrderNo=%s\n"
            "\tOrderLocalNo=%s\n"
            "\tMatchNo=%s\n"
            "\tExchangeMatchNo=%s\n"
            "\tMatchDateTime=%s\n"
            "\tUpperMatchDateTime=%s\n"
            "\tUpperNo=%s\n"
            "\tUpperChannelNo=%s\n"
            "\tUpperSettleNo=%s\n"
            "\tUpperUserNo=%s\n"
            "\tTradeNo=%s\n"
            "\tMatchPrice=%.6f\n"
            "\tMatchQty=%u\n"
            "\tIsBackInput=%02X\n"
            "\tIsDeleted=%02X\n"
            "\tIsAddOne=%02X\n"
            "\tMatchStreamID=%u\n"
            "\tUpperStreamID=%u\n"
            "\tOpenCloseMode=%02X\n"
            "\tContractSize=%.6f\n"
            "\tCommodityCurrencyGroup=%s\n"
            "\tCommodityCurrency=%s\n"
            "\tFeeMode=%02X\n"
            "\tFeeParam=%.6f\n"
            "\tFeeCurrencyGroup=%s\n"
            "\tFeeCurrency=%s\n"
            "\tPreSettlePrice=%.6f\n"
            "\tFeeValue=%.6f\n"
            "\tIsManualFee=%02X\n"
            "\tTurnover=%.6f\n"
            "\tPremiumIncome=%.6f\n"
            "\tPremiumPay=%.6f\n"
            "\tOppoMatchNo=%s\n"
            "\tCloseProfit=%.6f\n"
            "\tUnExpProfit=%.6f\n"
            "\tUpperMatchPrice=%.6f\n"
            "\tQuotePrice=%.6f\n"
            "\tClosePL=%.6f\n",
            p->AccountNo,                      ///< 客户资金帐号
            p->ParentAccountNo,                ///< 上级资金账号
            p->ExchangeNo,                     ///< 交易所编号
            p->CommodityType,                  ///< 品种类型
            p->CommodityNo,                    ///< 品种编码类型
            p->ContractNo,                     ///< 合约1
            p->StrikePrice,                    ///< 执行价格
            p->CallOrPutFlag,                  ///< 看张看跌
            p->ExchangeNoRef,                  ///< 交易所编号
            p->CommodityTypeRef,               ///< 品种类型
            p->CommodityNoRef,                 ///< 品种编码类型
            p->ContractNoRef,                  ///< 合约
            p->StrikePriceRef,                 ///< 执行价格
            p->CallOrPutFlagRef,               ///< 看张看跌
            p->OrderExchangeNo,                ///< 交易所编号
            p->OrderCommodityType,             ///< 品种类型
            p->OrderCommodityNo,               ///< 品种编码类型
            p->OrderType,                      ///< 委托类型
            p->MatchSource,                    ///< 委托来源
            p->TimeInForce,                    ///< 委托有效类型
            p->ExpireTime,                     ///< 有效日期(GTD情况下使用)
            p->IsRiskOrder,                    ///< 是否风险报单
            p->MatchSide,                      ///< 买入卖出
            p->PositionEffect,                 ///< 开平标志1
            p->PositionEffectRef,              ///< 开平标志2
            p->HedgeFlag,                      ///< 投机保值
            p->ServerFlag,                     ///< 服务器标识
            p->OrderNo,                        ///< 委托编码
            p->OrderLocalNo,                   ///< 委托本地号
            p->MatchNo,                        ///< 本地成交号
            p->ExchangeMatchNo,                ///< 交易所成交号
            p->MatchDateTime,                  ///< 成交时间
            p->UpperMatchDateTime,             ///< 上手成交时间
            p->UpperNo,                        ///< 上手号
            p->UpperChannelNo,                 ///< 上手通道号
            p->UpperSettleNo,                  ///< 会员号和清算代码
            p->UpperUserNo,                    ///< 上手登录号
            p->TradeNo,                        ///< 交易编码
            p->MatchPrice,                     ///< 成交价
            p->MatchQty,                       ///< 成交量
            p->IsBackInput,                    ///< 是否为录入委托单
            p->IsDeleted,                      ///< 委托成交删除标
            p->IsAddOne,                       ///< 是否为T+1单
            p->MatchStreamID,                  ///< 委托流水号
            p->UpperStreamID,                  ///< 上手流号
            p->OpenCloseMode,                  ///< 开平方式
            p->ContractSize,                   ///< 每手乘数，计算参数
            p->CommodityCurrencyGroup,         ///< 品种币种组
            p->CommodityCurrency,              ///< 品种币种
            p->FeeMode,                        ///< 手续费计算方式
            p->FeeParam,                       ///< 手续费参数值 冻结手续费均按照开仓手续费计算
            p->FeeCurrencyGroup,               ///< 客户手续费币种组
            p->FeeCurrency,                    ///< 客户手续费币种
            p->PreSettlePrice,                 ///< 昨结算价
            p->FeeValue,                       ///< 手续费
            p->IsManualFee,                    ///< 人工客户手续费标记
            p->Turnover,                       ///< 成交金额
            p->PremiumIncome,                  ///< 权利金收取
            p->PremiumPay,                     ///< 权利金支付
            p->OppoMatchNo,                    ///< 对手本地成交号（开仓和平仓对应编号）
            p->CloseProfit,                    ///< 平仓盈亏
            p->UnExpProfit,                    ///< 未到期平盈
            p->UpperMatchPrice,                ///< 上手成交价
            p->QuotePrice,                     ///< 当前行情价
            p->ClosePL                         ///< 逐笔平盈
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIFillInfo <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIPositionInfo* p)
{
    char buf[10240];
    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIPositionInfo\n"
            "\tAccountNo=%s\n"
            "\tParentAccountNo=%s\n"
            "\tExchangeNo=%s\n"
            "\tCommodityType=%02X\n"
            "\tCommodityNo=%s\n"
            "\tContractNo=%s\n"
            "\tStrikePrice=%s\n"
            "\tCallOrPutFlag=%02X\n"
            "\tExchangeNoRef=%s\n"
            "\tCommodityTypeRef=%02X\n"
            "\tCommodityNoRef=%s\n"
            "\tContractNoRef=%s\n"
            "\tStrikePriceRef=%s\n"
            "\tCallOrPutFlagRef=%02X\n"
            "\tOrderExchangeNo=%s\n"
            "\tOrderCommodityType=%02X\n"
            "\tOrderCommodityNo=%s\n"
            "\tOrderType=%02X\n"
            "\tMatchSource=%02X\n"
            "\tTimeInForce=%02X\n"
            "\tExpireTime=%s\n"
            "\tMatchSide=%02X\n"
            "\tHedgeFlag=%02X\n"
            "\tPositionNo=%s\n"
            "\tServerFlag=%02X\n"
            "\tOrderNo=%s\n"
            "\tMatchNo=%s\n"
            "\tExchangeMatchNo=%s\n"
            "\tMatchDate=%s\n"
            "\tMatchTime=%s\n"
            "\tUpperMatchTime=%s\n"
            "\tUpperNo=%s\n"
            "\tUpperSettleNo=%s\n"
            "\tUpperUserNo=%s\n"
            "\tTradeNo=%s\n"
            "\tPositionPrice=%.6f\n"
            "\tPositionQty=%u\n"
            "\tIsBackInput=%02X\n"
            "\tIsAddOne=%02X\n"
            "\tMatchStreamID=%u\n"
            "\tPositionStreamId=%u\n"
            "\tOpenCloseMode=%02X\n"
            "\tContractSize=%.6f\n"
            "\tCommodityCurrencyGroup=%s\n"
            "\tCommodityCurrency=%s\n"
            "\tPreSettlePrice=%.6f\n"
            "\tSettlePrice=%.6f\n"
            "\tTurnover=%.6f\n"
            "\tAccountMarginMode=%02X\n"
            "\tAccountMarginParam=%.6f\n"
            "\tUpperMarginMode=%02X\n"
            "\tUpperMarginParam=%.6f\n"
            "\tAccountInitialMargin=%.6f\n"
            "\tAccountMaintenanceMargin=%.6f\n"
            "\tUpperInitialMargin=%.6f\n"
            "\tUpperMaintenanceMargin=%.6f\n"
            "\tPositionProfit=%.6f\n"
            "\tLMEPositionProfit=%.6f\n"
            "\tOptionMarketValue=%.6f\n"
            "\tMatchCmbNo=%s\n"
            "\tIsHistory=%02X\n"
            "\tFloatingPL=%.6f\n",
            p->AccountNo,                      ///< 客户资金帐号
            p->ParentAccountNo,                ///< 上级资金账号
            p->ExchangeNo,                     ///< 交易所编号
            p->CommodityType,                  ///< 品种类型
            p->CommodityNo,                    ///< 品种编码类型
            p->ContractNo,                     ///< 合约1
            p->StrikePrice,                    ///< 执行价格
            p->CallOrPutFlag,                  ///< 看张看跌
            p->ExchangeNoRef,                  ///< 交易所编号
            p->CommodityTypeRef,               ///< 品种类型
            p->CommodityNoRef,                 ///< 品种编码类型
            p->ContractNoRef,                  ///< 合约
            p->StrikePriceRef,                 ///< 执行价格
            p->CallOrPutFlagRef,               ///< 看张看跌
            p->OrderExchangeNo,                ///< 交易所编号
            p->OrderCommodityType,             ///< 品种类型
            p->OrderCommodityNo,               ///< 品种编码类型
            p->OrderType,                      ///< 委托类型
            p->MatchSource,                    ///< 委托来源
            p->TimeInForce,                    ///< 委托有效类型
            p->ExpireTime,                     ///< 有效日期(GTD情况下使用)
            p->MatchSide,                      ///< 买入卖出
            p->HedgeFlag,                      ///< 投机保值
            p->PositionNo,                     ///< 本地持仓号，服务器编写
            p->ServerFlag,                     ///< 服务器标识
            p->OrderNo,                        ///< 委托编码
            p->MatchNo,                        ///< 本地成交号
            p->ExchangeMatchNo,                ///< 交易所成交号
            p->MatchDate,                      ///< 成交日期
            p->MatchTime,                      ///< 成交时间
            p->UpperMatchTime,                 ///< 上手成交时间
            p->UpperNo,                        ///< 上手号
            p->UpperSettleNo,                  ///< 会员号和清算代码
            p->UpperUserNo,                    ///< 上手登录号
            p->TradeNo,                        ///< 交易编码
            p->PositionPrice,                  ///< 持仓价
            p->PositionQty,                    ///< 持仓量
            p->IsBackInput,                    ///< 是否为录入委托单
            p->IsAddOne,                       ///< 是否为T+1单
            p->MatchStreamID,                  ///< 成交流水号
            p->PositionStreamId,               ///< 持仓流号
            p->OpenCloseMode,                  ///< 开平方式
            p->ContractSize,                   ///< 每手乘数，计算参数
            p->CommodityCurrencyGroup,         ///< 品种币种组
            p->CommodityCurrency,              ///< 品种币种
            p->PreSettlePrice,                 ///< 昨结算价
            p->SettlePrice,                    ///< 当前计算价格
            p->Turnover,                       ///< 持仓金额
            p->AccountMarginMode,              ///< 保证金计算方式
            p->AccountMarginParam,             ///< 保证金参数值
            p->UpperMarginMode,                ///< 保证金计算方式
            p->UpperMarginParam,               ///< 保证金参数值
            p->AccountInitialMargin,           ///< 客户初始保证金
            p->AccountMaintenanceMargin,       ///< 客户维持保证金
            p->UpperInitialMargin,             ///< 上手初始保证金
            p->UpperMaintenanceMargin,         ///< 上手维持保证金
            p->PositionProfit,                 ///< 持仓盈亏
            p->LMEPositionProfit,              ///< LME持仓盈亏
            p->OptionMarketValue,              ///< 期权市值
            p->MatchCmbNo,                     ///< 组合持仓号
            p->IsHistory,                      ///< 是否历史持仓
            p->FloatingPL                      ///< 逐笔浮盈
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIPositionInfo <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPICloseInfo* p)
{
    char buf[10240];
    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPICloseInfo\n"
            "\tAccountNo=%s\n"
            "\tParentAccountNo=%s\n"
            "\tExchangeNo=%s\n"
            "\tCommodityType=%02X\n"
            "\tCommodityNo=%s\n"
            "\tContractNo=%s\n"
            "\tStrikePrice=%s\n"
            "\tCallOrPutFlag=%02X\n"
            "\tOpenOrderExchangeNo=%s\n"
            "\tOpenOrderCommodityType=%02X\n"
            "\tOpenOrderCommodityNo=%s\n"
            "\tCloseOrderExchangeNo=%s\n"
            "\tCloseOrderCommodityType=%02X\n"
            "\tCloseOrderCommodityNo=%s\n"
            "\tOpenMatchSource=%02X\n"
            "\tCloseMatchSource=%02X\n"
            "\tCloseSide=%02X\n"
            "\tCloseQty=%u\n"
            "\tOpenPrice=%.6f\n"
            "\tClosePrice=%.6f\n"
            "\tOpenServerFlag=%02X\n"
            "\tOpenOrderNo=%s\n"
            "\tOpenMatchNo=%s\n"
            "\tOpenExchangeMatchNo=%s\n"
            "\tOpenMatchDateTime=%s\n"
            "\tCloseServerFlag=%02X\n"
            "\tCloseOrderNo=%s\n"
            "\tCloseMatchNo=%s\n"
            "\tCloseExchangeMatchNo=%s\n"
            "\tCloseMatchDateTime=%s\n"
            "\tCloseStreamId=%u\n"
            "\tOpenCloseMode=%02X\n"
            "\tContractSize=%.6f\n"
            "\tCommodityCurrencyGroup=%s\n"
            "\tCommodityCurrency=%s\n"
            "\tPreSettlePrice=%.6f\n"
            "\tPremiumIncome=%.6f\n"
            "\tPremiumPay=%.6f\n"
            "\tCloseProfit=%.6f\n"
            "\tUnExpProfit=%.6f\n"
            "\tClosePL=%.6f\n",
            p->AccountNo,                      ///< 客户资金帐号
            p->ParentAccountNo,                ///< 上级资金账号
            p->ExchangeNo,                     ///< 交易所编号
            p->CommodityType,                  ///< 品种类型
            p->CommodityNo,                    ///< 品种编码类型
            p->ContractNo,                     ///< 合约
            p->StrikePrice,                    ///< 执行价格
            p->CallOrPutFlag,                  ///< 看张看跌
            p->OpenOrderExchangeNo,            ///< 交易所编号
            p->OpenOrderCommodityType,         ///< 品种类型
            p->OpenOrderCommodityNo,           ///< 品种编码类型
            p->CloseOrderExchangeNo,           ///< 交易所编号
            p->CloseOrderCommodityType,        ///< 品种类型
            p->CloseOrderCommodityNo,          ///< 品种编码类型
            p->OpenMatchSource,
            p->CloseMatchSource,
            p->CloseSide,                      ///< 平仓一边的买入卖出
            p->CloseQty,                       ///< 平仓成交量
            p->OpenPrice,                      ///< 开仓成交价
            p->ClosePrice,                     ///< 平仓成交价
            p->OpenServerFlag,                 ///< 服务器标识
            p->OpenOrderNo,                    ///< 委托编码
            p->OpenMatchNo,                    ///< 本地成交号
            p->OpenExchangeMatchNo,            ///< 交易所成交号
            p->OpenMatchDateTime,              ///< 成交时间
            p->CloseServerFlag,                ///< 服务器标识
            p->CloseOrderNo,                   ///< 委托编码
            p->CloseMatchNo,                   ///< 本地成交号
            p->CloseExchangeMatchNo,           ///< 交易所成交号
            p->CloseMatchDateTime,             ///< 成交时间
            p->CloseStreamId,                  ///< 平仓流号
            p->OpenCloseMode,                  ///< 开平方式
            p->ContractSize,                   ///< 每手乘数，计算参数
            p->CommodityCurrencyGroup,         ///< 品种币种组
            p->CommodityCurrency,              ///< 品种币种
            p->PreSettlePrice,                 ///< 昨结算价
            p->PremiumIncome,                  ///< 权利金收取
            p->PremiumPay,                     ///< 权利金支付
            p->CloseProfit,                    ///< 平仓盈亏
            p->UnExpProfit,                    ///< 未到期平盈
            p->ClosePL                         ///< 逐笔平盈
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPICloseInfo <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIPositionProfitNotice* pp)
{
    char buf[10240];
    TapAPIPositionProfit *p = NULL;
    if (pp) p = pp->Data;

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIPositionProfitNotice\n"
            "\tIsLast=%02X\n"
            "\tPositionNo=%s\n"
            "\tPositionStreamId=%u\n"
            "\tPositionProfit=%.6f\n"
            "\tLMEPositionProfit=%.6f\n"
            "\tOptionMarketValue=%.6f\n"
            "\tCalculatePrice=%.6f\n"
            "\tFloatingPL=%.6f\n",
            pp->IsLast,                        ///< 是否最后一包
            p->PositionNo,                     ///< 本地持仓号，服务器编写
            p->PositionStreamId,               ///< 持仓流号
            p->PositionProfit,                 ///< 持仓盈亏
            p->LMEPositionProfit,              ///< LME持仓盈亏
            p->OptionMarketValue,              ///< 期权市值
            p->CalculatePrice,                 ///< 计算价格
            p->FloatingPL                      ///< 逐笔浮盈
            );
    }
    else if (pp)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIPositionProfitNotice\n"
            "\tIsLast=%02X\n"
            "\tsub struct TapAPIPositionProfit <null>",
            pp->IsLast                        ///< 是否最后一包
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIPositionProfitNotice <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIDeepQuoteQryRsp *ppp)
{
    char buf[10240];
    const TapAPIDeepQuoteInfo *p = NULL;
    const TapAPIContract *pp = NULL;
    if (ppp)
    {
        pp = &ppp->Contract;
        p = &ppp->DeepQuote;
    }

    if (pp && p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIDeepQuoteQryRsp\n"
            "\tExchangeNo=%s\n"
            "\tCommodityType=%02X\n"
            "\tCommodityNo=%s\n"
            "\tContractNo1=%s\n"
            "\tStrikePrice1=%s\n"
            "\tCallOrPutFlag1=%02X\n"
            "\tContractNo2=%s\n"
            "\tStrikePrice2=%s\n"
            "\tCallOrPutFlag2=%02X\n"
            "\tSide=%02X\n"
            "\tPrice=%.6f\n"
            "\tQty=%u\n",
            pp->Commodity.ExchangeNo,       ///< 交易所编码
            pp->Commodity.CommodityType,    ///< 品种类型
            pp->Commodity.CommodityNo,      ///< 品种编号
            pp->ContractNo1,                ///< 合约代码1
            pp->StrikePrice1,               ///< 执行价1
            pp->CallOrPutFlag1,             ///< 看涨看跌标示1
            pp->ContractNo2,                ///< 合约代码2
            pp->StrikePrice2,               ///< 执行价2
            pp->CallOrPutFlag2,             ///< 看涨看跌标示2
            p->Side,                        ///< 买卖方向
            p->Price,                       ///< 挂单价格
            p->Qty                          ///< 挂单数量
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIDeepQuoteQryRsp <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIExchangeStateInfo *p)
{
    char buf[10240];

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIExchangeStateInfo\n"
            "\tUpperChannelNo=%s\n"
            "\tExchangeNo=%s\n"
            "\tCommodityType=%02X\n"
            "\tCommodityNo=%s\n"
            "\tExchangeTime=%s\n"
            "\tTradingState=%02X\n",
            p->UpperChannelNo,              ///< 上手通道编号
            p->ExchangeNo,                  ///< 交易所编号
            p->CommodityType,               ///< 品种类型
            p->CommodityNo,                 ///< 品种编号
            p->ExchangeTime,                ///< 交易所时间
            p->TradingState                 ///< 交易所状态
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIExchangeStateInfo <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIExchangeStateInfoNotice *pp)
{
    char buf[10240];
    const TapAPIExchangeStateInfo * p = NULL;
    if (pp) p = &pp->ExchangeStateInfo;

    if (p)
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIExchangeStateInfoNotice\n"
            "\tIsLast=%02X\n"
            "\tUpperChannelNo=%s\n"
            "\tExchangeNo=%s\n"
            "\tCommodityType=%02X\n"
            "\tCommodityNo=%s\n"
            "\tExchangeTime=%s\n"
            "\tTradingState=%02X\n",
            pp->IsLast,                     ///< 是否最后一包
            p->UpperChannelNo,              ///< 上手通道编号
            p->ExchangeNo,                  ///< 交易所编号
            p->CommodityType,               ///< 品种类型
            p->CommodityNo,                 ///< 品种编号
            p->ExchangeTime,                ///< 交易所时间
            p->TradingState                 ///< 交易所状态
            );
    }
    else
    {
        snprintf(buf, sizeof(buf), "structName=TapAPIExchangeStateInfoNotice <null>");
    }

    return buf;
}
