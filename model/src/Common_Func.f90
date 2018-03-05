!************************************************************************
! Copyright (C), 2010-2090, MYCapital Asset Management Ltd.
! Create Data:  2014-12-20
! File Name:     Common_Func.f90
! Author:        Zhenyu WEI
! Description:  This file contains some small common functions and subroutines used in project
!**************************************************************************/
    
    
Module Common_Func
	use iso_c_binding
	use Common_Var_Def
	implicit none
    contains  
    
    
    !----------------------------------------------------
    ! subroutine s_read_position(lcTickerIn, lstcStructPositionIn, lstEachPOS)
    ! description: read position info from trading system, and convert info from C structure to Fortran structure
    ! input: lstcStructPositionIn
    !        lcTickerIn 
    ! output: lstEachPOS
    !----------------------------------------------------	    
	subroutine s_read_position(lcTickerIn, lstcStructPositionIn, lstEachPOS)
		implicit none
		character(len=*), intent(in)			::	lcTickerIn
	    type(cStruct_symbol_pos), intent(in)	::	lstcStructPositionIn
		type(position_data), intent(inout)		::	lstEachPOS
		integer						        	::	liN, liLB, liLen
		character				        		::	lcTicker*6
		
		lcTicker = f_convert_c_char_to_f_string(lstcStructPositionIn%ticker, 6)
		if (trim(lcTicker) == trim(lcTickerIn)) then
			lstEachPOS%cTicker = trim(lcTicker(1:6))
			lstEachPOS%iPOSD = lstcStructPositionIn%long_volume
			lstEachPOS%iPOSK = lstcStructPositionIn%short_volume
			lstEachPOS%iPOSChg = lstcStructPositionIn%pos_change
		end if
    end subroutine s_read_position    
    

     !----------------------------------------------------
    ! character*1 function f_int_2_cha1(liIn)
    ! desprition : convert 1-digit-int to char 
    !----------------------------------------------------        
	character*1 function f_int_2_cha1(liIn)
		implicit none
		integer, intent(in)	::	liIn
		character			::	lcIn*1
		
		lcIn = '0'
		if (liIn > 0) lcIn = char(mod(liIn,10)+48)
		f_int_2_cha1 = lcIn
    end function f_int_2_cha1   
    
    !----------------------------------------------------
    ! character*2 function f_int_2_cha2(liIn)
    ! desprition : convert 2-digit-int to char 
    !----------------------------------------------------        
	character*2 function f_int_2_cha2(liIn)
		implicit none
		integer, intent(in)	::	liIn
		character			::	lcIn*2
		integer				::	liN
		
		lcIn = '0   '
		if (int(liIn/10) > 0) then
			lcIn = f_int_2_cha1(int(liIn/10))//f_int_2_cha1(liIn)//'  '
			goto 999
		end if
		if (liIn > 0) then
			lcIn = '0'//f_int_2_cha1(liIn)//'  '
			goto 999
		end if
999		f_int_2_cha2 = lcIn
    end function f_int_2_cha2 
    !----------------------------------------------------
    ! character*4 function f_int_2_cha4(liIn)
    ! desprition : convert 4-digit-int to char 
    !----------------------------------------------------      
	character*4 function f_int_2_cha4(liIn)
		implicit none
		integer, intent(in)	::	liIn
		character			::	lcIn*4
		
		lcIn = '0   '
		if (int(liIn/1000) > 0) then
			lcIn = f_int_2_cha1(int(liIn/1000))//f_int_2_cha1(int(liIn/100))//f_int_2_cha1(int(liIn/10))//f_int_2_cha1(liIn)
			goto 999
		end if
		if (int(liIn/100) > 0) then
			lcIn = f_int_2_cha1(int(liIn/100))//f_int_2_cha1(int(liIn/10))//f_int_2_cha1(liIn)//' '
			goto 999
		end if
		if (int(liIn/10) > 0) then
			lcIn = f_int_2_cha1(int(liIn/10))//f_int_2_cha1(liIn)//'  '
			goto 999
		end if
		if (liIn > 0) then
			lcIn = f_int_2_cha1(liIn)//'   '
			goto 999
		end if
