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




struct struct_mm_outdata gastMK2OutData[iOUT_LEN];
int giMK2OutDataIndex, giMK2TraceInit;
struct struct_MK2_paras gstMK2_Paras;

int gilastThreeRec[3];
double grlastThreeRecPrice[3];
int gilastThreeRecTime[3];
int giZhangDieTingTag = 0;
int giSleep = -1;
int giLast3Vol[3];


int giOrderStartTime = 0;
double grBaseLivePL = 0;

struct strat_out_log m_strat_log;

int m_signal;

void s_init_MK2_parameters()
{

	memset(&gstLocTradeInfo, 0, sizeof(st_loc_trade_info)*MAX_NUM_CONTRACT);
	memset(&gastMK2OutData, 0, sizeof(struct struct_mm_outdata)*iOUT_LEN);

	giMK2OutDataIndex = 0;
	giMK2TraceInit = 0;

	//giPriceMarkBuy = (int*)malloc(sizeof(int)*(giMaxMMlevel+2));
	//giPriceMarkSel = (int*)malloc(sizeof(int)*(giMaxMMlevel+2));

	gstMK2_Paras.rBVSum = 0;
	gstMK2_Paras.rSVSum = 0;
	gstMK2_Paras.rStopLossThreshold = 0.1;
	gstMK2_Paras.rVolBuy = 0;
	gstMK2_Paras.rVolSel = 0;
	gstMK2_Paras.iStop = 0;
	gstMK2_Paras.iUPstop = 0;
	gstMK2_Paras.iDNstop = 0 ;

	gstMK2_Paras.iLimitRange = 5;
    gstMK2_Paras.iLimitSqRange = 5;

	gstMK2_Paras.iLimitSqFlg = 0;
	gstMK2_Paras.iWithClose = 1;

	gaiFullOrderBookBuy[giTradeInfo] = NULL;
	gaiFullOrderBookSel[giTradeInfo] = NULL;

	giSleep = -1;
	giLast3Vol[0] = 0;
	giLast3Vol[1] = 0;
	giLast3Vol[2] = 0;

	gilastThreeRec[0] = 0;
	gilastThreeRec[1] = 0;
	gilastThreeRec[2] = 0;
	grlastThreeRecPrice[0] = 0;
	grlastThreeRecPrice[1] = 0;
	grlastThreeRecPrice[2] = 0;
	gilastThreeRecTime[0] = 0;
	gilastThreeRecTime[1] = 0;
	gilastThreeRecTime[2] = 0;

	giOrderStartTime = 0; grBaseLivePL = 0;

	giZhangDieTingTag = 0;

	memset(&m_strat_log, 0, sizeof(struct strat_out_log));
	pfDayLogFile = NULL;

	m_signal = 0;
	giTickNo = 0;

}

void s_destroy_MK2_parameters()
{
    int i;
    for(i=0; i<giNumContract; i++)
    {
        RELEASE(gaiFullOrderBookBuy[i]);
        RELEASE(gaiFullOrderBookSel[i]);
    }

    strat_destroy();
    if(pfDayLogFile != NULL) {fclose(pfDayLogFile); pfDayLogFile = NULL; }
}

int update_lastThreeRec(int iBS, double rPrice)
{
    int i;

    if(iBS == gilastThreeRec[0] && fabs(rPrice - grlastThreeRecPrice[0]) < 0.1*gstGlobalPara[giTradeInfo].stSec.rTickSize) return 0;

    for(i=2; i>0; i--)
    {
        gilastThreeRec[i] = gilastThreeRec[i-1];
        grlastThreeRecPrice[i] = grlastThreeRecPrice[i-1];
        gilastThreeRecTime[i] = gilastThreeRecTime[i-1];
    }
    gilastThreeRec[0] = iBS;
    grlastThreeRecPrice[0] = rPrice;
    gilastThreeRecTime[0] = gstCurrIn.iTime;

    return 0;
}

//----------------------------------------------------
// subroutine s_hi5_sig_gen()
// description: set Hi78 model signal generation
//----------------------------------------------------
int s_MK2_sig_gen(struct in_data *lstCurrInData, struct in_data *lstPreInData,
	struct all_parameter *lstGlobalPara, st_loc_trade_info *p_stLocTradeInfo,
	int liTickNo, int *laiFullOrderBookBuy, int *laiFullOrderBookSel,
	int liLenFullOrderBook, st_usr_order *stCurUsrOrder, int iCurUsrOrder,
	st_usr_order *stNewUsrOrder, int iMaxUsrOrder, int *piNewUsrOrder)
{

    m_signal = strat_get_signal(lstCurrInData, lstPreInData);

    if(lstCurrInData->iTime > lstGlobalPara->stSec.iSecFinalSqTime * 1000 + strat_time_shift)
    {
        s_MK2_eod_final_square(lstCurrInData, lstGlobalPara, p_stLocTradeInfo, liTickNo,
            stCurUsrOrder, iCurUsrOrder, stNewUsrOrder, iMaxUsrOrder, piNewUsrOrder);
    }
    else if((lstGlobalPara->stSec.iSecTradeEndTime02 == -1 && lstCurrInData->iTime >= lstGlobalPara->stSec.iSecTradeEndTime *1000 + strat_time_shift)
            || (lstGlobalPara->stSec.iSecTradeEndTime02 > 0 && lstCurrInData->iTime >= lstGlobalPara->stSec.iSecTradeEndTime02 *1000 + strat_time_shift)
            || giZhangDieTingTag == 1)
    {
        s_MK2_eod_square(lstCurrInData, lstGlobalPara, p_stLocTradeInfo, liTickNo,
            stCurUsrOrder, iCurUsrOrder, stNewUsrOrder, iMaxUsrOrder, piNewUsrOrder);
    }
    else if((lstGlobalPara->stSec.iSecTradeStartTime02 != -1
            && lstCurrInData->iTime >= lstGlobalPara->stSec.iSecTradeEndTime *1000
            && lstCurrInData->iTime < lstGlobalPara->stSec.iSecTradeStartTime02 * 1000))
    {
        s_MK2_eod_square_flat_netpos(lstCurrInData, lstGlobalPara, p_stLocTradeInfo, liTickNo,
            stCurUsrOrder, iCurUsrOrder, stNewUsrOrder, iMaxUsrOrder, piNewUsrOrder);
    }
    else
    {
        s_MK2_sig_gen_normal(lstCurrInData, lstPreInData, lstGlobalPara, p_stLocTradeInfo,
             liTickNo, laiFullOrderBookBuy, laiFullOrderBookSel, liLenFullOrderBook,
             stCurUsrOrder, iCurUsrOrder, stNewUsrOrder, iMaxUsrOrder, piNewUsrOrder);
    }

	return 0;
}

