#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <memory.h>
#include "common.h"
#include "common_mod.h"
#include "common_func.h"
#include "common_mm.h"
#include "interface_v30.h"
#include "MK2_var_def.h"
#include "MK2_func.h"


st_usr_order gstUserNewOrder[MAX_ORDER_SIZE*4], gstUserCurOrder[MAX_ORDER_SIZE*4];
int giUserNewOrder, giUserCurOrder, giMaxUsrOrder;

st_order_info gstOrderBuy[MAX_ORDER_SIZE], gstOrderSel[MAX_ORDER_SIZE], gstOrderCancel[MAX_ORDER_SIZE*2];
int giMaxMyOrder;

int gstCurrSigId;
struct struct_exe_order_rec gstLastRec;


void s_initializer_usr_order()
{
    giUserNewOrder = 0;
    giUserCurOrder = 0;
    giMaxUsrOrder = MAX_ORDER_SIZE*4;

    memset(&(gstUserNewOrder[0]), 0, sizeof(st_usr_order)*giMaxUsrOrder);
    memset(&(gstUserCurOrder[0]), 0, sizeof(st_usr_order)*giMaxUsrOrder);

    reset_exe_order_rec();
}

void s_initializer_my_order()
{
    gstCurrSigId = 1;
    giMaxMyOrder = MAX_ORDER_SIZE;

    memset(&(gstOrderBuy[0]), 0, sizeof(st_order_info)*giMaxMyOrder);
    memset(&(gstOrderSel[0]), 0, sizeof(st_order_info)*giMaxMyOrder);
    memset(&(gstOrderCancel[0]), 0, sizeof(st_order_info)*giMaxMyOrder*2);
}

void reset_exe_order_rec()
{
    memset(&gstLastRec, 0, sizeof(struct struct_exe_order_rec));
}

int s_check_usr_order(struct in_data *lstCurrInData, st_usr_order *gstUsrOrderIn, const int giUsrOrderIn)
{
    int i, i1;

    for(i=0; i<giUsrOrderIn; i++)
    {
        if(gstUsrOrderIn[i].dPrice > lstCurrInData->dPLmtUp || gstUsrOrderIn[i].dPrice < lstCurrInData->dPLmtDn)
        {
            memset(&(gstUsrOrderIn[i]), 0, sizeof(st_usr_order));
        }
    }

    for(i=0; i<giUsrOrderIn-1; i++)
    for(i1=i+1; i1<giUsrOrderIn; i1++)
    {
        if(gstUsrOrderIn[i].iBS == 0) continue;

        if(gstUsrOrderIn[i].dPrice == gstUsrOrderIn[i1].dPrice &&
           strncmp(gstUsrOrderIn[i].szContract, gstUsrOrderIn[i1].szContract,16)== 0)
        {
            if(gstUsrOrderIn[i].iBS == USER_ORDER_CANCEL && gstUsrOrderIn[i1].iBS == USER_ORDER_CANCEL)
            {
                memset(&(gstUsrOrderIn[i1]), 0, sizeof(st_usr_order));
            }
            if((gstUsrOrderIn[i].iBS == USER_ORDER_BUY && gstUsrOrderIn[i1].iBS == USER_ORDER_SELL)
                || (gstUsrOrderIn[i1].iBS == USER_ORDER_BUY && gstUsrOrderIn[i].iBS == USER_ORDER_SELL))
            {
                if(gstUsrOrderIn[i].iVol > gstUsrOrderIn[i1].iVol)
                {
                    gstUsrOrderIn[i].iVol = gstUsrOrderIn[i].iVol - gstUsrOrderIn[i1].iVol;
                    memset(&(gstUsrOrderIn[i1]), 0, sizeof(st_usr_order));
                }
                else if(gstUsrOrderIn[i].iVol < gstUsrOrderIn[i1].iVol)
                {
                    gstUsrOrderIn[i1].iVol = gstUsrOrderIn[i1].iVol - gstUsrOrderIn[i].iVol;
                    memset(&(gstUsrOrderIn[i]), 0, sizeof(st_usr_order));
                }
                else
                {
                    memset(&(gstUsrOrderIn[i1]), 0, sizeof(st_usr_order));
                    memset(&(gstUsrOrderIn[i]), 0, sizeof(st_usr_order));
                }
            }
        }
    }

    return 0;
}

int s_update_my_order_from_usr_order(st_usr_order *gstUsrOrderIn, const int giUsrOrderIn,
         struct in_data *lstCurrInData, int *laiFullOrderBookBuy, int *laiFullOrderBookSel,
         struct all_parameter *lstGlobalPara, st_loc_trade_info *p_stLocTradeInfo)
{
    int  i, i1;
    int liIndex;
	//int icnt_long[2],icnt_short[2];
	int iVol_target_long[2], iVol_target_short[2];
	double lrTick;

	p_stLocTradeInfo->iCurrOrderSize = 0;
	p_stLocTradeInfo->iCurrCancelSize = 0;

	if(giUsrOrderIn == 0)
    {
        //s_replace_flat_order(lstCurrInData, laiFullOrderBookBuy, laiFullOrderBookSel,
        //    lstGlobalPara, p_stLocTradeInfo);

        return 0;
    }

	lrTick = lstGlobalPara->stSec.rTickSize;

	s_check_usr_order(lstCurrInData, gstUsrOrderIn, giUsrOrderIn);

    iVol_target_long[0] = p_stLocTradeInfo->iPosK;
    iVol_target_short[0] = p_stLocTradeInfo->iPosD;

