/*
 *      quote_interface_tap.h
 *
 *      the interface of TapApi
 *
 *      Copyright(c) 2007-2015, by MY Capital Inc.
 */
#if !defined(MY_QUOTE_INTERFACE_TAP_H_)
#define MY_QUOTE_INTERFACE_TAP_H_

#include "quote_datatype_tap.h"

#include <set>
#include <string>
#include <functional>


// 订阅的合约，集合类型
typedef std::set<std::string> SubscribeContracts;

// forward declare
class ConfigData;

namespace TAP
{
		class DLL_PUBLIC MYQuoteData
		{
		public:

			/**
			 * @param subscribe_contracts 需要订阅的合约集合。(指针为空指针，或集合为空，将返回行情接口接收到的所有合约数据)
			 * @param provider_config_file 行情的配置文件名
			 */
			MYQuoteData(const SubscribeContracts *subscribe_contracts, const std::string &provider_config_file);

			// tap market data handlers
			void SetQuoteDataHandler(std::function<void(const  TapAPIQuoteWhole_MY *)> quote_handler);
			
			void SetQuoteDataHandler(std::function<void(const int *)> quote_handler){}

			~MYQuoteData();

		private:
			// 禁止拷贝和赋值
			MYQuoteData(const MYQuoteData & other);
			MYQuoteData operator=(const MYQuoteData & other);

			// 内部实现接口
			bool InitInterface(const SubscribeContracts *subscribe_contracts, const ConfigData &cfg);

			void *interface_;
			int quote_provider_type_;
		};
}

#endif  //MY_QUOTE_INTERFACE_CZCE_LEVEL2_H_
