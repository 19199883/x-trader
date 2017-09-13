#include "pending_sig_producer.h"

PendingSigProducer::PendingSigProducer(struct vrt_queue  *queue)
:module_name_("PendingSigProducer")
{
	ended_ = false;

	this->ParseConfig();

	clog_info("[%s] PENDING_SIG_BUFFER_SIZE: %d;", module_name_, PENDINGSIG_BUFFER_SIZE);

	(this->producer_ = vrt_producer_new("pendingsig_producer", 1, queue));

	if(strcmp(config_.yield, "threaded") == 0){
		this->producer_ ->yield = vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		this->producer_ ->yield = vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		this->producer_ ->yield = vrt_yield_strategy_hybrid();
	}
}

void PendingSigProducer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    

	// yield strategy
    TiXmlElement *comp_node = RootElement->FirstChildElement("Compliance");
	if (comp_node != NULL){
		this->yield = comp_node->Attribute("yield");
	} else { clog_error("[%s] x-trader.config error: Compliance node missing.", module_name_); }
}

PendingSigProducer::~PendingSigProducer()
{
//	if (this->producer_ != NULL){
//		vrt_producer_free(this->producer_);
//		this->producer_ = NULL;
//		clog_info("[%s] release pendingsig_producer.", module_name_);
//	}
}

void PendingSigProducer::End()
{
	ended_ = true;
	(vrt_producer_eof(producer_));
}

void PendingSigProducer::Publish(const signal_t& sig)
{
	if (ended_) return;

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	(vrt_producer_claim(producer_, &vvalue));
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = Push(sig);
	ivalue->data = PENDING_SIGNAL;

	clog_debug("[%s] Publish strategy id:%d signal id:%d",
				module_name_, sig.st_id, sig.sig_id);

	(vrt_producer_publish(producer_));
}

int32_t PendingSigProducer::Push(const signal_t& sig)
{
	static int32_t cursor = PENDINGSIG_BUFFER_SIZE - 1;
	cursor++;
	if ((cursor % PENDINGSIG_BUFFER_SIZE) == 0){
		cursor = 0;
	}
	sig_buffer_[cursor] = sig;

	clog_debug("[%s] push Pending Sig: cursor,%d; strategy id:%d; Sig Id:%d;",
				module_name_, cursor, sig.st_id, sig.sig_id);

	return cursor;
}

signal_t* PendingSigProducer::GetSignal(int32_t index)
{
	return &(sig_buffer_[index]);
}
