/*
 * MyExchangeAgent.cpp
 *
 *  Created on: Jun 18, 2015
 *      Author: root
 */

#include <iostream>
#include <dlfcn.h>
#include "MyExchangeAgent.h"
#include <log4cxx/logger.h>
#include <log4cxx/xml/domconfigurator.h>




using namespace trading_channel_agent;

map<string,void*> MyExchangeFactory::so_handles_;
map<string,DestroyExchange_t*> MyExchangeFactory::destroies_;

MYExchangeInterface * MyExchangeFactory::create(string ex_so_file,struct my_xchg_cfg& tunnel_conf)
{
	MYExchangeInterface* ex = NULL;

	void *so_handle_ = dlopen(ex_so_file.c_str(), RTLD_NOW );
	if (NULL == so_handle_) {
		LOG4CXX_INFO(log4cxx::Logger::getRootLogger(),
				"MyExchangeFactory::create execution failed, cause:" << dlerror());
	}
	else{
		so_handles_[ex_so_file] = so_handle_;

		CreateExchange_t* create_ex = (CreateExchange_t*) dlsym(so_handle_, "CreateExchange");
		const char* dlsym_error = dlerror();
		if (dlsym_error) {
			LOG4CXX_INFO(log4cxx::Logger::getRootLogger(),
				"MyExchangeFactory::create execution failed, cause:" << dlsym_error);
		}

		DestroyExchange_t* destroy_ex = (DestroyExchange_t*) dlsym(so_handle_, "DestroyExchange");
		dlsym_error = dlerror();
		if (dlsym_error) {
			LOG4CXX_INFO(log4cxx::Logger::getRootLogger(),
				"MyExchangeFactory::create execution failed, cause:" << dlsym_error);
		}
		destroies_[ex_so_file] = destroy_ex;

		if(NULL != create_ex){
			ex = create_ex(tunnel_conf);
		}
	}

	return ex;
}

void MyExchangeFactory::destroy(string ex_so_file,MYExchangeInterface * ex)
{
	destroies_[ex_so_file](ex);
	dlclose(so_handles_[ex_so_file]);
}

