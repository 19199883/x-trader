Module Hi5_Var_Def
	Use Common_Var_Def
	Implicit None
	Save
    
    type hi5_paras
        real(dp) :: rBVSum, rSVSum, rStopLossThreshold, rVolBuy, rVolSel, rVolBuyPre, rVolSelPre, rPNet,rBSVSumRatio,rBSVChgSumRatio
        integer  :: iStop, iChgStatus,iLimitSqRange
    end type hi5_paras
    
    !----------------------------------------------------
    ! output data type
    !----------------------------------------------------	    
    type comm_out_data
        character	::	cTicker*6
		integer :: iTime, iRecordNo, iV
        real(dp) :: rP, rBP1, rSP1, rVal, rOI, rBuyPrice, rSelPrice
        integer :: iBV1, iSV1, iHaveOrder, iTS, iCancel, iOpenSize, iCloseSize, iToTradeSize, iLongPos, iShortPos, iMaxPos, iTotalOrderSize, iTotalCancelSize        
    end type comm_out_data    

    type hi5_outdata
        type(comm_out_data) :: stCommOutData
        integer :: iTime, iVol, iBV1, iSV1, iSignalType, iVol2Trade, iPos, iPOSD, iPOSK, iRounds, iVol2Pass, iTickNo, iNewPOS, iNetPOS
        integer ::  iRound, iCanCelOrder
        real(dp) :: rP, rBP1, rSP1, rBVSVRatio, rTradeBP, rTradeSP, rCash, rVround, rPnl,rLivePL, rAvgP, rVolBuy, rVolSel, rPNet
        character :: cTradeSig*3, cTime*9   
    end type hi5_outdata

   type(hi5_outdata), allocatable	::	gastHi5OutData(:)  
   type (hi5_paras) :: gstHi5_Paras
   integer :: giHi5OutDataIndex, giHi5TraceInit
   real(dp), dimension(:), allocatable ::	garWgtArray(:), garWgtArrayInv(:) 
   integer, parameter   :: iHI5BPK = 2, iHI5BP = 9, iHI5BKP = 2, iHI5SPK = -2, iHI5SP = -9,iHI5K=2, iHI5P=9 
   integer              :: gsigno=0
   real(dp):: grbuyprice_old,grselprice_old
End Module Hi5_Var_Def