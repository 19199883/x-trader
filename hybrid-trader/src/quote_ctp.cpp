#include "quote_ctp.h"
#include <iomanip>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>

#include "quote_cmn_config.h"
#include "quote_cmn_utility.h"

#include "trade_ctp.h"

using namespace my_cmn;
using namespace std;

MYCTPDataHandler::MYCTPDataHandler(const SubscribeContracts *subscribe_contracts, const ConfigData &cfg)
    : cfg_(cfg), p_save_(NULL)
{
    if (subscribe_contracts)
    {
        subscribe_contracts_ = *subscribe_contracts;
    }

    // 初始化
    logoned_ = false;
    api_ = NULL;
    char qtm_name_buf[64];
    sprintf(qtm_name_buf, "ctp_level1_%lu", getpid());
    qtm_name = qtm_name_buf;
    QuoteUpdateState(qtm_name.c_str(), QtmState::INIT);

    // get information of contracts
    if (!MYCTPTradeInterface::securities_get_finished)
    {
        MYCTPTradeInterface *trade_inf = new MYCTPTradeInterface(cfg_);
        // 等待交易接口获取到合约列表
        while (!MYCTPTradeInterface::securities_get_finished)
        {
            boost::this_thread::sleep(boost::posix_time::millisec(50));
        }
        boost::this_thread::sleep(boost::posix_time::millisec(1));
        delete trade_inf;
    }

    p_save_ = new QuoteDataSave<CDepthMarketDataField>(cfg_, qtm_name, "quote_level1", SHFE_EX_QUOTE_TYPE);
    const SubsribeDatas &code_list = cfg_.Subscribe_datas();
    const LogonConfig &logon_cfg = cfg_.Logon_config();

    pp_instruments_ = NULL;
    sub_count_ = 0;
    if (code_list.empty() ||
        (code_list.size() == 1 && *(code_list.begin()) == "All"))
    {
        // 订阅全部
        sub_count_ = MYCTPTradeInterface::securities_exchcode.size();
        pp_instruments_ = new char *[sub_count_];
        int i = 0;
        for (const MYSecurityCollection::value_type &value : MYCTPTradeInterface::securities_exchcode)
        {
            instruments_.append(value.first + "|");
            pp_instruments_[i] = new char[value.first.length() + 1];
            memcpy(pp_instruments_[i], value.first.c_str(), value.first.length() + 1);
            ++i;
        }
    }
    else
    {
        // 解析订阅列表
        sub_count_ = code_list.size();
        pp_instruments_ = new char *[code_list.size()];
        int i = 0;
        for (const std::string &value : code_list)
        {
            instruments_.append(value + "|");
            pp_instruments_[i] = new char[value.length() + 1];
            memcpy(pp_instruments_[i], value.c_str(), value.length() + 1);
            ++i;
        }
    }
    if (!instruments_.empty())
    {
        instruments_.pop_back();
    }

    // 初始化
    api_ = CThostFtdcMdApi::CreateFtdcMdApi();
    api_->RegisterSpi(this);

    // set front address
    for (const std::string &v : logon_cfg.quote_provider_addrs)
    {
        char *addr_tmp = new char[v.size() + 1];
        memcpy(addr_tmp, v.c_str(), v.size() + 1);
        api_->RegisterFront(addr_tmp);
        MY_LOG_INFO("CTP - RegisterFront, addr: %s", addr_tmp);
        delete[] addr_tmp;
    }
    QuoteUpdateState(qtm_name.c_str(), QtmState::SET_ADDRADRESS_SUCCESS);

    api_->Init();
}

MYCTPDataHandler::~MYCTPDataHandler(void)
{
    QuoteUpdateState(qtm_name.c_str(), QtmState::LOG_OUT);

    if (api_)
    {
        api_->RegisterSpi(NULL);
        api_->Release();
        api_ = NULL;
    }

    if (p_save_)
        delete p_save_;
}

void MYCTPDataHandler::OnFrontConnected()
{
    MY_LOG_INFO("shfe_ex(CTP): OnFrontConnected");
    QuoteUpdateState(qtm_name.c_str(), QtmState::CONNECT_SUCCESS);

    CThostFtdcReqUserLoginField req_login;
    memset(&req_login, 0, sizeof(CThostFtdcReqUserLoginField));
    api_->ReqUserLogin(&req_login, 0);

    MY_LOG_INFO("CTP - request logon");
}

void MYCTPDataHandler::OnFrontDisconnected(int nReason)
{
    logoned_ = false;
    MY_LOG_ERROR("CTP - OnFrontDisconnected, nReason: %d", nReason);
    QuoteUpdateState(qtm_name.c_str(), QtmState::DISCONNECT);
}

