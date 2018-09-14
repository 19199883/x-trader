#ifndef MK2_VAR_DEF_H
#define MK2_VAR_DEF_H

#define MK2_MAX_POS 50
#define MK2_MM_LEVEL 6
#define MK2_MAX_MM_TIME 300

struct struct_out_order
{
    double price;
    int ivol;
    int vol2pass;
};

struct struct_mm_outdata
{
	struct comm_out_data stCommOutData;
    int iTime, iVol, iBV1, iSV1, iSignalType, iVol2Trade, iPos, iPOSD, iPOSK, iRounds, iVol2Pass, iTickNo, iNewPOS, iNetPOS;
    int iRound, iCanCelOrder, iMaxNetPos, iMaxSidePos;
    double rP, rBP1, rSP1, rBVSVRatio, rTradeBP, rTradeSP, rCash, rVround, rPnl,rLivePL, rAvgP, rVolBuy, rVolSel, rPNet;
    char cTradeSig[4], cTime[10];
    double rBigVolScore, rMaxDD;
    double rsig1,rsig2,rsig3;
    struct struct_out_order out_order[10];
};

struct struct_MK2_paras
{
	double rBVSum, rSVSum, rStopLossThreshold, rVolBuy, rVolSel, rVolBuyPre, rVolSelPre, rPNet;
    int iStop, iUPstop, iDNstop, iWithClose;
    int iLimitRange, iLimitStop, iLimitSqRange, iLimitSqFlg, iPreviousSigType, iRecPreviousSigType;
};

static const int giMaxMMlevel = MK2_MM_LEVEL, giKeepOrderTime = MK2_MAX_MM_TIME;


extern struct struct_mm_outdata gastMK2OutData[iOUT_LEN];
extern int giMK2OutDataIndex, giMK2TraceInit;
extern struct struct_MK2_paras gstMK2_Paras;
extern int gilastThreeRec[3];
//extern int giSingleOrderMaxVol;
extern struct strat_out_log m_strat_log;

#endif




