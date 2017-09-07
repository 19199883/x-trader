#include <string.h>
#include <stdio.h>
#include "compliance.h"

Compliance::Compliance(): min_counter_(0), max_counter_(0)
{
	memset(ord_buffer_, 0, sizeof(ord_buffer_));
	for(int i = 0; i < COUNTER_UPPER_LIMIT; i++){
		ord_buffer_[i].valid = false;
	}
}

bool Compliance::TryReqOrderInsert(int ord_counter, const char * contract,
			double price, TX1FtdcBuySellTypeType side)
{
    bool ret = true;

	for (int i = min_counter_; i<= max_counter_; i++){
		OrderInfo& ord = ord_buffer_[i];
		if (!ord.valid) continue;

		if (strcmp(ord.contract, contract)==0 && side != ord.side){
			if ((side == X1_FTDC_SPD_BUY && (price + DOUBLE_CHECH_PRECISION) >= ord.price) || 
				(side != X1_FTDC_SPD_BUY && (price - DOUBLE_CHECH_PRECISION) <= ord.price)){
				ret = false;

				TNL_LOG_WARN("OrderRef(%lld) of Acc(%s) maybe matched with OrderRef(%lld) of Acc(%s) ",
					order_ref, account, value.first, account);
				break;
			}
		} // if (strcmp(ord.contract, contract)==0 && side != ord.side)
	} // for (int i = min_counter_; i< max_counter_; i++)

	if (ret){
		if (0 == min_counter_) min_counter_ = ord_counter;
		max_counter_ = ord_counter;

		OrderInfo& ord = ord_buffer_[ord_counter];
		ord.valid = true;
		strcmp(ord.contract, contract);
		ord.side = side;
		ord.price = price;
	}

    return ret;
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
}

