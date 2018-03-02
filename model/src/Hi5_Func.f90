    
Module Hi5_Func
	Use Common_Var_Def
    Use Common_Func
    Use Hi5_Var_Def
 	Implicit None
	Save
    Contains
    
    
    !----------------------------------------------------
    ! subroutine s_hi5_variable_init()
    ! description: init hi5 global variables
    !----------------------------------------------------  
    subroutine s_hi5_variable_init(lstCurrInData, lstGlobalPara)  
        implicit none
        type(in_data), intent(in)	::	lstCurrInData
        type (all_parameter), intent(in)      ::    lstGlobalPara 
        
        gstHi5_Paras%rBVSum = rZERO
        gstHi5_Paras%rSVSum = rZERO 
        gstHi5_Paras%rStopLossThreshold = 0.01
        gstHi5_Paras%rVolBuy = rZERO
        gstHi5_Paras%rVolSel = rZERO   
        gstHi5_Paras%iStop = iZERO
        gstHi5_Paras%rBSVSumRatio=rZERO
        gstHi5_Paras%rBSVChgSumRatio=rZERO
        gstHi5_Paras%iChgStatus = iZERO
        if (giWriteOutputFileFlg == 1) then   
		    if (allocated(gastHi5OutData)) deallocate(gastHi5OutData)
		    allocate(gastHi5OutData(1:iOUT_LEN))
        end if
        giHi5OutDataIndex = 1
        giHi5TraceInit = 0
        if (trim(gcExch) == cCFFE) then
            gstHi5_Paras%iLimitSqRange = 8             
        else
            gstHi5_Paras%iLimitSqRange = 3 
        end if       
        call s_init_wgt_array(garWgtArray, garWgtArrayInv, lstCurrInData, lstGlobalPara)! init weighting factors
    end subroutine s_hi5_variable_init  
    
    !----------------------------------------------------
    ! subroutine s_init_wgt_array()
    ! description: init weighting factor array
    !----------------------------------------------------      
	subroutine s_init_wgt_array(larWgtArray, larWgtArrayInv, lstCurrInData, lstGlobalPara)
		implicit none
		real(dp), allocatable, intent(inout) ::	larWgtArray(:), larWgtArrayInv(:)
        type(in_data), intent(in)	::	lstCurrInData
        type (all_parameter), intent(in)      ::    lstGlobalPara  
        integer :: liTo, liHalftime, liN
		
		if (allocated(larWgtArray)) deallocate(larWgtArray)
		liTo = int((lstCurrInData%rPLmtUp - lstCurrInData%rPLmtDn) / lstGlobalPara%stSec%rTickSize  + 0.5) + 1
		allocate(larWgtArray(1:liTo))
        if (trim(gcExch) == cSHFE) then
            liHalftime = 3
        else
		    liHalftime = 5
        end if
		forall (liN = 1:liTo) larWgtArray(liN) = (2._dp)**(-(1._dp)*(liN-1)/liHalftime)

		if (allocated(larWgtArrayInv)) deallocate(larWgtArrayInv)
        allocate(larWgtArrayInv(1:liTo))
        forall (liN = 1:liTo) larWgtArrayInv(liN) = larWgtArray(liTo - liN + 1)
        
    end subroutine s_init_wgt_array   
    
    !----------------------------------------------------
    ! subroutine s_hi5_sig_gen_normal() 
    ! description: set Hi5 model signal generation
    !----------------------------------------------------  
        
    subroutine s_hi5_sig_gen(lstCurrInData, lstPreInData, lstGlobalPara, lstPosData, lstTradeInfo, lstPreTradeInfo, liTickNo, laiFullOrderBookBuy, laiFullOrderBookSel,& 
                             laiPreFullOrderBookBuy, laiPreFullOrderBookSel,liFullOrderBookL, liFullOrderBookU, lstTSignal) 
        type(all_parameter), intent(in)	::	lstGlobalPara
        type(in_data), intent(in)		::	lstCurrInData, lstPreInData
        type(position_data), intent(in) :: lstPosData
        type(trade_info), intent(inout)		::  lstTradeInfo, lstPreTradeInfo
        integer, intent(in)  :: liFullOrderBookL, liFullOrderBookU, liTickNo
        integer, dimension(liFullOrderBookL:liFullOrderBookU), intent(inout)	::	laiFullOrderBookBuy, laiFullOrderBookSel,laiPreFullOrderBookBuy, laiPreFullOrderBookSel
        type(myTSignal), dimension(0:(iSIGNAL_LEN - 1))	::	lstTSignal
        real(dp) :: lrTradeP, lrVolSel, lrVolBuy, lrBVSum, lrSVSum, larBP(30), larSP(30), lrTick, lrPNet, lrOI
        real(dp) :: lrBuyPrice, lrSelPrice, lrOrderP,lrPriceL, lrPriceH, rBSVSumRatio,rBSVChgSumRatio
        integer  :: laiBV(30), laiSV(30), liVLo, liVHi,iChgStatus
        integer  :: liI, liLevel, liFrom, liTo, liIndex
        integer  :: liRefreshBidFrom, liRefreshBidTo, liRefreshAskFrom, liRefreshAskTo

        !data prepare begin
        lrTradeP = lstCurrInData%rAvgP
        lrTick = lstGlobalPara%stSec%rTickSize  
        larBP = lstCurrInData%rBP
        larSP = lstCurrInData%rSP
        laiBV = lstCurrInData%iBV
        laiSV = lstCurrInData%iSV
        liLevel = lstGlobalPara%stExch%iPInLevel 

        if (larBP(liLevel) < lrTick) then
            liRefreshBidFrom = 1
        else
            liRefreshBidFrom = max(int((larBP(liLevel)-lstCurrInData%rPLmtDn)/lrTick+0.5) + 1, 1)
        end if
        liRefreshBidTo = max(int((max(larBP(1), lstPreInData%rBP(1)) - lstCurrInData%rPLmtDn)/lrTick+0.5) + 1, 1)
        
        if (larSP(liLevel) < lrTick) then
            liRefreshAskTo = liFullOrderBookU - liFullOrderBookL + 1
        else
            liRefreshAskTo = int((larSP(liLevel)-lstCurrInData%rPLmtDn)/lrTick+0.5) + 1
        end if
        if (larSP(1) < lrTick .and. lstPreInData%rSP(1) < lrTick) then
            liRefreshAskFrom = liFullOrderBookU - liFullOrderBookL + 1
        else if (min(larSP(1), lstPreInData%rSP(1)) < lrTick) then
            liRefreshAskFrom = int((max(larSP(1), lstPreInData%rSP(1))-lstCurrInData%rPLmtDn)/lrTick+0.5) + 1
        else
            liRefreshAskFrom = int((min(larSP(1), lstPreInData%rSP(1))-lstCurrInData%rPLmtDn)/lrTick+0.5) + 1
        end if        
      
        ! data prepare
        laiFullOrderBookBuy(liRefreshBidFrom : liRefreshBidTo) = 0
        laiFullOrderBookSel(liRefreshAskFrom : liRefreshAskTo) = 0        
      
        Do liI = 1, liLevel, 1   ! build order book
            if (larBP(liI) > lrTick/2 .and. abs(larBP(liI) - lstCurrInData%rPLmtDn) > lrTick/2) then
                liIndex = int((larBP(liI)-lstCurrInData%rPLmtDn)/lrTick+0.5) + 1
                laiFullOrderBookBuy(liIndex) = laiBV(liI) 
            end if
            if (larSP(liI) > lrTick/2 .and. abs(larSP(liI) - lstCurrInData%rPLmtUp) > lrTick/2) then
                liIndex = int((larSP(liI)-lstCurrInData%rPLmtDn)/lrTick+0.5) + 1
                laiFullOrderBookSel(liIndex) = laiSV(liI) 
            end if   
            if (abs(larBP(liI) - lstCurrInData%rPLmtDn) < lrTick/2) laiBV(liI) = 0
            if (abs(larSP(liI) - lstCurrInData%rPLmtUp) < lrTick/2) laiSV(liI) = 0
        end Do
        
        ! divide the order to last BP1 and SP1
        if (lrTradeP > lstPreInData%rSP(1)) lrTradeP = lstPreInData%rSP(1)
        if (lrTradeP < lstPreInData%rBP(1)) lrTradeP = lstPreInData%rBP(1)
        if (abs(lstPreInData%rSP(1) - lstPreInData%rBP(1)) < 0.0001) then
            lrVolBuy = 0.0_dp
            lrVolSel = 0.0_dp        
        else
            lrVolBuy = lstCurrInData%iVol * (lstPreInData%rSP(1)-lrTradeP)/(lstPreInData%rSP(1) - lstPreInData%rBP(1)) ! trade vol @ last bp1
            lrVolSel = lstCurrInData%iVol - lrVolBuy  ! trade vol @ last sp1
        end if
        
        if (liTickNo <= 2) then 
            lrVolBuy = 0.0_dp
            lrVolSel = 0.0_dp
            laiPreFullOrderBookBuy = laiFullOrderBookBuy
            laiPreFullOrderBookSel = laiFullOrderBookSel             
        end if 
		
        if(giHi5Type==iHI56 .or. giHi5Type==iHI57 .or. giHi5Type==iHI58 .or. giHi5Type==iHI59) then  
           lrTradeP = lstCurrInData%rAvgP 
           gaiFullOrderBookBuyDiff(max(1,liRefreshBidFrom-50) : min(liRefreshBidTo+50, liFullOrderBookU)) = 0
           gaiFullOrderBookSelDiff(max(1,liRefreshAskFrom-50) : min(liRefreshAskTo+50, liFullOrderBookU)) = 0
           gaiFullOrderBookBuyDiff(liRefreshBidFrom : liRefreshBidTo) = laiFullOrderBookBuy(liRefreshBidFrom : liRefreshBidTo) - laiPreFullOrderBookBuy(liRefreshBidFrom : liRefreshBidTo) ! get orderbook diff
           gaiFullOrderBookSelDiff(liRefreshAskFrom : liRefreshAskTo) = laiFullOrderBookSel(liRefreshAskFrom : liRefreshAskTo) - laiPreFullOrderBookSel(liRefreshAskFrom : liRefreshAskTo)      
      
           lrPriceL = max(int(lrTradeP/lrTick)*lrTick, lstCurrInData%rPLmtDn)    ! immediate tick price below tradep
           lrPriceH = min(lrPriceL + lrTick, lstCurrInData%rPLmtUp)              ! immediate tick price over tradep
           liVLo = lstCurrInData%iVol * (lrPriceH - lrTradeP) / lrTick + 0.5     ! last trade vol @ priceL
           liVHi = lstCurrInData%iVol - liVLo                                    ! last trade vol @ priceH
           liIndex = int((lrPriceL-lstCurrInData%rPLmtDn)/lrTick+0.5) + 1 
           if (liIndex >= 1 .and. liIndex <= liFullOrderBookU) then
              gaiFullOrderBookBuyDiff(liIndex) = gaiFullOrderBookBuyDiff(liIndex) + liVLo
              gaiFullOrderBookSelDiff(liIndex) = gaiFullOrderBookSelDiff(liIndex) + liVLo
           end if
           liIndex = int((lrPriceH-lstCurrInData%rPLmtDn)/lrTick+0.5) + 1 
           if (liIndex >= 1 .and. liIndex <= liFullOrderBookU) then
               gaiFullOrderBookBuyDiff(liIndex) = gaiFullOrderBookBuyDiff(liIndex) + liVHi 
               gaiFullOrderBookSelDiff(liIndex) = gaiFullOrderBookSelDiff(liIndex) + liVHi 
           end if
        end if 
        
        if (trim(gcExch) == cCZCE) then
            gstHi5_Paras%rVolBuy = lrVolBuy
            gstHi5_Paras%rVolSel = lrVolSel
            if (lstPreInData%iTime == lstCurrInData%iTime .and. lstCurrInData%iVol <= iZERO) then
                gstHi5_Paras%rVolBuy = gstHi5_Paras%rVolBuyPre
                gstHi5_Paras%rVolSel = gstHi5_Paras%rVolSelPre 
                lrVolBuy = gstHi5_Paras%rVolBuy
                lrVolSel = gstHi5_Paras%rVolSel
            end if        
            gstHi5_Paras%rVolBuyPre = gstHi5_Paras%rVolBuy
            gstHi5_Paras%rVolSelPre = gstHi5_Paras%rVolSel
         end if
        ! compute BVSum and SVSum
        lrBVSum = 1.0;lrSVSum = 1.0 
   
        select case (giHi5Type)  ! each strategy
        case (iHI50)   
            call s_hi50_cal(larBP, larSP, laiBV, laiSV, lrVolSel, lrVolBuy, lrTick, lrBVSum, lrSVSum)
        case default
        end select
                   
        gstHi5_Paras%rBVSum = lrBVSum
        gstHi5_Paras%rSVSum = lrSVSum   
   
       if (lstCurrInData%iTime < lstGlobalPara%stSec%iSecTradeStartTime * 1000) goto 9998 !no trade before tradestrattime
       if (lstCurrInData%iTime - lstTradeInfo%iTradeTime < lstGlobalPara%stSec%iMinTradeSec * 1000) goto 9998  ! no trade too frequency   
       if (trim(gcExch) == cCZCE .and. (abs(sum(lstCurrInData%rBP(2:5)) - sum(lstPreInData%rBP(2:5))) < 0.01 .and.  abs(sum(lstCurrInData%rSP(2:5)) - sum(lstPreInData%rSP(2:5))) < 0.01 .and. &
          (sum(lstCurrInData%iBV(2:5)) - sum(lstPreInData%iBV(2:5))) == iZERO .and. (sum(lstCurrInData%iSV(2:5)) - sum(lstPreInData%iSV(2:5))) == iZERO) .and. &
           lstCurrInData%iTotalBuyVol == lstPreInData%iTotalBuyVol .and. lstCurrInData%iTotalSelVol == lstPreInData%iTotalSelVol) goto 9998                
       
        call s_vpmodel_generate_trade(lstCurrInData, lstTradeInfo, lstPreTradeInfo, lstPosData, lstGlobalPara, liTickNo, lstTSignal) 
        
