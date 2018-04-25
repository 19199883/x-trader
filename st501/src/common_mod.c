#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <memory.h>
#include "common.h"
#include "common_mod.h"
#include "common_func.h"
#include "interface_v30.h"
#include "MK2_var_def.h"
#include "MK2_func.h"

//#define USE_OLD_LOG_

int s_read_fut_struct(int liTickNo)
{

	if(gcExch == cCFFE)
        s_read_gta_fut_struct(gstcStructCFFEIn, &gstCurrIn);
	else if(gcExch == cDCE)
        s_read_dl_fut_struct(gstcStructDCEIn, &gstCurrIn);
	else if(gcExch == cSHFE)
        s_read_sh_fut_struct(gstcStructSHFEIn, &gstCurrIn);
	else if(gcExch == cCZCE)
        s_read_zz_fut_struct(gstcStructCZCEIn, &gstCurrIn);

	return 0;
}

int s_set_global_parameters(char lcExch, char *lcSecName, struct all_parameter *p_stGlobalPara)
{
	s_set_exch_parameters(lcExch, lcSecName, p_stGlobalPara);
	s_set_convertion_parameters(lcSecName, &(p_stGlobalPara->stSec));

	return 0;
}

int f_check_valid_data(struct in_data *p_stCurrInData, struct in_data *p_stPreInData,
	int liTickNo, struct all_parameter *p_stGlobalPara)
{
	int ireturn = iYES;
	int i, isum1, isum2;
	double dsum1, dsum2, dsum3, dsum4;

	if ((int)(p_stCurrInData->dTotV + 0.5) % p_stGlobalPara->stExch.iVolDivider != 0)
	{	ireturn = iNO; }
	if ((p_stCurrInData->dBP[0] - p_stCurrInData->dSP[0]) > -p_stGlobalPara->stSec.rTickSize/2)
	{	ireturn = iNO; }
	if (p_stCurrInData->iVol < 0 || p_stCurrInData->dVal < 0 )
	{	ireturn = iNO; }
	if (p_stCurrInData->dPLmtUp < p_stGlobalPara->stSec.rTickSize || p_stCurrInData->dPLmtDn < p_stGlobalPara->stSec.rTickSize )
	{	ireturn = iNO; }
	if (gcExch == cSHFE && fabs(p_stCurrInData->dSP[0] - p_stCurrInData->dBP[0]) > 6*p_stGlobalPara->stSec.rTickSize)
	{	ireturn = iNO; }

	if (abs(p_stCurrInData->iVol) > p_stGlobalPara->stSec.iWrongVol || p_stCurrInData->iVol < 0)
	{	ireturn = iNO; }

	if (gcExch != cCZCE)
    {
        if(p_stCurrInData->iTime < p_stPreInData->iTime)
        {	ireturn = iNO; }

        if(liTickNo==1)
            giMaxTime = p_stCurrInData->iTime;
        else
        {
            if(p_stCurrInData->iTime < giMaxTime)
            {	ireturn = iNO; }
            else
            {
                giMaxTime = p_stCurrInData->iTime;
            }
        }
    }


	if (p_stCurrInData->iVol == 0 && p_stCurrInData->dVal > 1.0 * p_stCurrInData->dP)
	{	ireturn = iNO;}

	if (p_stCurrInData->dVal > 1.0 && p_stCurrInData->iVol >= 1)
	{
		if (p_stCurrInData->dAvgP > (p_stCurrInData->dPLmtUp + p_stGlobalPara->stSec.rTickSize/2) ) ireturn = iNO;
		if (p_stCurrInData->dAvgP < (p_stCurrInData->dPLmtDn - p_stGlobalPara->stSec.rTickSize/2) ) ireturn = iNO;
	}


	isum1 = 0; isum2 = 0; dsum1=0; dsum2 = 0;
	dsum3 = 0; dsum4 = 0;
	for(i=0; i<5; i++)
	{
		dsum1 = dsum1 + fabs(p_stCurrInData->dBP[i] - p_stPreInData->dBP[i]);
		dsum2 = dsum2 + fabs(p_stCurrInData->dSP[i] - p_stPreInData->dSP[i]);
		isum1 = isum1 + abs(p_stCurrInData->iBV[i] - p_stPreInData->iBV[i]);
		isum2 = isum2 + abs(p_stCurrInData->iSV[i] - p_stPreInData->iSV[i]);
		if(i>0)
        {
            dsum3 += fabs(p_stCurrInData->dBP[i]);
            dsum4 += fabs(p_stCurrInData->dSP[i]);
        }
	}
	if((p_stCurrInData->dPLmtUp - p_stCurrInData->dSP[0] > 4*p_stGlobalPara->stSec.rTickSize
       && dsum3 < p_stGlobalPara->stSec.rTickSize) ||
       (p_stCurrInData->dBP[0] - p_stCurrInData->dPLmtDn > 4*p_stGlobalPara->stSec.rTickSize
       && dsum4 < p_stGlobalPara->stSec.rTickSize))
    {
        ireturn = iNO;
    }
	if (gcExch != cCZCE)
	{
		if (liTickNo > 1 && p_stCurrInData->iTime == p_stPreInData->iTime)
			ireturn = iNO;

		if ( fabs(dsum1) < 0.0001 && fabs(dsum2) <0.0001 && isum1 == 0 && isum2 == 0 &&
			p_stCurrInData->iVol == 0) ireturn = iNO;
	}
	else
	{
		{
			if (dsum1 < 0.01 && dsum2 < 0.01 && isum1 == 0 && isum2 == 0 &&
			(liTickNo > 1))
				ireturn = iNO;
		}
	}

	return ireturn;
}

