#include "md_producer.h"

MDProducer::MDProducer(struct vrt_queue  *queue){
	rip_check(this->procucer_ = vrt_producer_new("md_producer", 1, queue));

	md_provider_ = build_quote_provider(subs_);

	std::function<void (const MDBestAndDeep_MY*)> f_bestanddeep = std::bind(&MDProducer::OnGTAQuoteData, this, _1);
	md_provider_->SetQuoteDataHandler(f_bestanddeep);

	std::function<void (const MDOrderStatistic_MY*)> f_orderstatics = std::bind(&MDProducer::OnGTAQuoteData, this, _1);
	md_provider_->SetQuoteDataHandler(f_orderstatics);
}

MYQuoteData* MDProducer::build_quote_provider(SubscribeContracts &subscription) {
	TiXmlDocument config = TiXmlDocument("trasev.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    
	TiXmlElement* MarketData = RootElement->FirstChildElement("MarketData");
	if (NULL != MarketData) {
		md_config = MarketData->Attribute("config");
		return new MYQuoteData(&subs_, md_config);
	}
	else{
		// write error into log file
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
	rpi_check(vrt_producer_publish(producer_));
}

int32_t MDProducer::push(const MDBestAndDeep_MY& md){
	int32_t bestanddeep_cursor = MD_BUFFER_SIZE - 1;
	bestanddeep_cursor++;
	if (bestanddeep_cursor/MD_BUFFER_SIZE == 0){
		bestanddeep_cursor = 0;
	}
	bestanddeep_buffer_[bestanddeep_cursor] = md;
	return bestanddeep_cursor;
}

void MDProducer::OnGTAQuoteData(const MDOrderStatistic_MY* md){
	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	rpi_check(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = push(*md);
	ivalue->data = ORDERSTATISTIC;
	rpi_check(vrt_producer_publish(producer_));
}

int32_t MDProducer::push(const MDOrderStatistic_MY& md){
	int32_t orderstatics_cursor = MD_BUFFER_SIZE - 1;
	orderstatics_cursor++;
	if (orderstatics_cursor/MD_BUFFER_SIZE == 0){
		orderstatics_cursor = 0;
	}
	orderstatics_buffer_[orderstatics_cursor] = md;
	return orderstatics_cursor;
}
