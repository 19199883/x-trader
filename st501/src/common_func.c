#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <memory.h>
#include "common.h"
#include "common_mod.h"
#include "common_func.h"

int giNightMarket = 0;  // 1: night;   0: day;
int giRunModeFlg = 1;  // 1: real time runing; 0: simulation;
int giStrateType = iSTRATEGY_MK2;
int giMK2Type = iMK201;
int giStratID = 501;

int giTimeDelay = 10000; //in millisecond

char gcExch, gcSecName[8], gcTicker[8];
int giCurDate;
char gszEVname[256];

int  giLb, giLen;
int  giPosLb, giPosLen, giPendVLb, giPendVLen;

struct in_data  gstCurrIn, gstPreIn[MAX_NUM_CONTRACT], gstRecCurrIn,
		gstRecPreIn[MAX_NUM_CONTRACT];
struct cfex_deep_quote     *gstcStructCFFEIn;
struct cStruct_dl_quote     *gstcStructDCEIn;
struct shfe_my_quote     *gstcStructSHFEIn;
struct czce_my_deep_quote     *gstcStructCZCEIn;

st_all_parameter    gstGlobalPara[MAX_NUM_CONTRACT];
int  giTickNo, giSumExchangeVol;

int *gaiFullOrderBookBuy[MAX_NUM_CONTRACT];
int *gaiFullOrderBookSel[MAX_NUM_CONTRACT];
int *gaiPreOrderBookBuy[MAX_NUM_CONTRACT];
int *gaiPreOrderBookSel[MAX_NUM_CONTRACT];

int  giLenFullOrderBook[MAX_NUM_CONTRACT];
double  grSimuPara1, grSimuPara2, grSimuPara3;
int  giSizePerTrade;
FILE  *pfDayLogFile;
char  gcFileName[512];
int  giInitFlg[MAX_NUM_CONTRACT];
int  giWriteOutputFileFlg, giWriteOutputFileInitFlg;

double grSimuValueTot, grSimuValueTemp, grSimuValueMax, grSimuDrawDown;
double grTotBench, grSimuDrawDownRatio;
int  giMaxTime;

int giStID, giMaxPos;

st_loc_trade_info gstLocTradeInfo[MAX_NUM_CONTRACT];
int giNumContract, giTradeInfo;
char file_history_log[1500];



int s_read_gta_fut_struct(struct cfex_deep_quote *p_stGtaQuoteIn, struct in_data *p_stInData)
{
	char  lcTemp[20];

	p_stInData->dP = p_stGtaQuoteIn->dLastPrice;
	p_stInData->dYestP = p_stGtaQuoteIn->dPreSettlementPrice;
	p_stInData->dPYestCl = p_stGtaQuoteIn->dPreClosePrice;
	p_stInData->dYestOI = p_stGtaQuoteIn->dPreOpenInterest;
	p_stInData->dPOpen = p_stGtaQuoteIn->dOpenPrice;
	p_stInData->dPHi = p_stGtaQuoteIn->dHighestPrice;
	p_stInData->dPLo = p_stGtaQuoteIn->dLowestPrice;
	p_stInData->dTotV = p_stGtaQuoteIn->nVolume;
	p_stInData->dTotVal = p_stGtaQuoteIn->dTurnover;
	p_stInData->dOI = p_stGtaQuoteIn->dOpenInterest;
	p_stInData->dPClose = p_stGtaQuoteIn->dClosePrice;
	p_stInData->dPSettle = p_stGtaQuoteIn->dSettlementPrice;
	p_stInData->dPLmtUp = p_stGtaQuoteIn->dUpperLimitPrice;
	p_stInData->dPLmtDn = p_stGtaQuoteIn->dLowerLimitPrice;

	strncpy(lcTemp, p_stGtaQuoteIn->szUpdateTime, strlen(p_stGtaQuoteIn->szUpdateTime));
	p_stInData->cTime[0] = lcTemp[0]; p_stInData->cTime[1] = lcTemp[1];
	p_stInData->cTime[2] = lcTemp[3]; p_stInData->cTime[3] = lcTemp[4];
	p_stInData->cTime[4] = lcTemp[6]; p_stInData->cTime[5] = lcTemp[7];
	p_stInData->cTime[6] = '\0';

	p_stInData->iTime= atoi(p_stInData->cTime);
	p_stInData->iTime = p_stInData->iTime * 1000 + p_stGtaQuoteIn->nUpdateMillisec;
	if (p_stInData->iTime < 30000000) p_stInData->iTime = p_stInData->iTime + 240000000;

	strncpy(p_stInData->cTicker, p_stGtaQuoteIn->szInstrumentID, 7);

	p_stInData->dBP1 = p_stGtaQuoteIn->dBidPrice1; p_stInData->iBV1 = p_stGtaQuoteIn->nBidVolume1;
	p_stInData->dSP1 = p_stGtaQuoteIn->dAskPrice1; p_stInData->iSV1 = p_stGtaQuoteIn->nAskVolume1;
	p_stInData->dBP2 = p_stGtaQuoteIn->dBidPrice2; p_stInData->iBV2 = p_stGtaQuoteIn->nBidVolume2;
	p_stInData->dSP2 = p_stGtaQuoteIn->dAskPrice2; p_stInData->iSV2 = p_stGtaQuoteIn->nAskVolume2;
	p_stInData->dBP3 = p_stGtaQuoteIn->dBidPrice3; p_stInData->iBV3 = p_stGtaQuoteIn->nBidVolume3;
	p_stInData->dSP3 = p_stGtaQuoteIn->dAskPrice3; p_stInData->iSV3 = p_stGtaQuoteIn->nAskVolume3;
	p_stInData->dBP4 = p_stGtaQuoteIn->dBidPrice4; p_stInData->iBV4 = p_stGtaQuoteIn->nBidVolume4;
	p_stInData->dSP4 = p_stGtaQuoteIn->dAskPrice4; p_stInData->iSV4 = p_stGtaQuoteIn->nAskVolume4;
	p_stInData->dBP5 = p_stGtaQuoteIn->dBidPrice5; p_stInData->iBV5 = p_stGtaQuoteIn->nBidVolume5;
	p_stInData->dSP5 = p_stGtaQuoteIn->dAskPrice5; p_stInData->iSV5 = p_stGtaQuoteIn->nAskVolume5;
	p_stInData->cExchCode = cCFFE;
	p_stInData->cSecType = cFUT;
	p_stInData->dV = 0;
	p_stInData->dVal = 0; p_stInData->dPMean = 0;

	p_stInData->dBP[0] = p_stInData->dBP1; p_stInData->dBP[1] = p_stInData->dBP2; p_stInData->dBP[2] = p_stInData->dBP3; p_stInData->dBP[3] = p_stInData->dBP4; p_stInData->dBP[4] = p_stInData->dBP5;
	p_stInData->dSP[0] = p_stInData->dSP1; p_stInData->dSP[1] = p_stInData->dSP2; p_stInData->dSP[2] = p_stInData->dSP3; p_stInData->dSP[3] = p_stInData->dSP4;	p_stInData->dSP[4] = p_stInData->dSP5;
	p_stInData->iBV[0] = p_stInData->iBV1; p_stInData->iBV[1] = p_stInData->iBV2; p_stInData->iBV[2] = p_stInData->iBV3; p_stInData->iBV[3] = p_stInData->iBV4; p_stInData->iBV[4] = p_stInData->iBV5;
	p_stInData->iSV[0] = p_stInData->iSV1; p_stInData->iSV[1] = p_stInData->iSV2; p_stInData->iSV[2] = p_stInData->iSV3; p_stInData->iSV[3] = p_stInData->iSV4;	p_stInData->iSV[4] = p_stInData->iSV5;
	memset(p_stInData->iImpBV, 0, sizeof(int)*30);
	memset(p_stInData->iImpSV, 0, sizeof(int)*30);

	return 0;
}