int s_MK2_sig_gen_normal(struct in_data *lstCurrInData, struct in_data *lstPreInData,
	struct all_parameter *lstGlobalPara, st_loc_trade_info *p_stLocTradeInfo,
	int liTickNo, int *laiFullOrderBookBuy, int *laiFullOrderBookSel,
	int liLenFullOrderBook, st_usr_order *stCurUsrOrder, int iCurUsrOrder,
	st_usr_order *stNewUsrOrder, int iMaxUsrOrder, int *piNewUsrOrder)
{

	double lrTick; //, lrOI;
	//double lrOrderP;
	int  i, i1;
	int inew;
	int iCurSelOrder, iCurBuyOrder;
	//int iRemainBuyOrder, iRemainSelOrder;
	double order_price;
	int cur_vol, order_vol;

	//data prepare begin
	*piNewUsrOrder = 0;
	inew = 0;
	memset(&(stNewUsrOrder[0]), 0, sizeof(st_usr_order) * iMaxUsrOrder);

	lrTick = lstGlobalPara->stSec.rTickSize;

    iCurBuyOrder = 0;
    iCurSelOrder = 0;
    for(i1=0; i1<iCurUsrOrder; i1++)
    {
        if(stCurUsrOrder[i1].iBS == USER_ORDER_SELL) iCurSelOrder ++;
        else if(stCurUsrOrder[i1].iBS == USER_ORDER_BUY) iCurBuyOrder ++;
    }
	// data prepare end


    gstMK2_Paras.iLimitSqFlg = iNO;
	if (fabs(lstCurrInData->dBP[4] - lstCurrInData->dPLmtDn) < gstMK2_Paras.iLimitSqRange * lrTick || lstCurrInData->dBP[4]<lrTick)
	{
	    gstMK2_Paras.iLimitSqFlg = iYES;
		giZhangDieTingTag = 1;
        goto END;
	}
	else if (fabs(lstCurrInData->dSP[4] - lstCurrInData->dPLmtUp) < gstMK2_Paras.iLimitSqRange * lrTick || lstCurrInData->dSP[4] < lrTick)
    {
        gstMK2_Paras.iLimitSqFlg = iYES ;
        giZhangDieTingTag = 1;
        goto END;
	}

    if (lstCurrInData->iTime < lstGlobalPara->stSec.iSecTradeStartTime * 1000)
    {
        goto END;
    }

    if(m_signal == 1)
    {
        order_price = strat_order_price; //lstCurrInData->dBP[0] + lrTick;
        cur_vol = p_stLocTradeInfo->iPosD - p_stLocTradeInfo->iPosK;
        order_vol = giMaxPos - cur_vol;
        for(i=0; i<iCurUsrOrder; i++)
        {
            if(strncmp(stCurUsrOrder[i].szContract,lstCurrInData->cTicker,8) != 0) continue;
            if(stCurUsrOrder[i].iBS == USER_ORDER_BUY && fabs(stCurUsrOrder[i].dPrice - order_price) < lrTick/2)
            {
                order_vol = order_vol - stCurUsrOrder[i].iVol;
            }
            else
            {
                stNewUsrOrder[inew].dPrice = stCurUsrOrder[i].dPrice;
                stNewUsrOrder[inew].iBS = USER_ORDER_CANCEL;
                stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_NONE;
                memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);

                inew ++;
            }
        }

        if(order_vol > 0)
        {
            stNewUsrOrder[inew].dPrice = order_price;
            stNewUsrOrder[inew].iVol = order_vol;
            stNewUsrOrder[inew].iBS = USER_ORDER_BUY;
            stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_NONE;
            memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);
            inew ++;
        }

    }
    else if(m_signal == -1)
    {
        order_price = strat_order_price;//lstCurrInData->dSP[0] - lrTick;
        cur_vol = p_stLocTradeInfo->iPosK - p_stLocTradeInfo->iPosD;
        order_vol = giMaxPos - cur_vol;
        for(i=0; i<iCurUsrOrder; i++)
        {
            if(strncmp(stCurUsrOrder[i].szContract,lstCurrInData->cTicker,8) != 0) continue;
            if(stCurUsrOrder[i].iBS == USER_ORDER_SELL && fabs(stCurUsrOrder[i].dPrice - order_price) < lrTick/2)
            {
                order_vol = order_vol - stCurUsrOrder[i].iVol;
            }
            else
            {
                stNewUsrOrder[inew].dPrice = stCurUsrOrder[i].dPrice;
                stNewUsrOrder[inew].iBS = USER_ORDER_CANCEL;
                stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_NONE;
                memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);

                inew ++;
            }
        }

        if(order_vol > 0)
        {
            stNewUsrOrder[inew].dPrice = order_price;
            stNewUsrOrder[inew].iVol = order_vol;
            stNewUsrOrder[inew].iBS = USER_ORDER_SELL;
            stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_NONE;
            memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);
            inew ++;
        }

    }
    else if(m_signal == 2)
    {
        order_price = strat_order_price; //lstCurrInData->dBP[0] + lrTick;
        cur_vol = p_stLocTradeInfo->iPosK - p_stLocTradeInfo->iPosD;
        order_vol =  cur_vol;
        for(i=0; i<iCurUsrOrder; i++)
        {
            if(strncmp(stCurUsrOrder[i].szContract,lstCurrInData->cTicker,8) != 0) continue;
            if(stCurUsrOrder[i].iBS == USER_ORDER_SELL)
            {
                stNewUsrOrder[inew].dPrice = stCurUsrOrder[i].dPrice;
                stNewUsrOrder[inew].iBS = USER_ORDER_CANCEL;
                stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_NONE;
                memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);

                inew ++;
            }
            else if(stCurUsrOrder[i].iBS == USER_ORDER_BUY && stCurUsrOrder[i].iOpenFlat == USER_ORDER_OPT_FLAT
                    && fabs(stCurUsrOrder[i].dPrice - order_price) < lrTick/2)
            {
                order_vol = order_vol - stCurUsrOrder[i].iVol;
            }
            else if(stCurUsrOrder[i].iBS == USER_ORDER_BUY && stCurUsrOrder[i].iOpenFlat == USER_ORDER_OPT_FLAT)
            {
                stNewUsrOrder[inew].dPrice = stCurUsrOrder[i].dPrice;
                stNewUsrOrder[inew].iBS = USER_ORDER_CANCEL;
                stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_NONE;
                memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);

                inew ++;
            }

        }

        if(order_vol > 0)
        {
            stNewUsrOrder[inew].dPrice = order_price;
            stNewUsrOrder[inew].iVol = order_vol;
            stNewUsrOrder[inew].iBS = USER_ORDER_BUY;
            stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_FLAT;
            memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);
            inew ++;
        }

    }
    else if(m_signal == -2)
    {
        order_price = strat_order_price; //lstCurrInData->dSP1 - lrTick;
        cur_vol = p_stLocTradeInfo->iPosD - p_stLocTradeInfo->iPosK;
        order_vol =  cur_vol;
        for(i=0; i<iCurUsrOrder; i++)
        {
            if(strncmp(stCurUsrOrder[i].szContract,lstCurrInData->cTicker,8) != 0) continue;
            if(stCurUsrOrder[i].iBS == USER_ORDER_BUY)
            {
                stNewUsrOrder[inew].dPrice = stCurUsrOrder[i].dPrice;
                stNewUsrOrder[inew].iBS = USER_ORDER_CANCEL;
                stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_NONE;
                memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);

                inew ++;
            }
            else if(stCurUsrOrder[i].iBS == USER_ORDER_SELL && stCurUsrOrder[i].iOpenFlat == USER_ORDER_OPT_FLAT
                    && fabs(stCurUsrOrder[i].dPrice - order_price) < lrTick/2)
            {
                order_vol = order_vol - stCurUsrOrder[i].iVol;
            }
            else if(stCurUsrOrder[i].iBS == USER_ORDER_SELL && stCurUsrOrder[i].iOpenFlat == USER_ORDER_OPT_FLAT)
            {
                stNewUsrOrder[inew].dPrice = stCurUsrOrder[i].dPrice;
                stNewUsrOrder[inew].iBS = USER_ORDER_CANCEL;
                stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_NONE;
                memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);

                inew ++;
            }
        }

        if(order_vol > 0)
        {
            stNewUsrOrder[inew].dPrice = order_price;
            stNewUsrOrder[inew].iVol = order_vol;
            stNewUsrOrder[inew].iBS = USER_ORDER_SELL;
            stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_FLAT;
            memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);
            inew ++;
        }

    }


