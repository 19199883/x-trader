#ifndef __VRT_VALUE_OBJ_H_
#define __VRT_VALUE_OBJ_H_
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include "quote_datatype_shfe_deep.h"

#define gettid() syscall(__NR_gettid)

// 如果要支持INE的行情，需要定义INE_ENABLE宏
#define INE_ENABLE


/*
 * 如下宏根据情况进行定义
*/
// 行情持久化开关
//#define PERSISTENCE_ENABLED
//#define LATENCY_MEASURE

// 飞马极速行情
#define FEMAS_TOPSPEED_QUOTE
// 盛立极速行情 API multicast
//#define EES_UDP_TOPSPEED_QUOTE

// software license
#define SERVER_NAME  "zjtest1"

#define CLOG_CHANNEL  "x-trader"
// 通过合约查找订阅该合约行情的方法:
// 1: unordered_multimap  
// 2: two-dimensional array
// 3: strcmp
#define FIND_STRATEGIES 3
// 一个trader支持最多信号数 
#define COUNTER_UPPER_LIMIT 15000
#define INVALID_PENDING_SIGNAL 999999
// 满足一天足够的下单量，以空间换时间
#define RPT_BUFFER_SIZE 15000

#ifdef PERSISTENCE_ENABLED 
	#define MAX_CONTRACT_COUNT 2048
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

	class MDPackEx
	{
		public:
			MDPackEx(): damaged(false) { }

			MDPackEx(MDPack &cur_content): damaged(false)
			{
				this->content = cur_content;
			}

			MDPack content;
			bool damaged;
	};

	class Log
	{
		public:
			static FILE *fp;/*文件指针*/
	};

	/* --------------------------------------------------------------
	 * x-trader varon-t value and type
	 */

	enum HybridData {
		L1_MD = 0, 
		EFH_LEV2,
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

/*
 * 合约只需要比较品种部分和日期部分的后3位，如：rb1910,只需要比较rb和910
 */
bool IsEqualContract(char *contract1, char* contract2);


bool IsEmptyString(char *str);


void get_curtime(char buffer[],int size);

#endif