int s_read_sh_fut_struct(struct shfe_my_quote *p_stSHQuoteIn, struct in_data *p_stInData)
{
	char lcTemp[20];
    int i;

	p_stInData->dP = p_stSHQuoteIn->LastPrice;
	p_stInData->dYestP = p_stSHQuoteIn->PreSettlementPrice;
	p_stInData->dPYestCl = p_stSHQuoteIn->PreClosePrice;
	p_stInData->dYestOI = p_stSHQuoteIn->PreOpenInterest;
	p_stInData->dPOpen = p_stSHQuoteIn->OpenPrice;
	p_stInData->dPHi = p_stSHQuoteIn->HighestPrice;
	p_stInData->dPLo = p_stSHQuoteIn->LowestPrice;
	p_stInData->dTotV = p_stSHQuoteIn->Volume;
	p_stInData->dTotVal = p_stSHQuoteIn->Turnover;
	p_stInData->dOI = p_stSHQuoteIn->OpenInterest;
	p_stInData->dPClose = p_stSHQuoteIn->ClosePrice;
	p_stInData->dPSettle = p_stSHQuoteIn->SettlementPrice;
	p_stInData->dPLmtUp = p_stSHQuoteIn->UpperLimitPrice;
	p_stInData->dPLmtDn = p_stSHQuoteIn->LowerLimitPrice;

	strncpy(lcTemp, p_stSHQuoteIn->UpdateTime, 9);
	p_stInData->cTime[0] = lcTemp[0]; p_stInData->cTime[1] = lcTemp[1];
	p_stInData->cTime[2] = lcTemp[3]; p_stInData->cTime[3] = lcTemp[4];
	p_stInData->cTime[4] = lcTemp[6]; p_stInData->cTime[5] = lcTemp[7];
	p_stInData->cTime[6] = '\0';

	p_stInData->iTime = atoi(p_stInData->cTime) ;
	p_stInData->iTime = p_stInData->iTime * 1000 + p_stSHQuoteIn->UpdateMillisec;
	if (p_stInData->iTime < 30000000) p_stInData->iTime = p_stInData->iTime + 240000000;

	strncpy(p_stInData->cTicker, p_stSHQuoteIn->InstrumentID, 7);
//	if (fabs(p_stInData->dBP1 - p_stSHQuoteIn->BidPrice1) > 0.001 ||
//		fabs(p_stInData->dSP1 - p_stSHQuoteIn->AskPrice1) > 0.001)
//		p_stInData->iData_Flag  = 2;

	if (p_stSHQuoteIn->data_flag == 1)
	{
		p_stInData->dBP1 = p_stSHQuoteIn->BidPrice1; p_stInData->iBV1 = p_stSHQuoteIn->BidVolume1;
		p_stInData->dSP1 = p_stSHQuoteIn->AskPrice1; p_stInData->iSV1 = p_stSHQuoteIn->AskVolume1 ;
		p_stInData->dBP2 = 0; p_stInData->iBV2 = 0;
		p_stInData->dSP2 = 0; p_stInData->iSV2 = 0;
		p_stInData->dBP3 = 0; p_stInData->iBV3 = 0;
		p_stInData->dSP3 = 0; p_stInData->iSV3 = 0;
		p_stInData->dBP4 = 0; p_stInData->iBV4 = 0;
		p_stInData->dSP4 = 0; p_stInData->iSV4 = 0;
		p_stInData->dBP5 = 0; p_stInData->iBV5 = 0;
		p_stInData->dSP5 = 0; p_stInData->iSV5 = 0;

		memcpy(p_stInData->dBP, p_stSHQuoteIn->buy_price, sizeof(double)*MY_SHFE_QUOTE_PRICE_POS_COUNT);
        memcpy(p_stInData->dSP, p_stSHQuoteIn->sell_price, sizeof(double)*MY_SHFE_QUOTE_PRICE_POS_COUNT);
        memcpy(p_stInData->iBV, p_stSHQuoteIn->buy_volume, sizeof(int)*MY_SHFE_QUOTE_PRICE_POS_COUNT);
        memcpy(p_stInData->iSV, p_stSHQuoteIn->sell_volume, sizeof(int)*MY_SHFE_QUOTE_PRICE_POS_COUNT);
	}
	else if (p_stSHQuoteIn->data_flag <= 3)
	{
		p_stInData->dBP1 = p_stSHQuoteIn->buy_price[0]; p_stInData->iBV1 = p_stSHQuoteIn->buy_volume[0];
		p_stInData->dSP1 = p_stSHQuoteIn->sell_price[0]; p_stInData->iSV1 = p_stSHQuoteIn->sell_volume[0];
		p_stInData->dBP2 = p_stSHQuoteIn->buy_price[1]; p_stInData->iBV2 = p_stSHQuoteIn->buy_volume[1];
		p_stInData->dSP2 = p_stSHQuoteIn->sell_price[1]; p_stInData->iSV2 = p_stSHQuoteIn->sell_volume[1];
		p_stInData->dBP3 = p_stSHQuoteIn->buy_price[2]; p_stInData->iBV3 = p_stSHQuoteIn->buy_volume[2];
		p_stInData->dSP3 = p_stSHQuoteIn->sell_price[2]; p_stInData->iSV3 = p_stSHQuoteIn->sell_volume[2];
		p_stInData->dBP4 = p_stSHQuoteIn->buy_price[3]; p_stInData->iBV4 = p_stSHQuoteIn->buy_volume[3];
		p_stInData->dSP4 = p_stSHQuoteIn->sell_price[3]; p_stInData->iSV4 = p_stSHQuoteIn->sell_volume[3];
		p_stInData->dBP5 = p_stSHQuoteIn->buy_price[4]; p_stInData->iBV5 = p_stSHQuoteIn->buy_volume[4];
		p_stInData->dSP5 = p_stSHQuoteIn->sell_price[4]; p_stInData->iSV5 = p_stSHQuoteIn->sell_volume[4];

		memcpy(p_stInData->dBP, p_stSHQuoteIn->buy_price, sizeof(double)*MY_SHFE_QUOTE_PRICE_POS_COUNT);
        memcpy(p_stInData->dSP, p_stSHQuoteIn->sell_price, sizeof(double)*MY_SHFE_QUOTE_PRICE_POS_COUNT);
        memcpy(p_stInData->iBV, p_stSHQuoteIn->buy_volume, sizeof(int)*MY_SHFE_QUOTE_PRICE_POS_COUNT);
        memcpy(p_stInData->iSV, p_stSHQuoteIn->sell_volume, sizeof(int)*MY_SHFE_QUOTE_PRICE_POS_COUNT);
	}
	else if (p_stSHQuoteIn->data_flag <= 6) //data_flag: 5 or 6
	{
		p_stInData->dBP1 = p_stSHQuoteIn->buy_price[29]; p_stInData->iBV1 = p_stSHQuoteIn->buy_volume[29];
		p_stInData->dSP1 = p_stSHQuoteIn->sell_price[29]; p_stInData->iSV1 = p_stSHQuoteIn->sell_volume[29];
		p_stInData->dBP2 = p_stSHQuoteIn->buy_price[28]; p_stInData->iBV2 = p_stSHQuoteIn->buy_volume[28];
		p_stInData->dSP2 = p_stSHQuoteIn->sell_price[28]; p_stInData->iSV2 = p_stSHQuoteIn->sell_volume[28];
		p_stInData->dBP3 = p_stSHQuoteIn->buy_price[27]; p_stInData->iBV3 = p_stSHQuoteIn->buy_volume[27];
		p_stInData->dSP3 = p_stSHQuoteIn->sell_price[27]; p_stInData->iSV3 = p_stSHQuoteIn->sell_volume[27];
		p_stInData->dBP4 = p_stSHQuoteIn->buy_price[26]; p_stInData->iBV4 = p_stSHQuoteIn->buy_volume[26];
		p_stInData->dSP4 = p_stSHQuoteIn->sell_price[26]; p_stInData->iSV4 = p_stSHQuoteIn->sell_volume[26];
		p_stInData->dBP5 = p_stSHQuoteIn->buy_price[25]; p_stInData->iBV5 = p_stSHQuoteIn->buy_volume[25];
		p_stInData->dSP5 = p_stSHQuoteIn->sell_price[25]; p_stInData->iSV5 = p_stSHQuoteIn->sell_volume[25];

		for(i=0; i<30; i++)
        {
            p_stInData->dBP[i] = p_stSHQuoteIn->buy_price[29-i];
            p_stInData->dSP[i] = p_stSHQuoteIn->sell_price[29-i];
            p_stInData->iBV[i] = p_stSHQuoteIn->buy_volume[29-i];
            p_stInData->iSV[i] = p_stSHQuoteIn->sell_volume[29-i];
        }
	}

	p_stInData->cExchCode = cSHFE;
	p_stInData->cSecType = cFUT;
	p_stInData->dV = 0;
	p_stInData->dVal = 0; p_stInData->dPMean = 0;

	memset(p_stInData->iImpBV, 0, sizeof(int)*MY_SHFE_QUOTE_PRICE_POS_COUNT);
	memset(p_stInData->iImpSV, 0, sizeof(int)*MY_SHFE_QUOTE_PRICE_POS_COUNT);

	p_stInData->iTotalBuyVol = p_stSHQuoteIn->buy_total_volume;
	p_stInData->iTotalSelVol = p_stSHQuoteIn->sell_total_volume;
	p_stInData->dWeightedBuyOrderP = p_stSHQuoteIn->buy_weighted_avg_price;
	p_stInData->dWeightedSelOrderP = p_stSHQuoteIn->sell_weighted_avg_price;
	p_stInData->iData_Flag = p_stSHQuoteIn->data_flag;
	if(p_stInData->iData_Flag >= 5) p_stInData->iData_Flag = p_stInData->iData_Flag - 3;

	return 0;
}

