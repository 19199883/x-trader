#include <string.h>
#include <stdio.h>
#include "vrt_value_obj.h"
#include "compliance.h"
#include <tinyxml.h>
#include <tinystr.h>
#include <ctime>
#include <chrono>
#include <ratio>

Compliance::Compliance(): min_counter_(0), max_counter_(0),module_name_("Compliance")
{
	clog_warning("[%s] Compliance on.", module_name_);

	ParseConfig();

	memset(contracts_, 0, sizeof(contracts_));
	memset(cur_cancel_times_, 0, sizeof(cur_cancel_times_));

	memset(ord_buffer_, 0, sizeof(ord_buffer_));
	for(int i = 0; i < COUNTER_UPPER_LIMIT; i++){
		ord_buffer_[i].valid = false;
	}
}

void Compliance::Save()
{
	int i = 0;
	for(; i < MAX_CONTRACT_NUMBER; i++){
		if (IsEmptyString(contracts_[i])) break;

		clog_warning("[%s] contract:%s; cancel times:%d",
			module_name_, contracts_[i], cur_cancel_times_[i]);
	}
}

Compliance::~Compliance()
{
}

void Compliance::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    
    TiXmlElement *comp_node = RootElement->FirstChildElement("Compliance");
	if (comp_node != NULL){
		cancel_upper_limit_ = atoi(comp_node->Attribute("cancelUpperLimit"));
		clog_warning("[%s] cancelUpperLimit:%d;", module_name_, cancel_upper_limit_);
	} else { clog_error("[%s] x-trader.config error: Compliance node missing.", module_name_); }
}

int Compliance::GetCancelTimes(const char* contract)
{
	int i = 0;
	for(; i < MAX_CONTRACT_NUMBER; i++){
		if (IsEmptyString(contracts_[i])) break;

#ifdef ONE_PRODUCT_ONE_CONTRACT
		// 如果一个交易程序中一个品种只有一种合约，那么只需要比较品种部分即可
		if(IsEqualProduct((char*)contract, contracts_[i])){
#else
		if(strcmp(contract, contracts_[i]) == 0){
#endif
			return cur_cancel_times_[i];
		}
	}

	if(i < MAX_CONTRACT_NUMBER){
		strcpy(contracts_[i], contract);
	}

	return cur_cancel_times_[i];

}

bool Compliance::TryReqOrderInsert(int ord_counter, 
			const char * contract, 
			double price, 
			TX1FtdcBuySellTypeType side,
			TX1FtdcOpenCloseTypeType offset)
{
    bool ret = true;

	if(offset == X1_FTDC_SPD_OPEN && (GetCancelTimes(contract) >= cancel_upper_limit_))
	{
		char time[80];
		get_curtime(time,sizeof(time));
		clog_error("[%s][%s] rejected for cancel upper limit. "
					"ord counter:%d; cur times:%d ",
					module_name_, 
					time,
					ord_counter, 
					GetCancelTimes(contract));

		return false;
	}

	for (int i = min_counter_; i<= max_counter_; i++){
		OrderInfo& ord = ord_buffer_[i];
		if (!ord.valid) continue;

#ifdef ONE_PRODUCT_ONE_CONTRACT
		// 如果一个交易程序中一个品种只有一种合约，那么只需要比较品种部分即可
		if (IsEqualProduct((char*)ord.contract, (char*)contract) && side != ord.side)
		{
#else
		if (strcmp(ord.contract, contract)==0 && side != ord.side)
		{
#endif
			if ((side == X1_FTDC_SPD_BUY && (price + DOUBLE_CHECH_PRECISION) >= ord.price) || 
				(side != X1_FTDC_SPD_BUY && (price - DOUBLE_CHECH_PRECISION) <= ord.price))
			{
				ret = false;

				char time[80];
				get_curtime(time,sizeof(time));
				clog_error("[%s][%s] contract:%s; matched with myself. ord counter:%d; "
							"queue counter:%d ",
							module_name_, 
							time,
							contract,
							ord_counter, 
							i);
				break;
			}
		} // if (strcmp(ord.contract, contract)==0 && side != ord.side)
	} // for (int i = min_counter_; i< max_counter_; i++)

	if (ret)
	{
		if (0 == min_counter_) min_counter_ = ord_counter;
		max_counter_ = ord_counter;

		OrderInfo& ord = ord_buffer_[ord_counter];
		ord.valid = true;
		strcpy(ord.contract, contract);
		ord.side = side;
		ord.price = price;
	}

	clog_debug("[%s] TryReqOrderInsert ord counter:%d; min counter:%d; max counter:%d; ret:%d",
				module_name_, ord_counter, min_counter_, max_counter_, ret);

    return ret;
}


void Compliance::AccumulateCancelTimes(const char* contract)
{
	int i = 0;
	for(; i < MAX_CONTRACT_NUMBER; i++){
		if (IsEmptyString(contracts_[i])) break;

#ifdef ONE_PRODUCT_ONE_CONTRACT
		// 如果一个交易程序中一个品种只有一种合约，那么只需要比较品种部分即可
		if(IsEqualProduct((char*)contract, contracts_[i])){
#else
		if(strcmp(contract, contracts_[i]) == 0){
#endif
			cur_cancel_times_[i]++;
			return;
		}
	}

	if(i < MAX_CONTRACT_NUMBER){
		strcpy(contracts_[i], contract);
		cur_cancel_times_[i]++;
	}
}

void Compliance::End(int ord_counter)
{
	OrderInfo& ord = ord_buffer_[ord_counter];
	ord.valid = false;

	if (ord_counter == min_counter_ || !ord_buffer_[min_counter_].valid){
		int i = min_counter_;
		for(; i <= max_counter_; i++){
			OrderInfo& ord = ord_buffer_[i];
			if(ord.valid){
				min_counter_ = i;
				break;
			}
		} // for(int i = ord_counter + 1; i < max_counter_; i++)
		if(i == (max_counter_ + 1)){
			min_counter_ = max_counter_;
		}
	} // if (ord_counter == min_counter_)

	clog_debug("[%s] End min counter:%d; max counter:%d; ord counter:%d",
				module_name_, min_counter_, max_counter_, ord_counter);
}

