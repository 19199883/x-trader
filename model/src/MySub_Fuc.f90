Module MySub_Fuc
	Use Common_Var_Def
    use Common_Mod
	Implicit None
    contains

    !----------------------------------------------------
    ! subroutine mysub0(liFileNumber, lcFileName)
    ! description: initialization function
    ! input: liFileNumber
    !        lcFileName 
    !----------------------------------------------------	       
    subroutine mysub0(liFileNumber, lcFileName)
	    implicit none
	    integer, intent(in)				::	liFileNumber
	    character(len=*), intent(in)	::	lcFileName
       ! integer date_time(8)
       ! character*10 b(3)

       ! TODO: wangying debug
!       integer::i,k=1
!       real::e
!       real,dimension(2):: t(2)
!       open (unit=2000, file='test.txt')
!       e = dtime( t )
!       write(unit=2000, fmt=100),e,t(1),t(2)
!       100 format('elapsed:', f5.4, ', user:', f5.4, ', sys:', f5.4)
!       do i = 1, 10000
!          k=k+1
!       end do
!       e = dtime( t )
!       write(unit=2000, fmt=200),e,t(1),t(2)
!       200 format('elapsed:', f5.4, ', user:', f5.4, ', sys:', f5.4)
!       close (2000)
!
        !call date_and_time(b(1), b(2), b(3), date_time)
        giFileNumber = liFileNumber
        gcFileName = lcFileName
        !giFileNumber=11111
        !gcFileName=trim(gcStrategyName)//'_'//trim(gcticker)//trim(int2cha(date_time(1)))//trim(int2cha(date_time(2)))//trim(int2cha(date_time(3)))//'_'//trim(int2cha(giNightMarket))//'_strag.txt'		
        giWriteOutputFileFlg = 1
        giWriteOutputFileInitFlg = 0
        giRunModeFlg = 1
        giSimuPara1 = 0
        giSimuPara2 = 0
        giSimuPara3 = 0        
        giInitPOS = 0
        giInitFlg = 0
        giMaxTime = 0	

    !    if (giWriteOutputFileFlg == 1 .and. giWriteOutputFileInitFlg == 0) then     
    !        if (giRunModeFlg == 0) then
    !            open(giFileNumber,file=trim(gcFileName),status='unknown')
				!write(giFileNumber,"(a10,1x,i12,1x,a256)")    'mysub0',giFileNumber, gcFileName 	
    !        else
    !            open(giFileNumber,file=trim(gcFileName),status='unknown',  position = 'append')	
    !            !write(giFileNumber,"(a10,1x,i12,1x,a256)")    'mysub0',giFileNumber, gcFileName				
    !        end if
    !        giWriteOutputFileInitFlg = 1
    !    end if      
    
    end subroutine mysub0
    
    !----------------------------------------------------
    ! subroutine mysub_if(lstStructIn,  lastStructTSignal, liLB, liLen, liNoTSignal)
    ! description: mysub for if
    ! input: lstStructIn, liLB, liLen
    ! output: lastStructTSignal, liNoTSignal
    !----------------------------------------------------	       
    subroutine mysub_if(lstStructIn,  lastStructTSignal, liLB, liLen, liNoTSignal)
	    implicit none
        type(cStruct_gta_quote), intent(inout) ::	lstStructIn
        integer(c_int), intent(in)	        ::	liLB, liLen
        type(cStruct_TSignal), dimension(0:(iSIGNAL_LEN - 1)), intent(inout)	::	lastStructTSignal
        integer(c_int), intent(inout)		::	liNoTSignal
	    character							::	lcTSignal*1024
	    type(in_data)						::	lstInData
        
        ! init  global parameters
        if (giInitFlg == 0) then
            gcExch = cCFFE
            giTickNo = 0
            gstRecCurrIn%iTime=0
        end if
    
        ! load data from lstStructIn to gstCurrIn
        if (giRunModeFlg == 1) then
            gstcStructCFFEIn = lstStructIn
            call s_read_fut_struct(giTickNo) 
        end if
    
        call s_run_each_record(liNoTSignal) 
        if(gstRecCurrIn%iTime<gstCurrIn%iTime) then        
        gstRecCurrIn = gstCurrIn
     !   ! send signal
	    !call s_assign_out_tsignal(gstTSignal, lbound(gstTSignal, 1), ubound(gstTSignal, 1), giNoTradeItemEachRun, &
					!			    lastStructTSignal, lbound(lastStructTSignal, 1), iSIGNAL_LEN, liNoTSignal, &
					!			    gstGlobalPara%stSec%iTickSizeMulti, gstGlobalPara%stSec%iPMultiplier) 
       end if
    end subroutine mysub_if    
    !
    !----------------------------------------------------
    ! subroutine mysub_dl(lstStructIn,  lastStructTSignal, liLB, liLen, liNoTSignal)
    ! description: mysub for dl
    ! input: lstStructIn, liLB, liLen
    ! output: lastStructTSignal, liNoTSignal
    !----------------------------------------------------	       
    subroutine mysub_dl(lstStructIn,  lastStructTSignal, liLB, liLen, liNoTSignal)
	    implicit none
        type(cStruct_dl_quote), intent(inout) ::	lstStructIn
        integer(c_int), intent(in)	        ::	liLB, liLen
        type(cStruct_TSignal), dimension(0:(iSIGNAL_LEN - 1)), intent(inout)	::	lastStructTSignal
        integer(c_int), intent(inout)		::	liNoTSignal
	    character							::	lcTSignal*1024
	    type(in_data)						::	lstInData
        
        ! init  global parameters
        if (giInitFlg == 0) then
            gcExch = cDCE
            giTickNo = 0
            gstRecCurrIn%iTime=0
        end if
       
        ! load data from lstStructIn to gstCurrIn
        if (giRunModeFlg == 1) then
            gstcStructDCEIn = lstStructIn
            call s_read_fut_struct(giTickNo) 
        end if
    
        call s_run_each_record(liNoTSignal) 
        if(gstRecCurrIn%iTime<gstCurrIn%iTime) then        
        gstRecCurrIn = gstCurrIn
        ! send signal
	    !call s_assign_out_tsignal(gstTSignal, lbound(gstTSignal, 1), ubound(gstTSignal, 1), giNoTradeItemEachRun, &
					!			    lastStructTSignal, lbound(lastStructTSignal, 1), iSIGNAL_LEN, liNoTSignal, &
					!			    gstGlobalPara%stSec%iTickSizeMulti, gstGlobalPara%stSec%iPMultiplier) 
       end if
    end subroutine mysub_dl    
    
    !----------------------------------------------------
    ! subroutine mysub_shfe(lstStructIn,  lastStructTSignal, liLB, liLen, liNoTSignal)
    ! description: mysub for shfe
    ! input: lstStructIn, liLB, liLen
    ! output: lastStructTSignal, liNoTSignal
    !----------------------------------------------------	       
    subroutine mysub_shfe(lstStructIn,  lastStructTSignal, liLB, liLen, liNoTSignal)
	    implicit none
        type(cStruct_shfe_quote), intent(inout) ::	lstStructIn
        integer(c_int), intent(in)	        ::	liLB, liLen
        type(cStruct_TSignal), dimension(0:(iSIGNAL_LEN - 1)), intent(inout)	::	lastStructTSignal
        integer(c_int), intent(inout)		::	liNoTSignal
	    character							::	lcTSignal*1024
	    type(in_data)						::	lstInData
        
        ! init  global parameters
        if (giInitFlg == 0) then
            gcExch = cSHFE
            giTickNo = 0
            gstRecCurrIn%iTime=0
        end if
       
        ! load data from lstStructIn to gstCurrIn
        if (giRunModeFlg == 1) then
            gstcStructSHFEIn = lstStructIn
            if (mod(gstcStructSHFEIn%ndata_flag,3) /= 0) return
            call s_read_fut_struct(giTickNo) 
        end if
    
       if(mod(gstcStructSHFEIn%ndata_flag,3)==0) then
        call s_run_each_record(liNoTSignal) 
        ! send signal
         if(gstRecCurrIn%iTime<gstCurrIn%iTime) then        
           gstRecCurrIn = gstCurrIn
           if(mod(gstCurrIn%iData_flag,3)==0 .and. mod(gstBfIn%iData_flag,3)==0 .and. 1.5*gstCurrIn%iTotalBuyvol >1.0*gstBfIn%iTotalBuyvol .and.1.5*gstCurrIn%iTotalSelvol>1.0*gstBfIn%iTotalSelvol .and.&
                                                                        1.5*gstBfIn%iTotalBuyvol >1.0*gstCurrIn%iTotalBuyvol .and.1.5*gstBfIn%iTotalSelvol>1.0*gstCurrIn%iTotalSelvol) then
	          call s_assign_out_tsignal(gstTSignal, lbound(gstTSignal, 1), ubound(gstTSignal, 1), giNoTradeItemEachRun, &
								        lastStructTSignal, lbound(lastStructTSignal, 1), iSIGNAL_LEN, liNoTSignal, &
								        gstGlobalPara%stSec%iTickSizeMulti, gstGlobalPara%stSec%iPMultiplier)
          end if       								    
          if(mod(gstCurrIn%iData_flag,3)==0) gstBfIn=gstCurrIn
         end if
       end if
    end subroutine mysub_shfe
    
    !----------------------------------------------------
    ! subroutine mysub_czce(lstStructIn,  lastStructTSignal, liLB, liLen, liNoTSignal)
    ! description: mysub for czce
    ! input: lstStructIn, liLB, liLen
    ! output: lastStructTSignal, liNoTSignal
    !----------------------------------------------------	       
    subroutine mysub_czce(lstStructIn,  lastStructTSignal, liLB, liLen, liNoTSignal)
	    implicit none
        type(cStruct_zz_quote), intent(inout) ::	lstStructIn
        integer(c_int), intent(in)	        ::	liLB, liLen
        type(cStruct_TSignal), dimension(0:(iSIGNAL_LEN - 1)), intent(inout)	::	lastStructTSignal
        integer(c_int), intent(inout)		::	liNoTSignal
	    character							::	lcTSignal*1024
	    type(in_data)						::	lstInData
        
        ! init  global parameters
        if (giInitFlg == 0) then
            gcExch = cCZCE
            giTickNo = 0
            gstRecCurrIn%iTime=0
        end if
         ! load data from lstStructIn to gstCurrIn
        if (giRunModeFlg == 1) then
            gstcStructCZCEIn = lstStructIn
            call s_read_fut_struct(giTickNo) 
        end if
    
    
        call s_run_each_record(liNoTSignal) 
        if(gstRecCurrIn%iTime<gstCurrIn%iTime) then
           gstRecCurrIn = gstCurrIn		
    
	       call s_assign_out_tsignal(gstTSignal, lbound(gstTSignal, 1), ubound(gstTSignal, 1), giNoTradeItemEachRun, &
								     lastStructTSignal, lbound(lastStructTSignal, 1), iSIGNAL_LEN, liNoTSignal, &
								     gstGlobalPara%stSec%iTickSizeMulti, gstGlobalPara%stSec%iPMultiplier) 
        
        end if
    end subroutine mysub_czce      
    
    
    End Module MySub_Fuc   
    
   !----------------------------------------------------
    ! subroutine st_feed_sig_resp(s_cStructIn, liNoTSignal, lastStructTSignal)
    ! description: initialization function
    ! input: liFileNumber
    !        lcFileName 
    !----------------------------------------------------
    subroutine st_feed_sig_resp(lstStructOrderStatusIn, lstStructPositionIn, liNoTSignal, lastStructTSignal,lstrat_out_log)
        Use Common_Var_Def
        Use Common_Func
        Use Common_Mod
	    implicit none
        type(cStruct_signal_resp_t), intent(in) ::	lstStructOrderStatusIn
        type(cStruct_symbol_pos), intent(in) 	::	lstStructPositionIn
	    integer(c_int), intent(inout)	        ::	liNoTSignal
	    type(cStruct_TSignal), dimension(0:(iSIGNAL_LEN-1)), intent(inout)	::	lastStructTSignal
        type(strat_out_log), intent(inout)          ::  lstrat_out_log
    
        liNoTSignal = iZERO
        giNoTradeItemEachRun = iZERO
   	    call st_feed_position(lstStructPositionIn, liNoTSignal, lastStructTSignal)
	    if (giNoTradeItemEachRun > iZERO) then
	        call s_assign_out_tsignal(gstTSignal, lbound(gstTSignal, 1), ubound(gstTSignal, 1), giNoTradeItemEachRun, &
								      lastStructTSignal, lbound(lastStructTSignal, 1), iSIGNAL_LEN, liNoTSignal, &
								      gstGlobalPara%stSec%iTickSizeMulti, gstGlobalPara%stSec%iPMultiplier)  
 !if (giNoTradeItemEachRun >0 ) write(giFileNumber,"(a5,1x,i3,1x,i3)")    'out2',lbound(lastStructTSignal, 1),giNoTradeItemEachRun   									  
        end if
        lstrat_out_log=gstrat_out_log
        lstrat_out_log%exch_time=0
    end subroutine st_feed_sig_resp    

   !----------------------------------------------------
    ! subroutine st_feed_position(lstStructPositionIn, liNoTSignal, lastStructTSignal)
    ! description: get position info
    !----------------------------------------------------    
    subroutine st_feed_position(lstStructPositionIn, liNoTSignal, lastStructTSignal)
    	Use Common_Var_Def
        Use Common_Func
        Use Common_Mod
	    implicit none
	    type(cStruct_symbol_pos), intent(in)	::	lstStructPositionIn
	    integer(c_int), intent(inout)			::	liNoTSignal
	    type(cStruct_TSignal), dimension(0:(iSIGNAL_LEN - 1)), intent(inout)	::	lastStructTSignal

	    liNoTSignal = iZERO
        giNoTradeItemEachRun = iZERO
	    call s_read_position(gcTicker, lstStructPositionIn, gstPosDataToday)
	    gstPosData%cTicker = trim(gcTicker)
	    gstPosData%iPOSD = gstPosDataToday%iPOSD
        gstPosData%iPOSK = gstPosDataToday%iPOSK
        gstPosData%iPOSChg = gstPosDataToday%iPOSChg
	    gstPosData%iMaxPOS = giMaxPos
	    giInitPOS = 1
    end subroutine st_feed_position    
    

    !----------------------------------------------------
    ! subroutine st_feed_init_position(lstStructPositionIn, liNoTSignal, lastStructTSignal)
    ! description: initialization position function
    !----------------------------------------------------        

    subroutine st_feed_init_position(lstStructPositionIn,lstrat_out_log)    
    	Use Common_Var_Def
        Use Common_Func
        Use Common_Mod
	    implicit none
        type(cStruct_init_position), intent(in) ::	lstStructPositionIn
        type(strat_out_log), intent(inout)          ::  lstrat_out_log
        integer :: liI,liNoTSignal


       ! TODO: wangying debug
       interface 
            subroutine timestamp() bind ( c )
                use iso_c_binding
            end subroutine timestamp
        end interface
        call timestamp()

