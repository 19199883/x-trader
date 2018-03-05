
Module Common_Var_Def
	Use iso_c_binding
	Implicit None
	Save
    
    integer   :: giRunModeFlg = 1 ! 0: similation; 1: real run
    integer   :: giNightMarket = 0 ! 0: Day;  1: Night
    character :: gcExch*1 ! exchage code, SHFE = A, DCE = B, CZCE = C, CFFE = G
    character :: gcSecName*8
    character :: gcTicker*8
    integer   :: giStrateType = 5 ! 5: Hi5;
    integer   :: giHi5Type = 500  ! Hi50 : 500;   Hi51 : 510;
                                  ! Hi52 : 520;   Hi53 : 530;
                                  ! Hi54 : 540;   Hi55 : 550;
                                  ! Hi56 : 560;   Hi57 : 570;
                                  ! Hi58 : 580;   Hi59 : 590;
    
    character   ::  gcStrategyName*10 = 'tst0' !
    integer     ::  giReadParaFailedFlg = 0
    integer, parameter   :: iSTRATEGY_HI5 = 5
    integer, parameter   :: iHI50 = 500, iHI51 = 510, &
                            iHI52 = 520, iHI53 = 530, &
                            iHI54 = 540, iHI55 = 550, &
                            iHI56 = 560, iHI57 = 570, &
                            iHI58 = 580, iHI59 = 590    
    
    integer     ::  giFillRate = 30
    
    !----------------------------------------------------
    ! other global parameter variables
    !----------------------------------------------------    
	character, parameter	::	cBid*1 = 'B', cOffer*1 = 'O', cBP*2 = 'BP', cSP*2 = 'SP', cBPK*3 = 'BPK', cSPK*3 = 'SPK', &
								cCD*2 = 'CD', cBK*2 = 'BK', cETF*1 = 'E', cSTK*1 = 'S', cFUT*1 = 'F', cIDX*1 = 'I', &
								cSTK_STATUS_OPEN*1 = 'O', cZERO*1 = '0', cSYNC*1 = 's', cASYNC*1 = 'a', cRESUME_POS*1 = 'Y', &
								cIGNORE_POS*1 ='N', cIS_FAK*1 = '1', cNOT_FAK*1 = '0', cNIGHT*1 = 'N', cDAY*1 = 'D', &
								cBLANK*1 = '', cFILLED = 'c', cPART_FILLED = 'p'    
	integer, parameter		::	iSigStatusEntrusted = 1, iSigStatusPart = 2, iSigStatusCancel = 3, &
								iSigInstructionROD = 0, iSigInstructionFOK = 1, iSigInstructionFAK = 2, iSigInstructionMRK = 3, &
								iSigActNull = z'00', iSigActBuy = z'01', iSigActSell = z'02', iSigActCancel = z'04', &
								iBUY = 1, iSELL = -1, iZERO = 0, iLONG = 1, &
								iSHORT = -1, iUP = 1, iDN = -1, iFLAT = 0, iYES = 1, iNO = 0, &
                                iBID = 1, iOFFER = -1, iCD = 2, iXD = 1, &
								iBPK = 1, iSPK = 2, iBP = 3, iSP = 4, iBK = 5, iSK = 6, &
								iSIGNAL_LEN = 1000, iPOS_LEN = 400, iPENDINGV_LEN = 1000, iOUT_LEN = 100
	character, parameter	::	cSHFE*1 = 'A', cDCE*1 = 'B', cCZCE = 'C', cCFFE*1 = 'G', &
								cSZSE = '0', cSSE = '1'
	integer, parameter		::	dp = kind(0.d0)
	real(dp), parameter		::	rZERO = 0._dp 
    
    
    
        type, bind(C)	::	cStruct_gta_quote
		    character(c_char)	::	szTradingDay(9), szSettlementGroupID(9)
		    integer(c_int)		::	nSettlementID
		    real(c_double)		::	dLastPrice, dPreSettlementPrice, dPreClosePrice, dPreOpenInterest, dOpenPrice
		    real(c_double)		::	dHighestPrice, dLowestPrice
		    integer(c_int)		::	nVolume
		    real(c_double)		::	dTurnover, dOpenInterest, dClosePrice, dSettlementPrice, dUpperLimitPrice, dLowerLimitPrice
		    real(c_double)		::	dPreDelta, dCurrDelta
		    character(c_char)	::	szUpdateTime(9)
		    integer(c_int)		::	nUpdateMillisec
		    character(c_char)	::	szInstrumentID(31)
		    real(c_double)		::	dBidPrice1
		    integer(c_int)		::	nBidVolume1
		    real(c_double)		::	dAskPrice1
		    integer(c_int)		::	nAskVolume1
		    real(c_double)		::	dBidPrice2
		    integer(c_int)		::	nBidVolume2
		    real(c_double)		::	dAskPrice2
		    integer(c_int)		::	nAskVolume2
		    real(c_double)		::	dBidPrice3
		    integer(c_int)		::	nBidVolume3
		    real(c_double)		::	dAskPrice3
		    integer(c_int)		::	nAskVolume3
		    real(c_double)		::	dBidPrice4
		    integer(c_int)		::	nBidVolume4
		    real(c_double)		::	dAskPrice4
		    integer(c_int)		::	nAskVolume4
		    real(c_double)		::	dBidPrice5
		    integer(c_int)		::	nBidVolume5
		    real(c_double)		::	dAskPrice5
		    integer(c_int)		::	nAskVolume5
    end type cStruct_gta_quote
    
    !----------------------------------------------------
    ! add SHFE 30 level data structure
    !----------------------------------------------------	
	type, bind(C)	::	cStruct_shfe_quote
		character(c_char)	::	szTradingDay(9), szSettlementGroupID(9)
		integer(c_int)		::	nSettlementID
		real(c_double)		::	dLastPrice, dPreSettlementPrice, dPreClosePrice, dPreOpenInterest, dOpenPrice
		real(c_double)		::	dHighestPrice, dLowestPrice
		integer(c_int)		::	nVolume
		real(c_double)		::	dTurnover, dOpenInterest, dClosePrice, dSettlementPrice, dUpperLimitPrice, dLowerLimitPrice
		real(c_double)		::	dPreDelta, dCurrDelta
		character(c_char)	::	szUpdateTime(9)
		integer(c_int)		::	nUpdateMillisec
		character(c_char)	::	szInstrumentID(31)
		real(c_double)		::	dBidPrice1
		integer(c_int)		::	nBidVolume1
		real(c_double)		::	dAskPrice1
		integer(c_int)		::	nAskVolume1
		real(c_double)		::	dBidPrice2
		integer(c_int)		::	nBidVolume2
		real(c_double)		::	dAskPrice2
		integer(c_int)		::	nAskVolume2
		real(c_double)		::	dBidPrice3
		integer(c_int)		::	nBidVolume3
		real(c_double)		::	dAskPrice3
		integer(c_int)		::	nAskVolume3
		real(c_double)		::	dBidPrice4
		integer(c_int)		::	nBidVolume4
		real(c_double)		::	dAskPrice4
		integer(c_int)		::	nAskVolume4
		real(c_double)		::	dBidPrice5
		integer(c_int)		::	nBidVolume5
		real(c_double)		::	dAskPrice5
		integer(c_int)		::	nAskVolume5
		character(c_char)	::  szActionDay(9)
		! the belowing is new added
		integer(c_int)      ::  ndata_flag  ! data flag, 1: market data is valid, 2: quote data is valid, 3: both are valid
		integer(c_short)    ::  nstart_index
		integer(c_short)    ::  ndata_count  ! level number: 30
		real(c_double)      ::  dbuy_price(30)
		integer(c_int)      ::  nbuy_volume(30)
		real(c_double)      ::  dsell_price(30)
		integer(c_int)      ::  nsell_volume(30)
		integer(c_int)      ::  nbuy_total_volume  ! quoted total buy volume
		integer(c_int)      ::  nsell_total_volume  ! quoted total sell volume
		real(c_double)      ::  dbuy_weighted_avg_price  ! weighted averaged quoted buy price
		real(c_double)      ::  dsell_weighted_avg_price  ! weighted averaged quoted sell price	
    end type cStruct_shfe_quote	
    
    !----------------------------------------------------
    ! DCE level data structure
    !----------------------------------------------------	
	type, bind(C)	::	cStruct_dl_quote
		character(c_char)	::	cType(1)
		integer(c_int)		::	iLength, iVersion, iTime
		character(c_char)	::	cExch(3), cContract(80)
		integer(c_int)		::	iSuspensionSign
		real(c_float)		::	rLastClearPrice, rClearPrice, rAvgPrice, rLastClose, rClose, rOpenPrice
		integer(c_int)		::	iLastOpenInterest, iOpenInterest
		real(c_float)		::	rLastPrice
		integer(c_int)		::	iMatchTotQty
		real(c_double)		::	rTurnover
		real(c_float)		::	rRiseLimit, rFallLimit, rHighPrice, rLowPrice, rPreDelta, rCurrDelta, rBuyPriceOne
		integer(c_int)		::	iBuyQtyOne, iBuyImplyQtyOne
		real(c_float)		::	rBuyPriceTwo
		integer(c_int)		::	iBuyQtyTwo, iBuyImplyQtyTwo
		real(c_float)		::	rBuyPriceThree
		integer(c_int)		::	iBuyQtyThree, iBuyImplyQtyThree
		real(c_float)		::	rBuyPriceFour
		integer(c_int)		::	iBuyQtyFour, iBuyImplyQtyFour
		real(c_float)		::	rBuyPriceFive
		integer(c_int)		::	iBuyQtyFive, iBuyImplyQtyFive
		real(c_float)		::	rSellPriceOne
		integer(c_int)		::	iSellQtyOne, iSellImplyQtyOne
		real(c_float)		::	rSellPriceTwo
		integer(c_int)		::	iSellQtyTwo, iSellImplyQtyTwo
		real(c_float)		::	rSellPriceThree
		integer(c_int)		::	iSellQtyThree, iSellImplyQtyThree
		real(c_float)		::	rSellPriceFour
		integer(c_int)		::	iSellQtyFour, iSellImplyQtyFour
		real(c_float)		::	rSellPriceFive
		integer(c_int)		::	iSellQtyFive, iSellImplyQtyFive
		character(c_char)	::	cGenTime(13)
		integer(c_int)		::	iLastMatchQty, iInterestChg
		real(c_float)		::	rLifeLow, rLifeHigh
		real(c_double)		::	rDelta, rGamma, rRho, rTheta, rVega
		character(c_char)	::	cTradeDate(9), cLocalDate(9)
    end type cStruct_dl_quote
    
    type, bind(C)	::	cStruct_dl_orderstat
	    character(c_char)		::	cType(1)
	    integer(c_int)			::	iLen
	    character(c_char)		::	cContractID(80)
	    integer(c_int)			::	iTotalBuyOrderNum
	    integer(c_int)			::	iTotalSellOrderNum
	    real(c_double)			::	rWeightedAverageBuyOrderPrice
	    real(c_double)			::	rWeightedAverageSellOrderPrice
    end type cStruct_dl_orderstat
    !----------------------------------------------------
    ! CZCE level data structure
    !----------------------------------------------------	
	type, bind(C)	::	cStruct_zz_quote
		character(c_char)	::	ContractID(50)
		character(c_char)	::	ContractIDType(1)
		real(c_double)		::	PreSettle
		real(c_double)		::	PreClose
		integer(c_int)		::	PreOpenInterest
		real(c_double)		::	OpenPrice
		real(c_double)		::	HighPrice
		real(c_double)		::	LowPrice
		real(c_double)		::	LastPrice
		real(c_double)		::	BidPrice(0:4)
		real(c_double)		::	AskPrice(0:4)
		integer(c_int)		::	BidLot(0:4)
		integer(c_int)		::	AskLot(0:4)
		integer(c_int)		::	TotalVolume
		integer(c_int)		::	OpenInterest
		real(c_double)		::	ClosePrice
		real(c_double)		::	SettlePrice
		real(c_double)		::	AveragePrice
		real(c_double)		::	LifeHigh
		real(c_double)		::	LifeLow
		real(c_double)		::	HighLimit
		real(c_double)		::	LowLimit
		integer(c_int)		::	TotalBidLot
		integer(c_int)		::	TotalAskLot
		character(c_char)	::	TimeStamp(24)!2014-02-03 13:23:45
	end type cStruct_zz_quote    
    
	type, bind(C)	::	cStruct_ctp_quote	
		character(c_char)	::	szTradingDay(9), szSettlementGroupID(9)
		integer(c_int)		::	nSettlementID
		real(c_double)		::	dLastPrice, dPreSettlementPrice, dPreClosePrice, dPreOpenInterest, dOpenPrice
		real(c_double)		::	dHighestPrice, dLowestPrice
		integer(c_int)		::	nVolume
		real(c_double)		::	dTurnover, dOpenInterest, dClosePrice, dSettlementPrice, dUpperLimitPrice, dLowerLimitPrice
		real(c_double)		::	dPreDelta, dCurrDelta
		character(c_char)	::	szUpdateTime(9)
		integer(c_int)		::	nUpdateMillisec
		character(c_char)	::	szInstrumentID(31)
		real(c_double)		::	dBidPrice1
		integer(c_int)		::	nBidVolume1
		real(c_double)		::	dAskPrice1
		integer(c_int)		::	nAskVolume1
		real(c_double)		::	dBidPrice2
		integer(c_int)		::	nBidVolume2
		real(c_double)		::	dAskPrice2
		integer(c_int)		::	nAskVolume2
		real(c_double)		::	dBidPrice3
		integer(c_int)		::	nBidVolume3
		real(c_double)		::	dAskPrice3
		integer(c_int)		::	nAskVolume3
		real(c_double)		::	dBidPrice4
		integer(c_int)		::	nBidVolume4
		real(c_double)		::	dAskPrice4
		integer(c_int)		::	nAskVolume4
		real(c_double)		::	dBidPrice5
		integer(c_int)		::	nBidVolume5
		real(c_double)		::	dAskPrice5
		integer(c_int)		::	nAskVolume5
		character(c_char)	::  ActionDay(9)
    end type cStruct_ctp_quote 
    
    
    !----------------------------------------------------
    ! fortran position data type
    !----------------------------------------------------	    
    type position_data
		character	::	cTicker*6
		integer		::	iMaxPOS, iPOSD, iPOSK, iYestPOSD, iYestPOSK, iPOSChg
		real(dp)	::	rAvailCash
    end type position_data    
    
     type myTSignal
		integer			::	iStID, iSigID,  iBP, iSP, iOpenV, iCloseV
		real(dp)		::	rBP, rSP
		character		::	cExchCode*1, cTicker*6
		integer			::	iSigAction, iSigOpenClose, iInstruction, iCancelSigID
    end type myTSignal 
    !----------------------------------------------------
    ! input data type
    !----------------------------------------------------	    
    type in_data
		character		::	cTime*9, cTicker*6, cSecType*1, cExchCode*1
		real(dp)		::	rYestP, rP, rV, rOI, rTotV, rPLmtUp, rPLmtDn, rPOpen, rPHi, rPLo, rPYestCl, rPClose
		real(dp)		::	rYestOI, rPMean, rTotVal, rVal, rPSettle
		real(dp)		::	rBP1, rBP2, rBP3, rBP4, rBP5, rSP1, rSP2, rSP3, rSP4, rSP5
		integer			::	iBV1, iBV2, iBV3, iBV4, iBV5, iSV1, iSV2, iSV3, iSV4, iSV5, iTime
		real(dp)        ::  rBP(30), rSP(30)
		integer         ::  iBV(30), iSV(30), iImpBV(30), iImpSV(30)
        integer         ::  iVol, iTotalBuyVol, iTotalSelVol
        real(dp)        ::  rAvgP, rOIChg, rWeightedBuyOrderP, rWeightedSelOrderP, rPLifeHigh, rPLifeLow
        integer         ::  iData_Flag ! for SHFE data
    end type in_data  

     type, bind(C) :: cStruct_TSignal
		integer(c_long)		::	sig_id, cancel_sig_id
		integer(c_int)		::	st_id 
		integer(c_int)		::	exch
		character(c_char)	::	ticker(64)
        integer(c_int)		::	open_volume
		real(c_double)		::	buy_price
        integer(c_int)		::	close_volume
		real(c_double)		::	sell_price
		integer(c_short)	::	sig_action, sig_openclose, instruction
     end type cStruct_TSignal 
     
   
    !----------------------------------------------------
    ! environment data structure
    !----------------------------------------------------	   
	type sec_parameter
		integer		::	iSecTradeStartTime, iSecTradeEndTime, iSecOpenEndTime, iSecSqTime, iSecEndTime
		integer		::	iUnitPerContract, iWrongVol, iPMultiplier, iTickSizeMulti, iDecimalPt 
		integer		::	iMinTradeSec, iWithCancel, iLookBackRecordNo
		real(dp)	::	rTickSize, rTVCompRatio
        real(dp)	::	rRssTCostByVol, rRssTCostByVal
    end type sec_parameter
    
	type exch_parameter
		integer		::	iVolDivider, iPInLevel, iMktStartTime, iMktEndTime
    end type exch_parameter
 !     
    type hi5_parameter
        real(dp)			::	rMKTThreshold, rOpenThreshold, rCloseThreshold    
    end type hi5_parameter
    
   type all_parameter
		type(exch_parameter)	::	stExch
		type(sec_parameter)		::	stSec
        type(hi5_parameter)     ::  stHi5Para
   end type all_parameter
   
   
    !----------------------------------------------------
    ! structure to store information in trading
    !----------------------------------------------------      
    type trade_info
        integer :: iTargetPos, iPos, iSignPos, iVol2Pass, iTradeTickNo, iTradeTime, iSigID, iSignalType, iLatestSigID
        real(dp) :: rTradeBP, rTradeSP
        integer :: iBV1Sum, iSV1Sum
        real(dp) :: rPnl, rTotalPnL, rCash,  rVround, rLivePL,rGrossPnl, rGrossCash, rAmount
        real(dp) ::	rTradeCost, rTotalCost, rWinPt, rLossPt, rPrevTradeEndCash
        integer :: iRounds, iVol2Trade, iLastTradeSize, iNoWin, iNoLoss, iTotalTradeSize
        real(dp) :: rOrderP, rRecOrderP
        integer :: iToTrade, iOpenSize, iCloseSize, iTS, iNewPosStatus
         integer :: iOrderSize, iTotalOrderSize, iTotalCancelSize, iCancelFlg, iContinousFlg
    end type trade_info   
   
	type input_parameter
        integer     :: iInputTradeStartTime, iInputSecSqTime, iInputOpenEndTime
        real(dp)    :: rInputPara1, rInputPara2, rInputPara3
        real(dp)	:: rInputCostByVol, rInputCostByVal
    end type input_parameter      
 
    !----------------------------------------------------
    ! strat_out_log output data type
    !----------------------------------------------------	    
    type,bind(C)  :: strat_out_log
        
        integer(c_int)		:: exch_time
        character(c_char)	:: contract(16)
        integer(c_int)		:: n_tick
        real(c_double)		:: price
        integer(c_int)		:: vol,bv1
        real(c_double)		:: bp1,sp1
        integer(c_int)		:: sv1
        integer(c_long)		:: amt,oi
        real(c_double)		:: buy_price,sell_price
        integer(c_int)		:: open_vol,close_vol,long_pos,short_pos,tot_ordervol,tot_cancelvol,order_cnt,cancel_cnt
        real(c_double)		:: cash,live
        integer(c_int)		:: tot_vol
        real(c_double)		:: max_dd
        integer(c_int)		:: max_net_pos,max_side_pos
        real(c_double)		:: sig(12)
        
    end type strat_out_log  
    
    !----------------------------------------------------
    ! Following interface structure for system 3.0, begin 
    !----------------------------------------------------	
	type, bind(C)	::	cStruct_symbol_config
		integer(c_int)		::	exch
		character(c_char)	::	ticker(64)
		integer(c_int)		::	max_pos
		integer(c_int)		::	ticker_log_id
		character(c_char)	::	ticker_log_name(256)
    end type cStruct_symbol_config  
    
	type, bind(C)	::	cStruct_strategy_config
		integer(c_int)		::	strategy_id
		character(c_char)	::	strategy_name(64)
		integer(c_int)		::	log_id
		character(c_char)	::	log_name(256)
		integer(c_int)		::	intvariable_id
		character(c_char)	::	intvariable_name(256)
		integer(c_int)		::	extvariable_id
		character(c_char)	::	extvariable_name(256)
		integer(c_int)		::	ticker_count
		type(cStruct_symbol_config)	::	ticker_detail(0:4)        
    end type cStruct_strategy_config
    
	type, bind(C)	::	cStruct_symbol_pos
		character(c_char)	::	ticker(64)
		integer(c_int)		::	long_volume
		real(c_double)		::	long_price
		integer(c_int)		::	short_volume
		real(c_double)		::	short_price
		integer(c_int)		::	pos_change
        character(c_char)   ::  exchg_code
    end type cStruct_symbol_pos
    
	type, bind(C)	::	cStruct_position
		integer(c_int)		::	count
		type(cStruct_symbol_pos)	::	pos(0:9)        
    end type cStruct_position
    
    
	type, bind(C)	::	cStruct_init_position
		character(c_char)	:: st_name(64)
		type(cStruct_position)	::	cur_pos
    end type cStruct_init_position  
    
	type, bind(C)	:: cStruct_signal_resp_t
		integer(c_long)		::	sig_id
		character(c_char)	::	ticker(64)
		integer(c_short)	::	sig_act
		integer(c_int)		::	order_volume
		real(c_double)		::	order_price
		real(c_double)		::	exe_price
		integer(c_int)		::	exe_volume
		real(c_double)		::	avg_price
		integer(c_int)		::	cum_volume
		integer(c_int)		::	status
		integer(c_int)		::	ack
		integer(c_int)		::	cancelled
		integer(c_int)		::	rejected
		integer(c_int)		::	error_no
    end type cStruct_signal_resp_t    
    
    
    !----------------------------------------------------
    ! global variable definition
    !----------------------------------------------------    
	type(position_data), dimension(0:(iPOS_LEN - 1))			::	gastEachPOSArray  
    type(position_data)	::	gstPosData, gstPosDataYest, gstPosDataToday
     type (in_data) :: gstCurrIn, gstPreIn, gstBfIn, gstRecCurrIn , gstRecPreIn
    type (cStruct_gta_quote) ::	gstcStructCFFEIn  ! input IF C structure
    type (cStruct_dl_quote)  ::	gstcStructDCEIn   ! input DCE C structure
    type (cStruct_shfe_quote)  ::	gstcStructSHFEIn  ! input SHFE C structure
    type (cStruct_zz_quote)  ::	gstcStructCZCEIn  ! input CZCE C structure
    type (all_parameter)      ::    gstGlobalPara
    type (trade_info)         ::    gstCurrTradeInfo, gstPreTradeInfo
    type(myTSignal), dimension(0:(iSIGNAL_LEN - 1))	::	gstTSignal
    
    integer :: giNoTradeItemEachRun, giLb, giLen
        
    integer :: giFileNumber  ! output detailed log
    character :: gcFileName*512
    integer :: giWriteOutputFileFlg, giWriteOutputFileInitFlg
    integer :: giInnerPar1Flg = iYES, giInnerPar2Flg = iYES, giInnerPar3Flg = iYES 
    type (input_parameter)    ::    gstInputParas
    integer :: giSimuPara1, giSimuPara2, giSimuPara3
    integer :: giInitFlg
    type (strat_out_log)      ::    gstrat_out_log
    integer :: giTickNo
    integer :: giStID, giMaxPos, giInitPOS
    integer :: giFullOrderBookL, giFullOrderBookU
    integer, dimension(:), allocatable :: gaiFullOrderBookBuy, gaiFullOrderBookSel, gaiPreFullOrderBookBuy, gaiPreFullOrderBookSel, &
                                          gaiFullOrderBookBuyDiff, gaiFullOrderBookSelDiff  
    integer   :: giMaxTime,gidata_vaild=1
    
End Module Common_Var_Def    