void MYCTPDataHandler::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID,
    bool bIsLast)
{
    int error_code = pRspInfo ? pRspInfo->ErrorID : 0;
    MY_LOG_INFO("CTP - OnRspUserLogin, error code: %d", error_code);

    if (error_code == 0)
    {
        logoned_ = true;
        api_->SubscribeMarketData(pp_instruments_, sub_count_);
        MY_LOG_INFO("CTP - SubMarketData, codelist: %s", instruments_.c_str());
        QuoteUpdateState(qtm_name.c_str(), QtmState::LOG_ON_SUCCESS);
    }
    else
    {
        std::string err_str("null");
        if (pRspInfo && pRspInfo->ErrorMsg[0] != '\0')
        {
            err_str = pRspInfo->ErrorMsg;
        }
        MY_LOG_WARN("CTP - Logon fail, error code: %d; error info: %s", error_code, err_str.c_str());

        // 登录失败
        QuoteUpdateState(qtm_name.c_str(), QtmState::LOG_ON_FAIL);
    }
}

void MYCTPDataHandler::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo,
    int nRequestID, bool bIsLast)
{
    MY_LOG_DEBUG("CTP - OnRspSubMarketData, code: %s", pSpecificInstrument->InstrumentID);
    if (bIsLast)
    {
        QuoteUpdateState(qtm_name.c_str(), QtmState::API_READY);
    }
}

void MYCTPDataHandler::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    MY_LOG_DEBUG("CTP - OnRspUnSubMarketData, code: %s", pSpecificInstrument->InstrumentID);
}

void MYCTPDataHandler::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *p)
{
    try
    {
        timeval t;
        gettimeofday(&t, NULL);

        RalaceInvalidValue_CTP(*p);
        CDepthMarketDataField q_level1 = Convert(*p);

        if (quote_data_handler_
            && (subscribe_contracts_.empty() || subscribe_contracts_.find(p->InstrumentID) != subscribe_contracts_.end()))
        {
            quote_data_handler_(&q_level1);
        }

        // 存起来
        p_save_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, &q_level1);
    }
    catch (...)
    {
        MY_LOG_FATAL("CTP - Unknown exception in OnRtnDepthMarketData.");
    }
}

void MYCTPDataHandler::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    int error_code = pRspInfo ? 0 : pRspInfo->ErrorID;
    if (error_code != 0)
    {
        MY_LOG_INFO("CTP - OnRspError, code: %d; info: %s", error_code, pRspInfo->ErrorMsg);
    }
}

void MYCTPDataHandler::SetQuoteDataHandler(boost::function<void(const CDepthMarketDataField *)> quote_data_handler)
{
    quote_data_handler_ = quote_data_handler;
}

void MYCTPDataHandler::RalaceInvalidValue_CTP(CThostFtdcDepthMarketDataField &d)
{
    d.Turnover = InvalidToZeroD(d.Turnover);
    d.LastPrice = InvalidToZeroD(d.LastPrice);
    d.UpperLimitPrice = InvalidToZeroD(d.UpperLimitPrice);
    d.LowerLimitPrice = InvalidToZeroD(d.LowerLimitPrice);
    d.HighestPrice = InvalidToZeroD(d.HighestPrice);
    d.LowestPrice = InvalidToZeroD(d.LowestPrice);
    d.OpenPrice = InvalidToZeroD(d.OpenPrice);
    d.ClosePrice = InvalidToZeroD(d.ClosePrice);
    d.PreClosePrice = InvalidToZeroD(d.PreClosePrice);
    d.OpenInterest = InvalidToZeroD(d.OpenInterest);
    d.PreOpenInterest = InvalidToZeroD(d.PreOpenInterest);
    d.AveragePrice = InvalidToZeroD(d.AveragePrice);

    d.BidPrice1 = InvalidToZeroD(d.BidPrice1);
//    d.BidPrice2 = InvalidToZeroD(d.BidPrice2);
//    d.BidPrice3 = InvalidToZeroD(d.BidPrice3);
//    d.BidPrice4 = InvalidToZeroD(d.BidPrice4);
//    d.BidPrice5 = InvalidToZeroD(d.BidPrice5);

    d.AskPrice1 = InvalidToZeroD(d.AskPrice1);
//    d.AskPrice2 = InvalidToZeroD(d.AskPrice2);
//    d.AskPrice3 = InvalidToZeroD(d.AskPrice3);
//    d.AskPrice4 = InvalidToZeroD(d.AskPrice4);
//    d.AskPrice5 = InvalidToZeroD(d.AskPrice5);

    d.SettlementPrice = InvalidToZeroD(d.SettlementPrice);
    d.PreSettlementPrice = InvalidToZeroD(d.PreSettlementPrice);

//    d.PreDelta = InvalidToZeroD(d.PreDelta);
//    d.CurrDelta = InvalidToZeroD(d.CurrDelta);
}

