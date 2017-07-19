#pragma once

#include <string>
#include <sstream>
#include "exchange_names.h"
#include "quotecategoryoptions.h"
#include "my_order.h"
#include <set>
#include "model_config.h"

using namespace quote_agent;
using namespace std;
using namespace trading_channel_agent;

enum POS_DIRECT {
	UNKNOWN = 0,
	POS_BUY = 1,
	POS_SELL = 2,
};

class symbol_pos_t {
public:
	/* 合约名称 */
	char symbol[64];

	/* 多仓仓位 */
	int long_volume;

	/* 多仓成交平均价格 */
	double long_price;

	/* 空仓仓位 */
	int short_volume;

	double short_price;

	/*
	 * 1: position changed
	 * 0:position did NOT change
	 */
	int changed;

	int today_buy_volume; /* 今天的总买量 */
	double today_aver_price_buy; /* 今天的买平均价格 */
	int today_sell_volume; /* 今天的总卖量 */
	double today_aver_price_sell; /* 今天卖平均价格 */

	unsigned char exchg_code; // 代表交易所代码

public:
	symbol_pos_t(){
		memset(symbol,'\0',64);
		long_volume = 0;
		long_price = 0.0;
		short_volume = 0;
		short_price = 0.0;
		today_buy_volume = 0;
		today_aver_price_buy = 0;
		today_sell_volume = 0;
		today_aver_price_sell = 0;
	}

	static void reset(symbol_pos_t &pos){
		memset(pos.symbol,'\0',64);
		pos.long_volume = 0;
		pos.long_price = 0.0;
		pos.short_volume = 0;
		pos.short_price = 0.0;
		pos.today_buy_volume = 0;
		pos.today_aver_price_buy = 0;
		pos.today_sell_volume = 0;
		pos.today_aver_price_sell = 0;
	}
};

class position_t{
public:
	position_t()
	{
		symbol_cnt = 0;
	}
	int symbol_cnt;

	/*
	 * fixed length: 10000];.
	 * specific element number is given by symbol_cnt member.
	 */
	symbol_pos_t s_pos[10000];
} ;

class strategy_init_pos_t {
public:
	strategy_init_pos_t()
	{
		acc_cnt = 0;
	}

	/* 策略名称 */
	char _name[64];

	/* 昨⽇日持仓 */
	position_t _yesterday_pos;

	/* 今天持仓 */
	position_t _today_pos;

	int acc_cnt;
	/* 今⽇日买卖统计量	*/
	acc_volume_t _today_acc_volume[10000];
 } ;

class order_request_t {
public:
	/* 合约名称 */
	char symbol[64];

	/* 挂单价格 */
	double price;

	/* 未成交量 */
	int volume;

	/* 卖买⽅方向, ‘0’表⽰示买，’1’表⽰示卖 */
	int direct;
} ;

class pending_order_t {
public:
	/* pending order的数量 */
	int req_cnt;

	/* pending order */
	order_request_t pending_req[10000];
} ;
