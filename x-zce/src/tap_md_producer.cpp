#include <unistd.h>
#include <functional>   // std::bind
#include <fstream>      // std::fstream
#include <iostream>
#include "tap_md_producer.h"
#include "TapQuoteAPI.h"
#include "TapTradeAPIDataType.h"
#include "perfctx.h"
#include "TapAPIError.h"

using namespace std::placeholders;
using namespace std;

TapAPIQuoteWhole_MY* L1MDProducerHelper::GetLastDataImp(const char *contract, int32_t last_index,
	TapAPIQuoteWhole_MY*buffer, int32_t buffer_size, int32_t dominant_contract_count)
{
	TapAPIQuoteWhole_MY* data = NULL;

	// 需要一档行情时，从缓存最新位置向前查找13个位置（假设有13个主力合约），找到即停
	int i = 0;
	for(; i<dominant_contract_count; i++){
		int data_index = last_index - i;
		if(data_index < 0){
			data_index = data_index + buffer_size;
		}

		TapAPIQuoteWhole_MY &tmp = buffer[data_index];
		if(IsEqualSize4(contract, tmp.CommodityNo, tmp.ContractNo1)){ // contract: e.g. SR1801
			data = &tmp; 
			break;
		}
	}

	return data;
}

TapMDProducer::TapMDProducer(struct vrt_queue  *queue)
:module_name_("TapMDProducer")
{
	l1data_cursor_ = L1MD_BUFFER_SIZE - 1;
	ended_ = false;
    api_ = NULL;
	clog_warning("[%s] L1MD_BUFFER_SIZE:%d;",module_name_,L1MD_BUFFER_SIZE);

	ParseConfig();
	
	// init dominant contracts
	memset(dominant_contracts_, 0, sizeof(dominant_contracts_));
	dominant_contract_count_ = LoadDominantContracts(config_.contracts_file, dominant_contracts_);

	memset(&target_data_, 0, sizeof(target_data_));
	memset(&md_buffer_, 0, sizeof(md_buffer_));
    sID = new unsigned int;
	InitApi();
	Login();

	(this->producer_ = vrt_producer_new("md_producer", 1, queue));
	clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0){
		this->producer_ ->yield = vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		this->producer_ ->yield = vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		this->producer_ ->yield = vrt_yield_strategy_hybrid();
	}
}

void TapMDProducer::InitApi()
{
    /* 设置接口所需信息 */
    struct TapAPIApplicationInfo p_info_;
	strcpy(p_info_.AuthCode,
        "B112F916FE7D27BCE7B97EB620206457946CED32E26C1EAC946CED32E26C1EAC946CED32E26C1EAC946CED32E26C1EAC5211AF9FEE541DDE123D2F2F8E7F3E4B946CED32E26C1EAC5A51B81D8526AB6A67D1B6302B4DDA7D946CED32E26C1EACD33D6030790F8965ABD9B8F170E14F8847D3EA0BF4E191F50905910EA362CB063C704B1E62DE54B938D80BD82C58B3980985E67B9910AF76A06C27260450E7F792D349532A6533D9952A55F6D7C8C437456145239FEDE5078EA7CBC5AB74E107BA8DC0B7CE56681E22C185C880AC2723510A31A504180EE423496CBBE968917E1A292DAECE9F5F491626856EE3C81F0C3F2F4454DC7EB391DA8AF4EC06A48782");
	getcwd(p_info_.KeyOperationLogPath, 301);

    int iResult = 0;
    api_ = CreateTapQuoteAPI(&p_info_, iResult);
    if ( NULL == api_) {
        clog_error("[%s] TAP - CreateTapQuoteAPI failed, the error code is %d", 
			module_name_, iResult);
    }else{
        api_->SetAPINotify(this);
        char *addr_tmp = new char[sizeof(config_.addr)];
        char *addr_tmp2, *port_tmp;
        strcpy(addr_tmp, config_.addr.c_str());
        clog_warning("[%s] TAP - prepare to connect quote provider: %s",
			module_name_, config_.addr.c_str());
        addr_tmp2 = strtok(addr_tmp, ":");
        port_tmp = strtok(NULL, ":");
		int result = api_->SetHostAddress(addr_tmp2, atoi(port_tmp));
        if (0 != result){
			clog_error("[%s] TAP - SetHostAddress failed:%d", module_name_, result);
		}

        delete[] addr_tmp;
    }
}

