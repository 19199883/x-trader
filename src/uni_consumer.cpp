#include <tinyxml.h>
#include <tinystr.h>
#include "uni_consumer.h"

UniConsumer::UniConsumer(struct vrt_queue  *queue, MDProducer *md_producer)
: module_name_("uni_consumer"),running_(true), md_producer_(md_producer)
{
	clog_info("[%s] STRA_TABLE_SIZE: %d;", module_name_, STRA_TABLE_SIZE);

	rip_check(this->consumer_ = vrt_consumer_new(module_name_, queue));
	this->consumer_->yield = vrt_yield_strategy_threaded();

	ParseConfig();
	// create Stratedy objects
	CreateStrategies();
}

~UniConsumer::UniConsumer()
{
	running_ = false;

	if (this->consumer_ != NULL){
		vrt_consumer_free(this->consumer_);
		this->consumer_ = NULL;
		clog_info("[%s] release uni_consumer.", module_name_);
	}
}

void UniConsumer::ParseConfig()
{
	std::string config_file = "trasev.config";

	TiXmlDocument doc = TiXmlDocument(config_file.c_str());
    doc.LoadFile();
    TiXmlElement *root = config.RootElement();    
    TiXmlElement *strategies_ele = root->FirstChildElement("strategies");
	if (strategies_ele != 0){
		TiXmlElement *strategy_ele = strategies_ele->FirstChildElement();
		while (strategy_ele != 0){
			StrategySetting strategy_setting = this->CreateStrategySetting(strategy_ele);
			this->strategy_settings_.push_back(strategy_setting);
			strategy_ele = strategy_ele->NextSiblingElement();			
		}
	}  
}

StrategySetting UniConsumer::CreateStrategySetting(const TiXmlElement *ele)
{
	StrategySetting setting;
	strcpy(setting.config_.st_name, ele->Attribute("model_file"));
	setting.config_.st_id = stoi(ele->Attribute("id"));
	// T29
	setting.id = stol(ele->Attribute("id"));
	setting.file = ele->Attribute("model_file");

	// log_id
	setting.config_.log_id = setting.id *10 + 0;
	// log_name
	strcpy(setting.config_.log_name, ele->Attribute("log_name"));
	// iv_id
	setting.config_.iv_id = setting.id *10 + 1;
	// iv_name
	strcpy(setting.config_.iv_name ,ele->Attribute("iv_name"));
	// ev_id
	setting.config_.ev_id = setting.id*10 + 2;
	// ev_name
	const char * name = ele->Attribute("ev_name");
	strcpy(setting.config_.ev_name, ele->Attribute("ev_name"));

	int counter = 0;
	TiXmlElement* symbol_ele = ele->FirstChildElement();		
	while (symbol_ele != 0)	{		
		symbol_t &tmp = setting.config_.symbols[counter];

		symbol_ele->QueryIntAttribute("max_pos", &tmp.max_pos);
		strcpy(tmp.name, symbol_ele->Attribute("name"));
		string exc_str = symbol_ele->Attribute("exchange");
		tmp.exchange = static_cast<exchange_names>(exc_str[0]);
		symbol_ele = symbol_ele->NextSiblingElement();
		counter++;
	}	//end while (symbol_ele != 0)
	setting.config_.symbols_cnt = counter;
	   
	return setting;
}

void UniConsumer::CreateStrategies()
{
	int i = 0;
	for (auto &setting : this->strategy_settings_){
		stra_table_[i].init(setting);
		// mapping table
		straid_straidx_map_table_.insert(std::make_pair<int32_t, int32_t>(setting.id, i));
		// only support one contract for one strategy
		cont_straidx_map_table_.emplace(setting.config_.symbols[0].name, i);
		i++;
	}
}

void UniConsumer::start()
{
	struct vrt_value  *vvalue;
	while (running_ &&
		   (rc = vrt_consumer_next(c->c, &vvalue)) != VRT_QUEUE_EOF) {
		if (rc == 0) {
			struct vrt_hybrid_value *ivalue = cork_container_of(vvalue, struct vrt_hybrid_value, parent);
			switch (ivalue){
				case BESTANDDEEP:
					ProcBestAndDeep(ivalue->index);
					break;
				case ORDERSTATICS:
					ProcOrderStatistic(ivalue->index);
					break;
				case PENDING_SIGNAL:
					ProcPendingSig(ivalue->index);
					break;
				case TUNN_RPT:
					ProcTunnRpt(ivalue->index);
					break;
				default:
					// TODO: log
					break;
			}
		}
	} // end while (running_ &&
}
void UniConsumer::ProcBestAndDeep(int32_t index)
{
	MDBestAndDeep_MY* md = md_producer->GetBestAnddeep(index);
	auto range = myumm.equal_range(md->Contract);
	for_each (
		range.first,
		range.second,
		[=](std::unordered_multimap<std::string, int32_t>::value_type& x){
			stra_table[x.second].;
			// TODO: here
		}
	);
}

void UniConsumer::FeedBestAndDeep()
{
	const MDBestAndDeep_MY* md
}

void UniConsumer::ProcOrderStatistic(int32_t index)
{
	const MDOrderStatistic_MY* md = GetOrderStatistic(index);
}
void UniConsumer::ProcPendingSig(int32_t index)
{
}

void UniConsumer::ProcTunnRpt(int32_t)
{
}
