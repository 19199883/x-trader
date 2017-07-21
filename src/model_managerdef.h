#include <boost/foreach.hpp>
#include <log4cxx/logger.h>

using namespace strategy_manager;
using namespace log4cxx;
using namespace log4cxx::helpers;

template<typename SPIFQuoteT,typename CFQuoteT,typename StockQuoteT,typename FullDepthQuoteT,typename QuoteT5>
model_manager<SPIFQuoteT,CFQuoteT,StockQuoteT,FullDepthQuoteT,QuoteT5>::model_manager(void)
{	
	_pproxy = CLoadLibraryProxy::CreateLoadLibraryProxy();
}

template<typename SPIFQuoteT,typename CFQuoteT,typename StockQuoteT,typename FullDepthQuoteT,typename QuoteT5>
model_manager<SPIFQuoteT,CFQuoteT,StockQuoteT,FullDepthQuoteT,QuoteT5>::~model_manager(void)
{
	this->finalize();
}

template<typename SPIFQuoteT,typename CFQuoteT,typename StockQuoteT,typename FullDepthQuoteT,typename QuoteT5>
void model_manager<SPIFQuoteT,CFQuoteT,StockQuoteT,FullDepthQuoteT,QuoteT5>::initialize(void)
{
	setting.initialize();
	BOOST_FOREACH(model_setting setting, this->setting.models)
	{
		ModelAdapterT *model = new ModelAdapterT(setting);
		this->models.push_back(shared_ptr<ModelAdapterT>(model));
		model->initialize();
	}
}

template<typename SPIFQuoteT,typename CFQuoteT,typename StockQuoteT,typename FullDepthQuoteT,typename QuoteT5>
void model_manager<SPIFQuoteT,CFQuoteT,StockQuoteT,FullDepthQuoteT,QuoteT5>::finalize(void)
{
	for (typename ModelAdapterListT::iterator it=models.begin();it!=models.end();++it)
	{
		(*it)->finalize();
		LOG4CXX_INFO(log4cxx::Logger::getRootLogger(),"delete model succesfully:" << (*it)->setting.id);
	}
	models.clear();

	setting.finalize();

	if (_pproxy!= NULL)
	{
		_pproxy->DeleteLoadLibraryProxy();
		_pproxy = NULL;
	}

	LOG4CXX_INFO(log4cxx::Logger::getRootLogger(),"delete shared library succesfully");
}
