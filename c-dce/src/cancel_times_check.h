#ifndef CANCELTIMESCHECK_H_
#define CANCELTIMESCHECK_H_

#include "check_schedule.h"

namespace CancelTimesCheck
{
	// 初始化
	void Init(
	    const char * account,       // account
		int warn_threthold,			// 撤单警告阈值
		int max_cancel_times		// 最大撤单次数
		);

    // 撤单次数（合约当前的撤单次数）
    void ComplianceCheck_SetCancelTimes(
        const char * account,       // account
        const char * code,          // 证券代码
        int cancel_times            // 撤单次数
        );

	// 撤单尝试，增加该品种撤单计数
	RESULT_TYPE TryReqOrderAction(
	    const char * account,       // account
		const char * code, 			// 证券代码
		OrderRefDataType order_ref  // 报单引用
		);

	// 撤单失败，减少该品种撤单次数
	void OnOrderCancelFailed(
	    const char * account,       // account
		const char * code, 			// 证券代码
		OrderRefDataType order_ref  // 报单引用
		);

    // 撤单successful
    void OnOrderCanceled(
        const char * account,       // account
        const char * code,          // 证券代码
        OrderRefDataType order_ref  // 报单引用
        );

	// 达到撤单告警阈值后，不允许开仓
	bool ReachWarnThreshold(
	    const char * account,       // account
		const char * code 			// 证券代码
		);
}

#endif // CANCELTIMESCHECK_H_
