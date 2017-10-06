#ifndef MY_SHFE_MD_QUOTE_INTERFACE_JR_H_
#define MY_SHFE_MD_QUOTE_INTERFACE_JR_H_

#include <sys/types.h>
#include <stdint.h>
#include <dirent.h>
#include <string>
#include <vector>

#include <functional>   // std::bind
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::lock_guard

#include "my_cmn_util_funcs.h"
#include "quote_cmn_utility.h"
#include "quote_cmn_config.h"
#include "quote_cmn_save.h"

#include "quote_datatype_shfe_my.h"
#include "quote_datatype_shfe_deep.h"
//#include "quote_datatype_level1.h"

#include "quote_interface_shfe_my.h"

#include "shfe_my_data_manager.h"
#include "quote_femas.h"
#include "repairer.h"

/* 1. recevies SHFEQuote by UDP and send it to MYShfeMDManager and subscriber
 * 2. receives CDepthMarketDataField by Femas and send it to MYShfeMDManager and subscriber
 * 3. receives MYShfeMarketData from MYShfeMDManager and subscriber
 */
class QuoteInterface_MY_SHFE_MD: public MYShfeMDHandler
{
public:
    // 构造函数
    QuoteInterface_MY_SHFE_MD(const SubscribeContracts *subscribe_contracts, const ConfigData &cfg);

    // 数据处理回调函数设置
    void SetQuoteDataHandler(std::function<void(const SHFEQuote *)> quote_data_handler)
    {
        shfe_deep_data_handler_ = quote_data_handler;
    }
    void SetQuoteDataHandler(std::function<void(const CDepthMarketDataField *)> quote_data_handler)
    {
        shfe_ex_handler_ = quote_data_handler;
    }
    void SetQuoteDataHandler(std::function<void(const MYShfeMarketData *)> quote_data_handler)
    {
        my_shfe_md_handler_ = quote_data_handler;
    }

    virtual ~QuoteInterface_MY_SHFE_MD();

private:
    // quote data handlers
    void OnMYShfeMDData(MYShfeMarketData *data);
    void ShfeDepthMarketDataHandler(const CDepthMarketDataField * data);
    void ShfeMBLHandler();

    int CreateUdpFD(const std::string &addr_ip, unsigned short port);

	void proc_udp_data(MDPackEx &data);

    // 数据处理函数对象
    std::function<void(const SHFEQuote *)> shfe_deep_data_handler_;
    std::function<void(const CDepthMarketDataField *)> shfe_ex_handler_;
    std::function<void(const MYShfeMarketData *)> my_shfe_md_handler_;

	std::string ToString(const MDPack &d);

    // 订阅合约集合
    SubscribeContracts subscribe_contracts_;

    // 配置数据对象
    ConfigData cfg_;
    char qtm_name_[32];

    int seq_no_;
    int server_;

    // save assistant object
    QuoteDataSave<SHFEQuote> *p_shfe_deep_save_;
    QuoteDataSave<CDepthMarketDataField> *p_shfe_ex_save_;
    QuoteDataSave<MYShfeMarketData> *p_my_shfe_md_save_;

    // receive threads
    volatile bool running_flag_;
    std::thread *p_mbl_handler_;

    MYShfeMDManager my_shfe_md_inf_;

    CMdclientHandler *femas_inf_;
};

#endif
