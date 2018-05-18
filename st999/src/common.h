#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <string.h>


#define MY_SHFE_QUOTE_PRICE_POS_COUNT 30
#define USE_SHFE_LV1_AND_DEEP

#define MAX(a,b)  ((a)>(b)?(a):(b))
#define MIN(a,b)  ((a)>(b)?(b):(a))
#define RELEASE(mem) if(mem != NULL) {free(mem); mem = NULL;}

typedef enum {false=0,true=!false} BOOL;

#define iSIGNAL_LEN 1000
#define iOUT_LEN 100

#define iSTRATEGY_MK2 100
#define iMK201 101

#define MAX_NUM_CONTRACT 10

static const char gcVersion[50]= "0.1.1.5tb";
extern int giNightMarket;
extern int giRunModeFlg;
extern int giStrateType, giMK2Type;
extern int giStratID;

extern char gcExch, gcSecName[8], gcTicker[8];
extern int giCurDate;
extern char gszEVname[256];

static const int iSigStatusSuccess = 0, iSigStatusPart = 1,
				 iSigStatusCancel = 2, iSigStatusRejected = 3;
static const int iSigInstructionROD = 0, iSigInstructionFOK = 1,
				 iSigInstructionFAK = 2, iSigInstructionMRK = 3;
static const int iSigInstructionQuote = 4, iSigInstructionMAX = 5;

static const int iBUY = 1, iSELL = -1, iZERO = 0, iLONG = 1;
static const int iSHORT = -1, iUP = 1, iDN = -1, iFLAT = 0, iYES = 1, iNO = 0;
static const int iBID = 1, iOFFER = -1, iCD = 2, iXD = 1;
static const int iBPK = 1, iSPK = 2, iBP = 3, iSP = 4, iBK = 5, iSK = 6;

static const char cSHFE = 'A', cDCE = 'B', cCZCE = 'C', cCFFE = 'G', cFUT = 'F',
				  cBLANK ='\0';


struct cfex_deep_quote {
	char szTradingDay[9];
	char szSettlementGroupID[9];
	int nSettlementID;
	double dLastPrice;
	double dPreSettlementPrice;
	double dPreClosePrice;
	double dPreOpenInterest;
	double dOpenPrice;
	double dHighestPrice;
	double dLowestPrice;
	int nVolume;
	double dTurnover;
	double dOpenInterest;
	double dClosePrice;
	double dSettlementPrice;
	double dUpperLimitPrice;
	double dLowerLimitPrice;
	double dPreDelta;
	double dCurrDelta;
	char szUpdateTime[9];
	int nUpdateMillisec;
	char szInstrumentID[31];
	double dBidPrice1;
	int nBidVolume1;
	double dAskPrice1;
	int nAskVolume1;
	double dBidPrice2;
	int nBidVolume2;
	double dAskPrice2;
	int nAskVolume2;
	double dBidPrice3;
	int nBidVolume3;
	double dAskPrice3;
	int nAskVolume3;
	double dBidPrice4;
	int nBidVolume4;
	double dAskPrice4;
	int nAskVolume4;
	double dBidPrice5;
	int nBidVolume5;
	double dAskPrice5;
	int nAskVolume5;
};




struct czce_my_deep_quote {
	char        ContractID[50];
	char  ContractIDType;
	double    PreSettle;
	double    PreClose;
	int    PreOpenInterest;
	double    OpenPrice;
	double    HighPrice;
	double    LowPrice;
	double    LastPrice;
	double    BidPrice[5];
	double    AskPrice[5];
	int    BidLot[5];
	int    AskLot[5];
	int    TotalVolume;
	int    OpenInterest;
	double    ClosePrice;
	double    SettlePrice;
	double    AveragePrice;
	double    LifeHigh;
	double    LifeLow;
	double    HighLimit;
	double    LowLimit;
	int    TotalBidLot;
	int    TotalAskLot;
	char   TimeStamp[24];
};

