// lic
#include<stdlib.h>
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
#include "pos_calcu.h"

using namespace std;
using namespace std::chrono;

Strategy::Strategy()
: module_name_("Strategy")
{
	valid_ = false;
	cursor_ = 0;

	pfn_init_;
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
	memset(sys_order_id_, 0, sizeof(sys_order_id_));
}

void Strategy::End(void)
{
//	if (valid_) SavePosition();

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
		// lic
		char cmd[1024];
		char bar_so[1024];
		sprintf(bar_so, "./lib/%s.so", this->setting_.file.c_str());
		pproxy_->deleteObject(bar_so);
		sprintf(cmd, "rm %s", bar_so);
		system(cmd);
		pproxy_ = NULL;
	}
}

string Strategy::generate_log_name(char* log_path)
 {
	string log_full_path = "";

	// parse model name
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

	// lic
	char cmd[1024];
	sprintf(cmd, "openssl des3 -d -k 617999 -salt -in %s.so | tar -xzf - -C ./lib", this->setting_.file.c_str());
	system(cmd);

	char bar_so[1024];
	sprintf(bar_so, "./lib/%s", this->setting_.file.c_str());

	pfn_init_ = (Init_ptr)pproxy_->findObject(bar_so, STRATEGY_METHOD_INIT);
	if (!pfn_init_){
		clog_warning("[%s] findObject failed, file:%s; method:%s; errno:%d", 
					module_name_, bar_so, STRATEGY_METHOD_INIT, errno);
	}

	pfn_feedshfemarketdata_ = (FeedShfeMarketData_ptr )pproxy_->findObject(
					bar_so, STRATEGY_METHOD_FEED_MD_MYSHFE);
	if (!pfn_feedshfemarketdata_ ){
		clog_warning("[%s] findObject failed, file:%s; method:%s; errno:%d", 
					module_name_, bar_so, STRATEGY_METHOD_FEED_MD_MYSHFE, errno);
	}

	pfn_feedinitposition_ = (FeedInitPosition_ptr)pproxy_->findObject(
				bar_so, STRATEGY_METHOD_FEED_INIT_POSITION);
	if (!pfn_feedinitposition_ ){
		clog_warning("[%s] findObject failed, file:%s; method:%s; errno:%d", 
					module_name_, bar_so, STRATEGY_METHOD_FEED_INIT_POSITION, errno);
	}

	pfn_feedsignalresponse_ = (FeedSignalResponse_ptr)pproxy_->findObject(
				bar_so, STRATEGY_METHOD_FEED_SIG_RESP);
	if (!pfn_feedsignalresponse_){
		clog_warning("[%s] findObject failed, file:%s; method:%s; errno:%d", 
					module_name_, bar_so, STRATEGY_METHOD_FEED_SIG_RESP, errno);
	}

	pfn_destroy_ = (Destroy_ptr)pproxy_->findObject(bar_so, STRATEGY_METHOD_FEED_DESTROY );
	if (!pfn_destroy_){
		clog_warning("[%s] findObject failed, file:%s; method:%s; errno:%d", 
					module_name_, bar_so, STRATEGY_METHOD_FEED_DESTROY, errno);
	}

	string model_log = generate_log_name(setting_.config.log_name);
	strcpy(setting_.config.log_name, model_log.c_str());
	setting_.config.log_id = setting_.config.st_id;

	clog_warning("[%s] open log file:%s", module_name_,setting_.config.log_name);

	// TODO: multi
	LoadPosition();
	
	// TODO: multi
	memset(&pos_cache_, 0, sizeof(position_t));
	for(int i=0; i<this->setting_.config.symbols_cnt; i++){
		strcpy(pos_cache_.s_pos[i].symbol, this->setting_.config.symbols[i].name);
	}
	pos_cache_.symbol_cnt = this->setting_.config.symbols_cnt;
	FillPositionRpt();

	string sym_log_name = generate_log_name(setting_.config.symbols[0].symbol_log_name);
	strcpy(setting_.config.symbols[0].symbol_log_name, sym_log_name.c_str());

	pfDayLogFile_ = fopen (setting_.config.log_name, "w");
	int err = 0;
	this->pfn_init_(&this->setting_.config, &err, log_.data()+log_cursor_);
	if((log_.data()+log_cursor_)->exch_time > 0) log_cursor_++;

	this->FeedInitPosition();
}

