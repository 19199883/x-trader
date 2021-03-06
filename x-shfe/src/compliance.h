﻿#ifndef SELFBUYSELLCHECK_H_
#define SELFBUYSELLCHECK_H_

#include "USTPFtdcUserApiDataType.h"
#include "vrt_value_obj.h"

#define DOUBLE_CHECH_PRECISION 0.000001

#define MAX_CONTRACT_NUMBER 16

struct OrderInfo
{
	bool valid;
    char contract[7];
    TUstpFtdcDirectionType  side;
    double price;
};

class Compliance
{
	public:
		Compliance();
		virtual ~Compliance();
		//
		/*
		 * 返回值：true表示成功；false表示失败 
		 */
		bool TryReqOrderInsert(int ord_counter, const char * contract, double price,
					TUstpFtdcDirectionType side,TUstpFtdcOffsetFlagType offset);

		void End(int ord_counter);
		void Save();
		void AccumulateCancelTimes(const char* contrace);

		int GetMinCounter()		
		{
			return min_counter_;
		}

		int GetMaxCounter()		
		{
			return max_counter_;
		}
		OrderInfo *GetOrdBuffer()
		{
			return ord_buffer_;
		}
		char **GetContracts()
		{
			return (char **)contracts_;
		}
		int *GetCurCancelTimes()
		{
			return cur_cancel_times_;
		}
		int GetCancelTimes(const char* contract);
		char contracts_[MAX_CONTRACT_NUMBER][12];
	private:
		void ParseConfig();

		// 记录未终结的中最小的counter
		int min_counter_;
		// 记录未终结的中最大的counter
		int max_counter_;
		// 存委托单信息，用于检查自成交。只有valid==true，才表示是有效的数据。
		// 存储方法：LocalOrderID的counter部分作为数组下标进行存储
		OrderInfo ord_buffer_[COUNTER_UPPER_LIMIT];
		const char * module_name_;  

		int cancel_upper_limit_;
		/*
		 * contracts存储不同的合约，合约在contracts的下标记为:n,
		 * 那么cur_cancel_times_[n]用于记录该合约累计撤单数 
		 */
		int cur_cancel_times_[MAX_CONTRACT_NUMBER];
};

#endif // SELFBUYSELLCHECK_H_
