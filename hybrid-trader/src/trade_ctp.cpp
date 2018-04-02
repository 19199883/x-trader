#include "trade_ctp.h"

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <boost/thread.hpp>

#include "quote_cmn_utility.h"

MYCTPTradeInterface::~MYCTPTradeInterface()
{
    if (p_trader_api_)
    {
        p_trader_api_->RegisterSpi(NULL);
        p_trader_api_->Release();
        p_trader_api_ = NULL;
    }
}

void MYCTPTradeInterface::OnFrontConnected()
{
    MY_LOG_INFO("CTP-Trade - OnFrontConnected.");

    // 已经获取合约列表了，无需登录
    if (securities_get_finished)
    {
        return;
    }

    CThostFtdcReqUserLoginField user_logon;
    memset(&user_logon, 0, sizeof(user_logon));
    memcpy(user_logon.BrokerID, broker_id_, strlen(broker_id_));
    memcpy(user_logon.UserID, user_, strlen(user_));
    memcpy(user_logon.Password, password_, strlen(password_));

    // 发出登陆请求
    p_trader_api_->ReqUserLogin(&user_logon, 1);
    MY_LOG_INFO("CTP-Trade - try to login trade server.");
}

void MYCTPTradeInterface::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    // 登陆了，则查查信息
    if (pRspInfo == NULL || pRspInfo->ErrorID == 0)
    {
        MY_LOG_INFO("CTP-Trade - OnRspUserLogin success.");
    }
    else
    {
        MY_LOG_ERROR("CTP-Trade - OnRspUserLogin, ErrorID: %d; ErrorMsg: %s",
            pRspInfo->ErrorID, pRspInfo->ErrorMsg);

        // 登录失败，不断重试
        boost::this_thread::sleep(boost::posix_time::millisec(1000)); // 查询要间隔1s
        CThostFtdcReqUserLoginField user_logon;
        memset(&user_logon, 0, sizeof(user_logon));
        memcpy(user_logon.BrokerID, broker_id_, strlen(broker_id_));
        memcpy(user_logon.UserID, user_, strlen(user_));
        memcpy(user_logon.Password, password_, strlen(password_));

        // 发出登陆请求
        p_trader_api_->ReqUserLogin(&user_logon, 1);
    }

    if (bIsLast)
    {
        // 查询交易编码
        CThostFtdcQryTradingCodeField qry_trading_code;
        memset(&qry_trading_code, 0, sizeof(qry_trading_code));
        memcpy(qry_trading_code.BrokerID, broker_id_, strlen(broker_id_));
        memcpy(qry_trading_code.InvestorID, user_, strlen(user_));
        memcpy(qry_trading_code.ExchangeID, "", strlen(""));
        memcpy(qry_trading_code.ClientID, user_, strlen(user_));
        qry_trading_code.ClientIDType = THOST_FTDC_CIDT_Speculation;

        boost::this_thread::sleep(boost::posix_time::millisec(1000)); // 查询要间隔1s
        int ret = p_trader_api_->ReqQryTradingCode(&qry_trading_code, ++nRequestID);
        MY_LOG_INFO("CTP-Trade - ReqQryTradingCode, return_code: %d", ret);
    }
}

