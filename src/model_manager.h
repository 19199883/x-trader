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
	template<typename SPIFQuoteT,typename CFQuoteT,typename StockQuoteT,typename FullDepthQuoteT,typename QuoteT5>
	class model_manager
	{
		typedef model_adapter<SPIFQuoteT,CFQuoteT,StockQuoteT,FullDepthQuoteT,QuoteT5> ModelAdapterT;
		typedef list<shared_ptr<ModelAdapterT> > ModelAdapterListT;

	public:
		ModelAdapterListT models;

		CLoadLibraryProxy *_pproxy;

	private:
		sm_settings setting;

	public:
		model_manager(void);
		~model_manager(void);

		void initialize(void);

		void finalize(void);				
	};
}

#include "model_managerdef.h"
