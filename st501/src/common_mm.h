#ifndef COMMON_MM_H
#define COMMON_MM_H

#include "common.h"
#include "interface_v30.h"


#define MAX_ORDER_SIZE 10
#define MAX_ORDER_VOLUME 1

typedef struct struct_user_order
{
    int iBS;        //1: buy;  2: sell;  4:cancel;
    double dPrice;  //order price
    int iVol;       //order volume
    int iOpenFlat;  //0: default, open or flat;  2:flat, no open; 2: open, no flat;

    //for user information, no need to set values;
    int sig_id;
    int sig_time;
    int vol2pass;
    char szContract[16];
}st_usr_order;

//iBS in st_usr_order
enum{
    USER_ORDER_BUY = 1,
    USER_ORDER_SELL = 2,
    USER_ORDER_CANCEL = 4,
};

//iOpenFlat in st_usr_order
enum{
    USER_ORDER_OPT_NONE = 0,
    USER_ORDER_OPT_OPEN = 1,
    USER_ORDER_OPT_FLAT = 2,
};

struct struct_exe_order_rec
{
    double rPrice;
    int iBS;
    int iOpenFlat;
    int iVol;
};


extern st_usr_order gstUserNewOrder[MAX_ORDER_SIZE*4], gstUserCurOrder[MAX_ORDER_SIZE*4];
extern int giUserNewOrder, giUserCurOrder, giMaxUsrOrder;

extern struct struct_exe_order_rec gstLastRec;


//extern st_order_info gstOrderBuy[MAX_ORDER_SIZE], gstOrderSel[MAX_ORDER_SIZE], gstOrderCancel[MAX_ORDER_SIZE*2];
//extern int giMaxMyOrder;


void s_initializer_usr_order();
void s_initializer_my_order();
void reset_exe_order_rec();

int s_update_my_order_from_usr_order(st_usr_order *gstUsrOrderIn, const int giUsrOrderIn,
         struct in_data *lstCurrInData, int *laiFullOrderBookBuy, int *laiFullOrderBookSel,
         struct all_parameter *lstGlobalPara, st_loc_trade_info *p_stLocTradeInfo);

int s_update_current_usr_order(st_usr_order *gstUsrOrderOut,int giMaxUsrOrder, int *piUsrOrderOut);

int s_replace_flat_order(struct in_data *lstCurrInData, int *laiFullOrderBookBuy, int *laiFullOrderBookSel,
         struct all_parameter *lstGlobalPara, st_loc_trade_info *p_stLocTradeInfo);

int s_set_mm_cancel_order(const int sig_id, char* szContract);

int s_mm_assign_out_tsignal(struct signal_t *pstTSignal, int *licNoTSignal, st_loc_trade_info *p_stLocTradeInfo);

int s_update_my_order_info(struct signal_resp_t *lstStructOrderStatusIn);

int s_mm_calc_profitloss_and_update(st_order_info *p_stOrder, struct sec_parameter *p_stSecPara,
    st_loc_trade_info *p_stLocTradeInfo, int istatus);

/******************************************
*********** From Simulation Use ***********
******************************************/
int s_simu_cancel_order();
int s_simu_calc_all_profitloss(struct sec_parameter *p_stSecPara, st_loc_trade_info *p_stLocTradeInfo);

int s_trade_simulation(struct in_data *p_stDataIn, struct in_data *p_stPreDataIn,
	struct all_parameter *p_stGlobalPara, int liTickNo,
	int *laiFullOrderBookBuy, int *laiFullOrderBookSel, int liLenFullOrderBook,
	st_loc_trade_info *p_stLocTradeInfo);

int s_trade_simulation_CFFE(struct in_data *p_stDataIn, struct in_data *p_stPreDataIn,
	struct all_parameter *p_stGlobalPara, int liTickNo,
	int *laiFullOrderBookBuy, int *laiFullOrderBookSel, int liLenFullOrderBook,
	st_loc_trade_info *p_stLocTradeInfo);

int s_trade_simulation_CZCE(struct in_data *p_stDataIn, struct in_data *p_stPreDataIn,
	struct all_parameter *p_stGlobalPara, int liTickNo,
	int *laiFullOrderBookBuy, int *laiFullOrderBookSel, int liLenFullOrderBook,
	st_loc_trade_info *p_stLocTradeInfo);


#endif //COMMON_MM_H
