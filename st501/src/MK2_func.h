#ifndef MK2_FUNC_H
#define MK2_FUNC_H

#include "common.h"
#include "common_mm.h"
#include "interface_v30.h"
#include "MK2_var_def.h"

void s_init_MK2_parameters();
void s_destroy_MK2_parameters();

int update_lastThreeRec(int iBS, double rPrice);

int s_MK2_sig_gen(struct in_data *lstCurrInData, struct in_data *lstPreInData,
	struct all_parameter *lstGlobalPara, st_loc_trade_info *p_stLocTradeInfo,
	int liTickNo, int *laiFullOrderBookBuy, int *laiFullOrderBookSel,
	int liLenFullOrderBook, st_usr_order *stCurUsrOrder, int iCurUsrOrder,
	st_usr_order *stNewUsrOrder, int iMaxUsrOrder, int *piNewUsrOrder);

int s_MK2_sig_gen_normal(struct in_data *lstCurrInData, struct in_data *lstPreInData,
	struct all_parameter *lstGlobalPara, st_loc_trade_info *p_stLocTradeInfo,
	int liTickNo, int *laiFullOrderBookBuy, int *laiFullOrderBookSel,
	int liLenFullOrderBook, st_usr_order *stCurUsrOrder, int iCurUsrOrder,
	st_usr_order *stNewUsrOrder, int iMaxUsrOrder, int *piNewUsrOrder);

int s_generate_MK2_cancel_orders(double *larBP, double *larSP, double lrTick, int iCurTime,
    st_loc_trade_info *p_stLocTradeInfo, st_usr_order *stCurUsrOrder, int iCurUsrOrder,
	st_usr_order *stNewUsrOrder, int iMaxUsrOrder, int *piNewUsrOrder);

int get_delayed_time(int itime, int iDelayMin);

int s_MK2_eod_square(const struct in_data *lstCurrInData, const struct all_parameter *lstGlobalPara,
	st_loc_trade_info *p_stLocTradeInfo, const int liTickNo,
	st_usr_order *stCurUsrOrder, int iCurUsrOrder,
	st_usr_order *stNewUsrOrder, int iMaxUsrOrder, int *piNewUsrOrder);

int s_MK2_eod_square_flat_netpos(const struct in_data *lstCurrInData, const struct all_parameter *lstGlobalPara,
	st_loc_trade_info *p_stLocTradeInfo, const int liTickNo,
	st_usr_order *stCurUsrOrder, int iCurUsrOrder,
	st_usr_order *stNewUsrOrder, int iMaxUsrOrder, int *piNewUsrOrder);

int s_MK2_eod_final_square(const struct in_data *lstCurrInData, const struct all_parameter *lstGlobalPara,
	st_loc_trade_info *p_stLocTradeInfo, const int liTickNo,
	st_usr_order *stCurUsrOrder, int iCurUsrOrder,
	st_usr_order *stNewUsrOrder, int iMaxUsrOrder, int *piNewUsrOrder);

int s_MK2_run_each_record(struct signal_t *lastStructTSignal, int *piNoTSignal,struct strat_out_log *outlog);

int s_MK2_variable_init(struct in_data *p_stCurrInData, struct all_parameter *p_stGlobalPara);

int s_MK2_common_variable_init(struct in_data *p_stCurrInData, struct all_parameter *p_stGlobalPara);

int s_MK2_output_log_new(int liTickNo, struct strat_out_log *outlog);

int s_MK2_set_parameters();

#endif
