#pragma once

#include <string>
#include "exchange_names.h"

using namespace std;
using namespace quote_agent;

enum if_sig_state_t {
	SIG_STATUS_SUCCESS = 0,

	/* 报单委托成功 */
	SIG_STATUS_ENTRUSTED,

	SIG_STATUS_PARTED,
	SIG_STATUS_CANCEL,
	SIG_STATUS_REJECTED,
};

enum  signal_act_t
{
	buy 			= 1,
	sell 		= 2,
	cancel 		= 4,
	quote 		= 8,
	undefined 	= -1,
};

enum instr_t
{
	ROD = 0,
	FOK = 1,
	FAK = 2,
	MARKET = 3,
	QUOTE = 4,
	REQUEST_OF_QUOTE = 5,
	instr_undefined = -1,
};

enum cancel_pattern_t {
	SIG_CANCEL_MODE_SYNC 	= 0,
	SIG_CANCEL_MODE_ASYNC	= 1,
};

enum alloc_position_effect_t {
	open_	= 1,
	close_	= 2,
	close_and_open =3,
};

class signal_resp_t{
public:
	struct timeval exchg_timestamp; 	/* 回报时间 */
	struct timeval local_timestamp; 	/* 本地时间 */
	unsigned long sig_id; 				/* 信号id */
	char symbol[64]; 					/* 合约号 */
	unsigned short sig_act; 			/* action */
	int order_volume; 					/* 此信号的总委托量 */
	double order_price; 				/* 此信号的委托价格 */
	double exec_price; 					/* 当前成交价格 */
	int exec_volume; 					/* 当前成交量 */
	double aver_price; 					/* 平均成交价格 */
	int acc_volume; 					/* 累计成交量 */
	int status; 						/* 成交状态 */
	int ack; 							/* 提交数量 */
	int killed; 						/* 撤销数量 */
	int rejected; 						/* 拒绝数量 */
	int error_no; 						/* 错误号 */
} ;

/*
 * if the field "instr" is equal to  QUOTE, the value of open_volume field is set to
 * the buy_volume field of quote order and the value close_volume field is set to the sell_volume field of quote order
 */
class signal_t {
public:
	/* 信号id */
	unsigned long sig_id;

	/*
	 * the signal id to cancel.
	 * it is valid only if sig_act==cancel
	 */
	unsigned long orig_sig_id;

	/*
	 * 0:synchronously execute canceling order signal
	 * 1:asynchronously execute canceling order signal
	 */
	cancel_pattern_t cancel_pattern;

	/* 策略id */
	int st_id;

	/* exchange code,the detail is the following:
	 * 'A': SHANGHAI FUTURES EXCHANGE，上海期货交易所
	 * 'G': CHINA FINANCIAL FUTURES EXCHANGE,中国金融期货交易所
	 * 'B': DALIAN COMMODITY EXCHANGE，大连商品交易所
	 * 'C': ZHENGZHOU COMMODITY EXCHANGE， 郑州商品交易所
	 * '0': SHENZHEN STOCK EXCHANGE，深圳股票交易所
	 * '1': SHANGHAI STOCK EXCHANGE，上海股票交易所
	 */
	exchange_names exchange;

	/* 合约号 */
	char symbol[64];

	/* 数量 */
	int open_volume;

	/* 价格 */
	double buy_price;

	/* 数量 */
	int close_volume;

	/* 价格 */
	double sell_price;

	/* 买/卖/取消 */
	unsigned short int sig_act;

	unsigned short sig_openclose; /* 开平标识，1表⽰示开；2表⽰示平;3 close and then open */

	/* FAK/FOK/MARKET/ROD/QUOTE */
	unsigned short int instr;

	/* 此信号是否参与内部撮合，0表⽰示 不参与，1表⽰示在某⼀一时间段参与内部撮合. */
	unsigned short internal_match;

	/* 做市商结构回复需要⼀一个长度为21字节的序列号 */
	char reply_quote_id[21];
} ;