    for(i1=0; i1<giUsrOrderIn; i1++)
    {
        if(gstUsrOrderIn[i1].dPrice < lrTick) continue;

        if(gstUsrOrderIn[i1].iBS == USER_ORDER_BUY && gstUsrOrderIn[i1].iOpenFlat == USER_ORDER_OPT_FLAT
           && iVol_target_long[0] > 0) // order buy
        {
            for(i=0; i<giMaxMyOrder; i++)
            {
                if(gstOrderBuy[i].sig_id == 0)
                {
                    //printf("1 gstCurrSigId %d ++ \n", gstCurrSigId);
                    gstCurrSigId ++;
                    gstOrderBuy[i].sig_id = gstCurrSigId;

                    gstOrderBuy[i].kaiping = SIG_ACTION_FLAT;
                    gstOrderBuy[i].price = gstUsrOrderIn[i1].dPrice;
                    gstOrderBuy[i].volume = gstUsrOrderIn[i1].iVol;
                    iVol_target_long[0] = iVol_target_long[0] - gstOrderBuy[i].volume;

                    liIndex = (int)((gstOrderBuy[i].price - lstCurrInData->dPLmtDn)/lrTick+0.5);
                    gstOrderBuy[i].vol2Pass = laiFullOrderBookBuy[liIndex];
                    gstOrderBuy[i].simu_status = 1;
                    gstOrderBuy[i].status = SIG_STATUS_INIT;
                    gstOrderBuy[i].exec_acc_vol = 0;
                    gstOrderBuy[i].exec_avg_price = 0;
                    gstOrderBuy[i].direct = SIG_DIRECTION_BUY;
                    gstOrderBuy[i].itime = lstCurrInData->iTime;
                    memcpy(gstOrderBuy[i].szContract, gstUsrOrderIn[i1].szContract,sizeof(char)*16);

                    p_stLocTradeInfo->iCurrOrderSize += gstOrderBuy[i].volume;

                    gstUsrOrderIn[i1].iVol = gstUsrOrderIn[i1].iVol - gstOrderBuy[i].volume;
                    break;
                }
            }
        }
        else if(gstUsrOrderIn[i1].iBS == USER_ORDER_BUY && gstUsrOrderIn[i1].iOpenFlat == USER_ORDER_OPT_OPEN) // order buy
        {
            for(i=0; i<giMaxMyOrder; i++)
            {
                if(gstOrderBuy[i].sig_id == 0)
                {
                    //printf("2 gstCurrSigId %d ++ \n", gstCurrSigId);
                    gstCurrSigId ++;
                    gstOrderBuy[i].sig_id = gstCurrSigId;

                    gstOrderBuy[i].kaiping = SIG_ACTION_OPEN;
                    gstOrderBuy[i].price = gstUsrOrderIn[i1].dPrice;
                    gstOrderBuy[i].volume = gstUsrOrderIn[i1].iVol;

                    liIndex = (int)((gstOrderBuy[i].price - lstCurrInData->dPLmtDn)/lrTick+0.5);
                    gstOrderBuy[i].vol2Pass = laiFullOrderBookBuy[liIndex];
                    gstOrderBuy[i].simu_status = 1;
                    gstOrderBuy[i].status = SIG_STATUS_INIT;
                    gstOrderBuy[i].exec_acc_vol = 0;
                    gstOrderBuy[i].exec_avg_price = 0;
                    gstOrderBuy[i].direct = SIG_DIRECTION_BUY;
                    gstOrderBuy[i].itime = lstCurrInData->iTime;
                    memcpy(gstOrderBuy[i].szContract, gstUsrOrderIn[i1].szContract,sizeof(char)*16);

                    p_stLocTradeInfo->iCurrOrderSize += gstOrderBuy[i].volume;

                    gstUsrOrderIn[i1].iVol = gstUsrOrderIn[i1].iVol - gstOrderBuy[i].volume;
                    break;
                }
            }
        }
        else if(gstUsrOrderIn[i1].iBS == USER_ORDER_BUY && gstUsrOrderIn[i1].iOpenFlat == USER_ORDER_OPT_NONE) // order buy
        {
            for(i=0; i<giMaxMyOrder; i++)
            {
                if(gstOrderBuy[i].sig_id == 0 && iVol_target_long[0] > 0)
                {
                    //printf("3 gstCurrSigId %d ++ \n", gstCurrSigId);
                    gstCurrSigId ++;
                    gstOrderBuy[i].sig_id = gstCurrSigId;

                    gstOrderBuy[i].kaiping = SIG_ACTION_FLAT;
                    gstOrderBuy[i].price = gstUsrOrderIn[i1].dPrice;
                    gstOrderBuy[i].volume = MIN(iVol_target_long[0], gstUsrOrderIn[i1].iVol);
                    iVol_target_long[0] = iVol_target_long[0] - gstOrderBuy[i].volume;

                    liIndex = (int)((gstOrderBuy[i].price - lstCurrInData->dPLmtDn)/lrTick+0.5);
                    gstOrderBuy[i].vol2Pass = laiFullOrderBookBuy[liIndex];
                    gstOrderBuy[i].simu_status = 1;
                    gstOrderBuy[i].status = SIG_STATUS_INIT;
                    gstOrderBuy[i].exec_acc_vol = 0;
                    gstOrderBuy[i].exec_avg_price = 0;
                    gstOrderBuy[i].direct = SIG_DIRECTION_BUY;
                    gstOrderBuy[i].itime = lstCurrInData->iTime;
                    memcpy(gstOrderBuy[i].szContract, gstUsrOrderIn[i1].szContract,sizeof(char)*16);

                    p_stLocTradeInfo->iCurrOrderSize += gstOrderBuy[i].volume;

                    gstUsrOrderIn[i1].iVol = gstUsrOrderIn[i1].iVol - gstOrderBuy[i].volume;
                    if(gstUsrOrderIn[i1].iVol == 0)
                        break;
                    else
                        continue;
                }
                else if(gstOrderBuy[i].sig_id == 0 && iVol_target_long[0] <= 0)
                {
                    //printf("4 gstCurrSigId %d ++ \n", gstCurrSigId);
                    gstCurrSigId ++;
                    gstOrderBuy[i].sig_id = gstCurrSigId;

                    gstOrderBuy[i].kaiping = SIG_ACTION_OPEN;
                    gstOrderBuy[i].price = gstUsrOrderIn[i1].dPrice;
                    gstOrderBuy[i].volume = gstUsrOrderIn[i1].iVol;

                    liIndex = (int)((gstOrderBuy[i].price - lstCurrInData->dPLmtDn)/lrTick+0.5);
                    gstOrderBuy[i].vol2Pass = laiFullOrderBookBuy[liIndex];
                    gstOrderBuy[i].simu_status = 1;
                    gstOrderBuy[i].status = SIG_STATUS_INIT;
                    gstOrderBuy[i].exec_acc_vol = 0;
                    gstOrderBuy[i].exec_avg_price = 0;
                    gstOrderBuy[i].direct = SIG_DIRECTION_BUY;
                    gstOrderBuy[i].itime = lstCurrInData->iTime;
                    memcpy(gstOrderBuy[i].szContract, gstUsrOrderIn[i1].szContract,sizeof(char)*16);

                    p_stLocTradeInfo->iCurrOrderSize += gstOrderBuy[i].volume;

                    gstUsrOrderIn[i1].iVol = gstUsrOrderIn[i1].iVol - gstOrderBuy[i].volume;
                    if(gstUsrOrderIn[i1].iVol == 0) break;
                }

            }
        }
        else if(gstUsrOrderIn[i1].iBS == USER_ORDER_SELL && gstUsrOrderIn[i1].iOpenFlat == USER_ORDER_OPT_FLAT
                && iVol_target_short[0] > 0) // order sell
        {
            for(i=0; i<giMaxMyOrder; i++)
            {
                if(gstOrderSel[i].sig_id == 0)
                {
                    //printf("5 gstCurrSigId %d ++ \n", gstCurrSigId);
                    gstCurrSigId ++;
                    gstOrderSel[i].sig_id = gstCurrSigId;

                    gstOrderSel[i].kaiping = SIG_ACTION_FLAT;
                    gstOrderSel[i].price = gstUsrOrderIn[i1].dPrice;
                    gstOrderSel[i].volume = gstUsrOrderIn[i1].iVol;
                    iVol_target_short[0] = iVol_target_short[0] - gstOrderSel[i].volume;

                    liIndex = (int)((gstOrderSel[i].price - lstCurrInData->dPLmtDn)/lrTick+0.5);
                    gstOrderSel[i].vol2Pass = laiFullOrderBookSel[liIndex];
                    gstOrderSel[i].simu_status = 1;
                    gstOrderSel[i].status = SIG_STATUS_INIT;
                    gstOrderSel[i].exec_acc_vol = 0;
                    gstOrderSel[i].exec_avg_price = 0;
                    gstOrderSel[i].direct = SIG_DIRECTION_SELL;
                    gstOrderSel[i].itime = lstCurrInData->iTime;
                    memcpy(gstOrderSel[i].szContract, gstUsrOrderIn[i1].szContract,sizeof(char)*16);

                    p_stLocTradeInfo->iCurrOrderSize += gstOrderSel[i].volume;

                    gstUsrOrderIn[i1].iVol = gstUsrOrderIn[i1].iVol - gstOrderSel[i].volume;
                    break;
                }
            }
        }
        else if(gstUsrOrderIn[i1].iBS == USER_ORDER_SELL && gstUsrOrderIn[i1].iOpenFlat == USER_ORDER_OPT_OPEN) // order sell
        {
            for(i=0; i<giMaxMyOrder; i++)
            {
                if(gstOrderSel[i].sig_id == 0)
                {
                    //printf("6 gstCurrSigId %d ++ \n", gstCurrSigId);
                    gstCurrSigId ++;
                    gstOrderSel[i].sig_id = gstCurrSigId;

                    gstOrderSel[i].kaiping = SIG_ACTION_OPEN;
                    gstOrderSel[i].price = gstUsrOrderIn[i1].dPrice;
                    gstOrderSel[i].volume = gstUsrOrderIn[i1].iVol;

                    liIndex = (int)((gstOrderSel[i].price - lstCurrInData->dPLmtDn)/lrTick+0.5);
                    gstOrderSel[i].vol2Pass = laiFullOrderBookSel[liIndex];
                    gstOrderSel[i].simu_status = 1;
                    gstOrderSel[i].status = SIG_STATUS_INIT;
                    gstOrderSel[i].exec_acc_vol = 0;
                    gstOrderSel[i].exec_avg_price = 0;
                    gstOrderSel[i].direct = SIG_DIRECTION_SELL;
                    gstOrderSel[i].itime = lstCurrInData->iTime;
                    memcpy(gstOrderSel[i].szContract, gstUsrOrderIn[i1].szContract,sizeof(char)*16);

                    p_stLocTradeInfo->iCurrOrderSize += gstOrderSel[i].volume;

                    gstUsrOrderIn[i1].iVol = gstUsrOrderIn[i1].iVol - gstOrderSel[i].volume;
                    break;
                }
            }
        }
        else if(gstUsrOrderIn[i1].iBS == USER_ORDER_SELL && gstUsrOrderIn[i1].iOpenFlat == USER_ORDER_OPT_NONE) // order sell
        {
            for(i=0; i<giMaxMyOrder; i++)
            {
                if(gstOrderSel[i].sig_id == 0 && iVol_target_short[0] > 0)
                {
                    //printf("7 gstCurrSigId %d ++ \n", gstCurrSigId);
                    gstCurrSigId ++;
                    gstOrderSel[i].sig_id = gstCurrSigId;

                    gstOrderSel[i].kaiping = SIG_ACTION_FLAT;
                    gstOrderSel[i].price = gstUsrOrderIn[i1].dPrice;
                    gstOrderSel[i].volume = MIN(iVol_target_short[0],gstUsrOrderIn[i1].iVol);
                    iVol_target_short[0] = iVol_target_short[0] - gstOrderSel[i].volume;

                    liIndex = (int)((gstOrderSel[i].price - lstCurrInData->dPLmtDn)/lrTick+0.5);
                    gstOrderSel[i].vol2Pass = laiFullOrderBookSel[liIndex];
                    gstOrderSel[i].simu_status = 1;
                    gstOrderSel[i].status = SIG_STATUS_INIT;
                    gstOrderSel[i].exec_acc_vol = 0;
                    gstOrderSel[i].exec_avg_price = 0;
                    gstOrderSel[i].direct = SIG_DIRECTION_SELL;
                    gstOrderSel[i].itime = lstCurrInData->iTime;
                    memcpy(gstOrderSel[i].szContract, gstUsrOrderIn[i1].szContract,sizeof(char)*16);

                    p_stLocTradeInfo->iCurrOrderSize += gstOrderSel[i].volume;

                    gstUsrOrderIn[i1].iVol = gstUsrOrderIn[i1].iVol - gstOrderSel[i].volume;
                    if(gstUsrOrderIn[i1].iVol == 0)
                        break;
                    else
                        continue;
                }
                else if(gstOrderSel[i].sig_id == 0 && iVol_target_short[0] <= 0)
                {
                    //printf("8 gstCurrSigId %d ++ \n", gstCurrSigId);
                    gstCurrSigId ++;
                    gstOrderSel[i].sig_id = gstCurrSigId;

                    gstOrderSel[i].kaiping = SIG_ACTION_OPEN;
                    gstOrderSel[i].price = gstUsrOrderIn[i1].dPrice;
                    gstOrderSel[i].volume = gstUsrOrderIn[i1].iVol;

                    liIndex = (int)((gstOrderSel[i].price - lstCurrInData->dPLmtDn)/lrTick+0.5);
                    gstOrderSel[i].vol2Pass = laiFullOrderBookSel[liIndex];
                    gstOrderSel[i].simu_status = 1;
                    gstOrderSel[i].status = SIG_STATUS_INIT;
                    gstOrderSel[i].exec_acc_vol = 0;
                    gstOrderSel[i].exec_avg_price = 0;
                    gstOrderSel[i].direct = SIG_DIRECTION_SELL;
                    gstOrderSel[i].itime = lstCurrInData->iTime;
                    memcpy(gstOrderSel[i].szContract, gstUsrOrderIn[i1].szContract,sizeof(char)*16);

                    p_stLocTradeInfo->iCurrOrderSize += gstOrderSel[i].volume;

                    gstUsrOrderIn[i1].iVol = gstUsrOrderIn[i1].iVol - gstOrderSel[i].volume;
                    if(gstUsrOrderIn[i1].iVol == 0) break;
                }
            }

        }
        else if(gstUsrOrderIn[i1].iBS == USER_ORDER_CANCEL) //cancel order
        {
            for(i=0; i<giMaxMyOrder; i++)
            {
                if(gstOrderBuy[i].sig_id != 0 && fabs(gstOrderBuy[i].price - gstUsrOrderIn[i1].dPrice)< 0.001
                   && strncmp(gstOrderBuy[i].szContract, gstUsrOrderIn[i1].szContract, 15) == 0)
                {
                    if(gstUsrOrderIn[i1].iOpenFlat == USER_ORDER_OPT_NONE ||
                       gstUsrOrderIn[i1].iOpenFlat == gstOrderBuy[i].kaiping)
                    {
                        s_set_mm_cancel_order(gstOrderBuy[i].sig_id, gstOrderBuy[i].szContract);
                        p_stLocTradeInfo->iCurrCancelSize += gstOrderBuy[i].volume - gstOrderBuy[i].exec_acc_vol;
                    }
                }

                if(gstOrderSel[i].sig_id != 0 && fabs(gstOrderSel[i].price - gstUsrOrderIn[i1].dPrice)< 0.001
                   && strncmp(gstOrderSel[i].szContract, gstUsrOrderIn[i1].szContract, 16) == 0)
                {
                    if(gstUsrOrderIn[i1].iOpenFlat == USER_ORDER_OPT_NONE ||
                       gstUsrOrderIn[i1].iOpenFlat == gstOrderSel[i].kaiping)
                    {
                        s_set_mm_cancel_order(gstOrderSel[i].sig_id, gstOrderSel[i].szContract);
                        p_stLocTradeInfo->iCurrCancelSize  += gstOrderSel[i].volume - gstOrderSel[i].exec_acc_vol;
                    }
                }
            }
        }
    }

//    for(i=0; i<giMaxMyOrder; i++)
//    {
//        if(gstOrderSel[i].sig_id != 0)
//        {
//            printf("my sell order: id=%d, %0.2f, %d\n",gstOrderSel[i].sig_id, gstOrderSel[i].price, gstOrderSel[i].volume);
//        }
//    }
//    for(i=0; i<giMaxMyOrder; i++)
//    {
//        if(gstOrderBuy[i].sig_id != 0)
//        {
//            printf("my buy  order: id=%d, %0.2f, %d\n",gstOrderBuy[i].sig_id, gstOrderBuy[i].price, gstOrderBuy[i].volume);
//        }
//    }

    return 0;
}


int s_update_current_usr_order(st_usr_order *gstUsrOrderOut,int giMaxUsrOrder, int *piUsrOrderOut)
{
    int i,i1,i2;
    st_usr_order stTemp;
    int iUsrOrderOut;

    memset(&(gstUsrOrderOut[0]), 0, sizeof(st_usr_order)*giMaxUsrOrder);

    i1 = 0;
    for(i=0; i<giMaxMyOrder; i++)
    {
        if(i1 == giMaxUsrOrder - 1) break;

        if(gstOrderSel[i].sig_id != 0)
        {
            if(gstOrderSel[i].volume == gstOrderSel[i].exec_acc_vol)
            {
                memset(&(gstOrderSel[i]), 0, sizeof(st_order_info));
                continue;
            }
            else
            {
                gstUsrOrderOut[i1].dPrice = gstOrderSel[i].price;
                gstUsrOrderOut[i1].iBS = USER_ORDER_SELL;
                gstUsrOrderOut[i1].iVol = gstOrderSel[i].volume - gstOrderSel[i].exec_acc_vol;
                gstUsrOrderOut[i1].iOpenFlat = gstOrderSel[i].kaiping;
                gstUsrOrderOut[i1].sig_id = gstOrderSel[i].sig_id;
                gstUsrOrderOut[i1].sig_time = gstOrderSel[i].itime;
                gstUsrOrderOut[i1].vol2pass = gstOrderSel[i].vol2Pass;
                memcpy(gstUsrOrderOut[i1].szContract, gstOrderSel[i].szContract,sizeof(char)*16);

                i1 ++;
            }
        }
    }

    for(i=0; i<giMaxMyOrder; i++)
    {
        if(i1 == giMaxUsrOrder - 1) break;

        if(gstOrderBuy[i].sig_id != 0)
        {
            if(gstOrderBuy[i].volume == gstOrderBuy[i].exec_acc_vol)
            {
                memset(&(gstOrderBuy[i]), 0, sizeof(st_order_info));
                continue;
            }
            else
            {
                gstUsrOrderOut[i1].dPrice = gstOrderBuy[i].price;
                gstUsrOrderOut[i1].iBS = USER_ORDER_BUY;
                gstUsrOrderOut[i1].iVol = gstOrderBuy[i].volume - gstOrderBuy[i].exec_acc_vol;
                gstUsrOrderOut[i1].iOpenFlat = gstOrderBuy[i].kaiping;
                gstUsrOrderOut[i1].sig_id = gstOrderBuy[i].sig_id;
                gstUsrOrderOut[i1].sig_time = gstOrderBuy[i].itime;
                gstUsrOrderOut[i1].vol2pass = gstOrderBuy[i].vol2Pass;
                memcpy(gstUsrOrderOut[i1].szContract, gstOrderBuy[i].szContract,sizeof(char)*16);

                i1 ++;
            }
        }
    }

    iUsrOrderOut = i1;

    for(i=0; i< iUsrOrderOut-1; i++)
        for(i2=i+1; i2< iUsrOrderOut; i2++)
    {
        if((strncmp(gstUsrOrderOut[i].szContract, gstUsrOrderOut[i2].szContract, 16) > 0))
        {
            memcpy(&stTemp, &(gstUsrOrderOut[i]),sizeof(st_usr_order));
            memcpy(&gstUsrOrderOut[i], &(gstUsrOrderOut[i2]),sizeof(st_usr_order));
            memcpy(&(gstUsrOrderOut[i2]),&stTemp,sizeof(st_usr_order));
        }
    }

    for(i=0; i< iUsrOrderOut-1; i++)
        for(i2=i+1; i2< iUsrOrderOut; i2++)
    {
        if((strncmp(gstUsrOrderOut[i].szContract, gstUsrOrderOut[i2].szContract, 16) == 0) &&
            gstUsrOrderOut[i].dPrice < gstUsrOrderOut[i2].dPrice)
        {
            memcpy(&stTemp, &(gstUsrOrderOut[i]),sizeof(st_usr_order));
            memcpy(&gstUsrOrderOut[i], &(gstUsrOrderOut[i2]),sizeof(st_usr_order));
            memcpy(&(gstUsrOrderOut[i2]),&stTemp,sizeof(st_usr_order));
        }
    }

    for(i2=0; i2<giMaxMyOrder*2; i2++)
    {
        if(i1 == giMaxUsrOrder - 1) break;

        if(gstOrderCancel[i2].status != SIG_STATUS_INIT)
        {
            memset(&(gstOrderCancel[i2]), 0, sizeof(st_order_info));
        }

//        if(gstOrderCancel[i2].sig_id != 0 && gstOrderCancel[i2].cancel_sig_id != 0)
//        {
//            for(i=0; i<giMaxMyOrder; i++)
//            {
//                if(gstOrderCancel[i2].cancel_sig_id == gstOrderSel[i].sig_id)
//                {
//                    gstUsrOrderOut[i1].dPrice = gstOrderSel[i].price;
//                    gstUsrOrderOut[i1].iBS = USER_ORDER_CANCEL;
//                    gstUsrOrderOut[i1].iVol = gstOrderSel[i].volume - gstOrderSel[i].exec_acc_vol;
//                    gstUsrOrderOut[i1].iOpenFlat = gstOrderSel[i].kaiping;
//                    gstUsrOrderOut[i1].sig_id = gstOrderSel[i].sig_id;
//                    gstUsrOrderOut[i1].sig_time = gstOrderSel[i].itime;
//
//                    i1 ++;
//                    break;
//                }
//                else if(gstOrderCancel[i2].cancel_sig_id == gstOrderBuy[i].sig_id)
//                {
//                    gstUsrOrderOut[i1].dPrice = gstOrderBuy[i].price;
//                    gstUsrOrderOut[i1].iBS = USER_ORDER_CANCEL;
//                    gstUsrOrderOut[i1].iVol = gstOrderBuy[i].volume - gstOrderBuy[i].exec_acc_vol;
//                    gstUsrOrderOut[i1].iOpenFlat = gstOrderBuy[i].kaiping;
//                    gstUsrOrderOut[i1].sig_id = gstOrderBuy[i].sig_id;
//                    gstUsrOrderOut[i1].sig_time = gstOrderBuy[i].itime;
//
//                    i1 ++;
//                    break;
//                }
//            }
//        }
    }

    *piUsrOrderOut = i1;

    return 0;
}


