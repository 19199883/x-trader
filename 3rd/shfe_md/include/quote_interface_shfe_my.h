﻿#if !defined(MY_QUOTE_INTERFACE_SHFE_MY_H_)
#define MY_QUOTE_INTERFACE_SHFE_MY_H_

#include <set>
#include <string>
#include <functional>   // std::bind
#include "quote_datatype_shfe_my.h"
#include "quote_datatype_shfe_deep.h"
#include "quote_datatype_level1.h"

// 订阅的合约，集合类型
typedef std::set<std::string> SubscribeContracts;

// forward declare
class ConfigData;

class DLL_PUBLIC MYQuoteData
{
public:

    /**
     * @param subscribe_contracts 需要订阅的合约集合。
	 * (指针为空指针，或集合为空，将返回行情接口接收到的所有合约数据)
     * @param provider_config_file 行情的配置文件名
     */
    MYQuoteData(const SubscribeContracts *subscribe_contracts,
		const std::string &provider_config_file);

    /**
     * @param quote_handler 行情处理的函数对象。shfe quote of my（xh提供）
     */
    void SetQuoteDataHandler(std::function<void(const SHFEQuote *)> quote_handler);
    void SetQuoteDataHandler(std::function<void(const CDepthMarketDataField *)> quote_handler);
    void SetQuoteDataHandler(std::function<void(const MYShfeMarketData *)> quote_handler);
	void SetQuoteDataHandler(std::function<void(const int *)> quote_handler){}

    ~MYQuoteData();

private:
    // 禁止拷贝和赋值
    MYQuoteData(const MYQuoteData & other);
    MYQuoteData operator=(const MYQuoteData & other);

    // 内部实现接口
    bool InitInterface();

private:
    // 数据处理函数对象
    std::function<void(const MYShfeMarketData *)> fulldepthmd_handler_;
};

#endif  //MY_QUOTE_INTERFACE_SHFE_MY_H_
