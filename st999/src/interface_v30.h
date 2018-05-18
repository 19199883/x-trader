#ifndef INTERFACE_V30_H
#define INTERFACE_V30_H

#include "stdbool.h"

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

	// SHENZHEN STOCK EXCHANGE
	XSHE = '0',

	// SHANGHAI STOCK EXCHANGE
	XSHG = '1',
	XHKE = '2',

	EXCHG_SGX = 'S',

	undefined = 'u'

};

struct	symbol_t
{
    /*exchange code,the detail is the following:
	 * 'A': SHANGHAI FUTURES EXCHANGE
	 * 'G': CHINA FINANCIAL FUTURES EXCHANGE
	 * 'B': DALIAN COMMODITY EXCHANGE
	 * 'C': ZHENGZHOU COMMODITY EXCHANGE
	 * '0': SHENZHEN STOCK EXCHANGE
	 * '1': SHANGHAI STOCK EXCHANGE
	 */
	enum exchange_names	exch;
	char	ticker[64];//contract name
	int	max_pos;
	int	ticker_log_id;
	char ticker_log_name[256];
};

//struct ContractInfo_t {
//	 char  contract_code[64];
//	 char    TradeDate[9];
//	 char    ExpireDate[9];
//};

struct st_config_t
{

	int	strategy_id;    //strategy id
	char	strategy_name[64];  //strategy name;

	int	log_id;     // model log file id in Fortran
	char	log_name[256];  /* model log file path,not including file name */

	int	intvariable_id;   /* the file id which is used to record intermediate variables by model. */
	char	intvariable_name[256];  /* the file name,including path,which is
                                    used to record intermediate variables by model.*/
	int	extvariable_id;     /* the file id which is used to record external variables by model.*/
	char	extvariable_name[256]; /* the file name,including path,which is
                                    used to record external variables by model. */
	//int	order_lmt_per_second;      /* order limit per second */
	//int	order_cancel_lmt_per_day;   /* order cancel limit per day*/

	int	ticker_count;          /* number of contracts */
	struct symbol_t	ticker_detail[50]; /* fixed length array and specific contract number
                                                    is specified by ticker_count*/
    //int log_flags;
	//int contract_info_count;
	//struct ContractInfo_t contracts[2000];
};

struct	symbol_pos_t
{
	char	ticker[64];
	int	long_volume;
	double	long_price;
	int	short_volume;
	double	short_price;
	int	pos_change; //1: pos changed; 0: position not change

	unsigned char exchg_code;
};


struct position_t
{
	int	symbol_cnt;
	struct symbol_pos_t	pos[10];
};


struct	strategy_init_pos_t
{
	char st_name[64];
	struct position_t	cur_pos;
};

//enum cancel_pattern_t {
//	SIG_CANCEL_MODE_SYNC 	= 0,
//	SIG_CANCEL_MODE_ASYNC	= 1,
//};

struct signal_t
{
	unsigned long	sig_id;
	unsigned long cancel_sig_id; //it is valid only if sig_act==cancel

	/*
	 * 0:synchronously execute canceling order signal
	 * 1:asynchronously execute canceling order signal
	 */
	//enum cancel_pattern_t cancel_mode;

	int	st_id;
	/* exchange code,the detail is the following:
	 * 'A': SHANGHAI FUTURES EXCHANGE，上海期货交易所
	 * 'G': CHINA FINANCIAL FUTURES EXCHANGE,中国金融期货交易所
	 * 'B': DALIAN COMMODITY EXCHANGE，大连商品交易所
	 * 'C': ZHENGZHOU COMMODITY EXCHANGE， 郑州商品交易所
	 * '0': SHENZHEN STOCK EXCHANGE，深圳股票交易所
	 * '1': SHANGHAI STOCK EXCHANGE，上海股票交易所
	 */
	enum exchange_names	exch;
	char	ticker[64];
	int	open_volume;
	double	buy_price;
	int	close_volume;
	double	sell_price;
	unsigned short int	sig_action; //buy sell, cancel
	unsigned short sig_openclose;
	unsigned short int instruction; /* FAK/FOK/MARKET/ROD/QUOTE */
	//unsigned short internal_match;
	//char mm_order_id[21];
};

enum {
     SIG_ACT_NULL=0x00,
     SIG_ACT_BUY = 0x01,
     SIG_ACT_SELL = 0x02,
     SIG_ACT_CANCEL = 0x04,
     //SIG_ACT_QUOTE = 0x08,
     SIG_ACT_REQOFQUOTE = 0x0A,
     SIG_ACT_CLOSE = 0x10,
     SIG_ACT_OPEN = 0x20
};