int s_read_sh_fut_struct_backup(struct shfe_my_quote *p_stSHQuoteIn, struct in_data *p_stInData)
{
	char lcTemp[20];


	p_stInData->dP = p_stSHQuoteIn->LastPrice;
	p_stInData->dYestP = p_stSHQuoteIn->PreSettlementPrice;
	p_stInData->dPYestCl = p_stSHQuoteIn->PreClosePrice;
	p_stInData->dYestOI = p_stSHQuoteIn->PreOpenInterest;
	p_stInData->dPOpen = p_stSHQuoteIn->OpenPrice;
	p_stInData->dPHi = p_stSHQuoteIn->HighestPrice;
	p_stInData->dPLo = p_stSHQuoteIn->LowestPrice;
	p_stInData->dTotV = p_stSHQuoteIn->Volume;
	p_stInData->dTotVal = p_stSHQuoteIn->Turnover;
	p_stInData->dOI = p_stSHQuoteIn->OpenInterest;
	p_stInData->dPClose = p_stSHQuoteIn->ClosePrice;
	p_stInData->dPSettle = p_stSHQuoteIn->SettlementPrice;
	p_stInData->dPLmtUp = p_stSHQuoteIn->UpperLimitPrice;
	p_stInData->dPLmtDn = p_stSHQuoteIn->LowerLimitPrice;

	strncpy(lcTemp, p_stSHQuoteIn->UpdateTime, 9);
	p_stInData->cTime[0] = lcTemp[0]; p_stInData->cTime[1] = lcTemp[1];
	p_stInData->cTime[2] = lcTemp[3]; p_stInData->cTime[3] = lcTemp[4];
	p_stInData->cTime[4] = lcTemp[6]; p_stInData->cTime[5] = lcTemp[7];
	p_stInData->cTime[6] = '\0';

	p_stInData->iTime = atoi(p_stInData->cTime) ;
	p_stInData->iTime = p_stInData->iTime * 1000 + p_stSHQuoteIn->UpdateMillisec;
	if (p_stInData->iTime < 30000000) p_stInData->iTime = p_stInData->iTime + 240000000;

	strncpy(p_stInData->cTicker, p_stSHQuoteIn->InstrumentID, 7);
//	if (fabs(p_stInData->dBP1 - p_stSHQuoteIn->BidPrice1) > 0.001 ||
//		fabs(p_stInData->dSP1 - p_stSHQuoteIn->AskPrice1) > 0.001)
//		p_stInData->iData_Flag  = 2;

	if (p_stSHQuoteIn->data_flag == 1)
	{
		p_stInData->dBP1 = p_stSHQuoteIn->BidPrice1; p_stInData->iBV1 = p_stSHQuoteIn->BidVolume1;
		p_stInData->dSP1 = p_stSHQuoteIn->AskPrice1; p_stInData->iSV1 = p_stSHQuoteIn->AskVolume1 ;
		p_stInData->dBP2 = 0; p_stInData->iBV2 = 0;
		p_stInData->dSP2 = 0; p_stInData->iSV2 = 0;
		p_stInData->dBP3 = 0; p_stInData->iBV3 = 0;
		p_stInData->dSP3 = 0; p_stInData->iSV3 = 0;
		p_stInData->dBP4 = 0; p_stInData->iBV4 = 0;
		p_stInData->dSP4 = 0; p_stInData->iSV4 = 0;
		p_stInData->dBP5 = 0; p_stInData->iBV5 = 0;
		p_stInData->dSP5 = 0; p_stInData->iSV5 = 0;
	}
	else
	{
		p_stInData->dBP1 = p_stSHQuoteIn->buy_price[0]; p_stInData->iBV1 = p_stSHQuoteIn->buy_volume[0];
		p_stInData->dSP1 = p_stSHQuoteIn->sell_price[0]; p_stInData->iSV1 = p_stSHQuoteIn->sell_volume[0];
		p_stInData->dBP2 = p_stSHQuoteIn->buy_price[1]; p_stInData->iBV2 = p_stSHQuoteIn->buy_volume[1];
		p_stInData->dSP2 = p_stSHQuoteIn->sell_price[1]; p_stInData->iSV2 = p_stSHQuoteIn->sell_volume[1];
		p_stInData->dBP3 = p_stSHQuoteIn->buy_price[2]; p_stInData->iBV3 = p_stSHQuoteIn->buy_volume[2];
		p_stInData->dSP3 = p_stSHQuoteIn->sell_price[2]; p_stInData->iSV3 = p_stSHQuoteIn->sell_volume[2];
		p_stInData->dBP4 = p_stSHQuoteIn->buy_price[3]; p_stInData->iBV4 = p_stSHQuoteIn->buy_volume[3];
		p_stInData->dSP4 = p_stSHQuoteIn->sell_price[3]; p_stInData->iSV4 = p_stSHQuoteIn->sell_volume[3];
		p_stInData->dBP5 = p_stSHQuoteIn->buy_price[4]; p_stInData->iBV5 = p_stSHQuoteIn->buy_volume[4];
		p_stInData->dSP5 = p_stSHQuoteIn->sell_price[4]; p_stInData->iSV5 = p_stSHQuoteIn->sell_volume[4];
	}

	p_stInData->cExchCode = cSHFE;
	p_stInData->cSecType = cFUT;
	p_stInData->dV = 0;
	p_stInData->dVal = 0; p_stInData->dPMean = 0;

	memcpy(p_stInData->dBP, p_stSHQuoteIn->buy_price, sizeof(double)*MY_SHFE_QUOTE_PRICE_POS_COUNT);
	memcpy(p_stInData->dSP, p_stSHQuoteIn->sell_price, sizeof(double)*MY_SHFE_QUOTE_PRICE_POS_COUNT);
	memcpy(p_stInData->iBV, p_stSHQuoteIn->buy_volume, sizeof(int)*MY_SHFE_QUOTE_PRICE_POS_COUNT);
	memcpy(p_stInData->iSV, p_stSHQuoteIn->sell_volume, sizeof(int)*MY_SHFE_QUOTE_PRICE_POS_COUNT);

	memset(p_stInData->iImpBV, 0, sizeof(int)*MY_SHFE_QUOTE_PRICE_POS_COUNT);
	memset(p_stInData->iImpSV, 0, sizeof(int)*MY_SHFE_QUOTE_PRICE_POS_COUNT);

	p_stInData->iTotalBuyVol = p_stSHQuoteIn->buy_total_volume;
	p_stInData->iTotalSelVol = p_stSHQuoteIn->sell_total_volume;
	p_stInData->dWeightedBuyOrderP = p_stSHQuoteIn->buy_weighted_avg_price;
	p_stInData->dWeightedSelOrderP = p_stSHQuoteIn->sell_weighted_avg_price;
	p_stInData->iData_Flag = p_stSHQuoteIn->data_flag;

	return 0;
}

int s_file_sh_flag_1_data(struct in_data *p_stInData, struct in_data *p_preInData)
{
    int i,i1,i2;
    int ilen;

    if(p_preInData->iData_Flag != 3 && p_stInData->iData_Flag != 1) return 1;
    if(p_preInData->iData_Flag == 3 &&
       fabs(p_preInData->dSP1 - p_preInData->dBP1) > 6*gstGlobalPara[giTradeInfo].stSec.rTickSize) return 1;

    ilen = MY_SHFE_QUOTE_PRICE_POS_COUNT;
    memset(p_stInData->dBP, 0, sizeof(double)*ilen);
    memset(p_stInData->dSP, 0, sizeof(double)*ilen);
    memset(p_stInData->iBV, 0, sizeof(int)*ilen);
    memset(p_stInData->iSV, 0, sizeof(int)*ilen);
    p_stInData->dBP[0] = p_stInData->dBP1;
    p_stInData->dSP[0] = p_stInData->dSP1;
    p_stInData->iBV[0] = p_stInData->iBV1;
    p_stInData->iSV[0] = p_stInData->iSV1;

    i1 = 0;
    for(i=0; i<30; i++)
    {
        if(p_preInData->dBP[i]< p_stInData->dBP1)
        {
            i1 = i; break;
        }
    }
    ilen = MIN(MY_SHFE_QUOTE_PRICE_POS_COUNT-1,MY_SHFE_QUOTE_PRICE_POS_COUNT-i1);
    memcpy(&(p_stInData->dBP[1]), &(p_preInData->dBP[i1]), sizeof(double)*ilen);
    memcpy(&(p_stInData->iBV[1]), &(p_preInData->iBV[i1]), sizeof(int)*ilen);

    i2 = 0;
    for(i=0; i<30; i++)
    {
        if(p_preInData->dSP[i]> p_stInData->dSP1)
        {
            i2 = i; break;
        }
    }
    ilen = MIN(MY_SHFE_QUOTE_PRICE_POS_COUNT-1,MY_SHFE_QUOTE_PRICE_POS_COUNT-i2);
    memcpy(&(p_stInData->dSP[1]), &(p_preInData->dSP[i2]), sizeof(double)*ilen);
    memcpy(&(p_stInData->iSV[1]), &(p_preInData->iSV[i2]), sizeof(int)*ilen);

    p_stInData->dBP2 = p_stInData->dBP[1]; p_stInData->iBV2 = p_stInData->iBV[1];
    p_stInData->dSP2 = p_stInData->dSP[1]; p_stInData->iSV2 = p_stInData->iSV[1];
    p_stInData->dBP3 = p_stInData->dBP[2]; p_stInData->iBV3 = p_stInData->iBV[2];
    p_stInData->dSP3 = p_stInData->dSP[2]; p_stInData->iSV3 = p_stInData->iSV[2];
    p_stInData->dBP4 = p_stInData->dBP[3]; p_stInData->iBV4 = p_stInData->iBV[3];
    p_stInData->dSP4 = p_stInData->dSP[3]; p_stInData->iSV4 = p_stInData->iSV[3];
    p_stInData->dBP5 = p_stInData->dBP[4]; p_stInData->iBV5 = p_stInData->iBV[4];
    p_stInData->dSP5 = p_stInData->dSP[4]; p_stInData->iSV5 = p_stInData->iSV[4];

    return 0;
}