END:    *piNewUsrOrder = inew;

//    if(lstCurrInData->iTime > 92500000)
//    {
//        printf("%d current order:\n",lstCurrInData->iTime);
//        for(i=0; i<iCurUsrOrder; i++ )
//        {
//            printf("%0.3f %d %d %d %d %d\n", stCurUsrOrder[i].dPrice, stCurUsrOrder[i].iBS, stCurUsrOrder[i].iVol,
//                   stCurUsrOrder[i].iOpenFlat, stCurUsrOrder[i].sig_time, stCurUsrOrder[i].sig_id);
//        }
//        printf("new order:\n");
//        for(i=0; i<inew; i++ )
//        {
//            printf("%0.3f %d %d %d %d %d\n", stNewUsrOrder[i].dPrice, stNewUsrOrder[i].iBS, stNewUsrOrder[i].iVol,
//                   stNewUsrOrder[i].iOpenFlat, stNewUsrOrder[i].sig_time, stNewUsrOrder[i].sig_id);
//        }
//        getchar();
//    }

    return 0;
}

int s_generate_MK2_cancel_orders(double *larBP, double *larSP, double lrTick, int iCurTime,
    st_loc_trade_info *p_stLocTradeInfo, st_usr_order *stCurUsrOrder, int iCurUsrOrder,
	st_usr_order *stNewUsrOrder, int iMaxUsrOrder, int *piNewUsrOrder)
{

    *piNewUsrOrder = 0;

    return 0;
}


