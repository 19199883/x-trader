#pragma once

#include <list>
#include <tinyxml.h>
#include <tinystr.h>
#include "model_setting.h"
#include "model_setting.h"

using namespace std;

namespace strategy_manager
{
	/**
	sm_settins管理sm模块的配置信息
	*/
	class sm_settings
	{
	public:
		/**
		该字段存储模型的配置
		*/
		list<model_setting> models;

		// 该常量定义配置文件的路径
		static string config_path;

	public:
		sm_settings(void);
		~sm_settings(void);

		void  initialize(void);

		void finalize(void);

	private:
		model_setting create_model_setting(TiXmlElement* xml_ele);

		/*
		 * get fund account by configuration files
		 */
		string get_account(long model_id);

		void init_model_if(model_setting &strategy);
	};
}

