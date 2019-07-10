#include <thread>         
#include <chrono>        
#include <algorithm>    
#include "uni_consumer.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "md_helper.h"

using namespace std::placeholders; 

UniConsumer::UniConsumer(struct vrt_queue  *queue, 
			TapMDProducer *l1md_producer, 
			L2MDProducer *l2md_producer)
:	 module_name_("uni_consumer"),
	running_(true), 
	l1_md_producer_(l1md_producer),
    l2_md_producer_(l2md_producer),
	io_service_()
{
	memset(valid_conn_, 0, sizeof(valid_conn_));
	for(int i=0; i<MAX_CONN_COUNT; i++){
		socks_.push_back(tcp::socket(io_service_));
	}

	
	ParseConfig();
	(this->consumer_ = vrt_consumer_new(module_name_, queue));
	clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0){
		this->consumer_->yield = vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		this->consumer_->yield = vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		this->consumer_->yield = vrt_yield_strategy_hybrid();
	}
}

UniConsumer::~UniConsumer()
{
}

void UniConsumer::ParseConfig()
{
	std::string config_file = "y-quote.config";
	TiXmlDocument doc = TiXmlDocument(config_file.c_str());
    doc.LoadFile();
    TiXmlElement *root = doc.RootElement();    
	
	// yield strategy
    TiXmlElement *dis_node = root->FirstChildElement("Disruptor");
	if (dis_node != NULL){
		strcpy(config_.yield, dis_node->Attribute("yield"));
		this->port_ = atoi(dis_node->Attribute("port"));
	} 
	else { 
		clog_error("[%s] y-quote.config error: Disruptor node missing.", module_name_); 
	}
}

void UniConsumer::Start()
{
	std::thread(&UniConsumer::Server, this).detach();
	
	running_  = true;

	MdHelper myquotedata(l2_md_producer_, l1_md_producer_);
	auto f_md = std::bind(&UniConsumer::ProcL2QuoteSnapshot, this,_1);
	myquotedata.SetQuoteDataHandler(f_md);

	int rc = 0;
	struct vrt_value  *vvalue;
	while (running_ &&
		   (rc = vrt_consumer_next(consumer_, &vvalue)) != VRT_QUEUE_EOF) {
		if (rc == 0) {
			struct vrt_hybrid_value *ivalue = cork_container_of(vvalue, struct vrt_hybrid_value, parent);
			switch (ivalue->data){
				case ZCE_L1_MD:
					myquotedata.ProcL1MdData(ivalue->index);
					break;
				case ZCE_L2_MD:
					myquotedata.ProcL2Data(ivalue->index);
					break;
				default:
					clog_error("[%s] [start] unexpected index: %d", module_name_, ivalue->index);
					break;
			}
		}
	} // end while (running_ &&

	if (rc == VRT_QUEUE_EOF) {
		clog_warning("[%s] [start] rev EOF.", module_name_);
	}
	clog_warning("[%s] [start] exit.", module_name_);
}

void UniConsumer::Stop()
{
	if(running_){
		l1_md_producer_->End();
		l2_md_producer_->End();		
		running_ = false;
		clog_warning("[%s] Stop exit.", module_name_);
	}

	fflush (Log::fp);
}

void UniConsumer::ProcL2QuoteSnapshot(YaoQuote* md)
{

#ifdef LATENCY_MEASURE
		high_resolution_clock::time_point t0 = high_resolution_clock::now();
#endif
	clog_debug("[test] proc [%s] [ProcL2QuoteSnapshot] contract:%s, time:%s", 
				module_name_, 
				md->symbol, 
				md->int_time);

	for(int i=0; i< MAX_CONN_COUNT; i++){		
		{
			std::lock_guard<std::mutex> lck (mtx_);
			if(0 == valid_conn_[i]) continue;
		}
		
		try{			
			 boost::system::error_code error;		  		
			 boost::asio::write(socks_[i], boost::asio::buffer(md, sizeof(YaoQuote)), error);			  
			 if (error){
				 valid_conn_[i] = 0;
				clog_warning("[%s] write error: %d", module_name_, error); 			
			 }
		}
		catch (std::exception& e){
			valid_conn_[i] = 0;
			clog_warning("[%s] send error:%s", module_name_, e.what()); 			
		}
	}	
#ifdef LATENCY_MEASURE
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		int latency = (t1.time_since_epoch().count() - t0.time_since_epoch().count()) / 1000;
		clog_warning("[%s] ProcL2QuoteSnapshotlatency:%d us", module_name_, latency); 
#endif
}

void UniConsumer::Server()
{
  tcp::acceptor a(io_service_, tcp::endpoint(tcp::v4(), port_));
  for (;;)
  {	
	int i = 0;
	for(; i<MAX_CONN_COUNT; i++){
		if(0 == valid_conn_[i]) break;
	}
	if(i < MAX_CONN_COUNT){
		a.accept(socks_[i]);
		{
			std::lock_guard<std::mutex> lck (mtx_);
			valid_conn_[i] = 1;
		}
	}
	else{
		clog_warning("[%s] socks_ is full.", module_name_);
	}
  }
}