int s_set_exch_parameters(char lcExch, char *lcSec, struct all_parameter *p_stOutData)
{

	//int liSqIntervalSec;

	//liSqIntervalSec = 5;

	if( lcExch == cSHFE)
    {
		if (giNightMarket == 0)
		{
			p_stOutData->stExch.iVolDivider = 2;
			p_stOutData->stExch.iPInLevel = 30;
			p_stOutData->stExch.iPreMktAucTime = 85900;
			p_stOutData->stExch.iMktStartTime = 90000;
			p_stOutData->stExch.iMktEndTime = 150000;
			p_stOutData->stSec.iSecEndTime = 145500;
			p_stOutData->stSec.iSecMktOpenTime = p_stOutData->stExch.iMktStartTime;

			p_stOutData->stSec.iSecTradeStartTime = p_stOutData->stExch.iMktStartTime + 0;
            p_stOutData->stSec.iSecTradeEndTime = 145300;
            p_stOutData->stSec.iSecTradeStartTime02 = -1;
            p_stOutData->stSec.iSecTradeEndTime02 = -1;

			p_stOutData->stSec.iSecFinalSqTime = 145500;
			p_stOutData->stSec.iSecTraceTime = 145900;
		}
		else if (giNightMarket == 1)
		{
			p_stOutData->stExch.iVolDivider = 2;
			p_stOutData->stExch.iPInLevel = 30;
			p_stOutData->stExch.iPreMktAucTime = 205900;
			p_stOutData->stExch.iMktStartTime = 210000;
			p_stOutData->stExch.iMktEndTime = 240000;
			p_stOutData->stSec.iSecEndTime = 235500;
			p_stOutData->stSec.iSecMktOpenTime = p_stOutData->stExch.iMktStartTime;

			p_stOutData->stSec.iSecTradeStartTime = p_stOutData->stExch.iMktStartTime + 0;
			p_stOutData->stSec.iSecTradeEndTime = 235400;

			p_stOutData->stSec.iSecTradeStartTime02 = -1;
            p_stOutData->stSec.iSecTradeEndTime02 = -1;

			p_stOutData->stSec.iSecFinalSqTime = 235600;
			p_stOutData->stSec.iSecTraceTime = 235900;
			if(strcmp(gcSecName, "ru") == 0 ||strcmp(gcSecName, "RU") == 0 ||
               strcmp(gcSecName, "hc") == 0 ||strcmp(gcSecName, "HC") == 0 ||
               strcmp(gcSecName, "bu") == 0 ||strcmp(gcSecName, "BU") == 0 ||
               strcmp(gcSecName, "rb") == 0 ||strcmp(gcSecName, "RB") == 0)
			{
				p_stOutData->stExch.iMktEndTime = 230000;
				p_stOutData->stSec.iSecEndTime = 225500;
				p_stOutData->stSec.iSecMktOpenTime = p_stOutData->stExch.iMktStartTime;
				p_stOutData->stSec.iSecTradeStartTime = p_stOutData->stExch.iMktStartTime + 0;
				p_stOutData->stSec.iSecTradeEndTime = 224500;

				p_stOutData->stSec.iSecTradeStartTime02 = -1;
                p_stOutData->stSec.iSecTradeEndTime02 = -1;

				p_stOutData->stSec.iSecFinalSqTime = 225600;
				p_stOutData->stSec.iSecTraceTime = 225900;
			}
		}


		p_stOutData->stSec.rExchSameDayDisc = 0;
		p_stOutData->stSec.rPctLmt = 0.06;
		p_stOutData->stSec.rBrokerPct = 0;
		p_stOutData->stSec.iCostSide = 2;
		p_stOutData->stSec.rOVCompRatio = 0;
		p_stOutData->stSec.rTVCompRatio = 0.5;
		p_stOutData->stSec.iMaxSizePerOrder = 500;
		p_stOutData->stSec.iRatioLevel = 10;
		//p_stOutData->stSec.iBaseSize = 0;
		//p_stOutData->stSec.cExchCode = cSHFE;
		p_stOutData->stSec.iWithCancel = iNO;
    }
	else if( lcExch == cDCE)
    {
		if (giNightMarket == 0)
		{
			p_stOutData->stExch.iVolDivider = 2;
			p_stOutData->stExch.iPInLevel = 5;
			p_stOutData->stExch.iPreMktAucTime = 85900;
			p_stOutData->stExch.iMktStartTime = 90000;
			p_stOutData->stExch.iMktEndTime = 150000;
			p_stOutData->stSec.iSecEndTime = 145500;
			p_stOutData->stSec.iSecMktOpenTime = p_stOutData->stExch.iMktStartTime;
			p_stOutData->stSec.iSecFinalSqTime = 145600;
			p_stOutData->stSec.iSecTraceTime = 145900;

			p_stOutData->stSec.iSecTradeStartTime = p_stOutData->stExch.iMktStartTime + 50;
            p_stOutData->stSec.iSecTradeEndTime = 145300;
            p_stOutData->stSec.iSecTradeStartTime02 = -1;
            p_stOutData->stSec.iSecTradeEndTime02 = -1;
		}
		else if (giNightMarket == 1)
		{
			p_stOutData->stExch.iVolDivider = 2;
			p_stOutData->stExch.iPInLevel = 5;
			p_stOutData->stExch.iPreMktAucTime = 205900;
			p_stOutData->stExch.iMktStartTime = 210000;
			p_stOutData->stExch.iMktEndTime = 233000;
			p_stOutData->stSec.iSecEndTime = 232500;
			p_stOutData->stSec.iSecMktOpenTime = p_stOutData->stExch.iMktStartTime;

			p_stOutData->stSec.iSecTradeStartTime = p_stOutData->stExch.iMktStartTime + 50;
			p_stOutData->stSec.iSecTradeEndTime = 232400;

			p_stOutData->stSec.iSecTradeStartTime02 = -1;
            p_stOutData->stSec.iSecTradeEndTime02 = -1;
			p_stOutData->stSec.iSecFinalSqTime = 232600;
			p_stOutData->stSec.iSecTraceTime = 232900;
		}

		p_stOutData->stSec.rExchSameDayDisc = 0.0;
		p_stOutData->stSec.rPctLmt = 0.04;
		p_stOutData->stSec.rBrokerPct = 0.10;
		p_stOutData->stSec.iCostSide = 2;
		p_stOutData->stSec.rOVCompRatio = 0.0;
		p_stOutData->stSec.rTVCompRatio = 0.5;
		p_stOutData->stSec.iMaxSizePerOrder = 1000;
		p_stOutData->stSec.iRatioLevel = 10;
		//p_stOutData->stSec.iBaseSize = 0;
		//p_stOutData->stSec.cExchCode = cDCE;
		p_stOutData->stSec.iWithCancel = iNO;
    }
    else if( lcExch == cCZCE)
	{

		if (giNightMarket == 0)
		{
			p_stOutData->stExch.iVolDivider = 2;
			p_stOutData->stExch.iPInLevel = 5;
			p_stOutData->stExch.iPreMktAucTime = 85900;
			p_stOutData->stExch.iMktStartTime = 90000;
			p_stOutData->stExch.iMktEndTime = 150000;
			p_stOutData->stSec.iSecEndTime = 145500;
			p_stOutData->stSec.iSecMktOpenTime = p_stOutData->stExch.iMktStartTime;
			p_stOutData->stSec.iSecFinalSqTime = 145600;
			p_stOutData->stSec.iSecTraceTime = 145900;

			p_stOutData->stSec.iSecTradeStartTime = p_stOutData->stExch.iMktStartTime + 0;
            p_stOutData->stSec.iSecTradeEndTime = 145300;
            p_stOutData->stSec.iSecTradeStartTime02 = -1;
            p_stOutData->stSec.iSecTradeEndTime02 = -1;
		}
		else if (giNightMarket == 1)
		{
			p_stOutData->stExch.iVolDivider = 2;
			p_stOutData->stExch.iPInLevel = 5;
			p_stOutData->stExch.iPreMktAucTime = 205900;
			p_stOutData->stExch.iMktStartTime = 210000;
			p_stOutData->stExch.iMktEndTime = 233000;
			p_stOutData->stSec.iSecEndTime = 232500;
			p_stOutData->stSec.iSecMktOpenTime = p_stOutData->stExch.iMktStartTime;
			p_stOutData->stSec.iSecFinalSqTime = 232600;
			p_stOutData->stSec.iSecTraceTime = 232900;

			p_stOutData->stSec.iSecTradeStartTime = p_stOutData->stExch.iMktStartTime + 0;
			p_stOutData->stSec.iSecTradeEndTime = 232400;

			p_stOutData->stSec.iSecTradeStartTime02 = -1;
            p_stOutData->stSec.iSecTradeEndTime02 = -1;
		}

		p_stOutData->stSec.rExchSameDayDisc = 0.0;
		p_stOutData->stSec.rPctLmt = 0.040;
		p_stOutData->stSec.rBrokerPct = 0.100;
		p_stOutData->stSec.iCostSide = 2;
		p_stOutData->stSec.rOVCompRatio = 0.0;
		p_stOutData->stSec.rTVCompRatio = 0.5;
		p_stOutData->stSec.iMaxSizePerOrder = 1000;
		p_stOutData->stSec.iRatioLevel = 10;
		//p_stOutData->stSec.iBaseSize = 0;
		//p_stOutData->stSec.cExchCode = cCZCE;
		p_stOutData->stSec.iWithCancel = iNO;
	}
	else if(lcExch == cCFFE)
    {
 		p_stOutData->stExch.iVolDivider = 1;
		p_stOutData->stExch.iPInLevel = 5;
		p_stOutData->stExch.iPreMktAucTime = 91400;
		p_stOutData->stExch.iMktStartTime = 91500;
		p_stOutData->stExch.iMktEndTime = 151500;
		p_stOutData->stSec.iSecEndTime = 151000;
		p_stOutData->stSec.iSecMktOpenTime = p_stOutData->stExch.iMktStartTime;
		p_stOutData->stSec.iSecFinalSqTime = 151300;
		p_stOutData->stSec.iSecTraceTime = 151400;

		p_stOutData->stSec.iSecTradeStartTime = p_stOutData->stExch.iMktStartTime + 0;
        p_stOutData->stSec.iSecTradeEndTime = 145300;
        p_stOutData->stSec.iSecTradeStartTime02 = -1;
        p_stOutData->stSec.iSecTradeEndTime02 = -1;

		p_stOutData->stSec.rExchSameDayDisc = 0.0;
		p_stOutData->stSec.rPctLmt = 0.1;
		p_stOutData->stSec.rBrokerPct = 0.100;
		p_stOutData->stSec.iCostSide = 2;
		p_stOutData->stSec.rOVCompRatio = 0.0;
		p_stOutData->stSec.rTVCompRatio = 0.5;
		p_stOutData->stSec.iMaxSizePerOrder = 100;
		p_stOutData->stSec.iRatioLevel = 10;
		//p_stOutData->stSec.iBaseSize = 0;
		//p_stOutData->stSec.cExchCode = cCFFE;
		p_stOutData->stSec.iWithCancel = iYES;
		//p_stOutData->stSec.rTVCompRatio = 0.20;
    }

	return 0;
}

