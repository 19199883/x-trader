#ifndef FEMAS_QUOTE_INTERFACE_H_
#define FEMAS_QUOTE_INTERFACE_H_

#include <string>
#include <sstream>
#include <list>
#include <unordered_map>
#include <map>
#include "mdclient.h"
#include "quote_datatype_level1.h"
#include "quote_cmn_utility.h"
#include "quote_cmn_config.h"
#include "quote_cmn_save.h"
#include "quote_interface_shfe_my.h"

/*
 *MYFEMASDataHandler receives market data from exchange by Femas system
 *and then send them to QuoteInterface_MY_SHFE_MD by QuoteInterface_MY_SHFE_MD::ShfeDepthMarketDataHandler 
 */
class CMdclientHandler : public CMdclientSpi
{
public:
	CMdclientHandler(const SubscribeContracts *subscribe_contracts, const ConfigData &cfg);

    // 数据处理回调函数设置
    void SetQuoteDataHandler(std::function<void(const CDepthMarketDataField *)> quote_data_handler);

    virtual ~CMdclientHandler(void);

	///深度行情通知
	virtual void OnRtnDepthMarketData(CDepthMarketDataField *pDepthMarketData);

private:
	CMdclientApi *api_;
	int sub_count_;

    // 数据处理函数对象
    std::function<void(const CDepthMarketDataField *)> quote_data_handler_;

    // 配置数据对象
    ConfigData cfg_;

    // save assistant object
    void RalaceInvalidValue_Femas(CDepthMarketDataField &d);
};

#endif // GTA_QUOTE_INTERFACE_H_
