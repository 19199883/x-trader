
// Femas Packer
class FEMASPacker
{
public:

    static void OrderRequest(const TunnelConfigData &cfg, const T_PlaceOrder *req,
        OrderRefDataType new_order_ref, CUstpFtdcInputOrderField &insert_order);

    static void OrderRespond(int error_no, SerialNoType serial_no, const char* entrust_no,
        char entrust_status, T_OrderRespond &order_respond);

    static void CancelRequest(const TunnelConfigData &cfg, const T_CancelOrder *req, OrderRefDataType order_ref,
        OrderRefDataType org_order_ref, CUstpFtdcOrderActionField &cancle_order);

    static void CancelRespond(int error_no, SerialNoType serial_no, const char* entrust_no, T_CancelRespond &cancle_order);

    static void OrderReturn(SerialNoType serial_no, const CUstpFtdcOrderField *rsp, T_OrderReturn &order_return);

    static void TradeReturn(const OriginalReqInfo *p_req, const CUstpFtdcTradeField *rsp, T_TradeReturn &trade_return);
};

#endif // MY_PROTOCOL_Packer_H_