!       integer::i,k=1
!       real::start,finish
!       real,dimension(2):: t(2)
!       open (unit=2000, file='test.txt')
!       call cpu_time( start )
!       do i = 1, 10000000
!          k=k+1
!       end do
!       call cpu_time( finish )
!       write(unit=2000, fmt=200),finish-start
!       200 format('sselapsed:', f15.8)
!       close (2000)
        
       ! TODO: wangying debug
       !integer::i,k=1
       !real::e
       !real,dimension(2):: t(2)
       !open (unit=2000, file='test.txt')
       !e = etime( t )
       !write(unit=2000, fmt=100),e,t(1),t(2)
       !100 format('elapsed:', f5.8, ', user:', f5.4, ', sys:', f5.4)
       !do i = 1, 1000000
       !   k=k+1
       !end do
       !e = etime( t )
       !write(unit=2000, fmt=200),e,t(1),t(2)
       !200 format('elapsed:', f5.8, ', user:', f5.4, ', sys:', f5.4)
       !close (2000)

	    liNoTSignal = iZERO
        giNoTradeItemEachRun = iZERO
        call s_init_eachPOS(gstPosDataYest)
      
        call s_init_eachPOS(gstPosDataToday)
        do liI = 0, (lstStructPositionIn%cur_pos%count-1)
            call s_read_position(gcTicker, lstStructPositionIn%cur_pos%pos(liI), gstPosDataToday)
        end do
	    gstPosData%cTicker = trim(gcTicker)
	    gstPosData%iPOSD = gstPosDataYest%iPOSD + gstPosDataToday%iPOSD
        gstPosData%iPOSK = gstPosDataYest%iPOSK + gstPosDataToday%iPOSK
	    gstPosData%iMaxPOS = giMaxPos
	    giInitPOS = 1
        lstrat_out_log=gstrat_out_log	
        lstrat_out_log%exch_time=0
    end subroutine st_feed_init_position
    
    !----------------------------------------------------
    ! subroutine st_init(s_cStructIn, liNoTSignal, lastStructTSignal)
    ! description: initialization function
    ! input: liFileNumber
    !        lcFileName 
    !----------------------------------------------------          
    subroutine st_init(lstStructInitConfig, liRetCode,lstrat_out_log)
    	Use Common_Var_Def
        Use Common_Func
        Use Common_Mod
        Use MySub_Fuc
	    implicit none
	    type(cStruct_strategy_config), intent(inout)	::	lstStructInitConfig
	    integer(c_int), intent(inout)				::	liRetCode
	    type(strat_out_log), intent(inout)          ::  lstrat_out_log
		character                                   ::  lcSecName*4
        
	    liRetCode = 1
	    gcTicker = f_convert_c_char_to_f_string(lstStructInitConfig%ticker_detail(0)%ticker, 6)
	    lcSecName = trim(f_find_security_name_new(gcTicker)) 
		giStID = lstStructInitConfig%strategy_id; 
        giMaxPos = lstStructInitConfig%ticker_detail(0)%max_pos
		
        !if(trim(lcsecname)=='cf') then
        !   lststructinitconfig%log_id=11111
        !   lststructinitconfig%extvariable_id=21111
        !else
        !   lststructinitconfig%log_id=11112
        !   lststructinitconfig%extvariable_id=21112
        !end if

        call mysub0(lstStructInitConfig%log_id, &
					    f_convert_c_char_to_f_string(lstStructInitConfig%ticker_detail(0)%ticker_log_name, 256))        
        !write(lstStructInitConfig%log_id,"(a10,1x,i12,1x,a256)")    'mysint0_ev1',lstStructInitConfig%log_id, f_convert_c_char_to_f_string(lstStructInitConfig%ticker_detail(0)%ticker_log_name, 256)
        !write(lstStructInitConfig%log_id,"(a10,1x,i12,1x,a256)")    'mysint0_ev2',lstStructInitConfig%extvariable_id, f_convert_c_char_to_f_string(lstStructInitConfig%extvariable_name, 256)
        call s_open_ev_data(lstStructInitConfig%extvariable_id, f_convert_c_char_to_f_string(lstStructInitConfig%extvariable_name, 256))
	    lstrat_out_log=gstrat_out_log
        lstrat_out_log%exch_time=0		    
        liRetCode = iZERO
    end subroutine st_init
    
    !----------------------------------------------------
    ! subroutine st_destroy()
    ! description: program quit
    !----------------------------------------------------           
    subroutine st_destroy()
    	Use Common_Var_Def
        Use Common_Func
        Use Common_Mod
	    implicit none
        
        close(giFileNumber)
        
    end subroutine st_destroy
        
    
    !----------------------------------------------------
    ! subroutine st_feed_marketinfo_0(lstStructIn, liNoTSignal, lastStructTSignal)
    ! description: market info function for CFFE; 0 cfex_deep_quote
    ! input: lstStructIn
    ! output:   liNoTSignal, lastStructTSignal 
    !----------------------------------------------------	
    subroutine st_feed_marketinfo_0(lstStructIn, liNoTSignal, lastStructTSignal,lstrat_out_log)
    	Use Common_Var_Def
        Use Common_Func
        Use Common_Mod
	    implicit none
        type(cStruct_gta_quote), intent(inout) ::	lstStructIn
        type(cStruct_TSignal), dimension(0:(iSIGNAL_LEN - 1)), intent(inout)	::	lastStructTSignal
        integer(c_int), intent(inout)		::	liNoTSignal
	    type(strat_out_log), intent(inout)          ::  lstrat_out_log
        
        ! init  global parameters
        if (giInitFlg == 0) then
            gcExch = cCFFE
            giTickNo = 0
            gstRecCurrIn%iTime=0
        end if

        ! load data from lstStructIn to gstCurrIn
        if (giRunModeFlg == 1) then
            gstcStructCFFEIn = lstStructIn
            call s_read_fut_struct(giTickNo)     
        end if

        call s_run_each_record(liNoTSignal) 
        if(gstRecCurrIn%iTime<gstCurrIn%iTime) then        
        gstRecCurrIn = gstCurrIn		
        ! send signal
	    call s_assign_out_tsignal(gstTSignal, lbound(gstTSignal, 1), ubound(gstTSignal, 1), giNoTradeItemEachRun, &
								    lastStructTSignal, lbound(lastStructTSignal, 1), iSIGNAL_LEN, liNoTSignal, &
								    gstGlobalPara%stSec%iTickSizeMulti, gstGlobalPara%stSec%iPMultiplier) 
     !    if (giNoTradeItemEachRun >0 ) write(giFileNumber,"(a5,1x,i3,1x,i3)")    'out3',lbound(lastStructTSignal, 1),giNoTradeItemEachRun          
       
        end if
           lstrat_out_log=gstrat_out_log
           lstrat_out_log%exch_time=0 
    end subroutine st_feed_marketinfo_0      

    !----------------------------------------------------
    ! subroutine st_feed_marketinfo_1(lstStructIn, liNoTSignal, lastStructTSignal)
    ! description: market info function for DCE table 1; 1 dce_my_best_deep_quote
    ! input: lstStructIn
    ! output:   liNoTSignal, lastStructTSignal 
    !----------------------------------------------------	
    subroutine st_feed_marketinfo_1(lstStructIn, liNoTSignal, lastStructTSignal,lstrat_out_log)
    	Use Common_Var_Def
        Use Common_Func
        Use Common_Mod
	    implicit none
        type(cStruct_dl_quote), intent(inout) ::	lstStructIn
        type(cStruct_TSignal), dimension(0:(iSIGNAL_LEN - 1)), intent(inout)	::	lastStructTSignal
        integer(c_int), intent(inout)		::	liNoTSignal
	    type(strat_out_log), intent(inout)          ::  lstrat_out_log
        
        ! init  global parameters
        if (giInitFlg == 0) then
            gcExch = cDCE
            giTickNo = 0
            gstRecCurrIn%iTime=0
        end if

        ! load data from lstStructIn to gstCurrIn
        if (giRunModeFlg == 1) then
            gstcStructDCEIn = lstStructIn
            call s_read_fut_struct(giTickNo)     
        end if
		gidata_vaild=1
        call s_run_each_record(liNoTSignal) 
        if(gstRecCurrIn%iTime<gstCurrIn%iTime) then        
        gstRecCurrIn = gstCurrIn		
        ! send signal
	    call s_assign_out_tsignal(gstTSignal, lbound(gstTSignal, 1), ubound(gstTSignal, 1), giNoTradeItemEachRun, &
								    lastStructTSignal, lbound(lastStructTSignal, 1), iSIGNAL_LEN, liNoTSignal, &
								    gstGlobalPara%stSec%iTickSizeMulti, gstGlobalPara%stSec%iPMultiplier) 
        ! if (giNoTradeItemEachRun >0 ) write(giFileNumber,"(a5,1x,i3,1x,i3)")    'out4',lbound(lastStructTSignal, 1),giNoTradeItemEachRun          
        end if
           lstrat_out_log=gstrat_out_log
           if(gidata_vaild==1) lstrat_out_log%exch_time=0  
    end subroutine st_feed_marketinfo_1  
    
   
    !----------------------------------------------------
    ! subroutine st_feed_marketinfo_3(lstcStructDLOrderStatIn, liNoTSignal, lastStructTSignal)
    ! description: market info function for DCE table 3; 3 dce_my_ord_stat_quote
    ! input: lstcStructDLOrderStatIn
    ! output:   liNoTSignal, lastStructTSignal 
    !----------------------------------------------------	
    subroutine st_feed_marketinfo_3(lstcStructDLOrderStatIn, liNoTSignal, lastStructTSignal)
    	Use Common_Var_Def
        Use Common_Func
        Use Common_Mod
	    implicit none
        type(cStruct_dl_orderstat), intent(in)		::	lstcStructDLOrderStatIn
        type(cStruct_TSignal), dimension(0:(iSIGNAL_LEN - 1)), intent(inout)	::	lastStructTSignal
        integer(c_int), intent(inout)		::	liNoTSignal
        
        call s_read_cstruct_dl_orderstat(lstcStructDLOrderStatIn, gstCurrIn)
        liNoTSignal = iZERO
        
    end subroutine st_feed_marketinfo_3 
 
    !----------------------------------------------------
    ! subroutine st_feed_marketinfo_6(lstStructIn, liNoTSignal, lastStructTSignal)
    ! description: market info function for SHFE;  6 shfe_my_quote
    ! input: lstStructIn
    ! output:   liNoTSignal, lastStructTSignal 
    !----------------------------------------------------	
    subroutine st_feed_marketinfo_6(lstStructIn, liNoTSignal, lastStructTSignal,lstrat_out_log)
    	Use Common_Var_Def
        Use Common_Func
        Use Common_Mod
	    implicit none
        type(cStruct_shfe_quote), intent(inout) ::	lstStructIn
        type(cStruct_TSignal), dimension(0:(iSIGNAL_LEN - 1)), intent(inout)	::	lastStructTSignal
        integer(c_int), intent(inout)		::	liNoTSignal
	    type(strat_out_log), intent(inout)          ::  lstrat_out_log
        
        ! init  global parameters
        if (giInitFlg == 0) then
            gcExch = cSHFE
            giTickNo = 0
            gstRecCurrIn%iTime=0
        end if

        ! load data from lstStructIn to gstCurrIn
        if (giRunModeFlg == 1) then
            gstcStructSHFEIn = lstStructIn
            if (mod(gstcStructSHFEIn%ndata_flag,3) /= 0) return            
            call s_read_fut_struct(giTickNo)    
        end if
		
		gidata_vaild=1
        if(mod(gstcStructSHFEIn%ndata_flag,3)==0) then
           call s_run_each_record(liNoTSignal) 
           ! send signal
           if(gstRecCurrIn%iTime<gstCurrIn%iTime) then        
              gstRecCurrIn = gstCurrIn      
              if(mod(gstCurrIn%iData_flag,3)==0 .and. mod(gstBfIn%iData_flag,3)==0 .and. 1.5*gstCurrIn%iTotalBuyvol >1.0*gstBfIn%iTotalBuyvol .and.1.5*gstCurrIn%iTotalSelvol>1.0*gstBfIn%iTotalSelvol .and.&
                                                      1.5*gstBfIn%iTotalBuyvol >1.0*gstCurrIn%iTotalBuyvol .and.1.5*gstBfIn%iTotalSelvol>1.0*gstCurrIn%iTotalSelvol) then
	          call s_assign_out_tsignal(gstTSignal, lbound(gstTSignal, 1), ubound(gstTSignal, 1), giNoTradeItemEachRun, &
								        lastStructTSignal, lbound(lastStructTSignal, 1), iSIGNAL_LEN, liNoTSignal, &
								        gstGlobalPara%stSec%iTickSizeMulti, gstGlobalPara%stSec%iPMultiplier)
              end if								    
         !   if (giNoTradeItemEachRun >0 ) write(giFileNumber,"(a6,1x,i3,1x,i3,2(1x,i3),4(1x,i10))")    'out5',lbound(lastStructTSignal, 1),giNoTradeItemEachRun,gstCurrIn%iData_flag,gstBfIn%iData_flag,gstCurrIn%iTotalBuyvol,gstBfIn%iTotalBuyvol,gstCurrIn%iTotalSelvol,gstBfIn%iTotalSelvol
              if(mod(gstCurrIn%iData_flag,3)==0) gstBfIn=gstCurrIn 
           end if
        end if
           lstrat_out_log=gstrat_out_log
           if(gidata_vaild==1) lstrat_out_log%exch_time=0 
    end subroutine st_feed_marketinfo_6    
    
    !----------------------------------------------------
    ! subroutine st_feed_marketinfo_8(lstStructIn, liNoTSignal, lastStructTSignal)
    ! description: market info function for CZCE;  7 czce_my_deep_quote
    ! input: lstStructIn
    ! output:   liNoTSignal, lastStructTSignal 
    !----------------------------------------------------	
    subroutine st_feed_marketinfo_8(lstStructIn, liNoTSignal, lastStructTSignal,lstrat_out_log)
    	Use Common_Var_Def
        Use Common_Func
        Use Common_Mod
	    implicit none
        type(cStruct_zz_quote), intent(inout) ::	lstStructIn
        type(cStruct_TSignal), dimension(0:(iSIGNAL_LEN - 1)), intent(inout)	::	lastStructTSignal
        integer(c_int), intent(inout)		::	liNoTSignal
	    type(strat_out_log), intent(inout)          ::  lstrat_out_log
        
        ! init  global parameters
        if (giInitFlg == 0) then
            gcExch = cCZCE
            giTickNo = 0
            gstRecCurrIn%iTime=0
        end if

        ! load data from lstStructIn to gstCurrIn
        if (giRunModeFlg == 1) then
            gstcStructCZCEIn = lstStructIn
            call s_read_fut_struct(giTickNo) 
        end if
		gidata_vaild=1
        call s_run_each_record(liNoTSignal) 
        if(gstRecCurrIn%iTime<gstCurrIn%iTime) then        
           gstRecCurrIn = gstCurrIn		
           ! send signal
	       call s_assign_out_tsignal(gstTSignal, lbound(gstTSignal, 1), ubound(gstTSignal, 1), giNoTradeItemEachRun, &
								     lastStructTSignal, lbound(lastStructTSignal, 1), iSIGNAL_LEN, liNoTSignal, &
								     gstGlobalPara%stSec%iTickSizeMulti, gstGlobalPara%stSec%iPMultiplier)
         !   if (giNoTradeItemEachRun >0 )  write(giFileNumber,"(a5,1x,i3,1x,i3)")    'out6',lbound(lastStructTSignal, 1),giNoTradeItemEachRun          
        end if
        lstrat_out_log=gstrat_out_log
        if(gidata_vaild==1) lstrat_out_log%exch_time=0 
    end subroutine st_feed_marketinfo_8        
    