int s_replace_flat_order(struct in_data *lstCurrInData, int *laiFullOrderBookBuy, int *laiFullOrderBookSel,
         struct all_parameter *lstGlobalPara, st_loc_trade_info *p_stLocTradeInfo)
{
    int i,i1;
    int liIndex;
    double lrTick;

    lrTick = lstGlobalPara->stSec.rTickSize;

    for(i=0; i<giMaxMyOrder; i++)
    {
        if(gstOrderBuy[i].price < lstCurrInData->dBP[2] && gstOrderBuy[i].kaiping == SIG_ACTION_FLAT)
        {
            s_set_mm_cancel_order(gstOrderBuy[i].sig_id, gstOrderBuy[i].szContract);
            p_stLocTradeInfo->iCurrCancelSize += gstOrderBuy[i].volume - gstOrderBuy[i].exec_acc_vol;

            for(i1=0; i1<giMaxMyOrder; i1++)
            {
                if(gstOrderBuy[i1].sig_id == 0)
                {
                    gstCurrSigId ++;
                    gstOrderBuy[i1].sig_id = gstCurrSigId;

                    gstOrderBuy[i1].kaiping = SIG_ACTION_OPEN;
                    gstOrderBuy[i1].price = gstOrderBuy[i].price;
                    gstOrderBuy[i1].volume = gstOrderBuy[i].volume - gstOrderBuy[i].exec_acc_vol;

                    liIndex = (int)((gstOrderBuy[i1].price - lstCurrInData->dPLmtDn)/lrTick+0.5);
                    gstOrderBuy[i1].vol2Pass = laiFullOrderBookBuy[liIndex];
                    gstOrderBuy[i].simu_status = 1;
                    gstOrderBuy[i1].status = SIG_STATUS_INIT;
                    gstOrderBuy[i1].exec_acc_vol = 0;
                    gstOrderBuy[i1].exec_avg_price = 0;
                    gstOrderBuy[i1].direct = SIG_DIRECTION_BUY;
                    gstOrderBuy[i1].itime = lstCurrInData->iTime;
                    memcpy(gstOrderBuy[i1].szContract, gstOrderBuy[i].szContract,sizeof(char)*16);

                    p_stLocTradeInfo->iCurrOrderSize += gstOrderBuy[i1].volume;

                    break;
                }
            }

            break;
        }
    }

    for(i=0; i<giMaxMyOrder; i++)
    {
        if(gstOrderSel[i].price > lstCurrInData->dSP[2] && gstOrderSel[i].kaiping == SIG_ACTION_FLAT)
        {
            s_set_mm_cancel_order(gstOrderSel[i].sig_id, gstOrderSel[i].szContract);
            p_stLocTradeInfo->iCurrCancelSize += gstOrderSel[i].volume - gstOrderSel[i].exec_acc_vol;

            for(i1=0; i1<giMaxMyOrder; i1++)
            {
                if(gstOrderSel[i1].sig_id == 0)
                {
                    gstCurrSigId ++;
                    gstOrderSel[i1].sig_id = gstCurrSigId;

                    gstOrderSel[i1].kaiping = SIG_ACTION_OPEN;
                    gstOrderSel[i1].price = gstOrderSel[i].price;
                    gstOrderSel[i1].volume = gstOrderSel[i].volume - gstOrderSel[i].exec_acc_vol;

                    liIndex = (int)((gstOrderSel[i1].price - lstCurrInData->dPLmtDn)/lrTick+0.5);
                    gstOrderSel[i1].vol2Pass = laiFullOrderBookSel[liIndex];
                    gstOrderSel[i].simu_status = 1;
                    gstOrderSel[i1].status = SIG_STATUS_INIT;
                    gstOrderSel[i1].exec_acc_vol = 0;
                    gstOrderSel[i1].exec_avg_price = 0;
                    gstOrderSel[i1].direct = SIG_DIRECTION_SELL;
                    gstOrderSel[i1].itime = lstCurrInData->iTime;
                    memcpy(gstOrderSel[i1].szContract, gstOrderSel[i].szContract,sizeof(char)*16);


                    p_stLocTradeInfo->iCurrOrderSize += gstOrderSel[i1].volume;
                    break;
                }
            }

            break;
        }
    }


    return 0;
}

int s_set_mm_cancel_order(const int sig_id, char* szContract)
{
    int i;


    for(i=0; i<giMaxMyOrder*2; i++)
    {
        if(gstOrderCancel[i].sig_id == 0)
        {
            //printf("9 gstCurrSigId %d ++ \n", gstCurrSigId);
            gstCurrSigId ++;

            gstOrderCancel[i].cancel_sig_id = sig_id;
            gstOrderCancel[i].sig_id = gstCurrSigId;
            gstOrderCancel[i].direct = SIG_DIRECTION_CANCEL;
            gstOrderCancel[i].status = SIG_STATUS_INIT;
            memcpy(gstOrderCancel[i].szContract, szContract, sizeof(char)*16);
            break;
        }
    }
    return 0;
}

int s_mm_assign_out_tsignal(struct signal_t *pstTSignal, int *licNoTSignal, st_loc_trade_info *p_stLocTradeInfo)
{

    int i;
    int numSignal;
    int liNumOrderlist;

    *licNoTSignal = 0;

    liNumOrderlist = giMaxMyOrder;

    numSignal = 0;

    for(i=0; i<liNumOrderlist*2; i++)
    {
        if(gstOrderCancel[i].status == SIG_STATUS_INIT && gstOrderCancel[i].direct == SIG_DIRECTION_CANCEL)
        {
            gstOrderCancel[i].status = SIG_STATUS_ENTRUSTED;

            pstTSignal[numSignal].exch = (int)gcExch;
            pstTSignal[numSignal].buy_price = gstOrderCancel[i].price;
            //pstTSignal[numSignal].cancel_mode = 0;
            pstTSignal[numSignal].sig_id = gstOrderCancel[i].sig_id;
            pstTSignal[numSignal].cancel_sig_id = gstOrderCancel[i].cancel_sig_id;
            pstTSignal[numSignal].sell_price = gstOrderCancel[i].price;
            pstTSignal[numSignal].instruction = 0;
            //pstTSignal[numSignal].internal_match = 0;
            //strncpy(pstTSignal[numSignal].mm_order_id,"\0",20);
            pstTSignal[numSignal].st_id = giStID;
            strncpy(pstTSignal[numSignal].ticker, gstOrderCancel[i].szContract, 16);
            pstTSignal[numSignal].sig_action = SIG_ACT_CANCEL;
            pstTSignal[numSignal].sig_openclose = 0;
            pstTSignal[numSignal].open_volume = 0;
            pstTSignal[numSignal].close_volume = 0;
            numSignal ++;

            //p_stLocTradeInfo->iTotalCancelCount ++;
            //fprintf(pfDayLogFile,"%d Send cancel order: %d %d\n", gstCurrIn.iTime, gstOrderCancel[i].sig_id, gstOrderCancel[i].cancel_sig_id);
        }
    }

    for(i=0; i<liNumOrderlist; i++)
    {
        if(gstOrderBuy[i].status == SIG_STATUS_INIT && gstOrderBuy[i].direct != SIG_DIRECTION_SELL)
        {
            gstOrderBuy[i].status = SIG_STATUS_ENTRUSTED;

            pstTSignal[numSignal].exch = (int)gcExch;
            pstTSignal[numSignal].buy_price = gstOrderBuy[i].price;
            //pstTSignal[numSignal].cancel_mode = 0;
            pstTSignal[numSignal].sig_id = gstOrderBuy[i].sig_id;
            pstTSignal[numSignal].cancel_sig_id = gstOrderBuy[i].cancel_sig_id;
            pstTSignal[numSignal].sell_price = gstOrderBuy[i].price;
            pstTSignal[numSignal].instruction = 0;
            //pstTSignal[numSignal].internal_match = 0;
            //strncpy(pstTSignal[numSignal].mm_order_id,"\0",20);
            pstTSignal[numSignal].st_id = giStID;
            strncpy(pstTSignal[numSignal].ticker, gstOrderBuy[i].szContract, 16);

            if(gstOrderBuy[i].direct == SIG_DIRECTION_CANCEL)
            {
                pstTSignal[numSignal].sig_action = SIG_ACT_CANCEL;
                pstTSignal[numSignal].sig_openclose = 0;
                pstTSignal[numSignal].open_volume = 0;
                pstTSignal[numSignal].close_volume = 0;
            }
            else
            {
                pstTSignal[numSignal].sig_action = SIG_ACT_BUY;
                if(gstOrderBuy[i].kaiping == SIG_ACTION_FLAT)
                {
                    pstTSignal[numSignal].sig_openclose = 2;
                    pstTSignal[numSignal].open_volume = 0;
                    pstTSignal[numSignal].close_volume = gstOrderBuy[i].volume;
                }
                else if(gstOrderBuy[i].kaiping == SIG_ACTION_OPEN)
                {
                    pstTSignal[numSignal].sig_openclose = 1;
                    pstTSignal[numSignal].open_volume = gstOrderBuy[i].volume;
                    pstTSignal[numSignal].close_volume = 0;
                }
            }
            numSignal ++;

            p_stLocTradeInfo->iTotalOrderCount ++;
            //fprintf(pfDayLogFile,"%d Send buy order: %d %0.2f %d\n", gstCurrIn.iTime, gstOrderBuy[i].sig_id, gstOrderBuy[i].price,gstOrderBuy[i].kaiping);
        }

        if(gstOrderSel[i].status == SIG_STATUS_INIT && gstOrderSel[i].direct != SIG_DIRECTION_BUY)
        {
            gstOrderSel[i].status = SIG_STATUS_ENTRUSTED;

            pstTSignal[numSignal].exch = (int)gcExch;
            pstTSignal[numSignal].buy_price = gstOrderSel[i].price;
            //pstTSignal[numSignal].cancel_mode = 0;
            pstTSignal[numSignal].sig_id = gstOrderSel[i].sig_id;
            pstTSignal[numSignal].cancel_sig_id = gstOrderSel[i].cancel_sig_id;
            pstTSignal[numSignal].sell_price = gstOrderSel[i].price;
            pstTSignal[numSignal].instruction = 0;
            //pstTSignal[numSignal].internal_match = 0;
            //strncpy(pstTSignal[numSignal].mm_order_id,"\0",20);
            pstTSignal[numSignal].st_id = giStID;
            strncpy(pstTSignal[numSignal].ticker, gstOrderSel[i].szContract, 16);

            if(gstOrderSel[i].direct == SIG_DIRECTION_CANCEL)
            {
                pstTSignal[numSignal].sig_action = SIG_ACT_CANCEL;
                pstTSignal[numSignal].sig_openclose = 0;
                pstTSignal[numSignal].open_volume = 0;
                pstTSignal[numSignal].close_volume = 0;
            }
            else
            {
                pstTSignal[numSignal].sig_action = SIG_ACT_SELL;
                if(gstOrderSel[i].kaiping == SIG_ACTION_FLAT)
                {
                    pstTSignal[numSignal].sig_openclose = 2;
                    pstTSignal[numSignal].open_volume = 0;
                    pstTSignal[numSignal].close_volume = gstOrderSel[i].volume;
                }
                else if(gstOrderSel[i].kaiping == SIG_ACTION_OPEN)
                {
                    pstTSignal[numSignal].sig_openclose = 1;
                    pstTSignal[numSignal].open_volume = gstOrderSel[i].volume;
                    pstTSignal[numSignal].close_volume = 0;
                }
            }
            numSignal ++;

            p_stLocTradeInfo->iTotalOrderCount ++;
            //fprintf(pfDayLogFile,"%d Send buy order: %d %0.2f %d\n", gstCurrIn.iTime, gstOrderSel[i].sig_id, gstOrderSel[i].price,gstOrderSel[i].kaiping);
        }
    }

    *licNoTSignal = numSignal;

    return 0;
}

