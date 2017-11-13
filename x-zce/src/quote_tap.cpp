#include "quote_tap.h"

using namespace my_cmn;
using namespace std;

void MYTAPDataHandler::req_login(int wait_seconds)
{
	/* 登录服务器 */
	struct TapAPIQuoteLoginAuth login_ ;
	strncpy(login_.UserNo, cfg_.Logon_config().account.c_str(), sizeof(login_.UserNo) - 1);
	strncpy(login_.Password, cfg_.Logon_config().password.c_str(), sizeof(login_.Password) - 1);
	login_.ISModifyPassword = 'N';     /* 不修改密码 */
	login_.ISDDA = 'N';                             /* 不需要动态验证 */

	ret = api_->Login(&login_);
}

MYTAPDataHandler::~MYTAPDataHandler(void)
{
    if (api_){
        /* 退出行情登录 */
        MY_LOG_INFO("TAP - cancel subscribe...");
        api_->Disconnect();
    }

    if (sID) { delete sID; }
}

bool MYTAPDataHandler::ParseConfig()
{
    subscribe_contracts_ = cfg_.Subscribe_datas();
}

void MYTAPDataHandler::OnRspLogin(TAPIINT32 errorCode, const TapAPIQuotLoginRspInfo *info)
{
    MY_LOG_INFO("TAP - OnRspLogin");
    if (0 == errorCode){
    } else {
        MY_LOG_WARN("TAP - login failed, ErrorCode = %d", errorCode);
    }
}

// TODO: subscribe to the contracts from the file
// 交易所(ZCE)，品种类型硬编码(F)
// 品种，日期从合约读取,e.g. SR
// contract：801
void MYTAPDataHandler::OnAPIReady()
{
    MY_LOG_INFO("TAP - OnAPIReady");

    for (SubscribeContracts::iterator it = subscribe_contracts_.begin(); it != subscribe_contracts_.end(); it++) {
		std::string exchange_no_;
		char commodity_type_;
		std::string commodity_no_;
		std::string contract_no_;

        int split_pos1 = it->find('.');
        int split_pos2 = it->substr(split_pos1 + 1).find('.');
        int split_pos3 = it->substr(split_pos1 + 1).substr(split_pos2 + 1).find('.');

        exchange_no_ = it->substr(0, split_pos1);
        commodity_type_ = (*it)[split_pos1 + 1];
        commodity_no_ = it->substr(split_pos1 + 1).substr(split_pos2 + 1).substr(0, split_pos3);
        contract_no_ = it->substr(split_pos1 + 1).substr(split_pos2 + 1).substr(split_pos3 + 1);

		TapAPIContract tmp;
		strcpy(tmp.Commodity.ExchangeNo, exchange_no_.c_str());
		tmp.Commodity.CommodityType = commodity_type_;
		strcpy(tmp.Commodity.CommodityNo, commodity_no_.c_str());
		strcpy(tmp.ContractNo1, contract_no_.c_str());
		tmp.CallOrPutFlag1 = TAPI_CALLPUT_FLAG_NONE;
		tmp.CallOrPutFlag2 = TAPI_CALLPUT_FLAG_NONE;

		std::thread thread_subscribe_quote(std::bind(&MYTAPDataHandler::subscribe_quote, this, tmp));
    }
}

void MYTAPDataHandler::subscribe_quote(TapAPIContract &contract)
{
    int ret = 0;

    ret = api_->SubscribeQuote(sID, &contract);
        MY_LOG_ERROR("TAP - SubscribeQuote failed, ExchangeNo is %s, CommodityNo is %s,"
			"ContractNo is %s, errorCode is %d.",
            contract.Commodity.ExchangeNo, contract.Commodity.CommodityNo, 
			contract.ContractNo1, ret);

    MY_LOG_INFO("TAP - SubscribeQuote successful, ExchangeNo is %s, CommodityNo is %s,"
		"ContractNo is %s.", 
		contract.Commodity.ExchangeNo, contract.Commodity.CommodityNo, contract.ContractNo1);
}

void MYTAPDataHandler::OnDisconnect(TAPIINT32 reasonCode)
{
    MY_LOG_INFO("TAP - OnDisconnect, reasonCode is %d, reconnecting.", reasonCode);
}

void MYTAPDataHandler::OnRspQryExchange(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIExchangeInfo *info) { }
void MYTAPDataHandler::OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteCommodityInfo *info) { }
void MYTAPDataHandler::OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteContractInfo *info) { }
void MYTAPDataHandler::OnRtnContract(const TapAPIQuoteContractInfo *info) { }