void MYCTPTradeInterface::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo == NULL || pRspInfo->ErrorID == 0)
    {
        MY_LOG_INFO("CTP-Trade - OnRspUserLogout success.");
        securities_get_finished = true;
    }
    else
    {
        MY_LOG_ERROR("CTP-Trade - OnRspUserLogout, ErrorID: %d; ErrorMsg: %s",
            pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
}

void MYCTPTradeInterface::OnRspQryTradingCode(CThostFtdcTradingCodeField *pTradingCode,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    // 成功了
    if (pRspInfo == NULL || pRspInfo->ErrorID == 0)
    {
        if (pTradingCode)
        {
            MY_LOG_INFO("OnRspQryTradingCode, InvestorID: %s; BrokerID: %s; ExchangeID: %s; ClientID: %s; ClientIDType: %c",
                pTradingCode->InvestorID,
                pTradingCode->BrokerID,
                pTradingCode->ExchangeID,
                pTradingCode->ClientID,
                pTradingCode->ClientIDType
                );
        }
        else
        {
            MY_LOG_INFO("OnRspQryTradingCode, response info is null.");
        }
    }
    else
    {
        MY_LOG_ERROR("OnRspQryTradingCode, ErrorID: %d; ErrorMsg: %s",
            pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }

    // 查别的
    if (bIsLast)
    {
        CThostFtdcQryInstrumentMarginRateField instrument_margin_rate;
        memset(&instrument_margin_rate, 0, sizeof(instrument_margin_rate));
        memcpy(instrument_margin_rate.BrokerID, broker_id_, strlen(broker_id_));
        memcpy(instrument_margin_rate.InvestorID, user_, strlen(user_));
        memcpy(instrument_margin_rate.InstrumentID, "", strlen(""));
        instrument_margin_rate.HedgeFlag = THOST_FTDC_CIDT_Speculation;

        boost::this_thread::sleep(boost::posix_time::millisec(1000)); // 查询要间隔1s
        int ret = p_trader_api_->ReqQryInstrumentMarginRate(&instrument_margin_rate, ++nRequestID);
        MY_LOG_INFO("ReqQryInstrumentMarginRate, return_code: %d", ret);
    }
}

void MYCTPTradeInterface::OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    // 成功了
    if (pRspInfo == NULL || pRspInfo->ErrorID == 0)
    {
        if (pInstrumentMarginRate)
        {
            MY_LOG_INFO("OnRspQryInstrumentMarginRate, InstrumentID: %s; "
                "InvestorRange: %c; BrokerID: %s; InvestorID: %s; HedgeFlag: %c; "
                "LongMarginRatioByMoney: %.4f; LongMarginRatioByVolume: %.4f; "
                "ShortMarginRatioByMoney: %.4f; ShortMarginRatioByVolume: %.4f; IsRelative: %d",
                pInstrumentMarginRate->InstrumentID,
                pInstrumentMarginRate->InvestorRange,
                pInstrumentMarginRate->BrokerID,
                pInstrumentMarginRate->InvestorID,
                pInstrumentMarginRate->HedgeFlag,
                pInstrumentMarginRate->LongMarginRatioByMoney,
                pInstrumentMarginRate->LongMarginRatioByVolume,
                pInstrumentMarginRate->ShortMarginRatioByMoney,
                pInstrumentMarginRate->ShortMarginRatioByVolume,
                pInstrumentMarginRate->IsRelative
                );
        }
        else
        {
            MY_LOG_INFO("OnRspQryInstrumentMarginRate, response info is null.");
        }
    }
    else
    {
        MY_LOG_ERROR("OnRspQryInstrumentMarginRate, ErrorID: %d; ErrorMsg: %s",
            pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }

    // 查别的
    if (bIsLast)
    {
        CThostFtdcQryInstrumentCommissionRateField instrument_commission_rate;
        memset(&instrument_commission_rate, 0, sizeof(instrument_commission_rate));
        memcpy(instrument_commission_rate.BrokerID, broker_id_, strlen(broker_id_));
        memcpy(instrument_commission_rate.InvestorID, user_, strlen(user_));
        memcpy(instrument_commission_rate.InstrumentID, "", strlen(""));

        boost::this_thread::sleep(boost::posix_time::millisec(1000)); // 查询要间隔1s
        int ret = p_trader_api_->ReqQryInstrumentCommissionRate(&instrument_commission_rate, ++nRequestID);
        MY_LOG_INFO("ReqQryInstrumentCommissionRate, return_code: %d", ret);
    }
}

void MYCTPTradeInterface::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    // 成功了
    if (pRspInfo == NULL || pRspInfo->ErrorID == 0)
    {
        if (pInstrumentCommissionRate)
        {
            MY_LOG_INFO("OnRspQryInstrumentCommissionRate, InstrumentID: %s"
                "; InvestorRange: %c"
                "; BrokerID: %s"
                "; InvestorID: %s"
                "; OpenRatioByMoney: %.4f"
                "; OpenRatioByVolume: %.4f"
                "; CloseRatioByMoney: %.4f"
                "; CloseRatioByVolume: %.4f"
                "; CloseTodayRatioByMoney: %.4f"
                "; CloseTodayRatioByVolume: %.4f",
                pInstrumentCommissionRate->InstrumentID,
                pInstrumentCommissionRate->InvestorRange,
                pInstrumentCommissionRate->BrokerID,
                pInstrumentCommissionRate->InvestorID,
                pInstrumentCommissionRate->OpenRatioByMoney,
                pInstrumentCommissionRate->OpenRatioByVolume,
                pInstrumentCommissionRate->CloseRatioByMoney,
                pInstrumentCommissionRate->CloseRatioByVolume,
                pInstrumentCommissionRate->CloseTodayRatioByMoney,
                pInstrumentCommissionRate->CloseTodayRatioByVolume
                );
        }
        else
        {
            MY_LOG_ERROR("OnRspQryInstrumentCommissionRate, response info is null.");
        }
    }
    else
    {
        MY_LOG_ERROR("OnRspQryInstrumentCommissionRate, ErrorID: %d; ErrorMsg: %s",
            pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }

    // 查别的
    if (bIsLast)
    {
        CThostFtdcQryInstrumentField instrument_struct;
        memset(&instrument_struct, 0, sizeof(instrument_struct));
        memcpy(instrument_struct.InstrumentID, "", strlen(""));
        memcpy(instrument_struct.ExchangeID, "", strlen(""));
        memcpy(instrument_struct.ExchangeInstID, "", strlen(""));
        memcpy(instrument_struct.ProductID, "", strlen(""));

        boost::this_thread::sleep(boost::posix_time::millisec(1000)); // 查询要间隔1s
        int ret = p_trader_api_->ReqQryInstrument(&instrument_struct, ++nRequestID);
        MY_LOG_INFO("ReqQryInstrument, return_code: %d", ret);
    }
}

void MYCTPTradeInterface::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    // 成功了
    if (pRspInfo == NULL || pRspInfo->ErrorID == 0)
    {
        if (pInstrument)
        {
            MY_LOG_INFO("OnRspQryInstrument, InstrumentID: %s"
                "; ExchangeID: %s"
                "; InstrumentName: %s"
                "; ExchangeInstID: %s"
                "; ProductID: %s"
                "; ProductClass: %c"
                "; DeliveryYear: %08d"
                "; DeliveryMonth: %02d"
                "; MaxMarketOrderVolume: %d"
                "; MinMarketOrderVolume: %d"
                "; MaxLimitOrderVolume: %d"
                "; MinLimitOrderVolume: %d"
                "; VolumeMultiple: %d"
                "; PriceTick: %0.4f"
                "; CreateDate: %s"
                "; OpenDate: %s"
                "; ExpireDate: %s"
                "; StartDelivDate: %s"
                "; EndDelivDate: %s"
                "; InstLifePhase: %c"
                "; IsTrading: %d"
                "; PositionType: %c"
                "; PositionDateType: %c"
                "; LongMarginRatio: %0.4f"
                "; ShortMarginRatio: %.4f",
                pInstrument->InstrumentID,
                pInstrument->ExchangeID,
                pInstrument->InstrumentName,
                pInstrument->ExchangeInstID,
                pInstrument->ProductID,
                pInstrument->ProductClass,
                pInstrument->DeliveryYear,
                pInstrument->DeliveryMonth,
                pInstrument->MaxMarketOrderVolume,
                pInstrument->MinMarketOrderVolume,
                pInstrument->MaxLimitOrderVolume,
                pInstrument->MinLimitOrderVolume,
                pInstrument->VolumeMultiple,
                pInstrument->PriceTick,
                pInstrument->CreateDate,
                pInstrument->OpenDate,
                pInstrument->ExpireDate,
                pInstrument->StartDelivDate,
                pInstrument->EndDelivDate,
                pInstrument->InstLifePhase,
                pInstrument->IsTrading,
                pInstrument->PositionType,
                pInstrument->PositionDateType,
                InvalidToZeroD(pInstrument->LongMarginRatio),
                InvalidToZeroD(pInstrument->ShortMarginRatio)
                );
            securities_exchcode.insert(std::make_pair(pInstrument->InstrumentID,
                ExchNameToExchCode(pInstrument->ExchangeID)));
            securities_volumn_multiple.insert(std::make_pair(pInstrument->InstrumentID,
                pInstrument->VolumeMultiple));
        }
        else
        {
            MY_LOG_ERROR("OnRspQryInstrument, response info is null.");
        }
    }
    else
    {
        MY_LOG_ERROR("OnRspQryInstrument, ErrorID: %d; ErrorMsg: %s",
            pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }

    // 查别的
    if (bIsLast)
    {
        MY_LOG_INFO("OnRspQryInstrument, task finished.");
        CThostFtdcQryBrokerTradingParamsField broker_param;
        memset(&broker_param, 0, sizeof(broker_param));
        memcpy(broker_param.BrokerID, broker_id_, strlen(broker_id_));
        memcpy(broker_param.InvestorID, user_, strlen(user_));

        boost::this_thread::sleep(boost::posix_time::millisec(1000)); // 查询要间隔1s
        int ret = p_trader_api_->ReqQryBrokerTradingParams(&broker_param, ++nRequestID);
        MY_LOG_INFO("ReqQryBrokerTradingParams, return_code: %d", ret);
    }
}

void MYCTPTradeInterface::OnRspQryBrokerTradingParams(CThostFtdcBrokerTradingParamsField *pBrokerTradingParams,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    // 成功了
    if (pRspInfo == NULL || pRspInfo->ErrorID == 0)
    {
        if (pBrokerTradingParams)
        {
            MY_LOG_INFO("OnRspQryBrokerTradingParams, BrokerID: %s"
                "; InvestorID: %s"
                "; MarginPriceType: %c"
                "; Algorithm: %c"
                "; AvailIncludeCloseProfit: %c",
                pBrokerTradingParams->BrokerID,
                pBrokerTradingParams->InvestorID,
                pBrokerTradingParams->MarginPriceType,
                pBrokerTradingParams->Algorithm,
                pBrokerTradingParams->AvailIncludeCloseProfit
                );
        }
        else
        {
            MY_LOG_ERROR("OnRspQryBrokerTradingParams, response info is null.");
        }
    }
    else
    {
        MY_LOG_ERROR("OnRspQryBrokerTradingParams, ErrorID: %d; ErrorMsg: %s",
            pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }

    // 处理完毕，登出，节约交易连接数
    CThostFtdcUserLogoutField user_logout;
    memset(&user_logout, 0, sizeof(user_logout));
    memcpy(user_logout.BrokerID, broker_id_, strlen(broker_id_));
    memcpy(user_logout.UserID, user_, strlen(user_));

    // 发出登出请求
    boost::this_thread::sleep(boost::posix_time::millisec(1000)); // 间隔1s
    int ret = p_trader_api_->ReqUserLogout(&user_logout, ++nRequestID);
    MY_LOG_INFO("ReqUserLogout, return_code: %d", ret);
}

MYCTPTradeInterface::MYCTPTradeInterface(const ConfigData &cfg)
{
    const LogonConfig &logon_info = cfg.Logon_config();

    front_addr_ = new char[logon_info.trade_server_addr.size() + 1];
    memcpy(front_addr_, logon_info.trade_server_addr.data(), logon_info.trade_server_addr.size() + 1);

    broker_id_ = new char[logon_info.broker_id.size() + 1];
    memcpy(broker_id_, logon_info.broker_id.data(), logon_info.broker_id.size() + 1);

    user_ = new char[logon_info.account.size() + 1];
    memcpy(user_, logon_info.account.data(), logon_info.account.size() + 1);

    password_ = new char[logon_info.password.size() + 1];
    memcpy(password_, logon_info.password.data(), logon_info.password.size() + 1);

    nRequestID = 1;

    p_trader_api_ = CThostFtdcTraderApi::CreateFtdcTraderApi();
    p_trader_api_->RegisterSpi(this);

    p_trader_api_->RegisterFront(front_addr_);
    p_trader_api_->SubscribePrivateTopic(THOST_TERT_QUICK);
    p_trader_api_->Init();
}

MYSecurityVolumnMultiple MYCTPTradeInterface::securities_volumn_multiple;

MYSecurityCollection MYCTPTradeInterface::securities_exchcode;

volatile bool MYCTPTradeInterface::securities_get_finished = false;

