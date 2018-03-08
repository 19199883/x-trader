!************************************************************************
! Copyright (C), 2010-2090, MYCapital Asset Management Ltd.
! Create Data:  2014-12-20
! File Name:     Common_Mod.f90
! Author:        Zhenyu WEI
! Description:  This file contains some small common modules used in mysub function
!**************************************************************************
    
Module Common_Mod
	Use Common_Var_Def
    Use Common_Func  
    use Hi5_Func
	Implicit None
	Save
    Contains
    
    !----------------------------------------------------
    ! subroutine s_read_fut_struct() 
    ! description: convert C structure to Fortran structure for each Exchange
    !----------------------------------------------------                                        
	subroutine s_read_fut_struct(liTickNo)    
		implicit none 
        integer, intent(in) :: liTickNo
        select case (trim(gcExch))
        case (cCFFE)
            call s_read_gta_fut_struct(gstcStructCFFEIn, gstCurrIn)
        case (cDCE)  
            call s_read_dl_fut_struct(gstcStructDCEIn, gstCurrIn)
        case (cSHFE)
            if(mod(gstcStructSHFEIn%ndata_flag,3)==0) call s_read_sh_fut_struct(gstcStructSHFEIn, gstCurrIn)
        case (cCZCE)
            call s_read_zz_fut_struct(gstcStructCZCEIn, gstCurrIn)
        case default
        end select        
    end subroutine s_read_fut_struct

    !----------------------------------------------------
    ! subroutine s_run_each_record(liNoTSignal) 
    ! description: s_run_each_record
    !----------------------------------------------------  
    subroutine s_run_each_record(liNoTSignal) 
        integer(c_int), intent(inout)		::	liNoTSignal
       !  get position info
        
        
        ! TODO: wangying debug
        interface 
            subroutine timestamp(n) bind ( c )
                use iso_c_binding
                integer ( c_int ), VALUE :: n
            end subroutine timestamp
        end interface
        !TODO: wangying 1
        integer ( c_int ) :: n = 1
        n=1
        call timestamp(n)

        ! init  variables 
        if (giInitFlg == 0) then
            gcSecName = trim(f_find_security_name(gstCurrIn%cTicker,gcExch))
            call s_set_global_parameters()
            call s_variable_init(gstCurrIn, gstGlobalPara, gstPosData)  
           
            if (gstCurrIn%rPLmtUp < 0.001 .or. gstCurrIn%rPLmtDn < 0.001) goto 9999
            if (giInitPOS == iZERO) goto 9999  
            if (trim(gcExch) == cSHFE .and. mod(gstCurrIn%iData_Flag,3) /= 0) goto 9999    
            if(gstGlobalPara%stHi5Para%rOpenThreshold>99 .or. gstGlobalPara%stHi5Para%rOpenThreshold<1 .or. &
               gstGlobalPara%stHi5Para%rCloseThreshold>99 .or. gstGlobalPara%stHi5Para%rCloseThreshold<1) goto 9999
             gstPreIn = gstCurrIn
            gstRecPreIn = gstRecCurrIn	    
            giInitFlg = 1
        end if   
        
        ! TODO:wangying debug 2
        n = 2
        call timestamp(n)

        call s_get_own_position(gstCurrIn%cTicker, gastEachPOSArray, lbound(gastEachPOSArray, 1), ubound(gastEachPOSArray, 1), gstPosData)
        ! TODO:wangying debug 4
        n = 3
        call timestamp(n)
        if (trim(gcExch) /= cCZCE) then
            gstCurrIn%iVol = int(gstCurrIn%rTotV - gstPreIn%rTotV)/gstGlobalPara%stExch%iVolDivider
            gstCurrIn%rVal = (gstCurrIn%rTotVal - gstPreIn%rTotVal)/gstGlobalPara%stExch%iVolDivider
            gstCurrIn%rOIChg = (gstCurrIn%rOI - gstPreIn%rOI)/gstGlobalPara%stExch%iVolDivider
            gstCurrIn%rAvgP = f_calc_curr_avgp(gstCurrIn%rVal, gstCurrIn%iVol, gstPreIn%rAvgP)/gstGlobalPara%stSec%iUnitPerContract                  
        else
            gstCurrIn%iVol = int(gstCurrIn%rTotV - gstPreIn%rTotV)/gstGlobalPara%stExch%iVolDivider
            gstCurrIn%rOIChg = (gstCurrIn%rOI - gstPreIn%rOI)/gstGlobalPara%stExch%iVolDivider
            gstCurrIn%rAvgP = gstCurrIn%rP    
            gstCurrIn%rVal = rZERO
        end if
        if (f_check_valid_data(gstCurrIn, gstRecPreIn, giTickNo, gstGlobalPara) == iNO) goto 9999   ! check whether data is valid
    
        ! TODO:wangying debug 3
        n = 4
        call timestamp(n)

        giTickNo = giTickNo + 1  ! tick number add 1
        if ((gstCurrIn%iTime < gstGlobalPara%stExch%iMktStartTime * 1000) .or.(gstCurrIn%iTime > gstGlobalPara%stExch%iMktEndTime * 1000)) goto 9999            
        if (giTickNo == 1 .and. gstCurrIn%iTime >= gstGlobalPara%stExch%iMktStartTime * 1000) goto 9999  ! used data is from second tick
        gstCurrTradeInfo%iBV1Sum = gstCurrTradeInfo%iBV1Sum + gstCurrIn%iBV(1)
        gstCurrTradeInfo%iSV1Sum = gstCurrTradeInfo%iSV1Sum + gstCurrIn%iSV(1) 
        
        call s_init_each_run(liNoTSignal, gstCurrTradeInfo, gstPreTradeInfo, gstPosData)  ! init variables for each running 

        ! TODO:wangying debug 4
        n = 5
        call timestamp(n)
           if (gstCurrTradeInfo%iLastTradeSize /= iZERO) then
                call s_rss_calc_profitloss(gstCurrTradeInfo, gstPreTradeInfo, gstGlobalPara%stSec, gstCurrIn)
                call s_adjust_to_trade(gstCurrTradeInfo, gstPreTradeInfo)
            end if
            gstCurrTradeInfo%rLivePL = gstCurrTradeInfo%rCash + gstCurrTradeInfo%iPos * gstCurrIn%rP
            gstCurrTradeInfo%rGrossPnl = gstCurrTradeInfo%rGrossCash + gstCurrTradeInfo%iPos * gstCurrIn%rP	            
        
         call s_hi5_sig_gen(gstCurrIn, gstPreIn, gstGlobalPara, gstPosData, gstCurrTradeInfo, gstPreTradeInfo, giTickNo, gaiFullOrderBookBuy, gaiFullOrderBookSel, &
                            gaiPreFullOrderBookBuy, gaiPreFullOrderBookSel, giFullOrderBookL, giFullOrderBookU, gstTSignal) ! generate signal


        ! TODO:wangying debug 5
        n = 6
        call timestamp(n)

         call s_end_of_each_run(gstCurrIn, gstPreIn, gstCurrTradeInfo, gstPreTradeInfo, giNoTradeItemEachRun)

