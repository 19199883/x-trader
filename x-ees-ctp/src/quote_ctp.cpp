#include "quote_ctp.h"
#include <iomanip>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>

#include "quote_cmn_config.h"
#include "quote_cmn_utility.h"

using namespace my_cmn;
using namespace std;

MYCTPDataHandler::MYCTPDataHandler(const SubscribeContracts *subscribe_contracts, const ConfigData &cfg)
    : cfg_(cfg), shfe_save_(NULL), dce_save_(NULL), cffex_save_(NULL), zce_save_(NULL)
{
    if (subscribe_contracts)
    {
        subscribe_contracts_ = *subscribe_contracts;
    }

    // 初始化
    logoned_ = false;
    api_ = NULL;
    char qtm_name_buf[64];
    sprintf(qtm_name_buf, "ctp_level2_%lu", getpid());
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

    tickprice = MYCTPTradeInterface::securities_tickprice;

    shfe_save_ = new QuoteDataSave<MYShfeMarketData>(cfg_, qtm_name, "shfe_level2_fake_", MY_SHFE_MD_QUOTE_TYPE);
    dce_save_ = new QuoteDataSave<MDBestAndDeep_MY>(cfg_, qtm_name, "dce_level2_fake_", DCE_MDBESTANDDEEP_QUOTE_TYPE);
    cffex_save_ = new QuoteDataSave<CFfexFtdcDepthMarketData>(cfg_, qtm_name, "cffex_level2_fake_", GTA_UDP_CFFEX_QUOTE_TYPE);
    zce_save_ = new QuoteDataSave<ZCEL2QuotSnapshotField_MY>(cfg_, qtm_name, "zce_level2_fake_", CZCE_LEVEL2_QUOTE_TYPE);

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

    if (shfe_save_) delete shfe_save_;
    if (dce_save_) delete dce_save_;
    if (cffex_save_) delete cffex_save_;
    if (zce_save_) delete zce_save_;
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
    QuoteUpdateState(qtm_name.c_str(), QtmState::DISCONNECT);
    MY_LOG_ERROR("CTP - OnFrontDisconnected, nReason: %d", nReason);
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

static inline long long GetNanoSeconds()
{
    // get ns(nano seconds) from Unix Epoch
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000000000 + ts.tv_nsec;
}

//static inline void SetTPinBuff(char *b, long long tp)
//{
//    long long *t = (long long *) (b + 20);
//    *t = tp;
//}

#define VOLUME_CONST 18
#define VOLUME_RATE_CONST 0.1
void MYCTPDataHandler::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *p)
{
    try
    {

	// added by wangying on 20160801
	MY_LOG_DEBUG("CTP - MarketData, InstrumentID: %s, PreSettlementPrice:%.4f, PreClosePrice:%.4f, OpenPrice:%.4f, BidPrice1:%.4f, BidVolume1: %d,BidVolume5: %d, BidPrice5:%.4f", 
		p->InstrumentID,p->PreSettlementPrice,p->PreClosePrice,p->OpenPrice,p->BidPrice1,p->BidVolume1,p->BidVolume5,p->BidPrice5);
	

        long long tp_ns = GetNanoSeconds();

        RalaceInvalidValue_CTP(*p);
        MYShfeMarketData shfe_md = ShfeConvert(*p);
        MDBestAndDeep_MY dce_md = DceConvert(*p);
        CFfexFtdcDepthMarketData cffex_md = CffexConvert(*p);
        ZCEL2QuotSnapshotField_MY zce_md = ZceConvert(*p);

        // TODO take recv time point in md, for measure performance.
//        SetTPinBuff(shfe_md.InstrumentID, tp_ns);
//        SetTPinBuff(dce_md.Contract, tp_ns);
//        SetTPinBuff(cffex_md.szInstrumentID, tp_ns);
//        SetTPinBuff(zce_md.ContractID, tp_ns);

        if (subscribe_contracts_.empty() || subscribe_contracts_.find(p->InstrumentID) != subscribe_contracts_.end())
        {
            if (shfe_handler_) shfe_handler_(&shfe_md);
            if (dce_handler_) dce_handler_(&dce_md);
            if (cffex_handler_) cffex_handler_(&cffex_md);
            if (zce_handler_) zce_handler_(&zce_md);
        }

        // 存起来
        shfe_save_->OnQuoteData(tp_ns / 1000, &shfe_md);
        dce_save_->OnQuoteData(tp_ns / 1000, &dce_md);
        cffex_save_->OnQuoteData(tp_ns / 1000, &cffex_md);
        zce_save_->OnQuoteData(tp_ns / 1000, &zce_md);
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

    d.AskPrice1 = InvalidToZeroD(d.AskPrice1);

    d.SettlementPrice = InvalidToZeroD(d.SettlementPrice);
    d.PreSettlementPrice = InvalidToZeroD(d.PreSettlementPrice);
}

double MYCTPDataHandler::GetTickPrice(const std::string &scode, double b1, double s1)
{
    double tp = s1 - b1;
    if (s1 < 0.000001 || b1 < 0.000001) tp = 10; // no b1 or no s1 (tick price of ni is 10)

    MYSecurityTickPrice::iterator it = tickprice.find(scode);
    if (it == tickprice.end())
    {
        tickprice.insert(std::make_pair(scode, tp));

        MY_LOG_WARN("add tick price of %s, set to %.4f.", scode.c_str(), tp);
        return tp;
    }
    else
    {
        double tp_c = it->second;
        if ((tp_c - 0.000001) > tp)
        {
            it->second = tp;
            tp_c = tp;
            MY_LOG_WARN("update tick price of %s, set to %.4f.", scode.c_str(), tp_c);
        }

        return tp_c;
    }
}

MYShfeMarketData MYCTPDataHandler::ShfeConvert(const CThostFtdcDepthMarketDataField& c)
{
    MYShfeMarketData md;
    bzero(&md, sizeof(md));
    double tp = GetTickPrice(c.InstrumentID, c.BidPrice1, c.AskPrice1);

    memcpy(md.TradingDay, c.TradingDay, 9);
    //SettlementGroupID[9];
    //SettlementID ;
    md.LastPrice = c.LastPrice;
    md.PreSettlementPrice = c.PreSettlementPrice;
    md.PreClosePrice = c.PreClosePrice;
    md.PreOpenInterest = c.PreOpenInterest;
    md.OpenPrice = c.OpenPrice;
    md.HighestPrice = c.HighestPrice;
    md.LowestPrice = c.LowestPrice;
    md.Volume = c.Volume;
    md.Turnover = c.Turnover;
    md.OpenInterest = c.OpenInterest;
    md.ClosePrice = c.ClosePrice;
    md.SettlementPrice = c.SettlementPrice;
    md.UpperLimitPrice = c.UpperLimitPrice;
    md.LowerLimitPrice = c.LowerLimitPrice;
    md.PreDelta = c.PreDelta;
    md.CurrDelta = c.CurrDelta;
    memcpy(md.UpdateTime, c.UpdateTime, 9);
    md.UpdateMillisec = c.UpdateMillisec;
    memcpy(md.InstrumentID, c.InstrumentID, 31);

    md.BidPrice1 = c.BidPrice1;
    md.BidVolume1 = c.BidVolume1;
    md.AskPrice1 = c.AskPrice1;
    md.AskVolume1 = c.AskVolume1;

    if (c.BidPrice1 > 0.000001)
    {
        md.BidPrice2 = c.BidPrice1 - tp;
        md.BidVolume2 = c.BidVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
        md.BidPrice3 = c.BidPrice1 - 2 * tp;
        md.BidVolume3 = c.BidVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
        md.BidPrice4 = c.BidPrice1 - 3 * tp;
        md.BidVolume4 = c.BidVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
        md.BidPrice5 = c.BidPrice1 - 4 * tp;
        md.BidVolume5 = c.BidVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
    }

    if (c.AskPrice1 > 0.000001)
    {
        md.AskPrice2 = c.AskPrice1 + tp;
        md.AskVolume2 = c.AskVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
        md.AskPrice3 = c.AskPrice1 + 2 * tp;
        md.AskVolume3 = c.AskVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
        md.AskPrice4 = c.AskPrice1 + 3 * tp;
        md.AskVolume4 = c.AskVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
        md.AskPrice5 = c.AskPrice1 + 4 * tp;
        md.AskVolume5 = c.AskVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
    }
    //ActionDay[9];

    // 以上为上期交易所的一档数据结构，下面部分为新增的字段
    md.data_flag = 3; // 数据标记，1：市场行情有效；2：委托行情有效；3：市场行情和委托行情都有效

    // 新增的30档委托数据（档数不足时，price、volume 都取值 0）
    md.start_index = 0;  // 数据的起始索引号，0
    md.data_count = 30;   // 委托数据档位数，30
    md.buy_price[0] = c.BidPrice1;
    md.buy_volume[0] = c.BidVolume1;
    md.sell_price[0] = c.AskPrice1;
    md.sell_volume[0] = c.AskVolume1;
    if (c.BidPrice1 > 0.000001)
    {
        for (int i = 1; i < MY_SHFE_QUOTE_PRICE_POS_COUNT; ++i)
        {
            md.buy_price[i] = c.BidPrice1 - i * tp;
            md.buy_volume[i] = c.BidVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
        }
    }
    if (c.AskPrice1 > 0.000001)
    {
        for (int i = 1; i < MY_SHFE_QUOTE_PRICE_POS_COUNT; ++i)
        {
            md.sell_price[i] = c.AskPrice1 + i * tp;
            md.sell_volume[i] = c.AskVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
        }
    }

    return md;
}

MDBestAndDeep_MY MYCTPDataHandler::DceConvert(const CThostFtdcDepthMarketDataField& c)
{
    MDBestAndDeep_MY md;
    bzero(&md, sizeof(md));
    double tp = GetTickPrice(c.InstrumentID, c.BidPrice1, c.AskPrice1);

    strcpy(md.Contract, c.InstrumentID);                   //合约代码
    md.LastClearPrice = c.PreSettlementPrice;                 //昨结算价
    md.ClearPrice = c.LastPrice;                     //今结算价
    md.AvgPrice = c.AveragePrice;                       //成交均价
    md.LastClose = c.PreClosePrice;                      //昨收盘
    md.Close = c.ClosePrice;                          //今收盘
    md.OpenPrice = c.OpenPrice;                      //今开盘
    md.LastOpenInterest = c.PreOpenInterest;               //昨持仓量
    md.OpenInterest = c.OpenInterest;                   //持仓量
    md.LastPrice = c.LastPrice;                      //最新价
    md.MatchTotQty = c.Volume;                    //成交数量
    md.Turnover = c.Turnover;                       //成交金额
    md.RiseLimit = c.UpperLimitPrice;                      //最高报价
    md.FallLimit = c.LowerLimitPrice;                      //最低报价
    md.HighPrice = c.HighestPrice;                      //最高价
    md.LowPrice = c.LowestPrice;                       //最低价

    md.BuyPriceOne = c.BidPrice1;              //买入价格1
    md.BuyQtyOne = c.BidVolume1;                //买入数量1
    if (c.BidPrice1 > 0.000001)
    {
        md.BuyPriceTwo = c.BidPrice1 - tp;
        md.BuyQtyTwo = c.BidVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
        md.BuyPriceThree = c.BidPrice1 - 2 * tp;
        md.BuyQtyThree = c.BidVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
        md.BuyPriceFour = c.BidPrice1 - 3 * tp;
        md.BuyQtyFour = c.BidVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
        md.BuyPriceFive = c.BidPrice1 - 4 * tp;
        md.BuyQtyFive = c.BidVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
    }

    md.SellPriceOne = c.AskPrice1;             //卖出价格1
    md.SellQtyOne = c.AskVolume1;               //买出数量1
    if (c.AskPrice1 > 0.000001)
    {
        md.SellPriceTwo = c.AskPrice1 + tp;
        md.SellQtyTwo = c.AskVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
        md.SellPriceThree = c.AskPrice1 + 2 * tp;
        md.SellQtyThree = c.AskVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
        md.SellPriceFour = c.AskPrice1 + 3 * tp;
        md.SellQtyFour = c.AskVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
        md.SellPriceFive = c.AskPrice1 + 4 * tp;
        md.SellQtyFive = c.AskVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
    }

    sprintf(md.GenTime, "%s.%03d", c.UpdateTime, c.UpdateMillisec);                    //行情产生时间
    strcpy(md.TradeDate, c.TradingDay);                   //行情日期
    strcpy(md.LocalDate, c.TradingDay);

    return md;
}

CFfexFtdcDepthMarketData MYCTPDataHandler::CffexConvert(const CThostFtdcDepthMarketDataField& c)
{
    CFfexFtdcDepthMarketData md;
    bzero(&md, sizeof(md));
    double tp = GetTickPrice(c.InstrumentID, c.BidPrice1, c.AskPrice1);

    memcpy(md.szTradingDay, c.TradingDay, 9);
    md.dLastPrice = c.LastPrice;
    md.dPreSettlementPrice = c.PreSettlementPrice;
    md.dPreClosePrice = c.PreClosePrice;
    md.dPreOpenInterest = c.PreOpenInterest;
    md.dOpenPrice = c.OpenPrice;
    md.dHighestPrice = c.HighestPrice;
    md.dLowestPrice = c.LowestPrice;
    md.nVolume = c.Volume;
    md.dTurnover = c.Turnover;
    md.dOpenInterest = c.OpenInterest;
    md.dClosePrice = c.ClosePrice;
    md.dSettlementPrice = c.SettlementPrice;
    md.dUpperLimitPrice = c.UpperLimitPrice;
    md.dLowerLimitPrice = c.LowerLimitPrice;
    md.dPreDelta = c.PreDelta;
    md.dCurrDelta = c.CurrDelta;
    memcpy(md.szUpdateTime, c.UpdateTime, 9);
    md.nUpdateMillisec = c.UpdateMillisec;
    memcpy(md.szInstrumentID, c.InstrumentID, 31);

    md.dBidPrice1 = c.BidPrice1;
    md.nBidVolume1 = c.BidVolume1;
    md.dAskPrice1 = c.AskPrice1;
    md.nAskVolume1 = c.AskVolume1;

    if (c.BidPrice1 > 0.000001)
    {
        md.dBidPrice2 = c.BidPrice1 - tp;
        md.nBidVolume2 = c.BidVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
        md.dBidPrice3 = c.BidPrice1 - 2 * tp;
        md.nBidVolume3 = c.BidVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
        md.dBidPrice4 = c.BidPrice1 - 3 * tp;
        md.nBidVolume4 = c.BidVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
        md.dBidPrice5 = c.BidPrice1 - 4 * tp;
        md.nBidVolume5 = c.BidVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
    }

    if (c.AskPrice1 > 0.000001)
    {
        md.dAskPrice2 = c.AskPrice1 + tp;
        md.nAskVolume2 = c.AskVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
        md.dAskPrice3 = c.AskPrice1 + 2 * tp;
        md.nAskVolume3 = c.AskVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
        md.dAskPrice4 = c.AskPrice1 + 3 * tp;
        md.nAskVolume4 = c.AskVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
        md.dAskPrice5 = c.AskPrice1 + 4 * tp;
        md.nAskVolume5 = c.AskVolume1 * VOLUME_RATE_CONST + VOLUME_CONST;
    }

    return md;
}

ZCEL2QuotSnapshotField_MY MYCTPDataHandler::ZceConvert(const CThostFtdcDepthMarketDataField& c)
{
    ZCEL2QuotSnapshotField_MY md;
    bzero(&md, sizeof(md));
    double tp = GetTickPrice(c.InstrumentID, c.BidPrice1, c.AskPrice1);

    strcpy(md.ContractID, c.InstrumentID); /*合约编码*/
    md.ContractIDType = '0'; /*合约类型 0->目前应该为0， 扩充：0:期货,1:期权,2:组合*/
    md.PreSettle = c.PreSettlementPrice; /*前结算价格*/
    md.PreClose = c.PreClosePrice; /*前收盘价格*/
    md.PreOpenInterest = c.PreOpenInterest; /*昨日空盘量*/
    md.OpenPrice = c.OpenPrice; /*开盘价*/
    md.HighPrice = c.HighestPrice; /*最高价*/
    md.LowPrice = c.LowestPrice; /*最低价*/
    md.LastPrice = c.LastPrice; /*最新价*/

    md.BidPrice[0] = c.BidPrice1;
    md.BidLot[0] = c.BidVolume1;
    if (c.BidPrice1 > 0.000001)
    {
        for (int i = 1; i < 5; ++i)
        {
            md.BidPrice[i] = c.BidPrice1 - i * tp; /*买入价格 下标从0开始*/
            md.BidLot[i] = c.BidVolume1 * VOLUME_RATE_CONST + VOLUME_CONST; /*买入数量 下标从0开始*/
        }
    }
    md.AskPrice[0] = c.AskPrice1;
    md.AskLot[0] = c.AskVolume1;
    if (c.AskPrice1 > 0.000001)
    {
        for (int i = 1; i < 5; ++i)
        {
            md.AskPrice[i] = c.AskPrice1 + i * tp; /*卖出价 下标从0开始*/
            md.AskLot[i] = c.AskVolume1 * VOLUME_RATE_CONST + VOLUME_CONST; /*卖出数量 下标从0开始*/
        }
    }
    md.TotalVolume = c.Volume; /*总成交量*/
    md.OpenInterest = c.OpenInterest; /*持仓量*/
    md.ClosePrice = c.ClosePrice; /*收盘价*/
    md.SettlePrice = c.SettlementPrice; /*结算价*/
    md.AveragePrice = c.AveragePrice; /*均价*/
    md.LifeHigh = c.HighestPrice; /*历史最高成交价格*/
    md.LifeLow = c.LowestPrice; /*历史最低成交价格*/
    md.HighLimit = c.UpperLimitPrice; /*涨停板*/
    md.LowLimit = c.LowerLimitPrice; /*跌停板*/
    md.TotalBidLot = c.BidVolume1 * 10; /*委买总量*/
    md.TotalAskLot = c.AskVolume1 * 10; /*委卖总量*/

    // md.TimeStamp[24];      //时间：如2014-02-03 13:23:45
    sprintf(md.TimeStamp, "%s-%s-%s %s.%03d",
        std::string(c.TradingDay, 4).c_str(),
        std::string(c.TradingDay + 4, 2).c_str(),
        std::string(c.TradingDay + 6, 2).c_str(),
        c.UpdateTime,
        c.UpdateMillisec);

    return md;
}
