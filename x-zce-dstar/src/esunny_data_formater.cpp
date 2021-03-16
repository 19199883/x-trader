/*
 * esunny_data_formater.cpp
 *
 *  Created on: 2013-8-13
 *      Author: oliver
 */

#include "vrt_value_obj.h"
#include "esunny_data_formater.h"

using namespace std;

std::string ESUNNYDatatypeFormater::ToString(const TapAPITradeLoginAuth* p)
{
    char buf[1024];
    if (p) {
        snprintf(buf, sizeof(buf), "structName=TapAPITradeLoginAuth "
            "UserNo=%s "
            "ISModifyPassword=%c "
            "Password=%s "
            "NewPassword=%s "
            "ISDDA=%c "
            "DDASerialNo=%s "
			"AppID=%s"
			"AuthCode=%s",
            p->UserNo,                   //用户名
            p->ISModifyPassword,         //是否修改密码
            p->Password,                 //密码
            p->NewPassword,              //新密码
            p->ISDDA,                    //是否需要动态认证
            p->DDASerialNo,               //动态认证码
			p->AppID,
			p->AuthCode
            );
    } else {
        snprintf(buf, sizeof(buf), "structName=TapAPITradeLoginAuth <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPICommodity* p)
{
    char buf[1024];
    if (p) {
        snprintf(buf, sizeof(buf), "structName=TapAPICommodity "
            "ExchangeNo=%s "
            "CommodityType=%c "
            "CommodityNo=%s ",
            p->ExchangeNo,                     ///< 交易所编码
            p->CommodityType,                  ///< 品种类型
            p->CommodityNo                     ///< 品种编号
            );
    } else {
        snprintf(buf, sizeof(buf), "structName=TapAPICommodity <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPINewOrder* p)
{
    char buf[1024];
    if (p) {
        snprintf(buf, sizeof(buf), "structName=TapAPINewOrder "
            "AccountNo=%s "
            "ExchangeNo=%s "
            "CommodityType=%c "
            "CommodityNo=%s "
            "ContractNo=%s "
            "StrikePrice=%s "
            "CallOrPutFlag=%c "
            "ContractNo2=%s "
            "StrikePrice2=%s "
            "CallOrPutFlag2=%c "
            "OrderType=%c "
            "OrderSource=%c "
            "TimeInForce=%c "
            "ExpireTime=%s "
            "IsRiskOrder=%c "
            "OrderSide=%c "
            "PositionEffect=%c "
            "PositionEffect2=%c "
            "InquiryNo=%s "
            "HedgeFlag=%c "
            "OrderPrice=%.4f "
            "OrderPrice2=%.4f "
            "StopPrice=%.4f "
            "OrderQty=%u "
            "OrderMinQty=%u "
            "MinClipSize=%u "
            "MaxClipSize=%u "
            "RefInt=%d "
            "RefString=%s "
            "TacticsType=%c "
            "TriggerCondition=%c "
            "TriggerPriceType=%c "
            "AddOneIsValid=%c "
            "OrderQty2=%u "
            "HedgeFlag2=%c "
            "MarketLevel=%u ",
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
    } else {
        snprintf(buf, sizeof(buf), "structName=TapAPINewOrder <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIOrderCancelReq* p)
{
    char buf[1024];
    if (p) {
        snprintf(buf, sizeof(buf), "structName=TapAPIOrderCancelReq "
            "RefInt=%d "
            "RefString=%s "
            "ServerFlag=%c "
            "OrderNo=%s ",
            p->RefInt,                         ///< 整型参考值
            p->RefString,                      ///< 字符串参考值
            p->ServerFlag,                     ///< 服务器标识
            p->OrderNo                         ///< 委托编码
            );
    } else {
        snprintf(buf, sizeof(buf), "structName=TapAPIOrderCancelReq <null>");
    }

    return buf;
}

// ok
std::string ESUNNYDatatypeFormater::ToString(const DstarApiRspLoginField *p)
{
    char buf[1024];
    if (p) 
	{
        snprintf(buf, 
				sizeof(buf), 
				"structName=DstarApiRspLoginField "
				" user:%s "
				"index:%u "
				"error:%u "
				"authcode:%u "
				"TradeDate=%s\n",
				p->AccountNo, 
				p->AccountIndex, 
				p->ErrorCode, 
				p->UdpAuthCode,
				p->TradeDate);
    } 
	else 
	{
        snprintf(buf, sizeof(buf), "structName=TradeDate <null>");
    }

    return buf;
}

// ok
std::string ESUNNYDatatypeFormater::ToString(const DstarApiRspUdpAuthField *p)
{
    char buf[1024];
    if (p) 
	{
        snprintf(buf, 
				sizeof(buf), 
				"structName=DstarApiRspUdpAuthField "
				"AccountIndex:%d "
				"UdpAuthCode:%u "
				"ErrorCode:%u " 
				"ReqIdMode:%hhu\n",
				p->AccountIndex, 
				p->UdpAuthCode, 
				p->ErrorCode,
				p->ReqIdMode);
    } 
	else 
	{
        snprintf(buf, sizeof(buf), "structName=DstarApiRspUdpAuthField <null>");
    }

    return buf;
}


// ok
std::string ESUNNYDatatypeFormater::ToString(const DstarApiContractField *p)
{
    char buf[1024];
    if (p) 
	{
        snprintf(buf, 
				sizeof(buf), 
				"structName=DstarApiContractField "
				"ExchangeId:%c "
				"CommodityType:%c "
				"ContractIndex:%u " 
				"ContractSize:%d"
				"ContractNo:%s \n" ,
				p->ExchangeId, 
				p->CommodityType, 
				p->ContractIndex,
				p->ContractSize,
				p->ContractNo);
    } 
	else 
	{
        snprintf(buf, sizeof(buf), "structName=DstarApiContractField <null>");
    }

    return buf;
}


// ok
std::string ESUNNYDatatypeFormater::ToString(const DstarApiSeatField *p)
{
    char buf[1024];
    if (p) 
	{
        snprintf(buf, 
				sizeof(buf), 
				"structName=DstarApiSeatField "
				"SeatIndex:%hhu "
				"SeatNo:%s "
				"Exchange:%c \n" ,
				p->SeatIndex, 
				p->SeatNo, 
				p->Exchange);
    } 
	else 
	{
        snprintf(buf, sizeof(buf), "structName=DstarApiSeatField <null>");
    }

    return buf;
}
///////////////////////////////

std::string ESUNNYDatatypeFormater::ToString(const TapAPIExchangeInfo* p)
{
    char buf[1024];
    if (p) {
        snprintf(buf, sizeof(buf), "structName=TapAPIExchangeInfo "
            "ExchangeNo=%s "
            "ExchangeName=%s ",
            p->ExchangeNo,                     ///< 交易所编码
            p->ExchangeName                    ///< 交易所名称
            );
    } else {
        snprintf(buf, sizeof(buf), "structName=TapAPIExchangeInfo <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPICommodityInfo* p)
{
    char buf[1024];
    if (p) {
        snprintf(buf, sizeof(buf), "structName=TapAPICommodityInfo "
            "ExchangeNo=%s "
            "CommodityType=%c "
            "CommodityNo=%s "
            "CommodityName=%s "
            "CommodityEngName=%s "
            "RelateExchangeNo=%s "
            "RelateCommodityType=%c "
            "RelateCommodityNo=%s "
            "RelateExchangeNo2=%s "
            "RelateCommodityType2=%c "
            "RelateCommodityNo2=%s "
            "TradeCurrency=%s "
            "SettleCurrency=%s "
            "ContractSize=%.6f "
            "MarginCalculateMode=%c "
            "OptionMarginCalculateMode=%c "
            "OpenCloseMode=%c "
            "StrikePriceTimes=%.6f "
            "IsOnlyQuoteCommodity=%c "
            "CommodityTickSize=%.6f "
            "CommodityDenominator=%d "
            "CmbDirect=%c "
            "OnlyCanCloseDays=%d "
            "DeliveryMode=%c "
            "DeliveryDays=%d "
            "AddOneTime=%s "
            "CommodityTimeZone=%d ",
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
    } else {
        snprintf(buf, sizeof(buf), "structName=TapAPICommodityInfo <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPITradeContractInfo* p)
{
    char buf[1024];
    if (p) {
        snprintf(buf, sizeof(buf), "structName=TapAPITradeContractInfo "
            "ExchangeNo=%s "
            "CommodityType=%c "
            "CommodityNo=%s "
            "ContractNo1=%s "
            "StrikePrice1=%s "
            "CallOrPutFlag1=%c "
            "ContractNo2=%s "
            "StrikePrice2=%s "
            "CallOrPutFlag2=%c "
            "ContractType=%c "
            "QuoteUnderlyingContract=%s "
            "ContractName=%s "
            "ContractExpDate=%s "
            "LastTradeDate=%s "
            "FirstNoticeDate=%s "
            "FutureContractNo=%s ",
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
    } else {
        snprintf(buf, sizeof(buf), "structName=TapAPITradeContractInfo <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIOrderInfoNotice* pp)
{
    char buf[1024];
    TapAPIOrderInfo *p = NULL;
    if (pp) p = pp->OrderInfo;

    if (p) {
        snprintf(buf, sizeof(buf), "structName=TapAPIOrderInfoNotice "
            "SessionID=%u "
            "ErrorCode=%u "
            "CommodityNo=%s "
            "ContractNo=%s "
            "OrderType=%c "
            "OrderSide=%c "
            "PositionEffect=%c "
            "OrderPrice=%.4f "
            "OrderQty=%u "
            "OrderCanceledQty=%u "
            "RefInt=%d "
            "RefString=%s "
            "ServerFlag=%c "
            "OrderNo=%s "
            "OrderInsertTime=%s "
            "OrderUpdateTime=%s "
            "OrderState=%c "
            "OrderMatchPrice=%.4f "
            "OrderMatchQty=%u "
            "ErrorCode=%u "
            "ErrorText=%s ",
            pp->SessionID,                     ///< 会话ID
            pp->ErrorCode,                     ///< 错误码
            p->CommodityNo,                    ///< 品种编码类型
            p->ContractNo,                     ///< 合约1
            p->OrderType,                      ///< 委托类型
            p->OrderSide,                      ///< 买入卖出
            p->PositionEffect,                 ///< 开平标志1
            p->OrderPrice,                     ///< 委托价格1
            p->OrderQty,                       ///< 委托数量
            p->OrderCanceledQty,               ///< 撤单数量
            p->RefInt,                         ///< 整型参考值
            p->RefString,                      ///< 字符串参考值
            p->ServerFlag,                     ///< 服务器标识
            p->OrderNo,                        ///< 委托编码
            p->OrderInsertTime,                ///< 委托更新时间
            p->OrderUpdateTime,                ///< 委托更新时间
            p->OrderState,                     ///< 委托状态
            p->OrderMatchPrice,                ///< 成交价1
            p->OrderMatchQty,                  ///< 成交量1
            p->ErrorCode,                      ///< 最后一次操作错误信息码
            p->ErrorText                      ///< 错误信息
            );
    } else {
        snprintf(buf, sizeof(buf), "structName=TapAPIOrderInfoNotice <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIOrderActionRsp* pp)
{
    char buf[1024];
    TapAPIOrderInfo *p = NULL;
    if (pp) p = pp->OrderInfo;

    if (p) {
        snprintf(buf, sizeof(buf), "structName=TapAPIOrderActionRsp "
            "ActionType=%c "
            "CommodityNo=%s "
            "ContractNo=%s "
            "OrderType=%c "
            "OrderSide=%c "
            "PositionEffect=%c "
            "OrderPrice=%.4f "
            "OrderQty=%u "
            "OrderCanceledQty=%u "
            "RefInt=%d "
            "RefString=%s "
            "ServerFlag=%c "
            "OrderNo=%s "
            "TradeNo=%s "
            "OrderInsertTime=%s "
            "OrderUpdateTime=%s "
            "OrderState=%c "
            "OrderMatchPrice=%.4f "
            "OrderMatchQty=%u "
            "ErrorCode=%u ",
            p->CommodityNo,                    ///< 品种编码类型
            p->ContractNo,                     ///< 合约1
            p->OrderType,                      ///< 委托类型
            p->OrderSide,                      ///< 买入卖出
            p->PositionEffect,                 ///< 开平标志1
            p->OrderPrice,                     ///< 委托价格1
            p->OrderQty,                       ///< 委托数量
            p->OrderCanceledQty,               ///< 撤单数量
            p->RefInt,                         ///< 整型参考值
            p->RefString,                      ///< 字符串参考值
            p->ServerFlag,                     ///< 服务器标识
            p->OrderNo,                        ///< 委托编码
            p->TradeNo,                        ///< 交易编码
            p->OrderInsertTime,                ///< 下单时间
            p->OrderUpdateTime,                ///< 委托更新时间
            p->OrderState,                     ///< 委托状态
            p->OrderMatchPrice,                ///< 成交价1
            p->OrderMatchQty,                  ///< 成交量1
            p->ErrorCode,                      ///< 最后一次操作错误信息码
            p->ErrorText                      ///< 错误信息
            );
    } else {
        snprintf(buf, sizeof(buf), "structName=TapAPIOrderActionRsp <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIOrderInfo* p)
{
    char buf[1024];
    if (p) {
        snprintf(buf, sizeof(buf), "structName=TapAPIOrderInfo "
            "CommodityNo=%s "
            "ContractNo=%s "
            "OrderType=%c "
            "OrderSide=%c "
            "PositionEffect=%c "
            "OrderPrice=%.4f "
            "OrderQty=%u "
            "OrderCanceledQty=%u "
            "RefInt=%d "
            "RefString=%s "
            "ServerFlag=%c "
            "OrderNo=%s "
            "TradeNo=%s "
            "OrderInsertTime=%s "
            "OrderUpdateTime=%s "
            "OrderState=%c "
            "OrderMatchPrice=%.4f "
            "OrderMatchQty=%u "
            "ErrorCode=%u "
            "ErrorText=%s ",
            p->CommodityNo,                    ///< 品种编码类型
            p->ContractNo,                     ///< 合约1
            p->OrderType,                      ///< 委托类型
            p->OrderSide,                      ///< 买入卖出
            p->PositionEffect,                 ///< 开平标志1
            p->OrderPrice,                     ///< 委托价格1
            p->OrderQty,                       ///< 委托数量
            p->OrderCanceledQty,               ///< 撤单数量
            p->RefInt,                         ///< 整型参考值
            p->RefString,                      ///< 字符串参考值
            p->ServerFlag,                     ///< 服务器标识
            p->OrderNo,                        ///< 委托编码
            p->TradeNo,                        ///< 交易编码
            p->OrderInsertTime,                ///< 下单时间
            p->OrderUpdateTime,                ///< 委托更新时间
            p->OrderState,                     ///< 委托状态
            p->OrderMatchPrice,                ///< 成交价1
            p->OrderMatchQty,                  ///< 成交量1
            p->ErrorCode,                      ///< 最后一次操作错误信息码
            p->ErrorText                      ///< 错误信息
            );
    } else {
        snprintf(buf, sizeof(buf), "structName=TapAPIOrderInfo <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIExchangeStateInfo *p)
{
    char buf[1024];

    if (p) {
        snprintf(buf, sizeof(buf), "structName=TapAPIExchangeStateInfo "
            "UpperChannelNo=%s "
            "ExchangeNo=%s "
            "CommodityType=%c "
            "CommodityNo=%s "
            "ExchangeTime=%s "
            "TradingState=%c ",
            p->UpperChannelNo,              ///< 上手通道编号
            p->ExchangeNo,                  ///< 交易所编号
            p->CommodityType,               ///< 品种类型
            p->CommodityNo,                 ///< 品种编号
            p->ExchangeTime,                ///< 交易所时间
            p->TradingState                 ///< 交易所状态
            );
    } else { snprintf(buf, sizeof(buf), "structName=TapAPIExchangeStateInfo <null>");
    }

    return buf;
}

std::string ESUNNYDatatypeFormater::ToString(const TapAPIExchangeStateInfoNotice *pp)
{
    char buf[1024];
    const TapAPIExchangeStateInfo * p = NULL;
    if (pp) p = &pp->ExchangeStateInfo;

    if (p) {
        snprintf(buf, sizeof(buf), "structName=TapAPIExchangeStateInfoNotice "
            "IsLast=%c "
            "UpperChannelNo=%s "
            "ExchangeNo=%s "
            "CommodityType=%c "
            "CommodityNo=%s "
            "ExchangeTime=%s "
            "TradingState=%c ",
            pp->IsLast,                     ///< 是否最后一包
            p->UpperChannelNo,              ///< 上手通道编号
            p->ExchangeNo,                  ///< 交易所编号
            p->CommodityType,               ///< 品种类型
            p->CommodityNo,                 ///< 品种编号
            p->ExchangeTime,                ///< 交易所时间
            p->TradingState                 ///< 交易所状态
            );
    } else {
        snprintf(buf, sizeof(buf), "structName=TapAPIExchangeStateInfoNotice <null>");
    }

    return buf;
}


std::string ESUNNYDatatypeFormater::ToString(const TapAPISubmitUserLoginRspInfo *pp)
{
    clog_warning("[%s] OnRspSubmitUserLoginInfo tostring.", "dd");
	fflush (Log::fp);

    char buf[1024];
    if (pp) {
        snprintf(buf, sizeof(buf), "structName=TapAPISubmitUserLoginRspInfo"
            "UserNo=%s "
            "ErrorCode=%d "
            "ErrorText=%s ",
            pp->UserNo, 
            pp->ErrorCode,
            pp->ErrorText);
    } else {
        snprintf(buf, sizeof(buf), "structName=TapAPISubmitUserLoginRspInfo<null>");
    }

    clog_warning("[%s] OnRspSubmitUserLoginInfo tostring end.", "dd");
	fflush (Log::fp);
    return buf;
}
