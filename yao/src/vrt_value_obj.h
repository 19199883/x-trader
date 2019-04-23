#ifndef __VRT_VALUE_OBJ_H_
#define __VRT_VALUE_OBJ_H_
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

//#define LATENCY_MEASURE
// 行情持久化开关
// #define PERSISTENCE_ENABLED

/*
 * 行情UDP通讯设置成非阻塞模式
 */
//#define MD_UDP_NOBLOCK

#define COMPLIANCE_CHECK
#define CLOG_CHANNEL  "x-trader"
// 通过合约查找订阅该合约行情的方法:
// 1: unordered_multimap  
// 2: two-dimensional array
// 3: strcmp
#define FIND_STRATEGIES 3

// 一个trader支持最多信号数 
#define COUNTER_UPPER_LIMIT 15000



/*
 * 10 power of 2
 */
#define RPT_BUFFER_SIZE 15000

#define INVALID_PENDING_SIGNAL 999999

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
	#include <inttypes.h>		 
	#include <bowsprit.h>
	#include <libcork/core.h>
	#include <libcork/ds.h>
	#include <libcork/helpers/errors.h>
	#include <clogger.h>
	#include <vrt.h>

#ifdef __cplusplus
	};
#endif /* __cplusplus */

	/* --------------------------------------------------------------
	 * x-trader varon-t value and type
	 */

class Log
{
	public:
		static FILE *fp;/*文件指针*/
};

	enum HybridData {
		BESTANDDEEP = 0, 
		ORDERSTATISTIC_DATA,
		L1_MD, 
		L2_MD, 
		HybridData, 
		PENDING_SIGNAL, 
		TUNN_RPT, 
		TRADER_EOF, 
	};

	struct vrt_hybrid_value {
		struct vrt_value  parent;
		enum HybridData data;
		int32_t index;
	};

	static struct vrt_value *
	vrt_hybrid_value_new(struct vrt_value_type *type)
	{
		struct vrt_hybrid_value *self = (struct vrt_hybrid_value*)cork_new(struct vrt_hybrid_value);
		return &self->parent;
	}

	static void
	vrt_hybrid_value_free(struct vrt_value_type *type, struct vrt_value *vself)
	{
		struct vrt_hybrid_value *iself =
			cork_container_of(vself, struct vrt_hybrid_value, parent);
		free(iself);
	}

	static struct vrt_value_type  _vrt_hybrid_value_type = {
		vrt_hybrid_value_new,
		vrt_hybrid_value_free
	};

	static struct vrt_value_type *
	vrt_hybrid_value_type(void)
	{
		return &_vrt_hybrid_value_type;
	}


#endif