int s_update_my_order_info(struct signal_resp_t *lstStructOrderStatusIn)
{
	int i;
	int liNumOrderlist;
	int iInfo;

	liNumOrderlist = giMaxMyOrder;

    iInfo = -1;
	for(i=0; i<giNumContract; i++)
    {
        if(strncmp(gstLocTradeInfo[i].szContract, lstStructOrderStatusIn->ticker, 16) == 0)
        {
            iInfo = i;
            break;
        }
    }
    if(iInfo < 0) return 1;


	if(lstStructOrderStatusIn->sig_act == 1) //buy
	{
	    if(lstStructOrderStatusIn->status == SIG_STATUS_CANCEL)
        {
            gstLocTradeInfo[iInfo].iTotalCancelCount ++;
            for(i=0; i< liNumOrderlist*2; i++)
            {
                if(gstOrderCancel[i].cancel_sig_id == lstStructOrderStatusIn->sig_id)
                {
                    memset(&(gstOrderCancel[i]), 0, sizeof(st_order_info));
                    break;
                }
            }

            for(i=0; i< liNumOrderlist; i++)
            {
                if(gstOrderBuy[i].sig_id == lstStructOrderStatusIn->sig_id)
                {
                    memset(&(gstOrderBuy[i]), 0, sizeof(st_order_info));
                    break;
                }
            }
        }
        else if(lstStructOrderStatusIn->status == SIG_STATUS_REJECTED)
        {
            for(i=0; i< liNumOrderlist; i++)
            {
                if(gstOrderBuy[i].sig_id == lstStructOrderStatusIn->sig_id)
                {
                    //fprintf(pfDayLogFile,"%d clear gstOrderBuy 3, sig_id %d\n", gstCurrIn.iTime, gstOrderBuy[i].sig_id);
                    gstCurrSigId ++;
                    memset(&(gstOrderBuy[i]), 0, sizeof(st_order_info));
                    break;
                }
            }
        }
        else if(lstStructOrderStatusIn->status == SIG_STATUS_SUCCESS ||
                lstStructOrderStatusIn->status == SIG_STATUS_PARTED)
        {
            for(i=0; i< liNumOrderlist; i++)
            {
                if(gstOrderBuy[i].sig_id == lstStructOrderStatusIn->sig_id)
                {
                    gstOrderBuy[i].exec_avg_price = lstStructOrderStatusIn->avg_price;
                    gstOrderBuy[i].exec_acc_vol = lstStructOrderStatusIn->cum_volume;
                    gstOrderBuy[i].exec_price = lstStructOrderStatusIn->exe_price;
                    gstOrderBuy[i].exec_vol = lstStructOrderStatusIn->exe_volume;
                    gstOrderBuy[i].status = lstStructOrderStatusIn->status;

                    if(gstOrderBuy[i].exec_vol != 0)
                    {
                        gstLastRec.iBS = 1;
                        gstLastRec.rPrice = gstOrderBuy[i].exec_price;
                        gstLastRec.iOpenFlat = gstOrderBuy[i].kaiping;
                        gstLastRec.iVol = gstOrderBuy[i].exec_vol;
                        update_lastThreeRec(gstLastRec.iBS, gstLastRec.rPrice);
                        int istatus = lstStructOrderStatusIn->status;

                        s_mm_calc_profitloss_and_update(&(gstOrderBuy[i]), &gstGlobalPara[iInfo].stSec, &gstLocTradeInfo[iInfo],istatus);
                        gstLocTradeInfo[iInfo].iLastTradeTime = gstCurrIn.iTime;
                    }
                    break;
                }
            }
        }
        else if(lstStructOrderStatusIn->status == SIG_STATUS_ENTRUSTED)
        {
            for(i=0; i< liNumOrderlist; i++)
            {
                if(gstOrderBuy[i].sig_id == lstStructOrderStatusIn->sig_id)
                {
                    gstOrderBuy[i].status = lstStructOrderStatusIn->status;
                    break;
                }
            }
        }
	}
	else if(lstStructOrderStatusIn->sig_act == 2) //sell order
    {
        if(lstStructOrderStatusIn->status == SIG_STATUS_CANCEL)
        {
            gstLocTradeInfo[iInfo].iTotalCancelCount ++;
            for(i=0; i< liNumOrderlist*2; i++)
            {
                if(gstOrderCancel[i].cancel_sig_id == lstStructOrderStatusIn->sig_id)
                {
                    memset(&(gstOrderCancel[i]), 0, sizeof(st_order_info));
                    break;
                }
            }

            for(i=0; i< liNumOrderlist; i++)
            {
                if(gstOrderSel[i].sig_id == lstStructOrderStatusIn->sig_id)
                {
                    //fprintf(pfDayLogFile,"%d clear gstOrderSel 2, sig_id %d\n", gstCurrIn.iTime, gstOrderSel[i].sig_id);
                    memset(&(gstOrderSel[i]), 0, sizeof(st_order_info));
                    break;
                }
            }
        }
        else if(lstStructOrderStatusIn->status == SIG_STATUS_REJECTED)
        {
            for(i=0; i< liNumOrderlist; i++)
            {
                if(gstOrderSel[i].sig_id == lstStructOrderStatusIn->sig_id)
                {
                    //fprintf(pfDayLogFile,"%d clear gstOrderSel 3, sig_id %d\n", gstCurrIn.iTime, gstOrderSel[i].sig_id);
                    gstCurrSigId ++;
                    memset(&(gstOrderSel[i]), 0, sizeof(st_order_info));
                    break;
                }
            }
        }
        else if(lstStructOrderStatusIn->status == SIG_STATUS_SUCCESS ||
                lstStructOrderStatusIn->status == SIG_STATUS_PARTED)
        {
            for(i=0; i< liNumOrderlist; i++)
            {
                if(gstOrderSel[i].sig_id == lstStructOrderStatusIn->sig_id)
                {
                    gstOrderSel[i].exec_avg_price = lstStructOrderStatusIn->avg_price;
                    gstOrderSel[i].exec_acc_vol = lstStructOrderStatusIn->cum_volume;
                    gstOrderSel[i].exec_price = lstStructOrderStatusIn->exe_price;
                    gstOrderSel[i].exec_vol = lstStructOrderStatusIn->exe_volume;
                    gstOrderSel[i].status = lstStructOrderStatusIn->status;

                    if(gstOrderSel[i].exec_vol != 0)
                    {
                        gstLastRec.iBS = -1;
                        gstLastRec.rPrice = gstOrderSel[i].exec_price;
                        gstLastRec.iOpenFlat = gstOrderSel[i].kaiping;
                        gstLastRec.iVol = gstOrderSel[i].exec_vol;
                        update_lastThreeRec(gstLastRec.iBS, gstLastRec.rPrice);
                        int istatus = lstStructOrderStatusIn->status;

                        s_mm_calc_profitloss_and_update(&(gstOrderSel[i]), &gstGlobalPara[iInfo].stSec, &gstLocTradeInfo[iInfo], istatus);
                        gstLocTradeInfo[iInfo].iLastTradeTime = gstCurrIn.iTime;
                    }
                    break;
                }
            }
        }
        else if(lstStructOrderStatusIn->status == SIG_STATUS_ENTRUSTED)
        {
            for(i=0; i< liNumOrderlist; i++)
            {
                if(gstOrderSel[i].sig_id == lstStructOrderStatusIn->sig_id)
                {
                    gstOrderSel[i].status = lstStructOrderStatusIn->status;
                    break;
                }
            }
        }
    }
    else if(lstStructOrderStatusIn->sig_act == 4) //cancel order
    {
        if(lstStructOrderStatusIn->status == SIG_STATUS_REJECTED)
        {
            for(i=0; i< liNumOrderlist*2; i++)
            {
                if(gstOrderCancel[i].sig_id == lstStructOrderStatusIn->sig_act)
                {
                    //gstOrderCancel[i].status = SIG_STATUS_INIT;
                    memset(&(gstOrderCancel[i]), 0, sizeof(st_order_info));
                    break;
                }
            }
        }
        else if(lstStructOrderStatusIn->status == SIG_STATUS_ENTRUSTED)
        {
            for(i=0; i< liNumOrderlist*2; i++)
            {
                if(gstOrderCancel[i].sig_id == lstStructOrderStatusIn->sig_id)
                {
                    gstOrderCancel[i].status = lstStructOrderStatusIn->status;
                    break;
                }
            }
        }
    }
	return 0;
}

int s_mm_calc_profitloss_and_update(st_order_info *p_stOrder, struct sec_parameter *p_stSecPara,
    st_loc_trade_info *p_stLocTradeInfo, int istatus)
{
	int liTradedSize;
	double lrCost, lrPrice;

    if(p_stOrder->exec_vol == 0) return 0;

	liTradedSize = p_stOrder->exec_vol;
	lrPrice = p_stOrder->exec_price;

	lrCost = p_stSecPara->rRssTCostByVol *liTradedSize + p_stSecPara->rRssTCostByVal* liTradedSize * lrPrice;

	if(p_stOrder->direct == SIG_DIRECTION_BUY)
    {
        p_stLocTradeInfo->rCash = p_stLocTradeInfo->rCash - lrPrice * liTradedSize - lrCost;
        p_stLocTradeInfo->rTotalCost = p_stLocTradeInfo->rTotalCost + lrCost;
    }
    else if(p_stOrder->direct == SIG_DIRECTION_SELL)
    {
        p_stLocTradeInfo->rCash = p_stLocTradeInfo->rCash + lrPrice * liTradedSize - lrCost;
        p_stLocTradeInfo->rTotalCost = p_stLocTradeInfo->rTotalCost + lrCost;
    }

    p_stLocTradeInfo->iTotalVol += p_stOrder->exec_vol;

    //update order info
    p_stOrder->exec_vol = 0;
    p_stOrder->exec_price = 0;
    if(p_stOrder->exec_acc_vol == p_stOrder->volume || istatus == 0)
    {
        //fprintf(pfDayLogFile,"%d clear order %d %d\n", gstCurrIn.iTime, p_stOrder->sig_id, p_stOrder->direct);
        memset(p_stOrder, 0, sizeof(st_order_info));
    }


	return 0;
}

/******************************************
*********** From Simulation Use ***********
******************************************/
int s_simu_cancel_order()
{
    int i, i1;
    int id_canceled;
    int liNumOrderlist;

    liNumOrderlist = giMaxMyOrder;

    for(i1=0; i1< liNumOrderlist*2; i1++)
    {
        if(gstOrderCancel[i1].sig_id !=0)
        {
            id_canceled = gstOrderCancel[i1].cancel_sig_id;

            for(i=0; i<giNumContract; i++)
            {
                if(strncmp(gstLocTradeInfo[i].szContract, gstOrderCancel[i1].szContract, 16) == 0)
                {
                    gstLocTradeInfo[i].iTotalCancelCount ++;
                    break;
                }
            }

            memset(&(gstOrderCancel[i1]), 0, sizeof(st_order_info));

            for(i=0; i< liNumOrderlist; i++)
            {
                if(gstOrderBuy[i].sig_id == id_canceled)
                {
                    memset(&(gstOrderBuy[i]), 0, sizeof(st_order_info));
                    break;
                }
                if(gstOrderSel[i].sig_id == id_canceled)
                {
                    memset(&(gstOrderSel[i]), 0, sizeof(st_order_info));
                    break;
                }
            }
        }
    }

    return 0;
}

int s_simu_calc_all_profitloss(struct sec_parameter *p_stSecPara, st_loc_trade_info *p_stLocTradeInfo)
{
    int i;
    for(i=0; i< giMaxMyOrder; i++)
    {
        if(gstOrderBuy[i].exec_vol > 0)
        {
            s_mm_calc_profitloss_and_update(&(gstOrderBuy[i]), p_stSecPara, p_stLocTradeInfo, 2);
        }
    }

    for(i=0; i< giMaxMyOrder; i++)
    {
        if(gstOrderSel[i].exec_vol > 0)
        {
            s_mm_calc_profitloss_and_update(&(gstOrderSel[i]), p_stSecPara, p_stLocTradeInfo, 2);
        }
    }

    return 0;
}