//----------------------------------------------------
// subroutine mysub0(liFileNumber, lcFileName)
// description: initialization function
// input: liFileNumber
//        lcFileName
//----------------------------------------------------
void mysub0(int liFileNumber, char *lcFileName)
{
    int i;
	//pfDayLogFile = liFileNumber;
	strncpy(gcFileName, lcFileName, 500);


	giSizePerTrade = 0;
	grSimuPara1 = 0;
	grSimuPara2 = 0;
	grSimuPara3 = 0;
	giWriteOutputFileFlg = 1;
	giWriteOutputFileInitFlg = 0;
	giRunModeFlg = 1;
	giMaxTime = 0;
	for(i=0; i<giNumContract; i++)
    {
        giInitFlg[i] = 0;
        giLenFullOrderBook[i] = 0;
    }


	s_init_MK2_parameters();
	s_initializer_usr_order();
	s_initializer_my_order();
}



int s_output_log(int liTickNo, FILE *p_iFileNumber, char *lcFileName, struct strat_out_log *outlog)
{
    s_MK2_output_log_new(liTickNo, outlog);


	return 0;
}

int s_update_full_order_book(struct in_data *lstCurrInData, struct in_data *lstPreInData,
	struct all_parameter *lstGlobalPara,int *laiFullOrderBookBuy, int *laiFullOrderBookSel,
	int liLenFullOrderBook)
{
    double larBP[30], larSP[30], lrTick;
	int laiBV[30], laiSV[30], laiImpBV[30], laiImpSV[30];
	int liLevel,liIndex;
	int liRefreshBidFrom, liRefreshBidTo, liRefreshAskFrom, liRefreshAskTo;
	int i;

    lrTick = lstGlobalPara->stSec.rTickSize;
	memcpy(larBP, lstCurrInData->dBP, sizeof(double)*30);
	memcpy(larSP, lstCurrInData->dSP, sizeof(double)*30);
	memcpy(laiBV, lstCurrInData->iBV, sizeof(int)*30);
	memcpy(laiSV, lstCurrInData->iSV, sizeof(int)*30);
	memcpy(laiImpBV, lstCurrInData->iImpBV, sizeof(int)*30);
	memcpy(laiImpSV, lstCurrInData->iImpSV, sizeof(int)*30);

	liLevel = lstGlobalPara->stExch.iPInLevel;


	//lrOI = lstCurrInData->dOI;

	gstMK2_Paras.iLimitStop = iNO;
	if ((larBP[0] < lstCurrInData->dPLmtDn + gstMK2_Paras.iLimitRange * lrTick) ||
		(larSP[0] > lstCurrInData->dPLmtUp - gstMK2_Paras.iLimitRange * lrTick))
			gstMK2_Paras.iLimitStop = iYES;

	if (larBP[liLevel-1] < lrTick)
		liRefreshBidFrom = 0;
	else
		liRefreshBidFrom = MAX((int)((larBP[liLevel-1]-lstCurrInData->dPLmtDn)/lrTick+0.5), 0);

	liRefreshBidTo = MAX((int)((MAX(larBP[0], lstPreInData->dBP[0]) - lstCurrInData->dPLmtDn)/lrTick+0.5) , 0);


	if (larSP[liLevel-1] < lrTick)
		liRefreshAskTo = liLenFullOrderBook -1;
	else
		liRefreshAskTo = (int)((larSP[liLevel-1]-lstCurrInData->dPLmtDn)/lrTick+0.5);

	if (larSP[0] < lrTick && lstPreInData->dSP[0] < lrTick)
		liRefreshAskFrom = liLenFullOrderBook -1;
	else if (MIN(larSP[0], lstPreInData->dSP[0]) < lrTick)
		liRefreshAskFrom = (int)((MAX(larSP[0], lstPreInData->dSP[0])-lstCurrInData->dPLmtDn)/lrTick+0.5);
	else
		liRefreshAskFrom = (int)((MIN(larSP[0], lstPreInData->dSP[0])-lstCurrInData->dPLmtDn)/lrTick+0.5);

	// reset buffer
	for(i= liRefreshBidFrom; i<= liRefreshBidTo; i++)
		laiFullOrderBookBuy[i] = 0;
	for(i= liRefreshAskFrom; i<= liRefreshAskTo; i++)
		laiFullOrderBookSel[i] = 0;


	for(i=0; i<liLevel; i++)
	{
		if (larBP[i] > lrTick/2 && larBP[i] - lstCurrInData->dPLmtDn > lrTick/2) //don't update lower_limit
		{
			liIndex = (int)((larBP[i]-lstCurrInData->dPLmtDn)/lrTick+0.5);
			laiFullOrderBookBuy[liIndex] = laiBV[i] - laiImpBV[i];
		}
		if (larSP[i] > lrTick/2 && larSP[i] - lstCurrInData->dPLmtUp < -lrTick/2) //don't update upper_limit
		{
			liIndex = (int)((larSP[i]-lstCurrInData->dPLmtDn)/lrTick+0.5);
			laiFullOrderBookSel[liIndex] = laiSV[i] - laiImpSV[i];
		}
	}

	if(giTickNo <= 1)
    {
        memcpy(gaiPreOrderBookBuy[giTradeInfo], laiFullOrderBookBuy, sizeof(int)*liLenFullOrderBook);
        memcpy(gaiPreOrderBookSel[giTradeInfo], laiFullOrderBookSel, sizeof(int)*liLenFullOrderBook);
    }

    return 0;
}