999		f_int_2_cha4 = lcIn
    end function f_int_2_cha4 
    
     !----------------------------------------------------
    ! real(dp) function f_calc_curr_avgp(lrVal, liV, lrPrevAvgP)
    ! desprition : calculate current average price
    !----------------------------------------------------      
	real(dp) function f_calc_curr_avgp(lrVal, liV, lrPrevAvgP)
		implicit none
		real(dp), intent(in)	::	lrVal, lrPrevAvgP
		integer, intent(in)		::	liV
		
		if (liV > iZERO .and. lrVal > rZERO) then
			f_calc_curr_avgp = lrVal / liV 
		else
			f_calc_curr_avgp = lrPrevAvgP * gstGlobalPara%stSec%iUnitPerContract 
		end if
    end function f_calc_curr_avgp  
    
   !----------------------------------------------------
    ! subroutine s_assign_out_tsignal(lastTSignal, liLBTSIn, liUBTSIn, liNoTradeItemEachRun, &
	!								lstcStructTSignal, liLB, liLen, licNoTSignal, liTickSizeMulti, liPMultiplier)
    ! description: sending signal according to C struture
    ! input: liNoTradeItemEachRun, liTickSizeMulti, liPMultiplier, liLBTSIn, liUBTSIn
    !        liLB, liLen
    !        lastTSignal : signai in Fortran structure
    ! output: lstcStructTSignal : signal info in C structure
    !         licNoTSignal : signal number
    !----------------------------------------------------  
 	subroutine s_assign_out_tsignal(lastTSignal, liLBTSIn, liUBTSIn, liNoTradeItemEachRun, &
									lstcStructTSignal, liLB, liLen, licNoTSignal, liTickSizeMulti, liPMultiplier)
		implicit none
		integer, intent(in)				::	liTickSizeMulti, liPMultiplier, liLBTSIn, liUBTSIn
		integer(c_int), intent(in)		::	liLB, liLen
		integer(c_int), intent(inout)	::	licNoTSignal
        integer, intent(inout)				::	liNoTradeItemEachRun 
		type(myTSignal), dimension(liLBTSIn:liUBTSIn), intent(in)	::	lastTSignal
		type(cStruct_TSignal), dimension(liLB:(liLB + liLen - 1)), intent(inout)	::	lstcStructTSignal
		integer				::	liN, liM

		licNoTSignal = liNoTradeItemEachRun
		if (liNoTradeItemEachRun == iZERO) return
		liM = iZERO
        liN = iZERO
        !write(giFileNumber,"(a3,i12,1x,i3,1x,i3)")    'a9',gstCurrIn%iTime,liLB,liNoTradeItemEachRun  		
        if (liNoTradeItemEachRun >3) liNoTradeItemEachRun=3
		forall (liN = liLB:(liLB + liNoTradeItemEachRun - 1))
			lstcStructTSignal(liN)%exch = ichar(lastTSignal(liN)%cExchCode(1:1))
			lstcStructTSignal(liN)%sig_id = lastTSignal(liN)%iSigID; lstcStructTSignal(liN)%st_id = lastTSignal(liN)%iStID
			lstcStructTSignal(liN)%open_volume = lastTSignal(liN)%iOpenV
			lstcStructTSignal(liN)%close_volume = lastTSignal(liN)%iCloseV
			lstcStructTSignal(liN)%sig_action = lastTSignal(liN)%iSigAction
            lstcStructTSignal(liN)%sig_openclose = lastTSignal(liN)%iSigOpenClose
			lstcStructTSignal(liN)%instruction = lastTSignal(liN)%iInstruction
			lstcStructTSignal(liN)%cancel_sig_id = lastTSignal(liN)%iCancelSigID
            lstcStructTSignal(liN)%buy_price = lastTSignal(liN)%rBP
            lstcStructTSignal(liN)%sell_price = lastTSignal(liN)%rSP
        end forall
        do liN = liLB, (liLB + liNoTradeItemEachRun - 1), 1 
            lstcStructTSignal(liN)%ticker = f_convert_f_string_to_c_char(trim(lastTSignal(liN)%cTicker), len_trim(lastTSignal(liN)%cTicker), &
					lbound(lstcStructTSignal(liN)%ticker, 1), ubound(lstcStructTSignal(liN)%ticker, 1))  
        end do  
         !write(giFileNumber,"(a4,1x,i3,1x,i3)")    'a10',liLB,liNoTradeItemEachRun  		
    end subroutine s_assign_out_tsignal  
    
    !----------------------------------------------------
    ! subroutine s_read_ctp_fut_struct(lstcCtpQuoteIn, lstInData)
    ! description: convert C structure to Fortran structure for CZCE data
    ! input: lstcGtaQuoteIn : CZCE data in C structure
    ! output: lstInData     : CZCE data in Fortran structure
    !----------------------------------------------------                                        
	subroutine s_read_ctp_fut_struct(lstcCtpQuoteIn, lstInData)
		implicit none
		type(cStruct_ctp_quote), intent(in)	::	lstcCtpQuoteIn
        type(in_data), intent(inout)		::	lstInData
        character		::	lcTemp*9
		lstInData%rP = lstcCtpQuoteIn%dLastPrice; lstInData%rYestP = lstcCtpQuoteIn%dPreSettlementPrice
		lstInData%rPYestCl = lstcCtpQuoteIn%dPreClosePrice; lstInData%rYestOI = lstcCtpQuoteIn%dPreOpenInterest
		lstInData%rPOpen = lstcCtpQuoteIn%dOpenPrice; lstInData%rPHi = lstcCtpQuoteIn%dHighestPrice
		lstInData%rPLo = lstcCtpQuoteIn%dLowestPrice; lstInData%rTotV = lstcCtpQuoteIn%nVolume
		lstInData%rTotVal = lstcCtpQuoteIn%dTurnover; lstInData%rOI = lstcCtpQuoteIn%dOpenInterest
		lstInData%rPClose = lstcCtpQuoteIn%dClosePrice; lstInData%rPSettle = lstcCtpQuoteIn%dSettlementPrice
		lstInData%rPLmtUp = lstcCtpQuoteIn%dUpperLimitPrice; lstInData%rPLmtDn = lstcCtpQuoteIn%dLowerLimitPrice
                        
		lstInData%cTime = f_convert_c_char_to_f_string(lstcCtpQuoteIn%szUpdateTime, ubound(lstcCtpQuoteIn%szUpdateTime, 1))
        lcTemp = lstInData%cTime 
        lstInData%cTime  = ''
		!s_inData%strTime = s_inData%strTime(:2)//s_inData%strTime(4:5)//s_inData%strTime(7:8)
        lstInData%cTime(1:2) = lcTemp(1:2)
        lstInData%cTime(3:4) = lcTemp(4:5)
        lstInData%cTime(5:6) = lcTemp(7:8)
		!read (s_inData%strTime, '(I12)') s_inData%iTime
        lstInData%iTime = cha2int(lstInData%cTime(1:6))
		lstInData%iTime = lstInData%iTime * 1000 + lstcCtpQuoteIn%nUpdateMillisec  
        if (lstInData%iTime < 30000000) lstInData%iTime = lstInData%iTime + 240000000
        
		lstInData%cTicker = f_convert_c_char_to_f_string(lstcCtpQuoteIn%szInstrumentID, 6)        

		lstInData%rBP1 = lstcCtpQuoteIn%dBidPrice1; lstInData%iBV1 = lstcCtpQuoteIn%nBidVolume1
		lstInData%rSP1 = lstcCtpQuoteIn%dAskPrice1; lstInData%iSV1 = lstcCtpQuoteIn%nAskVolume1 
		lstInData%rBP2 = lstcCtpQuoteIn%dBidPrice2; lstInData%iBV2 = lstcCtpQuoteIn%nBidVolume2
		lstInData%rSP2 = lstcCtpQuoteIn%dAskPrice2; lstInData%iSV2 = lstcCtpQuoteIn%nAskVolume2
		lstInData%rBP3 = lstcCtpQuoteIn%dBidPrice3; lstInData%iBV3 = lstcCtpQuoteIn%nBidVolume3
		lstInData%rSP3 = lstcCtpQuoteIn%dAskPrice3; lstInData%iSV3 = lstcCtpQuoteIn%nAskVolume3
		lstInData%rBP4 = lstcCtpQuoteIn%dBidPrice4; lstInData%iBV4 = lstcCtpQuoteIn%nBidVolume4
		lstInData%rSP4 = lstcCtpQuoteIn%dAskPrice4; lstInData%iSV4 = lstcCtpQuoteIn%nAskVolume4
		lstInData%rBP5 = lstcCtpQuoteIn%dBidPrice5; lstInData%iBV5 = lstcCtpQuoteIn%nBidVolume5
		lstInData%rSP5 = lstcCtpQuoteIn%dAskPrice5; lstInData%iSV5 = lstcCtpQuoteIn%nAskVolume5     
   

		lstInData%cExchCode = cCZCE; lstInData%cSecType = cFUT; lstInData%rV = rZERO
		lstInData%rVal = rZERO; lstInData%rPMean = rZERO
        lstInData%iImpBV = 0; lstInData%iImpSV = 0
		lstInData%rBP(1) = lstInData%rBP1; lstInData%rBP(2) = lstInData%rBP2; lstInData%rBP(3) = lstInData%rBP3; lstInData%rBP(4) = lstInData%rBP4; lstInData%rBP(5) = lstInData%rBP5;
		lstInData%rSP(1) = lstInData%rSP1; lstInData%rSP(2) = lstInData%rSP2; lstInData%rSP(3) = lstInData%rSP3; lstInData%rSP(4) = lstInData%rSP4;	lstInData%rSP(5) = lstInData%rSP5;
		lstInData%iBV(1) = lstInData%iBV1; lstInData%iBV(2) = lstInData%iBV2; lstInData%iBV(3) = lstInData%iBV3; lstInData%iBV(4) = lstInData%iBV4; lstInData%iBV(5) = lstInData%iBV5;
		lstInData%iSV(1) = lstInData%iSV1; lstInData%iSV(2) = lstInData%iSV2; lstInData%iSV(3) = lstInData%iSV3; lstInData%iSV(4) = lstInData%iSV4;	lstInData%iSV(5) = lstInData%iSV5;  	
    end subroutine s_read_ctp_fut_struct

    !----------------------------------------------------
    ! subroutine s_read_zz_fut_struct(lstcZZQuoteIn, lstInData)
    ! description: convert C structure to Fortran structure for CZCE data
    ! input: lstcZZQuoteIn : CZCE data in C structure
    ! output: lstInData     : CZCE data in Fortran structure
    !----------------------------------------------------                                        
	subroutine s_read_zz_fut_struct(lstcZZQuoteIn, lstInData)
		implicit none
		type(cStruct_zz_quote), intent(in)	::	lstcZZQuoteIn
        type(in_data), intent(inout)		::	lstInData
        character		::	lcTemp*13, lcTemp1*25
		lstInData%rP = lstcZZQuoteIn%LastPrice; lstInData%rYestP = lstcZZQuoteIn%PreSettle
		lstInData%rPYestCl = lstcZZQuoteIn%PreClose; lstInData%rYestOI = lstcZZQuoteIn%PreOpenInterest * 1._dp
		lstInData%rPOpen = lstcZZQuoteIn%OpenPrice; lstInData%rPHi = lstcZZQuoteIn%HighPrice
		lstInData%rPLo = lstcZZQuoteIn%LowPrice; lstInData%rTotV = lstcZZQuoteIn%TotalVolume * 1._dp
		lstInData%rTotVal = rZERO; lstInData%rOI = lstcZZQuoteIn%OpenInterest * 1._dp
		lstInData%rPClose = lstcZZQuoteIn%ClosePrice; lstInData%rPSettle = lstcZZQuoteIn%SettlePrice
		lstInData%rPLmtUp = lstcZZQuoteIn%HighLimit; lstInData%rPLmtDn = lstcZZQuoteIn%LowLimit
		lstInData%cExchCode = cCZCE; lstInData%cSecType = cFUT; lstInData%rV = rZERO
		lstInData%rVal = rZERO; lstInData%rPMean = lstcZZQuoteIn%AveragePrice 

        lstInData%iTotalBuyVol = lstcZZQuoteIn%TotalBidLot
        lstInData%iTotalSelVol = lstcZZQuoteIn%TotalAskLot
        lstInData%rWeightedBuyOrderP = rZERO
        lstInData%rWeightedSelOrderP = rZERO    

		lstInData%rPLifeHigh = lstcZZQuoteIn%LifeHigh; lstInData%rPLifeLow = lstcZZQuoteIn%LifeLow
        
		lcTemp1 = f_convert_c_char_to_f_string(lstcZZQuoteIn%TimeStamp, ubound(lstcZZQuoteIn%TimeStamp, 1))
        lcTemp = lcTemp1(12:24) 
        lstInData%cTime  = ''
        if(lcTemp(9:9) == ".") then
           lstInData%cTime(1:2) = lcTemp(1:2)
           lstInData%cTime(3:4) = lcTemp(4:5)
           lstInData%cTime(5:6) = lcTemp(7:8)
           lstInData%cTime(7:9) = lcTemp(10:12) 
          !s_inData%strTime = s_strTime(:2)//s_strTime(4:5)//s_strTime(7:8)//s_strTime(10:12)
        else 
           lstInData%cTime(1:2) = lcTemp(1:2)
           lstInData%cTime(3:4) = lcTemp(4:5)
           lstInData%cTime(5:6) = lcTemp(7:8)
           lstInData%cTime(7:9) = '000'   
		  !s_inData%strTime = s_strTime(:2)//s_strTime(4:5)//s_strTime(7:8)//'000'
        end if
        !read (s_inData%strTime, '(I12)') s_inData%iTime
        lstInData%iTime= cha2int(lstInData%cTime(1:9))  
        if (lstInData%iTime < 30000000) lstInData%iTime = lstInData%iTime + 240000000
		lstInData%cTicker = f_convert_c_char_to_f_string(lstcZZQuoteIn%ContractID, 6)        

		lstInData%rBP1 = lstcZZQuoteIn%BidPrice(0); lstInData%iBV1 = lstcZZQuoteIn%BidLot(0)
		lstInData%rSP1 = lstcZZQuoteIn%AskPrice(0); lstInData%iSV1 = lstcZZQuoteIn%AskLot(0) 
		lstInData%rBP2 = lstcZZQuoteIn%BidPrice(1); lstInData%iBV2 = lstcZZQuoteIn%BidLot(1)
		lstInData%rSP2 = lstcZZQuoteIn%AskPrice(1); lstInData%iSV2 = lstcZZQuoteIn%AskLot(1)
		lstInData%rBP3 = lstcZZQuoteIn%BidPrice(2); lstInData%iBV3 = lstcZZQuoteIn%BidLot(2)
		lstInData%rSP3 = lstcZZQuoteIn%AskPrice(2); lstInData%iSV3 = lstcZZQuoteIn%AskLot(2)
		lstInData%rBP4 = lstcZZQuoteIn%BidPrice(3); lstInData%iBV4 = lstcZZQuoteIn%BidLot(3)
		lstInData%rSP4 = lstcZZQuoteIn%AskPrice(3); lstInData%iSV4 = lstcZZQuoteIn%AskLot(3)
		lstInData%rBP5 = lstcZZQuoteIn%BidPrice(4); lstInData%iBV5 = lstcZZQuoteIn%BidLot(4)
		lstInData%rSP5 = lstcZZQuoteIn%AskPrice(4); lstInData%iSV5 = lstcZZQuoteIn%AskLot(4)     
   


        lstInData%iImpBV = 0; lstInData%iImpSV = 0
		lstInData%rBP(1) = lstInData%rBP1; lstInData%rBP(2) = lstInData%rBP2; lstInData%rBP(3) = lstInData%rBP3; lstInData%rBP(4) = lstInData%rBP4; lstInData%rBP(5) = lstInData%rBP5;
		lstInData%rSP(1) = lstInData%rSP1; lstInData%rSP(2) = lstInData%rSP2; lstInData%rSP(3) = lstInData%rSP3; lstInData%rSP(4) = lstInData%rSP4;	lstInData%rSP(5) = lstInData%rSP5;
		lstInData%iBV(1) = lstInData%iBV1; lstInData%iBV(2) = lstInData%iBV2; lstInData%iBV(3) = lstInData%iBV3; lstInData%iBV(4) = lstInData%iBV4; lstInData%iBV(5) = lstInData%iBV5;
		lstInData%iSV(1) = lstInData%iSV1; lstInData%iSV(2) = lstInData%iSV2; lstInData%iSV(3) = lstInData%iSV3; lstInData%iSV(4) = lstInData%iSV4;	lstInData%iSV(5) = lstInData%iSV5;  	
    end subroutine s_read_zz_fut_struct    



    !----------------------------------------------------
    ! subroutine s_read_gta_fut_struct(lstcGtaQuoteIn, lstInData)
    ! description: convert C structure to Fortran structure for IF data
    ! input: lstcGtaQuoteIn : IF data in C structure
    ! output: lstInData     : IF data in Fortran structure
    !----------------------------------------------------                                        
	subroutine s_read_gta_fut_struct(lstcGtaQuoteIn, lstInData)
		implicit none
		type(cStruct_gta_quote), intent(in)	::	lstcGtaQuoteIn
        type(in_data), intent(inout)		::	lstInData
        character		::	lcTime*9
        
		lstInData%rP = lstcGtaQuoteIn%dLastPrice; lstInData%rYestP = lstcGtaQuoteIn%dPreSettlementPrice
		lstInData%rPYestCl = lstcGtaQuoteIn%dPreClosePrice; lstInData%rYestOI = lstcGtaQuoteIn%dPreOpenInterest
		lstInData%rPOpen = lstcGtaQuoteIn%dOpenPrice; lstInData%rPHi = lstcGtaQuoteIn%dHighestPrice
		lstInData%rPLo = lstcGtaQuoteIn%dLowestPrice; lstInData%rTotV = lstcGtaQuoteIn%nVolume
		lstInData%rTotVal = lstcGtaQuoteIn%dTurnover; lstInData%rOI = lstcGtaQuoteIn%dOpenInterest
		lstInData%rPClose = lstcGtaQuoteIn%dClosePrice; lstInData%rPSettle = lstcGtaQuoteIn%dSettlementPrice
		lstInData%rPLmtUp = lstcGtaQuoteIn%dUpperLimitPrice; lstInData%rPLmtDn = lstcGtaQuoteIn%dLowerLimitPrice
		lcTime = f_convert_c_char_to_f_string(lstcGtaQuoteIn%szUpdateTime, ubound(lstcGtaQuoteIn%szUpdateTime, 1))
		lstInData%cTime(1:2) = lcTime(1:2)
        lstInData%cTime(3:4) = lcTime(4:5)
        lstInData%cTime(5:6) = lcTime(7:8)
        !lstInData%cTime = lstInData%cTime(:2)//lstInData%cTime(4:5)//lstInData%cTime(7:8)
		!read (lstInData%cTime, '(I12)') lstInData%iTime
        lstInData%iTime= cha2int(lstInData%cTime(1:6))
		lstInData%iTime = lstInData%iTime * 1000 + lstcGtaQuoteIn%nUpdateMillisec
        if (lstInData%iTime < 30000000) lstInData%iTime = lstInData%iTime + 240000000
		lstInData%cTicker = f_convert_c_char_to_f_string(lstcGtaQuoteIn%szInstrumentID, 6)
		lstInData%rBP1 = lstcGtaQuoteIn%dBidPrice1; lstInData%iBV1 = lstcGtaQuoteIn%nBidVolume1
		lstInData%rSP1 = lstcGtaQuoteIn%dAskPrice1; lstInData%iSV1 = lstcGtaQuoteIn%nAskVolume1
		lstInData%rBP2 = lstcGtaQuoteIn%dBidPrice2; lstInData%iBV2 = lstcGtaQuoteIn%nBidVolume2
		lstInData%rSP2 = lstcGtaQuoteIn%dAskPrice2; lstInData%iSV2 = lstcGtaQuoteIn%nAskVolume2
		lstInData%rBP3 = lstcGtaQuoteIn%dBidPrice3; lstInData%iBV3 = lstcGtaQuoteIn%nBidVolume3
		lstInData%rSP3 = lstcGtaQuoteIn%dAskPrice3; lstInData%iSV3 = lstcGtaQuoteIn%nAskVolume3
		lstInData%rBP4 = lstcGtaQuoteIn%dBidPrice4; lstInData%iBV4 = lstcGtaQuoteIn%nBidVolume4
		lstInData%rSP4 = lstcGtaQuoteIn%dAskPrice4; lstInData%iSV4 = lstcGtaQuoteIn%nAskVolume4
		lstInData%rBP5 = lstcGtaQuoteIn%dBidPrice5; lstInData%iBV5 = lstcGtaQuoteIn%nBidVolume5
		lstInData%rSP5 = lstcGtaQuoteIn%dAskPrice5; lstInData%iSV5 = lstcGtaQuoteIn%nAskVolume5
		lstInData%cExchCode = cCFFE; lstInData%cSecType = cFUT; lstInData%rV = rZERO
		lstInData%rVal = rZERO; lstInData%rPMean = rZERO
        
 		lstInData%rBP(1) = lstInData%rBP1; lstInData%rBP(2) = lstInData%rBP2; lstInData%rBP(3) = lstInData%rBP3; lstInData%rBP(4) = lstInData%rBP4; lstInData%rBP(5) = lstInData%rBP5;
		lstInData%rSP(1) = lstInData%rSP1; lstInData%rSP(2) = lstInData%rSP2; lstInData%rSP(3) = lstInData%rSP3; lstInData%rSP(4) = lstInData%rSP4;	lstInData%rSP(5) = lstInData%rSP5;
		lstInData%iBV(1) = lstInData%iBV1; lstInData%iBV(2) = lstInData%iBV2; lstInData%iBV(3) = lstInData%iBV3; lstInData%iBV(4) = lstInData%iBV4; lstInData%iBV(5) = lstInData%iBV5;
		lstInData%iSV(1) = lstInData%iSV1; lstInData%iSV(2) = lstInData%iSV2; lstInData%iSV(3) = lstInData%iSV3; lstInData%iSV(4) = lstInData%iSV4;	lstInData%iSV(5) = lstInData%iSV5;  
        lstInData%iImpBV = 0; lstInData%iImpSV = 0
    end subroutine s_read_gta_fut_struct    
    !----------------------------------------------------
    ! subroutine s_read_dl_fut_struct(lstcDLQuoteIn, lstInData)
    ! description: convert C structure to Fortran structure for DCE data
    ! input: lstcDLQuoteIn : DCE data in C structure
    ! output: lstInData     : DCE data in Fortran structure
    !----------------------------------------------------      
	subroutine s_read_dl_fut_struct(lstcDLQuoteIn, lstInData)
		implicit none
		type(cStruct_dl_quote), intent(in)	::	lstcDLQuoteIn
        type(in_data), intent(inout)		::	lstInData
        character(len=13)					::	lcTime

		lstInData%cExchCode = cDCE; lstInData%cSecType = cFUT
		lstInData%cTicker = f_convert_c_char_to_f_string(lstcDLQuoteIn%cContract, 6)
		lcTime = f_convert_c_char_to_f_string(lstcDLQuoteIn%cGenTime, ubound(lstcDLQuoteIn%cGenTime, 1))
        
        lstInData%cTime = ''
        if(lcTime(9:9) == ".") then
           lstInData%cTime(1:2) = lcTime(1:2)
           lstInData%cTime(3:4) = lcTime(4:5)
           lstInData%cTime(5:6) = lcTime(7:8)
           lstInData%cTime(7:9) = lcTime(10:12) 
          !s_inData%strTime = s_strTime(:2)//s_strTime(4:5)//s_strTime(7:8)//s_strTime(10:12)
        else 
           lstInData%cTime(1:2) = lcTime(1:2)
           lstInData%cTime(3:4) = lcTime(4:5)
           lstInData%cTime(5:6) = lcTime(7:8)
           lstInData%cTime(7:9) = '000'   
		  !s_inData%strTime = s_strTime(:2)//s_strTime(4:5)//s_strTime(7:8)//'000'
        end if
        !read (s_inData%strTime, '(I12)') s_inData%iTime
        lstInData%iTime= cha2int(lstInData%cTime(1:9))
        if (lstInData%iTime < 30000000) lstInData%iTime = lstInData%iTime + 240000000
		lstInData%rYestP = lstcDLQuoteIn%rLastClearPrice; lstInData%rP = lstcDLQuoteIn%rLastPrice
		lstInData%rV = lstcDLQuoteIn%iLastMatchQty * 1._dp; lstInData%rOI = lstcDLQuoteIn%iOpenInterest * 1._dp
		lstInData%rTotV = lstcDLQuoteIn%iMatchTotQty * 1._dp
		lstInData%rPLmtUp = lstcDLQuoteIn%rRiseLimit; lstInData%rPLmtDn = lstcDLQuoteIn%rFallLimit
		lstInData%rBP1 = lstcDLQuoteIn%rBuyPriceOne; lstInData%rBP2 = lstcDLQuoteIn%rBuyPriceTwo
		lstInData%rBP3 = lstcDLQuoteIn%rBuyPriceThree; lstInData%rBP4 = lstcDLQuoteIn%rBuyPriceFour
		lstInData%rBP5 = lstcDLQuoteIn%rBuyPriceFive; lstInData%rSP1 = lstcDLQuoteIn%rSellPriceOne
		lstInData%rSP2 = lstcDLQuoteIn%rSellPriceTwo; lstInData%rSP3 = lstcDLQuoteIn%rSellPriceThree
		lstInData%rSP4 = lstcDLQuoteIn%rSellPriceFour; lstInData%rSP5 = lstcDLQuoteIn%rSellPriceFive
		lstInData%iBV1 = lstcDLQuoteIn%iBuyQtyOne; lstInData%iBV2 = lstcDLQuoteIn%iBuyQtyTwo
		lstInData%iBV3 = lstcDLQuoteIn%iBuyQtyThree; lstInData%iBV4 = lstcDLQuoteIn%iBuyQtyFour
		lstInData%iBV5 = lstcDLQuoteIn%iBuyQtyFive; lstInData%iSV1 = lstcDLQuoteIn%iSellQtyOne
		lstInData%iSV2 = lstcDLQuoteIn%iSellQtyTwo; lstInData%iSV3 = lstcDLQuoteIn%iSellQtyThree
		lstInData%iSV4 = lstcDLQuoteIn%iSellQtyFour; lstInData%iSV5 = lstcDLQuoteIn%iSellQtyFive
		lstInData%rPOpen = lstcDLQuoteIn%rOpenPrice; lstInData%rPHi = lstcDLQuoteIn%rHighPrice
		lstInData%rPLo = lstcDLQuoteIn%rLowPrice; lstInData%rPYestCl = lstcDLQuoteIn%rLastClose
		lstInData%rYestOI = lstcDLQuoteIn%iLastOpenInterest * 1._dp; lstInData%rPMean = lstcDLQuoteIn%rAvgPrice
		lstInData%rTotVal = lstcDLQuoteIn%rTurnover; lstInData%rVal = rZERO
        
		lstInData%rBP(1) = lstInData%rBP1; lstInData%rBP(2) = lstInData%rBP2; lstInData%rBP(3) = lstInData%rBP3; lstInData%rBP(4) = lstInData%rBP4; lstInData%rBP(5) = lstInData%rBP5;
		lstInData%rSP(1) = lstInData%rSP1; lstInData%rSP(2) = lstInData%rSP2; lstInData%rSP(3) = lstInData%rSP3; lstInData%rSP(4) = lstInData%rSP4;	lstInData%rSP(5) = lstInData%rSP5;
		lstInData%iBV(1) = lstInData%iBV1; lstInData%iBV(2) = lstInData%iBV2; lstInData%iBV(3) = lstInData%iBV3; lstInData%iBV(4) = lstInData%iBV4; lstInData%iBV(5) = lstInData%iBV5;
		lstInData%iSV(1) = lstInData%iSV1; lstInData%iSV(2) = lstInData%iSV2; lstInData%iSV(3) = lstInData%iSV3; lstInData%iSV(4) = lstInData%iSV4;	lstInData%iSV(5) = lstInData%iSV5;
        lstInData%iImpBV(1) = lstcDLQuoteIn%iBuyImplyQtyOne; lstInData%iImpBV(2) = lstcDLQuoteIn%iBuyImplyQtyTwo; 
        lstInData%iImpBV(3) = lstcDLQuoteIn%iBuyImplyQtyThree; lstInData%iImpBV(4) = lstcDLQuoteIn%iBuyImplyQtyFour; lstInData%iImpBV(5) = lstcDLQuoteIn%iBuyImplyQtyFive; 
        lstInData%iImpSV(1) = lstcDLQuoteIn%iSellImplyQtyOne; lstInData%iImpSV(2) = lstcDLQuoteIn%iSellImplyQtyTwo; 
        lstInData%iImpSV(3) = lstcDLQuoteIn%iSellImplyQtyThree; lstInData%iImpSV(4) = lstcDLQuoteIn%iSellImplyQtyFour; lstInData%iImpSV(5) = lstcDLQuoteIn%iSellImplyQtyFive; 
        
       
    end subroutine s_read_dl_fut_struct

	subroutine s_read_cstruct_dl_orderstat(lstcStructDLOrderStatIn,  lstInData)	
	    implicit none
	    type(cStruct_dl_orderstat), intent(in)		::	lstcStructDLOrderStatIn
        type(in_data), intent(inout)		::	lstInData
        lstInData%iTotalBuyVol = lstcStructDLOrderStatIn%iTotalBuyOrderNum
        lstInData%iTotalSelVol = lstcStructDLOrderStatIn%iTotalSellOrderNum
        lstInData%rWeightedBuyOrderP = lstcStructDLOrderStatIn%rWeightedAverageBuyOrderPrice
        lstInData%rWeightedSelOrderP = lstcStructDLOrderStatIn%rWeightedAverageSellOrderPrice
   
	end subroutine s_read_cstruct_dl_orderstat    
    !----------------------------------------------------
    ! subroutine s_read_sh_fut_struct(lstcDLQuoteIn, lstInData)
    ! description: convert C structure to Fortran structure for SHFE data
    ! input: lstcDLQuoteIn : SHFE data in C structure
    ! output: lstInData     : SHFE data in Fortran structure
    !----------------------------------------------------     
	subroutine s_read_sh_fut_struct(lstcSHQuoteIn, lstInData)
		implicit none
		type(cStruct_shfe_quote), intent(in)	::	lstcSHQuoteIn
        type(in_data), intent(inout)		::	lstInData
        character		::	lcTemp*9
        integer         ::  liI
        
		lstInData%rP = lstcSHQuoteIn%dLastPrice; lstInData%rYestP = lstcSHQuoteIn%dPreSettlementPrice
		lstInData%rPYestCl = lstcSHQuoteIn%dPreClosePrice; lstInData%rYestOI = lstcSHQuoteIn%dPreOpenInterest
		lstInData%rPOpen = lstcSHQuoteIn%dOpenPrice; lstInData%rPHi = lstcSHQuoteIn%dHighestPrice
		lstInData%rPLo = lstcSHQuoteIn%dLowestPrice; lstInData%rTotV = lstcSHQuoteIn%nVolume
		lstInData%rTotVal = lstcSHQuoteIn%dTurnover; lstInData%rOI = lstcSHQuoteIn%dOpenInterest
		lstInData%rPClose = lstcSHQuoteIn%dClosePrice; lstInData%rPSettle = lstcSHQuoteIn%dSettlementPrice
		lstInData%rPLmtUp = lstcSHQuoteIn%dUpperLimitPrice; lstInData%rPLmtDn = lstcSHQuoteIn%dLowerLimitPrice
                        
		lstInData%cTime = f_convert_c_char_to_f_string(lstcSHQuoteIn%szUpdateTime, ubound(lstcSHQuoteIn%szUpdateTime, 1))
        lcTemp = lstInData%cTime 
        lstInData%cTime  = ''
		!s_inData%strTime = s_inData%strTime(:2)//s_inData%strTime(4:5)//s_inData%strTime(7:8)
        lstInData%cTime(1:2) = lcTemp(1:2)
        lstInData%cTime(3:4) = lcTemp(4:5)
        lstInData%cTime(5:6) = lcTemp(7:8)
		!read (s_inData%strTime, '(I12)') s_inData%iTime
        lstInData%iTime = cha2int(lstInData%cTime(1:6))
		lstInData%iTime = lstInData%iTime * 1000 + lstcSHQuoteIn%nUpdateMillisec     
        if (lstInData%iTime < 30000000) lstInData%iTime = lstInData%iTime + 240000000
        
		lstInData%cTicker = f_convert_c_char_to_f_string(lstcSHQuoteIn%szInstrumentID, 6)    
        if (abs(lstInData%rBP1 - lstcSHQuoteIn%dBidPrice1) > 0.001 .or. abs(lstInData%rSP1 - lstcSHQuoteIn%dAskPrice1) > 0.001) lstInData%iData_Flag  = 2
        if (lstcSHQuoteIn%ndata_flag == 1) then
		    lstInData%rBP1 = lstcSHQuoteIn%dBidPrice1; lstInData%iBV1 = lstcSHQuoteIn%nBidVolume1
		    lstInData%rSP1 = lstcSHQuoteIn%dAskPrice1; lstInData%iSV1 = lstcSHQuoteIn%nAskVolume1 
		    lstInData%rBP2 = 0; lstInData%iBV2 = 0
		    lstInData%rSP2 = 0; lstInData%iSV2 = 0
		    lstInData%rBP3 = 0; lstInData%iBV3 = 0
		    lstInData%rSP3 = 0; lstInData%iSV3 = 0
		    lstInData%rBP4 = 0; lstInData%iBV4 = 0
		    lstInData%rSP4 = 0; lstInData%iSV4 = 0
		    lstInData%rBP5 = 0; lstInData%iBV5 = 0
		    lstInData%rSP5 = 0; lstInData%iSV5 = 0 
            if(lstcSHQuoteIn%dbuy_price(29) < lstcSHQuoteIn%dbuy_price(30)) then 
            lstInData%rBP(1) = lstcSHQuoteIn%dbuy_price(30)
            lstInData%iBV(1) = lstcSHQuoteIn%nbuy_volume(30)
            else
            lstInData%rBP(1) = lstcSHQuoteIn%dbuy_price(1)
            lstInData%iBV(1) = lstcSHQuoteIn%nbuy_volume(1)
            end if    
             if(lstcSHQuoteIn%dsell_price(29) > lstcSHQuoteIn%dsell_price(30)) then 
            lstInData%rSP(1) = lstcSHQuoteIn%dsell_price(30)
            lstInData%iSV(1) = lstcSHQuoteIn%nsell_volume(30)
            else
            lstInData%rSP(1) = lstcSHQuoteIn%dsell_price(1)
            lstInData%iSV(1) = lstcSHQuoteIn%nsell_volume(1)
            end if                 
        else if(lstcSHQuoteIn%ndata_flag == 3) then
 		    lstInData%rBP1 = lstcSHQuoteIn%dbuy_price(1); lstInData%iBV1 = lstcSHQuoteIn%nbuy_volume(1)
		    lstInData%rSP1 = lstcSHQuoteIn%dsell_price(1); lstInData%iSV1 = lstcSHQuoteIn%nsell_volume(1)
		    lstInData%rBP2 = lstcSHQuoteIn%dbuy_price(2); lstInData%iBV2 = lstcSHQuoteIn%nbuy_volume(2)
		    lstInData%rSP2 = lstcSHQuoteIn%dsell_price(2); lstInData%iSV2 = lstcSHQuoteIn%nsell_volume(2)
		    lstInData%rBP3 = lstcSHQuoteIn%dbuy_price(3); lstInData%iBV3 = lstcSHQuoteIn%nbuy_volume(3)
		    lstInData%rSP3 = lstcSHQuoteIn%dsell_price(3); lstInData%iSV3 = lstcSHQuoteIn%nsell_volume(3)
		    lstInData%rBP4 = lstcSHQuoteIn%dbuy_price(4); lstInData%iBV4 = lstcSHQuoteIn%nbuy_volume(4)
		    lstInData%rSP4 = lstcSHQuoteIn%dsell_price(4); lstInData%iSV4 = lstcSHQuoteIn%nsell_volume(4)
		    lstInData%rBP5 = lstcSHQuoteIn%dbuy_price(5); lstInData%iBV5 = lstcSHQuoteIn%nbuy_volume(5)
		    lstInData%rSP5 = lstcSHQuoteIn%dsell_price(5); lstInData%iSV5 = lstcSHQuoteIn%nsell_volume(5)           
		    lstInData%rBP = lstcSHQuoteIn%dbuy_price
		    lstInData%rSP = lstcSHQuoteIn%dsell_price	
		    lstInData%iBV = lstcSHQuoteIn%nbuy_volume
		    lstInData%iSV = lstcSHQuoteIn%nsell_volume            
         else if(lstcSHQuoteIn%ndata_flag == 6) then           
		    lstInData%rBP1 = lstcSHQuoteIn%dbuy_price(30); lstInData%iBV1 = lstcSHQuoteIn%nbuy_volume(30)
		    lstInData%rSP1 = lstcSHQuoteIn%dsell_price(30); lstInData%iSV1 = lstcSHQuoteIn%nsell_volume(30)
		    lstInData%rBP2 = lstcSHQuoteIn%dbuy_price(29); lstInData%iBV2 = lstcSHQuoteIn%nbuy_volume(29)
		    lstInData%rSP2 = lstcSHQuoteIn%dsell_price(29); lstInData%iSV2 = lstcSHQuoteIn%nsell_volume(29)
		    lstInData%rBP3 = lstcSHQuoteIn%dbuy_price(28); lstInData%iBV3 = lstcSHQuoteIn%nbuy_volume(28)
		    lstInData%rSP3 = lstcSHQuoteIn%dsell_price(28); lstInData%iSV3 = lstcSHQuoteIn%nsell_volume(28)
		    lstInData%rBP4 = lstcSHQuoteIn%dbuy_price(27); lstInData%iBV4 = lstcSHQuoteIn%nbuy_volume(27)
		    lstInData%rSP4 = lstcSHQuoteIn%dsell_price(27); lstInData%iSV4 = lstcSHQuoteIn%nsell_volume(27)
		    lstInData%rBP5 = lstcSHQuoteIn%dbuy_price(26); lstInData%iBV5 = lstcSHQuoteIn%nbuy_volume(26)
		    lstInData%rSP5 = lstcSHQuoteIn%dsell_price(26); lstInData%iSV5 = lstcSHQuoteIn%nsell_volume(26)
            Do liI = 1, 30, 1
            lstInData%rBP(liI) = lstcSHQuoteIn%dbuy_price(31-liI)
		    lstInData%rSP(liI) = lstcSHQuoteIn%dsell_price(31-liI)
            lstInData%iBV(liI) = lstcSHQuoteIn%nbuy_volume(31-liI)
		    lstInData%iSV(liI) = lstcSHQuoteIn%nsell_volume(31-liI)
            end Do 
        end if            
		lstInData%cExchCode = cSHFE; lstInData%cSecType = cFUT; lstInData%rV = rZERO
		lstInData%rVal = rZERO; lstInData%rPMean = rZERO
        lstInData%iImpBV = 0; lstInData%iImpSV = 0
        lstInData%iTotalBuyVol = lstcSHQuoteIn%nbuy_total_volume
        lstInData%iTotalSelVol = lstcSHQuoteIn%nsell_total_volume
        lstInData%rWeightedBuyOrderP = lstcSHQuoteIn%dbuy_weighted_avg_price
        lstInData%rWeightedSelOrderP = lstcSHQuoteIn%dsell_weighted_avg_price
        lstInData%iData_Flag = lstcSHQuoteIn%ndata_flag
    end subroutine s_read_sh_fut_struct	 
    
    !----------------------------------------------------
    ! subroutine s_set_all_parameters(lcExch, lcSec, lstOutData, liStrategy)
    ! desprition : set all parameters, global
    !----------------------------------------------------                                          
	subroutine s_set_exch_parameters(lcExch, lcSec, lstOutData)
		implicit none
		character(len=*), intent(in)	::	lcExch, lcSec
		type(all_parameter), intent(inout)	::	lstOutData
		integer							::	liSqIntervalSec
		
		liSqIntervalSec = 5
		select case (trim(lcExch))
        case (cSHFE)
            if (giNightMarket == 0) then
			    lstOutData%stExch%iVolDivider = 2; lstOutData%stExch%iPInLevel = 30; 
			    lstOutData%stExch%iMktStartTime = 90000; lstOutData%stExch%iMktEndTime = 150000; lstOutData%stSec%iSecEndTime = 145700
			    lstOutData%stSec%iSecTradeStartTime = lstOutData%stExch%iMktStartTime
            else if (giNightMarket == 1) then
			    lstOutData%stExch%iVolDivider = 2; lstOutData%stExch%iPInLevel = 30; 
			    lstOutData%stExch%iMktStartTime = 210000; lstOutData%stExch%iMktEndTime = 240000; lstOutData%stSec%iSecEndTime = 235700
			    lstOutData%stSec%iSecTradeStartTime = lstOutData%stExch%iMktStartTime                
                if(trim(gcSecName) == 'ru' .or. trim(gcSecName) == 'RU' .or. trim(gcSecName) == 'hc' .or. trim(gcSecName) == 'HC' .or. trim(gcSecName) == 'rb' .or. trim(gcSecName) == 'RB' .or. trim(gcSecName) == 'bu' .or. trim(gcSecName) == 'BU') then
                    lstOutData%stExch%iMktEndTime = 230000; lstOutData%stSec%iSecEndTime = 225700
					lstOutData%stSec%iSecTradeStartTime = lstOutData%stExch%iMktStartTime
                else if (trim(gcSecName) == 'ag' .or. trim(gcSecName) == 'AG' .or. trim(gcSecName) == 'au' .or. trim(gcSecName) == 'AU') then
                    lstOutData%stExch%iMktEndTime = 263000; lstOutData%stSec%iSecEndTime = 262700
					lstOutData%stSec%iSecTradeStartTime = lstOutData%stExch%iMktStartTime
                else
                    lstOutData%stExch%iMktEndTime = 250000; lstOutData%stSec%iSecEndTime = 245700
					lstOutData%stSec%iSecTradeStartTime = lstOutData%stExch%iMktStartTime                 
                end if
            end if
			lstOutData%stSec%rTVCompRatio = 0.20_dp
			lstOutData%stSec%iMinTradeSec = 0; 
			lstOutData%stSec%iLookBackRecordNo = iZERO; 
			lstOutData%stSec%iWithCancel = iNO
        case (cDCE)            
            if (giNightMarket == 0) then
			    lstOutData%stExch%iVolDivider = 2; lstOutData%stExch%iPInLevel = 5; 
			    lstOutData%stExch%iMktStartTime = 90000; lstOutData%stExch%iMktEndTime = 150000; lstOutData%stSec%iSecEndTime = 145700
			    lstOutData%stSec%iSecTradeStartTime = 90050
            else if (giNightMarket == 1) then
			    lstOutData%stExch%iVolDivider = 2; lstOutData%stExch%iPInLevel = 5; 
			    lstOutData%stExch%iMktStartTime = 210000; lstOutData%stExch%iMktEndTime = 233000; lstOutData%stSec%iSecEndTime = 232700
			    lstOutData%stSec%iSecTradeStartTime = 210050
            end if            
			lstOutData%stSec%rTVCompRatio = 0.20_dp
			lstOutData%stSec%iMinTradeSec = 1; 
				lstOutData%stSec%iLookBackRecordNo = iZERO; 
			lstOutData%stSec%iWithCancel = iNO;            
        case (cCZCE)
            if (giNightMarket == 0) then
			    lstOutData%stExch%iVolDivider = 2; lstOutData%stExch%iPInLevel = 5; 
			    lstOutData%stExch%iMktStartTime = 90000; lstOutData%stExch%iMktEndTime = 150000; lstOutData%stSec%iSecEndTime = 145700
			    lstOutData%stSec%iSecTradeStartTime = 90000
            elseif (giNightMarket == 1) then
			    lstOutData%stExch%iVolDivider = 2; lstOutData%stExch%iPInLevel = 5; 
			    lstOutData%stExch%iMktStartTime = 210000; lstOutData%stExch%iMktEndTime = 233000; lstOutData%stSec%iSecEndTime = 232700
			    lstOutData%stSec%iSecTradeStartTime = 210000
            end if            
			lstOutData%stSec%rTVCompRatio = 0.25_dp
			lstOutData%stSec%iMinTradeSec = 1; 
			lstOutData%stSec%iLookBackRecordNo = iZERO; 
			lstOutData%stSec%iWithCancel = iNO;             
		case (cCFFE)
            if ((trim(gcSecName) /= 'tf') .and. (trim(gcSecName) /= 't') .and. (trim(gcSecName) /= 'TF') .and. (trim(gcSecName) /= 'T')) then
			    lstOutData%stExch%iVolDivider = 1; lstOutData%stExch%iPInLevel = 5; 
			    lstOutData%stExch%iMktStartTime = 93000; lstOutData%stExch%iMktEndTime = 150000; lstOutData%stSec%iSecEndTime = 145700
			    lstOutData%stSec%iSecTradeStartTime = lstOutData%stExch%iMktStartTime
            else
			    lstOutData%stExch%iVolDivider = 1; lstOutData%stExch%iPInLevel = 5; 
			    lstOutData%stExch%iMktStartTime = 91500; lstOutData%stExch%iMktEndTime = 151500; lstOutData%stSec%iSecEndTime = 151000
			    lstOutData%stSec%iSecTradeStartTime = lstOutData%stExch%iMktStartTime              
            end if
			lstOutData%stSec%rTVCompRatio = 0.30_dp
			lstOutData%stSec%iMinTradeSec = 0; 
			lstOutData%stSec%iLookBackRecordNo = 120; 
			lstOutData%stSec%iWithCancel = iYES
		case default
        end select
        
        lstOutData%stSec%rTVCompRatio = giFillRate * 1.0/100
    end subroutine s_set_exch_parameters   
    
    !----------------------------------------------------
    ! subroutine s_set_convertion_parameters(lcSecName, lstOutData, liStrategy)
    ! desprition : set all parameters, global
    !----------------------------------------------------       
	subroutine s_set_convertion_parameters(lcSecName, lstOutData)
		implicit none
		character(len=*), intent(in)		::	lcSecName
		type(sec_parameter), intent(inout)	::	lstOutData
		
		select case (trim(lcSecName))
		case ('if', 'IF')
            lstOutData%iUnitPerContract = 300; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 0.2_dp; lstOutData%iPMultiplier = 10; lstOutData%iTickSizeMulti = 2; lstOutData%iDecimalPt = 1
		case ('ic', 'IC')
            lstOutData%rRssTCostByVol = 0._dp; lstOutData%rRssTCostByVal = 0.00002625_dp
			lstOutData%iUnitPerContract = 200; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 0.2_dp; lstOutData%iPMultiplier = 10; lstOutData%iTickSizeMulti = 2; lstOutData%iDecimalPt = 1
 
		case ('ih', 'IH')
            lstOutData%rRssTCostByVol = 0._dp; lstOutData%rRssTCostByVal = 0.00002625_dp
			lstOutData%iUnitPerContract = 300; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 0.2_dp; lstOutData%iPMultiplier = 10; lstOutData%iTickSizeMulti = 2; lstOutData%iDecimalPt = 1
		case ('tf', 'TF')
            lstOutData%rRssTCostByVol = 0.000315_dp; lstOutData%rRssTCostByVal = 0.0_dp
			lstOutData%iUnitPerContract = 10000; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 0.005_dp; lstOutData%iPMultiplier = 10; lstOutData%iTickSizeMulti = 2; lstOutData%iDecimalPt = 1
	
		case ('t', 'T')
            lstOutData%rRssTCostByVol = 0.000165_dp; lstOutData%rRssTCostByVal = 0.0_dp
			lstOutData%iUnitPerContract = 10000; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 0.005_dp; lstOutData%iPMultiplier = 10; lstOutData%iTickSizeMulti = 2; lstOutData%iDecimalPt = 1
	
            
		case ('ag', 'AG')
            lstOutData%rRssTCostByVol = 0._dp; lstOutData%rRssTCostByVal = 0.0000525*0.75_dp !0.00001625_dp
			lstOutData%iUnitPerContract = 15; lstOutData%iWrongVol = 50000; 
			lstOutData%rTickSize = 1._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 0

		case ('au', 'AU')
            lstOutData%rRssTCostByVol = 0.00525*0.75_dp; lstOutData%rRssTCostByVal = 0._dp
			lstOutData%iUnitPerContract = 1000; lstOutData%iWrongVol = 50000; 
			lstOutData%rTickSize = 0.05_dp; lstOutData%iPMultiplier = 100; lstOutData%iTickSizeMulti = 5; lstOutData%iDecimalPt = 2


		case ('al', 'AL')
            lstOutData%rRssTCostByVol = 0.315*0.75_dp; lstOutData%rRssTCostByVal = 0._dp
			lstOutData%iUnitPerContract = 5; lstOutData%iWrongVol = 50000; 
			lstOutData%rTickSize = 5.0_dp; lstOutData%iPMultiplier = 100; lstOutData%iTickSizeMulti = 5; lstOutData%iDecimalPt = 2

            
		case ('bu', 'BU')
            lstOutData%rRssTCostByVol = 0.0_dp; lstOutData%rRssTCostByVal = 0.000105*0.75_dp             
			lstOutData%iUnitPerContract = 10; lstOutData%iWrongVol = 50000; 
			lstOutData%rTickSize = 2._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 0   
            
		case ('cu', 'CU')
            lstOutData%rRssTCostByVol = 0._dp; lstOutData%rRssTCostByVal = 0.00002625*0.75_dp !0.000008125_dp           
			lstOutData%iUnitPerContract = 5; lstOutData%iWrongVol = 50000; 
			lstOutData%rTickSize = 10._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 10; lstOutData%iDecimalPt = 0

		case ('hc', 'HC')
            lstOutData%rRssTCostByVol = 0._dp; lstOutData%rRssTCostByVal = 0.000105*0.75_dp !0.000014625_dp              
			lstOutData%iUnitPerContract = 10; lstOutData%iWrongVol = 50000; 
			lstOutData%rTickSize = 1._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 0
            
		case ('ni', 'NI')
            lstOutData%rRssTCostByVol = 18.9*0.75_dp; lstOutData%rRssTCostByVal = 0.0_dp              
			lstOutData%iUnitPerContract = 1; lstOutData%iWrongVol = 50000; 
			lstOutData%rTickSize = 10._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 5; lstOutData%iDecimalPt = 0  
			
		case ('pb', 'PB')
            lstOutData%rRssTCostByVol = 0.0_dp; lstOutData%rRssTCostByVal = 0.0000315*0.75_dp              
			lstOutData%iUnitPerContract = 5; lstOutData%iWrongVol = 50000; 
			lstOutData%rTickSize = 5._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 5; lstOutData%iDecimalPt = 0  			 
            
		case ('rb', 'RB')
            lstOutData%rRssTCostByVol = 0._dp; lstOutData%rRssTCostByVal = 0.000105*0.75_dp !0.000014625_dp              
			lstOutData%iUnitPerContract = 10; lstOutData%iWrongVol = 50000; 
			lstOutData%rTickSize = 1._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 0


		case ('ru', 'RU')
            lstOutData%rRssTCostByVol = 0._dp; lstOutData%rRssTCostByVal = 0.00004725*0.75_dp !0.00002925_dp              
			lstOutData%iUnitPerContract = 10; lstOutData%iWrongVol = 50000; 
			lstOutData%rTickSize = 5._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 5; lstOutData%iDecimalPt = 0
            
		case ('zn', 'ZN')
            lstOutData%rRssTCostByVol = 0.63*0.75_dp; lstOutData%rRssTCostByVal = 0.0_dp             
			lstOutData%iUnitPerContract = 5; lstOutData%iWrongVol = 50000; 
			lstOutData%rTickSize = 5._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 5; lstOutData%iDecimalPt = 0
                                     
		case ('a', 'A')
            lstOutData%rRssTCostByVol = 0.21_dp; lstOutData%rRssTCostByVal = 0.0_dp            
			lstOutData%iUnitPerContract = 10; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 1._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 0
			
		case ('bb', 'BB')
            lstOutData%rRssTCostByVol = 0._dp; lstOutData%rRssTCostByVal = 0.0000525_dp            
			lstOutData%iUnitPerContract = 500; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 0.05_dp; lstOutData%iPMultiplier = 100; lstOutData%iTickSizeMulti = 5; lstOutData%iDecimalPt = 2
            lstOutData%iWithCancel = iYES
			
		case ('c', 'C')
            lstOutData%rRssTCostByVol = 0.063_dp; lstOutData%rRssTCostByVal = 0.0_dp            
			lstOutData%iUnitPerContract = 10; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 1._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 0

		case ('cs', 'CS')
            lstOutData%rRssTCostByVol = 0.1575_dp; lstOutData%rRssTCostByVal = 0.0_dp            
			lstOutData%iUnitPerContract = 10; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 1._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 0
            
		case ('v', 'V')
            lstOutData%rRssTCostByVol = 0.21_dp; lstOutData%rRssTCostByVal = 0.0_dp            
			lstOutData%iUnitPerContract = 5; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 5._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 0
            
		case ('fb', 'FB')
            lstOutData%rRssTCostByVol = 0._dp; lstOutData%rRssTCostByVal = 0.0000525_dp             
			lstOutData%iUnitPerContract = 500; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 0.05_dp; lstOutData%iPMultiplier = 100; lstOutData%iTickSizeMulti = 5; lstOutData%iDecimalPt = 2
			lstOutData%iWithCancel = iYES
			
		case ('i', 'I')
            lstOutData%rRssTCostByVol = 0._dp; lstOutData%rRssTCostByVal = 0.000126_dp             
			lstOutData%iUnitPerContract = 100; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 0.5_dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 1

			
		case ('j', 'J')
            lstOutData%rRssTCostByVol = 0._dp; lstOutData%rRssTCostByVal = 0.000315_dp              
			lstOutData%iUnitPerContract = 100; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 0.5_dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 1

			
		case ('jd', 'JD')
            lstOutData%rRssTCostByVol = 0._dp; lstOutData%rRssTCostByVal = 0.0001575_dp              
			lstOutData%iUnitPerContract = 10; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 1._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 0
			lstOutData%iWithCancel = iYES
			
		case ('jm', 'JM')
            lstOutData%rRssTCostByVol = 0._dp; lstOutData%rRssTCostByVal = 0.000315_dp              
			lstOutData%iUnitPerContract = 60; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 0.5_dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 1
			
			
		case ('l', 'L')
            lstOutData%rRssTCostByVol = 0.42_dp; lstOutData%rRssTCostByVal = 0.0_dp              
			lstOutData%iUnitPerContract = 5; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 5._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 5; lstOutData%iDecimalPt = 0
			
			
		case ('m', 'M')
            lstOutData%rRssTCostByVol = 0.1575_dp; lstOutData%rRssTCostByVal = 0.0_dp                
			lstOutData%iUnitPerContract = 10; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 1._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 0
			lstOutData%iWithCancel = iYES
			
		case ('p', 'P')
            lstOutData%rRssTCostByVol = 0.2625_dp; lstOutData%rRssTCostByVal = 0.0_dp               
			lstOutData%iUnitPerContract = 10; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 2._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 2; lstOutData%iDecimalPt = 0
			
			
		case ('pp', 'PP')
            lstOutData%rRssTCostByVol = 0._dp; lstOutData%rRssTCostByVal = 0.000063_dp              
			lstOutData%iUnitPerContract = 5; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 1._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 0
			lstOutData%iWithCancel = iYES
			
		case ('y', 'Y')
            lstOutData%rRssTCostByVol = 0.2625_dp; lstOutData%rRssTCostByVal = 0.0_dp              
			lstOutData%iUnitPerContract = 10; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 2._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 2; lstOutData%iDecimalPt = 0
			
                        
		case ('CF', 'cf')
            lstOutData%rRssTCostByVol = 0.903_dp; lstOutData%rRssTCostByVal = 0.0_dp              
			lstOutData%iUnitPerContract = 5; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 5._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 5; lstOutData%iDecimalPt = 0
			 
 		case ('FG', 'fg')
            lstOutData%rRssTCostByVol = 1.26_dp; lstOutData%rRssTCostByVal = 0.0_dp              
			lstOutData%iUnitPerContract = 20; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 1._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 0
			                                                        
		case ('MA', 'ma')
            lstOutData%rRssTCostByVol = 0.63_dp; lstOutData%rRssTCostByVal = 0.0_dp              
			lstOutData%iUnitPerContract = 10; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 1._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 0
			   
            
		case ('OI', 'oi')
            lstOutData%rRssTCostByVol = 0.13125_dp; lstOutData%rRssTCostByVal = 0.0_dp              
			lstOutData%iUnitPerContract = 10; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 2._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 0
			             
 		case ('RM', 'rm')
            lstOutData%rRssTCostByVol = 0.315_dp; lstOutData%rRssTCostByVal = 0.0_dp
			lstOutData%iUnitPerContract = 10; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 1._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 0
			
            
 		case ('SF', 'sf')
            lstOutData%rRssTCostByVol = 1.26_dp; lstOutData%rRssTCostByVal = 0.0_dp              
			lstOutData%iUnitPerContract = 5; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 2._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 0
			
            
 		case ('SM', 'sm')
            lstOutData%rRssTCostByVol = 0.945_dp; lstOutData%rRssTCostByVal = 0.0_dp              
			lstOutData%iUnitPerContract = 5; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 2._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 0
			
 		case ('AP', 'ap')
            lstOutData%rRssTCostByVol = 0.0525_dp; lstOutData%rRssTCostByVal = 0.0_dp              
			lstOutData%iUnitPerContract = 10; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 1._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 0
            
 		case ('SR', 'sr')
            lstOutData%rRssTCostByVol = 0.1575_dp; lstOutData%rRssTCostByVal = 0.0_dp              
			lstOutData%iUnitPerContract = 10; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 1._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 0
			
            
		case ('TA', 'ta')
            lstOutData%rRssTCostByVol = 0.315_dp; lstOutData%rRssTCostByVal = 0.0_dp              
			lstOutData%iUnitPerContract = 5; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 2._dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 2; lstOutData%iDecimalPt = 0
			             
		case ('ZC', 'zc')
            lstOutData%rRssTCostByVol = 0.063_dp; lstOutData%rRssTCostByVal = 0.0_dp              
			lstOutData%iUnitPerContract = 100; lstOutData%iWrongVol = 100000; 
			lstOutData%rTickSize = 0.2_dp; lstOutData%iPMultiplier = 1; lstOutData%iTickSizeMulti = 1; lstOutData%iDecimalPt = 0		     
            
		case default
		end select
    end subroutine s_set_convertion_parameters      

    !----------------------------------------------------
    ! subroutine s_my_trading_signal()
    ! desprition : generate signal
    !----------------------------------------------------     
    subroutine s_my_trading_signal(liTSCounter, lcExchCode, lcTicker, liOrderType, liOrder, lrBP, lrSP, &
						           liCloseV, liOpenV, lbCancel, liTSLB, liTSLen, lastTSignal, liNoTradeItemEachRun, liCancelSigID)
		implicit none
		character(len = *), intent(in)	::	lcExchCode, lcTicker
		integer, intent(in)				::	liOrderType, liOrder, liCloseV, liOpenV, liCancelSigID
        real(dp), intent(in)			::	lrBP, lrSP
        logical, intent(in)				::	lbCancel
		integer, intent(in)				::	liTSLB, liTSLen
		type(myTSignal), dimension(liTSLB:(liTSLB + liTSLen - 1)), intent(inout)	::	lastTSignal
		integer, intent(inout)			::	liNoTradeItemEachRun, liTSCounter 
        
        liTSCounter = liTSCounter + 1
        lastTSignal(liNoTradeItemEachRun)%iStID = giStID
        lastTSignal(liNoTradeItemEachRun)%iSigID = liTSCounter
        lastTSignal(liNoTradeItemEachRun)%rBP = lrBP
        lastTSignal(liNoTradeItemEachRun)%rSP = lrSP 
        lastTSignal(liNoTradeItemEachRun)%iOpenV =  iZERO
        lastTSignal(liNoTradeItemEachRun)%iCloseV =  iZERO
        lastTSignal(liNoTradeItemEachRun)%cExchCode = trim(lcExchCode)
        lastTSignal(liNoTradeItemEachRun)%cTicker = trim(lcTicker)
        if (liOrderType == iCD .and. lbCancel) then
            lastTSignal(liNoTradeItemEachRun)%iSigAction = iSigActCancel
            lastTSignal(liNoTradeItemEachRun)%iCancelSigID = liCancelSigID
            lastTSignal(liNoTradeItemEachRun)%iSigOpenClose = 0
            goto 999
        end if
        if ((liOrder == iBPK .or. liOrder == iBK .or. liOrder == iSPK .or. liOrder == iSK)  .and. liOrderType == iXD) then
            if (liCloseV ==0) lastTSignal(liNoTradeItemEachRun)%iSigOpenClose = 1
        end if
        
        if ((liOrder == iBP .or.  liOrder == iSP)  .and. liOrderType == iXD) then
            lastTSignal(liNoTradeItemEachRun)%iSigOpenClose = 2
        end if 
        
        lastTSignal(liNoTradeItemEachRun)%iCancelSigID = iZERO
        if ((liOrder == iBPK .or. liOrder == iBP .or. liOrder == iBK)  .and. liOrderType == iXD) then
            lastTSignal(liNoTradeItemEachRun)%iSigAction = iSigActBuy
            lastTSignal(liNoTradeItemEachRun)%iOpenV =  liOpenV
            lastTSignal(liNoTradeItemEachRun)%iCloseV =  liCloseV
            lastTSignal(liNoTradeItemEachRun)%iInstruction = iSigInstructionROD
        end if  
        
        if ((liOrder == iSPK .or. liOrder == iSP .or. liOrder == iSK)  .and. liOrderType == iXD) then
            lastTSignal(liNoTradeItemEachRun)%iSigAction = iSigActSell
            lastTSignal(liNoTradeItemEachRun)%iOpenV =  liOpenV 
            lastTSignal(liNoTradeItemEachRun)%iCloseV = liCloseV 
            lastTSignal(liNoTradeItemEachRun)%iInstruction = iSigInstructionROD
        end if         


