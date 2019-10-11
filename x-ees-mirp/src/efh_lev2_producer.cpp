#include <functional>   // std::bind
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "efh_lev2_producer.h"
#include "quote_cmn_utility.h"

using namespace std::placeholders;
using namespace std;

EfhLev2Producer::EfhLev2Producer(struct vrt_queue  *queue)
:module_name_("EfhLev2Producer")
{
	ended_ = false;
	clog_warning("[%s] FULL_DEPTH_MD_BUFFER_SIZE: %d;", module_name_, FULL_DEPTH_MD_BUFFER_SIZE);

	ParseConfig();
	
	// init dominant contracts
	memset(dominant_contracts_, 0, sizeof(dominant_contracts_));
	dominant_contract_count_ = LoadDominantContracts(config_.contracts_file, dominant_contracts_);

	// disruptor
	this->producer_ = vrt_producer_new("efh_lev2_producer", 1, queue);
	clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	if(strcmp(config_.yield, "threaded") == 0){
		producer_ ->yield = vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		producer_ ->yield = vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		producer_ ->yield = vrt_yield_strategy_hybrid();
	}

	int err = InitMDApi();
	if(!err)
	{
		clog_warning("[%s] efh lev2 init failed.", module_name_);
	}
	else
	{
		clog_warning("[%s] efh lev2 init is successful.", module_name_);
	}
}

void EfhLev2Producer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    

	// yield strategy
    TiXmlElement *disruptor_node = RootElement->FirstChildElement("Disruptor");
	if (disruptor_node != NULL){
		strcpy(config_.yield, disruptor_node->Attribute("yield"));
	} else { clog_error("[%s] x-shmd.config error: Disruptor node missing.", module_name_); }

	// contracts file
    TiXmlElement *contracts_file_node = RootElement->FirstChildElement("Subscription");
	if (contracts_file_node != NULL){
		strcpy(config_.contracts_file, contracts_file_node->Attribute("contracts"));
	} else { clog_error("[%s] x-shmd.config error: Subscription node missing.", module_name_); }

    TiXmlElement *efhLev2  = RootElement->FirstChildElement("EfhLev2");
	strcpy(m_conf_info.m_remote_ip, efhLev2->Attribute("remote_ip"));
	int remote_port = 0;
	efhLev2->QueryIntAttribute("remote_port", &remote_port);
	this->m_conf_info.m_remote_port = remote_port;
	strcpy(m_conf_info.m_local_ip, efhLev2->Attribute("local_ip"));
	int local_port = 0;
	 efhLev2->QueryIntAttribute("local_port", &local_port);
	this->m_conf_info.m_local_port = local_port;
}

EfhLev2Producer::~EfhLev2Producer()
{
}

int EfhLev2Producer::InitMDApi()
{
    return m_efh3.init(m_conf_info, this);
}

void EfhLev2Producer::on_receive_quote(efh3_lev2* data)
{
	if(!IsDominant(data->m_symbol)) return;

	// TODO: commented for debug
	char buffer[2048];
	clog_info("[%s] rev efh3_lev2:%s", 
				module_name_,
				EfhLev2Producer::Format(*data,buffer));

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	vrt_producer_claim(producer_, &vvalue);
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = Push(*data);
	ivalue->data = EFH_LEV2;
	vrt_producer_publish(producer_);
}

void EfhLev2Producer::End()
{
	if(!ended_){
		ended_ = true;

		m_efh3.close();

		vrt_producer_eof(producer_);
		clog_warning("[%s] End exit", module_name_);
	}
	fflush (Log::fp);
}

int32_t EfhLev2Producer::Push(const efh3_lev2& md){
	static int32_t shfemarketdata_cursor = FULL_DEPTH_MD_BUFFER_SIZE - 1;
	shfemarketdata_cursor++;
	if (shfemarketdata_cursor%FULL_DEPTH_MD_BUFFER_SIZE == 0){
		shfemarketdata_cursor = 0;
	}
	shfemarketdata_buffer_[shfemarketdata_cursor] = md;

	return shfemarketdata_cursor;
}

efh3_lev2* EfhLev2Producer::GetData(int32_t index)
{
	return &shfemarketdata_buffer_[index];
}

bool EfhLev2Producer::IsDominant(const char *contract)
{
#ifdef PERSISTENCE_ENABLED 
	// 持久化行情时，需要记录所有合约
	//clog_warning("[%s] return TRUE in IsDominant.",module_name_);
	return true;
#else
	return IsDominantImp((char*)contract, dominant_contracts_, dominant_contract_count_);
#endif
}


