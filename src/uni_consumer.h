#ifndef __UNI_CONSUMER_H__
#define __UNI_CONSUMER_H__

#include <functional>
#include <array>
#include <libcork/core.h>
#include <libcork/helpers/errors.h>
#include <vrt.h>
#include "vrt_value_obj.h"

#define STRA_TABLE_SIZE 512 
#define CLOG_MODULE "uni_consumer" 

class UniConsumer
{
	public:

	private:
		struct vrt_consumer *consumer_;
		std::array<MDBestAndDeep_MY, STRA_TABLE_SIZE> stra_table_;
};

#endif