999		liNoTradeItemEachRun = liNoTradeItemEachRun + 1					
end subroutine s_my_trading_signal
                                   
                                   
    !----------------------------------------------------
    ! subroutine s_init_eachPOS(lstEachPOS)
    ! description: initialization of postion data structure
    ! input: lstEachPOS 
    ! output: lstEachPOS     
    !----------------------------------------------------        
 	subroutine s_init_eachPOS(lstEachPOS)
		implicit none
		type(position_data), intent(inout)	::	lstEachPOS
		
		lstEachPOS%cTicker = cBLANK; lstEachPOS%iMaxPOS = iZERO; lstEachPOS%iPOSD = iZERO; lstEachPOS%iPOSK = iZERO
		lstEachPOS%iYestPOSD = iZERO; lstEachPOS%iYestPOSK = iZERO; lstEachPOS%rAvailCash = rZERO
		lstEachPOS%iPOSChg = iZERO
    end subroutine s_init_eachPOS                                   
                                   
 	real(dp) function f_round_number(f_r, f_dec)
		implicit none
		real(dp), intent(in)	::	f_r, f_dec
		
		if (f_r > rZero) f_round_number = int(f_r/f_dec + 0.5_dp) * f_dec
		if (f_r < rZero) f_round_number = -int(-f_r/f_dec + 0.5_dp) * f_dec
		if (f_r == rZero) f_round_number = rZero
	end function f_round_number
                                   
                                   
	subroutine s_get_own_position(lcTicker, lastEachPOSArray, liLB, liUB, lstEachPOS)
		implicit none
		character(len=*), intent(in)	::	lcTicker
		integer, intent(in)				::	liLB, liUB
		type(position_data), dimension(liLB:liUB), intent(in)	::	lastEachPOSArray
		type(position_data), intent(inout)	::	lstEachPOS
		integer							::	liN
		
		do liN = liLB, liUB, 1
			if (trim(lastEachPOSArray(liN)%cTicker) == trim(lcTicker)) then
				lstEachPOS = lastEachPOSArray(liN); goto 9999
			end if
		end do