int do_list_sorting(double *old_lst, double* sorted_lst, int num_lst, int ibegin)
{
    int i,i1,i2;

    if(ibegin >= num_lst) return 1;

    for(i=ibegin; i<num_lst; i++)
        for(i1=0; i1 <= i; i1++)
    {
        if(i1 == i)
        {
            sorted_lst[i1]=old_lst[i];
            break;
        }

        if(old_lst[i] < sorted_lst[i1])
        {
            for(i2=i;i2>i1;i2--)
            {
                sorted_lst[i2] = sorted_lst[i2-1];
            }
            sorted_lst[i1] = old_lst[i];
            break;
        }
    }

    return 0;
}

double find_quantile_value(double* sorted_lst, int num_lst, double dquant)
{
    int itmp;

    if(dquant > 1.0) {printf("quantile error!\n"); return 0;}

    itmp = (int)(dquant * num_lst);

    return sorted_lst[itmp];
}

int s2_rss_secname_mapping(char *lcSecName,char *lcRssSecName)
{
	int i;
	char szName[32];

	for(i=0; i<(int)MIN(strlen(lcSecName),32); i++)
	{
		szName[i] = tolower(lcSecName[i]);
	}
	szName[strlen(lcSecName)] = '\0';

	if(strncmp(szName, "if", 7) == 0)
		strcpy(lcRssSecName, "IF");
	else if(strncmp(szName, "ic", 7) == 0)
		strcpy(lcRssSecName, "IC");
	else if(strncmp(szName, "ih", 7) == 0)
		strcpy(lcRssSecName, "IH");
	else if(strncmp(szName, "tf", 7) == 0)
		strcpy(lcRssSecName, "TF");
	else if(strncmp(szName, "t", 7) == 0)
		strcpy(lcRssSecName, "T");
	else if(strncmp(szName, "ag", 7) == 0)
		strcpy(lcRssSecName, "shag");
	else if(strncmp(szName, "au", 7) == 0)
		strcpy(lcRssSecName, "shau");
	else if(strncmp(szName, "al", 7) == 0)
		strcpy(lcRssSecName, "shal");
	else if(strncmp(szName, "bu", 7) == 0)
		strcpy(lcRssSecName, "shbu");
	else if(strncmp(szName, "cu", 7) == 0)
		strcpy(lcRssSecName, "shcu");
    else if(strncmp(szName, "hc", 7) == 0)
		strcpy(lcRssSecName, "shhc");
    else if(strncmp(szName, "pb", 7) == 0)
		strcpy(lcRssSecName, "shpb");
	else if(strncmp(szName, "rb", 7) == 0)
		strcpy(lcRssSecName, "shrb");
	else if(strncmp(szName, "ru", 7) == 0)
		strcpy(lcRssSecName, "shru");
	else if(strncmp(szName, "zn", 7) == 0)
		strcpy(lcRssSecName, "shzn");
	else if(strncmp(szName, "ni", 7) == 0)
		strcpy(lcRssSecName, "shni");
	else if(strncmp(szName, "a", 7) == 0)
		strcpy(lcRssSecName, "dla");
	else if(strncmp(szName, "bb", 7) == 0)
		strcpy(lcRssSecName, "dlbb");
	else if(strncmp(szName, "c", 7) == 0)
		strcpy(lcRssSecName, "dlc");
	else if(strncmp(szName, "cs", 7) == 0)
		strcpy(lcRssSecName, "dlcs");
	else if(strncmp(szName, "fb", 7) == 0)
		strcpy(lcRssSecName, "dlfb");
	else if(strncmp(szName, "i", 7) == 0)
		strcpy(lcRssSecName, "dli");
	else if(strncmp(szName, "j", 7) == 0)
		strcpy(lcRssSecName, "dlj");
	else if(strncmp(szName, "jd", 7) == 0)
		strcpy(lcRssSecName, "dljd");
	else if(strncmp(szName, "jm", 7) == 0)
		strcpy(lcRssSecName, "dljm");
	else if(strncmp(szName, "l", 7) == 0)
		strcpy(lcRssSecName, "dll");
	else if(strncmp(szName, "m", 7) == 0)
		strcpy(lcRssSecName, "dlm");
	else if(strncmp(szName, "p", 7) == 0)
		strcpy(lcRssSecName, "dlp");
	else if(strncmp(szName, "pp", 7) == 0)
		strcpy(lcRssSecName, "dlpp");
    else if(strncmp(szName, "v", 7) == 0)
		strcpy(lcRssSecName, "dlv");
	else if(strncmp(szName, "y", 7) == 0)
		strcpy(lcRssSecName, "dly");
	else if(strncmp(szName, "cf", 7) == 0)
		strcpy(lcRssSecName, "zzcf");
    else if(strncmp(szName, "fg", 7) == 0)
		strcpy(lcRssSecName, "zzfg");
    else if(strncmp(szName, "ma", 7) == 0)
		strcpy(lcRssSecName, "zzma");
    else if(strncmp(szName, "oi", 7) == 0)
		strcpy(lcRssSecName, "zzoi");
    else if(strncmp(szName, "rm", 7) == 0)
		strcpy(lcRssSecName, "zzrm");
    else if(strncmp(szName, "sf", 7) == 0)
		strcpy(lcRssSecName, "zzsf");
    else if(strncmp(szName, "sm", 7) == 0)
		strcpy(lcRssSecName, "zzsm");
	else if(strncmp(szName, "sr", 7) == 0)
		strcpy(lcRssSecName, "zzsr");
	else if(strncmp(szName, "ta", 7) == 0)
		strcpy(lcRssSecName, "zzta");
    else if(strncmp(szName, "wh", 7) == 0)
		strcpy(lcRssSecName, "zzwh");
    else if(strncmp(szName, "zc", 7) == 0)
		strcpy(lcRssSecName, "zzzc");
    else if(strncmp(szName, "ap", 7) == 0)
		strcpy(lcRssSecName, "zzap");

	return 0;
}

