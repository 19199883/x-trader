#pragma once

#include <string>
#include <map>
#include "position_entity.h"
#include "model_setting.h"
#include "quotecategoryoptions.h"
#include "model_config.h"
#include "my_order.h"

using namespace std;

/**
model_setting定义模型配置信息。
*/
class model_setting	{
public:
	// the configuration to tell model
	st_config_t config_;
	string account;
	string name;
	int id;
	/*
	 * model file's relative path,include file name.
	 */
	string file;

public:
	model_setting(void){}
	~model_setting(void){}
};
