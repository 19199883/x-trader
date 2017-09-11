#include <string.h>
#include <stdio.h>
#include "vrt_value_obj.h"
#include "compliance.h"

Compliance::Compliance(): min_counter_(0), max_counter_(0),module_name_("Compliance")
{
	clog_info("[%s] Compliance on.", module_name_);

	memset(ord_buffer_, 0, sizeof(ord_buffer_));
	for(int i = 0; i < COUNTER_UPPER_LIMIT; i++){
		ord_buffer_[i].valid = false;
	}
}

bool Compliance::TryReqOrderInsert(int ord_counter, const char * contract,
			double price, TUstpFtdcDirectionType side)
{
    bool ret = true;

	for (int i = min_counter_; i<= max_counter_; i++){
		OrderInfo& ord = ord_buffer_[i];
		if (!ord.valid) continue;

		if (strcmp(ord.contract, contract)==0 && side != ord.side){
			if ((side == USTP_FTDC_D_Buy && (price + DOUBLE_CHECH_PRECISION) >= ord.price) || 
				(side != USTP_FTDC_D_Buy && (price - DOUBLE_CHECH_PRECISION) <= ord.price)){
				ret = false;
				clog_debug("[%s] matched with myself. ord counter:%d; queue counter:%d ",
					module_name_, ord_counter, i);
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

	clog_debug("[%s] TryReqOrderInsert ord counter:%d; min counter:%d; max counter:%d; ret:%d",
				module_name_, ord_counter, min_counter_, max_counter_, ret);

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

	clog_debug("[%s] End min counter:%d; max counter:%d; ord counter:%d",
				module_name_, min_counter_, max_counter_, ord_counter);
}

