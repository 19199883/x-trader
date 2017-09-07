#ifndef SELFBUYSELLCHECK_H_
#define SELFBUYSELLCHECK_H_

#include "X1FtdcApiDataType.h"

#define DOUBLE_CHECH_PRECISION 0.000001
// 一个trader支持最多信号数 
#define COUNTER_UPPER_LIMIT 20000

struct OrderInfo
{
	bool valid;
    char contract[10];
    TX1FtdcBuySellTypeType side;
    double price;
};

class Compliance
{
	public:
		Compliance();
		//
		/*
		 * 返回值：true表示成功；false表示失败 
		 */
		bool TryReqOrderInsert(int ord_counter, const char * contract, double price,
					TX1FtdcBuySellTypeType side);

		void End(int ord_counter);

	private:
		// 记录未终结的中最小的counter
		int min_counter_;
		// 记录未终结的中最大的counter
		int max_counter_;
		// 存委托单信息，用于检查自成交。只有valid==true，才表示是有效的数据。
		// 存储方法：LocalOrderID的counter部分作为数组下标进行存储
		OrderInfo ord_buffer_[COUNTER_UPPER_LIMIT];
		const char * module_name_;  
};

#endif // SELFBUYSELLCHECK_H_