int get_delayed_time(int itime, int iDelayMin)
{
    int ihour, imin;

    ihour = itime/10000000;
    imin = (itime%10000000)/100000;

    imin = imin + iDelayMin;
    if(imin>=60)
    {
        ihour = ihour + 1;
        imin = imin - 60;
    }

    return ihour*10000000 + imin*100000 + itime%100000;
}


//----------------------------------------------------
// subroutine s_hi5_eod_square()
// description: square at the end of the market
//----------------------------------------------------
int s_MK2_eod_square(const struct in_data *lstCurrInData, const struct all_parameter *lstGlobalPara,
	st_loc_trade_info *p_stLocTradeInfo, const int liTickNo,
	st_usr_order *stCurUsrOrder, int iCurUsrOrder,
	st_usr_order *stNewUsrOrder, int iMaxUsrOrder, int *piNewUsrOrder)
{
	int i;
	int inew;
	int iFlatVol;

	*piNewUsrOrder = 0;
	inew = 0;

	iFlatVol = 0;
    for(i=0; i<iCurUsrOrder; i++)
    {
        if(strncmp(stCurUsrOrder[i].szContract,lstCurrInData->cTicker,8) != 0) continue;
        if(stCurUsrOrder[i].iBS == USER_ORDER_SELL)
        {
            if(fabs(stCurUsrOrder[i].dPrice - lstCurrInData->dSP[0]) > 0.01 || stCurUsrOrder[i].iOpenFlat != USER_ORDER_OPT_FLAT)
            {
                stNewUsrOrder[inew].dPrice = stCurUsrOrder[i].dPrice;
                stNewUsrOrder[inew].iBS = USER_ORDER_CANCEL;
                stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_NONE;
                memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);

                inew ++;
            }
            else
            {
                iFlatVol = stCurUsrOrder[i].iVol;
            }
        }
    }
    if(p_stLocTradeInfo->iPosD - iFlatVol > 0)
    {
        stNewUsrOrder[inew].dPrice = lstCurrInData->dSP[0];
        stNewUsrOrder[inew].iBS = USER_ORDER_SELL;
        stNewUsrOrder[inew].iVol = p_stLocTradeInfo->iPosD - iFlatVol;
        stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_FLAT;
        memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);

        inew ++;
    }

    iFlatVol = 0;
    for(i=0; i<iCurUsrOrder; i++)
    {
        if(strncmp(stCurUsrOrder[i].szContract,lstCurrInData->cTicker,8) != 0) continue;
        if(stCurUsrOrder[i].iBS == USER_ORDER_BUY)
        {
            if(fabs(stCurUsrOrder[i].dPrice - lstCurrInData->dBP[0]) > 0.01 || stCurUsrOrder[i].iOpenFlat != USER_ORDER_OPT_FLAT)
            {
                stNewUsrOrder[inew].dPrice = stCurUsrOrder[i].dPrice;
                stNewUsrOrder[inew].iBS = USER_ORDER_CANCEL;
                stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_NONE;
                memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);

                inew ++;
            }
            else
            {
                iFlatVol = stCurUsrOrder[i].iVol;
            }
        }
    }
    if(p_stLocTradeInfo->iPosK - iFlatVol > 0)
    {
        stNewUsrOrder[inew].dPrice = lstCurrInData->dBP[0];
        stNewUsrOrder[inew].iBS = USER_ORDER_BUY;
        stNewUsrOrder[inew].iVol = p_stLocTradeInfo->iPosK - iFlatVol;
        stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_FLAT;
        memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);

        inew ++;
    }

    *piNewUsrOrder = inew;
	return 0;
}

