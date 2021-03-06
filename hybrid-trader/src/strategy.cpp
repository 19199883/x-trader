#include <chrono>
#include <thread> 
#include <stdio.h>
#include <ratio>
#include <ctime>
#include <sstream>
#include <stdio.h>
#include <fstream>      // std::ifstream, std::ofstream
#include <stdio.h>
#include "strategy.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>  
#include <sys/stat.h> 

using namespace std;
using namespace std::chrono;

Strategy::Strategy()
: module_name_("Strategy")
{
	valid_ = false;
	cursor_ = 0;

	pfn_init_ = NULL;
	pfn_feedshfemarketdata_ = NULL;
	pfn_feedsignalresponse_ = NULL;
	pfn_destroy_ = NULL;
	pfn_feedinitposition_ = NULL;
	pfn_destroy_ = NULL;

	pproxy_ = NULL;

	log_ = vector<strat_out_log>(MAX_LINES_FOR_LOG);
	log_cursor_ = 0;
	pfDayLogFile_ = NULL;
	cur_ntick_ = -1;
	max_log_lines_ = 0;
	id_ = 0;

	memset(localorderid_sigandrptidx_map_table_, 0, sizeof(localorderid_sigandrptidx_map_table_));
	memset(sigid_localorderid_map_table_, 0, sizeof(sigid_localorderid_map_table_));
	memset(sigid_sigidx_map_table_, 0, sizeof(sigid_sigidx_map_table_));
}

void Strategy::End(void)
{
	fclose(pfDayLogFile_);
	clog_warning("[%s] strategy(id:%d) close log file", module_name_, this->setting_.config.st_id);

	if (this->pfn_destroy_ != NULL){
		//pfn_destroy_ ();
		clog_warning("[%s] strategy(id:%d) destroyed", module_name_, this->setting_.config.st_id);
	}
	fflush (Log::fp);
}

Strategy::~Strategy(void)
{
	if (pproxy_ != NULL){
		pproxy_->deleteObject(this->setting_.file);
		pproxy_ = NULL;
	}
}

string Strategy::generate_log_name(char* log_path)
 {
	string log_full_path = "";

	string model_name = "";
	unsigned found = this->setting_.file.find_last_of("/");
	if(found==string::npos){ model_name = this->setting_.file; }
	else{ model_name = this->setting_.file.substr(found+1); }

	time_t rawtime;
	struct tm * timeinfo;
	char buffer [80];
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	strftime (buffer,80,"%Y%m%d",timeinfo);

	log_full_path = log_path;
	log_full_path += "/";
	log_full_path += this->GetSoFile();
	log_full_path += "/";
	log_full_path += model_name;
	log_full_path += "_";

	log_full_path += buffer;
	log_full_path += ".txt";

	return log_full_path;
}

