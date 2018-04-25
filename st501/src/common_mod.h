#ifndef COMMON_MOD_H
#define COMMON_MOD_H




int s_read_fut_struct(int liTickNo);
int s_set_global_parameters(char lcExch, char *lcSecName, struct all_parameter *p_stGlobalPara);

int f_check_valid_data(struct in_data *p_stCurrInData, struct in_data *p_stPreInData,
	int liTickNo, struct all_parameter *p_stGlobalPara);

void mysub0(int liFileNumber, char *lcFileName);

int s_output_log(int liTickNo, FILE *p_iFileNumber, char *lcFileName, struct strat_out_log *outlog);

int s_update_full_order_book(struct in_data *lstCurrInData, struct in_data *lstPreInData,
	struct all_parameter *lstGlobalPara,int *laiFullOrderBookBuy, int *laiFullOrderBookSel,
	int liLenFullOrderBook);

int do_list_sorting(double *old_lst, double* sorted_lst, int num_lst, int ibegin);
double find_quantile_value(double* sorted_lst, int num_lst, double dquant);
int s_read_ev_file(char* ev_name, double *pdOpenParam, double *pdCloseParam);

int transfer_2_millisec(int itime);

int strat_divide_curr_volumn(struct in_data *p_stDataIn, struct in_data *p_stPreDataIn,
         struct all_parameter *p_stGlobalPara,
         double *pr_PriceLo, double *pr_PriceHi,int *pVolLo, int *pVolHi);

#endif
