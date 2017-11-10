#include "quote_tap.h"

using namespace my_cmn;
using namespace std;

MYTAPDataHandler::MYTAPDataHandler(const SubscribeContracts *subscribe_contracts, const ConfigData &cfg)
    : tap_handler_(NULL), cfg_(cfg)
{
    if (subscribe_contracts) {
        subscribe_contracts_ = *subscribe_contracts;
    }

    /* 初始化 */
    logoned_ = false;
    in_login_flag_ = false;
    api_ = NULL;
    sID = new unsigned int;
    check_login_ = false;

    /* 设置接口所需信息 */
    struct TapAPIApplicationInfo p_info_;
	strcpy(p_info_.AuthCode,
        "B112F916FE7D27BCE7B97EB620206457946CED32E26C1EAC946CED32E26C1EAC946CED32E26C1EAC946CED32E26C1EAC5211AF9FEE541DDE123D2F2F8E7F3E4B946CED32E26C1EAC5A51B81D8526AB6A67D1B6302B4DDA7D946CED32E26C1EACD33D6030790F8965ABD9B8F170E14F8847D3EA0BF4E191F50905910EA362CB063C704B1E62DE54B938D80BD82C58B3980985E67B9910AF76A06C27260450E7F792D349532A6533D9952A55F6D7C8C437456145239FEDE5078EA7CBC5AB74E107BA8DC0B7CE56681E22C185C880AC2723510A31A504180EE423496CBBE968917E1A292DAECE9F5F491626856EE3C81F0C3F2F4454DC7EB391DA8AF4EC06A48782");
	getcwd(p_info_.KeyOperationLogPath, 301);

    int iResult = 0;
    api_ = CreateTapQuoteAPI(&p_info_, iResult);
    if ( NULL == api_) {
        MY_LOG_ERROR("TAP - CreateTapQuoteAPI failed, the error code is %d", iResult);
        return;
    }else{
        api_->SetAPINotify(this);
        char *addr_tmp = new char[quote_addr_.size() + 1];
        char *addr_tmp2, *port_tmp;

        memcpy(addr_tmp, quote_addr_.c_str(), quote_addr_.size() + 1);
        MY_LOG_INFO("TAP - prepare to connect quote provider: %s", quote_addr_.c_str());
        addr_tmp2 = strtok(addr_tmp, ":");
        port_tmp = strtok(NULL, ":");

        /* 设置服务器地址 */
        if (0 == api_->SetHostAddress(addr_tmp2, atoi(port_tmp))) { }
        else { return; }
        delete[] addr_tmp;
        sleep(1);

		// TODO: refer to login of tunnel
    }
}

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
    /* 服务器地址 */
    quote_addr_ = cfg_.Logon_config().quote_provider_addrs.front();

    /* 用户名 密码 */
    user_ = cfg_.Logon_config().account;
    pswd_ = cfg_.Logon_config().password;

    /* 订阅集合 */
    subscribe_contracts_ = cfg_.Subscribe_datas();
}

void MYTAPDataHandler::OnRspLogin(TAPIINT32 errorCode, const TapAPIQuotLoginRspInfo *info)
{
    MY_LOG_INFO("TAP - OnRspLogin");
    check_login_ = true;
    if (0 == errorCode){
        logoned_ = true;
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
