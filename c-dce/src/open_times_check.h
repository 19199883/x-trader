#ifndef OPENTIMESCHECK_H_
#define OPENTIMESCHECK_H_

#include "check_schedule.h"

namespace OpenTimesCheck
{
	// 初始化
	void Init(
	    const char * account,       // account
		int max_open_speculate,		// 最大投机手数
		int max_open_arbitrage,		// 最大套利手数
		int max_open_total			// 最大总手数
		);

	// 下单尝试，增加 开仓计数
	RESULT_TYPE  TryReqOrderInsert(
	    const char * account,       // account
		char exchange_code,			// 交易所编码
        const char * code,          // 证券代码
		int volumn, 				// 数量
		char hedge_flag,			// 投机套保类型
		char open_close_flag,		// 开平方向
		OrderRefDataType * opened_count			// 【输出】导致自成交对手单的报单引用/当前开仓数
		);

	// 下单失败回报，删除该报单/回滚开仓计数
	void  OnOrderInsertFailed(
	    const char * account,       // account
		char exchange_code,			// 交易所编码
        const char * code,          // 证券代码
		int volumn, 				// 数量
		char hedge_flag,			// 投机套保类型
		char open_close_flag		// 开平方向
		);

	// 报单撤单时，删除该报单/更新开仓计数
	void  OnOrderCanceled(
	    const char * account,       // account
		char exchange_code,			// 交易所编码
        const char * code,          // 证券代码
		int canceled_volumn,		// 撤单数量
		char hedge_flag,			// 投机套保类型
		char open_close_flag		// 开平方向
		);
}

#endif // OPENTIMESCHECK_H_