int s_MK2_eod_square_flat_netpos(const struct in_data *lstCurrInData, const struct all_parameter *lstGlobalPara,
	st_loc_trade_info *p_stLocTradeInfo, const int liTickNo,
	st_usr_order *stCurUsrOrder, int iCurUsrOrder,
	st_usr_order *stNewUsrOrder, int iMaxUsrOrder, int *piNewUsrOrder)
{
	int i;
	int inew;
	int iFlatVol;

	*piNewUsrOrder = 0;
	inew = 0;

	iFlatVol = 0;
    for(i=0; i<iCurUsrOrder; i++)
    {
        if(strncmp(stCurUsrOrder[i].szContract,lstCurrInData->cTicker,8) != 0) continue;
        if(stCurUsrOrder[i].iBS == USER_ORDER_SELL)
        {
            if(fabs(stCurUsrOrder[i].dPrice - lstCurrInData->dSP[0]) > 0.01 || stCurUsrOrder[i].iOpenFlat != USER_ORDER_OPT_FLAT)
            {
                stNewUsrOrder[inew].dPrice = stCurUsrOrder[i].dPrice;
                stNewUsrOrder[inew].iBS = USER_ORDER_CANCEL;
                stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_NONE;
                memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);

                inew ++;
            }
            else
            {
                iFlatVol = stCurUsrOrder[i].iVol;
            }
        }
    }
    if((p_stLocTradeInfo->iPosD - p_stLocTradeInfo->iPosK)- iFlatVol > 0)
    {
        stNewUsrOrder[inew].dPrice = lstCurrInData->dSP[0];
        stNewUsrOrder[inew].iBS = USER_ORDER_SELL;
        stNewUsrOrder[inew].iVol = (p_stLocTradeInfo->iPosD - p_stLocTradeInfo->iPosK)- iFlatVol;
        stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_FLAT;
        memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);

        inew ++;
    }

    iFlatVol = 0;
    for(i=0; i<iCurUsrOrder; i++)
    {
        if(strncmp(stCurUsrOrder[i].szContract,lstCurrInData->cTicker,8) != 0) continue;
        if(stCurUsrOrder[i].iBS == USER_ORDER_BUY)
        {
            if(fabs(stCurUsrOrder[i].dPrice - lstCurrInData->dBP[0]) > 0.01 || stCurUsrOrder[i].iOpenFlat != USER_ORDER_OPT_FLAT)
            {
                stNewUsrOrder[inew].dPrice = stCurUsrOrder[i].dPrice;
                stNewUsrOrder[inew].iBS = USER_ORDER_CANCEL;
                stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_NONE;
                memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);

                inew ++;
            }
            else
            {
                iFlatVol = stCurUsrOrder[i].iVol;
            }
        }
    }
    if((p_stLocTradeInfo->iPosK - p_stLocTradeInfo->iPosD)- iFlatVol > 0)
    {
        stNewUsrOrder[inew].dPrice = lstCurrInData->dBP[0];
        stNewUsrOrder[inew].iBS = USER_ORDER_BUY;
        stNewUsrOrder[inew].iVol = (p_stLocTradeInfo->iPosK - p_stLocTradeInfo->iPosD)- iFlatVol;
        stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_FLAT;
        memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);

        inew ++;
    }

    *piNewUsrOrder = inew;
	return 0;
}

int s_MK2_eod_final_square(const struct in_data *lstCurrInData, const struct all_parameter *lstGlobalPara,
	st_loc_trade_info *p_stLocTradeInfo, const int liTickNo,
	st_usr_order *stCurUsrOrder, int iCurUsrOrder,
	st_usr_order *stNewUsrOrder, int iMaxUsrOrder, int *piNewUsrOrder)
{
	int i;
	int inew;
	int iFlatVol;

	*piNewUsrOrder = 0;
	inew = 0;

	iFlatVol = 0;
    for(i=0; i<iCurUsrOrder; i++)
    {
        if(strncmp(stCurUsrOrder[i].szContract,lstCurrInData->cTicker,8) != 0) continue;
        if(stCurUsrOrder[i].iBS == USER_ORDER_SELL)
        {
            stNewUsrOrder[inew].dPrice = stCurUsrOrder[i].dPrice;
            stNewUsrOrder[inew].iBS = USER_ORDER_CANCEL;
            stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_NONE;
            memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);

            inew ++;
        }
    }
    if(p_stLocTradeInfo->iPosD - iFlatVol > 0)
    {
        stNewUsrOrder[inew].dPrice = lstCurrInData->dBP[0];
        stNewUsrOrder[inew].iBS = USER_ORDER_SELL;
        stNewUsrOrder[inew].iVol = p_stLocTradeInfo->iPosD - iFlatVol;
        stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_FLAT;
        memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);

        inew ++;
    }

    iFlatVol = 0;
    for(i=0; i<iCurUsrOrder; i++)
    {
        if(strncmp(stCurUsrOrder[i].szContract,lstCurrInData->cTicker,8) != 0) continue;
        if(stCurUsrOrder[i].iBS == USER_ORDER_BUY)
        {
            stNewUsrOrder[inew].dPrice = stCurUsrOrder[i].dPrice;
            stNewUsrOrder[inew].iBS = USER_ORDER_CANCEL;
            stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_NONE;
            memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);

            inew ++;
        }
    }
    if(p_stLocTradeInfo->iPosK - iFlatVol > 0)
    {
        stNewUsrOrder[inew].dPrice = lstCurrInData->dSP[0];
        stNewUsrOrder[inew].iBS = USER_ORDER_BUY;
        stNewUsrOrder[inew].iVol = p_stLocTradeInfo->iPosK - iFlatVol;
        stNewUsrOrder[inew].iOpenFlat = USER_ORDER_OPT_FLAT;
        memcpy(stNewUsrOrder[inew].szContract,lstCurrInData->cTicker,sizeof(char)*8);

        inew ++;
    }

    *piNewUsrOrder = inew;
	return 0;
}