struct cStruct_dl_quote {
	char cType;
	int iLength, iVersion, iTime;
	char cExch[3], cContract[80];
	int iSuspensionSign;
	float rLastClearPrice, rClearPrice, rAvgPrice, rLastClose, rClose, rOpenPrice;
	int iLastOpenInterest, iOpenInterest;
	float rLastPrice;
	int iMatchTotQty;
	double rTurnover;
	float rRiseLimit, rFallLimit, rHighPrice, rLowPrice, rPreDelta, rCurrDelta,
		  rBuyPriceOne;
	int iBuyQtyOne, iBuyImplyQtyOne;
	float rBuyPriceTwo;
	int iBuyQtyTwo, iBuyImplyQtyTwo;
	float rBuyPriceThree;
	int iBuyQtyThree, iBuyImplyQtyThree;
	float rBuyPriceFour;
	int iBuyQtyFour, iBuyImplyQtyFour;
	float rBuyPriceFive;
	int iBuyQtyFive, iBuyImplyQtyFive;
	float rSellPriceOne;
	int iSellQtyOne, iSellImplyQtyOne;
	float rSellPriceTwo;
	int iSellQtyTwo, iSellImplyQtyTwo;
	float rSellPriceThree;
	int iSellQtyThree, iSellImplyQtyThree;
	float rSellPriceFour;
	int iSellQtyFour, iSellImplyQtyFour;
	float rSellPriceFive;
	int iSellQtyFive, iSellImplyQtyFive;
	char cGenTime[13];
	int iLastMatchQty, iInterestChg;
	float rLifeLow, rLifeHigh;
	double rDelta, rGamma, rRho, rTheta, rVega;
	char cTradeDate[9], cLocalDate[9];
};

struct    cStruct_dl_orderstat {
	char cType;
	unsigned int iLen;
	char cContractID[80];
	unsigned int iTotalBuyOrderNum;
	unsigned int iTotalSellOrderNum;
	double     rWeightedAverageBuyOrderPrice;
	double     rWeightedAverageSellOrderPrice;
};




struct czce_my_snapshot_quote {
	char     CmbType;
	char    ContractID[50];
	double    BidPrice;
	double    AskPrice;
	int    BidLot;
	int    AskLot;
	int    VolBidLot;
	int    VolAskLot;
	char        TimeStamp[24];
};


struct shfe_my_quote {

	char    TradingDay[9];
	char    SettlementGroupID[9];
	int    SettlementID;
	double    LastPrice;
	double    PreSettlementPrice;
	double    PreClosePrice;
	double    PreOpenInterest;
	double    OpenPrice;
	double    HighestPrice;
	double    LowestPrice;
	int    Volume;
	double    Turnover;
	double    OpenInterest;
	double    ClosePrice;
	double    SettlementPrice;
	double    UpperLimitPrice;
	double    LowerLimitPrice;
	double    PreDelta;
	double    CurrDelta;

	char    UpdateTime[9];
	int    UpdateMillisec;
	char    InstrumentID[31];
	double    BidPrice1;
	int    BidVolume1;
	double    AskPrice1;
	int    AskVolume1;
	double    BidPrice2;
	int    BidVolume2;
	double    AskPrice2;
	int    AskVolume2;
	double    BidPrice3;
	int    BidVolume3;
	double    AskPrice3;
	int    AskVolume3;
	double    BidPrice4;
	int    BidVolume4;
	double    AskPrice4;
	int    AskVolume4;
	double    BidPrice5;
	int    BidVolume5;
	double    AskPrice5;
	int    AskVolume5;

	char    ActionDay[9];
	int   data_flag;


	unsigned short  start_index;
	unsigned short  data_count;
	double buy_price[MY_SHFE_QUOTE_PRICE_POS_COUNT];
	int    buy_volume[MY_SHFE_QUOTE_PRICE_POS_COUNT];
	double sell_price[MY_SHFE_QUOTE_PRICE_POS_COUNT];
	int    sell_volume[MY_SHFE_QUOTE_PRICE_POS_COUNT];


	unsigned int   buy_total_volume;
	unsigned int   sell_total_volume;
	double         buy_weighted_avg_price;
	double         sell_weighted_avg_price;
};


