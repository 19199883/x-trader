#ifndef SELFBUYSELLCHECK_H_
#define SELFBUYSELLCHECK_H_

#include "check_schedule.h"

namespace SelfBuySellCheck
{
    // 初始化
    void Init();

	// 下单尝试，增加报单委托/增加 开仓计数
	RESULT_TYPE  TryReqOrderInsert(
	    const char * account,       // account
		OrderRefDataType order_ref,	// 报单引用
		const char * code, 			// 证券代码
		double price,				// 价格
		char price_flag,			// 价格类型
		char buy_sell_flag,			// 买卖方向
		char order_type,			// 报单类型
		OrderRefDataType * opposite_serial_no	// 【输出】导致自成交对手单的报单引用/当前开仓数
		);

    // 下单尝试，增加报单委托/增加 开仓计数（全部账号合并计算）
    RESULT_TYPE  TryReqOrderInsertMerge(
        const char * account,       // account
        OrderRefDataType order_ref, // 报单引用
        const char * code,          // 证券代码
        double price,               // 价格
        char price_flag,            // 价格类型
        char buy_sell_flag,         // 买卖方向
        char order_type,            // 报单类型
        OrderRefDataType * opposite_serial_no   // 【输出】导致自成交对手单的报单引用/当前开仓数
        );

    // 下单失败回报，删除该报单/回滚开仓计数
	void  OnOrderInsertFailed(
	    const char * account,       // account
		OrderRefDataType order_ref	// 报单引用
		);

	// 报单全成时，删除该报单
	void  OnOrderFilled(
	    const char * account,       // account
		OrderRefDataType order_ref	// 报单引用
		);

	// 报单撤单时，删除该报单/更新开仓计数
	void  OnOrderCanceled(
	    const char * account,       // account
		OrderRefDataType order_ref	// 报单引用
		);
}

#endif // SELFBUYSELLCHECK_H_
