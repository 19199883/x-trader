#pragma once

#include "../my_exchange.h"
#include "exchange_names.h"
#include "quotecategoryoptions.h"

using namespace quote_agent;

class ContractInfo_t {
public:
	 char  contract_code[64]; // 合约代码
	 char    TradeDate[9];    // 交易日
	 char    ExpireDate[9];   // 到期日
};


class acc_volume_t {
public:
	/* 合约名称 */
	char symbol[64];

	/* 买量 */
	int buy_volume;

	/* 买平均价格 */
	double buy_price;

	/* 卖量 */
	int sell_volume;

	/* 卖平均价格 */
	double sell_price;

	unsigned char exchg_code; // 代表交易所代码
} ;

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

	/* 每秒最⼤大订单数 */
	int orders_limit_per_sec;

	/* 每天最⼤大撤单数 */
	int cancel_limit_per_day;

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

	/* 每秒最⼤大订单数 */
	int orders_limit_per_sec;

	// /* 每天最⼤大撤单数 */
	int cancel_limit_per_day;

	/* 已配置合约的数量 */
	int symbols_cnt;

	/* 配置的合约清单
	 * fixed length array and specific contract number
	 * is specified by symbols_cnt member.
	 */
	symbol_t symbols[2000];
	int log_flags;
	int contract_info_count;
	ContractInfo_t contracts[2000];
} ;
