#include <functional>   // std::bind
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "lev2_producer.h"
#include "quote_cmn_utility.h"

using namespace std::placeholders;
using namespace std;

Lev2Producer::Lev2Producer(struct vrt_queue  *queue)
:module_name_("Lev2Producer")
{
	ended_ = false;
	clog_warning("[%s] FULL_DEPTH_MD_BUFFER_SIZE: %d;", module_name_, FULL_DEPTH_MD_BUFFER_SIZE);

	ParseConfig();
	
	// init dominant contracts
	memset(dominant_contracts_, 0, sizeof(dominant_contracts_));
	dominant_contract_count_ = LoadDominantContracts(config_.contracts_file, dominant_contracts_);

	// disruptor
	this->producer_ = vrt_producer_new("lev2_producer", 1, queue);
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
		clog_warning("[%s] lev2 init failed.", module_name_);
	}
	else
	{
		clog_warning("[%s] lev2 init is successful.", module_name_);
	}
}

void Lev2Producer::ParseConfig()
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

    TiXmlElement *lev2  = RootElement->FirstChildElement("Lev2");
	strcpy(conf_info_.m_remote_ip, lev2->Attribute("remote_ip"));
	int remote_port = 0;
	lev2->QueryIntAttribute("remote_port", &remote_port);
	this->conf_info_.m_remote_port = remote_port;
	strcpy(conf_info_.m_local_ip, lev2->Attribute("local_ip"));
	int local_port = 0;
	 lev2->QueryIntAttribute("local_port", &local_port);
	this->conf_info_.m_local_port = local_port;
}

Lev2Producer::~Lev2Producer()
{
}

int Lev2Producer::InitMDApi()
{
    return m_message_engine_.init(conf_info_, this);
}

void Lev2Producer::on_receive_quote(CDepthMarketDataField* data)
{
	if(!IsDominant(data->InstrumentID)) return;

	char buffer[2048];
	clog_info("[%s] rev lev2:%s", 
				module_name_,
				ShfeLev2Formater::Format(*data,buffer));

	struct vrt_value  *vvalue;
	struct vrt_hybrid_value  *ivalue;
	vrt_producer_claim(producer_, &vvalue);
	ivalue = cork_container_of (vvalue, struct vrt_hybrid_value, parent);
	ivalue->index = Push(*data);
	ivalue->data = LEV2_MD;
	vrt_producer_publish(producer_);
}

void Lev2Producer::End()
{
	if(!ended_){
		ended_ = true;

		m_message_engine_.close();

		vrt_producer_eof(producer_);
		clog_warning("[%s] End exit", module_name_);
	}
	fflush (Log::fp);
}

int32_t Lev2Producer::Push(const CDepthMarketDataField& md){
	static int32_t shfemarketdata_cursor = FULL_DEPTH_MD_BUFFER_SIZE - 1;
	shfemarketdata_cursor++;
	if (shfemarketdata_cursor%FULL_DEPTH_MD_BUFFER_SIZE == 0){
		shfemarketdata_cursor = 0;
	}
	shfemarketdata_buffer_[shfemarketdata_cursor] = md;

	return shfemarketdata_cursor;
}

CDepthMarketDataField* Lev2Producer::GetData(int32_t index)
{
	return &shfemarketdata_buffer_[index];
}

bool Lev2Producer::IsDominant(const char *contract)
{
#ifdef PERSISTENCE_ENABLED 
	// 持久化行情时，需要记录所有合约
	//clog_warning("[%s] return TRUE in IsDominant.",module_name_);
	return true;
#else
	return IsDominantImp((char*)contract, dominant_contracts_, dominant_contract_count_);
#endif
}


