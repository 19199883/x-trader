Compliance::Compliance():
	min_counter_(0), max_counter_(0)
{
}

int Compliance::TryReqOrderInsert(int ord_counter, const char * code,
			double price,	TX1FtdcBuySellTypeType side)
{
    int ret = COMPLIANCE_CHECK_RESULT_SUCCESS;

	AccOrderMap::iterator it = s_orders.find(account);
	if (it == s_orders.end()){
		it = s_orders.insert(make_pair(account, MyOrderMap())).first;
	}
	MyOrderMap & my_orders = it->second;
	for (MyOrderMapValue &value : my_orders){
		// 相同合约，方向相反
		if (value.second.code == code && buy_sell_flag != value.second.side){
			if ((side == X1_FTDC_SPD_BUY && (price + DOUBLE_CHECH_PRECISION) >= value.second.price) || 
				(side != X1_FTDC_SPD_BUY && (price - DOUBLE_CHECH_PRECISION) <= value.second.price)){
				ret = COMPLIANCE_CHECK_RESULT_SELFTRADE;
				*opposite_serial_no = value.first;
				TNL_LOG_WARN("OrderRef(%lld) of Acc(%s) maybe matched with OrderRef(%lld) of Acc(%s) ",
					order_ref, account, value.first, account);
				break;
			}
		}
	}

	if ((ret == COMPLIANCE_CHECK_RESULT_SUCCESS)
		&& (my_orders.find(order_ref) == my_orders.end())){
		my_orders.insert(std::make_pair(order_ref, OrderInfo(code, side, price)));
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
