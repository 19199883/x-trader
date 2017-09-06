#include "self_buy_sell_check.h"

#include <string>
#include <map>
#include <unordered_map>
#include <mutex>
#include <pthread.h>

#include "const_define.h"
#include "my_cmn_log.h"
#include "my_cmn_util_funcs.h"

using namespace std;

struct OrderInfo
{
	bool valid;
    std::string code;
    char buy_sell_flag;
    double price;

    OrderInfo(const char* code_, char buy_sell_flag_, double price_)
        : code(code_)
    {
		valid = false;
        buy_sell_flag = buy_sell_flag_;
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
    //pthread_spin_init(&s_self_match_mutex, 0);
}

RESULT_TYPE SelfBuySellCheck::TryReqOrderInsert(
    const char * account,       // account
    OrderRefDataType order_ref,	// 报单引用
    const char * code, 			// 证券代码
    double price,				// 价格
    char price_flag,			// 价格类型
    char buy_sell_flag,			// 买卖方向
    char order_type,			// 报单类型
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
		if (value.second.code == code && buy_sell_flag != value.second.buy_sell_flag){
			//// 现在高价买入，或低价卖出 (有非对价下单，需要判断更好价格，modified by chenyongshun 20131220)
			if ((buy_sell_flag == CONST_ENTRUST_BUY && (price + DOUBLE_CHECH_PRECISION) >= value.second.price)
				|| (buy_sell_flag != CONST_ENTRUST_BUY && (price - DOUBLE_CHECH_PRECISION) <= value.second.price)){
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
		my_orders.insert(std::make_pair(order_ref, OrderInfo(code, buy_sell_flag, price)));
	}

    return ret;
}


inline void RemoveTerminateOrder(const char * account, OrderRefDataType order_ref)
{
    AccOrderMap::iterator it = s_orders.find(account);
    if (it == s_orders.end())
    {
        it = s_orders.insert(make_pair(account, MyOrderMap())).first;
    }
    MyOrderMap & my_orders = it->second;
    my_orders.erase(order_ref);
}

void SelfBuySellCheck::OnOrderInsertFailed(
    const char * account,       // account
    OrderRefDataType order_ref  // 报单引用
    )
{
    return RemoveTerminateOrder(account, order_ref);
}

void SelfBuySellCheck::OnOrderFilled(
    const char * account,       // account
    OrderRefDataType order_ref  // 报单引用
    )
{
    return RemoveTerminateOrder(account, order_ref);
}

void SelfBuySellCheck::OnOrderCanceled(
    const char * account,       // account
    OrderRefDataType order_ref  // 报单引用
    )
{
    return RemoveTerminateOrder(account, order_ref);
}