9999	continue
    end subroutine s_get_own_position       
    
    !----------------------------------------------------
    ! integer function f_sign(liN)
    ! desprition : return the sign of the input number
    !----------------------------------------------------         
	integer function f_sign(liN)
		implicit none
		integer, intent(in)	::	liN

		if (liN > 0) f_sign = 1; if (liN == 0) f_sign = 0; if (liN < 0) f_sign = -1
    end function f_sign
    
    
    !----------------------------------------------------
    ! character*4 function f_find_security_name(lcTicker)
    ! desprition : fine name of the security
    !----------------------------------------------------     
	character*4 function f_find_security_name(lcTicker,lcExch)
		implicit none
		character(len=*), intent(in)	::	lcTicker
		character(len=*), intent(in)	::	lcExch
        if (trim(lcExch) /= cCZCE) then
		    f_find_security_name = lcTicker(:(len_trim(lcTicker)-4))
        else
            f_find_security_name = lcTicker(:(len_trim(lcTicker)-3))
        end if
    end function f_find_security_name

    !----------------------------------------------------
    ! character*4 function f_find_security_name_new(lcTicker)
    ! desprition : fine name of the security
    !----------------------------------------------------     
	character*4 function f_find_security_name_new(lcTicker)
		implicit none
		character(len=*), intent(in)	::	lcTicker
        
    f_find_security_name_new = lcTicker(1:2)
    if(trim(f_find_security_name_new(2:2))=='1') then
    f_find_security_name_new = lcTicker(1:1)
    end if
    end function f_find_security_name_new 	
    
    !----------------------------------------------------
    ! function f_convert_f_string_to_c_char(lcIn, liLen, liLB, liUB)
    ! desprition : convert Fortran string to C char
    !----------------------------------------------------      
	function f_convert_f_string_to_c_char(lcIn, liLen, liLB, liUB)
		implicit none
		integer, intent(in)				::	liLen, liLB, liUB
		character(len=*), intent(in)	::	lcIn
		character(c_char)				::	f_convert_f_string_to_c_char(liUB - liLB + 1)
		integer							::	liN, liM
		
		if (liLen > iZERO) then
			liM = iZERO
			do liN = liLB, liLB + liLen - 1, 1
				liM = liM + 1
				f_convert_f_string_to_c_char(liN) = lcIn(liM:liM)
			end do
			forall (liN = (liLB + liLen):liUB) f_convert_f_string_to_c_char(liN) = char(iZERO)
		else
			forall (liN = liLB:liUB) f_convert_f_string_to_c_char(liN) = char(iZERO)
		end if
    end function f_convert_f_string_to_c_char    
    
    !----------------------------------------------------
    ! function f_convert_c_char_to_f_string(lccIn, licInt)
    ! desprition : convert C char to Fortran string
    !----------------------------------------------------                             
	function f_convert_c_char_to_f_string(lccIn, licInt)
		implicit none
		integer(c_int), intent(in)		::	licInt
		character(c_char), intent(in)	::	lccIn(licInt)
		character(len=licInt)			::	f_convert_c_char_to_f_string
		integer(c_int)					::	liN
		
		f_convert_c_char_to_f_string = cBLANK
		if (licInt > iZERO) then
			do liN = 1, licInt, 1
				if (lccIn(liN) /= char(iZERO)) then
					f_convert_c_char_to_f_string(liN:liN)=lccIn(liN)
				else
					exit
				end if
			end do
		end if
    end function f_convert_c_char_to_f_string    
    
    !----------------------------------------------------
    ! integer function cha2int(lcIn)
    ! desprition : char to int
    !----------------------------------------------------      
	integer function cha2int(lcIn)
		implicit none
		character(len=*), intent(inout)	::	lcIn
		integer							::	liN, liI

		lcIn = adjustl(lcIn); liN = len_trim(lcIn); cha2int = iZERO
		do liI = 1, liN, 1
			cha2int = cha2int*10 + ichar(lcIn(liI:liI))-48
		end do
    end function cha2int   
     !----------------------------------------------------
    ! character(len=256) function int2cha(liIn)
    ! desprition : int to char
    !----------------------------------------------------       
	character(len=256) function int2cha(liIn)
		implicit none
		integer, intent(in)	::	liIn
		character(len=256)	::	n
		integer				::	i, sign, tempi, tempj, temp

        int2cha = char(0); i = liIn; sign = i; tempj = 1
		if(sign < iZERO) i = -1 * i
		do tempi = 1, 256, 1
			temp = mod(i,10); n(tempi:tempi) = char(temp + 48); i = i/10
			if(i == iZERO) exit
		end do
		if(sign < iZERO) then
			tempi = tempi + 1; n(tempi:tempi) = '-'
		end if
		do tempj = 1, tempi, 1
			int2cha(tempj:tempj) = n(tempi+1-tempj:tempi+1-tempj)
		end do
    end function int2cha     
    !----------------------------------------------------
    !  real function cha2num(cha)
    ! desprition : int to char
    !----------------------------------------------------       
    real function cha2num(cha)
        implicit none
        character(len=*) :: cha
        character :: cha2*32
        integer :: i, nn, lin
        real :: chatemp
        cha2 = adjustl(cha)
        nn = len_trim(cha2)
        do i=1, nn
            if(cha2(i:i)=='.') exit
        end do
        
        if(i >= nn) then
            cha2num = cha2int(cha2(1:(i-1)))
        else
            if(cha2(1:1)=='-') then
                cha2num = cha2int(cha2(1:(i-1))) - cha2int(cha2((i+1):nn))/(10.0**(nn-i))
            else
                cha2num = cha2int(cha2(1:(i-1))) + cha2int(cha2((i+1):nn))/(10.0**(nn-i))
            end if
        end if
        
        return
    end function cha2num       
    
    !----------------------------------------------------
    ! subroutine s_open_ev_data(liEvFileNumber, lcEvFileName)
    ! desprition : read input files
    !----------------------------------------------------        
    subroutine s_open_ev_data(liEvFileNumber, lcEvFileName)
	    implicit none
	    integer				::	liEvFileNumber
	    character(len=*), intent(in)	::	lcEvFileName
        character :: lcSoName * 20, lcString*512, lcSep*1,lcSecName*4
        integer :: liFindFlg, liTemp
        integer, dimension(1:100)	  ::  laiDivideP
        integer	::  liDivNum, liI        
        real (dp) :: lrTemp 

	
        lcSecName = trim(f_find_security_name_new(gcTicker))         
        lcSoName = trim(gcStrategyName)//'_'//trim(lcSecName)       
        liFindFlg = 0
        liDivNum = 0
        lcSep = '|'  
        laiDivideP=0 
		!write(giFileNumber,"(a10,1x,i12,1x,a256,1x,a20)")    'mysub0_ev1',liEvFileNumber, lcEvFileName, lcSoName
        open(unit=liEvFileNumber, file=lcEvFileName, status='OLD')
        !open(unit=liEvFileNumber, file=trim('D:/HFT/benben/live/vchg_ev/ev_new.txt'), status='old')        
        do while(.true.)
		 !  write(giFileNumber,"(a10,1x,a512)")    'mysub0_ev3',lcString
           read(liEvFileNumber, '(a512)',end=107) lcString
           liDivNum = 0
           do liI = 1, len_trim(lcString), 1
			    if (lcString(liI:liI)==lcSep) then
				    liDivNum = liDivNum + 1; laiDivideP(liDivNum) = liI
			    end if
           end do
           if (liDivNum == 0) exit
           if (trim(lcSoName) == trim(lcString(1:(laiDivideP(1)-1)))) then
               liFindFlg = 1
               exit
           end if
        end do
 107    continue  		
        
        if (liFindFlg == 1) then
            gstInputParas%iInputTradeStartTime = int(cha2num(lcString((laiDivideP(1)+1):(laiDivideP(2)-1))))
            gstInputParas%iInputSecSqTime = int(cha2num(lcString((laiDivideP(2)+1):(laiDivideP(3)-1))))
            gstInputParas%iInputOpenEndTime = int(cha2num(lcString((laiDivideP(3)+1):(laiDivideP(4)-1))))  
            gstInputParas%rInputPara1 = cha2num(lcString((laiDivideP(4)+1):(laiDivideP(5)-1)))            
            gstInputParas%rInputPara2 = cha2num(lcString((laiDivideP(5)+1):(laiDivideP(6)-1)))   
            gstInputParas%rInputPara3 = cha2num(lcString((laiDivideP(6)+1):(laiDivideP(7)-1)))   
            gstInputParas%rInputCostByVol = cha2num(lcString((laiDivideP(7)+1):(laiDivideP(8)-1)))               
            gstInputParas%rInputCostByVal = cha2num(lcString((laiDivideP(8)+1):(laiDivideP(9)-1)))    
            
