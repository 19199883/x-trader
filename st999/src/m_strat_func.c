#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <memory.h>
#include <time.h>
#include "common.h"
#include "common_mod.h"
#include "common_func.h"
#include "common_mm.h"
#include "interface_v30.h"
#include "MK2_var_def.h"
#include "MK2_func.h"
#include "m_strat_func.h"

//#define USER_TAG_STOP_WIN

int m_flag_inner_para = 1;

double m_add_base, m_thre_open, m_thre_close;
double m_divider1 = 1.0, m_divider2 = 1.0;
double m_multiplier = 1.0;
double m_stop_win = 500.0;
int m_double_round = 12;
int stop_win_done = 0;
int strat_time_shift; // in ms

int n_begin_tick;

double m_weight[20];

double strat_order_price;
int strat_signal;

struct st_evaluated_sig stSig[5];

double recSignal[120000];

my_hlog_bar mybar[5000];
my_hlog_bar newbar[MAX_NUM_CONTRACT][200];
int numbar;
int n_newbar[MAX_NUM_CONTRACT];
int refDate;
int intended_direction[MAX_NUM_CONTRACT];
int flag_signal;

int strat_init()
{
    int stratID = giStratID;

    //Define m_add_base
    m_add_base = 20.0;
    if(strcmp(gcSecName, "ag")==0) {m_add_base = 20.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "al")==0) {m_add_base = 20.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "au")==0) {m_add_base = 20.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "bu")==0) {m_add_base = 20.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "cu")==0) {m_add_base = 20.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "hc")==0) {m_add_base = 20.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "ni")==0) {m_add_base = 20.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "pb")==0) {m_add_base = 10.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "rb")==0) {m_add_base = 200.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "ru")==0) {m_add_base = 20.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "zn")==0) {m_add_base = 20.0; m_stop_win = 30.0;}

    else if(strcmp(gcSecName, "OI")==0) {m_add_base = 20.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "SR")==0) {m_add_base = 20.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "MA")==0) {m_add_base = 20.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "RM")==0) {m_add_base = 20.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "ZC")==0) {m_add_base = 20.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "CF")==0) {m_add_base = 20.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "FG")==0) {m_add_base = 20.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "TA")==0) {m_add_base = 100.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "SM")==0) {m_add_base = 20.0; m_stop_win = 30.0;}

    else if(strcmp(gcSecName, "m")==0) {m_add_base = 20.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "y")==0) {m_add_base = 20.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "p")==0) {m_add_base = 40.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "a")==0) {m_add_base = 20.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "cs")==0) {m_add_base = 30.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "l")==0) {m_add_base = 30.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "jd")==0) {m_add_base = 20.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "v")==0) {m_add_base = 20.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "c")==0) {m_add_base = 200.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "pp")==0) {m_add_base = 60.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "i")==0) {m_add_base = 100.0; m_stop_win = 30.0;}
    else if(strcmp(gcSecName, "j")==0) {m_add_base = 100.0; m_stop_win = 60.0;}
    else if(strcmp(gcSecName, "jm")==0) {m_add_base = 100.0; m_stop_win = 30.0;}
    m_stop_win = 5000.0;

    stop_win_done = 0;
    m_thre_open = 10.0; m_thre_close = 1.4;
    if(m_flag_inner_para != 1)
        s_read_ev_file(gszEVname,&m_thre_open,&m_thre_close);

    memset(m_weight, 0, sizeof(double)*20);
    m_divider1 = 1.0; m_divider2 = 1.0;
    strat_time_shift = 0;

    switch(stratID)
    {
    case 501: strat_init_501(); break;

    default:
        break;
    }

	memset(recSignal, 0, sizeof(double)*100000);
	memset(n_newbar, 0, sizeof(int)*MAX_NUM_CONTRACT);
	memset(intended_direction, 0, sizeof(int)*MAX_NUM_CONTRACT);
	flag_signal = 0;
	refDate = -1;

	s_read_hlog_file(file_history_log);

    return 0;

}

void strat_destroy()
{
}

