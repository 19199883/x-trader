#ifndef COMMON_FUNC_H
#define COMMON_FUNC_H


int s_read_gta_fut_struct(struct cfex_deep_quote *p_stGtaQuoteIn, struct in_data *p_stInData);
int s_read_sh_fut_struct(struct shfe_my_quote *p_stSHQuoteIn, struct in_data *p_stInData);
int s_file_sh_flag_1_data(struct in_data *p_stInData, struct in_data *p_preInData);

int s_set_exch_parameters(char lcExch, char *lcSec, struct all_parameter *p_stOutData);
int s_set_convertion_parameters(char *lcSecName, struct sec_parameter *p_stOutData);


int s_read_zz_fut_struct(struct czce_my_deep_quote *p_stcZZQuoteIn, struct in_data *p_stInData);
int s_read_dl_fut_struct(struct cStruct_dl_quote *p_stcDLQuoteIn, struct in_data *p_stInData);
int s_read_cstruct_dl_orderstat(struct cStruct_dl_orderstat *p_stcStructDLOrderStatIn,
	struct in_data *p_stInData)	;

int f_sign(int liN);
double f_calc_curr_avgp(double lrVal, int liV, double lrPrevAvgP,struct all_parameter *p_stGlobalPara);
int  f_real_to_int(double lrR, int liMultiplier);
int f_find_security_name(char* sc_name, char *lcTicker,char lcExch);

int f_is_limit(double lrBP1, double lrSP1, double lrLmtUpP, double lrLmtDnP);

double alnorm(double x, BOOL upper );



#endif