int s_MK2_run_each_record(struct signal_t *lastStructTSignal, int *piNoTSignal,struct strat_out_log *outlog)
{
    int iTemp;
    int i;

    *piNoTSignal = 0;
    memset(outlog, 0, sizeof(struct strat_out_log));

    giTradeInfo = -1;
	for(i=0; i<giNumContract; i++)
    {
        if(strncmp(gstLocTradeInfo[i].szContract, gstCurrIn.cTicker, 16) == 0)
        {
            giTradeInfo = i;
            break;
        }
    }
    if(giTradeInfo < 0) goto END2;

    gcExch = gstLocTradeInfo[giTradeInfo].cExch;
    memcpy(gcSecName,gstLocTradeInfo[giTradeInfo].szContract,sizeof(char)*8);

	// init  variables
	if (giInitFlg[giTradeInfo] == 0)
	{
		if (gstCurrIn.dPLmtUp < 0.001 || gstCurrIn.dPLmtDn < 0.001) goto END2;
		if (gstLocTradeInfo[giTradeInfo].cExch == cSHFE && gstCurrIn.iData_Flag == 2) goto END2;
		if (giNightMarket == 0 && (gstCurrIn.iTime<85500000 || gstCurrIn.iTime >= 153000000)) goto END2;
		if (giNightMarket == 1 && (gstCurrIn.iTime<205500000)) goto END2;

		//s_set_global_parameters(gcExch, gcSecName);
		s_MK2_variable_init(&gstCurrIn, &gstGlobalPara[giTradeInfo]);

		memcpy(&gstPreIn[giTradeInfo], (void*)(&gstCurrIn), sizeof(struct in_data));
		memcpy(&gstRecPreIn[giTradeInfo], (void*)(&gstRecCurrIn), sizeof(struct in_data));
		giInitFlg[giTradeInfo] = 1;
	}

	if (gstLocTradeInfo[giTradeInfo].cExch != cCZCE)
	{
		gstCurrIn.iVol = (int)(gstCurrIn.dTotV - gstPreIn[giTradeInfo].dTotV)/gstGlobalPara[giTradeInfo].stExch.iVolDivider;
		gstCurrIn.dVal = (gstCurrIn.dTotVal - gstPreIn[giTradeInfo].dTotVal)/gstGlobalPara[giTradeInfo].stExch.iVolDivider;
		gstCurrIn.dOIChg = (gstCurrIn.dOI - gstPreIn[giTradeInfo].dOI)/gstGlobalPara[giTradeInfo].stExch.iVolDivider;
		gstCurrIn.dAvgP = f_calc_curr_avgp(gstCurrIn.dVal, gstCurrIn.iVol, gstPreIn[giTradeInfo].dAvgP, &gstGlobalPara[giTradeInfo])
            /gstGlobalPara[giTradeInfo].stSec.iUnitPerContract;
	}
	else
	{

        gstCurrIn.iVol = (int)(gstCurrIn.dTotV - gstPreIn[giTradeInfo].dTotV)/gstGlobalPara[giTradeInfo].stExch.iVolDivider;
		gstCurrIn.dOIChg = (gstCurrIn.dOI - gstPreIn[giTradeInfo].dOI)/gstGlobalPara[giTradeInfo].stExch.iVolDivider;
		gstCurrIn.dAvgP = gstCurrIn.dP ;
		gstCurrIn.dVal = 0;
	}
#ifdef USE_SHFE_LV1_AND_DEEP
    if (gstCurrIn.iData_Flag == 1 && gstLocTradeInfo[giTradeInfo].cExch == cSHFE)
    {
        iTemp = s_file_sh_flag_1_data(&gstCurrIn, &gstPreIn[giTradeInfo]);
        if(iTemp == 1) goto END2;
        if(transfer_2_millisec(gstCurrIn.iTime)-transfer_2_millisec(gstPreIn[giTradeInfo].iTime) > 900) goto END2;
    }
    else if (gstCurrIn.iData_Flag == 2 && gstLocTradeInfo[giTradeInfo].cExch == cSHFE)
    {
        goto END2;
    }
#else
    if (gstCurrIn.iData_Flag != 3 && gstLocTradeInfo[giTradeInfo].cExch == cSHFE) goto END2;
#endif // USE_SHFE_LV1_AND_DEEP

	if (f_check_valid_data(&gstCurrIn, &gstRecPreIn[giTradeInfo], giTickNo, &gstGlobalPara[giTradeInfo]) == iNO) goto END2;   // check whether data is valid

	giTickNo = giTickNo + 1;  // tick number add 1

    s_update_full_order_book(&gstCurrIn, &gstPreIn[giTradeInfo], &gstGlobalPara[giTradeInfo],
           gaiFullOrderBookBuy[giTradeInfo], gaiFullOrderBookSel[giTradeInfo], giLenFullOrderBook[giTradeInfo]);

	if ((gstCurrIn.iTime < gstGlobalPara[giTradeInfo].stExch.iMktStartTime * 1000) ||
		(gstCurrIn.iTime > gstGlobalPara[giTradeInfo].stExch.iMktEndTime * 1000)) goto END;
	if (giTickNo <= giNumContract && gstCurrIn.iTime >= gstGlobalPara[giTradeInfo].stExch.iMktStartTime * 1000) goto END; // used data is from second tick



	gstLocTradeInfo[giTradeInfo].iTotalCancelSize =
        gstLocTradeInfo[giTradeInfo].iTotalCancelSize + gstLocTradeInfo[giTradeInfo].iCurrCancelSize;
	gstLocTradeInfo[giTradeInfo].iTotalOrderSize =
        gstLocTradeInfo[giTradeInfo].iTotalOrderSize + gstLocTradeInfo[giTradeInfo].iCurrOrderSize;

	giLast3Vol[1] = giLast3Vol[0];
	giLast3Vol[2] = giLast3Vol[1];
	giLast3Vol[0] = gstCurrIn.iVol;

	gstLocTradeInfo[giTradeInfo].iCurrCancelSize = 0;
    gstLocTradeInfo[giTradeInfo].iCurrOrderSize = 0;

    gstLocTradeInfo[giTradeInfo].rLivePL = gstLocTradeInfo[giTradeInfo].rCash +
        (gstLocTradeInfo[giTradeInfo].iPosD - gstLocTradeInfo[giTradeInfo].iPosK)* gstCurrIn.dP;
    gstLocTradeInfo[giTradeInfo].rMaxLivePL = MAX(gstLocTradeInfo[giTradeInfo].rMaxLivePL, gstLocTradeInfo[giTradeInfo].rLivePL);
    gstLocTradeInfo[giTradeInfo].rLiveDD = gstLocTradeInfo[giTradeInfo].rLivePL - gstLocTradeInfo[giTradeInfo].rMaxLivePL;
    gstLocTradeInfo[giTradeInfo].rMaxLiveDD = MIN(gstLocTradeInfo[giTradeInfo].rMaxLiveDD, gstLocTradeInfo[giTradeInfo].rLiveDD);

    gstLocTradeInfo[giTradeInfo].iMaxSidePos =
        MAX(MAX(gstLocTradeInfo[giTradeInfo].iPosD, gstLocTradeInfo[giTradeInfo].iPosK),gstLocTradeInfo[giTradeInfo].iMaxSidePos);

    iTemp = gstLocTradeInfo[giTradeInfo].iPosD - gstLocTradeInfo[giTradeInfo].iPosK;
    if(abs(gstLocTradeInfo[giTradeInfo].iMaxNetPos) < abs(iTemp)) gstLocTradeInfo[giTradeInfo].iMaxNetPos = iTemp;

	// calculate PNL
	if (giRunModeFlg == 1)  // for real run mode
	{
	}
	else  // for simulation
	{
		grSimuValueTemp = grSimuValueTot + gstLocTradeInfo[giTradeInfo].rLivePL;
		grSimuValueMax = MAX(grSimuValueMax, grSimuValueTemp);
		grSimuDrawDown = MIN(grSimuDrawDown, grSimuValueTemp - grSimuValueMax);
		grSimuDrawDownRatio = MIN(grSimuDrawDownRatio, grSimuDrawDown/(grSimuValueMax + grTotBench));
	}

	//Update user current order list
	s_update_current_usr_order(gstUserCurOrder, giMaxUsrOrder, &giUserCurOrder);

	// generate signal
	s_MK2_sig_gen(&gstCurrIn, &gstPreIn[giTradeInfo], &gstGlobalPara[giTradeInfo], &gstLocTradeInfo[giTradeInfo], giTickNo,
               gaiFullOrderBookBuy[giTradeInfo], gaiFullOrderBookSel[giTradeInfo],giLenFullOrderBook[giTradeInfo],
               gstUserCurOrder, giUserCurOrder, gstUserNewOrder, giMaxUsrOrder, &giUserNewOrder);

    s_update_my_order_from_usr_order(gstUserNewOrder, giUserNewOrder, &gstCurrIn,
        gaiFullOrderBookBuy[giTradeInfo], gaiFullOrderBookSel[giTradeInfo], &gstGlobalPara[giTradeInfo],
        &gstLocTradeInfo[giTradeInfo]);

    // send signal
	s_mm_assign_out_tsignal(lastStructTSignal, piNoTSignal, &gstLocTradeInfo[giTradeInfo]);

	// write log file
	if (giWriteOutputFileFlg == 1)
	{
		s_output_log(giTickNo, pfDayLogFile, gcFileName, outlog);
	}
END:
    if (gstLocTradeInfo[giTradeInfo].cExch != cSHFE || gstCurrIn.iData_Flag == 3)
    {
        memcpy(&gstPreIn[giTradeInfo], (void*)&gstCurrIn, sizeof(struct in_data));
        memcpy(&gstRecPreIn[giTradeInfo], (void*)&gstRecCurrIn, sizeof(struct in_data));
        if(giLenFullOrderBook[giTradeInfo] != 0)
        {
            memcpy(gaiPreOrderBookBuy[giTradeInfo], gaiFullOrderBookBuy[giTradeInfo], sizeof(int)*giLenFullOrderBook[giTradeInfo]);
            memcpy(gaiPreOrderBookSel[giTradeInfo], gaiFullOrderBookSel[giTradeInfo], sizeof(int)*giLenFullOrderBook[giTradeInfo]);
        }
    }
END2:
	//reset_exe_order_rec();
	return 0;
}

