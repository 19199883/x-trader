#pragma once
#include <stdio.h>
#include <string.h>
#include <string>

using namespace std;

struct StrategyLog1{

};

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

class symbol_t {
public:
	/* exchange code,the detail is the following:
	 * 'A': SHANGHAI FUTURES EXCHANGE，上海期货交易所
	 * 'G': CHINA FINANCIAL FUTURES EXCHANGE,中国金融期货交易所
	 * 'B': DALIAN COMMODITY EXCHANGE，大连商品交易所
	 * 'C': ZHENGZHOU COMMODITY EXCHANGE， 郑州商品交易所
	 * '0': SHENZHEN STOCK EXCHANGE，深圳股票交易所
	 * '1': SHANGHAI STOCK EXCHANGE，上海股票交易所
	 */
	exchange_names exchange;

	// contract name
	char name[64];

	/* 最⼤大持仓 */
	int max_pos;

	/* 基于合约记录⽂文件id。*/
	int symbol_log_id;

	/* 基于合约记录⽂文件名；含⽬目录。*/
	char symbol_log_name[256];
};

class st_config_t{
public:
	/* 策略id，策略的唯⼀一标志。*/
	int st_id;

	/* 策略名字。? */
	char st_name[64];

	/* 策略记录⽂文件id。
	 * model log file id in Fortran language.
	 */
	int log_id;

	/* 策略记录⽂文件名；含⽬目录。
	 * model log file path,not including file name.
	 */
	char log_name[256];

	/* 中间变量记录⽂文件id。
	 * the file id which is used to record intermediate
	 * variables by model.
	 */
	int iv_id;

	/* 中间变量记录⽂文件名，含⽬目录
	 * the file name,including path,which is
	 *  used to record intermediate variables by model.
	 */
	char iv_name[256];

	/* 外部变量记录⽂文件id。
	 * the file id which is used to record external
	 * variables by model.
	 */
	int ev_id;

	/* 外部变量记录⽂文件名，含⽬目录。
	 * the file name,including path,which is
	 *  used to record external variables by model.
	 */
	char ev_name[256];

	/* 已配置合约的数量 */
	int symbols_cnt;

	/* 配置的合约清单
	 * fixed length array and specific contract number
	 * is specified by symbols_cnt member.
	 */
	// TODO:
	// symbol_t symbols[50];
	symbol_t symbols[5];
} ;

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

	unsigned char exchg_code; // 代表交易所代码

public:
	symbol_pos_t(){
		memset(symbol,'\0',64);
		long_volume = 0;
		long_price = 0.0;
		short_volume = 0;
		short_price = 0.0;
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
	symbol_pos_t s_pos[10];
} ;

class strategy_init_pos_t {
public:
	strategy_init_pos_t()
	{
	}

	/* 策略名称 */
	char _name[64];

	/* 今天持仓 */
	position_t _cur_pos;
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
};

enum instr_t
{
	ROD = 0,
	FOK = 1,
	FAK = 2,
	MARKET = 3,
	instr_undefined = -1,
};

enum alloc_position_effect_t {
	open_	= 1,
	close_	= 2,
};

class signal_resp_t{
public:
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
} ;


