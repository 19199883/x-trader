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

#define STRATEGY_METHOD_INIT "st_init_"
#define STRATEGY_METHOD_FEED_MD_BESTANDDEEP "st_feed_marketinfo_1_"
#define STRATEGY_METHOD_FEED_MD_ORDERSTATISTICS "st_feed_marketinfo_3_"
#define STRATEGY_METHOD_FEED_SIG_RESP "st_feed_sig_resp_"
#define STRATEGY_METHOD_FEED_DESTROY "st_destroy_"
#define STRATEGY_METHOD_FEED_INIT_POSITION "st_feed_init_position_"

class Strategy	
{
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
	void FeedMd(MDBestAndDeep_MY &md, signal_t* signals, int &sig_cnt);
	void FeedMd(MDOrderStatistic_MY &md, signal_t* signals,int &sig_cnt);
	void feed_sig_response(signal_resp_t* rpt,symbol_pos_t *pos,
			pending_order_t *pending_ord,signal_t* sigs,int &sig_cnt);

	void finalize(void);

private:
	/*
	* feed configuration to model.
	*/
	 void init_imp(st_config_t *config, int *ret_code);
	 void feed_init_position_imp(strategy_init_pos_t *data, int *sig_cnt, signal_t *sig_out);
	void feed_spif_quote_imp(SPIFQuoteT &quote_ptr, signal_t* signals,int &signals_size);
	void feed_cf_quote_imp(CFQuoteT &quote_ptr,signal_t* signals,int &signals_size);
	void feed_sig_response_imp(signal_resp_t* rpt,symbol_pos_t *pos, pending_order_t *pending_ord,signal_t* sigs,int &sigs_len);

	void trace(string title,signal_t* signals,const int &signals_size);
	void trace(pending_order_t *ords);
	void trace(string title,symbol_pos_t *pos);
	void trace(const SPIFQuoteT &data);
	void trace(signal_resp_t* rpt);
	void trace(strategy_init_pos_t *data);

	string generate_log_name(char * log_path);

	Init_ptr pfn_init_;
	FeedBestAndDeep_ptr pfn_feedbestanddeep_;
	FeedOrderStatistic_ptr pfn_feedorderstatistic_;
	FeedSignalResponse_ptr pfn_feedsignalresponse_;
	Destroy_ptr pfn_destroy_;
	FeedInitPosition_ptr pfn_feedinitposition_;

	CLoadLibraryProxy *pproxy_;
	model_setting setting_;
};