!             write(giFileNumber,617) trim(lcSoName), gstInputParas%iInputTradeStartTime, gstInputParas%iInputSecSqTime, gstInputParas%iInputOpenEndTime, gstInputParas%rInputPara1, &
!                        gstInputParas%rInputPara2, gstInputParas%rInputPara3, gstInputParas%rInputCostByVol, gstInputParas%rInputCostByVal
!617         format('Input Info: so_name: ', a12, 1x ,'trade_start_time: ', i0, 1x, 'close_time: ', i0, 1x, 'open_end_time: ', i0, 1x, 'para1: ', f0.1,1x, 'para2: ', f0.1, 1x, &
!                        'para3: ', f0.1, 1x, 'fee_by_vol: ', f12.9, 1x, 'fee_by_amt: ', f12.9)                         
                        
            call s_check_inputparas()
        else
            giReadParaFailedFlg = 1
        end if
     !write(giFileNumber,"(a10,1x,i12,1x,a256,1x,i10,1x,i10)")    'mysub0_ev2',liEvFileNumber, lcEvFileName, liFindFlg,	giReadParaFailedFlg		  
        close(liEvFileNumber)

     end subroutine s_open_ev_data
    !----------------------------------------------------
    ! subroutine s_check_inputparas()
    ! desprition : check input data
    !----------------------------------------------------       
     subroutine s_check_inputparas()
   	    implicit none
        
        if (gstInputParas%iInputTradeStartTime > 240000 .or. gstInputParas%iInputSecSqTime > 270000 .or. gstInputParas%iInputOpenEndTime > 270000 &
            .or. (gstInputParas%iInputTradeStartTime > gstInputParas%iInputSecSqTime) .or. (gstInputParas%iInputTradeStartTime > gstInputParas%iInputOpenEndTime) &
            .or. (gstInputParas%iInputOpenEndTime > gstInputParas%iInputSecSqTime)) then
            giReadParaFailedFlg = 1
             !write(giFileNumber, *) 'Initfailed, input time error!'
        end if
            
        if (giStrateType == iSTRATEGY_HI5) then
            if (gstInputParas%rInputPara1 > 30 .or. gstInputParas%rInputPara1 < 0) then
                giReadParaFailedFlg = 1
                 !write(giFileNumber, *) 'Initfailed, hi5 para1 error!'               
            end if 
            if (gstInputParas%rInputPara2 > 30 .or. gstInputParas%rInputPara2 < 0) then
                giReadParaFailedFlg = 1
                 !write(giFileNumber, *) 'Initfailed, hi5 para2 error!'               
            end if  
            if (gstInputParas%rInputPara3 > 30 .or. gstInputParas%rInputPara3 < 0) then
                giReadParaFailedFlg = 1
                 !write(giFileNumber, *) 'Initfailed, hi5 para3 error!'               
            end if             
        end if
     end subroutine s_check_inputparas    
    
    
End Module Common_Func