int s_MK2_variable_init(struct in_data *p_stCurrInData, struct all_parameter *p_stGlobalPara)
{
    time_t cur_time;
    char tmp[64];

    s_MK2_common_variable_init(p_stCurrInData, p_stGlobalPara);

    grTotBench = 1;
    if (giRunModeFlg == 1) strat_init();

    cur_time = time(0);
    strftime( tmp, sizeof(tmp), "%Y%m%d",localtime(&cur_time) );
    if(atoi(tmp) >= 20180531) m_thre_open = 20.0;

	return 0;
}


int s_MK2_common_variable_init(struct in_data *p_stCurrInData, struct all_parameter *p_stGlobalPara)
{
	giLenFullOrderBook[giTradeInfo] = (int)((p_stCurrInData->dPLmtUp - p_stCurrInData->dPLmtDn)/ p_stGlobalPara->stSec.rTickSize + 0.5)+1;

	if (gaiFullOrderBookBuy[giTradeInfo] != NULL)
	{
		free(gaiFullOrderBookBuy[giTradeInfo]); gaiFullOrderBookBuy[giTradeInfo] = NULL;
 	}
	gaiFullOrderBookBuy[giTradeInfo] = (int*)malloc(giLenFullOrderBook[giTradeInfo] * sizeof(int));
	memset(gaiFullOrderBookBuy[giTradeInfo],0,giLenFullOrderBook[giTradeInfo] * sizeof(int));

	if (gaiFullOrderBookSel[giTradeInfo] != NULL)
	{
		free(gaiFullOrderBookSel[giTradeInfo]); gaiFullOrderBookSel[giTradeInfo] = NULL;
	}
	gaiFullOrderBookSel[giTradeInfo] = (int*)malloc(giLenFullOrderBook[giTradeInfo] * sizeof(int));
	memset(gaiFullOrderBookSel[giTradeInfo],0,giLenFullOrderBook[giTradeInfo] * sizeof(int));

	if (gaiPreOrderBookBuy[giTradeInfo] != NULL)
	{
		free(gaiPreOrderBookBuy[giTradeInfo]); gaiPreOrderBookBuy[giTradeInfo] = NULL;
	}
	gaiPreOrderBookBuy[giTradeInfo] = (int*)malloc(giLenFullOrderBook[giTradeInfo] * sizeof(int));
	memset(gaiPreOrderBookBuy[giTradeInfo],0,giLenFullOrderBook[giTradeInfo] * sizeof(int));

	if (gaiPreOrderBookSel[giTradeInfo] != NULL)
	{
		free(gaiPreOrderBookSel[giTradeInfo]); gaiPreOrderBookSel[giTradeInfo] = NULL;
	}
	gaiPreOrderBookSel[giTradeInfo] = (int*)malloc(giLenFullOrderBook[giTradeInfo] * sizeof(int));
	memset(gaiPreOrderBookSel[giTradeInfo],0,giLenFullOrderBook[giTradeInfo] * sizeof(int));

	p_stCurrInData->iTotalBuyVol = 0;
	p_stCurrInData->iTotalSelVol = 0;
	p_stCurrInData->dWeightedBuyOrderP = 0;
	p_stCurrInData->dWeightedSelOrderP = 0;

	return 0;
}