struct in_data {
	char  cTime[10], cTicker[16], cSecType, cExchCode;
	double  dYestP, dP, dV, dOI, dTotV, dPLmtUp, dPLmtDn, dPOpen, dPHi, dPLo,
			dPYestCl, dPClose;
	double  dYestOI, dPMean, dTotVal, dVal, dPSettle;
	double  dBP1, dBP2, dBP3, dBP4, dBP5, dSP1, dSP2, dSP3, dSP4, dSP5;
	int     iBV1, iBV2, iBV3, iBV4, iBV5, iSV1, iSV2, iSV3, iSV4, iSV5, iTime;
	double  dBP[30], dSP[30];
	int  iBV[30], iSV[30], iImpBV[30], iImpSV[30];
	int  iVol, iTotalBuyVol, iTotalSelVol;
	double  dAvgP, dOIChg, dWeightedBuyOrderP, dWeightedSelOrderP, dPLifeHigh,
			dPLifeLow;
	int  iData_Flag;
};

//struct struct_symbol_pos {
//	char  ticker[64];
//	int long_volume;
//	double long_price;
//	int short_volume;
//	double short_price;
//	int pos_change;
//};

//struct struct_position {
//	int count;
//	struct struct_symbol_pos pos[10000];
//};



struct cStruct_each_pos_in {
	char symbol[30];
	int exchange, maxPos, PDLongPos, PDShortPos, TDLongPos, TDShortPos;
	double availFund, commRatio;
	int longPosIncr, shortPosIncr;
	BOOL feedPos;
	int quoteCat;
};

struct cStruct_each_pending_vol_in {
	char symbol[30];
	double    price;
	int side, volume;
};

struct sec_parameter {
	//char cExchCode;
	int iSecMktOpenTime, iSecTradeStartTime, iSecTradeEndTime, iSecOpenEndTime;
	int iSecTradeStartTime02, iSecTradeEndTime02;
	int iSecEndTime, iSecFinalSqTime, iSecTraceTime;
	int iUnitPerContract, iWrongVol, iCostSide, iPMultiplier, iTickSizeMulti,
		iDecimalPt, iRatioLevel;
	int iWithCancel;
	int iMaxSizePerOrder;
	double rTCostByVol, rTCostByVal, rPctLmt, rBrokerPct, rTickSize, rOVCompRatio;
	double rTVCompRatio, rExchSameDayDisc;
	double rRssTCostByVol, rRssTCostByVal;
};

struct exch_parameter {
	int iVolDivider, iPInLevel, iPreMktAucTime, iMktStartTime, iMktEndTime;
};


typedef struct all_parameter {
	struct exch_parameter stExch;
	struct sec_parameter stSec;
} st_all_parameter;


struct myTSignal {
	int iStID, iSigID, iBV, iSV, iBP, iSP;
	double rBP, rSP;
	char cExchCode, cTicker[7];
	int iSigAction, iInstruction, iCancelSigID;
};




struct comm_out_data {
	char cTicker[8];
	int iTime, iRecordNo, iV;
	double rP, rBP1, rSP1, rVal, rOI, rBuyPrice, rSelPrice;
	int iBV1, iSV1, iHaveOrder, iTS, iCancel, iOpenSize, iCloseSize;
	int iToTradeSize, iLongPos, iShortPos, iSidePos, iTotalOrderSize,
		iTotalCancelSize;
    int iTotalOrderCount, iTotalCancelCount;
};

typedef struct struct_order_info{
	int sig_id;
	int itime;
	int cancel_sig_id;
	double price;
	int volume;
	int direct;  //1: buy;  2: sell  4: cancel
	int kaiping;  //0:ping;  1: kai
	int vol2Pass;
	double exec_avg_price;
	int exec_acc_vol;
	double exec_price;
	int exec_vol;
	int status;
	int simu_status; //0 or 1
	char szContract[16];
} st_order_info;

struct st_date_open_close_param
{
    int iDate;
    int iFlag; //0:day; 1:night
    int iStratID;
    double dOpenParam;
    double dCloseParam;
    char szSec[16];
};

