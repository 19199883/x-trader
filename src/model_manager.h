#pragma once
#include "../my_exchange.h"
#include <list>
#include <boost/shared_ptr.hpp>
#include "sm_settings.h"
#include "model_adapter.h"
#include <dlfcn.h>
#include "moduleloadlibrarylinux.h"
#include <memory>
#include "loadlibraryproxy.h"

using namespace std;

namespace strategy_manager
{
	/**
	(1) 根据合约查找模型，转发信息
	（2）管理模型
	TO LEARN:
	（1）对于虚函数，在子类的虚函数中如何调用基类的函数？
	*/
	template<typename SPIFQuoteT,typename CFQuoteT,typename StockQuoteT,typename FullDepthQuoteT,typename QuoteT5>
	class model_manager
	{
		typedef model_adapter<SPIFQuoteT,CFQuoteT,StockQuoteT,FullDepthQuoteT,QuoteT5> ModelAdapterT;
		typedef list<shared_ptr<ModelAdapterT> > ModelAdapterListT;

	public:
		/**
		该字段存储MyExchange中所有模型列表
		*/
		ModelAdapterListT models;

		CLoadLibraryProxy *_pproxy;

	private:
		/**
		该字段存储sm模块的配置信息
		*/
		sm_settings setting;

	public:
		model_manager(void);
		~model_manager(void);

		void initialize(void);

		void finalize(void);				
	};
}

#include "model_managerdef.h"
