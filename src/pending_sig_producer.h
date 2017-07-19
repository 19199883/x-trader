#ifndef __PENDING_SIG_PRODUCER_H__
#define __PENDING_SIG_PRODUCER_H__

#include <functional>
#include <array>
#include <libcork/core.h>
#include <libcork/helpers/errors.h>
#include <vrt.h>
#include "vrt_value_obj.h"
#include "signal_entity.h"

/*
 * 10 power of 2
 */
#define SIG_BUFFER_SIZE 32768 
#define CLOG_MODULE "PendingSigProducer" 

class PendingSigProducer
{
	public:
		PendingSigProducer(struct vrt_queue  *queue);
		~PendingSigProducer();

	private:
		int32_t push(const signal_t& sig);
		struct vrt_producer  *producer_;
		std::array<signal_t, SIG_BUFFER_SIZE> sig_buffer_;
};

#endif

