#include <functional>   // std::bind
#include "md_producer.h"

using namespace std::placeholders;
using namespace std;

MDProducer::MDProducer(struct vrt_queue  *queue)
:module_name_("MDProducer")
{
	clog_info("[%s] MD_BUFFER_SIZE: %d;", module_name_, MD_BUFFER_SIZE);

	(this->producer_ = vrt_producer_new("md_producer", 1, queue));
	this->producer_ ->yield = vrt_yield_strategy_threaded();

	md_provider_ = build_quote_provider(subs_);

	auto f_bestanddeep = std::bind(&MDProducer::OnGTAQuoteData,this,_1);
	md_provider_->SetQuoteDataHandler(f_bestanddeep);

	auto f_orderstatics = std::bind(&MDProducer::OnGTAQuoteData, this, _1);
	md_provider_->SetQuoteDataHandler(f_orderstatics);
}

MDProducer::~MDProducer(){
	if (md_provider_ != NULL){
		delete md_provider_;
		md_provider_ = NULL;

		clog_info("[%s] release md_provider.", module_name_);
	}

	if (this->producer_ != NULL){
		vrt_producer_free(this->producer_);
		this->producer_ = NULL;
		clog_info("[%s] release md_producer.", module_name_);
	}
}

MYQuoteData* MDProducer::build_quote_provider(SubscribeContracts &subscription) {
	TiXmlDocument config = TiXmlDocument("trasev.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    
	TiXmlElement* MarketData = RootElement->FirstChildElement("MarketData");
	if (NULL != MarketData) {
		md_config = MarketData->Attribute("config");
		clog_info("[%s] MarketData.config: %s", module_name_, md_config);
		return new MYQuoteData(&subs_, md_config);
	}
	else{
		clog_error("[%s] can not find 'MarkerData' node.", module_name_);
		return NULL;
	}
}
void MDProducer::OnGTAQuoteData(const MDBestAndDeep_MY* md){
	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	rpi_check(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = push(*md);
	ivalue->data = BESTANDDEEP;

	clog_debug("[%s] rev MDBestAndDeep: index,%d; data,%d; contracr:%s; time: %s",
				module_name_, ivalue->index, ivalue->data, md->Contract, md->GenTime);

	rpi_check(vrt_producer_publish(producer_));
}

int32_t MDProducer::push(const MDBestAndDeep_MY& md){
	static int32_t bestanddeep_cursor = MD_BUFFER_SIZE - 1;
	bestanddeep_cursor++;
	if (bestanddeep_cursor%MD_BUFFER_SIZE == 0){
		bestanddeep_cursor = 0;
	}
	bestanddeep_buffer_[bestanddeep_cursor] = md;

	clog_debug("[%s] push MDBestAndDeep: cursor,%d; contract:%s; time: %s",
				module_name_, orderstatics_cursor, md->Contract, md->GenTime);

	return bestanddeep_cursor;
}

MDBestAndDeep_MY* MDProducer::GetBestAnddeep(int32_t index)
{
	return &bestanddeep_buffer_[index];
}

void MDProducer::OnGTAQuoteData(const MDOrderStatistic_MY* md){
	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	rpi_check(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = push(*md);
	ivalue->data = ORDERSTATISTIC;

	clog_debug("[%s] rev MDOrderStatistic: index,%d; data,%d; contracr:%s; time: %s",
				module_name_, ivalue->index, ivalue->data, md->Contract, "");

	rpi_check(vrt_producer_publish(producer_));
}

int32_t MDProducer::push(const MDOrderStatistic_MY& md){
	static int32_t orderstatics_cursor = MD_BUFFER_SIZE - 1;
	orderstatics_cursor++;
	if (orderstatics_cursor%MD_BUFFER_SIZE == 0){
		orderstatics_cursor = 0;
	}
	orderstatics_buffer_[orderstatics_cursor] = md;


	clog_debug("[%s] push MDOrderStatistic: cursor,%d; contract:%s; time: %s",
				module_name_, orderstatics_cursor, md->Contract, "");

	return orderstatics_cursor;
}

MDOrderStatistic_MY* MDProducer::GetOrderStatistic(int32_t index)
{
	return &orderstatics_buffer_[index];
}
