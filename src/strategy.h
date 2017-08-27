#pragma once

#include <unordered_map>
#include <list>
#include <string>
#include <dlfcn.h>
#include <memory>
#include "signal.h"
#include "moduleloadlibrarylinux.h"
#include "loadlibraryproxy.h"
#include "quote_datatype_dce_level2.h"
#include "tunn_rpt_producer.h"
#include "strategy_log.h"

using namespace std;

#define STRATEGY_METHOD_INIT "st_init_"
#define STRATEGY_METHOD_FEED_MD_BESTANDDEEP "st_feed_marketinfo_1_"
#define STRATEGY_METHOD_FEED_MD_ORDERSTATISTICS "st_feed_marketinfo_3_"
#define STRATEGY_METHOD_FEED_SIG_RESP "st_feed_sig_resp_"
#define STRATEGY_METHOD_FEED_DESTROY "st_destroy_"
#define STRATEGY_METHOD_FEED_INIT_POSITION "st_feed_init_position_"
#define STRATEGY_METHOD_SET_LOG_FN "SetLogFn_"

#define SIGANDRPT_TABLE_SIZE 2048

struct StrategyPosition
{
	// long position
	int32_t cur_long;
	// short position
	int32_t cur_short;
	int32_t frozen_close_long;
	int32_t frozen_close_short;
	int32_t frozen_open_long;
	int32_t frozen_open_short;
};

struct StrategySetting
{
public:
	// the configuration to tell model
	st_config_t config;
	string file;
};

class Strategy	
{
public:
	typedef void ( *Output_ptr) (int log_type, int strategy_id, Log1 &content);

	typedef void (* Init_ptr)(st_config_t *config, int *ret_code);
	typedef void ( *FeedBestAndDeep_ptr)(MDBestAndDeep_MY* md, int *sig_cnt, signal_t* signals);	
	typedef void ( *FeedOrderStatistic_ptr)(MDOrderStatistic_MY* md, int *sig_cnt, signal_t* signals);
	typedef void ( *FeedSignalResponse_ptr)(signal_resp_t* rpt, symbol_pos_t *pos, pending_order_t *pending_ord, int *sig_cnt, signal_t* sigs);
	typedef void (*Destroy_ptr)();
	typedef void (*FeedInitPosition_ptr)(strategy_init_pos_t *data, int *sig_cnt,signal_t *sig_out);
	typedef void ( *SetLogFn_ptr )( int strategy_id, Output_ptr fn );


public:
	Strategy();
	virtual ~Strategy(void);

	// things relating to strategy interface
	void Init(StrategySetting &setting, CLoadLibraryProxy *pproxy);
	void FeedInitPosition();
	void FeedMd(MDBestAndDeep_MY* md, int *sig_cnt, signal_t* signals);
	void FeedMd(MDOrderStatistic_MY* md, int *sig_cnt, signal_t* signals);
	void feed_sig_response(signal_resp_t* rpt, symbol_pos_t *pos, pending_order_t *pending_ord, int *sig_cnt, signal_t* sigs);

	// things relating to x-trader internal logic
	void finalize(void);
	int32_t GetId();
	const char* GetContract();
	exchange_names GetExchange();
	int32_t GetMaxPosition();
	const char* GetSoFile();
	long GetLocalOrderID(int32_t sig_id);
	bool Deferred(unsigned short sig_openclose, unsigned short int sig_act, int32_t vol, int32_t &updated_vol);
	void PrepareForExecutingSig(long localorderid, signal_t &sig, int32_t actual_vol);
	void FeedTunnRpt(TunnRpt &rpt, int *sig_cnt, signal_t* sigs);
	bool HasFrozenPosition();

private:
	string generate_log_name(char * log_path);

	// things relating to strategy interface
	Init_ptr pfn_init_;
	FeedBestAndDeep_ptr pfn_feedbestanddeep_;
	FeedOrderStatistic_ptr pfn_feedorderstatistic_;
	FeedSignalResponse_ptr pfn_feedsignalresponse_;
	Destroy_ptr pfn_destroy_;
	FeedInitPosition_ptr pfn_feedinitposition_;
	SetLogFn_ptr pfn_setlogfn_;

	std::array<signal_t, SIGANDRPT_TABLE_SIZE> sig_table_;
	std::array<signal_resp_t, SIGANDRPT_TABLE_SIZE> sigrpt_table_;

	// key: signal id; value: signal or report index in sig_table_ or sigrpt_table_
	// TODO: replace map with array in the future. get original id value according to logic
	// 如果信号ID从0开始递增，可以：array[signal id]=LocalOrderID
	unordered_map<int32_t, int32_t> sigid_sigandrptidx_map_table_;

	// key: LocalOrderID; value: signal or report index in sig_table_ or sigrpt_table_
	// TODO: replace map with array in the future. get original id value according to logic
	// 根据规则，将LOcalOrderID拆分，得到核心的从0开始递增ID（core_LocalOrderID）值，
	// 则可以：array[核心ID]= 信号或信号报告索引
	std::unordered_map<long, int32_t> localorderid_sigandrptidx_map_table_;

	// TODO: replace map with array in the future
	// 如果信号ID从0开始递增，可以：array[signal id]=LocalOrderID
	std::unordered_map<int32_t,long > sigid_localorderid_map_table_;
	position_t pos_cache_;
	pending_order_t pending_order_cache_;

	// be used to check whether the stategy is valid
	bool valid_;

	CLoadLibraryProxy *pproxy_;
	StrategySetting setting_;
	const char *module_name_;  
	StrategyPosition position_;
	void LoadPosition();
	void SavePosition();
	/*
	 * 
	 */
	void UpdateSigrptByTunnrpt(signal_resp_t& sigrpt, const TunnRpt& tunnrpt);
	void UpdatePosition(const TunnRpt& rpt, unsigned short sig_openclose, unsigned short int sig_act);
	void FillPositionRpt(const TunnRpt& rpt, position_t& pos);
	const char * GetSymbol();
	bool Freeze(unsigned short sig_openclose, unsigned short int sig_act, int32_t updated_vol);
};