int strat_get_signal(struct in_data *lstCurrInData,struct in_data *lstPreInData)
{
    int stratID = giStratID;
    switch(stratID)
    {
    case 501:
        strat_signal = strat_get_signal_501(lstCurrInData, lstPreInData);
        break;

    default:
        break;
    }
    //printf("%d signal =%d\n",lstCurrInData->iTime, strat_signal);
    return strat_signal;
}

void strat_init_501()
{
    if(gcExch == cSHFE) {m_divider1 = 2.0; m_divider2 = 2.0; n_begin_tick = 20;}
    if(gcExch == cDCE || gcExch == cCZCE) {m_divider1 = 2.0; m_divider2 = 2.0; n_begin_tick = 2;}
    m_weight[0] = 3.0; m_weight[1] = 3.0;
    m_weight[2] = 3.0; m_weight[3] = 3.0;
    m_weight[4] = 3.0; m_weight[5] = 1.0;
    m_weight[6] = 1.0; m_weight[7] = 1.0;
    m_weight[8] = 1.0; m_weight[9] = 1.0;
    strat_time_shift = 0;

    if(m_flag_inner_para == 0) return;

    if(strcmp(gcSecName, "ni")==0 && giNightMarket == 0) {m_thre_open=10.0; m_thre_close = 2.0; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "ni")==0 && giNightMarket == 1) {m_thre_open=10.0; m_thre_close = 4.5; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "cu")==0 && giNightMarket == 0) {m_thre_open=3.5; m_thre_close = 1.4;  m_add_base = 20.0;}
    else if(strcmp(gcSecName, "cu")==0 && giNightMarket == 1) {m_thre_open=3.8; m_thre_close = 1.3;  m_add_base = 20.0;}
    else if(strcmp(gcSecName, "ru")==0 && giNightMarket == 0) {m_thre_open=4.0; m_thre_close = 1.5; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "ru")==0 && giNightMarket == 1) {m_thre_open=3.2; m_thre_close = 1.3; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "ag")==0 && giNightMarket == 0) {m_thre_open=3.0; m_thre_close = 1.3; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "ag")==0 && giNightMarket == 1) {m_thre_open=3.3; m_thre_close = 1.4; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "zn")==0 && giNightMarket == 0) {m_thre_open=4.0; m_thre_close = 1.3; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "zn")==0 && giNightMarket == 1) {m_thre_open=3.2; m_thre_close = 1.3; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "al")==0 && giNightMarket == 0) {m_thre_open=3.2; m_thre_close = 1.5; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "al")==0 && giNightMarket == 1) {m_thre_open=3.3; m_thre_close = 1.3; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "rb")==0 && giNightMarket == 0) {m_thre_open=2.5; m_thre_close = 1.4; m_add_base = 200.0;}
    else if(strcmp(gcSecName, "rb")==0 && giNightMarket == 1) {m_thre_open=3.5; m_thre_close = 1.25; m_add_base = 200.0;}
    else if(strcmp(gcSecName, "bu")==0 && giNightMarket == 0) {m_thre_open=3.0; m_thre_close = 1.6; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "bu")==0 && giNightMarket == 1) {m_thre_open=2.5; m_thre_close = 1.4; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "pb")==0 && giNightMarket == 0) {m_thre_open=5.0; m_thre_close = 1.6; m_add_base = 10.0;}
    else if(strcmp(gcSecName, "pb")==0 && giNightMarket == 1) {m_thre_open=4.0; m_thre_close = 1.4; m_add_base = 10.0;}
    else if(strcmp(gcSecName, "hc")==0 && giNightMarket == 0) {m_thre_open=6.0; m_thre_close = 1.4; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "hc")==0 && giNightMarket == 1) {m_thre_open=5.0; m_thre_close = 1.35; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "au")==0 && giNightMarket == 0) {m_thre_open=4.0; m_thre_close = 1.3; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "au")==0 && giNightMarket == 1) {m_thre_open=4.0; m_thre_close = 1.5; m_add_base = 20.0;}

    else if(strcmp(gcSecName, "OI")==0 && giNightMarket == 0) {m_thre_open=4.5; m_thre_close = 1.6; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "SR")==0 && giNightMarket == 0) {m_thre_open=3.8; m_thre_close = 1.4; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "MA")==0 && giNightMarket == 0) {m_thre_open=3.5; m_thre_close = 1.4; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "RM")==0 && giNightMarket == 0) {m_thre_open=3.2; m_thre_close = 1.6; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "ZC")==0 && giNightMarket == 0) {m_thre_open=4.0; m_thre_close = 1.6; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "CF")==0 && giNightMarket == 0) {m_thre_open=4.5; m_thre_close = 1.3; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "FG")==0 && giNightMarket == 0) {m_thre_open=3.5; m_thre_close = 1.6; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "TA")==0 && giNightMarket == 0) {m_thre_open=3.2; m_thre_close = 1.3; m_add_base = 100.0;}
    else if(strcmp(gcSecName, "SM")==0 && giNightMarket == 0) {m_thre_open=6.0; m_thre_close = 2.5; m_add_base = 20.0;}

    else if(strcmp(gcSecName, "m")==0 && giNightMarket == 0) {m_thre_open=2.8; m_thre_close = 1.3; m_add_base = 100.0;}
    else if(strcmp(gcSecName, "y")==0 && giNightMarket == 0) {m_thre_open=3.0; m_thre_close = 1.5; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "p")==0 && giNightMarket == 0) {m_thre_open=2.2; m_thre_close = 1.25; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "a")==0 && giNightMarket == 0) {m_thre_open=3.5; m_thre_close = 1.4; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "cs")==0) {m_thre_open=3.5; m_thre_close = 1.4; m_add_base = 30.0;}
    else if(strcmp(gcSecName, "l")==0) {m_thre_open=3.0; m_thre_close = 1.5; m_add_base = 30.0;}
    else if(strcmp(gcSecName, "jd")==0) {m_thre_open=4.0; m_thre_close = 1.8;m_add_base = 20.0;}
    else if(strcmp(gcSecName, "v")==0) {m_thre_open=3.5; m_thre_close = 1.8; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "c")==0) {m_thre_open=2.4; m_thre_close = 1.3; m_add_base = 200.0;}
    else if(strcmp(gcSecName, "pp")==0) {m_thre_open=12.5; m_thre_close = 2.5; m_add_base = 60.0;}
    else if(strcmp(gcSecName, "i")==0 && giNightMarket == 0) {m_thre_open=3.7; m_thre_close = 1.3; m_add_base = 100.0;}
    else if(strcmp(gcSecName, "j")==0 && giNightMarket == 0) {m_thre_open=3.5; m_thre_close = 1.6; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "jm")==0 && giNightMarket == 0) {m_thre_open=3.5; m_thre_close = 1.6; m_add_base = 20.0;}

    else if(strcmp(gcSecName, "OI")==0 && giNightMarket == 1) {m_thre_open=4.5; m_thre_close = 1.6; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "SR" )==0 && giNightMarket == 1) {m_thre_open=2.8; m_thre_close = 1.4; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "MA")==0 && giNightMarket == 1) {m_thre_open=3.0; m_thre_close = 1.3; m_add_base = 40.0;}
    else if(strcmp(gcSecName, "RM")==0 && giNightMarket == 1) {m_thre_open=2.4; m_thre_close = 1.3; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "ZC")==0 && giNightMarket == 1) {m_thre_open=4.0; m_thre_close = 1.3; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "CF")==0 && giNightMarket == 1) {m_thre_open=5.5; m_thre_close = 1.4; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "FG")==0 && giNightMarket == 1) {m_thre_open=3.5; m_thre_close = 1.5; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "TA")==0 && giNightMarket == 1) {m_thre_open=3.0; m_thre_close = 1.4; m_add_base = 100.0;}

    else if(strcmp(gcSecName, "m")==0 && giNightMarket == 1) {m_thre_open=2.2; m_thre_close = 1.3; m_add_base = 100.0;}
    else if(strcmp(gcSecName, "y")==0 && giNightMarket == 1) {m_thre_open=3.0; m_thre_close = 1.4; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "p")==0 && giNightMarket == 1) {m_thre_open=3.0; m_thre_close = 1.3; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "a")==0 && giNightMarket == 1) {m_thre_open=3.0; m_thre_close = 1.6; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "i")==0 && giNightMarket == 1) {m_thre_open=3.5; m_thre_close = 1.6; m_add_base = 100.0;}
    else if(strcmp(gcSecName, "j")==0 && giNightMarket == 1) {m_thre_open=1.5; m_thre_close = 1.3; m_add_base = 20.0;}
    else if(strcmp(gcSecName, "jm")==0 && giNightMarket == 1) {m_thre_open=3.5; m_thre_close = 1.6; m_add_base = 20.0;}

}


