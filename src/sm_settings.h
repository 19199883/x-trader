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
	sm_settins����smģ���������Ϣ
	*/
	class sm_settings
	{
	public:
		/**
		���ֶδ洢ģ�͵�����
		*/
		list<model_setting> models;

		// �ó������������ļ���·��
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