int s_load_oc_datamap(char *lcString, struct st_date_open_close_param *m_oc_param)
{

     int i;
     //int liRankin, liDate;
	 char *r;

     char buff[1024];
     int ipos[16], i0, i1;
     //char szpath[256], szTmp1[16], szTmp2[16], szTmp3[16];


     m_oc_param->iDate = 0;

     strncpy(buff, lcString, 1000);

     i1 = strlen(buff);
     i0 = 0;
     for(i=0; i<i1; i++)
     {
         if(buff[i] == '|' && i0<7)
         {
             ipos[i0] = i; buff[i] = '\0';
             i0++;
         }
     }

     if(i0 < 5) goto END;

     i = 0; r = &buff[i];
     m_oc_param->iDate = atoi(r);

     i = ipos[0] + 1; r = &buff[i];
     memcpy(m_oc_param->szSec, r, sizeof(char)*(strlen(r)+1));

     i = ipos[1] + 1; r = &buff[i];
     m_oc_param->iStratID = atoi(r);

     i = ipos[2] + 1; r = &buff[i];
     m_oc_param->iFlag = atoi(r);

     i = ipos[3] + 1; r = &buff[i];
     m_oc_param->dOpenParam =atof(r);

     i = ipos[4] + 1; r = &buff[i];
     m_oc_param->dCloseParam =atof(r);

     //printf("%d %s %d %0.4f %0.4f\n",m_oc_param->iDate,m_oc_param->szSec,m_oc_param->iFlag,m_oc_param->dOpenParam,m_oc_param->dCloseParam);

    return 0;
END:	return 1;
}

