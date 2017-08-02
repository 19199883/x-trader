#include "pending_sig_producer.h"

PendingSigProducer::PendingSigProducer(struct vrt_queue  *queue)
:module_name_("PendingSigProducer")
{
	clog_info("[%s] PENDING_SIG_BUFFER_SIZE: %d;", module_name_, PENDINGSIG_BUFFER_SIZE);

	(this->producer_ = vrt_producer_new("pendingsig_producer", 1, queue));
}

PendingSigProducer::~PendingSigProducer()
{
	if (this->producer_ != NULL){
		vrt_producer_free(this->producer_);
		this->producer_ = NULL;
		clog_info("[%s] release pendingsig_producer.", module_name_);
	}
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