//----------------------------------------------------
//subroutine s_trade_simulation()
// description: simulate trade completion
//----------------------------------------------------
int s_trade_simulation(struct in_data *p_stDataIn, struct in_data *p_stPreDataIn,
	struct all_parameter *p_stGlobalPara, int liTickNo,
	int *laiFullOrderBookBuy, int *laiFullOrderBookSel, int liLenFullOrderBook,
	st_loc_trade_info *p_stLocTradeInfo)
{
	double lrTradeP, lrPriceL, lrPriceH, lrTick, lrExeRatio;
	int  liVLo, liVHi, liVol2, liPosNew, liIndex, liLevel, liFrom, liTo, liI, i, liPreVol, i1;
	double larBP[30], larSP[30];
	int laiBV[30], laiSV[30];

//	if(p_stDataIn->iTime >= 94958000)
//    {
//        printf("%d\n",p_stDataIn->iTime);
//    }

	memcpy(larBP, p_stDataIn->dBP, 30*sizeof(double));
	memcpy(larSP, p_stDataIn->dSP, 30*sizeof(double));
	memcpy(laiBV, p_stDataIn->iBV, 30*sizeof(int));
	memcpy(laiSV, p_stDataIn->iSV, 30*sizeof(int));

	liLevel = 5;
	lrExeRatio = p_stGlobalPara->stSec.rTVCompRatio;
	lrTick = p_stGlobalPara->stSec.rTickSize ;
	if (larBP[liLevel-1] < lrTick)
		liFrom = 0;
	else
		liFrom = MAX((int)((larBP[liLevel-1] - p_stDataIn->dPLmtDn)/lrTick+0.5) , 0);

	if (larSP[liLevel-1] < lrTick)
		liTo = liLenFullOrderBook - 1;
	else
		liTo = (int)((larSP[liLevel-1]-p_stDataIn->dPLmtDn)/lrTick+0.5);

	for(i=liFrom; i<=liTo; i++)
	{
		laiFullOrderBookBuy[i] = 0;
		laiFullOrderBookSel[i] = 0;
	}


	for(liI = 0; liI<liLevel; liI++)   // build order book
	{
		if (larBP[liI] > lrTick/2)
		{
			liIndex = (int)((larBP[liI]-p_stDataIn->dPLmtDn)/lrTick+0.5);
			laiFullOrderBookBuy[liIndex] = laiBV[liI];
		}
		if (larSP[liI] > lrTick/2)
		{
			liIndex = (int)((larSP[liI]-p_stDataIn->dPLmtDn)/lrTick+0.5);
			laiFullOrderBookSel[liIndex] = laiSV[liI];
		}
	}


    p_stDataIn->iVol = (int)(p_stDataIn->dTotV - p_stPreDataIn->dTotV)/p_stGlobalPara->stExch.iVolDivider;
	p_stDataIn->dOIChg = (p_stDataIn->dOI - p_stPreDataIn->dOI)/p_stGlobalPara->stExch.iVolDivider;
	if (gcExch == cCZCE)
	{
		p_stDataIn->dVal  = 0;
		p_stDataIn->dAvgP = p_stDataIn->dP;
	}
	else
	{
		p_stDataIn->dVal = (p_stDataIn->dTotVal - p_stPreDataIn->dTotVal)/p_stGlobalPara->stExch.iVolDivider;
		p_stDataIn->dAvgP = f_calc_curr_avgp(p_stDataIn->dVal, p_stDataIn->iVol, p_stPreDataIn->dAvgP, p_stGlobalPara)/
			p_stGlobalPara->stSec.iUnitPerContract;
	}
	lrTradeP = p_stDataIn->dAvgP;

    if(lrTradeP >= p_stPreDataIn->dSP[0])
    {
        lrPriceL = p_stPreDataIn->dSP[0] - lrTick;
        lrPriceH = p_stPreDataIn->dSP[0];
        liVLo = 0;
        liVHi = p_stDataIn->iVol;
    }
    else if(lrTradeP <= p_stPreDataIn->dBP[0])
    {
        lrPriceL = p_stPreDataIn->dBP[0];
        lrPriceH = p_stPreDataIn->dBP[0]+lrTick;
        liVLo = p_stDataIn->iVol;
        liVHi = 0;
    }
    else
    {
        lrPriceL = (int)(lrTradeP/lrTick)*lrTick ;   // immediate tick price below tradep
        lrPriceH = lrPriceL + lrTick ;             // immediate tick price over tradep
        liVLo = (int)(p_stDataIn->iVol * (lrPriceH - lrTradeP) / lrTick + 0.5);   // last trade vol @ priceL
        liVHi = p_stDataIn->iVol - liVLo;   // last trade vol @ priceH
    }

    for(i=0; i< giMaxMyOrder; i++)
    {
        if(gstOrderBuy[i].sig_id == 0) continue;
        if(strncmp(gstOrderBuy[i].szContract, p_stDataIn->cTicker,8) != 0) continue;

        if(gstOrderBuy[i].status == SIG_STATUS_INIT) gstOrderBuy[i].status = SIG_STATUS_ENTRUSTED;

        if(gstOrderBuy[i].direct != SIG_DIRECTION_BUY) continue;

        if(gcExch == 'A' && gstOrderBuy[i].itime == p_stDataIn->iTime) continue;

        liPreVol = gstOrderBuy[i].exec_acc_vol;

		if(gstOrderBuy[i].price > lrPriceH - lrTick/2)
		{
		    if(gstOrderBuy[i].simu_status == 1 && gstOrderBuy[i].price <= p_stDataIn->dBP[0]+0.1*lrTick)
            {
                for(i1=0; i1<5; i1++)
                {
                    if(fabs(gstOrderBuy[i].price - p_stDataIn->dBP[i1]) < 0.1*lrTick)
                    {
                        gstOrderBuy[i].vol2Pass = (liVHi + p_stDataIn->iBV[i1])*lrExeRatio;
                        break;
                    }
                }
            }
			if(p_stDataIn->iVol < gstOrderBuy[i].vol2Pass)
			{
				gstOrderBuy[i].vol2Pass = gstOrderBuy[i].vol2Pass - p_stDataIn->iVol;
			}
			else
			{
				if(gstOrderBuy[i].exec_acc_vol==0)
                    liVol2 = (int)((p_stDataIn->iVol - gstOrderBuy[i].vol2Pass));
                else
                    liVol2 = (p_stDataIn->iVol - gstOrderBuy[i].vol2Pass);
				gstOrderBuy[i].vol2Pass = 0;
				gstOrderBuy[i].exec_acc_vol = MIN(liVol2 + gstOrderBuy[i].exec_acc_vol, gstOrderBuy[i].volume);
				gstOrderBuy[i].exec_price = lrPriceH;
			}
		}
		else if (gstOrderBuy[i].price > lrPriceL - lrTick/2)
		{
			if(liVLo < gstOrderBuy[i].vol2Pass)
			{
				gstOrderBuy[i].vol2Pass = gstOrderBuy[i].vol2Pass - liVLo;
			}
			else
			{
				liVol2 = (liVLo - gstOrderBuy[i].vol2Pass);
				gstOrderBuy[i].vol2Pass = 0;
				gstOrderBuy[i].exec_acc_vol = MIN(liVol2 + gstOrderBuy[i].exec_acc_vol, gstOrderBuy[i].volume);
				gstOrderBuy[i].exec_price = lrPriceL;
			}
		}
		liIndex = (int)((gstOrderBuy[i].price - p_stDataIn->dPLmtDn)/lrTick + 0.5);
		gstOrderBuy[i].vol2Pass = MIN(gstOrderBuy[i].vol2Pass, laiFullOrderBookBuy[liIndex]); // market order cancelations considered
		if (gstOrderBuy[i].price > (larSP[4] - lrTick/2) && larSP[4] > lrTick
			&& laiSV[0]+laiSV[1]+laiSV[2]+laiSV[3]+laiSV[4] >= gstOrderBuy[i].vol2Pass +
			abs(gstOrderBuy[i].volume - gstOrderBuy[i].exec_acc_vol))
		{
			liPosNew = MIN(gstOrderBuy[i].volume, gstOrderBuy[i].exec_acc_vol + laiSV[0]+laiSV[1]+laiSV[2]+laiSV[3]+laiSV[4]);
			gstOrderBuy[i].exec_acc_vol = liPosNew;
		}
		else if (gstOrderBuy[i].price > (larSP[3] - lrTick/2) && larSP[3] > lrTick
			&& laiSV[0]+laiSV[1]+laiSV[2]+laiSV[3] >= gstOrderBuy[i].vol2Pass +
			abs(gstOrderBuy[i].volume - gstOrderBuy[i].exec_acc_vol))
		{
			liPosNew = MIN(gstOrderBuy[i].volume, gstOrderBuy[i].exec_acc_vol + laiSV[0]+laiSV[1]+laiSV[2]+laiSV[3]);
			gstOrderBuy[i].exec_acc_vol = liPosNew;
		}
		else if (gstOrderBuy[i].price > (larSP[2] - lrTick/2) && larSP[2] > lrTick
			&& laiSV[0]+laiSV[1]+laiSV[2] >= gstOrderBuy[i].vol2Pass +
			abs(gstOrderBuy[i].volume - gstOrderBuy[i].exec_acc_vol))
		{
			liPosNew = MIN(gstOrderBuy[i].volume, gstOrderBuy[i].exec_acc_vol + laiSV[0]+laiSV[1]+laiSV[2]);
			gstOrderBuy[i].exec_acc_vol = liPosNew;
		}
		else if (gstOrderBuy[i].price > (larSP[1] - lrTick/2) && larSP[1] > lrTick
			&& laiSV[0]+laiSV[1] >= gstOrderBuy[i].vol2Pass +
			abs(gstOrderBuy[i].volume - gstOrderBuy[i].exec_acc_vol))
		{
				liPosNew = MIN(gstOrderBuy[i].volume, gstOrderBuy[i].exec_acc_vol + laiSV[0]+laiSV[1]);
				gstOrderBuy[i].exec_acc_vol = liPosNew;
		}
		else if (gstOrderBuy[i].price > (larSP[0] - lrTick/2) && larSP[0] > lrTick
			&& laiSV[0] >= gstOrderBuy[i].vol2Pass + abs(gstOrderBuy[i].volume - gstOrderBuy[i].exec_acc_vol))
		{
				liPosNew = MIN(gstOrderBuy[i].volume, gstOrderBuy[i].exec_acc_vol + laiSV[0]);
				gstOrderBuy[i].exec_acc_vol = liPosNew;
		}

		gstOrderBuy[i].exec_vol = gstOrderBuy[i].exec_acc_vol - liPreVol;
		gstOrderBuy[i].exec_price = gstOrderBuy[i].price;

		if(gstOrderBuy[i].simu_status == 1) gstOrderBuy[i].simu_status = 0;

		if(gstOrderBuy[i].exec_vol != 0 )
        {
            if(gstOrderBuy[i].kaiping == SIG_ACTION_OPEN)
                p_stLocTradeInfo->iPosD += gstOrderBuy[i].exec_vol;
            else if(gstOrderBuy[i].kaiping == SIG_ACTION_FLAT)
                p_stLocTradeInfo->iPosK -= gstOrderBuy[i].exec_vol;

            p_stLocTradeInfo->iLastTradeTime = gstCurrIn.iTime;

            gstLastRec.iBS = 1;
            gstLastRec.rPrice = gstOrderBuy[i].exec_price;
            gstLastRec.iOpenFlat = gstOrderBuy[i].kaiping;
            gstLastRec.iVol = gstOrderBuy[i].exec_vol;
            update_lastThreeRec(gstLastRec.iBS, gstLastRec.rPrice);
        }

//		if(gstOrderBuy[i].exec_vol != 0)
//            printf("exec: %8.1f %d\n",gstOrderBuy[i].exec_price, gstOrderBuy[i].exec_vol);
	}

	for(i=0; i< giMaxMyOrder; i++)
	{
	    if(gstOrderSel[i].sig_id == 0) continue;
	    if(strncmp(gstOrderSel[i].szContract, p_stDataIn->cTicker,8) != 0) continue;

	    if(gstOrderSel[i].status == SIG_STATUS_INIT) gstOrderSel[i].status = SIG_STATUS_ENTRUSTED;
	    if(gstOrderSel[i].direct != SIG_DIRECTION_SELL) continue;
	    if(gcExch == 'A' && gstOrderSel[i].itime == p_stDataIn->iTime) continue;

	    liPreVol = gstOrderSel[i].exec_acc_vol;

		if(gstOrderSel[i].price < lrPriceL + lrTick/2)
		{
		    if(gstOrderSel[i].simu_status == 1 && gstOrderSel[i].price >= p_stDataIn->dSP[0]-0.1*lrTick)
            {
                for(i1=0; i1<5; i1++)
                {
                    if(fabs(gstOrderSel[i].price - p_stDataIn->dSP[i1]) < 0.1*lrTick)
                    {
                        gstOrderSel[i].vol2Pass = (liVLo + p_stDataIn->iSV[i1])*lrExeRatio;
                        break;
                    }
                }
            }
			if(p_stDataIn->iVol < gstOrderSel[i].vol2Pass)
			{
				gstOrderSel[i].vol2Pass = gstOrderSel[i].vol2Pass - p_stDataIn->iVol;
			}
			else
			{
				if(gstOrderSel[i].exec_acc_vol==0)
                    liVol2 = (int)((p_stDataIn->iVol - gstOrderSel[i].vol2Pass));
                else
                    liVol2 = (p_stDataIn->iVol - gstOrderSel[i].vol2Pass);
				gstOrderSel[i].vol2Pass = 0;
				liPosNew = MIN(gstOrderSel[i].volume, gstOrderSel[i].exec_acc_vol + liVol2);
				gstOrderSel[i].exec_acc_vol = liPosNew;
			}
		}
		else if (gstOrderSel[i].price < lrPriceH + lrTick/2)
		{
			if(liVHi < gstOrderSel[i].vol2Pass)
			{
				gstOrderSel[i].vol2Pass = gstOrderSel[i].vol2Pass - liVHi;
			}
			else
			{
				liVol2 = (liVHi - gstOrderSel[i].vol2Pass);
				gstOrderSel[i].vol2Pass = 0;
				liPosNew = MIN(gstOrderSel[i].volume, gstOrderSel[i].exec_acc_vol + liVol2);
				gstOrderSel[i].exec_acc_vol = liPosNew;
			}
		}
		liIndex = (int)((gstOrderSel[i].price - p_stDataIn->dPLmtDn)/lrTick + 0.5);
		gstOrderSel[i].vol2Pass = MIN(gstOrderSel[i].vol2Pass, laiFullOrderBookSel[liIndex]); // market order cancelations considered
		if (gstOrderSel[i].price < (larBP[4] + lrTick/2) && larBP[4] > lrTick
			&& laiBV[0]+laiBV[1]+laiBV[2]+laiBV[3]+laiBV[4] >= gstOrderSel[i].vol2Pass +
			abs(gstOrderSel[i].volume - gstOrderSel[i].exec_acc_vol))
		{
			liPosNew = MIN(gstOrderSel[i].volume, gstOrderSel[i].exec_acc_vol + (laiBV[0]+laiBV[1]+laiBV[2]+laiBV[3]+laiBV[4]));
			gstOrderSel[i].exec_acc_vol = liPosNew;

		}
		else if (gstOrderSel[i].price < (larBP[3] + lrTick/2) && larBP[3] > lrTick
			&& laiBV[0]+laiBV[1]+laiBV[2]+laiBV[3] >= gstOrderSel[i].vol2Pass +
			abs(gstOrderSel[i].volume - gstOrderSel[i].exec_acc_vol))
		{
			liPosNew = MIN(gstOrderSel[i].volume, gstOrderSel[i].exec_acc_vol + (laiBV[0]+laiBV[1]+laiBV[2]+laiBV[3]));
			gstOrderSel[i].exec_acc_vol = liPosNew ;

		}
		else if (gstOrderSel[i].price < (larBP[2] + lrTick/2) && larBP[2] > lrTick
			&& laiBV[0]+laiBV[1]+laiBV[2] >= gstOrderSel[i].vol2Pass +
			abs(gstOrderSel[i].volume - gstOrderSel[i].exec_acc_vol))
		{
			liPosNew = MIN(gstOrderSel[i].volume, gstOrderSel[i].exec_acc_vol + (laiBV[0]+laiBV[1]+laiBV[2]));
			gstOrderSel[i].exec_acc_vol = liPosNew;

		}
		else if (gstOrderSel[i].price < (larBP[1] + lrTick/2) && larBP[1] > lrTick
			&& laiBV[0]+laiBV[1] >= gstOrderSel[i].vol2Pass +
			abs(gstOrderSel[i].volume - gstOrderSel[i].exec_acc_vol))
		{
			liPosNew = MIN(gstOrderSel[i].volume, gstOrderSel[i].exec_acc_vol + (laiBV[0]+laiBV[1]));
			gstOrderSel[i].exec_acc_vol = liPosNew;
		}
		else if (gstOrderSel[i].price < (larBP[0] + lrTick/2) && larBP[0] > lrTick
			&& laiBV[0] >= gstOrderSel[i].vol2Pass +
			abs(gstOrderSel[i].volume - gstOrderSel[i].exec_acc_vol))
		{
			liPosNew = MIN(gstOrderSel[i].volume, gstOrderSel[i].exec_acc_vol + laiBV[0]);
			gstOrderSel[i].exec_acc_vol = liPosNew;
		}

		gstOrderSel[i].exec_vol = gstOrderSel[i].exec_acc_vol - liPreVol;
		gstOrderSel[i].exec_price = gstOrderSel[i].price;
		if(gstOrderSel[i].simu_status == 1) gstOrderSel[i].simu_status = 0;
		if(gstOrderSel[i].exec_vol != 0 )
        {
            if(gstOrderSel[i].kaiping == SIG_ACTION_OPEN)
                p_stLocTradeInfo->iPosK += gstOrderSel[i].exec_vol;
            else if(gstOrderSel[i].kaiping == SIG_ACTION_FLAT)
                p_stLocTradeInfo->iPosD -= gstOrderSel[i].exec_vol;

            p_stLocTradeInfo->iLastTradeTime = gstCurrIn.iTime;

            gstLastRec.iBS = -1;
            gstLastRec.rPrice = gstOrderSel[i].exec_price;
            gstLastRec.iOpenFlat = gstOrderSel[i].kaiping;
            gstLastRec.iVol = gstOrderSel[i].exec_vol;
            update_lastThreeRec(gstLastRec.iBS, gstLastRec.rPrice);
        }

//		if(gstOrderSel[i].exec_vol != 0)
//            printf("exec: %8.1f %d\n",gstOrderSel[i].exec_price, gstOrderSel[i].exec_vol);

	}

	return 0;
}

