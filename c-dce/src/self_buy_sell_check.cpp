#include "self_buy_sell_check.h"

#include <string>
#include <map>
#include <unordered_map>
#include <mutex>

using namespace std;

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

using namespace std;

#define DOUBLE_CHECH_PRECISION 0.000001

void SelfBuySellCheck::Init()
{
}

RESULT_TYPE SelfBuySellCheck::TryReqOrderInsert(
    OrderRefDataType order_ref,	// 报单引用
    const char * code, 			// 证券代码
    double price,				// 价格
    TX1FtdcBuySellTypeType side,			// 买卖方向
    OrderRefDataType * opposite_serial_no	// 【输出】导致自成交对手单的报单引用
    )
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


inline void RemoveTerminateOrder( OrderRefDataType order_ref)
{
    AccOrderMap::iterator it = s_orders.find(account);
    if (it == s_orders.end())
    {
        it = s_orders.insert(make_pair(account, MyOrderMap())).first;
    }
    MyOrderMap & my_orders = it->second;
    my_orders.erase(order_ref);
}

void SelfBuySellCheck::OnOrderInsertFailed( OrderRefDataType order_ref)
{
    return RemoveTerminateOrder(account, order_ref);
}

void SelfBuySellCheck::OnOrderFilled( OrderRefDataType order_ref)
{
    return RemoveTerminateOrder(account, order_ref);
}

void SelfBuySellCheck::OnOrderCanceled( OrderRefDataType order_ref)
{
    return RemoveTerminateOrder(account, order_ref);
}