void Strategy::Init(StrategySetting &setting, CLoadLibraryProxy *pproxy)
{
	valid_ = true;

	this->setting_ = setting;
	this->pproxy_ = pproxy;
	id_ = this->setting_.config.st_id;

	max_log_lines_ = MAX_LINES_FOR_LOG - MAX_STRATEGY_COUNT * 100 + GetId() * 100;
	clog_warning("[%s] strategy:%d; max_log_lines_ :%d", module_name_, this->GetId(), max_log_lines_ ); 

	pfn_init_ = (Init_ptr)pproxy_->findObject(this->setting_.file, STRATEGY_METHOD_INIT);
	if (!pfn_init_){
		clog_warning("[%s] findObject failed, file:%s; method:%s; errno:%d", 
					module_name_, this->setting_.file.c_str(), STRATEGY_METHOD_INIT, errno);
	}

	pfn_feedbestanddeep_= (FeedBestAndDeep_ptr)pproxy_->findObject(
					this->setting_.file, STRATEGY_METHOD_FEED_MD_BESTANDDEEP);
	if (!pfn_feedbestanddeep_){
		clog_warning("[%s] findObject failed, file:%s; method:%s; errno:%d", 
			module_name_, this->setting_.file.c_str(), STRATEGY_METHOD_FEED_MD_BESTANDDEEP, errno);
	}

	pfn_feedshfemarketdata_ = (FeedShfeMarketData_ptr )pproxy_->findObject(
					this->setting_.file, STRATEGY_METHOD_FEED_MD_MYSHFE);
	if (!pfn_feedshfemarketdata_ ){
		clog_warning("[%s] findObject failed, file:%s; method:%s; errno:%d", 
			module_name_, this->setting_.file.c_str(), STRATEGY_METHOD_FEED_MD_MYSHFE, errno);
	}

	pfn_feedinitposition_ = (FeedInitPosition_ptr)pproxy_->findObject(
				this->setting_.file, STRATEGY_METHOD_FEED_INIT_POSITION);
	if (!pfn_feedinitposition_ ){
		clog_warning("[%s] findObject failed, file:%s; method:%s; errno:%d", 
			module_name_, this->setting_.file.c_str(), STRATEGY_METHOD_FEED_INIT_POSITION, errno);
	}

	pfn_feedsignalresponse_ = (FeedSignalResponse_ptr)pproxy_->findObject(
				this->setting_.file, STRATEGY_METHOD_FEED_SIG_RESP);
	if (!pfn_feedsignalresponse_){
		clog_warning("[%s] findObject failed, file:%s; method:%s; errno:%d", 
					module_name_, this->setting_.file.c_str(), STRATEGY_METHOD_FEED_SIG_RESP, errno);
	}

	pfn_destroy_ = (Destroy_ptr)pproxy_->findObject(
				this->setting_.file, STRATEGY_METHOD_FEED_DESTROY );
	if (!pfn_destroy_){
		clog_warning("[%s] findObject failed, file:%s; method:%s; errno:%d", 
					module_name_, this->setting_.file.c_str(), STRATEGY_METHOD_FEED_DESTROY, errno);
	}

	string model_log = generate_log_name(setting_.config.log_name);
	strcpy(setting_.config.log_name, model_log.c_str());
	setting_.config.log_id = setting_.config.st_id;

	clog_warning("[%s] open log file:%s", module_name_,setting_.config.log_name);

	for(StrategyPosition &position : positions_){
		memset(&position, 0, sizeof(position));
	}
	
	// TODO: to be tested
	memset(&pos_cache_, 0, sizeof(pos_cache_));
	pos_cache_.symbol_cnt = this->setting_.config.symbols_cnt;
	clog_warning("[%s] strategy:%d; pos_cache_.symbol_cnt:%d", module_name_,
			this->GetId(),pos_cache_.symbol_cnt);
	int count = 0;
	for(int count=0; count<pos_cache_.symbol_cnt; count++ ){
		symbol_t &symbol = this->setting_.config.symbols[count];
		strcpy(pos_cache_.s_pos[count].symbol, symbol.name);
		clog_warning("[%s] strategy:%d; pos_cache_.s_pos[%d]:%s", module_name_,this->GetId(),count,
			pos_cache_.s_pos[count].symbol);
		strcpy(positions_[count].contract, symbol.name);
		clog_warning("[%s] strategy:%d; positions_[%d]:%s", module_name_,this->GetId(),count,
			positions_[count].contract);
	}
	FillPositionRpt(pos_cache_);

	// TODO: deal with 0 to be modified
	for(int i=0; i<this->setting_.config.symbols_cnt; i++){
		string sym_log_name = generate_log_name(setting_.config.symbols[i].symbol_log_name);
		strcpy(setting_.config.symbols[i].symbol_log_name, sym_log_name.c_str());
	}

	pfDayLogFile_ = fopen (setting_.config.log_name, "w");
	int err = 0;
	char logfiles[1500];
	this->GetHistoryLogs(logfiles);
	clog_warning("[%s] strategy:%d; history logs:%s", module_name_,this->GetId(),logfiles);
	this->pfn_init_(&this->setting_.config, &err, log_.data()+log_cursor_,logfiles);
	if((log_.data()+log_cursor_)->exch_time > 0) log_cursor_++;

	this->FeedInitPosition();
}