//----------------------------------------------------
//subroutine s_trade_simulation()
// description: simulate trade completion
//----------------------------------------------------
int s_trade_simulation_CFFE(struct in_data *p_stDataIn, struct in_data *p_stPreDataIn,
	struct all_parameter *p_stGlobalPara, int liTickNo,
	int *laiFullOrderBookBuy, int *laiFullOrderBookSel, int liLenFullOrderBook,
	st_loc_trade_info *p_stLocTradeInfo)
{
    double lrPriceFrom, lrPriceTo, lrSd, lrCumExeVol;
	double lrTradeP, lrPriceL, lrPriceH, lrTick, lrExeRatio;
	int  liVLo, liVHi, liVol2, liPosNew, liIndex, liLevel, liFrom, liTo, liI, i, liPreVol, liCumExeVol;
	double larBP[30], larSP[30];
	int laiBV[30], laiSV[30];
	int isum1, isum2;
	double dsum1, dsum2;

	memcpy(larBP, p_stDataIn->dBP, 30*sizeof(double));
	memcpy(larSP, p_stDataIn->dSP, 30*sizeof(double));
	memcpy(laiBV, p_stDataIn->iBV, 30*sizeof(int));
	memcpy(laiSV, p_stDataIn->iSV, 30*sizeof(int));

	liLevel = 5;
	lrExeRatio = p_stGlobalPara->stSec.rTVCompRatio;
	lrTick = p_stGlobalPara->stSec.rTickSize ;
	if (larBP[liLevel-1] < lrTick)
		liFrom = 0;
	else
		liFrom = MAX((int)((larBP[liLevel-1] - p_stDataIn->dPLmtDn)/lrTick+0.5) , 0);

	if (larSP[liLevel-1] < lrTick)
		liTo = liLenFullOrderBook - 1;
	else
		liTo = (int)((larSP[liLevel-1]-p_stDataIn->dPLmtDn)/lrTick+0.5);

	for(i=liFrom; i<=liTo; i++)
	{
		laiFullOrderBookBuy[i] = 0;
		laiFullOrderBookSel[i] = 0;
	}


	for(liI = 0; liI<liLevel; liI++)   // build order book
	{
		if (larBP[liI] > lrTick/2)
		{
			liIndex = (int)((larBP[liI]-p_stDataIn->dPLmtDn)/lrTick+0.5);
			laiFullOrderBookBuy[liIndex] = laiBV[liI];
		}
		if (larSP[liI] > lrTick/2)
		{
			liIndex = (int)((larSP[liI]-p_stDataIn->dPLmtDn)/lrTick+0.5);
			laiFullOrderBookSel[liIndex] = laiSV[liI];
		}
	}

	p_stDataIn->iVol = (int)(p_stDataIn->dTotV - p_stPreDataIn->dTotV)/p_stGlobalPara->stExch.iVolDivider;
	p_stDataIn->dOIChg = (p_stDataIn->dOI - p_stPreDataIn->dOI)/p_stGlobalPara->stExch.iVolDivider;
    p_stDataIn->dVal = (p_stDataIn->dTotVal - p_stPreDataIn->dTotVal)/p_stGlobalPara->stExch.iVolDivider;
    p_stDataIn->dAvgP = f_calc_curr_avgp(p_stDataIn->dVal, p_stDataIn->iVol, p_stPreDataIn->dAvgP, p_stGlobalPara)/
        p_stGlobalPara->stSec.iUnitPerContract;

	lrTradeP = p_stDataIn->dAvgP;

    if(lrTradeP >= p_stPreDataIn->dSP[0])
    {
        lrPriceL = p_stPreDataIn->dSP[0] - lrTick;
        lrPriceH = p_stPreDataIn->dSP[0];
        liVLo = 0;
        liVHi = p_stDataIn->iVol;
    }
    else if(lrTradeP <= p_stPreDataIn->dBP[0])
    {
        lrPriceL = p_stPreDataIn->dBP[0];
        lrPriceH = p_stPreDataIn->dBP[0]+lrTick;
        liVLo = p_stDataIn->iVol;
        liVHi = 0;
    }
    else
    {
        lrPriceL = (int)(lrTradeP/lrTick)*lrTick ;   // immediate tick price below tradep
        lrPriceH = lrPriceL + lrTick ;             // immediate tick price over tradep
        liVLo = (int)(p_stDataIn->iVol * (lrPriceH - lrTradeP) / lrTick + 0.5);   // last trade vol @ priceL
        liVHi = p_stDataIn->iVol - liVLo;   // last trade vol @ priceH
    }

	if ((lrTradeP < lrTick/2) || (p_stDataIn->iVol == 0)) return 0;

	if (p_stDataIn->dPLo < p_stPreDataIn->dPLo - lrTick/2)
		lrPriceFrom = p_stDataIn->dPLo;
	else
	{
		lrPriceFrom = MAX(p_stDataIn->dPLo, MIN(MIN(p_stDataIn->dBP[0], p_stPreDataIn->dBP[0]), p_stDataIn->dP));
		if (lrPriceL < lrPriceFrom - lrTick/2)
			lrPriceFrom = MAX(p_stDataIn->dPLo, MIN(MIN(p_stDataIn->dBP[1], p_stPreDataIn->dBP[1]), MIN(lrPriceFrom, lrPriceL)));

	}

	if (p_stDataIn->dPHi > p_stPreDataIn->dPHi + lrTick/2)
		lrPriceTo = p_stDataIn->dPHi;
	else
	{
		lrPriceTo = MIN(p_stPreDataIn->dPHi, MAX(MAX(p_stDataIn->dSP[0], p_stPreDataIn->dSP[0]), p_stDataIn->dP));
		if (lrPriceH > lrPriceTo + lrTick/2)
			lrPriceTo = MIN(p_stDataIn->dPHi, MAX(MAX(p_stDataIn->dSP[1], p_stPreDataIn->dSP[1]), MAX(lrPriceTo, lrPriceH)));
	}

	lrSd = 0.14 * (lrPriceTo - lrPriceFrom) * (lrPriceTo - lrPriceFrom);
	if (fabs(lrSd) < 0.0001) return 0;

	isum1 = 0; isum2 = 0; dsum1=0; dsum2 = 0;
	for(i=1; i<5; i++)
	{
		dsum1 = dsum1 + fabs(p_stDataIn->dBP[i] - p_stPreDataIn->dBP[i]);
		dsum2 = dsum2 + fabs(p_stDataIn->dSP[i] - p_stPreDataIn->dSP[i]);
		isum1 = isum1 + abs(p_stDataIn->iBV[i] - p_stPreDataIn->iBV[i]);
		isum2 = isum2 + abs(p_stDataIn->iSV[i] - p_stPreDataIn->iSV[i]);
	}

    for(i=0; i< giMaxMyOrder; i++)
    {
        if(gstOrderBuy[i].sig_id == 0) continue;
        if(strncmp(gstOrderBuy[i].szContract, p_stDataIn->cTicker,8) != 0) continue;

        if(gstOrderBuy[i].status == SIG_STATUS_INIT) gstOrderBuy[i].status = SIG_STATUS_ENTRUSTED;

        if(gstOrderBuy[i].direct != SIG_DIRECTION_BUY) continue;

        liPreVol = gstOrderBuy[i].exec_acc_vol;

        lrCumExeVol = liVLo * alnorm((gstOrderBuy[i].price + lrTick/2 - lrPriceL)/lrSd, 0) +
            liVHi * alnorm((gstOrderBuy[i].price + lrTick/2 - lrPriceH)/lrSd, 0);
		if (lrPriceFrom - lrTick/2 > gstOrderBuy[i].price)
			lrCumExeVol = 0.0;

		liCumExeVol = (int)lrCumExeVol;


        if(liCumExeVol < gstOrderBuy[i].vol2Pass)
        {
            gstOrderBuy[i].vol2Pass = gstOrderBuy[i].vol2Pass - liCumExeVol;
        }
        else
        {
            if(gstOrderBuy[i].exec_acc_vol==0)
                liVol2 = (int)((liCumExeVol - gstOrderBuy[i].vol2Pass)*lrExeRatio);
            else
                liVol2 = (liCumExeVol - gstOrderBuy[i].vol2Pass);
            gstOrderBuy[i].vol2Pass = 0;
            gstOrderBuy[i].exec_acc_vol = MIN(liVol2+gstOrderBuy[i].exec_acc_vol, gstOrderBuy[i].volume);
            gstOrderBuy[i].exec_price = gstOrderBuy[i].price;
        }

		liIndex = (int)((gstOrderBuy[i].price - p_stDataIn->dPLmtDn)/lrTick + 0.5);
		gstOrderBuy[i].vol2Pass = MIN(gstOrderBuy[i].vol2Pass, laiFullOrderBookBuy[liIndex]); // market order cancelations considered
		if ((fabs(dsum1) > 0.01 ||  fabs(dsum2) > 0.01 || isum1 != 0 || isum2 != 0) &&
			 p_stDataIn->iTime > p_stPreDataIn->iTime)
        {
            if (gstOrderBuy[i].price > (larSP[4] - lrTick/2) && larSP[4] > lrTick
                && laiSV[0]+laiSV[1]+laiSV[2]+laiSV[3]+laiSV[4] >= gstOrderBuy[i].vol2Pass +
                abs(gstOrderBuy[i].volume - gstOrderBuy[i].exec_acc_vol))
            {
                liPosNew = MIN(gstOrderBuy[i].volume, gstOrderBuy[i].exec_acc_vol + laiSV[0]+laiSV[1]+laiSV[2]+laiSV[3]+laiSV[4]);
                gstOrderBuy[i].exec_acc_vol = liPosNew;
            }
            else if (gstOrderBuy[i].price > (larSP[3] - lrTick/2) && larSP[3] > lrTick
                && laiSV[0]+laiSV[1]+laiSV[2]+laiSV[3] >= gstOrderBuy[i].vol2Pass +
                abs(gstOrderBuy[i].volume - gstOrderBuy[i].exec_acc_vol))
            {
                liPosNew = MIN(gstOrderBuy[i].volume, gstOrderBuy[i].exec_acc_vol + laiSV[0]+laiSV[1]+laiSV[2]+laiSV[3]);
                gstOrderBuy[i].exec_acc_vol = liPosNew;
            }
            else if (gstOrderBuy[i].price > (larSP[2] - lrTick/2) && larSP[2] > lrTick
                && laiSV[0]+laiSV[1]+laiSV[2] >= gstOrderBuy[i].vol2Pass +
                abs(gstOrderBuy[i].volume - gstOrderBuy[i].exec_acc_vol))
            {
                liPosNew = MIN(gstOrderBuy[i].volume, gstOrderBuy[i].exec_acc_vol + laiSV[0]+laiSV[1]+laiSV[2]);
                gstOrderBuy[i].exec_acc_vol = liPosNew;
            }
            else if (gstOrderBuy[i].price > (larSP[1] - lrTick/2) && larSP[1] > lrTick
                && laiSV[0]+laiSV[1] >= gstOrderBuy[i].vol2Pass +
                abs(gstOrderBuy[i].volume - gstOrderBuy[i].exec_acc_vol))
            {
                    liPosNew = MIN(gstOrderBuy[i].volume, gstOrderBuy[i].exec_acc_vol + laiSV[0]+laiSV[1]);
                    gstOrderBuy[i].exec_acc_vol = liPosNew;
            }
            else if (gstOrderBuy[i].price > (larSP[0] - lrTick/2) && larSP[0] > lrTick
                && laiSV[0] >= gstOrderBuy[i].vol2Pass + abs(gstOrderBuy[i].volume - gstOrderBuy[i].exec_acc_vol))
            {
                    liPosNew = MIN(gstOrderBuy[i].volume, gstOrderBuy[i].exec_acc_vol + laiSV[0]);
                    gstOrderBuy[i].exec_acc_vol = liPosNew;
            }
        }

		gstOrderBuy[i].exec_vol = gstOrderBuy[i].exec_acc_vol - liPreVol;
		gstOrderBuy[i].exec_price = gstOrderBuy[i].price;

		if(gstOrderBuy[i].exec_vol != 0 )
        {
            if(gstOrderBuy[i].kaiping == SIG_ACTION_OPEN)
                p_stLocTradeInfo->iPosD += gstOrderBuy[i].exec_vol;
            else if(gstOrderBuy[i].kaiping == SIG_ACTION_FLAT)
                p_stLocTradeInfo->iPosK -= gstOrderBuy[i].exec_vol;

            p_stLocTradeInfo->iLastTradeTime = gstCurrIn.iTime;

            gstLastRec.iBS = 1;
            gstLastRec.rPrice = gstOrderBuy[i].exec_price;
            gstLastRec.iOpenFlat = gstOrderBuy[i].kaiping;
            gstLastRec.iVol = gstOrderBuy[i].exec_vol;
            update_lastThreeRec(gstLastRec.iBS, gstLastRec.rPrice);
        }

		//if(gstOrderBuy[i].exec_vol != 0)
        //    printf("exec: %8.1f %d\n",gstOrderBuy[i].exec_price, gstOrderBuy[i].exec_vol);
	}

	for(i=0; i< giMaxMyOrder; i++)
	{
	    if(gstOrderSel[i].sig_id == 0) continue;
	    if(strncmp(gstOrderSel[i].szContract, p_stDataIn->cTicker,8) != 0) continue;

	    if(gstOrderSel[i].status == SIG_STATUS_INIT) gstOrderSel[i].status = SIG_STATUS_ENTRUSTED;
	    if(gstOrderSel[i].direct != SIG_DIRECTION_SELL) continue;

	    liPreVol = gstOrderSel[i].exec_acc_vol;

	    lrCumExeVol = liVLo * alnorm((gstOrderSel[i].price - lrTick/2 - lrPriceL)/lrSd, 1) +
            liVHi * alnorm((gstOrderSel[i].price - lrTick/2 - lrPriceH)/lrSd,1);
		if (lrPriceTo + lrTick/2 < gstOrderSel[i].price)
			lrCumExeVol = 0.0;

		liCumExeVol = (int)lrCumExeVol;

        if(liCumExeVol < gstOrderSel[i].vol2Pass)
        {
            gstOrderSel[i].vol2Pass = gstOrderSel[i].vol2Pass - liCumExeVol;
        }
        else
        {
            if(gstOrderSel[i].exec_acc_vol==0)
                liVol2 = (int)((liCumExeVol - gstOrderSel[i].vol2Pass)*lrExeRatio);
            else
                liVol2 = (liCumExeVol - gstOrderSel[i].vol2Pass);
            gstOrderSel[i].vol2Pass = 0;
            liPosNew = MIN(gstOrderSel[i].volume, gstOrderSel[i].exec_acc_vol + liVol2);
            gstOrderSel[i].exec_acc_vol = liPosNew;
        }

		liIndex = (int)((gstOrderSel[i].price - p_stDataIn->dPLmtDn)/lrTick + 0.5);
		gstOrderSel[i].vol2Pass = MIN(gstOrderSel[i].vol2Pass, laiFullOrderBookSel[liIndex]); // market order cancelations considered
		if ((fabs(dsum1) > 0.01 ||  fabs(dsum2) > 0.01 || isum1 != 0 || isum2 != 0) &&
			 p_stDataIn->iTime > p_stPreDataIn->iTime)
        {

            if (gstOrderSel[i].price < (larBP[4] + lrTick/2) && larBP[4] > lrTick
                && laiBV[0]+laiBV[1]+laiBV[2]+laiBV[3]+laiBV[4] >= gstOrderSel[i].vol2Pass +
                abs(gstOrderSel[i].volume - gstOrderSel[i].exec_acc_vol))
            {
                liPosNew = MIN(gstOrderSel[i].volume, gstOrderSel[i].exec_acc_vol + (laiBV[0]+laiBV[1]+laiBV[2]+laiBV[3]+laiBV[4]));
                gstOrderSel[i].exec_acc_vol = liPosNew;
            }
            else if (gstOrderSel[i].price < (larBP[3] + lrTick/2) && larBP[3] > lrTick
                && laiBV[0]+laiBV[1]+laiBV[2]+laiBV[3] >= gstOrderSel[i].vol2Pass +
                abs(gstOrderSel[i].volume - gstOrderSel[i].exec_acc_vol))
            {
                liPosNew = MIN(gstOrderSel[i].volume, gstOrderSel[i].exec_acc_vol + (laiBV[0]+laiBV[1]+laiBV[2]+laiBV[3]));
                gstOrderSel[i].exec_acc_vol = liPosNew ;
            }
            else if (gstOrderSel[i].price < (larBP[2] + lrTick/2) && larBP[2] > lrTick
                && laiBV[0]+laiBV[1]+laiBV[2] >= gstOrderSel[i].vol2Pass +
                abs(gstOrderSel[i].volume - gstOrderSel[i].exec_acc_vol))
            {
                liPosNew = MIN(gstOrderSel[i].volume, gstOrderSel[i].exec_acc_vol + (laiBV[0]+laiBV[1]+laiBV[2]));
                gstOrderSel[i].exec_acc_vol = liPosNew;
            }
            else if (gstOrderSel[i].price < (larBP[1] + lrTick/2) && larBP[1] > lrTick
                && laiBV[0]+laiBV[1] >= gstOrderSel[i].vol2Pass +
                abs(gstOrderSel[i].volume - gstOrderSel[i].exec_acc_vol))
            {
                liPosNew = MIN(gstOrderSel[i].volume, gstOrderSel[i].exec_acc_vol + (laiBV[0]+laiBV[1]));
                gstOrderSel[i].exec_acc_vol = liPosNew;
            }
            else if (gstOrderSel[i].price < (larBP[0] + lrTick/2) && larBP[0] > lrTick
                && laiBV[0] >= gstOrderSel[i].vol2Pass +
                abs(gstOrderSel[i].volume - gstOrderSel[i].exec_acc_vol))
            {
                liPosNew = MIN(gstOrderSel[i].volume, gstOrderSel[i].exec_acc_vol + laiBV[0]);
                gstOrderSel[i].exec_acc_vol = liPosNew;
            }
        }

		gstOrderSel[i].exec_vol = gstOrderSel[i].exec_acc_vol - liPreVol;
		gstOrderSel[i].exec_price = gstOrderSel[i].price;
		if(gstOrderSel[i].exec_vol != 0 )
        {
            if(gstOrderSel[i].kaiping == SIG_ACTION_OPEN)
                p_stLocTradeInfo->iPosK += gstOrderSel[i].exec_vol;
            else if(gstOrderSel[i].kaiping == SIG_ACTION_FLAT)
                p_stLocTradeInfo->iPosD -= gstOrderSel[i].exec_vol;

            p_stLocTradeInfo->iLastTradeTime = gstCurrIn.iTime;

            gstLastRec.iBS = -1;
            gstLastRec.rPrice = gstOrderSel[i].exec_price;
            gstLastRec.iOpenFlat = gstOrderSel[i].kaiping;
            gstLastRec.iVol = gstOrderSel[i].exec_vol;
            update_lastThreeRec(gstLastRec.iBS, gstLastRec.rPrice);
        }


		//if(gstOrderSel[i].exec_vol != 0)
        //    printf("exec: %8.1f %d\n",gstOrderSel[i].exec_price, gstOrderSel[i].exec_vol);

	}

	return 0;
}