void TapMDProducer::Login()
{
    TapAPIQuoteLoginAuth stLoginAuth;
    memset(&stLoginAuth, 0, sizeof(stLoginAuth));
    strcpy(stLoginAuth.UserNo, config_.user);
    strcpy(stLoginAuth.Password, config_.password);
    stLoginAuth.ISModifyPassword = APIYNFLAG_NO;
    stLoginAuth.ISDDA = APIYNFLAG_NO;
    TAPIINT32 result = api_->Login(&stLoginAuth);
    if (TAPIERROR_SUCCEED != result) {
        clog_error("[%s] Login Error, result:%d",module_name_,result);
    }else{
        clog_warning("[%s] Login success", module_name_);
	}
}

void TapMDProducer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    

	// yield strategy
    TiXmlElement *disruptor_node = RootElement->FirstChildElement("Disruptor");
	if (disruptor_node != NULL){
		strcpy(config_.yield, disruptor_node->Attribute("yield"));
	} else { clog_error("[%s] x-trader.config error: Disruptor node missing.", module_name_); }

	// addr
    TiXmlElement *l1md_node = RootElement->FirstChildElement("L1Md");
	if (l1md_node != NULL){
		config_.addr = l1md_node->Attribute("addr");
		strcpy(config_.user, l1md_node->Attribute("user"));
		strcpy(config_.password, l1md_node->Attribute("password"));
	} else { clog_error("[%s] x-trader.config error: L1Md node missing.", module_name_); }
	
	// contracts file
    TiXmlElement *contracts_file_node = RootElement->FirstChildElement("Subscription");
	if (contracts_file_node != NULL){
		strcpy(config_.contracts_file, contracts_file_node->Attribute("contracts"));
	} else { clog_error("[%s] x-trader.config error: Subscription node missing.", module_name_); }

	size_t ipstr_start = config_.addr.find("//")+2;
	size_t ipstr_end = config_.addr.find(":",ipstr_start);
	strcpy(config_.ip, config_.addr.substr(ipstr_start,ipstr_end-ipstr_start).c_str());
	config_.port = stoi(config_.addr.substr(ipstr_end+1));
}

TapMDProducer::~TapMDProducer(){
    if (api_){
        api_->Disconnect();
    }

    if (sID) { delete sID; }
	clog_warning("[%s] L1MD exited.", module_name_);
}

void TapMDProducer::End()
{
	ended_ = true;
	(vrt_producer_eof(producer_));
	clog_warning("[%s] End exit", module_name_);
}

int32_t TapMDProducer::Push(const TapAPIQuoteWhole_MY& md){
	l1data_cursor_++;
	if (l1data_cursor_ % L1MD_BUFFER_SIZE == 0){
		l1data_cursor_ = 0;
	}
	md_buffer_[l1data_cursor_] = md;

	return l1data_cursor_;
}

TapAPIQuoteWhole_MY* TapMDProducer::GetData(int32_t index)
{
	return &md_buffer_[index];
}

void TapMDProducer::OnRspLogin(TAPIINT32 errorCode, const TapAPIQuotLoginRspInfo *info)
{
    clog_warning("[%s] TAP - OnRspLogin", module_name_);
    if (0 == errorCode){
    } else {
        clog_error("[%s] TAP - login failed, ErrorCode = %d", module_name_, errorCode);
    }
}

// 交易所(ZCE)，品种类型硬编码(F)
// 品种，日期从合约读取,e.g. SR
// contract：801
void TapMDProducer::OnAPIReady()
{
    clog_warning("[%s] TAP - OnAPIReady", module_name_);

	std::string exchange_no = "ZCE";
	char commodity_type = TAPI_COMMODITY_TYPE_FUTURES;

	std::ifstream is;
	is.open (config_.contracts_file);
	string contrs = "";
	if (is) {
		getline(is, contrs);
		contrs += " ";
		size_t start_pos = 0;
		size_t end_pos = 0;
		string contr = "";
		while ((end_pos=contrs.find(" ",start_pos)) != string::npos){
			contr = contrs.substr (start_pos, end_pos-start_pos);

			string commodity_no = contr.substr(0, 2);
			string contract_no = contr.substr(2, 3);;

			TapAPIContract tmp;
			strcpy(tmp.Commodity.ExchangeNo, exchange_no.c_str());
			tmp.Commodity.CommodityType = commodity_type;
			strcpy(tmp.Commodity.CommodityNo, commodity_no.c_str());
			strcpy(tmp.ContractNo1, contract_no.c_str());
			tmp.CallOrPutFlag1 = TAPI_CALLPUT_FLAG_NONE;
			tmp.CallOrPutFlag2 = TAPI_CALLPUT_FLAG_NONE;
			int ret = api_->SubscribeQuote(sID, &tmp);
			if(ret != 0){
				clog_error("[%s] TAP - SubscribeQuote failed, ExchangeNo is %s, CommodityNo is %s,"
					"ContractNo is %s, errorCode is %d.",
					module_name_, tmp.Commodity.ExchangeNo, tmp.Commodity.CommodityNo, 
					tmp.ContractNo1, ret);
			}

			clog_warning("TAP - SubscribeQuote successful, ExchangeNo is %s, CommodityNo is %s,"
				"ContractNo is %s.", 
				tmp.Commodity.ExchangeNo, tmp.Commodity.CommodityNo, tmp.ContractNo1);

			start_pos = end_pos + 1;
		}
	}else { 
		clog_error("[%s] can't open: %s", module_name_, config_.contracts_file); 
	}
}

