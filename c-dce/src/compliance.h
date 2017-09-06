#ifndef SELFBUYSELLCHECK_H_
#define SELFBUYSELLCHECK_H_

#include "X1FtdcApiDataType.h"

#define DOUBLE_CHECH_PRECISION 0.000001
// 一个trader支持最多信号数 
#define COUNTER_UPPER_LIMIT 20000


struct OrderInfo
{
	bool valid;
    std::string code;
    TX1FtdcBuySellTypeType side;
    double price;

    OrderInfo(const char* code_, TX1FtdcBuySellTypeType side_, double price_)
        : code(code_)
    {
		valid = false;
        side = side_;
        price = price_;
    }
};

// 定义别名，方便更换
typedef unordered_map<OrderRefDataType, OrderInfo> MyOrderMap;
typedef unordered_map<string, MyOrderMap> AccOrderMap;
typedef AccOrderMap::value_type AccOrderMapValue;
typedef MyOrderMap::value_type MyOrderMapValue;
typedef MyOrderMap::iterator MyOrderMapIt;

static AccOrderMap s_orders;

class Compliance
{
	public:
		Compliance();
		//
		// 下单尝试，增加报单委托/增加 开仓计数
		int TryReqOrderInsert(int ord_counter, const char * code, double price,
					TX1FtdcBuySellTypeType side);

		// 下单失败回报，删除该报单/回滚开仓计数
		void  OnOrderInsertFailed(int ord_counter);

		// 报单全成时，删除该报单
		void  OnOrderFilled(int ord_counter);

		// 报单撤单时，删除该报单/更新开仓计数
		void  OnOrderCanceled(int ord_counter);

	private:
		// 记录未终结的中最小的counter
		int min_counter_;
		// 记录未终结的中最大的counter
		int max_counter_;
		// 存委托单信息，用于检查自成交。只有valid==true，才表示是有效的数据。
		// 存储方法：LocalOrderID的counter部分作为数组下标进行存储
		OrderInfo ord_buffer_[COUNTER_UPPER_LIMIT];
		
};

#endif // SELFBUYSELLCHECK_H_
