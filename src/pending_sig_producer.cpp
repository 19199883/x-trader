#include "pending_sig_producer.h"

PendingSigProducer::PendingSigProducer(struct vrt_queue  *queue)
{
	clog_info("[%s] PENDING_SIG_BUFFER_SIZE: %d;", CLOG_MODULE, SIG_BUFFER_SIZE);

	rip_check(this->procucer_ = vrt_producer_new("pendingsig_producer", 1, queue));
}

~PendingSigProducer::PendingSigProducer()
{
	if (this->producer_ != NULL){
		vrt_producer_free(this->producer_);
		this->producer_ = NULL;
		clog_info("[%s] release pendingsig_producer.", CLOG_MODULE);
	}
}

int32_t PendingSigProducer::push(const signal_t& sig)
{
	static int32_t cursor = SIG_BUFFER_SIZE - 1;
	cursor++;
	if ((cursor % SIG_BUFFER_SIZE) == 0){
		cursor = 0;
	}
	sig_buffer_[cursor] = sig;

	clog_debug("[%s] push Pending Sig: cursor,%d; Sig Id:%d;",
				CLOG_MODULE, cursor, sig->sig_id);

	return cursor;
}
