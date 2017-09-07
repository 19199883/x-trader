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


void Compliance::RemoveTerminateOrder(int ord_counter)
{
    AccOrderMap::iterator it = s_orders.find(account);
    if (it == s_orders.end())
    {
        it = s_orders.insert(make_pair(account, MyOrderMap())).first;
    }
    MyOrderMap & my_orders = it->second;
    my_orders.erase(order_ref);
}

void Compliance::OnOrderInsertFailed(int ord_counter)
{
    return RemoveTerminateOrder(account, order_ref);
}

void Compliance::OnOrderFilled( int ord_counterOrder)
{
    return RemoveTerminateOrder(account, order_ref);
}

void Compliance::OnOrderCanceled( int ord_counterOrderOrder)
{
    return RemoveTerminateOrder(account, order_ref);
}