void Strategy::GetHistoryLogs(char logfiles[1500])
{
	// 创建以so名的文件夹，放该策略日志的
	logfiles[0] = 0;
	string basePath = "./log/";
	basePath += this->GetSoFile();
    DIR *dir;
    struct dirent *ptr;

    if ((dir=opendir(basePath.c_str())) == NULL){
        perror("Open dir error...");
        exit(1);
    }

	int cur_pos = 0;
    while ((ptr=readdir(dir)) != NULL){
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0){//current dir OR parrent dir
            continue;
		} else if(ptr->d_type == 8){ //file
			strcpy(logfiles+cur_pos,ptr->d_name);
			cur_pos += strlen(ptr->d_name);
			logfiles[cur_pos] = ';';
			cur_pos++;
        }
    }
    closedir(dir);

	if(cur_pos > 0) logfiles[cur_pos-1] = 0;
}

void Strategy::FeedInitPosition()
{
	signal_t sigs[10];
	int sig_cnt = 0;
	strategy_init_pos_t init_pos;

	position_t &today_pos = init_pos._cur_pos;
	// TODO: to be tested
	today_pos.symbol_cnt = this->setting_.config.symbols_cnt+1; 
	symbol_pos_t& first = today_pos.s_pos[0];
	strncpy(first.symbol, "#CASH", sizeof(first.symbol));

	int symbol_count = 1;
	// TODO: to be tested 实际长度，而不是最大长度
	for(int count=0; count<pos_cache_.symbol_cnt; count++ ){
		symbol_t &symbol = this->setting_.config.symbols[count];
		symbol_pos_t& second = today_pos.s_pos[symbol_count];
		strncpy(second.symbol,symbol.name, sizeof(second.symbol));
		second.long_volume = this->GetPosition(symbol.name)->cur_long;
		second.short_volume = this->GetPosition(symbol.name)->cur_short;
		second.exchg_code = symbol.exchange; 

		clog_warning("[%s] FeedInitPosition strategy id:%d; contract:%s; "
			"exchange:%d; long:%d; short:%d",
			module_name_, GetId(), second.symbol, second.exchg_code, 
			second.long_volume, second.short_volume);

		symbol_count++;
	}
		this->pfn_feedinitposition_(&init_pos, log_.data()+log_cursor_);
		if((log_.data()+log_cursor_)->exch_time > 0) log_cursor_++;
}

StrategyPosition* Strategy::GetPosition(const char*contract)
{
	// TODO: to be tested
	for(StrategyPosition &position : positions_){
		if(strcmp(contract,position.contract)==0){
			return &position;
		}
	}
	return NULL;
}

void Strategy::FeedMd(MYShfeMarketData* md, int *sig_cnt, signal_t* sigs)
{
	clog_info("[test] proc [%s] [FeedMd] contract:%s, time:%s", module_name_, 
		md->InstrumentID, md->GetQuoteTime().c_str());
	 clog_info("[%s] FeedMd MYShfeMarketData (data_flag=%d) signal: strategy id:%d;  ",
				module_name_, md->data_flag, GetId());				

#ifdef LATENCY_MEASURE
	high_resolution_clock::time_point t0 = high_resolution_clock::now();
#endif
	
	*sig_cnt = 0;
	(log_.data()+log_cursor_)->exch_time = 0;
	this->pfn_feedshfemarketdata_(md, sig_cnt, sigs, log_.data()+log_cursor_);
	if((log_.data()+log_cursor_)->exch_time > 0) log_cursor_++;

	for (int i = 0; i < *sig_cnt; i++ ){

#ifdef LATENCY_MEASURE
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		int latency = (t1.time_since_epoch().count() - t0.time_since_epoch().count()) / 1000;
		clog_warning("[%s] strategy latency:%d us", module_name_, latency); 
#endif

		sigs[i].st_id = this->GetId();

		 clog_info("[%s] FeedMd MYShfeMarketData(data_flag=%d) signal: strategy id:%d; sig_id:%d; "
					 "exchange:%d; symbol:%s; open_volume:%d; buy_price:%f; "
					 "close_volume:%d; sell_price:%f; sig_act:%d; sig_openclose:%d; orig_sig_id:%d",
					module_name_, md->data_flag, sigs[i].st_id, sigs[i].sig_id,
					sigs[i].exchange, sigs[i].symbol, sigs[i].open_volume, sigs[i].buy_price,
					sigs[i].close_volume, sigs[i].sell_price, sigs[i].sig_act, sigs[i].sig_openclose, sigs[i].orig_sig_id); 
	}
}