CDepthMarketDataField MYCTPDataHandler::Convert(const CThostFtdcDepthMarketDataField &ctp_data)
{
    CDepthMarketDataField quote_level1;
    memset(&quote_level1, 0, sizeof(CDepthMarketDataField));

    memcpy(quote_level1.TradingDay, ctp_data.TradingDay, 9); /// char       TradingDay[9];
    //SettlementGroupID[9];       /// char       SettlementGroupID[9];
    //SettlementID ;        /// int            SettlementID;
    quote_level1.LastPrice = ctp_data.LastPrice;           /// double LastPrice;
    quote_level1.PreSettlementPrice = ctp_data.PreSettlementPrice;  /// double PreSettlementPrice;
    quote_level1.PreClosePrice = ctp_data.PreClosePrice;       /// double PreClosePrice;
    quote_level1.PreOpenInterest = ctp_data.PreOpenInterest;     /// double PreOpenInterest;
    quote_level1.OpenPrice = ctp_data.OpenPrice;           /// double OpenPrice;
    quote_level1.HighestPrice = ctp_data.HighestPrice;        /// double HighestPrice;
    quote_level1.LowestPrice = ctp_data.LowestPrice;         /// double LowestPrice;
    quote_level1.Volume = ctp_data.Volume;              /// int            Volume;
    quote_level1.Turnover = ctp_data.Turnover;            /// double Turnover;
    quote_level1.OpenInterest = ctp_data.OpenInterest;        /// double OpenInterest;
    quote_level1.ClosePrice = ctp_data.ClosePrice;          /// double ClosePrice;
    quote_level1.SettlementPrice = ctp_data.SettlementPrice;     /// double SettlementPrice;
    quote_level1.UpperLimitPrice = ctp_data.UpperLimitPrice;     /// double UpperLimitPrice;
    quote_level1.LowerLimitPrice = ctp_data.LowerLimitPrice;     /// double LowerLimitPrice;
//    quote_level1.PreDelta = ctp_data.PreDelta;            /// double PreDelta;
//    quote_level1.CurrDelta = ctp_data.CurrDelta;           /// double CurrDelta;
    memcpy(quote_level1.UpdateTime, ctp_data.UpdateTime, 9);       /// char       UpdateTime[9]; typedef char TThostFtdcTimeType[9];
    quote_level1.UpdateMillisec = ctp_data.UpdateMillisec;      /// int            UpdateMillisec;
    memcpy(quote_level1.InstrumentID, ctp_data.InstrumentID, 31); /// char       InstrumentID[31]; typedef char TThostFtdcInstrumentIDType[31];
    quote_level1.BidPrice1 = ctp_data.BidPrice1;           /// double BidPrice1;
    quote_level1.BidVolume1 = ctp_data.BidVolume1;          /// int            BidVolume1;
    quote_level1.AskPrice1 = ctp_data.AskPrice1;           /// double AskPrice1;
    quote_level1.AskVolume1 = ctp_data.AskVolume1;          /// int            AskVolume1;
//    quote_level1.BidPrice2 = ctp_data.BidPrice2;           /// double BidPrice2;
//    quote_level1.BidVolume2 = ctp_data.BidVolume2;          /// int            BidVolume2;
//    quote_level1.AskPrice2 = ctp_data.AskPrice2;           /// double AskPrice2;
//    quote_level1.AskVolume2 = ctp_data.AskVolume2;          /// int            AskVolume2;
//    quote_level1.BidPrice3 = ctp_data.BidPrice3;           /// double BidPrice3;
//    quote_level1.BidVolume3 = ctp_data.BidVolume3;          /// int            BidVolume3;
//    quote_level1.AskPrice3 = ctp_data.AskPrice3;           /// double AskPrice3;
//    quote_level1.AskVolume3 = ctp_data.AskVolume3;          /// int            AskVolume3;
//    quote_level1.BidPrice4 = ctp_data.BidPrice4;           /// double BidPrice4;
//    quote_level1.BidVolume4 = ctp_data.BidVolume4;          /// int            BidVolume4;
//    quote_level1.AskPrice4 = ctp_data.AskPrice4;           /// double AskPrice4;
//    quote_level1.AskVolume4 = ctp_data.AskVolume4;          /// int            AskVolume4;
//    quote_level1.BidPrice5 = ctp_data.BidPrice5;           /// double BidPrice5;
//    quote_level1.BidVolume5 = ctp_data.BidVolume5;          /// int            BidVolume5;
//    quote_level1.AskPrice5 = ctp_data.AskPrice5;           /// double AskPrice5;
//    quote_level1.AskVolume5 = ctp_data.AskVolume5;          /// int            AskVolume5;
        //ActionDay[9];        /// char       ActionDay[9];

    return quote_level1;
}
