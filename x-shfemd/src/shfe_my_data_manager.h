#ifndef SHFE_MY_DATA_MANAGER_H_
#define SHFE_MY_DATA_MANAGER_H_

#include <map>
#include <vector>
#include <pthread.h>
#include "quote_cmn_config.h"
#include "quote_datatype_shfe_my.h"
#include "quote_datatype_shfe_deep.h"
#include "quote_datatype_level1.h"
#include "quote_cmn_utility.h"

class MYShfeMDHandler
{
public:
	virtual void OnMYShfeMDData(MYShfeMarketData *data) = 0;
};

class MYShfeMDManager
{
public:
    MYShfeMDManager(const ConfigData &cfg);
	void SetDataHandler(MYShfeMDHandler * data_handler)
	{
		data_handler_ = data_handler;
	}

private:
    const ConfigData &cfg_;
	void SendToClient(const std::string &code, SHFEMDQuoteSnapshot * const p_data);
	MYShfeMDHandler *data_handler_;
};

#endif // SHFE_MY_DATA_MANAGER_H_