void Strategy::FeedMd(MDBestAndDeep_MY* md, int *sig_cnt, signal_t* sigs)
{
	clog_info("[test] proc MDBestAndDeep_MY [%s] [FeedMd] contract:%s, time:%s", module_name_, 
		md->Contract, md->GetQuoteTime().c_str());

#ifdef LATENCY_MEASURE
	high_resolution_clock::time_point t0 = high_resolution_clock::now();
#endif
	
	*sig_cnt = 0;
	(log_.data()+log_cursor_)->exch_time = 0;
	this->pfn_feedbestanddeep_(md, sig_cnt, sigs, log_.data()+log_cursor_);
	if((log_.data()+log_cursor_)->exch_time > 0) log_cursor_++;

	for (int i = 0; i < *sig_cnt; i++ ){
#ifdef LATENCY_MEASURE
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		int latency = (t1.time_since_epoch().count() - t0.time_since_epoch().count()) / 1000;
		clog_warning("[%s] strategy latency:%d us", module_name_, latency); 
#endif
		sigs[i].st_id = this->GetId();
		 clog_info("[%s] FeedMd MDBestAndDeep signal: strategy id:%d; sig_id:%d; "
				 "exchange:%d; symbol:%s; open_volume:%d; buy_price:%f; "
				 "close_volume:%d; sell_price:%f; sig_act:%d; sig_openclose:%d; orig_sig_id:%d",
				module_name_, sigs[i].st_id, sigs[i].sig_id,
				sigs[i].exchange, sigs[i].symbol, sigs[i].open_volume, sigs[i].buy_price,
				sigs[i].close_volume, sigs[i].sell_price, sigs[i].sig_act, sigs[i].sig_openclose,
				sigs[i].orig_sig_id); 
	}
}

void Strategy::feed_sig_response(signal_resp_t* rpt, symbol_pos_t *pos, int *sig_cnt, signal_t* sigs)
{
	*sig_cnt = 0;
	(log_.data()+log_cursor_)->exch_time = 0;
	this->pfn_feedsignalresponse_(rpt, pos, sig_cnt, sigs, log_.data()+log_cursor_);
	if((log_.data()+log_cursor_)->exch_time > 0) log_cursor_++;

	for (int i = 0; i < *sig_cnt; i++ ){
		sigs[i].st_id = GetId();
		clog_info("[%s] feed_sig_respons esignal: strategy id:%d;sig_id:%d; exchange:%d; symbol:%s;"
			"open_volume:%d; buy_price:%f; close_volume:%d; sell_price:%f; sig_act:%d; sig_openclose:%d; orig_sig_id:%d",
			module_name_, sigs[i].st_id, sigs[i].sig_id,
			sigs[i].exchange, sigs[i].symbol, sigs[i].open_volume, sigs[i].buy_price,
			sigs[i].close_volume, sigs[i].sell_price, sigs[i].sig_act, sigs[i].sig_openclose, sigs[i].orig_sig_id); 
	}
}


int32_t Strategy::GetId()
{
	return id_;
}

exchange_names Strategy::GetExchange(const char *contract)
{
	// TODO: to be tested
	for(int i=0; i<this->setting_.config.symbols_cnt; i++){
		const symbol_t &symbol = this->setting_.config.symbols[i];
		if(strcmp(symbol.name,contract)==0){
			return symbol.exchange;
		}
	}
	
	return exchange_names::undefined;
}

int32_t Strategy::GetMaxPosition(const char *contract)
{
	for(int i=0; i<this->setting_.config.symbols_cnt; i++){
		const symbol_t &symbol = this->setting_.config.symbols[i];
		if(strcmp(symbol.name,contract)==0){
			return symbol.max_pos;
		}
	}
	return 0;
}

const char* Strategy::GetSoFile()
{
	return this->setting_.file.c_str();
}

long Strategy::GetLocalOrderID(int32_t sig_id)
{
	return sigid_localorderid_map_table_[sig_id];
}

