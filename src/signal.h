#pragma once

#include <string>

using namespace std;

enum exchange_names
{
	// SHANGHAI FUTURES EXCHANGE
	XSGE = 'A',

	// CHINA FINANCIAL FUTURES EXCHANGE
	CCFX = 'G',

	// DALIAN COMMODITY EXCHANGE
	XDCE = 'B',

	// ZHENGZHOU COMMODITY EXCHANGE
	XZCE = 'C',

	/*
	 * Shanghai Gold Exchange
	 */
	XGE = 'D',

	// SHENZHEN STOCK EXCHANGE
	XSHE = '0',

	// SHANGHAI STOCK EXCHANGE
	XSHG = '1',

	//HONG KONG STOCK EXCHANGE
	XHKE = '2',

	EXCHG_SGX = 'S',
	
	undefined = 'u'

};

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