9998 continue  
        if (giHi5Type == iHI56 .or. giHi5Type == iHI57 .or. giHi5Type == iHI58 .or. giHi5Type == iHI59) then
            laiPreFullOrderBookBuy(liRefreshBidFrom : liRefreshBidTo) = laiFullOrderBookBuy(liRefreshBidFrom : liRefreshBidTo)
            laiPreFullOrderBookSel(liRefreshAskFrom : liRefreshAskTo) = laiFullOrderBookSel(liRefreshAskFrom : liRefreshAskTo)  
        end if
        
 end subroutine s_hi5_sig_gen

                                    
 subroutine s_vpmodel_generate_trade(lstCurrInData, lstTradeInfo, lstPreTradeInfo, lstPosData, lstGlobalPara, liTickNo, lstTSignal)                                   
        type(all_parameter), intent(in)	::	lstGlobalPara
        type(in_data), intent(in)		::	lstCurrInData
        type(position_data), intent(in) :: lstPosData
        integer, intent(in)  :: liTickNo
        type(trade_info), intent(inout)		::  lstTradeInfo, lstPreTradeInfo
        type(myTSignal), dimension(0:(iSIGNAL_LEN - 1))	::	lstTSignal
        real(dp) :: lrBVSum, lrSVSum, lrTick
        real(dp) :: lrBuyPrice, lrSelPrice, lrOrderP, rBSVSumRatio,rBSVChgSumRatio
        real(dp)     :: buy0,sel0
        integer  :: liClosePos,liOpenPos   
        
        lrTick = lstGlobalPara%stSec%rTickSize  
        lrBVSum = gstHi5_Paras%rBVSum
        lrSVSum = gstHi5_Paras%rSVSum           
!++++++++++++++ Signal generation +++++++++++++++++    
        buy0=lstCurrInData%rBP(1)+lrTick
        sel0=lstCurrInData%rSP(1)-lrTick
        if(lstCurrInData%iBV(1) < lstCurrInData%iSV(1) .and. lstCurrInData%iBV(1) * 2 < lstTradeInfo%iBV1Sum /liTickNo.and. trim(gcExch) /= cCFFE) buy0=lstCurrInData%rBP(1)
	    if(lstCurrInData%iSV(1) < lstCurrInData%iBV(1) .and. lstCurrInData%iSV(1) * 2 < lstTradeInfo%iSV1Sum /liTickNo.and. trim(gcExch) /= cCFFE) sel0=lstCurrInData%rSP(1)
	    if(lstCurrInData%rSP(1) < lrTick) buy0=lstCurrInData%rP			! avoid buy0=0 on UpStop
	    if(lstCurrInData%rBP(1) < lrTick) sel0=lstCurrInData%rP			! avoid sel0=0 on DnStop

        if (abs(lstCurrInData%rBP(1) - lstCurrInData%rPLmtDn) <= gstHi5_Paras%iLimitSqRange * lrTick  .or. lstCurrInData%rBP(1) < lrTick/2) gstHi5_Paras%istop=1 ! plmtdn
        if (abs(lstCurrInData%rSP(1) - lstCurrInData%rPLmtUp) <= gstHi5_Paras%iLimitSqRange * lrTick  .or. lstCurrInData%rSP(1) < lrTick/2) gstHi5_Paras%istop=1 ! plmtup
        if (lstTradeInfo%rLivePL < -1*gstHi5_Paras%rStopLossThreshold * lstCurrInData%rp * lstPosData%iMaxPOS .or. lstTradeInfo%irounds > 50) gstHi5_Paras%istop=1 !StopLoss or round
