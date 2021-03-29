#ifndef __VRT_VALUE_OBJ_H_
#define __VRT_VALUE_OBJ_H_
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

// socket RCVBUF size
#define RCV_BUF_SIZE	212992

/*
* DCE_DATA_FEED: 大连data feed组播行情
* DCE_OLD: 大连李杨时的旧行情(DCE_L2 if.txt)
 */
#define  DCE_OLD

/*
 * 锁仓，非锁仓开关.
 *  
 */
//#define LOCK_POSITION_ENABLE	// 开启锁仓  
#define LOCK_POSITION_DISNABLE	// 关闭锁仓  

// 延迟度量
//#define LATENCY_MEASURE
// 行情持久化开关
//#define PERSISTENCE_ENABLED

// software license
#define SERVER_NAME  "JRdl-test2"

// 通过合约查找订阅该合约行情的方法:
// 1: unordered_multimap  
// 2: two-dimensional array
// 3: strcmp
#define FIND_STRATEGIES 3

#define CLOG_CHANNEL  "x-trader"
// 一个trader支持最多信号数 
#define COUNTER_UPPER_LIMIT 15000

// 满足一天足够的下单量，以空间换时间
#define RPT_BUFFER_SIZE 15000

#define INVALID_PENDING_SIGNAL 999999

#ifdef PERSISTENCE_ENABLED 
	#define MAX_CONTRACT_COUNT 5120
#else
	#define MAX_CONTRACT_COUNT 1024
#endif

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

	class Log
	{
		public:
			static FILE *fp;/*文件指针*/
	};
	/* --------------------------------------------------------------
	 * x-trader varon-t value and type
	 */

	enum HybridData {
		BESTANDDEEP = 0, 
		ORDERSTATISTIC_DATA,
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