enum instr_t
{
	ROD = 0,
	FOK = 1,
	FAK = 2,
	MARKET = 3,
	instr_undefined = -1,
};

struct signal_resp_t
{
	unsigned long	sig_id;
	char ticker[64];
	unsigned short	sig_act;
	int	order_volume;
	double	order_price;
	double	exe_price;
	int	exe_volume;
	double	avg_price;
	int	cum_volume;
	int	status;
	int	ack;
	int	cancelled;
	int	rejected;
	int	error_no;
};


struct cStruct_local_quote
{
  long long time;
  char  contract[64];
  int  curr_volume;
  int  b_total_volume;
  int  s_total_volume;
  double  last_price;
  double  b_open_interest;
  double  s_open_interest;
  double  buy_price[30];
  int   buy_vol[30];
  double  sell_price[30];
  int   sel_vol[30];
};

struct in_local_data
{
	int iTime, iCurrVol, iBuyTotVol, iSelTotVol, iBidVol[30], iAskVol[30];
	char cContract[64], cDirection;
	double rLastP, rLongPos, rShortPos, rBidPrice[30], rAskPrice[30] ;
};

struct	cStruct_quote_data
{
	char ticker[64], serial_no[21], exch_tv[9];
};

//////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////



//struct in_local_data gstCurrInLocalData, gstPreInLocalData;

//int giInternalQuoteFlg;


void st_feed_localquote_(struct cStruct_local_quote *lstStructIn, int *liNoTSignal,
	struct signal_t *lastStructTSignal);

//void st_feed_sig_resp_(struct signal_resp_t *lstStructOrderStatusIn,
//	struct symbol_pos_t *lstStructPositionIn, struct pending_order_t
//	*lstStructPendingPOSIn, int *liNoTSignal, struct signal_t *lastStructTSignal);
void st_feed_sig_resp_(struct signal_resp_t* lstStructOrderStatusIn, struct symbol_pos_t *lstStructPositionIn,
    int *sig_cnt, struct signal_t* sigs,struct strat_out_log *outlog);
void st_feed_position_(struct symbol_pos_t *lstStructPositionIn, int *liNoTSignal,
	struct signal_t *lastStructTSignal);
//void st_feed_init_position_(struct strategy_init_pos_t *lstStructPositionIn, int *liNoTSignal,
//	struct signal_t *lastStructTSignal);
void st_feed_init_position_(struct strategy_init_pos_t *lstStructPositionIn,struct strat_out_log *outlog);
void st_init_(struct st_config_t *lstStructInitConfig, int *liRetCode,struct strat_out_log *outlog,
              char history_log[1500]);
void st_destroy_();
void st_idle_(int *liNoTSignal, struct signal_t *lastStructTSignal);
void st_feed_quote_(struct cStruct_quote_data *lstStructQuote, int *liNoTSignal,
	struct signal_t *lastStructTSignal);
void st_feed_marketinfo_0_(struct cfex_deep_quote *lstStructIn,
	int *liNoTSignal, struct signal_t *lastStructTSignal,struct strat_out_log *outlog);
void st_feed_marketinfo_1_(struct cStruct_dl_quote *lstStructIn,
	int *liNoTSignal, struct signal_t *lastStructTSignal,struct strat_out_log *outlog);
void st_feed_marketinfo_2_(struct cStruct_dl_quote *lstStructIn,
	int *liNoTSignal, struct signal_t *lastStructTSignal,struct strat_out_log *outlog);
void st_feed_marketinfo_3_(struct cStruct_dl_orderstat *lstStructIn,
	int *liNoTSignal, struct signal_t *lastStructTSignal,struct strat_out_log *outlog);
void st_feed_marketinfo_4_(struct cStruct_dl_quote *lstStructIn,
	int *liNoTSignal, struct signal_t *lastStructTSignal,struct strat_out_log *outlog);
void st_feed_marketinfo_5_(struct cStruct_dl_quote *lstStructIn,
	int *liNoTSignal, struct signal_t *lastStructTSignal,struct strat_out_log *outlog);
void st_feed_marketinfo_6_(struct shfe_my_quote *lstStructIn,
	int *liNoTSignal, struct signal_t *lastStructTSignal,struct strat_out_log *outlog);
void st_feed_marketinfo_8_(struct czce_my_deep_quote *lstStructIn,
	int *liNoTSignal, struct signal_t *lastStructTSignal,struct strat_out_log *outlog);
void st_feed_marketinfo_11_(struct cStruct_local_quote *lstStructIn,
	int *liNoTSignal, struct signal_t *lastStructTSignal,struct strat_out_log *outlog);

// lic
char * test();
char * test1();
bool check_lic();
#endif