!	Cover short & buy +++++    
       if(lstCurrInData%iTime < lstGlobalPara%stSec%iSecSqTime * 1000 .and.gstHi5_Paras%istop < 1) then ! trade before  iSecSqTime
       if (lstCurrInData%rSP(1) - lstCurrInData%rBP(1) > 1.5*lrTick .and. &
          ( trim(gcExch) == cCFFE .or. trim(gcSecName) == 'JD' .or. trim(gcSecName) == 'jd' .or. trim(gcExch) == cSHFE)) goto 60        
	   if((giHi5Type <= iHI55 .and. lrBVSum > lstGlobalPara%stHi5Para%rOpenThreshold*lrSVSum) .or. &
          (giHi5Type >= iHI56 .and. gstHi5_Paras%iChgStatus == iUP .and. gstHi5_Paras%rBSVSumRatio > lstGlobalPara%stHi5Para%rOpenThreshold)) then     
           if(lstPosData%iPOSD == lstPosData%iMaxPOS .and. lstPreTradeInfo%iSignalType > 0) go to 15
           if(lstCurrInData%iTime /1000 >= lstGlobalPara%stSec%iSecOpenEndTime) goto 15
           if(abs(buy0-lstCurrInData%rPLmtDn) <= 5*lrtick) goto 15	! no buy close to DNstop
           if(lstPreTradeInfo%iSignalType == iHI5BPK .and. buy0 <= grbuyprice_old) goto 15	! keep same price BPK
           lstTradeInfo%iSignalType = iHI5BPK        
           lstTradeInfo%iTS = iBUY 
           lstTradeInfo%iNewPosStatus = iLONG
           liClosePos=lstPosData%iPOSK 
           liOpenPos=min(lstPosData%iMaxPOS, lstPosData%iMaxPOS - lstPosData%iPOSD)
           lstTradeInfo%iOpenSize = liOpenPos
           lstTradeInfo%iCloseSize = liClosePos
           lstTradeInfo%iTargetPos = lstPosData%iMaxPOS
           grbuyprice_old=buy0
           lstTradeInfo%rOrderP=f_round_number(buy0,lrTick)
           lstTradeInfo%rRecOrderP =  lstTradeInfo%rOrderP
           lstTradeInfo%rTradeBP = lstTradeInfo%rOrderP
           lstTradeInfo%rTradeSP = lstTradeInfo%rOrderP
           lstTradeInfo%iTradeTime = lstCurrInData%iTime
           lstTradeInfo%iTradeTickNo = liTickNo         
           lstTradeInfo%iVol2Trade=liClosePos+liOpenPos 
           lstTradeInfo%iVol2Pass = iZERO
           if (abs(lstTradeInfo%rOrderP - lstCurrInData%rBP(1)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iBV(1)
           if (abs(lstTradeInfo%rOrderP - lstCurrInData%rBP(2)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iBV(2)
           if (abs(lstTradeInfo%rOrderP - lstCurrInData%rBP(3)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iBV(3)
           if (abs(lstTradeInfo%rOrderP - lstCurrInData%rBP(4)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iBV(4)
           if (abs(lstTradeInfo%rOrderP - lstCurrInData%rBP(5)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iBV(5) 
           
                    if(giRunModeFlg == 1) then
                    if(gsigno==1) call s_my_trading_signal(lstTradeInfo%iSigID, gcExch, lstCurrInData%cTicker, iCD, iZERO, rZERO, rZERO, iZERO, iZERO, .TRUE., &
	                                                  giLb, giLen, lstTSignal, giNoTradeItemEachRun, lstTradeInfo%iLatestSigID-1)  ! cancel signal 
                    call s_my_trading_signal(lstTradeInfo%iSigID, gcExch, lstCurrInData%cTicker, iCD, iZERO, rZERO, rZERO, iZERO, iZERO, .TRUE., &
	                                                  giLb, giLen, lstTSignal, giNoTradeItemEachRun, lstTradeInfo%iLatestSigID)  ! cancel signal  
                    end if       
           if(giRunModeFlg == 1) then
              if(liClosePos > 0) call s_my_trading_signal(lstTradeInfo%iSigID, gcExch, lstCurrInData%cTicker, iXD, iBP,   lstTradeInfo%rOrderP, rZERO, &
			                         			       liClosePos, iZERO, .FALSE., giLb, giLen, lstTSignal, giNoTradeItemEachRun, lstTradeInfo%iLatestSigID)
              if(liOpenPos > 0)  call s_my_trading_signal(lstTradeInfo%iSigID, gcExch, lstCurrInData%cTicker, iXD, iBK,   lstTradeInfo%rOrderP, rZERO, &
				                 		               iZERO, liOpenPos, .FALSE., giLb, giLen, lstTSignal, giNoTradeItemEachRun, lstTradeInfo%iLatestSigID)
              if(liClosePos > 0 .and. liOpenPos > 0)  gsigno=1
              lstTradeInfo%iLatestSigID = lstTradeInfo%iSigID
           end if 
           !write(giFileNumber,"(a10,1x,i6,1x,i6,1x,i6)")    'sig_bpk1',liClosePos, liOpenPos, giNoTradeItemEachRun		   
15     continue 
       else if((giHi5Type <= iHI55 .and. lrBVSum > lstGlobalPara%stHi5Para%rCloseThreshold * lrSVSum).or. &
               (giHi5Type >= iHI56 .and. (gstHi5_Paras%iChgStatus == iDN .and. gstHi5_Paras%rBSVSumRatio > lstGlobalPara%stHi5Para%rCloseThreshold) .or. &
						                 (gstHi5_Paras%iChgStatus == iUP .and. gstHi5_Paras%rBSVSumRatio > rZERO))) then
            if(lstPosData%iPOSK == 0 .and.  lstPreTradeInfo%iSignalType > 0) go to 10
	        if(lstPosData%iPOSK == 0 .and.  abs(lstPreTradeInfo%iSignalType) /= iHI5K) goto 10
!           if(lstPreTradeInfo%iSignalType == iHI5BPK .or. (lstPreTradeInfo%iSignalType==iHI5BP.and. buy0 <=grbuyprice_old)) goto 10
            if(lstPreTradeInfo%iSignalType > 0.and. buy0 <= grbuyprice_old) goto 10
            lstTradeInfo%iSignalType = iHI5BP 
            lstTradeInfo%iTS = iBUY 
            lstTradeInfo%iNewPosStatus = iFLAT
            liClosePos=lstPosData%iPOSK
            liOpenPos=iZERO
            lstTradeInfo%iOpenSize = liOpenPos
            lstTradeInfo%iCloseSize = liClosePos
            lstTradeInfo%iTargetPos = iZERO       
            grbuyprice_old=buy0
            lstTradeInfo%rOrderP=f_round_number(buy0,lrTick)
            lstTradeInfo%rRecOrderP =  lstTradeInfo%rOrderP
            lstTradeInfo%rTradeBP = lstTradeInfo%rOrderP
            lstTradeInfo%rTradeSP = lstTradeInfo%rOrderP
            lstTradeInfo%iTradeTime = lstCurrInData%iTime
            lstTradeInfo%iTradeTickNo = liTickNo         
            lstTradeInfo%iVol2Trade=liClosePos  
            lstTradeInfo%iVol2Pass = iZERO
            if (abs(lstTradeInfo%rOrderP - lstCurrInData%rBP(1)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iBV(1)
            if (abs(lstTradeInfo%rOrderP - lstCurrInData%rBP(2)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iBV(2)
            if (abs(lstTradeInfo%rOrderP - lstCurrInData%rBP(3)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iBV(3)
            if (abs(lstTradeInfo%rOrderP - lstCurrInData%rBP(4)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iBV(4)
            if (abs(lstTradeInfo%rOrderP - lstCurrInData%rBP(5)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iBV(5)
            
                    if(giRunModeFlg == 1) then
                    if(gsigno==1) call s_my_trading_signal(lstTradeInfo%iSigID, gcExch, lstCurrInData%cTicker, iCD, iZERO, rZERO, rZERO, iZERO, iZERO, .TRUE., &
	                                                  giLb, giLen, lstTSignal, giNoTradeItemEachRun, lstTradeInfo%iLatestSigID-1)  ! cancel signal 
                    call s_my_trading_signal(lstTradeInfo%iSigID, gcExch, lstCurrInData%cTicker, iCD, iZERO, rZERO, rZERO, iZERO, iZERO, .TRUE., &
	                                                  giLb, giLen, lstTSignal, giNoTradeItemEachRun, lstTradeInfo%iLatestSigID)  ! cancel signal  
                    end if      
            if(giRunModeFlg == 1)   then
               if(liClosePos > 0) call s_my_trading_signal(lstTradeInfo%iSigID, gcExch, lstCurrInData%cTicker, iXD, iBP, lstTradeInfo%rOrderP, rZERO, &
						                             liClosePos, iZERO, .FALSE., giLb, giLen, lstTSignal, giNoTradeItemEachRun, lstTradeInfo%iLatestSigID) 
               gsigno=0
               lstTradeInfo%iLatestSigID = lstTradeInfo%iSigID
            end if 
           !write(giFileNumber,"(a10,1x,i6,1x,i6,1x,i6)")    'sig_bp2',liClosePos, liOpenPos, giNoTradeItemEachRun					
10     continue 
       else if((giHi5Type <= iHI55 .and. lrSVSum > lstGlobalPara%stHi5Para%rOpenThreshold * lrBVSum) .or. &
               (giHi5Type >= iHI56 .and. gstHi5_Paras%iChgStatus == iDN .and. gstHi5_Paras%rBSVSumRatio < -lstGlobalPara%stHi5Para%rOpenThreshold)) then    
            if(lstPosData%iPOSK == lstPosData%iMaxPOS .and. lstPreTradeInfo%iSignalType < 0) go to 25
            if(lstCurrInData%iTime /1000 >= lstGlobalPara%stSec%iSecOpenEndTime) goto 25
            if(abs(sel0-lstCurrInData%rPLmtUp) <= 5*lrtick) goto 25	! no sell close to UPstop
	        if(lstPreTradeInfo%iSignalType == iHI5SPK .and. sel0 >= grselprice_old) goto 25	! keep same price SPK
            lstTradeInfo%iSignalType = iHI5SPK        
            lstTradeInfo%iTS = iSELL 
            lstTradeInfo%iNewPosStatus = iSHORT;
            liClosePos=lstPosData%iPOSD
            liOpenPos=min(lstPosData%iMaxPOS, lstPosData%iMaxPOS - lstPosData%iPOSK)
            lstTradeInfo%iOpenSize = liOpenPos
            lstTradeInfo%iCloseSize = liClosePos
            lstTradeInfo%iTargetPos = -1*lstPosData%iMaxPOS
            grselprice_old=sel0
            lstTradeInfo%rOrderP=f_round_number(sel0,lrTick)
            lstTradeInfo%rRecOrderP =  lstTradeInfo%rOrderP
            lstTradeInfo%rTradeBP = lstTradeInfo%rOrderP
            lstTradeInfo%rTradeSP = lstTradeInfo%rOrderP
            lstTradeInfo%iTradeTime = lstCurrInData%iTime
            lstTradeInfo%iTradeTickNo = liTickNo         
            lstTradeInfo%iVol2Trade=-1*(liClosePos+liOpenPos)   
            lstTradeInfo%iVol2Pass = iZERO
            if (abs(lstTradeInfo%rOrderP - lstCurrInData%rSP(1)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iSV(1)
            if (abs(lstTradeInfo%rOrderP - lstCurrInData%rSP(2)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iSV(2)
            if (abs(lstTradeInfo%rOrderP - lstCurrInData%rSP(3)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iSV(3)
            if (abs(lstTradeInfo%rOrderP - lstCurrInData%rSP(4)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iSV(4)
            if (abs(lstTradeInfo%rOrderP - lstCurrInData%rSP(5)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iSV(5) 
            
                    if(giRunModeFlg == 1) then
                    if(gsigno==1) call s_my_trading_signal(lstTradeInfo%iSigID, gcExch, lstCurrInData%cTicker, iCD, iZERO, rZERO, rZERO, iZERO, iZERO, .TRUE., &
	                                                  giLb, giLen, lstTSignal, giNoTradeItemEachRun, lstTradeInfo%iLatestSigID-1)  ! cancel signal 
                    call s_my_trading_signal(lstTradeInfo%iSigID, gcExch, lstCurrInData%cTicker, iCD, iZERO, rZERO, rZERO, iZERO, iZERO, .TRUE., &
	                                                  giLb, giLen, lstTSignal, giNoTradeItemEachRun, lstTradeInfo%iLatestSigID)  ! cancel signal  
                    end if        
            if(giRunModeFlg == 1) then
               if(liClosePos > 0)  call s_my_trading_signal(lstTradeInfo%iSigID, gcExch, lstCurrInData%cTicker, iXD, iSP,   rZERO, lstTradeInfo%rOrderP, &
                                                     liClosePos, iZERO, .FALSE., giLb, giLen, lstTSignal, giNoTradeItemEachRun, lstTradeInfo%iLatestSigID)
               if(liOpenPos > 0)   call s_my_trading_signal(lstTradeInfo%iSigID, gcExch, lstCurrInData%cTicker, iXD, iSK, rZERO, lstTradeInfo%rOrderP, &
						                             iZERO, liOpenPos, .FALSE., giLb, giLen, lstTSignal, giNoTradeItemEachRun, lstTradeInfo%iLatestSigID)
               if(liClosePos > 0 .and. liOpenPos > 0)  gsigno=1
               lstTradeInfo%iLatestSigID = lstTradeInfo%iSigID
            end if
           !write(giFileNumber,"(a10,1x,i6,1x,i6,1x,i6)")    'sig_spk3',liClosePos, liOpenPos, giNoTradeItemEachRun				
    25  continue        
    else if((giHi5Type <= iHI55 .and. lrSVSum > lstGlobalPara%stHi5Para%rCloseThreshold*lrBVSum) .or. &
            (giHi5Type >= iHI56 .and. (gstHi5_Paras%iChgStatus == iUP .and. gstHi5_Paras%rBSVSumRatio < -lstGlobalPara%stHi5Para%rCloseThreshold) .or. &
						              (gstHi5_Paras%iChgStatus == iDN .and. gstHi5_Paras%rBSVSumRatio < rZERO))) then     
            if(lstPosData%iPOSD == iZERO .and.  lstPreTradeInfo%iSignalType < 0) go to 20
            if(lstPosData%iPOSD == iZERO .and. abs(lstPreTradeInfo%iSignalType) /= iHI5K) goto 20
!           if(lstPreTradeInfo%iSignalType == iHI5SPK .or. (lstPreTradeInfo%iSignalType == iHI5SP.and. sel0 >= grselprice_old)) goto 20	! keep SPK & same-price SP
            if(lstPreTradeInfo%iSignalType < 0 .and. sel0 >= grselprice_old) goto 20	! keep SPK & same-price SP    
            lstTradeInfo%iSignalType = iHI5SP        
            lstTradeInfo%iTS = iSELL; 
            lstTradeInfo%iNewPosStatus = iFLAT;
            liClosePos=lstPosData%iPOSD
            liOpenPos=iZERO
            lstTradeInfo%iOpenSize = liOpenPos
            lstTradeInfo%iCloseSize = liClosePos
            lstTradeInfo%iTargetPos = iZERO
            grselprice_old=sel0
            lstTradeInfo%rOrderP=f_round_number(sel0,lrTick)
            lstTradeInfo%rRecOrderP =  lstTradeInfo%rOrderP
            lstTradeInfo%rTradeBP = lstTradeInfo%rOrderP
            lstTradeInfo%rTradeSP = lstTradeInfo%rOrderP
            lstTradeInfo%iTradeTime = lstCurrInData%iTime
            lstTradeInfo%iTradeTickNo = liTickNo         
            lstTradeInfo%iVol2Trade=-1*liClosePos
            lstTradeInfo%iVol2Pass = iZERO
            if (abs(lstTradeInfo%rOrderP - lstCurrInData%rSP(1)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iSV(1)
            if (abs(lstTradeInfo%rOrderP - lstCurrInData%rSP(2)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iSV(2)
            if (abs(lstTradeInfo%rOrderP - lstCurrInData%rSP(3)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iSV(3)
            if (abs(lstTradeInfo%rOrderP - lstCurrInData%rSP(4)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iSV(4)
            if (abs(lstTradeInfo%rOrderP - lstCurrInData%rSP(5)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iSV(5)
            
                    if(giRunModeFlg == 1) then
                    if(gsigno==1) call s_my_trading_signal(lstTradeInfo%iSigID, gcExch, lstCurrInData%cTicker, iCD, iZERO, rZERO, rZERO, iZERO, iZERO, .TRUE., &
	                                                  giLb, giLen, lstTSignal, giNoTradeItemEachRun, lstTradeInfo%iLatestSigID-1)  ! cancel signal 
                    call s_my_trading_signal(lstTradeInfo%iSigID, gcExch, lstCurrInData%cTicker, iCD, iZERO, rZERO, rZERO, iZERO, iZERO, .TRUE., &
	                                                  giLb, giLen, lstTSignal, giNoTradeItemEachRun, lstTradeInfo%iLatestSigID)  ! cancel signal  
                    end if      
            if(giRunModeFlg == 1) then
               if(liClosePos > 0 ) call s_my_trading_signal(lstTradeInfo%iSigID, gcExch, lstCurrInData%cTicker, iXD, iSP, rZERO, lstTradeInfo%rOrderP, &
                                                     liClosePos, iZERO, .FALSE., giLb, giLen, lstTSignal, giNoTradeItemEachRun, lstTradeInfo%iLatestSigID)
               gsigno=0
               lstTradeInfo%iLatestSigID = lstTradeInfo%iSigID
            end if
           !write(giFileNumber,"(a10,1x,i6,1x,i6,1x,i6)")    'sig_sp4',liClosePos, liOpenPos, giNoTradeItemEachRun					
    20  continue
    end if
    else if(lstCurrInData%iTime > lstGlobalPara%stSec%iSecSqTime * 1000 .or. gstHi5_Paras%istop == 1) then  ! just bp or sp after iSecSqTime
!++++++++setting trade price +++++++             
            buy0 = lstCurrInData%rSP(1)
	        sel0 = lstCurrInData%rBP(1)   
            if(lstCurrInData%rSP(1) < lrtick) buy0=lstCurrInData%rP
            if(lstCurrInData%rBP(1) < lrtick) sel0=lstCurrInData%rP
            if(lstPosData%iPOSK > 0)then
               if(lstPosData%iPOSK /= 0 .and. (lstCurrInData%rBP(1) < lrtick .and. lstCurrInData%iBV(1) == 0)) goto 60 ! keep short position on DnStop
               if(lstPreTradeInfo%iSignalType == iHI5BP .and. buy0 < grbuyprice_old) goto 60
                  lstTradeInfo%iSignalType = iHI5BP        
                  lstTradeInfo%iTS = iBUY; 
                  lstTradeInfo%iNewPosStatus = iFLAT;
                  liClosePos=abs(lstTradeInfo%iPos)
                  liOpenPos=iZERO
                  lstTradeInfo%iOpenSize = liOpenPos
                  lstTradeInfo%iCloseSize = liClosePos
                  lstTradeInfo%iTargetPos = iZERO
                  grbuyprice_old=buy0
                  lstTradeInfo%rOrderP=f_round_number(buy0,lrTick)
                  lstTradeInfo%rRecOrderP =  lstTradeInfo%rOrderP
                  lstTradeInfo%rTradeBP = lstTradeInfo%rOrderP
                  lstTradeInfo%rTradeSP = lstTradeInfo%rOrderP
                  lstTradeInfo%iTradeTime = lstCurrInData%iTime
                  lstTradeInfo%iTradeTickNo = liTickNo         
                  lstTradeInfo%iVol2Trade=liClosePos 
                 lstTradeInfo%iVol2Pass = iZERO
                 if (abs(lstTradeInfo%rOrderP - lstCurrInData%rBP(1)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iBV(1) 
                 
                    if(giRunModeFlg == 1) then
                    if(gsigno==1) call s_my_trading_signal(lstTradeInfo%iSigID, gcExch, lstCurrInData%cTicker, iCD, iZERO, rZERO, rZERO, iZERO, iZERO, .TRUE., &
	                                                  giLb, giLen, lstTSignal, giNoTradeItemEachRun, lstTradeInfo%iLatestSigID-1)  ! cancel signal 
                    call s_my_trading_signal(lstTradeInfo%iSigID, gcExch, lstCurrInData%cTicker, iCD, iZERO, rZERO, rZERO, iZERO, iZERO, .TRUE., &
	                                                  giLb, giLen, lstTSignal, giNoTradeItemEachRun, lstTradeInfo%iLatestSigID)  ! cancel signal  
                    end if       
                  if(giRunModeFlg == 1) then
                     if(liClosePos > 0) call s_my_trading_signal(lstTradeInfo%iSigID, gcExch, lstCurrInData%cTicker, iXD, iBP, lstTradeInfo%rOrderP, rZERO, &
						                             liClosePos, iZERO, .FALSE., giLb, giLen, lstTSignal, giNoTradeItemEachRun, lstTradeInfo%iLatestSigID)
                     gsigno=0
                     lstTradeInfo%iLatestSigID = lstTradeInfo%iSigID
                 end if 
              !write(giFileNumber,"(a10,1x,i6,1x,i6,1x,i6)")    'sig_bp5',liClosePos, liOpenPos, giNoTradeItemEachRun					 
              else if(lstPosData%iPOSD > 0 ) then
                 if(lstPosData%iPOSD  /= 0 .and. (lstCurrInData%rSP(1) < lrtick .and. lstCurrInData%iSV(1) ==0)) goto 60	! keep long position on UpStop
	             if(lstPreTradeInfo%iSignalType == iHI5SP .and. sel0 > grselprice_old) goto 60 
                    lstTradeInfo%iSignalType = iHI5SP
                    lstTradeInfo%iTS = iSELL; 
                    lstTradeInfo%iNewPosStatus = iFLAT;
                    liClosePos=abs(lstTradeInfo%iPos)
                    liOpenPos=iZERO
                    lstTradeInfo%iOpenSize = liOpenPos
                    lstTradeInfo%iCloseSize = liClosePos
                    lstTradeInfo%iTargetPos = iZERO
                    grselprice_old=sel0
                    lstTradeInfo%rOrderP=f_round_number(sel0,lrTick)
                    lstTradeInfo%rRecOrderP =  lstTradeInfo%rOrderP
                    lstTradeInfo%rTradeBP = lstTradeInfo%rOrderP
                    lstTradeInfo%rTradeSP = lstTradeInfo%rOrderP
                    lstTradeInfo%iTradeTime = lstCurrInData%iTime
                    lstTradeInfo%iTradeTickNo = liTickNo  
                    lstTradeInfo%iVol2Trade=-1*liClosePos   
                   lstTradeInfo%iVol2Pass = iZERO
                   if (abs(lstTradeInfo%rOrderP - lstCurrInData%rSP(1)) < lrTick/2) lstTradeInfo%iVol2Pass  = lstCurrInData%iSV(1)                     
                    if(giRunModeFlg == 1) then
                    if(gsigno==1) call s_my_trading_signal(lstTradeInfo%iSigID, gcExch, lstCurrInData%cTicker, iCD, iZERO, rZERO, rZERO, iZERO, iZERO, .TRUE., &
	                                                  giLb, giLen, lstTSignal, giNoTradeItemEachRun, lstTradeInfo%iLatestSigID-1)  ! cancel signal 
                    call s_my_trading_signal(lstTradeInfo%iSigID, gcExch, lstCurrInData%cTicker, iCD, iZERO, rZERO, rZERO, iZERO, iZERO, .TRUE., &
	                                                  giLb, giLen, lstTSignal, giNoTradeItemEachRun, lstTradeInfo%iLatestSigID)  ! cancel signal  
                    end if 
                   if(giRunModeFlg == 1) then
                     if(liClosePos > 0) call s_my_trading_signal(lstTradeInfo%iSigID, gcExch, lstCurrInData%cTicker, iXD, iSP, rZERO, lstTradeInfo%rOrderP, &
                                                                 liClosePos, iZERO, .FALSE., giLb, giLen, lstTSignal, giNoTradeItemEachRun, lstTradeInfo%iLatestSigID)
                     gsigno=0
                     lstTradeInfo%iLatestSigID = lstTradeInfo%iSigID
                   end if 
              !write(giFileNumber,"(a10,1x,i6,1x,i6,1x,i6)")    'sig_sp6',liClosePos, liOpenPos, giNoTradeItemEachRun					   
             end if
    end if
60  continue                                      
  
end subroutine  s_vpmodel_generate_trade                                    
                                    
   
     !----------------------------------------------------
    ! subroutine s_hi50_cal()
    ! description: hi5 BVSum and SVSum calculation
    !----------------------------------------------------   
    subroutine s_hi50_cal(larBP, larSP, laiBV, laiSV, lrVolSel, lrVolBuy, lrTick, lrBVSum, lrSVSum) 
        implicit none
        real(dp), intent(in) :: lrVolSel, lrVolBuy,  larBP(30), larSP(30), lrTick
        integer, intent(inout)  :: laiBV(30), laiSV(30)
        real(dp), intent(inout) :: lrBVSum, lrSVSum

        lrBVSum = 2*laiBV(1)+ laiBV(2)*lrTick/max(larBP(1)-larBP(2), lrTick) &    ! price-adjusted volume
                + 2*laiBV(3)*lrTick/max(larBP(1)-larBP(3), lrTick) &
                + 3*laiBV(4)*lrTick/max(larBP(1)-larBP(4), lrTick) &
                + 4*laiBV(5)*lrTick/max(larBP(1)-larBP(5), lrTick) 
        lrSVSum = 2*laiSV(1)+ laiSV(2)*lrTick/max(larSP(2)-larSP(1), lrTick) &
                + 2*laiSV(3)*lrTick/max(larSP(3)-larSP(1), lrTick) &
                + 3*laiSV(4)*lrTick/max(larSP(4)-larSP(1), lrTick) &
                + 4*laiSV(5)*lrTick/max(larSP(5)-larSP(1), lrTick) 
        lrBVSum = lrBVSum +  gstGlobalPara%stHi5Para%rMKTThreshold * lrVolSel
        lrSVSum = lrSVSum +  gstGlobalPara%stHi5Para%rMKTThreshold * lrVolBuy

    end subroutine s_hi50_cal
    
                    
    

    !----------------------------------------------------
    ! subroutine f_calc_ratio() 
    ! description: calculate BSV and BSVChg ratio
    !----------------------------------------------------   
	real(dp) function f_calc_ratio(lrBVSum, lrSVSum)
		implicit none
		real(dp), intent(in)	::	lrBVSum, lrSVSum
		
		f_calc_ratio = rZERO
		if (lrBVSum > rZERO .and. lrSVSum > rZERO) then
			if (lrBVSum > lrSVSum) then
				f_calc_ratio = lrBVSum/lrSVSum
			else
				f_calc_ratio = -lrSVSum/lrBVSum
			end if
		end if
		if (lrBVSum > rZERO .and. lrSVSum < rZERO) then
			if (lrBVSum > abs(lrSVSum)) then
				f_calc_ratio = -lrBVSum/lrSVSum
			else
				f_calc_ratio = -lrSVSum/lrBVSum
			end if
		end if
		if (lrBVSum < rZERO .and. lrSVSum > rZERO) then
			if (abs(lrBVSum) > lrSVSum) then
				f_calc_ratio = lrBVSum/lrSVSum
			else
				f_calc_ratio = lrSVSum/lrBVSum
			end if
		end if
		if (lrBVSum < rZERO .and. lrSVSum < rZERO) then
			if (-lrBVSum > -lrSVSum) then
				f_calc_ratio = -lrBVSum/lrSVSum
			else
				f_calc_ratio = lrSVSum/lrBVSum
			end if
		end if
    end function f_calc_ratio    
    
    !----------------------------------------------------
    ! subroutine f_calc_change_direction() 
    ! description: calculate BSV change direction
    !----------------------------------------------------  
	integer function f_calc_change_direction(lrBVChgSum, lrSVChgSum, lrChgRatio, lrChgThreshold)
		implicit none
		real(dp), intent(in)	::	lrBVChgSum, lrSVChgSum, lrChgRatio, lrChgThreshold
		f_calc_change_direction = iFLAT
		if (lrChgRatio > lrChgThreshold) f_calc_change_direction = iUP
		if (lrChgRatio < -lrChgThreshold) f_calc_change_direction = iDN
		if (lrChgRatio == rZERO) then
			if (lrBVChgSum > iZERO .or. lrSVChgSum < iZERO) f_calc_change_direction = iUP
			if (lrBVChgSum < iZERO .or. lrSVChgSum > iZERO) f_calc_change_direction = iDN
			if (lrBVChgSum == iZERO .and. lrSVChgSum == iZERO) f_calc_change_direction = iFLAT
		end if
    end function f_calc_change_direction                          

   !----------------------------------------------------
    ! subroutine s_hi5_output_log_new(liTickNo, liFileNumber)
    ! description: output log
    !----------------------------------------------------   

    subroutine s_hi5_output_log_new(liTickNo, liFileNumber) 
        implicit none
        integer, intent(in) :: liTickNo, liFileNumber
        integer :: liIndex
        real(dp) :: lrLivePL, lrBuyPrice, lrSelPrice        
        character ::  cStrFmt*5, cOutFmt*512
        lrLivePL = gstCurrTradeInfo%rCash + gstCurrTradeInfo%iPos * gstCurrIn%rP
        lrBuyPrice = 0
        lrSelPrice = 0
        if (gstCurrTradeInfo%iTS > 0) then
            lrBuyPrice = gstCurrTradeInfo%rOrderP
        else if (gstCurrTradeInfo%iTS < 0) then
            lrSelPrice = gstCurrTradeInfo%rOrderP
        end if     
        
        if (giRunModeFlg /= 1) then       
       ! common output        
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%iTime = gstCurrIn%iTime
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%cTicker = trim(gstCurrIn%cTicker)
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%iRecordNo = liTickNo
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%rP = gstCurrIn%rP
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%iV = gstCurrIn%iVol
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%rBP1 = gstCurrIn%rBP(1) 
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%iBV1 = gstRecCurrIn%iBV(1)
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%rSP1 = gstCurrIn%rSP(1)        
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%iSV1 = gstRecCurrIn%iSV(1)
!        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%iBV1 = gstRecCurrIn%iTOTALBUYVOL
!        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%iSV1 = gstRecCurrIn%iTOTALSELVOL
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%rVal = gstCurrIn%rVal
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%rOI = gstCurrIn%rOI
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%iHaveOrder = f_sign(giNoTradeItemEachRun) 
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%iTS = gstCurrTradeInfo%iTS
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%iCancel = gstCurrTradeInfo%iCancelFlg
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%rBuyPrice = lrBuyPrice
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%rSelPrice = lrSelPrice
!       gastHi5OutData(giHi5OutDataIndex)%stCommOutData%rBuyPrice = gstCurrIn%rWEIGHTEDBUYORDERP 
!       gastHi5OutData(giHi5OutDataIndex)%stCommOutData%rSelPrice = gstCurrIn%rWEIGHTEDSELORDERP
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%iOpenSize = gstCurrTradeInfo%iOpenSize
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%iCloseSize = gstCurrTradeInfo%iCloseSize
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%iToTradeSize = gstCurrTradeInfo%iVol2Trade
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%iMaxPos = gstPosData%iMaxPos
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%iLongPos = gstPosData%iPOSD
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%iShortPos = -gstPosData%iPOSK        
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%iTotalOrderSize = gstCurrTradeInfo%iTotalOrderSize
        gastHi5OutData(giHi5OutDataIndex)%stCommOutData%iTotalCancelSize = izero          
        ! specific output 
        gastHi5OutData(giHi5OutDataIndex)%iBV1 = gstCurrIn%iBV(1)     
        gastHi5OutData(giHi5OutDataIndex)%iSV1 = gstCurrIn%iSV(1)        
        gastHi5OutData(giHi5OutDataIndex)%iNewPOS = gstCurrTradeInfo%iNewPosStatus
!       gastHi5OutData(giHi5OutDataIndex)%rCash = gstCurrTradeInfo%rCash; gastHi5OutData(giHi5OutDataIndex)%rLivePL = lrLivePL        
        gastHi5OutData(giHi5OutDataIndex)%rCash = gstHi5_Paras%rBVSum; gastHi5OutData(giHi5OutDataIndex)%rLivePL = gstHi5_Paras%rSVSum  
        
!        gastHi5OutData(giHi5OutDataIndex)%iNetPOS = gstCurrTradeInfo%iPos
        gastHi5OutData(giHi5OutDataIndex)%iRound = gstCurrTradeInfo%iRounds
!        gastHi5OutData(giHi5OutDataIndex)%iRound = gstHi5_Paras%iChgStatus
        gastHi5OutData(giHi5OutDataIndex)%iCanCelOrder = iZero
        gastHi5OutData(giHi5OutDataIndex)%rPNet = gstHi5_Paras%rPNet
        gastHi5OutData(giHi5OutDataIndex)%rBVSVRatio =  min(999d0, gstHi5_Paras%rBVSum/max(gstHi5_Paras%rSVSum,1d0))
!       gastHi5OutData(giHi5OutDataIndex)%rBVSVRatio =   gstHi5_Paras%rBSVSumRatio
        if (giNoTradeItemEachRun == 0 .or. giHi5OutDataIndex == iOUT_LEN) then
			cStrFmt = 'F14.'//f_int_2_cha1(gstGlobalPara%stSec%iDecimalPt)            
			cOutFmt = '(i12,1x, a6, 1x,'//cStrFmt//',2(1x,i12),2(1x,f12.4), 1x, i12, 1x, i14, 1x, i12,'//&
                      '3(1x, i8), 2(1x, f12.4), 6(1x, i8),'//&
                      '1x, f12.4, 2(1x, f16.2),1x, i8) '
          
            if (giHi5TraceInit == 0) then
                   write (liFileNumber, '(25a18)') 'time','symbol','price','vol','bv1','bp1','sp1','sv1', 'amt', 'oi',&
							'iorder','ibs','icancel','buy_price','sell_price','open_vol','close_vol', 'pending_vol', 'limit_vol', 'long_pos', 'short_pos', &
                            'bsvratio', 'cash','live','round'                        
                giHi5TraceInit = 1
            end if
            Do liIndex = 1, giHi5OutDataIndex, 1
                     write(liFileNumber,trim(cOutFmt)) gastHi5OutData(liIndex)%stCommOutData%iTime,&
                                                      gastHi5OutData(liIndex)%stCommOutData%cTicker,&
                                                      gastHi5OutData(liIndex)%stCommOutData%rP,&
                                                      gastHi5OutData(liIndex)%stCommOutData%iV, &
                                                      gastHi5OutData(liIndex)%stCommOutData%iBV1,&
                                                      gastHi5OutData(liIndex)%stCommOutData%rBP1, &
                                                      gastHi5OutData(liIndex)%stCommOutData%rSP1,&        
                                                      gastHi5OutData(liIndex)%stCommOutData%iSV1,&
                                                      int(gastHi5OutData(liIndex)%stCommOutData%rVal),&
                                                      int(gastHi5OutData(liIndex)%stCommOutData%rOI), &
                                                      gastHi5OutData(liIndex)%stCommOutData%iHaveOrder,& 
                                                      gastHi5OutData(liIndex)%stCommOutData%iTS,&
                                                      gastHi5OutData(liIndex)%stCommOutData%iCancel,&
                                                      gastHi5OutData(liIndex)%stCommOutData%rBuyPrice,&
                                                      gastHi5OutData(liIndex)%stCommOutData%rSelPrice, &
                                                      gastHi5OutData(liIndex)%stCommOutData%iOpenSize, &
                                                      gastHi5OutData(liIndex)%stCommOutData%iCloseSize,&
                                                      gastHi5OutData(liIndex)%stCommOutData%iToTradeSize,&
                                                      gastHi5OutData(liIndex)%stCommOutData%iMaxPos,&
                                                      gastHi5OutData(liIndex)%stCommOutData%iLongPos,&
                                                      gastHi5OutData(liIndex)%stCommOutData%iShortPos,&
                                                      gastHi5OutData(liIndex)%rBVSVRatio,  &
                                                      gastHi5OutData(liIndex)%rCash, gastHi5OutData(liIndex)%rLivePL,&

                        gastHi5OutData(liIndex)%iRound
             end Do  
            giHi5OutDataIndex = 0
        end if
        giHi5OutDataIndex = mod(giHi5OutDataIndex, iOUT_LEN) + 1
        else if(giRunModeFlg == 1)  then
        gstrat_out_log%exch_time=gstCurrIn%iTime
        !gstrat_out_log%contract=trim(gstCurrIn%cTicker)
        gstrat_out_log%contract=f_convert_f_string_to_c_char(trim(gstCurrIn%cTicker), len_trim(gstCurrIn%cTicker), 1, 16)
        gstrat_out_log%n_tick=liTickNo
        gstrat_out_log%price=gstCurrIn%rP
        gstrat_out_log%vol=gstCurrIn%iVol
        gstrat_out_log%bv1=gstRecCurrIn%iBV(1)
        gstrat_out_log%bp1=gstCurrIn%rBP(1)
        gstrat_out_log%sp1=gstCurrIn%rSP(1)
        gstrat_out_log%sv1=gstRecCurrIn%iSV(1)
        gstrat_out_log%amt=gstCurrIn%rVal
        gstrat_out_log%oi=gstCurrIn%rOI
        gstrat_out_log%buy_price=lrBuyPrice
        gstrat_out_log%sell_price=lrSelPrice
        gstrat_out_log%open_vol=gstCurrTradeInfo%iOpenSize
        gstrat_out_log%close_vol=gstCurrTradeInfo%iCloseSize
        gstrat_out_log%long_pos=gstPosData%iPOSD
        gstrat_out_log%short_pos=-gstPosData%iPOSK 
        gstrat_out_log%tot_ordervol=gstCurrTradeInfo%iVol2Trade
        gstrat_out_log%tot_cancelvol=izero
        gstrat_out_log%order_cnt=f_sign(giNoTradeItemEachRun)
        gstrat_out_log%cancel_cnt=gstCurrTradeInfo%iCancelFlg
        !gstrat_out_log%cash=gstCurrTradeInfo%rCash
        !gstrat_out_log%live=lrLivePL
        gstrat_out_log%cash=gstHi5_Paras%rBVSum
        gstrat_out_log%live=gstHi5_Paras%rSVSum
        gstrat_out_log%tot_vol=gstCurrIn%rTotV 
        gstrat_out_log%max_dd=gstHi5_Paras%rPNet
        gstrat_out_log%max_net_pos=gstPosData%iPOSD+gstPosData%iPOSK
        gstrat_out_log%max_side_pos=gstPosData%iMaxPos
        gstrat_out_log%sig=gstCurrTradeInfo%iTS*1.0_dp
        end if    
        
    end subroutine s_hi5_output_log_new      
                         
    !----------------------------------------------------
    ! subroutine s_hi5_set_parameters() 
    ! description: set Hi5 global parameters, incluing closing time and threshold for each strategy
    !----------------------------------------------------   
    subroutine s_hi5_set_parameters()  
        implicit none
           if(trim(gcExch)==cCFFE)  then
              gstGlobalPara%stSec%iSecOpenEndTime = 145000                
                select case (giHi5Type)
                       case (iHI50)
                            gstGlobalPara%stSec%iSecSqTime = 145450
                        case (iHI51)
                            gstGlobalPara%stSec%iSecSqTime = 145453
                        case (iHI52)
                            gstGlobalPara%stSec%iSecSqTime = 145456
                        case (iHI53)
                            gstGlobalPara%stSec%iSecSqTime = 145459
                        case (iHI54)
                            gstGlobalPara%stSec%iSecSqTime = 145505
                        case (iHI55)
                            gstGlobalPara%stSec%iSecSqTime = 145508
                        case (iHI56)
                            gstGlobalPara%stSec%iSecSqTime = 145511
                        case (iHI57)
                            gstGlobalPara%stSec%iSecSqTime = 145514
                        case (iHI58)
                            gstGlobalPara%stSec%iSecSqTime = 145515
                        case (iHI59)
                            gstGlobalPara%stSec%iSecSqTime = 145518
                        case default
                end select                                    
                if ((trim(gcSecName) /= 'tf') .and. (trim(gcSecName) /= 't') .and. (trim(gcSecName) /= 'TF') .and. (trim(gcSecName) /= 'T')) then
                     gstGlobalPara%stSec%iSecTradeStartTime = 93000
                     gstGlobalPara%stSec%iSecOpenEndTime = 145800
                     gstGlobalPara%stSec%iSecSqTime = 145845                    
                end if
           else      
           select case (giHi5Type)
            case (iHI50)
                gstGlobalPara%stSec%iSecSqTime = gstGlobalPara%stSec%iSecEndTime + 5 
            case (iHI51)
                gstGlobalPara%stSec%iSecSqTime = gstGlobalPara%stSec%iSecEndTime + 15 
            case (iHI52)
                gstGlobalPara%stSec%iSecSqTime = gstGlobalPara%stSec%iSecEndTime + 25 
            case (iHI53)
                gstGlobalPara%stSec%iSecSqTime = gstGlobalPara%stSec%iSecEndTime + 35  
            case (iHI54)
                gstGlobalPara%stSec%iSecSqTime = gstGlobalPara%stSec%iSecEndTime + 45                                    
            case (iHI55)
                gstGlobalPara%stSec%iSecSqTime = gstGlobalPara%stSec%iSecEndTime + 55    
            case (iHI56)
                gstGlobalPara%stSec%iSecSqTime = gstGlobalPara%stSec%iSecEndTime + 105    
            case (iHI57)
                gstGlobalPara%stSec%iSecSqTime = gstGlobalPara%stSec%iSecEndTime + 115    
            case (iHI58)
                gstGlobalPara%stSec%iSecSqTime = gstGlobalPara%stSec%iSecEndTime + 125    
            case (iHI59)
                gstGlobalPara%stSec%iSecSqTime = gstGlobalPara%stSec%iSecEndTime + 135    
            case default
            end select 
		    if (mod(int(gstGlobalPara%stSec%iSecSqTime/100), 100) == iZERO) then
			    gstGlobalPara%stSec%iSecOpenEndTime = gstGlobalPara%stSec%iSecSqTime - 4100
		    else
			    gstGlobalPara%stSec%iSecOpenEndTime = gstGlobalPara%stSec%iSecSqTime - 100
            end if
            end if
            
            gstGlobalPara%stHi5Para%rMKTThreshold = 1.0_dp; gstGlobalPara%stHi5Para%rOpenThreshold = 999.0_dp; gstGlobalPara%stHi5Para%rCloseThreshold = 999.0_dp   
            
            if (giInnerPar1Flg == iNO) gstGlobalPara%stHi5Para%rMKTThreshold   = giSimuPara1 * 0.1_dp
            if (giInnerPar2Flg == iNO) gstGlobalPara%stHi5Para%rOpenThreshold  = giSimuPara2 * 0.1_dp
            if (giInnerPar3Flg == iNO) gstGlobalPara%stHi5Para%rCloseThreshold = giSimuPara3 * 0.1_dp
             gstGlobalPara%stSec%iSecTradeEndTime = gstGlobalPara%stSec%iSecSqTime
       !     if (giRunModeFlg == 1) then                  
                gstGlobalPara%stSec%iSecTradeStartTime = gstInputParas%iInputTradeStartTime
                gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime
                gstGlobalPara%stSec%iSecOpenEndTime = gstInputParas%iInputOpenEndTime
                gstGlobalPara%stHi5Para%rMKTThreshold = gstInputParas%rInputPara1
                gstGlobalPara%stHi5Para%rOpenThreshold = gstInputParas%rInputPara2
                gstGlobalPara%stHi5Para%rCloseThreshold = gstInputParas%rInputPara3                
                gstGlobalPara%stSec%rRssTCostByVol = gstInputParas%rInputCostByVol
                gstGlobalPara%stSec%rRssTCostByVal = gstInputParas%rInputCostByVal

                
                select case (giHi5Type)
                case (iHI50)
                    gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime + 5 
                case (iHI51)
                    gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime + 15 
                case (iHI52)
                    gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime + 25 
                case (iHI53)
                    gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime + 35 
                case (iHI54)
                    gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime + 45 
                case (iHI55)
                    gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime + 55  
                case (iHI56)
                    gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime + 105 
                case (iHI57)
                    gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime + 115 
                case (iHI58)
                    gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime + 125                     
                case (iHI59)
                    gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime + 135    
                case default
                end select 
		        if (mod(int(gstGlobalPara%stSec%iSecSqTime/100), 100) == iZERO) then
			        gstGlobalPara%stSec%iSecOpenEndTime = gstGlobalPara%stSec%iSecSqTime - 4100
		        else
			        gstGlobalPara%stSec%iSecOpenEndTime = gstGlobalPara%stSec%iSecSqTime - 100
                end if                 
       ! end if   
      !write(giFileNumber,"(a10,1x,a6,1x,a6,1x,i6,1x,i4,1x,f10.4,1x,f10.4,1x,f10.4)")    'para_d',gcExch,gcSecName,giHi5Type,giInnerPar3Flg, gstGlobalPara%stHi5Para%rMKTThreshold,gstGlobalPara%stHi5Para%rOpenThreshold,gstGlobalPara%stHi5Para%rCloseThreshold			
	  
    end subroutine s_hi5_set_parameters

    
  !----------------------------------------------------
    ! subroutine s_hi5_set_parameters_night() 
    ! description: set Hi5 global parameters, incluing closing time and threshold for each strategy
    !----------------------------------------------------   
    subroutine s_hi5_set_parameters_night()  
        implicit none
            select case (giHi5Type)
            case (iHI50)
                gstGlobalPara%stSec%iSecSqTime = gstGlobalPara%stSec%iSecEndTime + 5 
            case (iHI51)
                gstGlobalPara%stSec%iSecSqTime = gstGlobalPara%stSec%iSecEndTime + 15 
            case (iHI52)
                gstGlobalPara%stSec%iSecSqTime = gstGlobalPara%stSec%iSecEndTime + 25 
            case (iHI53)
                gstGlobalPara%stSec%iSecSqTime = gstGlobalPara%stSec%iSecEndTime + 35 
            case (iHI54)
                gstGlobalPara%stSec%iSecSqTime = gstGlobalPara%stSec%iSecEndTime + 45                                    
            case (iHI55)
                gstGlobalPara%stSec%iSecSqTime = gstGlobalPara%stSec%iSecEndTime + 55    
            case (iHI56)
                gstGlobalPara%stSec%iSecSqTime = gstGlobalPara%stSec%iSecEndTime + 105    
            case (iHI57)
                gstGlobalPara%stSec%iSecSqTime = gstGlobalPara%stSec%iSecEndTime + 115    
            case (iHI58)
                gstGlobalPara%stSec%iSecSqTime = gstGlobalPara%stSec%iSecEndTime + 125    
            case (iHI59)
                gstGlobalPara%stSec%iSecSqTime = gstGlobalPara%stSec%iSecEndTime + 135    
                case default
            end select 
		    if (mod(int(gstGlobalPara%stSec%iSecSqTime/100), 100) == iZERO) then
			    gstGlobalPara%stSec%iSecOpenEndTime = gstGlobalPara%stSec%iSecSqTime - 4100
		    else
			    gstGlobalPara%stSec%iSecOpenEndTime = gstGlobalPara%stSec%iSecSqTime - 100
            end if          
            gstGlobalPara%stHi5Para%rMKTThreshold = 1.0_dp; gstGlobalPara%stHi5Para%rOpenThreshold = 999.0_dp; gstGlobalPara%stHi5Para%rCloseThreshold = 999.0_dp        
            if (giInnerPar1Flg == iNO) gstGlobalPara%stHi5Para%rMKTThreshold   = giSimuPara1 * 0.1_dp
            if (giInnerPar2Flg == iNO) gstGlobalPara%stHi5Para%rOpenThreshold  = giSimuPara2 * 0.1_dp
            if (giInnerPar3Flg == iNO) gstGlobalPara%stHi5Para%rCloseThreshold = giSimuPara3 * 0.1_dp
            gstGlobalPara%stSec%iSecTradeEndTime = gstGlobalPara%stSec%iSecSqTime  
            
           ! if (giRunModeFlg == 1) then                
                gstGlobalPara%stSec%iSecTradeStartTime = gstInputParas%iInputTradeStartTime
                gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime
                gstGlobalPara%stSec%iSecOpenEndTime = gstInputParas%iInputOpenEndTime
                gstGlobalPara%stHi5Para%rMKTThreshold = gstInputParas%rInputPara1
                gstGlobalPara%stHi5Para%rOpenThreshold = gstInputParas%rInputPara2
                gstGlobalPara%stHi5Para%rCloseThreshold = gstInputParas%rInputPara3                
                gstGlobalPara%stSec%rRssTCostByVol = gstInputParas%rInputCostByVol
                gstGlobalPara%stSec%rRssTCostByVal = gstInputParas%rInputCostByVal
                
                select case (giHi5Type)
                case (iHI50)
                    gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime + 5 
                case (iHI51)
                    gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime + 15 
                case (iHI52)
                    gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime + 25 
                case (iHI53)
                    gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime + 35 
                case (iHI54)
                    gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime + 45 
                case (iHI55)
                    gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime + 55  
                case (iHI56)
                    gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime + 105 
                case (iHI57)
                    gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime + 115 
                case (iHI58)
                    gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime + 125                     
                case (iHI59)
                    gstGlobalPara%stSec%iSecSqTime = gstInputParas%iInputSecSqTime + 135    
                case default
                end select 
		        if (mod(int(gstGlobalPara%stSec%iSecSqTime/100), 100) == iZERO) then
			        gstGlobalPara%stSec%iSecOpenEndTime = gstGlobalPara%stSec%iSecSqTime - 4100
		        else
			        gstGlobalPara%stSec%iSecOpenEndTime = gstGlobalPara%stSec%iSecSqTime - 100
                end if                 
           ! end if                
		   !write(giFileNumber,"(a10,1x,a6,1x,a6,1x,i6,1x,i4,1x,f10.4,1x,f10.4,1x,f10.4)")    'para_n',gcExch,gcSecName,giHi5Type,giInnerPar3Flg, gstGlobalPara%stHi5Para%rMKTThreshold,gstGlobalPara%stHi5Para%rOpenThreshold,gstGlobalPara%stHi5Para%rCloseThreshold
     end subroutine s_hi5_set_parameters_night   	
End Module Hi5_Func