int s_read_ev_file(char* ev_name, double *pdOpenParam, double *pdCloseParam)
{
     char buff[1024];
     FILE * pfile;
     struct st_date_open_close_param cur_oc_param;
     char lcRssSecName[16];
     struct st_date_open_close_param m_oc[1000];
     int num_oc = 0;
     int i,i0,idiff;

     pfile = fopen(ev_name, "r");
     if(pfile == NULL) return 0;

     s2_rss_secname_mapping(gcSecName, lcRssSecName);
     while(fgets(buff, 1000, pfile) != NULL)
     {
        s_load_oc_datamap(buff, &cur_oc_param);
        if(cur_oc_param.iDate <= giCurDate && strcmp(lcRssSecName,cur_oc_param.szSec) == 0
           && cur_oc_param.iFlag == giNightMarket && cur_oc_param.iStratID == giStratID)
        {
            memcpy(&(m_oc[num_oc]), &cur_oc_param, sizeof(struct st_date_open_close_param));
            num_oc ++;
        }
     }
     fclose(pfile);

     i0 = 0;
     idiff = giCurDate - m_oc[0].iDate;
     for(i=1; i<num_oc; i++)
     {
         if(idiff > giCurDate - m_oc[i].iDate)
         {
             i0 = i;
             idiff = giCurDate - m_oc[i].iDate;
         }
     }
     *pdOpenParam = m_oc[i0].dOpenParam;
     *pdCloseParam = m_oc[i0].dCloseParam;
     //printf("%d %d %d %0.2f %0.2f\n",giCurDate,num_oc,i0, *pdOpenParam, *pdCloseParam);
     if(*pdOpenParam <= 1.0 || *pdCloseParam <= 1.0)
     {
         *pdOpenParam = 10000.0;
         *pdCloseParam = 1.2;
     }

     return 1;
}

int transfer_2_millisec(int itime)
{
    int isec,ims;
    int ihour, imin;
    ihour = itime/10000000;
    imin = (itime%10000000)/100000;
    isec = (itime%100000)/1000;
    ims = itime%1000;
    return (ihour*3600 + imin*60 + isec)*1000+ims;
}

int strat_divide_curr_volumn(struct in_data *p_stDataIn, struct in_data *p_stPreDataIn,
                         struct all_parameter *p_stGlobalPara,
                         double *pr_PriceLo, double *pr_PriceHi,int *pVolLo, int *pVolHi)
{
    int liVLo, liVHi;
    double lrTradeP, lrPriceL, lrPriceH;

    double lrTick = p_stGlobalPara->stSec.rTickSize ;

	lrTradeP = p_stDataIn->dAvgP;

    if(p_stPreDataIn->dSP[0] < lrTick)
    {
        lrPriceL = lrTradeP;
        lrPriceH = lrTradeP;
        liVLo = p_stDataIn->iVol;
        liVHi = 0;
    }
    else if(p_stPreDataIn->dBP[0] < lrTick)
    {
        lrPriceL = lrTradeP;
        lrPriceH = lrTradeP;
        liVLo = 0;
        liVHi = p_stDataIn->iVol;
    }
    else if(lrTradeP >= p_stPreDataIn->dSP[0])
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

    *pr_PriceLo = lrPriceL;
    *pr_PriceHi = lrPriceH;
    *pVolLo = liVLo;
    *pVolHi = liVHi;

    return 0;
}