//----------------------------------------------------
//subroutine s_trade_simulation()
// description: simulate trade completion
//----------------------------------------------------
int s_trade_simulation_CZCE(struct in_data *p_stDataIn, struct in_data *p_stPreDataIn,
	struct all_parameter *p_stGlobalPara, int liTickNo,
	int *laiFullOrderBookBuy, int *laiFullOrderBookSel, int liLenFullOrderBook,
	st_loc_trade_info *p_stLocTradeInfo)
{
	double lrTradeP, lrTick, lrExeRatio;
	int  liVol2, liPosNew, liIndex, liLevel, liFrom, liTo, liI, i, liPreVol, i1;
	double larBP[30], larSP[30];
	int laiBV[30], laiSV[30];
	int isum1,isum2;
	double dsum1, dsum2;

	memcpy(larBP, p_stDataIn->dBP, 30*sizeof(double));
	memcpy(larSP, p_stDataIn->dSP, 30*sizeof(double));
	memcpy(laiBV, p_stDataIn->iBV, 30*sizeof(int));
	memcpy(laiSV, p_stDataIn->iSV, 30*sizeof(int));

	liLevel = 5;
	lrExeRatio = p_stGlobalPara->stSec.rTVCompRatio;
	lrTick = p_stGlobalPara->stSec.rTickSize ;
	if (larBP[liLevel-1] < lrTick)
		liFrom = 0;
	else
		liFrom = MAX((int)((larBP[liLevel-1] - p_stDataIn->dPLmtDn)/lrTick+0.5) , 0);

	if (larSP[liLevel-1] < lrTick)
		liTo = liLenFullOrderBook - 1;
	else
		liTo = (int)((larSP[liLevel-1]-p_stDataIn->dPLmtDn)/lrTick+0.5);

	for(i=liFrom; i<=liTo; i++)
	{
		laiFullOrderBookBuy[i] = 0;
		laiFullOrderBookSel[i] = 0;
	}


	for(liI = 0; liI<liLevel; liI++)   // build order book
	{
		if (larBP[liI] > lrTick/2)
		{
			liIndex = (int)((larBP[liI]-p_stDataIn->dPLmtDn)/lrTick+0.5);
			laiFullOrderBookBuy[liIndex] = laiBV[liI];
		}
		if (larSP[liI] > lrTick/2)
		{
			liIndex = (int)((larSP[liI]-p_stDataIn->dPLmtDn)/lrTick+0.5);
			laiFullOrderBookSel[liIndex] = laiSV[liI];
		}
	}


    p_stDataIn->iVol = (int)(p_stDataIn->dTotV - p_stPreDataIn->dTotV)/p_stGlobalPara->stExch.iVolDivider;
	p_stDataIn->dOIChg = (p_stDataIn->dOI - p_stPreDataIn->dOI)/p_stGlobalPara->stExch.iVolDivider;
	if (gcExch == cCZCE)
	{
		p_stDataIn->dVal  = 0;
		p_stDataIn->dAvgP = p_stDataIn->dP;
	}
	else
	{
		p_stDataIn->dVal = (p_stDataIn->dTotVal - p_stPreDataIn->dTotVal)/p_stGlobalPara->stExch.iVolDivider;
		p_stDataIn->dAvgP = f_calc_curr_avgp(p_stDataIn->dVal, p_stDataIn->iVol, p_stPreDataIn->dAvgP, p_stGlobalPara)/
			p_stGlobalPara->stSec.iUnitPerContract;
	}
	lrTradeP = p_stDataIn->dAvgP;

	isum1 = 0; isum2 = 0; dsum1=0; dsum2 = 0;
	for(i=0; i<5; i++)
	{
		dsum1 = dsum1 + fabs(p_stDataIn->dBP[i] - p_stPreDataIn->dBP[i]);
		dsum2 = dsum2 + fabs(p_stDataIn->dSP[i] - p_stPreDataIn->dSP[i]);
		isum1 = isum1 + abs(p_stDataIn->iBV[i] - p_stPreDataIn->iBV[i]);
		isum2 = isum2 + abs(p_stDataIn->iSV[i] - p_stPreDataIn->iSV[i]);
	}

	if (fabs(dsum1) < 0.01 &&  fabs(dsum2) < 0.01 && isum1 == 0 && isum2 == 0 &&
			p_stDataIn->iTotalBuyVol == p_stPreDataIn->iTotalBuyVol &&
			p_stDataIn->iTotalSelVol == p_stPreDataIn->iTotalSelVol &&
			p_stDataIn->iVol ==  0)  return 0;

	isum1 = 0; isum2 = 0; dsum1=0; dsum2 = 0;
	for(i=1; i<5; i++)
	{
		dsum1 = dsum1 + fabs(p_stDataIn->dBP[i] - p_stPreDataIn->dBP[i]);
		dsum2 = dsum2 + fabs(p_stDataIn->dSP[i] - p_stPreDataIn->dSP[i]);
		isum1 = isum1 + abs(p_stDataIn->iBV[i] - p_stPreDataIn->iBV[i]);
		isum2 = isum2 + abs(p_stDataIn->iSV[i] - p_stPreDataIn->iSV[i]);
	}

    for(i=0; i< giMaxMyOrder; i++)
    {
        if(gstOrderBuy[i].sig_id == 0) continue;
        if(strncmp(gstOrderBuy[i].szContract, p_stDataIn->cTicker,8) != 0) continue;

        if(gstOrderBuy[i].status == SIG_STATUS_INIT) gstOrderBuy[i].status = SIG_STATUS_ENTRUSTED;

        if(gstOrderBuy[i].direct != SIG_DIRECTION_BUY) continue;

        liPreVol = gstOrderBuy[i].exec_acc_vol;

		if(gstOrderBuy[i].price > lrTradeP - lrTick/2)
		{
		    if(gstOrderBuy[i].simu_status == 1 && gstOrderBuy[i].price <= p_stDataIn->dBP[0]+0.1*lrTick)
            {
                for(i1=0; i1<5; i1++)
                {
                    if(fabs(gstOrderBuy[i].price - p_stDataIn->dBP[i1]) < 0.1*lrTick)
                    {
                        gstOrderBuy[i].vol2Pass = (p_stDataIn->iVol + p_stDataIn->iBV[i1])*lrExeRatio;
                        break;
                    }
                }
            }
			if(p_stDataIn->iVol < gstOrderBuy[i].vol2Pass)
			{
				gstOrderBuy[i].vol2Pass = gstOrderBuy[i].vol2Pass - p_stDataIn->iVol;
			}
			else
			{
				if(gstOrderBuy[i].exec_acc_vol==0)
                    liVol2 = (int)((p_stDataIn->iVol - gstOrderBuy[i].vol2Pass));
                else
                    liVol2 = (p_stDataIn->iVol - gstOrderBuy[i].vol2Pass);
				gstOrderBuy[i].vol2Pass = 0;
				gstOrderBuy[i].exec_acc_vol = MIN(liVol2 + gstOrderBuy[i].exec_acc_vol, gstOrderBuy[i].volume);
				gstOrderBuy[i].exec_price = gstOrderBuy[i].price;
			}
		}

		liIndex = (int)((gstOrderBuy[i].price - p_stDataIn->dPLmtDn)/lrTick + 0.5);
		gstOrderBuy[i].vol2Pass = MIN(gstOrderBuy[i].vol2Pass, laiFullOrderBookBuy[liIndex]); // market order cancelations considered
		if ((fabs(dsum1) > 0.01 ||  fabs(dsum2) > 0.01 || isum1 != 0 || isum2 != 0)
			&& p_stDataIn->iTime > p_stPreDataIn->iTime)
        {

            if (gstOrderBuy[i].price > (larSP[4] - lrTick/2) && larSP[4] > lrTick
                && laiSV[0]+laiSV[1]+laiSV[2]+laiSV[3]+laiSV[4] >= gstOrderBuy[i].vol2Pass +
                abs(gstOrderBuy[i].volume - gstOrderBuy[i].exec_acc_vol))
            {
                liPosNew = MIN(gstOrderBuy[i].volume, gstOrderBuy[i].exec_acc_vol + laiSV[0]+laiSV[1]+laiSV[2]+laiSV[3]+laiSV[4]);
                gstOrderBuy[i].exec_acc_vol = liPosNew;
            }
            else if (gstOrderBuy[i].price > (larSP[3] - lrTick/2) && larSP[3] > lrTick
                && laiSV[0]+laiSV[1]+laiSV[2]+laiSV[3] >= gstOrderBuy[i].vol2Pass +
                abs(gstOrderBuy[i].volume - gstOrderBuy[i].exec_acc_vol))
            {
                liPosNew = MIN(gstOrderBuy[i].volume, gstOrderBuy[i].exec_acc_vol + laiSV[0]+laiSV[1]+laiSV[2]+laiSV[3]);
                gstOrderBuy[i].exec_acc_vol = liPosNew;
            }
            else if (gstOrderBuy[i].price > (larSP[2] - lrTick/2) && larSP[2] > lrTick
                && laiSV[0]+laiSV[1]+laiSV[2] >= gstOrderBuy[i].vol2Pass +
                abs(gstOrderBuy[i].volume - gstOrderBuy[i].exec_acc_vol))
            {
                liPosNew = MIN(gstOrderBuy[i].volume, gstOrderBuy[i].exec_acc_vol + laiSV[0]+laiSV[1]+laiSV[2]);
                gstOrderBuy[i].exec_acc_vol = liPosNew;
            }
            else if (gstOrderBuy[i].price > (larSP[1] - lrTick/2) && larSP[1] > lrTick
                && laiSV[0]+laiSV[1] >= gstOrderBuy[i].vol2Pass +
                abs(gstOrderBuy[i].volume - gstOrderBuy[i].exec_acc_vol))
            {
                    liPosNew = MIN(gstOrderBuy[i].volume, gstOrderBuy[i].exec_acc_vol + laiSV[0]+laiSV[1]);
                    gstOrderBuy[i].exec_acc_vol = liPosNew;
            }
            else if (gstOrderBuy[i].price > (larSP[0] - lrTick/2) && larSP[0] > lrTick
                && laiSV[0] >= gstOrderBuy[i].vol2Pass + abs(gstOrderBuy[i].volume - gstOrderBuy[i].exec_acc_vol))
            {
                    liPosNew = MIN(gstOrderBuy[i].volume, gstOrderBuy[i].exec_acc_vol + laiSV[0]);
                    gstOrderBuy[i].exec_acc_vol = liPosNew;
            }
        }

		gstOrderBuy[i].exec_vol = gstOrderBuy[i].exec_acc_vol - liPreVol;
		gstOrderBuy[i].exec_price = gstOrderBuy[i].price;
        if(gstOrderBuy[i].simu_status == 1) gstOrderBuy[i].simu_status = 0;

		if(gstOrderBuy[i].exec_vol != 0 )
        {
            if(gstOrderBuy[i].kaiping == SIG_ACTION_OPEN)
                p_stLocTradeInfo->iPosD += gstOrderBuy[i].exec_vol;
            else if(gstOrderBuy[i].kaiping == SIG_ACTION_FLAT)
                p_stLocTradeInfo->iPosK -= gstOrderBuy[i].exec_vol;

            p_stLocTradeInfo->iLastTradeTime = gstCurrIn.iTime;

            gstLastRec.iBS = 1;
            gstLastRec.rPrice = gstOrderBuy[i].exec_price;
            gstLastRec.iOpenFlat = gstOrderBuy[i].kaiping;
            gstLastRec.iVol = gstOrderBuy[i].exec_vol;
            update_lastThreeRec(gstLastRec.iBS, gstLastRec.rPrice);
        }

		//if(gstOrderBuy[i].exec_vol != 0)
        //    printf("exec: %8.1f %d\n",gstOrderBuy[i].exec_price, gstOrderBuy[i].exec_vol);
	}

	for(i=0; i< giMaxMyOrder; i++)
	{
	    if(gstOrderSel[i].sig_id == 0) continue;
	    if(strncmp(gstOrderSel[i].szContract, p_stDataIn->cTicker,8) != 0) continue;

	    if(gstOrderSel[i].status == SIG_STATUS_INIT) gstOrderSel[i].status = SIG_STATUS_ENTRUSTED;
	    if(gstOrderSel[i].direct != SIG_DIRECTION_SELL) continue;

	    liPreVol = gstOrderSel[i].exec_acc_vol;

		if(gstOrderSel[i].price < lrTradeP + lrTick/2)
		{
		    if(gstOrderSel[i].simu_status == 1 && gstOrderSel[i].price >= p_stDataIn->dSP[0]-0.1*lrTick)
            {
                for(i1=0; i1<5; i1++)
                {
                    if(fabs(gstOrderSel[i].price - p_stDataIn->dSP[i1]) < 0.1*lrTick)
                    {
                        gstOrderSel[i].vol2Pass = (p_stDataIn->iVol + p_stDataIn->iSV[i1])*lrExeRatio;
                        break;
                    }
                }
            }
			if(p_stDataIn->iVol < gstOrderSel[i].vol2Pass)
			{
				gstOrderSel[i].vol2Pass = gstOrderSel[i].vol2Pass - p_stDataIn->iVol;
			}
			else
			{
				if(gstOrderSel[i].exec_acc_vol==0)
                    liVol2 = (int)((p_stDataIn->iVol - gstOrderSel[i].vol2Pass));
                else
                    liVol2 = (p_stDataIn->iVol - gstOrderSel[i].vol2Pass);
				gstOrderSel[i].vol2Pass = 0;
				liPosNew = MIN(gstOrderSel[i].volume, gstOrderSel[i].exec_acc_vol + liVol2);
				gstOrderSel[i].exec_acc_vol = liPosNew;
			}
		}

		liIndex = (int)((gstOrderSel[i].price - p_stDataIn->dPLmtDn)/lrTick + 0.5);
		gstOrderSel[i].vol2Pass = MIN(gstOrderSel[i].vol2Pass, laiFullOrderBookSel[liIndex]); // market order cancelations considered
		if ((fabs(dsum1) > 0.01 ||  fabs(dsum2) > 0.01 || isum1 != 0 || isum2 != 0)
			&& p_stDataIn->iTime > p_stPreDataIn->iTime)
        {

            if (gstOrderSel[i].price < (larBP[4] + lrTick/2) && larBP[4] > lrTick
                && laiBV[0]+laiBV[1]+laiBV[2]+laiBV[3]+laiBV[4] >= gstOrderSel[i].vol2Pass +
                abs(gstOrderSel[i].volume - gstOrderSel[i].exec_acc_vol))
            {
                liPosNew = MIN(gstOrderSel[i].volume, gstOrderSel[i].exec_acc_vol + (laiBV[0]+laiBV[1]+laiBV[2]+laiBV[3]+laiBV[4]));
                gstOrderSel[i].exec_acc_vol = liPosNew;
            }
            else if (gstOrderSel[i].price < (larBP[3] + lrTick/2) && larBP[3] > lrTick
                && laiBV[0]+laiBV[1]+laiBV[2]+laiBV[3] >= gstOrderSel[i].vol2Pass +
                abs(gstOrderSel[i].volume - gstOrderSel[i].exec_acc_vol))
            {
                liPosNew = MIN(gstOrderSel[i].volume, gstOrderSel[i].exec_acc_vol + (laiBV[0]+laiBV[1]+laiBV[2]+laiBV[3]));
                gstOrderSel[i].exec_acc_vol = liPosNew ;
            }
            else if (gstOrderSel[i].price < (larBP[2] + lrTick/2) && larBP[2] > lrTick
                && laiBV[0]+laiBV[1]+laiBV[2] >= gstOrderSel[i].vol2Pass +
                abs(gstOrderSel[i].volume - gstOrderSel[i].exec_acc_vol))
            {
                liPosNew = MIN(gstOrderSel[i].volume, gstOrderSel[i].exec_acc_vol + (laiBV[0]+laiBV[1]+laiBV[2]));
                gstOrderSel[i].exec_acc_vol = liPosNew;
            }
            else if (gstOrderSel[i].price < (larBP[1] + lrTick/2) && larBP[1] > lrTick
                && laiBV[0]+laiBV[1] >= gstOrderSel[i].vol2Pass +
                abs(gstOrderSel[i].volume - gstOrderSel[i].exec_acc_vol))
            {
                liPosNew = MIN(gstOrderSel[i].volume, gstOrderSel[i].exec_acc_vol + (laiBV[0]+laiBV[1]));
                gstOrderSel[i].exec_acc_vol = liPosNew;
            }
            else if (gstOrderSel[i].price < (larBP[0] + lrTick/2) && larBP[0] > lrTick
                && laiBV[0] >= gstOrderSel[i].vol2Pass +
                abs(gstOrderSel[i].volume - gstOrderSel[i].exec_acc_vol))
            {
                liPosNew = MIN(gstOrderSel[i].volume, gstOrderSel[i].exec_acc_vol + laiBV[0]);
                gstOrderSel[i].exec_acc_vol = liPosNew;
            }
        }

		gstOrderSel[i].exec_vol = gstOrderSel[i].exec_acc_vol - liPreVol;
		gstOrderSel[i].exec_price = gstOrderSel[i].price;
		if(gstOrderSel[i].simu_status == 1) gstOrderSel[i].simu_status = 0;

		if(gstOrderSel[i].exec_vol != 0 )
        {
            if(gstOrderSel[i].kaiping == SIG_ACTION_OPEN)
                p_stLocTradeInfo->iPosK += gstOrderSel[i].exec_vol;
            else if(gstOrderSel[i].kaiping == SIG_ACTION_FLAT)
                p_stLocTradeInfo->iPosD -= gstOrderSel[i].exec_vol;

            p_stLocTradeInfo->iLastTradeTime = gstCurrIn.iTime;

            gstLastRec.iBS = -1;
            gstLastRec.rPrice = gstOrderSel[i].exec_price;
            gstLastRec.iOpenFlat = gstOrderSel[i].kaiping;
            gstLastRec.iVol = gstOrderSel[i].exec_vol;
            update_lastThreeRec(gstLastRec.iBS, gstLastRec.rPrice);
        }


		//if(gstOrderSel[i].exec_vol != 0)
        //    printf("exec: %8.1f %d\n",gstOrderSel[i].exec_price, gstOrderSel[i].exec_vol);

	}

	return 0;
}