void Strategy::FeedInitPosition()
{
	signal_t sigs[10];
	int sig_cnt = 0;
	strategy_init_pos_t init_pos;
	memset(&init_pos, 0, sizeof(strategy_init_pos_t));

	// TODO: multi
	position_t &today_pos = init_pos._cur_pos;
	today_pos.symbol_cnt = this->setting_.config.symbols_cnt+1; 
	strncpy(today_pos.s_pos[0].symbol, "#CASH", sizeof(today_pos.s_pos[0].symbol));
	clog_warning("[%s] FeedInitPosition strategy id:%d; contract:%s; exchange:%d; long:%d; short:%d",
				module_name_, GetId(), today_pos.s_pos[0].symbol, today_pos.s_pos[0].exchg_code, 
				today_pos.s_pos[0].long_volume, today_pos.s_pos[0].short_volume);

	for(int i=0; i< this->setting_.config.symbols_cnt; i++){
		const char* cur_sym = this->setting_.config.symbols[i].name;
		symbol_pos_t &sym_pos = today_pos.s_pos[i+1];
		strncpy(sym_pos.symbol, cur_sym, sizeof(sym_pos.symbol));
		sym_pos.long_volume = 0;
		sym_pos.short_volume = 0;
		sym_pos.exchg_code = this->GetExchange(cur_sym); 

		clog_warning("[%s] FeedInitPosition strategy id:%d; contract:%s; exchange:%d; long:%d; short:%d",
					module_name_, GetId(), sym_pos.symbol, sym_pos.exchg_code, 
					sym_pos.long_volume, sym_pos.short_volume);
	}

	this->pfn_feedinitposition_(&init_pos, log_.data()+log_cursor_);
	if((log_.data()+log_cursor_)->exch_time > 0) log_cursor_++;
}