double strat_compute_501_signal(struct in_data *lstCurrInData, struct in_data *lstPreInData, int *idirection)
{

    int tflag1, tflag2;
    int x, i;
    my_ref_return my_rtn[MAX_NUM_CONTRACT];
    double rmax, rmin;

    x = lstCurrInData->iTime;
    tflag1 = (int)(x/10000000)*100 + (int)(((int)(x/100000) - (int)(x/10000000)*100)/5);
    //x = get_delayed_msecs(lstCurrInData->iTime, 250);
    x = lstPreInData->iTime;
    tflag2 = (int)(x/10000000)*100 + (int)(((int)(x/100000) - (int)(x/10000000)*100)/5);

    if((tflag1 == 901 && tflag2 == 900 && flag_signal == 0) || (tflag1 == 902 && tflag2 == 901 && flag_signal == 1))
    {
        compute_ref_date_return(tflag2, my_rtn);
        rmax = -0.1; rmin = 0.1;
        for(i=0; i<giNumContract; i++)
        {
            rmax = MAX(rmax, my_rtn[i].drtn);
            rmin = MIN(rmin, my_rtn[i].drtn);
        }
        if(rmax >= 0.025 && rmin <= -0.025)
        {
            for(i=0; i<giNumContract; i++)
            {
                if(my_rtn[i].drtn >= 0.025) idirection[i] = -1;
                else if(my_rtn[i].drtn <= -0.025) idirection[i] = 1;
                else idirection[i] = 0;
            }
        }
        else if(rmax >= 0.025)
        {
            for(i=0; i<giNumContract; i++)
            {
                if(my_rtn[i].drtn >= 0.025) idirection[i] = -1;
                else if(my_rtn[i].drtn == rmin) idirection[i] = 1;
                else idirection[i] = 0;
            }
        }
        else if(rmin <= -0.025)
        {
            for(i=0; i<giNumContract; i++)
            {
                if(my_rtn[i].drtn <= -0.025) idirection[i] = 1;
                else if(my_rtn[i].drtn == rmax) idirection[i] = -1;
                else idirection[i] = 0;
            }
        }
        else
        {
            for(i=0; i<giNumContract; i++)
                idirection[i] = 0;
        }
        flag_signal ++;

        if(giNumContract >= 2)
        {
            idirection[0] = 1;
            idirection[1] = -1;
        }

//        for(i=0; i<giNumContract; i++)
//            printf("%d %s %0.4f %d; ",lstCurrInData->iTime, gstLocTradeInfo[i].szContract, my_rtn[i].drtn, idirection[i]);
//        printf("\n");
    }

    return 0;
}

