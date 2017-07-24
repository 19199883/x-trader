#include <sstream>
#include <stdio.h>
#include "strategy.h"

using namespace std;

Strategy::Strategy()
{
}

Strategy::~Strategy(void)
{
	if (this->pfn_destroy_ != NULL){
		pfn_destroy_ ();
		clog_info("[%s] strategy(id:%d) destroyed", CLOG_MODULE, this->setting_.id);
	}

	if (pproxy_ != NULL){
		pproxy_->deleteObject(this->setting_.file);
		pproxy_->DeleteLoadLibraryProxy();
		pproxy_ = NULL;
	}

	clog_info("[%s] strategy(id:%d) released", CLOG_MODULE, this->setting_.id);
}

string Strategy::generate_log_name(char* log_path)
 {
	string log_full_path = "";

	// parse model name
	string model_name = "";
	unsigned found = this->setting.file.find_last_of("/");
	if(found==string::npos){ model_name = this->setting.file; }
	else{ model_name = this->setting.file.substr(found+1); }

	time_t rawtime;
	struct tm * timeinfo;
	char buffer [80];
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	strftime (buffer,80,"%Y%m%d",timeinfo);

	log_full_path = log_path;
	log_full_path += "/";
	log_full_path += model_name;
	log_full_path += "_";

	log_full_path += buffer;
	log_full_path += ".txt";

	return log_full_path;
}

void Strategy::init(StrategySetting setting)
{
	this->setting_ = setting;
	this->pproxy_ = CLoadLibraryProxy::CreateLoadLibraryProxy();

	pfn_init_ = (Init_ptr)pproxy_->findObject(this->setting_.file, STRATEGY_METHOD_INIT);
	if (!pfn_init_){
		clog_info("[%s] findObject failed, file:%s; method:%s; errno:%d", CLOG_MODULE, this->setting_.file, STRATEGY_METHOD_INIT, errno);
	}

	pfn_feedbestanddeep = (FeedBestAndDeep_ptr)pproxy_->findObject(
					this->setting_.file, STRATEGY_METHOD_FEED_MD_BESTANDDEEP);
	if (!pfn_feedbestanddeep){
		clog_info("[%s] findObject failed, file:%s; method:%s; errno:%d", CLOG_MODULE, this->setting_.file, STRATEGY_METHOD_FEED_MD_BESTANDDEEP, errno);
	}

	pfn_feedorderstatistic_ = (FeedOrderStatistic_ptr)pproxy_->findObject(
					this->setting_.file, STRATEGY_METHOD_FEED_MD_ORDERSTATISTICS);
	if (!pfn_feedorderstatistic_){
		clog_info("[%s] findObject failed, file:%s; method:%s; errno:%d", CLOG_MODULE, this->setting_.file, STRATEGY_METHOD_FEED_MD_ORDERSTATISTICS, errno);
	}

	pfn_feedinitposition_ = (FeedInitPosition_ptr)_pproxy_->findObject(
				this->setting_.file, STRATEGY_METHOD_FEED_INIT_POSITION);
	if (!pfn_feedinitposition_ ){
		clog_info("[%s] findObject failed, file:%s; method:%s; errno:%d", CLOG_MODULE, this->setting_.file, STRATEGY_METHOD_FEED_INIT_POSITION, errno);
	}

	pfn_feedsignalresponse_ = (FeedSignalResponse_ptr)_pproxy_->findObject(
				this->setting_.file, STRATEGY_METHOD_FEED_SIG_RESP);
	if (!pfn_feedsignalresponse_){
		clog_info("[%s] findObject failed, file:%s; method:%s; errno:%d", CLOG_MODULE, this->setting_.file, STRATEGY_METHOD_FEED_SIG_RESP, errno);
	}

	pfn_destroy_ = (Destroy_ptr)pproxy_->findObject(
				this->setting_.file, STRATEGY_METHOD_FEED_DESTROY );
	if (!pfn_destroy_){
		clog_info("[%s] findObject failed, file:%s; method:%s; errno:%d", CLOG_MODULE, this->setting_.file, STRATEGY_METHOD_FEED_DESTROY, errno);
	}

	string model_log = generate_log_name(setting_.config_.log_name);
	strcpy(setting_.config_.log_name, model_log.c_str());
	setting_.config_.log_id = setting_.config_.st_id;

	int err = 0;
	this->pfn_init_(&this->setting_.config_, &err);
}

void Strategy::feed_init_position(strategy_init_pos_t *data,int *sig_cnt, signal_t *sigs)
{
	*sig_cnt = 0;
	this->pfn_feedinitposition_(data,sig_cnt, sigs);
	for (int i = 0; i < *sig_cnt; i++ ){
		sigs[i].st_id = this->setting_.id;
	}
}

void Strategy::FeedMd(MDBestAndDeep_MY* md, int *sig_cnt, signal_t* sigs)
{
	*sig_cnt = 0;
	this->pfn_feedbestanddeep_(md, sig_cnt, sigs);
	for (int i = 0; i < sig_cnt; i++ ){
		sigs[i].st_id = this->setting_.id;
	}
}

void Strategy::FeedMd(OrderStatistic* md, int *sig_cnt, signal_t* sigs)
{
	*sig_cnt = 0;
	this->pfn_feedorderstatistic_(md, sig_cnt, sigs);
	for (int i = 0; i < sig_cnt; i++ ){
		sigs[i].st_id = this->setting_.id;
	}
}

void Strategy ::feed_sig_response(signal_resp_t* rpt, symbol_pos_t *pos, pending_order_t *pending_ord, int *sig_cnt, signal_t* sigs)
{
	*sig_cnt = 0;
	this->pfn_feedsignalresponse_(rpt, pos, pending_ord, sig_cnt, sigs);
	for (int i = 0; i < sig_cnt; i++ ){
		sigs[i].st_id = this->setting_.id;
	}
}