int s_set_convertion_parameters(char *lcSecName, struct sec_parameter *p_stOutData)
{
	char cSeclow[10];
	int i, imax;

	imax = strlen(lcSecName);
	for(i=0; i< (int)strlen(lcSecName); i++)
		cSeclow[i] = tolower(lcSecName[i]);
	cSeclow[imax] = '\0';

	if(strncmp(cSeclow, "if", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0; p_stOutData->rTCostByVal = 0.0000250;
		p_stOutData->rRssTCostByVol = 0.0; p_stOutData->rRssTCostByVal = 0.000026250;
		p_stOutData->iUnitPerContract = 300; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 0.20; p_stOutData->iPMultiplier = 10; p_stOutData->iTickSizeMulti = 2; p_stOutData->iDecimalPt = 1;
		p_stOutData->iRatioLevel = 15;
	}
	else if(strncmp(cSeclow, "ic", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0; p_stOutData->rTCostByVal = 0.0000250;
		p_stOutData->rRssTCostByVol = 0.0; p_stOutData->rRssTCostByVal = 0.000026250;
		p_stOutData->iUnitPerContract = 200; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 0.20; p_stOutData->iPMultiplier = 10; p_stOutData->iTickSizeMulti = 2; p_stOutData->iDecimalPt = 1;
		p_stOutData->iRatioLevel = 15 ;
	}
	else if(strncmp(cSeclow, "ih", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0; p_stOutData->rTCostByVal = 0.0000250;
		p_stOutData->rRssTCostByVol = 0.0; p_stOutData->rRssTCostByVal = 0.000026250;
		p_stOutData->iUnitPerContract = 300; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 0.20; p_stOutData->iPMultiplier = 10; p_stOutData->iTickSizeMulti = 2; p_stOutData->iDecimalPt = 1;
		p_stOutData->iRatioLevel = 15;
	}
	else if(strncmp(cSeclow, "tf", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 0.000424; p_stOutData->rRssTCostByVal = 0.0;
		p_stOutData->iUnitPerContract = 10000; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 0.005; p_stOutData->iPMultiplier = 10; p_stOutData->iTickSizeMulti = 2; p_stOutData->iDecimalPt = 1;
		p_stOutData->iRatioLevel = 15;
	}
	else if(strncmp(cSeclow, "t", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 0.000212; p_stOutData->rRssTCostByVal = 0.0;
		p_stOutData->iUnitPerContract = 10000; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 0.005; p_stOutData->iPMultiplier = 10; p_stOutData->iTickSizeMulti = 2; p_stOutData->iDecimalPt = 1;
		p_stOutData->iRatioLevel = 15;
	}
	else if(strncmp(cSeclow, "ag", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0; p_stOutData->rTCostByVal = 0.000050;
		p_stOutData->rRssTCostByVol = 0.0; p_stOutData->rRssTCostByVal = 0.0000550;
		p_stOutData->iUnitPerContract = 15; p_stOutData->iWrongVol = 50000; p_stOutData->iCostSide = 1;
		p_stOutData->rTickSize = 1.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 20;
	}
	else if(strncmp(cSeclow, "al", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.01 ; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 0.3300; p_stOutData->rRssTCostByVal = 0.0;
		p_stOutData->iUnitPerContract = 5; p_stOutData->iWrongVol = 50000; p_stOutData->iCostSide = 1;
		p_stOutData->rTickSize = 5.0; p_stOutData->iPMultiplier = 100; p_stOutData->iTickSizeMulti = 5; p_stOutData->iDecimalPt = 2;
		p_stOutData->iRatioLevel = 35;
	}
	else if(strncmp(cSeclow, "au", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.01 ; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 0.00550; p_stOutData->rRssTCostByVal = 0.0;
		p_stOutData->iUnitPerContract = 1000; p_stOutData->iWrongVol = 50000; p_stOutData->iCostSide = 1;
		p_stOutData->rTickSize = 0.050; p_stOutData->iPMultiplier = 100; p_stOutData->iTickSizeMulti = 5; p_stOutData->iDecimalPt = 2;
		p_stOutData->iRatioLevel = 35;
	}
	else if(strncmp(cSeclow, "bu", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0 ; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 0.0; p_stOutData->rRssTCostByVal = 0.000165;
		p_stOutData->iUnitPerContract = 10; p_stOutData->iWrongVol = 50000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 2.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 35;
	}
	else if(strncmp(cSeclow, "cu", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0; p_stOutData->rTCostByVal = 0.000025 ;
		p_stOutData->rRssTCostByVol = 0.0; p_stOutData->rRssTCostByVal = 0.0000275;
		p_stOutData->iUnitPerContract = 5; p_stOutData->iWrongVol = 50000; p_stOutData->iCostSide = 1;
		p_stOutData->rTickSize = 10.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 10; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 30;
	}
	else if(strncmp(cSeclow, "fu", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0; p_stOutData->rTCostByVal = 0.0000450;
		p_stOutData->rRssTCostByVol = 0.0; p_stOutData->rRssTCostByVal = 0.00002200;
		p_stOutData->iUnitPerContract = 50; p_stOutData->iWrongVol = 50000; p_stOutData->iCostSide = 1;
		p_stOutData->rTickSize = 1.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 30;
	}
	else if(strncmp(cSeclow, "hc", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0; p_stOutData->rTCostByVal = 0.0000450;
		p_stOutData->rRssTCostByVol = 0.0; p_stOutData->rRssTCostByVal = 0.00033;
		p_stOutData->iUnitPerContract = 10; p_stOutData->iWrongVol = 50000; p_stOutData->iCostSide = 1;
		p_stOutData->rTickSize = 1.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 30;
	}
	else if(strncmp(cSeclow, "ni", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0; p_stOutData->rTCostByVal = 0.000000;
		p_stOutData->rRssTCostByVol = 19.8; p_stOutData->rRssTCostByVal = 0.0  ;
		p_stOutData->iUnitPerContract = 1; p_stOutData->iWrongVol = 50000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 10.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 5; p_stOutData->iDecimalPt = 0 ;
	}
	else if(strncmp(cSeclow, "pb", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0; p_stOutData->rTCostByVal = 0.0000450;
		p_stOutData->rRssTCostByVol = 0.0; p_stOutData->rRssTCostByVal = 0.000132;
		p_stOutData->iUnitPerContract = 5; p_stOutData->iWrongVol = 50000; p_stOutData->iCostSide = 1;
		p_stOutData->rTickSize = 5.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 30;
	}
	else if(strncmp(cSeclow, "rb", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0; p_stOutData->rTCostByVal = 0.0000450;
		p_stOutData->rRssTCostByVol = 0.0; p_stOutData->rRssTCostByVal = 0.00011;// 0.00033;
		p_stOutData->iUnitPerContract = 10; p_stOutData->iWrongVol = 50000; p_stOutData->iCostSide = 1;
		p_stOutData->rTickSize = 1.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 30;
	}
	else if(strncmp(cSeclow, "ru", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0; p_stOutData->rTCostByVal = 0.0000450;
		p_stOutData->rRssTCostByVol = 0.0; p_stOutData->rRssTCostByVal = 0.0000495;
		p_stOutData->iUnitPerContract = 10; p_stOutData->iWrongVol = 50000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 5.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 5; p_stOutData->iDecimalPt = 0;
	}
	else if(strncmp(cSeclow, "zn", 2) == 0)
	{
		p_stOutData->rTCostByVol = 3.0/5; p_stOutData->rTCostByVal = 0.000000;
		p_stOutData->rRssTCostByVol = 1.98; p_stOutData->rRssTCostByVal = 0.00;
		p_stOutData->iUnitPerContract = 5; p_stOutData->iWrongVol = 50000; p_stOutData->iCostSide = 1;
		p_stOutData->rTickSize = 5.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 5; p_stOutData->iDecimalPt = 0;
	}
	else if(strncmp(cSeclow, "a", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.20; p_stOutData->rTCostByVal = 0.0; p_stOutData->rExchSameDayDisc = 0.00;
		p_stOutData->rRssTCostByVol = 0.220; p_stOutData->rRssTCostByVal = 0.0    ;
		p_stOutData->iUnitPerContract = 10; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 1.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 50;
	}
	else if(strncmp(cSeclow, "bb", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0; p_stOutData->rTCostByVal = 0.00010; p_stOutData->rExchSameDayDisc = 0.50;
		p_stOutData->rRssTCostByVol = 0.0; p_stOutData->rRssTCostByVal = 0.00006 ;
		p_stOutData->iUnitPerContract = 500; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 0.050; p_stOutData->iPMultiplier = 100; p_stOutData->iTickSizeMulti = 5; p_stOutData->iDecimalPt = 2;
		p_stOutData->iRatioLevel = 10; p_stOutData->iWithCancel = iYES;
	}
	else if(strncmp(cSeclow, "c", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.120; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 0.0660; p_stOutData->rRssTCostByVal = 0.0  ;
		p_stOutData->iUnitPerContract = 10; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 1; p_stOutData->iMaxSizePerOrder = 2000;
		p_stOutData->rTickSize = 1.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 0;
	}
	else if(strncmp(cSeclow, "cs", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.120; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 0.165; p_stOutData->rRssTCostByVal = 0.0  ;
		p_stOutData->iUnitPerContract = 10; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2; p_stOutData->iMaxSizePerOrder = 2000;
		p_stOutData->rTickSize = 1.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 0;
	}
	else if(strncmp(cSeclow, "fb", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0; p_stOutData->rTCostByVal = 0.00010; p_stOutData->rExchSameDayDisc = 0.50;
		p_stOutData->rRssTCostByVol = 0.0; p_stOutData->rRssTCostByVal = 0.00006;
		p_stOutData->iUnitPerContract = 500; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 1;
		p_stOutData->rTickSize = 0.050; p_stOutData->iPMultiplier = 100; p_stOutData->iTickSizeMulti = 5; p_stOutData->iDecimalPt = 2;
		p_stOutData->iRatioLevel = 10; p_stOutData->iWithCancel = iYES;
	}
	else if(strncmp(cSeclow, "i", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0; p_stOutData->rTCostByVal = 0.000080; p_stOutData->rExchSameDayDisc = 0.50;
		p_stOutData->rRssTCostByVol = 0.0; p_stOutData->rRssTCostByVal = 0.000066 ;
		p_stOutData->iUnitPerContract = 100; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 0.50; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 1;
		p_stOutData->iRatioLevel = 10;
	}
	else if(strncmp(cSeclow, "j", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0; p_stOutData->rTCostByVal = 0.000080; p_stOutData->rExchSameDayDisc = 0.50;
		p_stOutData->rRssTCostByVol = 0.0; p_stOutData->rRssTCostByVal = 0.00033;
		p_stOutData->iUnitPerContract = 100; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2; p_stOutData->iMaxSizePerOrder = 500;
		p_stOutData->rTickSize = 0.50; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 1;
		p_stOutData->iRatioLevel = 10;
	}
	else if(strncmp(cSeclow, "jd", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0; p_stOutData->rTCostByVal = 0.000150; p_stOutData->rExchSameDayDisc = 0.00;
		p_stOutData->rRssTCostByVol = 0.0; p_stOutData->rRssTCostByVal = 0.000165;
		p_stOutData->iUnitPerContract = 10; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2; p_stOutData->iMaxSizePerOrder = 300;
		p_stOutData->rTickSize = 1.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 20; p_stOutData->iWithCancel = iYES;
	}
	else if(strncmp(cSeclow, "jm", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0; p_stOutData->rTCostByVal = 0.00010; p_stOutData->rExchSameDayDisc = 0.50;
		p_stOutData->rRssTCostByVol = 0.0; p_stOutData->rRssTCostByVal = 0.00033;
		p_stOutData->iUnitPerContract = 60; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 0.50; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 1;
		p_stOutData->iRatioLevel = 10;
	}
	else if(strncmp(cSeclow, "l", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.50; p_stOutData->rTCostByVal = 0.0; p_stOutData->rExchSameDayDisc = 0.50;
		p_stOutData->rRssTCostByVol = 0.44; p_stOutData->rRssTCostByVal = 0.0  ;
		p_stOutData->iUnitPerContract = 5; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 5.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 5; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 10;
	}
	else if(strncmp(cSeclow, "m", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.20; p_stOutData->rTCostByVal = 0.0; p_stOutData->rExchSameDayDisc = 0.50;
		p_stOutData->rRssTCostByVol = 0.165; p_stOutData->rRssTCostByVal = 0.0 ;
		p_stOutData->iUnitPerContract = 10; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 1.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 15; p_stOutData->iWithCancel = iYES;
	}
	else if(strncmp(cSeclow, "p", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.250; p_stOutData->rTCostByVal = 0.0; ; p_stOutData->rExchSameDayDisc = 0.00;
		p_stOutData->rRssTCostByVol = 0.275; p_stOutData->rRssTCostByVal = 0.00;
		p_stOutData->iUnitPerContract = 10; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 2.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 2; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 10;
	}
	else if(strncmp(cSeclow, "pp", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.0; p_stOutData->rTCostByVal = 0.000050; p_stOutData->rExchSameDayDisc = 0.50;
		p_stOutData->rRssTCostByVol = 0.0; p_stOutData->rRssTCostByVal = 0.000066;
		p_stOutData->iUnitPerContract = 5; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 1.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 45; p_stOutData->iWithCancel = iYES;
	}
	else if(strncmp(cSeclow, "v", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.250; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 0.220; p_stOutData->rRssTCostByVal = 0.00;
		p_stOutData->iUnitPerContract = 5; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 5.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 2; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 10;
	}
	else if(strncmp(cSeclow, "y", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.250; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 0.275; p_stOutData->rRssTCostByVal = 0.00;
		p_stOutData->iUnitPerContract = 10; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 2.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 2; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 10;
	}
	else if(strncmp(cSeclow, "cf", 2) == 0)
	{
		p_stOutData->rTCostByVol = 4.30 / 5.0; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 0.95; p_stOutData->rRssTCostByVal = 0.00;
		p_stOutData->iUnitPerContract = 5; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 5.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 5; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 10;
	}
	else if(strncmp(cSeclow, "fg", 2) == 0)
	{
		p_stOutData->rTCostByVol = 3.0 / 20.0; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 0.7425; p_stOutData->rRssTCostByVal = 0.00;
		p_stOutData->iUnitPerContract = 20; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 1;
		p_stOutData->rTickSize = 1.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 10;
	}
	else if(strncmp(cSeclow, "ma", 2) == 0)
	{
		p_stOutData->rTCostByVol = 7.0 / 50.0; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 0.44; p_stOutData->rRssTCostByVal = 0.00;
		p_stOutData->iUnitPerContract = 10; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 1.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 10;
	}
	else if(strncmp(cSeclow, "oi", 2) == 0)
	{
		p_stOutData->rTCostByVol = 7.0 / 50.0; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 0.1375; p_stOutData->rRssTCostByVal = 0.00;
		p_stOutData->iUnitPerContract = 10; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 2.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 10;
	}
	else if(strncmp(cSeclow, "rm", 2) == 0)
	{
		p_stOutData->rTCostByVol = 7.0 / 50.0; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 0.2475; p_stOutData->rRssTCostByVal = 0.00;
		p_stOutData->iUnitPerContract = 10; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 1.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 10;
	}
	else if(strncmp(cSeclow, "sf", 2) == 0)
	{
		p_stOutData->rTCostByVol = 7.0 / 50.0; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 1.32; p_stOutData->rRssTCostByVal = 0.00;
		p_stOutData->iUnitPerContract = 5; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 2.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 10;
	}
	else if(strncmp(cSeclow, "sm", 2) == 0)
	{
		p_stOutData->rTCostByVol = 7.0 / 50.0; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 0.99; p_stOutData->rRssTCostByVal = 0.00;
		p_stOutData->iUnitPerContract = 5; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 2.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 10;
	}
	else if(strncmp(cSeclow, "sr", 2) == 0)
	{
		p_stOutData->rTCostByVol = 0.30; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 0.165; p_stOutData->rRssTCostByVal = 0.00;
		p_stOutData->iUnitPerContract = 10; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 1;
		p_stOutData->rTickSize = 1.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 1; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 10;
	}
	else if(strncmp(cSeclow, "ta", 2) == 0)
	{
		p_stOutData->rTCostByVol = 3.0 / 5.0; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 0.33; p_stOutData->rRssTCostByVal = 0.00;
		p_stOutData->iUnitPerContract = 5; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 2.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 2; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 10;
	}
	else if(strncmp(cSeclow, "wh", 2) == 0)
	{
		p_stOutData->rTCostByVol = 3.0 / 5.0; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 0.06875; p_stOutData->rRssTCostByVal = 0.00;
		p_stOutData->iUnitPerContract = 20; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 1; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 2; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 10;
	}
	else if(strncmp(cSeclow, "zc", 2) == 0)
	{
		p_stOutData->rTCostByVol = 3.0 / 5.0; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 0.066; p_stOutData->rRssTCostByVal = 0.00;
		p_stOutData->iUnitPerContract = 100; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 0.2; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 2; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 10;
	}
	else if(strncmp(cSeclow, "ap", 2) == 0)
	{
		p_stOutData->rTCostByVol = 3.0 / 5.0; p_stOutData->rTCostByVal = 0.0;
		p_stOutData->rRssTCostByVol = 0.0275; p_stOutData->rRssTCostByVal = 0.00;
		p_stOutData->iUnitPerContract = 10; p_stOutData->iWrongVol = 100000; p_stOutData->iCostSide = 2;
		p_stOutData->rTickSize = 1.0; p_stOutData->iPMultiplier = 1; p_stOutData->iTickSizeMulti = 2; p_stOutData->iDecimalPt = 0;
		p_stOutData->iRatioLevel = 10;
	}



	return 0;
}




//----------------------------------------------------
// subroutine s_read_zz_fut_struct(lstcZZQuoteIn, lstInData)
// description: convert C structure to Fortran structure for CZCE data
// input: lstcZZQuoteIn : CZCE data in C structure
// output: lstInData     : CZCE data in Fortran structure
//----------------------------------------------------
int s_read_zz_fut_struct(struct czce_my_deep_quote *p_stcZZQuoteIn, struct in_data *p_stInData)
{

	char lcTemp[20], lcTemp1[30], *p_temp;

	p_stInData->dP = p_stcZZQuoteIn->LastPrice;
	p_stInData->dYestP = p_stcZZQuoteIn->PreSettle;
	p_stInData->dPYestCl = p_stcZZQuoteIn->PreClose;
	p_stInData->dYestOI = p_stcZZQuoteIn->PreOpenInterest * 1.0;
	p_stInData->dPOpen = p_stcZZQuoteIn->OpenPrice;
	p_stInData->dPHi = p_stcZZQuoteIn->HighPrice;
	p_stInData->dPLo = p_stcZZQuoteIn->LowPrice;
	p_stInData->dTotV = p_stcZZQuoteIn->TotalVolume * 1.0;
	p_stInData->dTotVal = 0.0;
	p_stInData->dOI = p_stcZZQuoteIn->OpenInterest * 1.0;
	p_stInData->dPClose = p_stcZZQuoteIn->ClosePrice;
	p_stInData->dPSettle = p_stcZZQuoteIn->SettlePrice;
	p_stInData->dPLmtUp = p_stcZZQuoteIn->HighLimit;
	p_stInData->dPLmtDn = p_stcZZQuoteIn->LowLimit;
	p_stInData->cExchCode = cCZCE;
	p_stInData->cSecType = cFUT; p_stInData->dV = 0.0;
	p_stInData->dVal = 0.0;
	p_stInData->dPMean = p_stcZZQuoteIn->AveragePrice;

	p_stInData->iTotalBuyVol = p_stcZZQuoteIn->TotalBidLot;
	p_stInData->iTotalSelVol = p_stcZZQuoteIn->TotalAskLot;
	p_stInData->dWeightedBuyOrderP = p_stcZZQuoteIn->BidPrice[0] - 20;
	p_stInData->dWeightedSelOrderP = p_stcZZQuoteIn->AskPrice[0] + 20;

	p_stInData->dPLifeHigh = p_stcZZQuoteIn->LifeHigh;
	p_stInData->dPLifeLow = p_stcZZQuoteIn->LifeLow;

	strncpy(lcTemp1, p_stcZZQuoteIn->TimeStamp, 25);
	p_temp = &(lcTemp1[11]);
	strncpy(lcTemp, p_temp, 15);
	if(lcTemp[8] == '.')
	{
	   p_stInData->cTime[0]= lcTemp[0]; p_stInData->cTime[1]= lcTemp[1];
	   p_stInData->cTime[2]= lcTemp[3]; p_stInData->cTime[3]= lcTemp[4];
	   p_stInData->cTime[4]= lcTemp[6]; p_stInData->cTime[5]= lcTemp[7];
	   p_stInData->cTime[6]= lcTemp[9]; p_stInData->cTime[7]= lcTemp[10];
	   p_stInData->cTime[8]= lcTemp[11]; p_stInData->cTime[9]= '\0';
	}
	else
	{
	   p_stInData->cTime[0]= lcTemp[0]; p_stInData->cTime[1]= lcTemp[1];
	   p_stInData->cTime[2]= lcTemp[3]; p_stInData->cTime[3]= lcTemp[4];
	   p_stInData->cTime[4]= lcTemp[6]; p_stInData->cTime[5]= lcTemp[7];
	   p_stInData->cTime[6]= '0'; p_stInData->cTime[7]= '0';
	   p_stInData->cTime[8]= '0'; p_stInData->cTime[9]= '\0';
	}
	//read (s_inData%strTime, '(I12)') s_inData%iTime
	p_stInData->iTime= atoi(p_stInData->cTime);
	if (p_stInData->iTime < 30000000) p_stInData->iTime = p_stInData->iTime + 240000000;
	strncpy(p_stInData->cTicker, p_stcZZQuoteIn->ContractID, 7);

	p_stInData->dBP1 = p_stcZZQuoteIn->BidPrice[0]; p_stInData->iBV1 = p_stcZZQuoteIn->BidLot[0];
	p_stInData->dSP1 = p_stcZZQuoteIn->AskPrice[0]; p_stInData->iSV1 = p_stcZZQuoteIn->AskLot[0] ;
	p_stInData->dBP2 = p_stcZZQuoteIn->BidPrice[1]; p_stInData->iBV2 = p_stcZZQuoteIn->BidLot[1];
	p_stInData->dSP2 = p_stcZZQuoteIn->AskPrice[1]; p_stInData->iSV2 = p_stcZZQuoteIn->AskLot[1];
	p_stInData->dBP3 = p_stcZZQuoteIn->BidPrice[2]; p_stInData->iBV3 = p_stcZZQuoteIn->BidLot[2];
	p_stInData->dSP3 = p_stcZZQuoteIn->AskPrice[2]; p_stInData->iSV3 = p_stcZZQuoteIn->AskLot[2];
	p_stInData->dBP4 = p_stcZZQuoteIn->BidPrice[3]; p_stInData->iBV4 = p_stcZZQuoteIn->BidLot[3];
	p_stInData->dSP4 = p_stcZZQuoteIn->AskPrice[3]; p_stInData->iSV4 = p_stcZZQuoteIn->AskLot[3];
	p_stInData->dBP5 = p_stcZZQuoteIn->BidPrice[4]; p_stInData->iBV5 = p_stcZZQuoteIn->BidLot[4];
	p_stInData->dSP5 = p_stcZZQuoteIn->AskPrice[4]; p_stInData->iSV5 = p_stcZZQuoteIn->AskLot[4] ;


    memset((void*)p_stInData->iImpBV, 0, sizeof(int)*30);
    memset((void*)p_stInData->iImpSV, 0, sizeof(int)*30);
	//p_stInData->iImpBV = 0; p_stInData->iImpSV = 0;
	p_stInData->dBP[0] = p_stInData->dBP1; p_stInData->dBP[1] = p_stInData->dBP2; p_stInData->dBP[2] = p_stInData->dBP3; p_stInData->dBP[3] = p_stInData->dBP4; p_stInData->dBP[4] = p_stInData->dBP5;
	p_stInData->dSP[0] = p_stInData->dSP1; p_stInData->dSP[1] = p_stInData->dSP2; p_stInData->dSP[2] = p_stInData->dSP3; p_stInData->dSP[3] = p_stInData->dSP4;	p_stInData->dSP[4] = p_stInData->dSP5;
	p_stInData->iBV[0] = p_stInData->iBV1; p_stInData->iBV[1] = p_stInData->iBV2; p_stInData->iBV[2] = p_stInData->iBV3; p_stInData->iBV[3] = p_stInData->iBV4; p_stInData->iBV[4] = p_stInData->iBV5;
	p_stInData->iSV[0] = p_stInData->iSV1; p_stInData->iSV[1] = p_stInData->iSV2; p_stInData->iSV[2] = p_stInData->iSV3; p_stInData->iSV[3] = p_stInData->iSV4;	p_stInData->iSV[4] = p_stInData->iSV5;
	return 0;
}


//----------------------------------------------------
// subroutine s_read_dl_fut_struct(lstcDLQuoteIn, lstInData)
// description: convert C structure to Fortran structure for DCE data
// input: lstcDLQuoteIn : DCE data in C structure
// output: lstInData     : DCE data in Fortran structure
//----------------------------------------------------
int s_read_dl_fut_struct(struct cStruct_dl_quote *p_stcDLQuoteIn, struct in_data *p_stInData)
{
	char  lcTemp[20];

	p_stInData->cExchCode = cDCE; p_stInData->cSecType = cFUT;
	strncpy(p_stInData->cTicker, p_stcDLQuoteIn->cContract, 7);
	strncpy(lcTemp, p_stcDLQuoteIn->cGenTime, 13);

	p_stInData->cTime[0] = '\0';
	if(lcTemp[8] == '.')
	{
	   p_stInData->cTime[0]= lcTemp[0]; p_stInData->cTime[1]= lcTemp[1];
	   p_stInData->cTime[2]= lcTemp[3]; p_stInData->cTime[3]= lcTemp[4];
	   p_stInData->cTime[4]= lcTemp[6]; p_stInData->cTime[5]= lcTemp[7];
	   p_stInData->cTime[6]= lcTemp[9]; p_stInData->cTime[7]= lcTemp[10];
	   p_stInData->cTime[8]= lcTemp[11]; p_stInData->cTime[9]= '\0';
	}
	else
	{
	   p_stInData->cTime[0]= lcTemp[0]; p_stInData->cTime[1]= lcTemp[1];
	   p_stInData->cTime[2]= lcTemp[3]; p_stInData->cTime[3]= lcTemp[4];
	   p_stInData->cTime[4]= lcTemp[6]; p_stInData->cTime[5]= lcTemp[7];
	   p_stInData->cTime[6]= '0'; p_stInData->cTime[7]= '0';
	   p_stInData->cTime[8]= '0'; p_stInData->cTime[9]= '\0';
	}

	p_stInData->iTime = atoi(p_stInData->cTime);
	if (p_stInData->iTime < 30000000) p_stInData->iTime = p_stInData->iTime + 240000000;

	p_stInData->dYestP = p_stcDLQuoteIn->rLastClearPrice;
	p_stInData->dP = p_stcDLQuoteIn->rLastPrice;
	p_stInData->dV = p_stcDLQuoteIn->iLastMatchQty * 1.0;
	p_stInData->dOI = p_stcDLQuoteIn->iOpenInterest * 1.0;
	p_stInData->dTotV = p_stcDLQuoteIn->iMatchTotQty * 1.0;
	p_stInData->dPLmtUp = p_stcDLQuoteIn->rRiseLimit;
	p_stInData->dPLmtDn = p_stcDLQuoteIn->rFallLimit;
	p_stInData->dBP1 = p_stcDLQuoteIn->rBuyPriceOne;
	p_stInData->dBP2 = p_stcDLQuoteIn->rBuyPriceTwo;
	p_stInData->dBP3 = p_stcDLQuoteIn->rBuyPriceThree;
	p_stInData->dBP4 = p_stcDLQuoteIn->rBuyPriceFour;
	p_stInData->dBP5 = p_stcDLQuoteIn->rBuyPriceFive;
	p_stInData->dSP1 = p_stcDLQuoteIn->rSellPriceOne;
	p_stInData->dSP2 = p_stcDLQuoteIn->rSellPriceTwo;
	p_stInData->dSP3 = p_stcDLQuoteIn->rSellPriceThree;
	p_stInData->dSP4 = p_stcDLQuoteIn->rSellPriceFour;
	p_stInData->dSP5 = p_stcDLQuoteIn->rSellPriceFive;
	p_stInData->iBV1 = p_stcDLQuoteIn->iBuyQtyOne;
	p_stInData->iBV2 = p_stcDLQuoteIn->iBuyQtyTwo;
	p_stInData->iBV3 = p_stcDLQuoteIn->iBuyQtyThree;
	p_stInData->iBV4 = p_stcDLQuoteIn->iBuyQtyFour;
	p_stInData->iBV5 = p_stcDLQuoteIn->iBuyQtyFive;
	p_stInData->iSV1 = p_stcDLQuoteIn->iSellQtyOne;
	p_stInData->iSV2 = p_stcDLQuoteIn->iSellQtyTwo;
	p_stInData->iSV3 = p_stcDLQuoteIn->iSellQtyThree;
	p_stInData->iSV4 = p_stcDLQuoteIn->iSellQtyFour;
	p_stInData->iSV5 = p_stcDLQuoteIn->iSellQtyFive;
	p_stInData->dPOpen = p_stcDLQuoteIn->rOpenPrice;
	p_stInData->dPHi = p_stcDLQuoteIn->rHighPrice;
	p_stInData->dPLo = p_stcDLQuoteIn->rLowPrice;
	p_stInData->dPYestCl = p_stcDLQuoteIn->rLastClose;
	p_stInData->dYestOI = p_stcDLQuoteIn->iLastOpenInterest * 1.0;
	p_stInData->dPMean = p_stcDLQuoteIn->rAvgPrice;
	p_stInData->dTotVal = p_stcDLQuoteIn->rTurnover;
	p_stInData->dVal = 0.0;

	p_stInData->dBP[0] = p_stInData->dBP1; p_stInData->dBP[1] = p_stInData->dBP2; p_stInData->dBP[2] = p_stInData->dBP3; p_stInData->dBP[3] = p_stInData->dBP4; p_stInData->dBP[4] = p_stInData->dBP5;
	p_stInData->dSP[0] = p_stInData->dSP1; p_stInData->dSP[1] = p_stInData->dSP2; p_stInData->dSP[2] = p_stInData->dSP3; p_stInData->dSP[3] = p_stInData->dSP4;	p_stInData->dSP[4] = p_stInData->dSP5;
	p_stInData->iBV[0] = p_stInData->iBV1; p_stInData->iBV[1] = p_stInData->iBV2; p_stInData->iBV[2] = p_stInData->iBV3; p_stInData->iBV[3] = p_stInData->iBV4; p_stInData->iBV[4] = p_stInData->iBV5;
	p_stInData->iSV[0] = p_stInData->iSV1; p_stInData->iSV[1] = p_stInData->iSV2; p_stInData->iSV[2] = p_stInData->iSV3; p_stInData->iSV[3] = p_stInData->iSV4;	p_stInData->iSV[4] = p_stInData->iSV5;
	p_stInData->iImpBV[0] = p_stcDLQuoteIn->iBuyImplyQtyOne; p_stInData->iImpBV[1] = p_stcDLQuoteIn->iBuyImplyQtyTwo;
	p_stInData->iImpBV[2] = p_stcDLQuoteIn->iBuyImplyQtyThree; p_stInData->iImpBV[3] = p_stcDLQuoteIn->iBuyImplyQtyFour; p_stInData->iImpBV[4] = p_stcDLQuoteIn->iBuyImplyQtyFive;
	p_stInData->iImpSV[0] = p_stcDLQuoteIn->iSellImplyQtyOne; p_stInData->iImpSV[1] = p_stcDLQuoteIn->iSellImplyQtyTwo;
	p_stInData->iImpSV[2] = p_stcDLQuoteIn->iSellImplyQtyThree; p_stInData->iImpSV[3] = p_stcDLQuoteIn->iSellImplyQtyFour; p_stInData->iImpSV[4] = p_stcDLQuoteIn->iSellImplyQtyFive;

	return 0;
}

int s_read_cstruct_dl_orderstat(struct cStruct_dl_orderstat *p_stcStructDLOrderStatIn,
	struct in_data *p_stInData)
{
	p_stInData->iTotalBuyVol = p_stcStructDLOrderStatIn->iTotalBuyOrderNum;
	p_stInData->iTotalSelVol = p_stcStructDLOrderStatIn->iTotalSellOrderNum;
	p_stInData->dWeightedBuyOrderP = p_stcStructDLOrderStatIn->rWeightedAverageBuyOrderPrice;
	p_stInData->dWeightedSelOrderP = p_stcStructDLOrderStatIn->rWeightedAverageSellOrderPrice;
	return 0;
}


//----------------------------------------------------
// integer function f_sign(liN)
// desprition : return the sign of the input number
//----------------------------------------------------
int f_sign(int liN)
{
	int i_sign;
	if (liN > 0) i_sign = 1;
	if (liN == 0) i_sign = 0;
	if (liN < 0) i_sign = -1;

	return i_sign;
}

//----------------------------------------------------
// real(dp) function f_calc_curr_avgp(lrVal, liV, lrPrevAvgP)
// desprition : calculate current average price
//----------------------------------------------------
double f_calc_curr_avgp(double lrVal, int liV, double lrPrevAvgP,struct all_parameter *p_stGlobalPara)
{
	double dres;
	if (liV > 0 && lrVal > 0.0)
		dres = lrVal / liV;
	else
		dres = lrPrevAvgP * p_stGlobalPara->stSec.iUnitPerContract;

	return dres;
}

//----------------------------------------------------
// integer function f_real_to_int(lrR, liMultiplier)
// desprition : convert real price to integer price
//----------------------------------------------------
int  f_real_to_int(double lrR, int liMultiplier)
{
	return (int)(lrR*liMultiplier);
}


//----------------------------------------------------
// character*4 function f_find_security_name(lcTicker)
// desprition : fine name of the security
//----------------------------------------------------
int f_find_security_name(char* sc_name, char *lcTicker,char lcExch)
{
	if (lcExch != cCZCE)
	{
		strncpy(sc_name, lcTicker, strlen(lcTicker));
		sc_name[strlen(lcTicker)-4] = '\0';

	}
	else
	{
		strncpy(sc_name, lcTicker, strlen(lcTicker));
		sc_name[strlen(lcTicker)-3] = '\0';
	}
	return 0;
}


//----------------------------------------------------
// integer function f_is_limit(lrBP1, lrSP1, lrLmtUpP, lrLmtDnP)
// desprition : find whether it up/dn limit, and return limit type
//----------------------------------------------------
int f_is_limit(double lrBP1, double lrSP1, double lrLmtUpP, double lrLmtDnP)
{
	int irst;

	irst = iFLAT;
	if (fabs(lrBP1 - lrLmtUpP) < 0.001)
		irst = iUP;

	if (fabs(lrSP1 - lrLmtDnP) < 0.001)
		irst = iDN;

	return irst;
}



//*****************************************************************************80
//// ALNORM computes the cumulative density of the standard normal distribution.
//  Licensing:
//    This code is distributed under the GNU LGPL license.
//  Modified:
//    13 January 2008
//  Author:
//    Original FORTRAN77 version by David Hill.
//    FORTRAN90 version by John Burkardt.
//  Reference:
//    David Hill,
//    Algorithm AS 66:
//    The Normal Integral,
//    Applied Statistics,
//    Volume 22, Number 3, 1973, pages 424-427.
//  Parameters:
//    Input, real ( kind = 8 ) X, is one endpoint of the semi-infinite interval
//    over which the integration takes place.
//
//    Input, logical UPPER, determines whether the upper or lower
//    interval is to be integrated:
//    .TRUE.  => integrate from X to + Infinity;
//    .FALSE. => integrate from - Infinity to X.
//
//    Output, real ( kind = 8 ) ALNORM, the integral of the standard normal
//    distribution over the desired interval.
//
double alnorm(double x, BOOL upper )
{

  const double a1 = 5.758854804580;
  const double a2 = 2.624331216790;
  const double a3 = 5.928857244380;
  double rst;
  const double b1 = -29.82135578070;
  const double b2 = 48.69599306920;
  const double c1 = -0.0000000380520;
  const double c2 = 0.0003980647940;
  const double c3 = -0.1516791166350;
  const double c4 = 4.83859128080;
  const double c5 = 0.7423809240270;
  const double c6 = 3.990194170110;
  const double con = 1.280;
  const double d1 = 1.000006153020;
  const double d2 = 1.986153813640;
  const double d3 = 5.293303249260;
  const double d4 = -15.15089724510;
  const double d5 = 30.7899330340;
  const double ltone = 7.00;
  const double p = 0.3989422804440;
  const double q = 0.399903485040;
  const double r = 0.3989422803850;
  BOOL up;
  const double utzero = 18.660;
  double y;
  double z;

  up = upper;
  z = x;

  if ( z < 0.0 )
  {
	up = !up;
	z = -z;
  }

  if ( ltone < z && ( ( !up ) || utzero < z ) )
  {
	if ( up )
	  rst = 0.00;
	else
	  rst = 1.00;

	return rst;
  }

  y = 0.50 * z * z;

  if ( z <= con )
	rst = 0.50 - z * ( p - q * y / ( y + a1 + b1 / ( y + a2 + b2/( y + a3 ))));
  else
	rst = r * exp(-y)/ ( z + c1 + d1/ ( z + c2 + d2 / ( z + c3 + d3 / ( z + c4 + d4
	  / ( z + c5 + d5 / ( z + c6 ))))));


  if ( !up )
	rst = 1.0 - rst;

  return rst;
}


