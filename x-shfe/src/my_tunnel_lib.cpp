FemasTunnel::FemasTunnel(const std::string &provider_config_file)
{
    // load config file
    std::string cfg_file("my_trade_tunnel_femas.xml");
    if (!provider_config_file.empty())
    {
        cfg_file = provider_config_file;
    }
    //TunnelConfigData cfg;
    lib_cfg_ = new TunnelConfigData();
    lib_cfg_->Load(cfg_file);
    exchange_code_ = lib_cfg_->Logon_config().exch_code.c_str()[0];
    tunnel_info_.account = lib_cfg_->Logon_config().investorid;    
        InitInf(*lib_cfg_);         
}

std::string FemasTunnel::GetClientID()
{
    return tunnel_info_.account;
}

bool FemasTunnel::InitInf(const TunnelConfigData& cfg)
{
    TNL_LOG_INFO("prepare to start FEMAS tunnel server.");
   
    char init_msg[127];
    sprintf(init_msg, "%s: Init compliance check", tunnel_info_.account.c_str());
        trade_inf_ = new MyFemasTradeSpi(cfg);
    return true;
}

void FemasTunnel::PlaceOrder(const T_PlaceOrder *pPlaceOrder)
{
    MyFemasTradeSpi * p_tunnel = static_cast<MyFemasTradeSpi *>(trade_inf_);    
        OrderRefDataType order_ref = p_tunnel->femas_trade_context_.GetNewOrderRef();
        TNL_LOG_DEBUG("serial_no: %ld map order_ref: %lld", pPlaceOrder->serial_no, order_ref);
        int request_id = p_tunnel->femas_trade_context_.GetRequestID();                                      
            CUstpFtdcInputOrderField counter_req;
            FEMASPacker::OrderRequest(*lib_cfg_, pPlaceOrder, order_ref, counter_req);           
            process_result = p_tunnel->ReqOrderInsert(&counter_req, request_id);                
}

void FemasTunnel::CancelOrder(const T_CancelOrder *pCancelOrder)
{
    MyFemasTradeSpi * p_tunnel = static_cast<MyFemasTradeSpi *>(trade_inf_);
        OrderRefDataType order_ref = p_tunnel->femas_trade_context_.GetNewOrderRef();
        TNL_LOG_DEBUG("cancel_serial_no: %ld map action_ref: %lld", pCancelOrder->serial_no, order_ref);
        int request_id = p_tunnel->femas_trade_context_.GetRequestID();
        OrderRefDataType org_order_ref = p_tunnel->femas_trade_context_.GetOrderRefBySerialNo(pCancelOrder->org_serial_no);               
            CUstpFtdcOrderActionField counter_req;
            FEMASPacker::CancelRequest(*lib_cfg_, pCancelOrder, order_ref, org_order_ref, counter_req);
            char cancel_org_sn[64];
            sprintf(cancel_org_sn, "%ld", pCancelOrder->org_serial_no);            
            process_result = p_tunnel->ReqOrderAction(&counter_req, request_id);           
}


FemasTunnel::~FemasTunnel()
{
    MyFemasTradeSpi * p_tunnel = static_cast<MyFemasTradeSpi *>(trade_inf_);
    if (p_tunnel)
    {
        delete p_tunnel;
        trade_inf_ = NULL;
    }
    qtm_finish();
    LogUtil::Stop();
}

