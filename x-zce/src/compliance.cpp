#include <string.h>
#include <stdio.h>
#include "vrt_value_obj.h"
#include "compliance.h"
#include <tinyxml.h>
#include <tinystr.h>

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
		if (strcmp(contracts_[i], "") == 0) break;

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
		if (strcmp(contracts_[i], "") == 0) break;

		if(strcmp(contract, contracts_[i]) == 0){
			return cur_cancel_times_[i];
		}
	}

	if(i < MAX_CONTRACT_NUMBER){
		strcpy(contracts_[i], contract);
	}

	return cur_cancel_times_[i];

}

bool Compliance::TryReqOrderInsert(int ord_counter, const char * contract,
			double price, TAPISideType side,TAPIPositionEffectType offset)
{
    bool ret = true;

	if(offset==TAPI_PositionEffect_OPEN && (GetCancelTimes(contract) >= cancel_upper_limit_)){
		clog_warning("[%s] rejected for cancel upper limit. ord counter:%d; cur times:%d ",
			module_name_, ord_counter, GetCancelTimes(contract));
		return false;
	}

	for (int i = min_counter_; i<= max_counter_; i++){
		OrderInfo& ord = ord_buffer_[i];
		if (!ord.valid) continue;

		if (strcmp(ord.contract, contract)==0 && side != ord.side){
			if ((side == TAPI_SIDE_BUY && (price + DOUBLE_CHECH_PRECISION) >= ord.price) || 
				(side != TAPI_SIDE_BUY && (price - DOUBLE_CHECH_PRECISION) <= ord.price)){
				ret = false;
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

	clog_info("[%s] TryReqOrderInsert ord counter:%d; min counter:%d; max counter:%d; ret:%d",
				module_name_, ord_counter, min_counter_, max_counter_, ret);

    return ret;
}

void Compliance::AccumulateCancelTimes(const char* contract)
{
	int i = 0;
	for(; i < MAX_CONTRACT_NUMBER; i++){
		if (strcmp(contracts_[i], "") == 0) break;

		if(strcmp(contract, contracts_[i]) == 0){
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

	clog_warning("[%s] End min counter:%d; max counter:%d; ord counter:%d",
				module_name_, min_counter_, max_counter_, ord_counter);
}

