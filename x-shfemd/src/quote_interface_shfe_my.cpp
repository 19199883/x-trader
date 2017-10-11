#include "quote_interface_shfe_my.h"

#include <string>
#include <thread>         // std::thread
#include "my_cmn_util_funcs.h"
#include "quote_cmn_utility.h"
#include "quote_shfe_my.h"
#include "vrt_value_obj.h"
#include "md_producer.h"

using namespace std;
using namespace my_cmn;

/* Note that the parameter for queue size is a power of 2. */
#define  QUEUE_SIZE  4096

// TODO:因消费者需要合并一档与全挡数据，所以需要考虑消费
// 者如何引用缓存在一档生产者的数据，一档数据与全挡数据合并后，需清空
// 一档行情缓存列表，合约映射 index（一档生产者缓存中的索引）


// TODO: 该类为消费者
MYQuoteData::MYQuoteData(const SubscribeContracts *subscribe,const string &provider_config)
	: module_name_("uni_consumer"),running_(true),seq_no_(0),server_(0)
{
	// clog init
	clog_set_minimum_level(CLOG_LEVEL_WARNING);
	clog_fp_ = fopen("./x-shfemd.log","w+");
	struct clog_handler *clog_handler = clog_stream_handler_new_fp(fp, true, "%l %m");
	clog_handler_push_process(clog_handler);

	ParseConfig();
	clog_warning("[%s] yield:%s", module_name_, config_.yield); 

	// disruptor init
	rip_check(queue_ = vrt_queue_new("x-shfemd queue",vrt_hybrid_value_type(),QUEUE_SIZE));
	fulldepth_md_producer_ = new FullDepthMDProducer(queue_);
	l1_md_producer_ = new L1MDProducer(queue_);
	consumer_ = vrt_consumer_new(module_name_, queue_);
	if(strcmp(config_.yield, "threaded") == 0){
		consumer_ ->yield = vrt_yield_strategy_threaded();
	}else if(strcmp(config_.yield, "spin") == 0){
		consumer_ ->yield = vrt_yield_strategy_spin_wait();
	}else if(strcmp(config_.yield, "hybrid") == 0){
		consumer_ ->yield = vrt_yield_strategy_hybrid();
	}

	for (int i=0; i<10; i++) repairers[i].server_ = i;

	consumer_thread_ = new thread(MYQuoteData::Start);
	consumer_thread_->detach();
}

MYQuoteData::~MYQuoteData()
{
	Stop();

	vrt_queue_free(queue);
	delete fulldepth_md_producer_;
	delete l1_md_producer_;

	fflush (clog_fp_);
	clog_handler_free(clog_handler);
}

void MYQuoteData::SetQuoteDataHandler(std::function<void(const SHFEQuote*)> quote_handler)
{
	// do not support
}

void MYQuoteData::SetQuoteDataHandler(std::function<void(const CDepthMarketDataField*)> quote_handler)
{
	// do not support
}

void MYQuoteData::SetQuoteDataHandler(std::function<void(const MYShfeMarketData*)> quote_handler)
{
	fulldepthmd_handler_ = quote_handler;
}

//
// TODO: move to consumer process method
void MYQuoteData::Send(MYShfeMarketData *data)
{
    if (NULL != my_shfe_md_handler_) { my_shfe_md_handler_(data); }
}

// TODO:改成从disruptor中接收并处理数据
// 在repairer等地方对MDPack数据，采用直接引用生产者缓存的数据，而不能再赋值一份
void Proc()
{
    while (!ended_){
        MDPack *p = (MDPack *)recv_buf;
    } // while (!ended_)
}

void UniConsumer::ParseConfig()
{
	std::string config_file = "x-trader.config";
	TiXmlDocument doc = TiXmlDocument(config_file.c_str());
    doc.LoadFile();
    TiXmlElement *root = doc.RootElement();    

	// yield strategy
    TiXmlElement *comp_node = root->FirstChildElement("Disruptor");
	if (comp_node != NULL){
		strcpy(config_.yield, comp_node->Attribute("yield"));
		clog_warning("[%s] yield:%s", module_name_, config_.yield); 
	} else { clog_error("[%s] x-trader.config error: Disruptor node missing.", module_name_); }
}

void MYQuoteData::Start()
{
	clog_debug("[%s] thread id:%ld", module_name_,std::this_thread::get_id() );

	running_  = true;
	int rc = 0;
	struct vrt_value *vvalue;
	while (running_ &&
		   (rc = vrt_consumer_next(consumer_, &vvalue)) != VRT_QUEUE_EOF) {
		if (rc == 0) {
			struct vrt_hybrid_value *ivalue = cork_container_of(vvalue, struct vrt_hybrid_value,
						parent);
			switch (ivalue->data){
				case L1_MD:
					ProcL1MdData(ivalue->index);
					break;
				case FULL_DEPTH_MD:
					ProcFullDepthData(ivalue->index);
					break;
				default:
					clog_error("[%s] [start] unexpected index: %d", module_name_, ivalue->index);
					break;
			}
		}
	} // end while (running_ &&

	if (rc == VRT_QUEUE_EOF) {
		clog_warning("[%s] [start] rev EOF.", module_name_);
	}
	clog_warning("[%s] start exit.", module_name_);
}

void UniConsumer::Stop()
{
	if(running_){
		running_ = false;
		fulldepth_md_producer_->End();
		l1_md_producer_->End();
		clog_warning("[%s] End exit", module_name_);
	}
}

void UniConsumer::ProcL1MdData(int32_t index)
{
	CDepthMarketDataField* md = md_producer_->GetData(index);
	// TODO:全息行情需要一档行情时，从缓存最新位置向前查找13个位置（假设有13个主力合约），找到即停
}

void UniConsumer::ProcFullDepthData(int32_t index)
{
	MDPackEx* md = fulldepth_md_producer_->GetData(index);
	// TODO:
	int new_svr = p->seqno % 10;
	if (new_svr != server_) { MY_LOG_INFO("server from %d to %d", server_, new_svr); }

	repairers[new_svr].rev(index);
	bool empty = true;
	// TODO:here 一个行情只需一份，不用多分拷贝
	// TODO:此段代码要继续考虑如何写
	char cur_contract[10];
	strcpy(cur_contract,"");
	char new_contract[10];
	strcpy(new_contract,"");
	MDPackEx* data = repairers[new_svr].next(empty);
	while (!empty) { 
		if(strcmp(cur_contract) == ""){
			strcpy(cur_contract,data->instrument);
		}
		strcpy(new_contract,data->instrument);
		if(strcmp(cur_contract,new_contract) != 0){
			// TODO: 合并一档行情
			Send(target_md_);
		}else{
			// TODO:生成最终的行情数据
			proc_udp_data(target_md_,data);
		}

		strcpy(cur_contract,data->Instrument);
		data = repairers[new_svr].next(empty);
		if(empty){
			// TODO: 合并一档行情
			Send(target_md_);
		}
	}

	server_ = new_svr;
}


