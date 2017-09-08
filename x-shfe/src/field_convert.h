//将FEMAS的委托状态转换为统一的委托状态
char FEMASFieldConvert::EntrustStatusTrans(char OrderStatus)
{
    switch (OrderStatus)
    {
        //全部成交 --> 全部成交
        case USTP_FTDC_OS_AllTraded:
            return MY_TNL_OS_COMPLETED;

            //部分成交还在队列中 -->  部分成交
        case USTP_FTDC_OS_PartTradedQueueing:
            return MY_TNL_OS_PARTIALCOM;

            //部分成交不在队列中 -->  部成部撤
        case USTP_FTDC_OS_PartTradedNotQueueing:
            return MY_TNL_OS_WITHDRAWED;

            //未成交还在队列中  -->  已经报入
        case USTP_FTDC_OS_NoTradeQueueing:
            return MY_TNL_OS_REPORDED;

            //未成交不在队列中  -->  已经报入
        case USTP_FTDC_OS_NoTradeNotQueueing:
            return MY_TNL_OS_REPORDED;

            //撤单 -->  已经撤销
        case USTP_FTDC_OS_Canceled:
            return MY_TNL_OS_WITHDRAWED;

        default:
            return MY_TNL_OS_ERROR;
    }

    //错误委托
    return MY_TNL_OS_ERROR;
}


#endif // FIELD_CONVERT_H_