bool Strategy::Freeze(const char *contract,unsigned short sig_openclose,
	unsigned short int sig_act, int32_t updated_vol)
{
	// TODO: to be tested
	StrategyPosition *position = this->GetPosition(contract);
	// 开仓限制要使用多空仓位的差值，锁仓部分不算
	if (sig_openclose==alloc_position_effect_t::open_&& sig_act==signal_act_t::buy){
		position->frozen_open_long += updated_vol;
	} else if (sig_openclose==alloc_position_effect_t::open_&& sig_act==signal_act_t::sell){
		position->frozen_open_short += updated_vol;
	} 

	if (sig_openclose==alloc_position_effect_t::close_&& sig_act==signal_act_t::buy){
		position->frozen_close_short += updated_vol;
	}
	else if (sig_openclose==alloc_position_effect_t::close_&& sig_act==signal_act_t::sell){
		position->frozen_close_long += updated_vol;
	}

	clog_debug("[%s] Freeze: strategy id:%d; current long:%d; current short:%d; \
				frozen_close_long:%d; frozen_close_short:%d; frozen_open_long:%d; \
				frozen_open_short:%d; ",
				module_name_, setting_.config.st_id, position->cur_long, position->cur_short,
				position->frozen_close_long, position->frozen_close_short,
				position->frozen_open_long, position->frozen_open_short);
}

int Strategy::GetVol(const signal_t &sig)
{
	int32_t vol = 0;
	if (sig.sig_openclose == alloc_position_effect_t::open_){
		vol = sig.open_volume;
	} else if (sig.sig_openclose == alloc_position_effect_t::close_){
		vol = sig.close_volume;
	} else{ clog_error("[%s] PlaceOrder: do support sig_openclose value:%d;", module_name_,
				sig.sig_openclose); }

	return vol;
}

int Strategy::GetAvailableVol(const char*contract,int sig_id, unsigned short sig_openclose, 
	unsigned short int sig_act, int32_t vol)
{
	int updated_vol = 0;

	// TODO: to be tested
	StrategyPosition *position = this->GetPosition(contract);
	if (sig_openclose==alloc_position_effect_t::open_&& sig_act==signal_act_t::buy){
		if (position->frozen_open_long==0){
			updated_vol = GetMaxPosition(contract) - position->cur_long + position->cur_short;
		} 
	}
	else if (sig_openclose==alloc_position_effect_t::open_&& sig_act==signal_act_t::sell){
		if (position->frozen_open_short==0){
			updated_vol = GetMaxPosition(contract) - position->cur_short + position->cur_long;
		} 
	} else if (sig_openclose==alloc_position_effect_t::close_&& sig_act==signal_act_t::buy){
		if (position->frozen_close_short==0){
			updated_vol = GetMaxPosition(contract)+ position->cur_short - position->cur_long;
			if (updated_vol >  position->cur_short) updated_vol =  position->cur_short;
		} 
	}
	else if (sig_openclose==alloc_position_effect_t::close_&& sig_act==signal_act_t::sell){
		if (position->frozen_close_long==0){
			updated_vol = GetMaxPosition(contract) + position->cur_long - position->cur_short;
			if (updated_vol >  position->cur_long) updated_vol =  position->cur_long;
		}
	}
	else{ 
		clog_error("[%s] GetAvailableVol: strategy id:%d; act:%d; sig_openclose:%d, sig id:%d",
					module_name_, setting_.config.st_id, sig_act, sig_openclose, sig_id); 
	}

	if (updated_vol > vol) updated_vol = vol; 

	clog_debug("[%s] GetAvailableVol: strategy id:%d; signal id:%d; current long:%d; current short:%d; "
				"frozen_close_long:%d; frozen_close_short:%d; frozen_open_long:%d; "
				"frozen_open_short:%d; updated vol:%d",
				module_name_, setting_.config.st_id, sig_id, position->cur_long, position->cur_short,
				position->frozen_close_long, position->frozen_close_short,
				position->frozen_open_long, position->frozen_open_short, updated_vol);

	return updated_vol;
} 