int strat_get_signal_501(struct in_data *lstCurrInData,struct in_data *lstPreInData)
{
    int i;
    int isig = 0;
    strat_compute_501_signal(lstCurrInData,lstPreInData, intended_direction);

    record_new_bar_data(lstCurrInData, lstPreInData);

    i = giTradeInfo;
    if(intended_direction[i]== 1 && gstLocTradeInfo[i].iPosD - gstLocTradeInfo[i].iPosK < gstLocTradeInfo[i].isetMaxPos)
    {
        isig = 1;
        if(flag_signal == 1) intended_direction[i] =2;
    }
    else if(intended_direction[i]== 0 && gstLocTradeInfo[i].iPosD - gstLocTradeInfo[i].iPosK < 0)
    {
        isig = 2;
        if(flag_signal == 1) intended_direction[i] =2;
    }
    else if(intended_direction[i]== 0 && gstLocTradeInfo[i].iPosD - gstLocTradeInfo[i].iPosK > 0)
    {
        isig = -2;
        if(flag_signal == 1) intended_direction[i] =2;
    }
    else if(intended_direction[i]== -1 && gstLocTradeInfo[i].iPosD - gstLocTradeInfo[i].iPosK > -gstLocTradeInfo[i].isetMaxPos)
    {
        isig = -1;
        if(flag_signal == 1) intended_direction[i] =2;
    }

    recSignal[giTickNo-1] = (double)isig;

#ifdef USE_SHFE_LV1_AND_DEEP
    //if(lstCurrInData->iData_Flag == 1 && (fabs(cur_sig) <= m_thre_open*1.2)) cur_sig = 1.0;
#endif // USE_SHFE_LV1_AND_DEEP

    if(isig > 0 && flag_signal == 1)
        strat_order_price = lstCurrInData->dBP1;
    else if(isig > 0 && flag_signal == 2)
        strat_order_price = lstCurrInData->dBP1 + gstGlobalPara[giTradeInfo].stSec.rTickSize;
    else if(isig <= 0 && flag_signal == 1)
        strat_order_price = lstCurrInData->dSP1;
    else if(isig <= 0 && flag_signal == 2)
        strat_order_price = lstCurrInData->dSP1 - gstGlobalPara[giTradeInfo].stSec.rTickSize;

    return isig;
}

