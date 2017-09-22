#include <functional>   // std::bind
#include "md_producer.h"
#include "perfctx.h"

using namespace std::placeholders;
using namespace std;

MDProducer::MDProducer(struct vrt_queue  *queue)
:module_name_("MDProducer")
{
	ended_ = false;

	ParseConfig();

	clog_info("[%s] MD_BUFFER_SIZE: %d;", module_name_, MD_BUFFER_SIZE);

	(this->producer_ = vrt_producer_new("md_producer", 1, queue));

	clog_info("[%s] yield:%s", module_name_, config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0){
		this->producer_ ->yield = vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		this->producer_ ->yield = vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		this->producer_ ->yield = vrt_yield_strategy_hybrid();
	}

	md_provider_ = build_quote_provider(subs_);

	auto f_shfemarketdata = std::bind(&MDProducer::OnShfeMarketData, this,_1);
	md_provider_->SetQuoteDataHandler(f_shfemarketdata);
}

void MDProducer::ParseConfig()
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

MDProducer::~MDProducer(){
	if (md_provider_ != NULL){
		delete md_provider_;
		md_provider_ = NULL;
		clog_info("[%s] release md_provider.", module_name_);
	}

//	if (this->producer_ != NULL){
//		vrt_producer_free(this->producer_);
//		this->producer_ = NULL;
//		clog_info("[%s] release md_producer.", module_name_);
//	}
}

MYQuoteData* MDProducer::build_quote_provider(SubscribeContracts &subscription) {
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    
	TiXmlElement* MarketData = RootElement->FirstChildElement("MarketData");
	if (NULL != MarketData) {
		string md_config = MarketData->Attribute("config");
		clog_info("[%s] MarketData.config: %s", module_name_, md_config.c_str());
		return new MYQuoteData(&subs_, md_config);
	}
	else{
		clog_error("[%s] can not find 'MarkerData' node.", module_name_);
		return NULL;
	}
}

void MDProducer::End()
{
	ended_ = true;
	(vrt_producer_eof(producer_));
	clog_info("[%s] End exit", module_name_);
}

void MDProducer::OnShfeMarketData(const MYShfeMarketData * md)
{
	if (ended_) return;

	// 目前三个市场，策略支持的品种的合约长度是：5或6个字符
	if (strlen(md->InstrumentID) > 6) return;

#ifdef LATENCY_MEASURE
	// latency measure
	static int cnt = 0;
	perf_ctx::insert_t0(cnt);
	cnt++;
#endif

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = push(*md);
	ivalue->data = SHFEMARKETDATA;
	(vrt_producer_publish(producer_));

	clog_debug("[%s] rev ShfeMarketData: index,%d; data,%d; contract:%s; time:%s",
				module_name_, ivalue->index, ivalue->data, md->InstrumentID,
				md->GetQuoteTime().c_str());
}

int32_t MDProducer::push(const MYShfeMarketData& md){
	static int32_t shfemarketdata_cursor = MD_BUFFER_SIZE - 1;
	shfemarketdata_cursor++;
	if (shfemarketdata_cursor%MD_BUFFER_SIZE == 0){
		shfemarketdata_cursor = 0;
	}
	shfemarketdata_buffer_[shfemarketdata_cursor] = md;

	clog_debug("[%s] push MDBestAndDeep: cursor,%d; contract:%s; time:%s",
				module_name_, shfemarketdata_cursor, md.InstrumentID, 
				md.GetQuoteTime().c_str());

	return shfemarketdata_cursor;
}

MYShfeMarketData* MDProducer::GetShfeMarketData(int32_t index)
{
	return &shfemarketdata_buffer_[index];
}