if (giWriteOutputFileFlg == 1) call s_hi5_output_log_new(giTickNo, giFileNumber)! write log file
!         call s_hi5_output_log_new(giTickNo, giFileNumber)! write log file
		 gidata_vaild=0
9999    continue 
        if ((mod(gstCurrIn%iData_Flag,3) /= 2 .and. trim(gcExch) == cSHFE) .or. trim(gcExch) /= cSHFE) then   
             gstPreIn = gstCurrIn 
             gstRecPreIn = gstRecCurrIn
        end if
        ! TODO:wangying debug 6
        n = 7
        call timestamp(n)
    end subroutine s_run_each_record
    
    
    
    !----------------------------------------------------
    !subroutine s_rss_calc_profitloss()
    ! description: calculate PNL
    !----------------------------------------------------      
    subroutine s_rss_calc_profitloss(lstCurTradeInfo, lstPreTradeInfo, lstSecPara, lstDataIn)
        implicit none   
        type(trade_info), intent(inout)		::  lstCurTradeInfo, lstPreTradeInfo
        type(sec_parameter), intent(in)     ::  lstSecPara
        type(in_data), intent(in)		::	lstDataIn
        integer   ::  liTradedSize, liClosedSize, liOpenedSize
        real(dp)  ::  lrCost, lrOrderP, lrPrice
        
        liTradedSize = lstCurTradeInfo%iPos - lstPreTradeInfo%iPos
        lrPrice = lstDataIn%rP
        if (liTradedSize /= iZERO) lrOrderP = lstCurTradeInfo%rOrderP
        if (liTradedSize /= iZERO) then
             if (lstCurTradeInfo%iSignPos ==  lstPreTradeInfo%iSignPos) then ! if holding side remains the same
                if (f_sign(liTradedSize) == lstCurTradeInfo%iSignPos) then ! if trade direction is the same as holding, only increase holding
                    liOpenedSize = abs(liTradedSize) ; liClosedSize = iZERO
                else
                    liOpenedSize = iZERO ; liClosedSize = abs(liTradedSize)
                end if
            else
                if (abs(liTradedSize) >= abs(lstPreTradeInfo%iPos)) then
                    liOpenedSize = abs(liTradedSize) - abs(lstPreTradeInfo%iPos) 
                    liClosedSize = abs(lstPreTradeInfo%iPos)
                else
                    liOpenedSize = iZERO
                    liClosedSize = abs(liTradedSize)
                end if
            end if
            
            lrCost = rZERO
            if (liClosedSize > iZERO) then
               lrCost = lstSecPara%rRssTCostByVol *liClosedSize + lstSecPara%rRssTCostByVal* liClosedSize * lrPrice                 
                if (liClosedSize == abs(lstPreTradeInfo%iPos)) then
                    lstCurTradeInfo%rPnl = lstCurTradeInfo%rCash + lrOrderP *lstPreTradeInfo%iPos - lrCost - lstCurTradeInfo%rPrevTradeEndCash
                    lstCurTradeInfo%rTotalPnL = lstCurTradeInfo%rTotalPnL +  lstCurTradeInfo%rPnl
                    if(lstCurTradeInfo%rPnl > rZERO) then
                        lstCurTradeInfo%rWinPt = lstCurTradeInfo%rWinPt + lstCurTradeInfo%rPnl;
                        lstCurTradeInfo%iNoWin = lstCurTradeInfo%iNoWin + 1
                    end if
                    
                    if(lstCurTradeInfo%rPnl < rZERO) then
                        lstCurTradeInfo%rLossPt = lstCurTradeInfo%rLossPt + lstCurTradeInfo%rPnl;
                        lstCurTradeInfo%iNoLoss = lstCurTradeInfo%iNoLoss + 1
                    end if 
                    lstCurTradeInfo%rPrevTradeEndCash = lstCurTradeInfo%rCash + lrOrderP * lstPreTradeInfo%iPos - lrCost
                end if
            end if 
            
            if (liOpenedSize > iZERO) lrCost = lrCost + lstSecPara%rRssTCostByVol *liOpenedSize + lstSecPara%rRssTCostByVal* liOpenedSize * lrPrice
            lstCurTradeInfo%rCash = lstCurTradeInfo%rCash + lrOrderP *(-liTradedSize) - lrCost
            lstCurTradeInfo%rTotalCost = lstCurTradeInfo%rTotalCost + lrCost
            lstCurTradeInfo%rGrossCash = lstCurTradeInfo%rGrossCash + lrOrderP *(-liTradedSize)
            lstCurTradeInfo%rAmount = lstCurTradeInfo%rAmount + lrOrderP * abs(liTradedSize)            
        end if
    end subroutine s_rss_calc_profitloss
    

    !----------------------------------------------------
    !subroutine s_adjust_to_trade(lstCurTradeInfo, lstPreTradeInfo)
    ! description: adjust trade info
    !----------------------------------------------------      
    subroutine s_adjust_to_trade(lstCurTradeInfo, lstPreTradeInfo)
        implicit none
        type(trade_info), intent(inout)		::  lstCurTradeInfo, lstPreTradeInfo
        integer							    ::	liPrevToTradeSize
        
        liPrevToTradeSize = lstCurTradeInfo%iVol2Trade
        if (liPrevToTradeSize /= iZERO) lstCurTradeInfo%iVol2Trade = lstCurTradeInfo%iVol2Trade - lstCurTradeInfo%iLastTradeSize
		lstCurTradeInfo%iTotalTradeSize = lstCurTradeInfo%iTotalTradeSize + abs(lstCurTradeInfo%iLastTradeSize)
		if (lstCurTradeInfo%iSignPOS /= lstPreTradeInfo%iSignPOS .and. lstCurTradeInfo%iSignPOS /= iFLAT) lstCurTradeInfo%iRounds = lstCurTradeInfo%iRounds + 1
		if (lstCurTradeInfo%iVol2Trade == iZERO .or. abs(lstCurTradeInfo%iLastTradeSize) - abs(liPrevToTradeSize) > iZERO) then
			lstCurTradeInfo%iToTrade = iFLAT; lstCurTradeInfo%iVol2Trade = iZERO; lstCurTradeInfo%iOpenSize = iFLAT
			lstCurTradeInfo%iCloseSize = iZERO
            lstCurTradeInfo%rOrderP = rZERO; 
		end if        
    end subroutine s_adjust_to_trade      
    
  !   !----------------------------------------------------
  !  ! function integer function f_check_valid_data
  !  ! description: check input data is whether valid
  !  !----------------------------------------------------  
    integer function f_check_valid_data(lstCurrInData, lstPreInData, liTickNo, lstGlobalPara)
		implicit none
		type(in_data), intent(in)	::	lstCurrInData, lstPreInData
		type (all_parameter), intent(in)      ::    lstGlobalPara
        integer, intent(in) :: liTickNo
        real(dp) :: lrTick
        
		lrTick = lstGlobalPara%stSec%rTickSize
		f_check_valid_data = iYES  
  
        if (mod(int(lstCurrInData%rTotV + 0.5), lstGlobalPara%stExch%iVolDivider) /= iZERO) f_check_valid_data = iNO
        if (abs(lstCurrInData%iVol) > lstGlobalPara%stSec%iWrongVol .or. lstCurrInData%iVol < iZERO ) f_check_valid_data = iNO        
		if (lstCurrInData%iVol == iZERO .and. lstCurrInData%rVal > 1._dp * lstCurrInData%rP) f_check_valid_data = iNO
        if (lstCurrInData%rBP(1) < lrTick/2 .and. lstCurrInData%rSP(1) < lrTick/2) f_check_valid_data = iNO
        if(liTickNo==1) then
            giMaxTime = lstCurrInData%iTime
        else
            if(lstCurrInData%iTime < giMaxTime) then
                f_check_valid_data = iNO
            else
                giMaxTime = lstCurrInData%iTime
            end if
        end if
        
        if(lstCurrInData%rBP(1)>=lstCurrInData%rSP(1).and. lstCurrInData%rSP(1)>lrTick) f_check_valid_data = iNO
        if(lstCurrInData%rSP(1)>lstCurrInData%rSP(2) .and. lstCurrInData%rSP(2)>lrTick) f_check_valid_data = iNO
        if(lstCurrInData%rSP(2)>lstCurrInData%rSP(3) .and. lstCurrInData%rSP(3)>lrTick) f_check_valid_data = iNO
        if(lstCurrInData%rSP(3)>lstCurrInData%rSP(4) .and. lstCurrInData%rSP(4)>lrTick) f_check_valid_data = iNO
        if(lstCurrInData%rSP(4)>lstCurrInData%rSP(5) .and. lstCurrInData%rSP(5)>lrTick) f_check_valid_data = iNO
        if(lstCurrInData%rBP(1)<lstCurrInData%rBP(2) .and. lstCurrInData%rBP(1)>lrTick) f_check_valid_data = iNO
        if(lstCurrInData%rBP(2)<lstCurrInData%rBP(3) .and. lstCurrInData%rBP(2)>lrTick) f_check_valid_data = iNO
        if(lstCurrInData%rBP(3)<lstCurrInData%rBP(4) .and. lstCurrInData%rBP(3)>lrTick) f_check_valid_data = iNO
        if(lstCurrInData%rBP(4)<lstCurrInData%rBP(5) .and. lstCurrInData%rBP(4)>lrTick) f_check_valid_data = iNO
        if(lstCurrInData%rSP(5)>lstCurrInData%rPLmtUp.and. lstCurrInData%rSP(5)>lrTick) f_check_valid_data = iNO
        if(lstCurrInData%rBP(5)<lstCurrInData%rPLmtDn.and. lstCurrInData%rBP(5)>lrTick) f_check_valid_data = iNO      
        
        if (trim(gcExch) /= cCZCE) then
            if (liTickNo > 1 .and. lstCurrInData%iTime == lstPreInData%iTime) f_check_valid_data = iNO
            if (sum(abs(lstCurrInData%rBP(1:5) - lstPreInData%rBP(1:5))) < 0.01 .and.  sum(abs(lstCurrInData%rSP(1:5) - lstPreInData%rSP(1:5))) < 0.01 .and. &
                sum(abs(lstCurrInData%iBV(1:5) - lstPreInData%iBV(1:5))) == iZERO .and. sum(abs(lstCurrInData%iSV(1:5) - lstPreInData%iSV(1:5))) == iZERO &
                .and. lstCurrInData%iVol == iZERO) f_check_valid_data = iNO
        else
           if (sum(abs(lstCurrInData%rBP(1:5) - lstPreInData%rBP(1:5))) < 0.01 .and.  sum(abs(lstCurrInData%rSP(1:5) - lstPreInData%rSP(1:5))) < 0.01 .and. &
            sum(abs(lstCurrInData%iBV(1:5) - lstPreInData%iBV(1:5))) == iZERO .and. sum(abs(lstCurrInData%iSV(1:5) - lstPreInData%iSV(1:5))) == iZERO .and. &
                lstCurrInData%iTotalBuyVol == lstPreInData%iTotalBuyVol .and. lstCurrInData%iTotalSelVol == lstPreInData%iTotalSelVol) f_check_valid_data = iNO 
        end if      
    end function f_check_valid_data     
    
    !----------------------------------------------------
    ! subroutine s_init_each_run()
    ! description: init each run
    !----------------------------------------------------  
    subroutine s_init_each_run(liNoTSignal, lstTradeInfo, lstPreTradeInfo, lstPosData)  
        implicit none
        integer(c_int), intent(inout)		::	liNoTSignal 
        type (trade_info), intent(inout)   ::    lstTradeInfo, lstPreTradeInfo
        type (position_data), intent(in)   ::	 lstPosData       
        liNoTSignal = 0
        giNoTradeItemEachRun = 0
        giLb = 0
        giLen = iSIGNAL_LEN
        lstTradeInfo%iPos = lstPosData%iPOSD - lstPosData%iPOSK
        lstTradeInfo%iSignPos = f_sign(lstTradeInfo%iPos)
        lstTradeInfo%iLastTradeSize = lstTradeInfo%iPos - lstPreTradeInfo%iPos
        lstTradeInfo%iTS = iFLAT
        lstTradeInfo%iNewPosStatus = iFLAT
        lstTradeInfo%iOrderSize = iZERO
        lstTradeInfo%iCancelFlg = iZERO 
        lstTradeInfo%iContinousFlg = 1
    end subroutine s_init_each_run  
    
    !----------------------------------------------------
    ! subroutine s_end_of_each_run()
    ! description: ending of each run
    !----------------------------------------------------   
	subroutine s_end_of_each_run(lstCurrInData, lstPreInData, lstCurrTradeInfo, lstPreTradeInfo, liSignalNum)
		implicit none
		type(in_data), intent(inout)		::	lstCurrInData, lstPreInData
		type(trade_info), intent(inout)		::  lstCurrTradeInfo, lstPreTradeInfo
		integer, intent(in)	::	liSignalNum
		integer :: liNewPosStatus
		lstPreInData = lstCurrInData
        liNewPosStatus = lstPreTradeInfo%iNewPosStatus
		if (liSignalNum /= iZERO .or. lstCurrTradeInfo%iLastTradeSize /= iZERO .or. lstCurrTradeInfo%iSignalType/=iZERO) then
            lstPreTradeInfo = lstCurrTradeInfo
            if (giNoTradeItemEachRun == iZERO) lstPreTradeInfo%iNewPosStatus = liNewPosStatus
        end if
        if (giNoTradeItemEachRun == iZERO .and. lstCurrInData%iTime < gstGlobalPara%stSec%iSecSqTime * 1000) lstCurrTradeInfo%rOrderP =  lstCurrTradeInfo%rRecOrderP
    end subroutine s_end_of_each_run    
    
    
     !----------------------------------------------------
    ! subroutine s_set_global_parameters() 
    ! description: set all global parameters
    !----------------------------------------------------     
    subroutine s_set_global_parameters()  
        implicit none
        
        call s_set_exch_parameters(gcExch, gcSecName, gstGlobalPara)
        call s_set_convertion_parameters(gcSecName, gstGlobalPara%stSec)
        if (giNightMarket == 0) call s_hi5_set_parameters()
        if (giNightMarket == 1) call s_hi5_set_parameters_night()   
    end subroutine s_set_global_parameters   
    
    subroutine s_variable_init(lstCurrInData, lstGlobalPara, lstPosData)  
         implicit none
        type(in_data), intent(inout)	::	lstCurrInData
        type (all_parameter), intent(in)      ::    lstGlobalPara
        type(position_data)	::	lstPosData
        
        call s_common_variable_init(lstCurrInData, lstGlobalPara, lstPosData)
        call s_hi5_variable_init(lstCurrInData, lstGlobalPara)

    end subroutine s_variable_init   
    
    
    !----------------------------------------------------
    ! subroutine s_common_variable_init()
    ! description: init common variables
    !----------------------------------------------------  
    subroutine s_common_variable_init(lstCurrInData, lstGlobalPara, lstPosData)  
        implicit none
        type(in_data), intent(inout)	::	lstCurrInData
        type (all_parameter), intent(in)      ::    lstGlobalPara
        type(position_data)	::	lstPosData
        
        giFullOrderBookL = 1
        giFullOrderBookU = int((lstCurrInData%rPLmtUp - lstCurrInData%rPLmtDn)/ lstGlobalPara%stSec%rTickSize + 0.5) + 1
        if (allocated(gaiFullOrderBookBuy)) deallocate(gaiFullOrderBookBuy)
		allocate(gaiFullOrderBookBuy(giFullOrderBookL: giFullOrderBookU)) 
        
        if (allocated(gaiFullOrderBookSel)) deallocate(gaiFullOrderBookSel)
		allocate(gaiFullOrderBookSel(giFullOrderBookL: giFullOrderBookU))  
        
        if (allocated(gaiPreFullOrderBookBuy)) deallocate(gaiPreFullOrderBookBuy)
		allocate(gaiPreFullOrderBookBuy(giFullOrderBookL: giFullOrderBookU)) 
        
        if (allocated(gaiPreFullOrderBookSel)) deallocate(gaiPreFullOrderBookSel)
		allocate(gaiPreFullOrderBookSel(giFullOrderBookL: giFullOrderBookU))          
        
        if (allocated(gaiFullOrderBookBuyDiff)) deallocate(gaiFullOrderBookBuyDiff)
		allocate(gaiFullOrderBookBuyDiff(giFullOrderBookL: giFullOrderBookU)) 
        
        if (allocated(gaiFullOrderBookSelDiff)) deallocate(gaiFullOrderBookSelDiff)
		allocate(gaiFullOrderBookSelDiff(giFullOrderBookL: giFullOrderBookU))  
        
        gaiFullOrderBookBuy = 0; gaiFullOrderBookSel = 0
        gaiPreFullOrderBookBuy = 0; gaiPreFullOrderBookSel = 0        
        gaiFullOrderBookBuyDiff = 0; gaiFullOrderBookSelDiff = 0       
        call s_trade_info_init(gstCurrTradeInfo, lstPosData)
        gstPreTradeInfo = gstCurrTradeInfo        
        lstCurrInData%iTotalBuyVol = iZERO
        lstCurrInData%iTotalSelVol = iZERO
        lstCurrInData%rWeightedBuyOrderP = rZERO
        lstCurrInData%rWeightedSelOrderP = rZERO
    end subroutine s_common_variable_init 
    
    !----------------------------------------------------
    ! subroutine s_trade_info_init()
    ! description: init trade_info variables
    !----------------------------------------------------
    subroutine s_trade_info_init(lstTradeInfo, lstPosData)
        implicit none
        type (trade_info), intent(inout)   ::    lstTradeInfo
        type (position_data), intent(in)   ::	 lstPosData
        lstTradeInfo%iTargetPos = iZERO; lstTradeInfo%iPos = iZERO; lstTradeInfo%iSignPos = iZERO;
        lstTradeInfo%iVol2Pass = iZERO; lstTradeInfo%iTradeTickNo = iZERO; lstTradeInfo%iSigID = iZERO; lstTradeInfo%iLatestSigID = iZERO; lstTradeInfo%iSignalType = iZERO; lstTradeInfo%iTradeTime = iZERO;
        lstTradeInfo%iBV1Sum = iZERO; lstTradeInfo%iSV1Sum = iZERO; lstTradeInfo%iRounds = iZERO; lstTradeInfo%iVol2Trade = iZERO;
        lstTradeInfo%iLastTradeSize = iZERO; lstTradeInfo%iNoWin = iZERO; lstTradeInfo%iNoLoss = iZERO; lstTradeInfo%iTotalTradeSize = iZERO; 
        lstTradeInfo%iToTrade = iFLAT;  lstTradeInfo%iOpenSize = iZERO;  lstTradeInfo%iCloseSize =  iZERO
        lstTradeInfo%iTS = iFLAT; lstTradeInfo%iNewPosStatus = iFLAT
        lstTradeInfo%iOrderSize = iZERO; lstTradeInfo%iTotalOrderSize = iZERO; lstTradeInfo%iTotalCancelSize = iZERO;
        lstTradeInfo%iCancelFlg = iZERO; lstTradeInfo%iContinousFlg = 1
  
        
        lstTradeInfo%rTradeBP = rZERO; lstTradeInfo%rTradeSP = rZERO; lstTradeInfo%rLivePL = rZERO;
        lstTradeInfo%rPnl = rZERO; lstTradeInfo%rTotalPnL = rZERO; lstTradeInfo%rCash = rZERO; lstTradeInfo%rVround = rZERO;  
        lstTradeInfo%rGrossPnl = rZERO; lstTradeInfo%rGrossCash = rZERO; lstTradeInfo%rAmount = rZERO;
        lstTradeInfo%rTradeCost = rZERO; lstTradeInfo%rTotalCost = rZERO; lstTradeInfo%rWinPt = rZERO; lstTradeInfo%rLossPt = rZERO; lstTradeInfo%rPrevTradeEndCash = rZERO;
        lstTradeInfo%rOrderP = rZERO;
    end subroutine s_trade_info_init     
    
    
End Module Common_Mod