int s_load_hlog_datamap(char *lcString, my_hlog_bar *mbar)
{

     int i;
     //int liRankin, liDate;
	 char *r;

     char buff[2000];
     int ipos[16], i0, i1;


     mbar->idate = 0;

     strncpy(buff, lcString, 2000);

     i1 = strlen(buff);
     i0 = 0;
     for(i=0; i<i1; i++)
     {
         if(buff[i] == ',' && i0<7)
         {
             ipos[i0] = i; buff[i] = '\0';
             i0++;
         }
     }

     if(i0 < 2) goto END;

     i = 0; r = &buff[i];
     mbar->idate = atoi(r);

     i = ipos[0] + 1; r = &buff[i];
     strncpy(mbar->szContract,r,16);

     i = ipos[1] + 1; r = &buff[i];
     mbar->itime = atoi(r);

     i = ipos[2] + 1; r = &buff[i];
     mbar->dClose = atof(r);

     //printf("%d %s %d %0.4f %0.4f\n",m_oc_param->iDate,m_oc_param->szSec,m_oc_param->iFlag,m_oc_param->dOpenParam,m_oc_param->dCloseParam);

    return 0;
END:	return 1;
}

int s_retrieve_hlog_name(char* old_log_name, char* real_log_name)
{
     int i;
     //int liRankin, liDate;
	 char *r;

     char buff[1500];
     int ipos[16], i0, i1;

     strncpy(buff, old_log_name, 1500);

     i1 = strlen(buff);
     i0 = 0;
     for(i=0; i<i1; i++)
     {
         if(buff[i] == ';' && i0<10)
         {
             ipos[i0] = i; buff[i] = '\0';
             i0++;
         }
     }

     i = 0; r = &buff[i];
     strncpy(real_log_name,r,256);

     return 1;
}

