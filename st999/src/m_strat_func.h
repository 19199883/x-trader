#ifndef M_STRAT_FUNC_H
#define M_STRAT_FUNC_H

#define SIZE_BIG_VOL 20

struct st_evaluated_sig
{
    int refNum;
    double dSig;
    int action;
};

struct st_big_vol_rec
{
    int itime, iBS, iVol;
    double dmidP;
};

typedef struct st_hlog_bar
{
    int idate;
    int itime;
    double dClose;
    char szContract[16];
    int tflag;
} my_hlog_bar;

typedef struct st_ref_return
{
    double drtn;
    char szContract[16];
} my_ref_return;

extern double m_add_base, m_thre_open, m_thre_close;
extern double m_divider1, m_divider2;
extern int strat_time_shift; // in ms

extern double strat_order_price;
extern int strat_signal;

extern double recSignal[120000];

extern int rec_total_BV[150000], rec_total_SV[150000];
extern int num_totalV;

extern my_hlog_bar mybar[5000];
extern int numbar;

int strat_init();
void strat_destroy();
int strat_get_signal(struct in_data *lstCurrInData,struct in_data *lstPreInData);

void strat_init_501();



double strat_compute_501_signal(struct in_data *lstCurrInData, struct in_data *lstPreInData, int *idirection);

int strat_get_signal_501(struct in_data *lstCurrInData,struct in_data *lstPreInData);
int strat_update_freeze_info(struct in_data *lstCurrInData,struct in_data *lstPreInData);
int strat_update_freeze_info2(struct in_data *lstCurrInData,struct in_data *lstPreInData);

int s_load_hlog_datamap(char *lcString, my_hlog_bar *mbar);
int s_read_hlog_file(char* hlog_name);
int record_new_bar_data(struct in_data *lstCurrInData,struct in_data *lstPreInData);
int compute_ref_date_return(int ref_tflag, my_ref_return *my_rtn);

#endif