void Strategy::FeedMd(MYShfeMarketData* md, int *sig_cnt, signal_t* sigs)
{
	//clog_info("[test] proc [%s] [FeedMd] contract:%s, time:%s", module_name_, 
	//	md->InstrumentID, md->GetQuoteTime().c_str());
//	 clog_info("[%s] FeedMd MDBestAndDeep(data_flag=%d) signal: strategy id:%d;  ",
//				module_name_, md->data_flag, GetId());				

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

	//	 clog_info("[%s] FeedMd MDBestAndDeep(data_flag=%d) signal: strategy id:%d; sig_id:%d; "
	//				 "exchange:%d; symbol:%s; open_volume:%d; buy_price:%f; "
	//				 "close_volume:%d; sell_price:%f; sig_act:%d; sig_openclose:%d; orig_sig_id:%d",
	//				module_name_, md->data_flag, sigs[i].st_id, sigs[i].sig_id,
	//				sigs[i].exchange, sigs[i].symbol, sigs[i].open_volume, sigs[i].buy_price,
	//				sigs[i].close_volume, sigs[i].sell_price, sigs[i].sig_act, sigs[i].sig_openclose, sigs[i].orig_sig_id); 
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

// TODO: 需要支持多个合约
const char* Strategy::GetContract()
{
	return this->setting_.config.symbols[0].name;
}

exchange_names Strategy::GetExchange(const char* contract)
{
	for(int i=0; i< this->setting_.config.symbols_cnt; i++){
		if (strcmp(this->setting_.config.symbols[i].name, contract) == 0){
			return this->setting_.config.symbols[i].exchange;
		}
	}
}

int32_t Strategy::GetMaxPosition(const char* contract)
{
	for(int i=0; i< this->setting_.config.symbols_cnt; i++){
		if (strcmp(this->setting_.config.symbols[i].name, contract) == 0){
			return this->setting_.config.symbols[i].max_pos;
		}
	}
}

const char* Strategy::GetSoFile()
{
	return this->setting_.file.c_str();
}

int Strategy::GetLocalOrderID(int32_t sig_id)
{
	return sigid_localorderid_map_table_[sig_id];
}

// TODO: multi 
// 
bool Strategy::Freeze(StrategyPosition *stra_pos,unsigned short sig_openclose, 
			unsigned short int sig_act, int32_t updated_vol)
{
	// 开仓限制要使用多空仓位的差值，锁仓部分不算
	if (sig_openclose==alloc_position_effect_t::open_&& sig_act==signal_act_t::buy){
		stra_pos->frozen_open_long += updated_vol;
	} else if (sig_openclose==alloc_position_effect_t::open_&& sig_act==signal_act_t::sell){
		stra_pos->frozen_open_short += updated_vol;
	} 

	if (sig_openclose==alloc_position_effect_t::close_&& sig_act==signal_act_t::buy){
		stra_pos->frozen_close_short += updated_vol;
	}
	else if (sig_openclose==alloc_position_effect_t::close_&& sig_act==signal_act_t::sell){
		stra_pos->frozen_close_long += updated_vol;
	}

	clog_debug("[%s] Freeze: strategy id:%d; current long:%d; current short:%d; \
				frozen_close_long:%d; frozen_close_short:%d; frozen_open_long:%d; \
				frozen_open_short:%d; ",
				module_name_, 
				setting_.config.st_id, 
				stra_pos->cur_long, 
				stra_pos->cur_short,
				stra_pos->frozen_close_long, 
				stra_pos->frozen_close_short,
				stra_pos->frozen_open_long, 
				stra_pos->frozen_open_short);
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

// TODO: multi 
int Strategy::GetAvailableVol(int sig_id, unsigned short sig_openclose, 
			unsigned short int sig_act, int32_t vol, const char *contract)
{
	StrategyPosition *cur_pos = NULL;
	for(int i=0; i<this->setting_.config.symbols_cnt; i++){
		if(strcmp(position_[i].contract, contract)==0){
			cur_pos = &position_[i];
			break;
		}
	}

	int updated_vol = 0;

	if (sig_openclose==alloc_position_effect_t::open_&& sig_act==signal_act_t::buy){
		if (cur_pos->frozen_open_long==0){
			updated_vol = GetMaxPosition(contract) - cur_pos->cur_long + cur_pos->cur_short;
		} 
	}
	else if (sig_openclose==alloc_position_effect_t::open_&& sig_act==signal_act_t::sell){
		if (cur_pos->frozen_open_short==0){
			updated_vol = GetMaxPosition(contract) - cur_pos->cur_short + cur_pos->cur_long;
		} 
	} else if (sig_openclose==alloc_position_effect_t::close_&& sig_act==signal_act_t::buy){
		if (cur_pos->frozen_close_short==0){
			updated_vol = GetMaxPosition(contract)+ cur_pos->cur_short - cur_pos->cur_long;
			if (updated_vol >  cur_pos->cur_short) updated_vol =  cur_pos->cur_short;
		} 
	}
	else if (sig_openclose==alloc_position_effect_t::close_&& sig_act==signal_act_t::sell){
		if (cur_pos->frozen_close_long==0){
			updated_vol = GetMaxPosition(contract) + cur_pos->cur_long - cur_pos->cur_short;
			if (updated_vol >  cur_pos->cur_long) updated_vol =  cur_pos->cur_long;
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
				module_name_, 
				setting_.config.st_id, 
				sig_id, 
				cur_pos->cur_long, 
				cur_pos->cur_short,
				cur_pos->frozen_close_long, 
				cur_pos->frozen_close_short,
				cur_pos->frozen_open_long, 
				cur_pos->frozen_open_short, 
				updated_vol);

	return updated_vol;
} 


// TODO: multi
bool Strategy::Deferred(int sig_id, unsigned short sig_openclose, unsigned short int sig_act)
{
	const char* contract = GetContractBySigId(sig_id);
	StrategyPosition *cur_pos = NULL;
	for(int i=0; i<this->setting_.config.symbols_cnt; i++){
		if(strcmp(position_[i].contract, contract)==0){
			cur_pos = &position_[i];
			break;
		}
	}
	bool result = false;

	if (sig_openclose==alloc_position_effect_t::open_&& sig_act==signal_act_t::buy){
		if (cur_pos->frozen_open_long==0){
			result = false;
		} else { result = true; }
	}
	else if (sig_openclose==alloc_position_effect_t::open_&& sig_act==signal_act_t::sell){
		if (cur_pos->frozen_open_short==0){
			result = false;
		} else { result = true; }
	} else if (sig_openclose==alloc_position_effect_t::close_&& sig_act==signal_act_t::buy){
		if (cur_pos->frozen_close_short==0){
			result = false;
		} else { result = true; }
	}
	else if (sig_openclose==alloc_position_effect_t::close_&& sig_act==signal_act_t::sell){
		if (cur_pos->frozen_close_long==0){
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
				module_name_, 
				setting_.config.st_id, 
				sig_id, 
				cur_pos->cur_long, 
				cur_pos->cur_short,
				cur_pos->frozen_close_long, 
				cur_pos->frozen_close_short,
				cur_pos->frozen_open_long, 
				cur_pos->frozen_open_short);

	return result;
} 

signal_t* Strategy::GetSignalBySigID(int32_t sig_id)
{
	int32_t cursor = sigid_sigidx_map_table_[sig_id];
	return &(sig_table_[cursor]);
}

if_sig_state_t Strategy::GetStatusBySigIdx(int32_t sig_idx)
{
	return (if_sig_state_t )sigrpt_table_[sig_idx].status;
}

char* Strategy::GetSysOrderIdBySigID(int32_t sig_id)
{
	int32_t cursor = sigid_sigidx_map_table_[sig_id];
	return sys_order_id_[cursor];
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

void Strategy::PrepareForExecutingSig(int localorderid, const signal_t &sig, int32_t actual_vol)
{
	int32_t cursor = sigid_sigidx_map_table_[sig.sig_id];
	StrategyPosition *cur_pos = NULL;
	for(int i=0; i<this->setting_.config.symbols_cnt; i++){
		if(strcmp(position_[i].contract, sig.symbol)==0){
			cur_pos = &position_[i];
			break;
		}
	}
	this->Freeze(cur_pos,sig.sig_openclose, sig.sig_act, actual_vol);
	sigrpt_table_[cursor].order_volume = actual_vol;
	// mapping table
	// sigid_sigandrptidx_map_table_[sig.sig_id] = cursor;
	int32_t counter = GetCounterByLocalOrderID(localorderid);
	localorderid_sigandrptidx_map_table_[counter] = cursor;
	sigid_localorderid_map_table_[sig.sig_id] = localorderid;

	clog_info("[%s] PrepareForExecutingSig: strategy id:%d; sig id: %d; cursor,%d;"
		"LocalOrderID:%d;", module_name_, sig.st_id, sig.sig_id, cursor, localorderid);
}


int Strategy::GetLocalOrderIDByCounter(int counter)
{
    return GetId() + counter * 1000;		
}

int32_t Strategy::GetCounterByLocalOrderID(int local_ord_id)
{
	return (local_ord_id - GetId()) / 1000;
}

// improve place, cancel
int32_t Strategy::GetSignalIdxBySigId(long sigid)
{
	return sigid_sigidx_map_table_[sigid];
}

// improve place, cancel
int32_t Strategy::GetSignalIdxByLocalOrdId(int localordid)
{
	// get signal report by LocalOrderID
	int32_t counter = GetCounterByLocalOrderID(localordid);
	int32_t index = localorderid_sigandrptidx_map_table_[counter];
	return index;
}

// improve place, cancel
void Strategy::FeedTunnRpt(int32_t sigidx, const TunnRpt &rpt, int *sig_cnt, signal_t* sigs)
{
	signal_resp_t& sigrpt = sigrpt_table_[sigidx];
	signal_t& sig = sig_table_[sigidx];
	strncpy(sys_order_id_[sigidx], rpt.OrderSysID, sizeof(sys_order_id_[sigidx]));

	// TODO:
	if_sig_state_t status; // 转换状态 = rpt.OrderStatus;
	// update signal report
	UpdateSigrptByTunnrpt(rpt.MatchedAmount, rpt.MatchedPrice, sigrpt, status, rpt.ErrorID);
	// update strategy's position
	// TODO: multi 
	StrategyPosition *cur_pos = NULL;
	for(int i=0; i<this->setting_.config.symbols_cnt; i++){
		if(strcmp(position_[i].contract, sig.symbol)==0){
			cur_pos = &position_[i];
			break;
		}
	}
	UpdatePosition(cur_pos, rpt.MatchedAmount, status, sig.sig_openclose, sig.sig_act, rpt.ErrorID);
	if (rpt.MatchedAmount > 0){
		// fill signal position report by tunnel report
		FillPositionRpt();
	}

	// TODO: multi 
	for(int i=0; i<this->setting_.config.symbols_cnt; i++){
		if(strcmp(pos_cache_.s_pos[i].symbol, sig.symbol)==0){
			feed_sig_response(&sigrpt, &pos_cache_.s_pos[i], sig_cnt, sigs);

			clog_info("[%s] FeedTunnRpt: strategy id:%d; contract:%s; long:%d; short:%d; sig_id:%d; \
						symbol:%s; sig_act:%d; order_volume:%d; order_price:%f; exec_price:%f; \
						exec_volume:%d; acc_volume:%d; status:%d; killed:%d; rejected:%d",
						module_name_, setting_.config.st_id, pos_cache_.s_pos[i].symbol, 
						pos_cache_.s_pos[0].long_volume, 
						pos_cache_.s_pos[i].short_volume, sigrpt.sig_id, sigrpt.symbol,
						sigrpt.sig_act, sigrpt.order_volume, sigrpt.order_price, sigrpt.exec_price,
						sigrpt.exec_volume, sigrpt.acc_volume,sigrpt.status,sigrpt.killed,
						sigrpt.rejected);
		}
	}

}

// TODO: multi 
bool Strategy::HasFrozenPosition(const char *contract)
{
	StrategyPosition *cur_pos = NULL;
	for(int i=0; i<this->setting_.config.symbols_cnt; i++){
		if(strcmp(position_[i].contract, contract)==0){
			cur_pos = &position_[i];
			break;
		}
	}
	if (cur_pos->frozen_open_long > 0 ||
		cur_pos->frozen_open_short > 0 || 
		cur_pos->frozen_close_long > 0 || 
		cur_pos->frozen_close_short > 0){
		return true;
	}else{
		return false;
	}

}

void Strategy::UpdatePosition(StrategyPosition *stra_pos, int32_t lastqty, if_sig_state_t status,
			unsigned short sig_openclose, unsigned short int sig_act, int err)
{
	// TODO: multi
	if (lastqty > 0){
		if (sig_openclose==alloc_position_effect_t::open_ && sig_act==signal_act_t::buy){
			stra_pos->cur_long += lastqty;
			stra_pos->frozen_open_long -= lastqty;
		}
		else if (sig_openclose==alloc_position_effect_t::open_ && sig_act==signal_act_t::sell){
			stra_pos->cur_short += lastqty;
			stra_pos->frozen_open_short -= lastqty;
		}
		else if (sig_openclose==alloc_position_effect_t::close_ && sig_act==signal_act_t::buy){
			stra_pos->cur_short -= lastqty;
			stra_pos->frozen_close_short -= lastqty;
		}
		else if (sig_openclose==alloc_position_effect_t::close_ && sig_act==signal_act_t::sell){
			stra_pos->cur_long -= lastqty;
			stra_pos->frozen_close_long -= lastqty;
		}
	} //end if (rpt.MatchedAmount > 0)

	// TODO: multi contracts
	// 从pending队列中撤单 done
	if (err != CANCELLED_FROM_PENDING){
		if (status==SIG_STATUS_SUCCESS ||
			status==SIG_STATUS_CANCEL||
			status==SIG_STATUS_REJECTED){ // 释放冻结仓位
			if (sig_openclose==alloc_position_effect_t::open_ && sig_act==signal_act_t::buy){
				stra_pos->frozen_open_long = 0;
			}
			else if (sig_openclose==alloc_position_effect_t::open_ && sig_act==signal_act_t::sell){
				stra_pos->frozen_open_short = 0;
			}
			else if (sig_openclose==alloc_position_effect_t::close_ && sig_act==signal_act_t::buy){
				stra_pos->frozen_close_short = 0;
			}
			else if (sig_openclose==alloc_position_effect_t::close_ && sig_act==signal_act_t::sell){
				stra_pos->frozen_close_long = 0;
			}
		}
	}

	clog_info("[%s] UpdatePosition: strategy id:%d; current long:%d; current short:%d; \
				frozen_close_long:%d; frozen_close_short:%d; frozen_open_long:%d; \
				frozen_open_short:%d; ",
				module_name_, setting_.config.st_id, stra_pos->cur_long, stra_pos->cur_short,
				stra_pos->frozen_close_long, stra_pos->frozen_close_short,
				stra_pos->frozen_open_long, stra_pos->frozen_open_short);
}

void Strategy::FillPositionRpt()
{
	position_t &pos = pos_cache_;
	// TODO: multi
	// 注意pos.s_pos与position_以同样的合约顺序存储
	for(int i=0; i<this->setting_.config.symbols_cnt; i++){
		pos.s_pos[i].long_volume  = position_[i].cur_long;
		pos.s_pos[i].short_volume = position_[i].cur_short;
		pos.s_pos[i].changed = 1;
	}
}
void Strategy::UpdateSigrptByTunnrpt(int32_t lastqty, double last_price, 
			signal_resp_t& sigrpt, if_sig_state_t &status, int err)
{
	if (lastqty > 0){
		sigrpt.exec_price = last_price;
		sigrpt.exec_volume = lastqty;
		sigrpt.acc_volume += lastqty;
	}
	if(status==SIG_STATUS_PARTED){
		if (sigrpt.acc_volume == sigrpt.order_volume ){
			status = SIG_STATUS_SUCCESS;
		}else{ 
			status = SIG_STATUS_PARTED;
		}
	}

	if (status == SIG_STATUS_CANCEL){
		sigrpt.killed = sigrpt.order_volume - sigrpt.acc_volume;
	}else{ sigrpt.killed = 0; }

	sigrpt.rejected = 0; 
	sigrpt.error_no = err;
	sigrpt.status = status;
}

void Strategy::LoadPosition()
{
//	pos_calc* pos_calc_ins = pos_calc::instance();
//
//	TODO: multi
	memset(&position_, 0, sizeof(position_));
	for(int i=0; i< this->setting_.config.symbols_cnt; i++){
		strcpy(position_[i].contract, this->setting_.config.symbols[i].name);
	}
//	string sett_cont = GetSymbol();
//	string stra = GetSoFile();
//	string cont = "";
//	if (pos_calc_ins->exists(stra)){
//		pos_calc_ins->get_pos(stra, position_.cur_long, position_.cur_short, cont);
//	} else {
//		position_.cur_long = 0;
//		position_.cur_short = 0;
//		cont = sett_cont;
//	}
//	if(sett_cont != cont){
//		clog_error("[%s] pos_calc error:strategy ID(%d); pos contract(%s); setting contract(%s)",
//			GetId(), cont.c_str(), sett_cont.c_str());
//	}
}

//void Strategy::SavePosition()
//{
//	char buffer[1024];
//	//sprintf (buffer, "%d;%s;%s;%d;%d",GetId(), GetSoFile(),
//	//	GetContract(), position_.cur_long, position_.cur_short); 
//
//	string fname = this->GetSoFile();
//	fname += ".pos";
//	std::ofstream outfile (fname);
//	outfile.write (buffer, strlen(buffer));
//	outfile.close();
//}

//const char * Strategy::GetSymbol()
//{
//	return setting_.config.symbols[0].name;
//}

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


const char* Strategy::GetContractBySigId(int32_t sig_id)
{
	signal_t& sig = sig_table_[sigid_sigidx_map_table_[sig_id]];
	return sig.symbol;
}

const char* Strategy::GetContractBySigIdx(int32_t sig_idx)
{
	signal_t& sig = sig_table_[sig_idx];
	return sig.symbol;
}