void TapMDProducer::OnDisconnect(TAPIINT32 reasonCode)
{
    clog_error("[%s] TAP - OnDisconnect, reasonCode is %d, reconnecting.", module_name_, reasonCode);
}

void TapMDProducer::OnRspQryExchange(TAPIUINT32 sessionID, TAPIINT32 errorCode, 
	TAPIYNFLAG isLast, const TapAPIExchangeInfo *info) { }

void TapMDProducer::OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
	const TapAPIQuoteCommodityInfo *info) { }

void TapMDProducer::OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast,
	const TapAPIQuoteContractInfo *info) { }

void TapMDProducer::OnRtnContract(const TapAPIQuoteContractInfo *info) { }

void TapMDProducer::OnRspSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, 
	TAPIYNFLAG isLast, const TapAPIQuoteWhole *info)
{
	if(ended_) return;

    clog_info("[%s] TAP - OnRspSubscribeQuote", module_name_);

    if (errorCode == 0 && NULL != info){
		// 抛弃非主力合约
		if(!(IsDominant(info->Contract.Commodity.CommodityNo, info->Contract.ContractNo1))) return;

		Convert(*info, target_data_);

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		vrt_producer_claim(producer_, &vvalue);
		ivalue = cork_container_of(vvalue, struct vrt_hybrid_value,parent);
		ivalue->index = Push(target_data_);
		ivalue->data = L1_MD;
		vrt_producer_publish(producer_);

        clog_info("[%s] TAP - OnRspSubscribeQuote Successful, ExchangNo is %s, "
			"CommodityNo is %s, ContractNo is %s.", module_name_,
            info->Contract.Commodity.ExchangeNo, info->Contract.Commodity.CommodityNo, 
			info->Contract.ContractNo1);
    }else{
        clog_error("[%s] TAP - SubscribeQuote failed, the error code is %d.", module_name_,
			errorCode);
    }
}

void TapMDProducer::OnRtnQuote(const TapAPIQuoteWhole *info)
{
	if(ended_) return;

    clog_info("[%s] TAP - OnRtnQuote", module_name_);
    if ( NULL != info) {
		// 抛弃非主力合约
		if(!(IsDominant(info->Contract.Commodity.CommodityNo, info->Contract.ContractNo1))) return;

		Convert(*info, target_data_);

		struct vrt_value  *vvalue;
		struct vrt_hybrid_value  *ivalue;
		vrt_producer_claim(producer_, &vvalue);
		ivalue = cork_container_of(vvalue, struct vrt_hybrid_value,parent);
		ivalue->index = Push(target_data_);
		ivalue->data = L1_MD;
		vrt_producer_publish(producer_);
		
        clog_info("[%s] TAP - OnRtnQuote Successful, ExchangNo is %s, CommodityNo is %s,"
			"ContractNo is %s.",
            info->Contract.Commodity.ExchangeNo, info->Contract.Commodity.CommodityNo, 
			info->Contract.ContractNo1);
    } else {
        clog_error("[%s] TAP - RtnQuote failed, the pointer is null.", module_name_);
    }
}

void TapMDProducer::Convert(const TapAPIQuoteWhole &other, TapAPIQuoteWhole_MY &data)
{
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
}

void TapMDProducer::OnRspUnSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, 
	TAPIYNFLAG isLast, const TapAPIContract *info) { }

void TapMDProducer::OnRspChangePassword(TAPIUINT32 sessionID, TAPIINT32 errorCode) { } 

TapAPIQuoteWhole_MY* TapMDProducer::GetLastData(const char *contract, int32_t last_index)
{
	TapAPIQuoteWhole_MY* data = L1MDProducerHelper::GetLastDataImp(
				contract, last_index, md_buffer_, L1MD_BUFFER_SIZE, dominant_contract_count_);
	return data;
}

bool TapMDProducer::IsDominant(const char*commciodity_no, const char* contract_no)
{
	return IsDominantImp(commciodity_no, contract_no, dominant_contracts_, 
			dominant_contract_count_);
}