bool Strategy::Deferred(const char*contract,int sig_id, unsigned short sig_openclose, unsigned short int sig_act)
{
	bool result = false;

	// TODO: to be tested
	StrategyPosition *position = this->GetPosition(contract);
	if (sig_openclose==alloc_position_effect_t::open_&& sig_act==signal_act_t::buy){
		if (position->frozen_open_long==0){
			result = false;
		} else { result = true; }
	}
	else if (sig_openclose==alloc_position_effect_t::open_&& sig_act==signal_act_t::sell){
		if (position->frozen_open_short==0){
			result = false;
		} else { result = true; }
	} else if (sig_openclose==alloc_position_effect_t::close_&& sig_act==signal_act_t::buy){
		if (position->frozen_close_short==0){
			result = false;
		} else { result = true; }
	}
	else if (sig_openclose==alloc_position_effect_t::close_&& sig_act==signal_act_t::sell){
		if (position->frozen_close_long==0){
			result = false;
		} else { result = true; }
	}
	else{ 
		clog_error("[%s] Deferred: strategy id:%d; act:%d; sig_openclose:%d, sig id:%d",
					module_name_, setting_.config.st_id, sig_act, sig_openclose, sig_id); 
	}


	clog_debug("[%s] Deferred: strategy id:%d; signal id:%d; current long:%d; current short:%d; "
				"frozen_close_long:%d; frozen_close_short:%d; frozen_open_long:%d; "
				"frozen_open_short:%d; ",
				module_name_, setting_.config.st_id, sig_id, position->cur_long, position->cur_short,
				position->frozen_close_long, position->frozen_close_short,
				position->frozen_open_long, position->frozen_open_short);

	return result;
} 

signal_t* Strategy::GetSignalBySigID(int32_t sig_id)
{
	int32_t cursor = sigid_sigidx_map_table_[sig_id];
	return &(sig_table_[cursor]);
}

void Strategy::Push(const signal_t &sig)
{
	sig_table_[cursor_] = sig;
	sigid_sigidx_map_table_[sig.sig_id] = cursor_;

	// signal response
	memset(&(sigrpt_table_[cursor_]), 0, sizeof(signal_resp_t));
	sigrpt_table_[cursor_].status = -1; 
	sigrpt_table_[cursor_].sig_id = sig.sig_id;
	sigrpt_table_[cursor_].sig_act = sig.sig_act;
	strcpy(sigrpt_table_[cursor_].symbol, sig.symbol);
	if (sig.sig_act==signal_act_t::buy){
		sigrpt_table_[cursor_].order_price = sig.buy_price;
	} else if (sig.sig_act==signal_act_t::sell){
		sigrpt_table_[cursor_].order_price = sig.sell_price;
	}

	// 从pending队列中撤单 done
	if (sig.sig_openclose == alloc_position_effect_t::open_){
		sigrpt_table_[cursor_].order_volume = sig.open_volume;
	}else if (sig.sig_openclose == alloc_position_effect_t::close_){
		sigrpt_table_[cursor_].order_volume = sig.close_volume;
	}

	cursor_++;
}

void Strategy::PrepareForExecutingSig(const char*contract,long localorderid,
			const signal_t &sig, int32_t actual_vol)
{
	int32_t cursor = sigid_sigidx_map_table_[sig.sig_id];
	this->Freeze(contract,sig.sig_openclose, sig.sig_act, actual_vol);
	sigrpt_table_[cursor].order_volume = actual_vol;
	// mapping table
	// sigid_sigandrptidx_map_table_[sig.sig_id] = cursor;
	int32_t counter = GetCounterByLocalOrderID(localorderid);
	localorderid_sigandrptidx_map_table_[counter] = cursor;
	sigid_localorderid_map_table_[sig.sig_id] = localorderid;

	clog_info("[%s] PrepareForExecutingSig: strategy id:%d; sig id: %d; cursor,%d;"
		"LocalOrderID:%ld;", module_name_, sig.st_id, sig.sig_id, cursor, localorderid);
}


long Strategy::GetLocalOrderIDByCounter(long counter)
{
    return GetId() + counter * 1000;		
}

int32_t Strategy::GetCounterByLocalOrderID(long local_ord_id)
{
	return (local_ord_id - GetId()) / 1000;
}

// improve place, cancel
int32_t Strategy::GetSignalIdxBySigId(long sigid)
{
	return sigid_sigidx_map_table_[sigid];
}

