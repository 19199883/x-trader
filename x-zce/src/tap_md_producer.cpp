#include <functional>   // std::bind
#include "tap_md_producer.h"
#include "perfctx.h"

using namespace std::placeholders;
using namespace std;

TapMDProducer::TapMDProducer(struct vrt_queue  *queue)
:module_name_("TapMDProducer")
{
	ended_ = false;

	ParseConfig();

	clog_warning("[%s] MD_BUFFER_SIZE: %d;", module_name_, MD_BUFFER_SIZE);

	(this->producer_ = vrt_producer_new("md_producer", 1, queue));

	clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0){
		this->producer_ ->yield = vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		this->producer_ ->yield = vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		this->producer_ ->yield = vrt_yield_strategy_hybrid();
	}

    sID = new unsigned int;
	InitApi();
	Login();
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
        strcpy(addr_tmp, config_.addr);
        clog_warning("[%s] TAP - prepare to connect quote provider: %s",
			module_name_, config_.addr);
        addr_tmp2 = strtok(addr_tmp, ":");
        port_tmp = strtok(NULL, ":");
		int result = api_->SetHostAddress(addr_tmp2, atoi(port_tmp))
        if (0 != result){
			clog_error("[%s] TAP - SetHostAddress failed:%d", module_name_, result);
		}

        delete[] addr_tmp;
    }
}

void TapMDProducer::Login()
{
	// TODO: to here
	// TODO:
		// TODO: refer to login of tunnel
}

void TapMDProducer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    

	// yield strategy
    TiXmlElement *comp_node = RootElement->FirstChildElement("Disruptor");
	if (comp_node != NULL){
		strcpy(config_.yield, comp_node->Attribute("yield"));
	} else { clog_error("[%s] x-trader.config error: Disruptor node missing.", module_name_); }
}

TapMDProducer::~TapMDProducer(){
	if (md_provider_ != NULL){
		delete md_provider_;
		md_provider_ = NULL;
		clog_warning("[%s] release md_provider.", module_name_);
	}
}

MYQuoteData* TapMDProducer::build_quote_provider(SubscribeContracts &subscription) {
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    
	TiXmlElement* MarketData = RootElement->FirstChildElement("MarketData");
	if (NULL != MarketData) {
		string md_config = MarketData->Attribute("config");
		clog_warning("[%s] MarketData.config: %s", module_name_, md_config.c_str());
		return new MYQuoteData(&subs_, md_config);
	}
	else{
		clog_error("[%s] can not find 'MarkerData' node.", module_name_);
		return NULL;
	}
}

void TapMDProducer::End()
{
	ended_ = true;
	(vrt_producer_eof(producer_));
}

void TapMDProducer::OnMD(const ZCEL2QuotSnapshotField_MY* md)
{
	if (ended_) return;

	// 目前三个市场，策略支持的品种的合约长度是：5或6个字符
	if (strlen(md->ContractID) > 6) return;

#ifdef LATENCY_MEASURE
	static int cnt = 0;
	perf_ctx::insert_t0(cnt);
	cnt++;
#endif

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = push(*md);
	ivalue->data = L2QUOTESNAPSHOT;
	(vrt_producer_publish(producer_));

	clog_debug("[%s] rev ZCEL2QuotSnapshotField: index,%d; data,%d; contract:%s; time: %s",
				module_name_, ivalue->index, ivalue->data, md->ContractID, md->TimeStamp);
}

int32_t TapMDProducer::push(const ZCEL2QuotSnapshotField_MY& md){
	static int32_t l2quotesnapshot_cursor = MD_BUFFER_SIZE - 1;
	l2quotesnapshot_cursor++;
	if (l2quotesnapshot_cursor%MD_BUFFER_SIZE == 0){
		l2quotesnapshot_cursor = 0;
	}
	l2quotesnapshot_buffer_[l2quotesnapshot_cursor] = md;

	return l2quotesnapshot_cursor;
}

ZCEL2QuotSnapshotField_MY* TapMDProducer::GetL2QuoteSnapshot(int32_t index)
{
	return &l2quotesnapshot_buffer_[index];
}


