#include "pending_sig_producer.h"

PendingSigProducer::PendingSigProducer(struct vrt_queue  *queue)
:module_name_("PendingSigProducer")
{
	ended_ = false;
	clog_info("[%s] PENDING_SIG_BUFFER_SIZE: %d;", module_name_, PENDINGSIG_BUFFER_SIZE);

	(this->producer_ = vrt_producer_new("pendingsig_producer", 1, queue));
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
	ivalue->index = push(sig);
	ivalue->data = PENDING_SIGNAL;

	clog_debug("[%s] Publish strategy id:%d signal id:%d",
				module_name_, ivalue->index, ivalue->data, sig.st_id, sig.sig_id);

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

	clog_debug("[%s] push Pending Sig: cursor,%d; Sig Id:%d;",
				module_name_, cursor, sig.sig_id);

	return cursor;
}

signal_t* PendingSigProducer::GetSignal(int32_t index)
{
	return &(sig_buffer_[index]);
}