// improve place, cancel
int32_t Strategy::GetSignalIdxByLocalOrdId(long localordid)
{
	// get signal report by LocalOrderID
	int32_t counter = GetCounterByLocalOrderID(localordid);
	int32_t index = localorderid_sigandrptidx_map_table_[counter];
	return index;
}

// improve place, cancel
void Strategy::FeedTunnRpt(int32_t sigidx, const TunnRpt &rpt, int *sig_cnt, signal_t* sigs)
{
	// 成交状态不推给策略，放到OnRtnTrade阶段再推送给策略
	if(rpt.OrderStatus==THOST_FTDC_OST_AllTraded || rpt.OrderStatus==THOST_FTDC_OST_PartTradedQueueing){ 
		return;
	}
	
	signal_resp_t& sigrpt = sigrpt_table_[sigidx];
	signal_t& sig = sig_table_[sigidx];

	TThostFtdcOrderStatusType status = rpt.OrderStatus;
	// update signal report
	UpdateSigrptByTunnrpt(rpt.MatchedAmount, rpt.TradePrice, sigrpt, status, rpt.ErrorID);
	// update strategy's position
	UpdatePosition(rpt.contract,rpt.MatchedAmount, status, sig.sig_openclose, sig.sig_act, rpt.ErrorID);
	if (rpt.MatchedAmount > 0){
		// fill signal position report by tunnel report
		FillPositionRpt(pos_cache_);
	}

	feed_sig_response(&sigrpt, &pos_cache_.s_pos[0], sig_cnt, sigs);

	clog_info("[%s] FeedTunnRpt: strategy id:%d; contract:%s; long:%d; short:%d; sig_id:%d; \
				symbol:%s; sig_act:%d; order_volume:%d; order_price:%f; exec_price:%f; \
				exec_volume:%d; acc_volume:%d; status:%d; killed:%d; rejected:%d",
				module_name_, setting_.config.st_id, pos_cache_.s_pos[0].symbol, pos_cache_.s_pos[0].long_volume, 
				pos_cache_.s_pos[0].short_volume, sigrpt.sig_id, sigrpt.symbol,
				sigrpt.sig_act, sigrpt.order_volume, sigrpt.order_price, sigrpt.exec_price,
				sigrpt.exec_volume, sigrpt.acc_volume,sigrpt.status,sigrpt.killed,sigrpt.rejected);

}

bool Strategy::HasFrozenPosition(const char *contract)
{
	// TODO: to be tested 
	StrategyPosition *position = this->GetPosition(contract);
	if (position->frozen_open_long > 0 ||
		position->frozen_open_short > 0 || 
		position->frozen_close_long > 0 || 
		position->frozen_close_short > 0){
		return true;
	}else{
		return false;
	}

}

void Strategy::UpdatePosition(const char *contract,int32_t lastqty, TThostFtdcOrderStatusType status,
			unsigned short sig_openclose, unsigned short int sig_act, TThostFtdcErrorIDType err)
{
	// TODO: position be tested
	StrategyPosition *position = this->GetPosition(contract);
	if (lastqty > 0){
		if (sig_openclose==alloc_position_effect_t::open_ && sig_act==signal_act_t::buy){
			position->cur_long += lastqty;
			position->frozen_open_long -= lastqty;
		} else if (sig_openclose==alloc_position_effect_t::open_ && sig_act==signal_act_t::sell){
			position->cur_short += lastqty;
			position->frozen_open_short -= lastqty;
		} else if (sig_openclose==alloc_position_effect_t::close_ && sig_act==signal_act_t::buy){
			position->cur_short -= lastqty;
			position->frozen_close_short -= lastqty;
		} else if (sig_openclose==alloc_position_effect_t::close_ && sig_act==signal_act_t::sell){
			position->cur_long -= lastqty;
			position->frozen_close_long -= lastqty;
		}
	} //end if (rpt.MatchedAmount > 0)

	// 从pending队列中撤单 done
	if (err != CANCELLED_FROM_PENDING){
		if (status==THOST_FTDC_OST_AllTraded ||
			status==THOST_FTDC_OST_PartTradedNotQueueing ||
			status==THOST_FTDC_OST_Canceled){ // 释放冻结仓位
			if (sig_openclose==alloc_position_effect_t::open_ && sig_act==signal_act_t::buy){
				position->frozen_open_long = 0;
			} else if (sig_openclose==alloc_position_effect_t::open_ && sig_act==signal_act_t::sell){
				position->frozen_open_short = 0;
			} else if (sig_openclose==alloc_position_effect_t::close_ && sig_act==signal_act_t::buy){
				position->frozen_close_short = 0;
			} else if (sig_openclose==alloc_position_effect_t::close_ && sig_act==signal_act_t::sell){
				position->frozen_close_long = 0;
			}
		}
	}

	clog_info("[%s] UpdatePosition: strategy id:%d; current long:%d; current short:%d; \
				frozen_close_long:%d; frozen_close_short:%d; frozen_open_long:%d; \
				frozen_open_short:%d; ",
				module_name_, setting_.config.st_id, position->cur_long, position->cur_short,
				position->frozen_close_long, position->frozen_close_short,
				position->frozen_open_long, position->frozen_open_short);
}

