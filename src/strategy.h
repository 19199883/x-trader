#pragma once

#include <list>
#include <string>
#include <dlfcn.h>
#include <memory>
#include "signal_entity.h"
#include "model_setting.h"
#include "model_config.h"
#include "model_adapter.h"
#include "moduleloadlibrarylinux.h"
#include "loadlibraryproxy.h"
#include "position_entity.h"

using namespace std;


#define CLOG_MODULE "Strategy" 

#define STRATEGY_METHOD_INIT "st_init_"
#define STRATEGY_METHOD_FEED_MD_BESTANDDEEP "st_feed_marketinfo_1_"
#define STRATEGY_METHOD_FEED_MD_ORDERSTATISTICS "st_feed_marketinfo_3_"
#define STRATEGY_METHOD_FEED_SIG_RESP "st_feed_sig_resp_"
#define STRATEGY_METHOD_FEED_DESTROY "st_destroy_"
#define STRATEGY_METHOD_FEED_INIT_POSITION "st_feed_init_position_"

struct StrategySetting
{
public:
	// the configuration to tell model
	st_config_t config;
	string account;
	string name;
	int id;
	string file;
};

class Strategy	
{
public:

	typedef void (* Init_ptr)(st_config_t *config, int *ret_code);
	typedef void ( *FeedBestAndDeep_ptr)(MDBestAndDeep_MY* md, int *sig_cnt, signal_t* signals);	
	typedef void ( *FeedOrderStatistic_ptr)(OrderStatistic* md, int *sig_cnt, signal_t* signals);
	typedef void ( *FeedSignalResponse_ptr)(signal_resp_t* rpt, symbol_pos_t *pos, pending_order_t *pending_ord, int *sig_cnt, signal_t* sigs);
	typedef void (*Destroy_ptr)();
	typedef void (*FeedInitPosition_ptr)(strategy_init_pos_t *data, int *sig_cnt,signal_t *sig_out);

public:
	Strategy();
	virtual ~Strategy(void);

	 void init();
	 void feed_init_position(strategy_init_pos_t *data,int *sig_cnt, signal_t *sig_out);
	void FeedMd(MDBestAndDeep_MY* md, int *sig_cnt, signal_t* signals);
	void FeedMd(OrderStatistic* md, int *sig_cnt, signal_t* signals);
	void feed_sig_response(signal_resp_t* rpt, symbol_pos_t *pos, pending_order_t *pending_ord, int *sig_cnt, signal_t* sigs);

	void finalize(void);

private:
	string generate_log_name(char * log_path);

	Init_ptr pfn_init_;
	FeedBestAndDeep_ptr pfn_feedbestanddeep_;
	FeedOrderStatistic_ptr pfn_feedorderstatistic_;
	FeedSignalResponse_ptr pfn_feedsignalresponse_;
	Destroy_ptr pfn_destroy_;
	FeedInitPosition_ptr pfn_feedinitposition_;

	CLoadLibraryProxy *pproxy_;
	Setting setting_;
};