int s_MK2_output_log_new(int liTickNo, struct strat_out_log *outlog)
{

	// common output
	m_strat_log.exch_time = gstCurrIn.iTime;
	strncpy(m_strat_log.contract, gstCurrIn.cTicker, 15);
	m_strat_log.n_tick = liTickNo;
	m_strat_log.price = gstCurrIn.dP;
	m_strat_log.vol = gstCurrIn.iVol;
    m_strat_log.bv1 = gstCurrIn.iBV[0];
    m_strat_log.bp1 = gstCurrIn.dBP[0];
    m_strat_log.sp1 = gstCurrIn.dSP[0];
    m_strat_log.sv1 = gstCurrIn.iSV[0];
    m_strat_log.amt = (long)gstCurrIn.dVal;
    m_strat_log.oi = (long)gstCurrIn.dOI;

    if(gstLastRec.iBS == 1)
    {
        m_strat_log.buy_price = gstLastRec.rPrice;
        m_strat_log.sell_price = 0.;
    }
    else if(gstLastRec.iBS == -1)
    {
        m_strat_log.buy_price = 0.;
        m_strat_log.sell_price = gstLastRec.rPrice;
    }
    if(gstLastRec.iOpenFlat==SIG_ACTION_OPEN)
    {
        m_strat_log.open_vol = gstLastRec.iVol;
    }
    else if(gstLastRec.iOpenFlat==SIG_ACTION_FLAT)
    {
        m_strat_log.close_vol = gstLastRec.iVol;
    }

    m_strat_log.long_pos = gstLocTradeInfo[giTradeInfo].iPosD;
	m_strat_log.short_pos = gstLocTradeInfo[giTradeInfo].iPosK;
	m_strat_log.tot_ordervol = gstLocTradeInfo[giTradeInfo].iTotalOrderSize;
    m_strat_log.tot_cancelvol = gstLocTradeInfo[giTradeInfo].iTotalCancelSize;
    m_strat_log.order_cnt = gstLocTradeInfo[giTradeInfo].iTotalOrderCount;
    m_strat_log.cancel_cnt = gstLocTradeInfo[giTradeInfo].iTotalCancelCount;
    m_strat_log.cash = gstLocTradeInfo[giTradeInfo].rCash;
    m_strat_log.live = gstLocTradeInfo[giTradeInfo].rLivePL ;
    m_strat_log.tot_vol = gstLocTradeInfo[giTradeInfo].iTotalVol;
    m_strat_log.max_dd = gstLocTradeInfo[giTradeInfo].rMaxLiveDD;
    m_strat_log.max_net_pos = gstLocTradeInfo[giTradeInfo].iMaxNetPos;
    m_strat_log.max_side_pos = gstLocTradeInfo[giTradeInfo].iMaxSidePos;
    m_strat_log.sig[0] = gstCurrIn.dAvgP;
    m_strat_log.sig[1] = gstGlobalPara[giTradeInfo].stSec.rTickSize;
    m_strat_log.sig[2] = gstGlobalPara[giTradeInfo].stSec.iUnitPerContract;


    memcpy(outlog, &m_strat_log, sizeof(struct strat_out_log));
	return 0;
}

int s_MK2_set_parameters()
{
	return 0;
}

