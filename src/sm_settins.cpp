#include <boost/filesystem/path.hpp>
#include "boost/algorithm/string.hpp"  
#include "boost/filesystem/path.hpp"  
#include "boost/filesystem/operations.hpp"  
#include "boost/format.hpp" 
#include "sm_settings.h"
#include <string>     // std::string, std::stol
#include "model_config.h"
#include <log4cxx/logger.h>
#include <log4cxx/xml/domconfigurator.h>

using namespace strategy_manager;
using namespace std;
using namespace quote_agent;
using namespace std;
using namespace log4cxx;
using namespace log4cxx::xml;
using namespace log4cxx::helpers;

string sm_settings::config_path = "trasev.config";

sm_settings::sm_settings(void){

}

sm_settings::~sm_settings(void){}

void sm_settings::initialize(void){
	TiXmlDocument config = TiXmlDocument(sm_settings::config_path.c_str());
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    
    TiXmlElement *strategies = RootElement->FirstChildElement("strategies");
	if (strategies != 0){
		TiXmlElement *strategy = strategies->FirstChildElement();
		while (strategy != 0){
			model_setting source = this->create_model_setting(strategy);
			this->models.push_back(source);
			strategy = strategy->NextSiblingElement();			
		}
	}  
}

model_setting sm_settings::create_model_setting(TiXmlElement* xml_ele)
{
	model_setting strategy;

	strcpy(strategy.config_.st_name, xml_ele->Attribute("model_file"));

	strategy.config_.st_id = stoi(xml_ele->Attribute("id"));
	// T29
	strategy.id = stol(xml_ele->Attribute("id"));
	strategy.file = xml_ele->Attribute("model_file");

	// log_id
	strategy.config_.log_id = strategy.id *10 + 0;
	// log_name
	strcpy(strategy.config_.log_name, xml_ele->Attribute("log_name"));
	// iv_id
	strategy.config_.iv_id = strategy.id *10 + 1;
	// iv_name
	strcpy(strategy.config_.iv_name ,xml_ele->Attribute("iv_name"));
	// ev_id
	strategy.config_.ev_id = strategy.id*10 + 2;
	// ev_name
	const char * name = xml_ele->Attribute("ev_name");
	strcpy(strategy.config_.ev_name ,xml_ele->Attribute("ev_name"));

	int counter = 0;
	TiXmlElement* symbol_ele = xml_ele->FirstChildElement();		
	while (symbol_ele != 0)	{		
		symbol_t &tmp = strategy.config_.symbols[counter];

		symbol_ele->QueryIntAttribute("max_pos", &tmp.max_pos);
		strcpy(tmp.name,symbol_ele->Attribute("name"));
		string exc_str = symbol_ele->Attribute("exchange");
		tmp.exchange = static_cast<exchange_names>(exc_str[0]);
		symbol_ele = symbol_ele->NextSiblingElement();
		counter++;
	}	//end while (symbol_ele != 0)
	strategy.config_.symbols_cnt = counter;
	   
	return strategy;
}

void sm_settings::finalize(void)
{
}	


