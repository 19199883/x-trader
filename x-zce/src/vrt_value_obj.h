#ifndef __VRT_VALUE_OBJ_H_
#define __VRT_VALUE_OBJ_H_
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

// 如果定义了，则使用UDP下单操作；否则使用TCP下单操作
#define UPD_ORDER_OPERATION

//#define LATENCY_MEASURE
// 行情持久化开关
//#define PERSISTENCE_ENABLED

// software license
#define SERVER_NAME  "zztest3"

#define CLOG_CHANNEL  "x-trader"
// 通过合约查找订阅该合约行情的方法:
// 1: unordered_multimap  
// 2: two-dimensional array
// 3: strcmp
#define FIND_STRATEGIES 3

// 一个trader支持最多信号数 
#define COUNTER_UPPER_LIMIT 15000

#ifdef PERSISTENCE_ENABLED 
	#define MAX_CONTRACT_COUNT 1000
#else
	#define MAX_CONTRACT_COUNT 25
#endif


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
		L1_MD = 0, 
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

bool IsEmptyString(char *str);
bool IsEqualContract(char *contract1, char* contract2);

void get_curtime(char buffer[],int size);

#endif