int s_read_hlog_file(char* hlog_name)
{
     char buff[2048];
     FILE * pfile;
     my_hlog_bar tmp_bar;
     char lcRssSecName[16];
     int idatelist[2000];
     int idt, itmp;
     int num_oc = 0;
     int i,i0,idiff;
     int x;
     char file_log[256];

     s_retrieve_hlog_name(hlog_name, file_log);

     pfile = fopen(file_log, "r");
     if(pfile == NULL) return 0;

     idt = 0;
     fgets(buff, 2048, pfile);
     while(fgets(buff, 2048, pfile) != NULL)
     {
        //printf("%s", buff);
        s_load_hlog_datamap(buff, &tmp_bar);
        if(tmp_bar.idate < giCurDate)
        {
            //if(idt > 0) printf("%s %d %d %d\n",tmp_bar.szContract, idt, tmp_bar.idate,idatelist[idt-1]);
            if(idt > 0 && idatelist[idt-1] != tmp_bar.idate)
            {
                idatelist[idt] =  tmp_bar.idate; idt ++;
            }
            else if(idt == 0)
            {
                idatelist[idt] =  tmp_bar.idate; idt ++;
            }
        }
     }
     for(i=0; i<idt-1; i++)
        for(i0=i+1; i0<idt; i0++)
     {
         if(idatelist[i]>idatelist[i0]) {itmp = idatelist[i0]; idatelist[i0] = idatelist[i]; idatelist[i] = itmp;}
     }
     rewind(pfile);
     refDate = idatelist[idt-3];

     i = 0;
     while(fgets(buff, 2048, pfile) != NULL)
     {
        s_load_hlog_datamap(buff, &tmp_bar);
        if(tmp_bar.idate < giCurDate && tmp_bar.idate >= idatelist[idt-3])
        {
            mybar[i].idate = tmp_bar.idate;
            mybar[i].itime = tmp_bar.itime;
            mybar[i].dClose = tmp_bar.dClose;
            x = tmp_bar.itime;
            mybar[i].tflag = (int)(x/10000000)*100 + (int)(((int)(x/100000) - (int)(x/10000000)*100)/5);
            strncpy(mybar[i].szContract, tmp_bar.szContract, 16);
            i++;
        }
     }
     numbar = i;

     fclose(pfile);

     return 1;
}


int record_new_bar_data(struct in_data *lstCurrInData,struct in_data *lstPreInData)
{
    int tflag1, tflag2;
    int x, i;
    x = lstPreInData->iTime;
    tflag1 = (int)(x/10000000)*100 + (int)(((int)(x/100000) - (int)(x/10000000)*100)/5);
    x = lstCurrInData->iTime;
    tflag2 = (int)(x/10000000)*100 + (int)(((int)(x/100000) - (int)(x/10000000)*100)/5);
    if(tflag1 < tflag2)
    {
        i = n_newbar[giTradeInfo];
        newbar[giTradeInfo][i].idate = giCurDate;
        newbar[giTradeInfo][i].dClose = (lstPreInData->dBP1 + lstPreInData->dSP1)/2;
        newbar[giTradeInfo][i].itime = (int)(tflag1/100)*10000000 + (tflag1%100)*500000;
        newbar[giTradeInfo][i].tflag = tflag1;
        strncpy(newbar[giTradeInfo][i].szContract, lstPreInData->cTicker, 8);
        n_newbar[giTradeInfo] ++;
    }

    return 1;
}

int get_delayed_msecs(int itime, int ims_delay)
{
    int ihour, imin, isec, ims;

    ihour = itime/10000000;
    imin = (itime%10000000)/100000;
    isec = (itime%100000)/1000;
    ims = ims%1000;

    ims = (ims + ims_delay)%1000;
    isec = ims/1000 + isec;
    if(isec >= 60) {imin += 1; isec -= 60;}
    if(imin>=60)
    {
        ihour = ihour + 1;
        imin = imin - 60;
    }

    return ihour*10000000 + imin*100000 + isec*1000 + ims;
}

int compute_ref_date_return(int ref_tflag, my_ref_return *my_rtn)
{
    int i, i0;
    my_hlog_bar refbar[MAX_NUM_CONTRACT];
    int n_refbar;

    memset(refbar, 0, sizeof(my_hlog_bar)*MAX_NUM_CONTRACT);
    memset(my_rtn, 0, sizeof(my_ref_return)*MAX_NUM_CONTRACT);

    i0 = 0;
    for(i=0; i< numbar; i++)
    {
        if(mybar[i].idate == refDate && mybar[i].tflag == ref_tflag)
        {
            memcpy(&refbar[i0], &mybar[i], sizeof(my_hlog_bar));
            i0 ++;
        }
    }
    n_refbar = i0;

    for(i=0; i<giNumContract; i++)
        for(i0=0; i0< n_refbar; i0++)
    {
        if(strncmp(gstPreIn[i].cTicker, refbar[i0].szContract, 8) == 0)
        {
            strncpy(my_rtn[i].szContract,gstPreIn[i].cTicker, 8);
            my_rtn[i].drtn = ((gstPreIn[i].dBP1 + gstPreIn[i].dSP1)/2 - refbar[i0].dClose)/refbar[i0].dClose;
            break;
        }
    }
    return 1;
}