void Strategy::FillPositionRpt(position_t &pos)
{
	// TODO: to be tested
	for(int i=0; i<pos.symbol_cnt; i++){
		pos.s_pos[i].long_volume = positions_[i].cur_long;
		pos.s_pos[i].short_volume = positions_[i].cur_short;
		pos.s_pos[i].changed = 1;
	}
}
void Strategy::UpdateSigrptByTunnrpt(int32_t lastqty, TThostFtdcPriceType last_price, 
			signal_resp_t& sigrpt, TThostFtdcOrderStatusType &status, TThostFtdcErrorIDType err)
{
	if (lastqty > 0){
		sigrpt.exec_price = last_price;
		sigrpt.exec_volume = lastqty;
		sigrpt.acc_volume += lastqty;
	}
	if(status==TUNN_ORDER_STATUS_UNDEFINED){
		if (sigrpt.acc_volume == sigrpt.order_volume ){
			status = THOST_FTDC_OST_AllTraded;
		}else{ 
			status = THOST_FTDC_OST_PartTradedQueueing;
		}
	}

	if (status == THOST_FTDC_OST_Canceled){
		sigrpt.killed = sigrpt.order_volume - sigrpt.acc_volume;
	}else{ sigrpt.killed = 0; }

	sigrpt.rejected = 0; 
	sigrpt.error_no = err;

	if (status == THOST_FTDC_OST_Canceled ||
		status == THOST_FTDC_OST_PartTradedNotQueueing){
		sigrpt.status = if_sig_state_t::SIG_STATUS_CANCEL;
	} else if(status == THOST_FTDC_OST_AllTraded){
		sigrpt.status = if_sig_state_t::SIG_STATUS_SUCCESS;
	} else if(status == THOST_FTDC_OST_PartTradedQueueing){
		sigrpt.status = if_sig_state_t::SIG_STATUS_PARTED;
	} else if(status== THOST_FTDC_OST_NoTradeQueueing || 
			sigrpt.status == THOST_FTDC_OST_NoTradeNotQueueing){
		sigrpt.status = if_sig_state_t::SIG_STATUS_ENTRUSTED;
	} else{
		clog_error("[%s] unexpected status:%d", module_name_,status);
	}
}

// strategy log
FILE * Strategy::get_log_file()
{
	return pfDayLogFile_;
}

/*
 * 因为lock_log_并不能对不同策略间的日志读写缓存进行同步，
 * 所以必须将不同的策略日志时间错开，保证一个策略日志落地后，
 * 再落地另外策略的日志
 *
 */
int32_t Strategy::FullLineCount()
{	
	return max_log_lines_ ;
}

bool Strategy::IsLogFull()
{
	if(log_cursor_ == FullLineCount()) {
		return true;
	}else{
		return false;
	}
}

void Strategy::get_log(vector<strat_out_log> &log_buffer, int32_t &count)
{
	log_buffer.swap(log_);
	count = log_cursor_;
	log_cursor_ = 0;
}


bool Strategy::Subscribed(const char*contract)
{
	bool subscribed = false;
	for(int i=0; i<this->setting_.config.symbols_cnt; i++){
		const symbol_t &symbol = this->setting_.config.symbols[i];
		if(0==strcmp(contract, symbol.name)){
			subscribed = true;
			break;
		}
	}
	return subscribed;
}

