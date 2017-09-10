
// Femas Packer
class FEMASPacker
{
public:

    static void OrderRequest(const TunnelConfigData &cfg, const T_PlaceOrder *req,
        OrderRefDataType new_order_ref, CUstpFtdcInputOrderField &insert_order);

    static void CancelRequest(const TunnelConfigData &cfg, const T_CancelOrder *req, OrderRefDataType order_ref,
        OrderRefDataType org_order_ref, CUstpFtdcOrderActionField &cancle_order);

    static void CancelRespond(int error_no, SerialNoType serial_no, const char* entrust_no, T_CancelRespond &cancle_order);

};

#endif // MY_PROTOCOL_Packer_H_