void MYTAPDataHandler::OnRspSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteWhole *info)
{
    MY_LOG_INFO("TAP - OnRspSubscribeQuote");

    if (errorCode == 0 && NULL != info){
		TapAPIQuoteWhole_MY data_my = Convert(*info);
		/* 发出去 */
		if (tap_handler_) { tap_handler_(&data_my); }

        MY_LOG_INFO("TAP - OnRspSubscribeQuote Successful, ExchangNo is %s, CommodityNo is %s, ContractNo is %s.",
            info->Contract.Commodity.ExchangeNo, info->Contract.Commodity.CommodityNo, info->Contract.ContractNo1);
    } else {
        MY_LOG_WARN("TAP - SubscribeQuote failed, the error code is %d.", errorCode);
    }
}

void MYTAPDataHandler::OnRtnQuote(const TapAPIQuoteWhole *info)
{
    //MY_LOG_INFO("TAP - OnRtnQuote");
    if ( NULL != info) {
		TapAPIQuoteWhole_MY data_my = Convert(*info);
		if (tap_handler_) { tap_handler_(&data_my); }
      //  MY_LOG_INFO("TAP - OnRtnQuote Successful, ExchangNo is %s, CommodityNo is %s, ContractNo is %s.",
       //     info->Contract.Commodity.ExchangeNo, info->Contract.Commodity.CommodityNo, info->Contract.ContractNo1);
    } else {
        MY_LOG_ERROR("TAP - RtnQuote failed, the pointer is null.");
    }
}

TapAPIQuoteWhole_MY MYTAPDataHandler::Convert(const TapAPIQuoteWhole &other)
{
    TapAPIQuoteWhole_MY data;

    strcpy(data.ExchangeNo, other.Contract.Commodity.ExchangeNo);
    data.CommodityType = other.Contract.Commodity.CommodityType;
    strcpy(data.CommodityNo, other.Contract.Commodity.CommodityNo);
    strcpy(data.ContractNo1, other.Contract.ContractNo1);
    strcpy(data.StrikePrice1, other.Contract.StrikePrice1);
    data.CallOrPutFlag1 = other.Contract.CallOrPutFlag1;
    strcpy(data.ContractNo2, other.Contract.ContractNo2);
    strcpy(data.StrikePrice2, other.Contract.StrikePrice2);
    data.CallOrPutFlag2 = other.Contract.CallOrPutFlag2;

    strcpy(data.CurrencyNo, other.CurrencyNo);
    data.TradingState = other.TradingState;
    strcpy(data.DateTimeStamp, other.DateTimeStamp);

    data.QPreClosingPrice = other.QPreClosingPrice;
    data.QPreSettlePrice = other.QPreSettlePrice;
    data.QPrePositionQty = other.QPrePositionQty;
    data.QOpeningPrice = other.QOpeningPrice;
    data.QLastPrice = other.QLastPrice;
    data.QHighPrice = other.QHighPrice;
    data.QLowPrice = other.QLowPrice;
    data.QHisHighPrice = other.QHisHighPrice;
    data.QHisLowPrice = other.QHisLowPrice;
    data.QLimitUpPrice = other.QLimitUpPrice;
    data.QLimitDownPrice = other.QLimitDownPrice;
    data.QTotalQty = other.QTotalQty;
    data.QTotalTurnover = other.QTotalTurnover;
    data.QPositionQty = other.QPositionQty;
    data.QAveragePrice = other.QAveragePrice;
    data.QClosingPrice = other.QClosingPrice;
    data.QSettlePrice = other.QSettlePrice;
    data.QLastQty = other.QLastQty;
    for(int i = 0; i < 20; i++) {
        data.QBidPrice[i] = other.QBidPrice[i];
        data.QBidQty[i] = other.QBidQty[i];
        data.QAskPrice[i] = other.QAskPrice[i];
        data.QAskQty[i] = other.QAskQty[i];
    }
    data.QImpliedBidPrice = other.QImpliedBidPrice;
    data.QImpliedBidQty = other.QImpliedBidQty;
    data.QImpliedAskPrice = other.QImpliedAskPrice;
    data.QImpliedAskQty = other.QImpliedAskQty;
    data.QPreDelta = other.QPreDelta;
    data.QCurrDelta = other.QCurrDelta;
    data.QInsideQty = other.QInsideQty;
    data.QOutsideQty = other.QOutsideQty;
    data.QTurnoverRate = other.QTurnoverRate;
    data.Q5DAvgQty = other.Q5DAvgQty;
    data.QPERatio = other.QPERatio;
    data.QTotalValue = other.QTotalValue;
    data.QNegotiableValue = other.QNegotiableValue;
    data.QPositionTrend = other.QPositionTrend;
    data.QChangeSpeed = other.QChangeSpeed;
    data.QChangeRate = other.QChangeRate;
    data.QChangeValue = other.QChangeValue;
    data.QSwing = other.QSwing;
    data.QTotalBidQty = other.QTotalBidQty;
    data.QTotalAskQty = other.QTotalAskQty;

    return data;
}

void MYTAPDataHandler::OnRspUnSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIContract *info) { }
void MYTAPDataHandler::OnRspChangePassword(TAPIUINT32 sessionID, TAPIINT32 errorCode) { } 