//st_order_info.status
enum {
		 SIG_STATUS_INIT = -1,			 //new order
	 	 SIG_STATUS_SUCCESS, 	 	     /* 报单全部成交 */
	 	 SIG_STATUS_ENTRUSTED, 	 	   /* 报单委托成功 */
	 	 SIG_STATUS_PARTED, 	 	 	 /* 报单部分成交 */
	 	 SIG_STATUS_CANCEL, 	 	 	 /* 报单被撤销 */
	 	 SIG_STATUS_REJECTED, 	 	 	 /* 报单被拒绝 */
};

//st_order_info.kaiping
enum {
    SIG_ACTION_OPEN = 1,
    SIG_ACTION_FLAT = 2,
};

//st_order_info.direct
enum {
    SIG_DIRECTION_BUY = 1,
    SIG_DIRECTION_SELL= 2,
    SIG_DIRECTION_CANCEL = 4,
};


typedef struct struct_local_trade_info
{
    int iPosD, iPosK;
    double rCash, rTotalCost;
    double rLivePL;
    int iLastTradeTime;
    int iTotalVol;
    int iTotalOrderSize, iTotalCancelSize;
    int iCurrOrderSize, iCurrCancelSize;
    int iTotalOrderCount, iTotalCancelCount;
    double rMaxLivePL, rMaxLiveDD, rLiveDD;
    int iMaxSidePos, iMaxNetPos;
    char szContract[16];
    char szSecName[16];
    char cExch;
    int isetMaxPos;
}st_loc_trade_info;

struct strat_out_log
{
    int exch_time; //exchange time
    char contract[16]; //contract
    int n_tick;
    double price;
    int vol;
    int bv1;
    double bp1;
    double sp1;
    int sv1;
    long amt;
    long oi;
    double buy_price, sell_price;
    int open_vol, close_vol;
    int long_pos, short_pos;
    int tot_ordervol, tot_cancelvol;
    int order_cnt, cancel_cnt;
    double cash, live;
    int tot_vol;
    double max_dd;
    int max_net_pos, max_side_pos;
    double sig[12];
};

extern int  giLb, giLen;
extern int  giPosLb, giPosLen, giPendVLb, giPendVLen;

extern struct in_data gstCurrIn;
extern struct in_data gstPreIn[MAX_NUM_CONTRACT];
extern struct in_data gstRecCurrIn;
extern struct in_data gstRecPreIn[MAX_NUM_CONTRACT];
extern struct cfex_deep_quote     *gstcStructCFFEIn;
extern struct cStruct_dl_quote     *gstcStructDCEIn;
extern struct shfe_my_quote     *gstcStructSHFEIn;
extern struct czce_my_deep_quote     *gstcStructCZCEIn;

extern st_all_parameter    gstGlobalPara[MAX_NUM_CONTRACT];
extern int  giTickNo;

extern int *gaiFullOrderBookBuy[MAX_NUM_CONTRACT];
extern int *gaiFullOrderBookSel[MAX_NUM_CONTRACT];
extern int *gaiPreOrderBookBuy[MAX_NUM_CONTRACT];
extern int *gaiPreOrderBookSel[MAX_NUM_CONTRACT];

extern int  giLenFullOrderBook[MAX_NUM_CONTRACT];
extern double  grSimuPara1, grSimuPara2, grSimuPara3;
extern int  giSizePerTrade;
extern FILE  *pfDayLogFile;
extern char  gcFileName[512];
extern int  giInitFlg[MAX_NUM_CONTRACT];
extern int  giWriteOutputFileFlg, giWriteOutputFileInitFlg;

extern double grSimuValueTot, grSimuValueTemp, grSimuValueMax, grSimuDrawDown;
extern double grTotBench, grSimuDrawDownRatio;
extern int  giMaxTime;

extern int giStID, giMaxPos;

extern st_loc_trade_info gstLocTradeInfo[MAX_NUM_CONTRACT];
extern int giNumContract, giTradeInfo;
extern char file_history_log[1500];

#endif
