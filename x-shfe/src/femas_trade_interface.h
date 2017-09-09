class MyFemasTradeSpi: public CUstpFtdcTraderSpi
{
public:

	// TODO: here
    int Front_id() const
    {
        return front_id_;
    }
    int Session_id() const
    {
        return session_id_;
    }

private:
      
    void OnRtnOrderNormal(CUstpFtdcOrderField * pOrder, const OriginalReqInfo * p, OrderRefDataType order_ref);

    void HandleFillupRsp(long entrust_no, SerialNoType serial_no);
    void HandleFillupRtn(long entrust_no, char order_status, const OriginalReqInfo *p);

private:
    int front_id_;
    int session_id_;    

    Tunnel_Info tunnel_info_;
    std::string user_;
    std::string pswd_;
    std::string quote_addr_;
    std::string exchange_code_;

    // 配置数据对象
    TunnelConfigData cfg_;  
    void ReportErrorState(int api_error_no, const std::string &error_msg);
    CUstpFtdcOrderActionField CreatCancelParam(const CUstpFtdcOrderField & order_field);
    bool IsOrderTerminate(const CUstpFtdcOrderField & order_field);

};

#endif //
