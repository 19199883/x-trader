#include <chrono>
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

	pfn_init_;
	pfn_feedshfemarketdata_ = NULL;
	pfn_feedsignalresponse_ = NULL;
	pfn_destroy_ = NULL;
	pfn_feedinitposition_ = NULL;
	pfn_destroy_ = NULL;
	pfn_setlogfn1_ = NULL;
	pfn_setlogfn2_ = NULL;

	pproxy_ = NULL;

	memset(log1_, 0, sizeof(log1_));
	log1_cursor_ = -1;
}

Strategy::~Strategy(void)
{
	if (valid_) SavePosition();

	WriteStrategyLog1();

	if (this->pfn_destroy_ != NULL){
		// TODO:
		//pfn_destroy_ ();
		clog_info("[%s] strategy(id:%d) destroyed", module_name_, this->setting_.config.st_id);
	}
	if (pproxy_ != NULL){
		pproxy_->deleteObject(this->setting_.file);
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

	// set breakpoint here
	this->setting_ = setting;
	this->pproxy_ = pproxy;

	pfn_init_ = (Init_ptr)pproxy_->findObject(this->setting_.file, STRATEGY_METHOD_INIT);
	if (!pfn_init_){
		clog_info("[%s] findObject failed, file:%s; method:%s; errno:%d", 
					module_name_, this->setting_.file.c_str(), STRATEGY_METHOD_INIT, errno);
	}

	pfn_feedshfemarketdata_ = (FeedShfeMarketData_ptr )pproxy_->findObject(
					this->setting_.file, STRATEGY_METHOD_FEED_MD_MYSHFE);
	if (!pfn_feedshfemarketdata_ ){
		clog_info("[%s] findObject failed, file:%s; method:%s; errno:%d", 
					module_name_, this->setting_.file.c_str(), STRATEGY_METHOD_FEED_MD_MYSHFE, errno);
	}

	pfn_feedinitposition_ = (FeedInitPosition_ptr)pproxy_->findObject(
				this->setting_.file, STRATEGY_METHOD_FEED_INIT_POSITION);
	if (!pfn_feedinitposition_ ){
		clog_info("[%s] findObject failed, file:%s; method:%s; errno:%d", 
					module_name_, this->setting_.file.c_str(), STRATEGY_METHOD_FEED_INIT_POSITION, errno);
	}

	pfn_feedsignalresponse_ = (FeedSignalResponse_ptr)pproxy_->findObject(
				this->setting_.file, STRATEGY_METHOD_FEED_SIG_RESP);
	if (!pfn_feedsignalresponse_){
		clog_info("[%s] findObject failed, file:%s; method:%s; errno:%d", 
					module_name_, this->setting_.file.c_str(), STRATEGY_METHOD_FEED_SIG_RESP, errno);
	}

	pfn_destroy_ = (Destroy_ptr)pproxy_->findObject(
				this->setting_.file, STRATEGY_METHOD_FEED_DESTROY );
	if (!pfn_destroy_){
		clog_info("[%s] findObject failed, file:%s; method:%s; errno:%d", 
					module_name_, this->setting_.file.c_str(), STRATEGY_METHOD_FEED_DESTROY, errno);
	}
	

	pfn_setlogfn1_ = (SetLogFn1Ptr)pproxy_->findObject(
				this->setting_.file, STRATEGY_METHOD_SET_LOG_FN1);
	if (!pfn_setlogfn1_ ){
		clog_info("[%s] findObject failed, file:%s; method:%s; errno:%d", 
					module_name_, this->setting_.file.c_str(), STRATEGY_METHOD_SET_LOG_FN1, errno);
	} else {
		//pfn_setlogfn1_(GetId(), StrategyLog::Log1);
	}

	pfn_setlogfn2_ = (SetLogFn2Ptr)pproxy_->findObject(
				this->setting_.file, STRATEGY_METHOD_SET_LOG_FN2);
	if (!pfn_setlogfn2_ ){
		clog_info("[%s] findObject failed, file:%s; method:%s; errno:%d", 
					module_name_, this->setting_.file.c_str(), STRATEGY_METHOD_SET_LOG_FN2, errno);
	} else {
		//pfn_setlogfn2_(GetId(), StrategyLog::Log2);
	}

	string model_log = generate_log_name(setting_.config.log_name);
	strcpy(setting_.config.log_name, model_log.c_str());
	setting_.config.log_id = setting_.config.st_id;

	LoadPosition();
	
	memset(&pos_cache_.s_pos[0], 0, sizeof(symbol_pos_t));
	strcpy(pos_cache_.s_pos[0].symbol, GetSymbol());
	pos_cache_.symbol_cnt = 1;

	string sym_log_name = generate_log_name(setting_.config.symbols[0].symbol_log_name);
	strcpy(setting_.config.symbols[0].symbol_log_name, sym_log_name.c_str());

	int err = 0;
	this->pfn_init_(&this->setting_.config, &err);
	this->FeedInitPosition();
}

void Strategy::FeedInitPosition()
{
	signal_t sigs[10];
	int sig_cnt = 0;
	strategy_init_pos_t init_pos;

	position_t &today_pos = init_pos._cur_pos;
	today_pos.symbol_cnt = 2; 
	symbol_pos_t& first = today_pos.s_pos[0];
	symbol_pos_t& second = today_pos.s_pos[1];

	strncpy(first.symbol, "#CASH", sizeof(first.symbol));

	strncpy(second.symbol, this->GetContract(), sizeof(second.symbol));
	second.long_volume = position_.cur_long;
	second.short_volume = position_.cur_short;
	second.exchg_code = this->GetExchange(); 

	this->pfn_feedinitposition_(&init_pos);

	clog_info("[%s] FeedInitPosition strategy id:%d; contract:%s; exchange:%d; long:%d; short:%d",
				module_name_, GetId(), second.symbol, second.exchg_code, 
				second.long_volume, second.short_volume);
}

void Strategy::FeedMd(MYShfeMarketData* md, int *sig_cnt, signal_t* sigs)
{
	clog_debug("[%s] rev MYShfeMarketDatacontract:%s; time:%s %s", module_name_, 
				md->InstrumentID, md->UpdateTime, md->UpdateMillisec);

	// perf
	// high_resolution_clock::time_point t0 = high_resolution_clock::now();
	
	*sig_cnt = 0;
	this->pfn_feedshfemarketdata_(md, sig_cnt, sigs);
	for (int i = 0; i < *sig_cnt; i++ ){

		// perf
	//	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	//	int latency = (t1.time_since_epoch().count() - t0.time_since_epoch().count()) / 1000;
	//	clog_warning("[%s] strategy latency:%d us", module_name_, latency); 

		sigs[i].st_id = this->GetId();

		// debug
		// clog_info("[%s] FeedMd(MDBestAndDeep signal: strategy id:%d; sig_id:%d; exchange:%d; symbol:%s; open_volume:%d; buy_price:%f; close_volume:%d; sell_price:%f; sig_act:%d; sig_openclose:%d; orig_sig_id:%d",
	//				module_name_, sigs[i].st_id, sigs[i].sig_id,
	//				sigs[i].exchange, sigs[i].symbol, sigs[i].open_volume, sigs[i].buy_price,
	//				sigs[i].close_volume, sigs[i].sell_price, sigs[i].sig_act, sigs[i].sig_openclose, sigs[i].orig_sig_id); 
	}
}

void Strategy::feed_sig_response(signal_resp_t* rpt, symbol_pos_t *pos, int *sig_cnt, signal_t* sigs)
{
	*sig_cnt = 0;
	this->pfn_feedsignalresponse_(rpt, pos, sig_cnt, sigs);
	for (int i = 0; i < *sig_cnt; i++ ){
		sigs[i].st_id = GetId();
		// debug
		//clog_info("[%s] feed_sig_respons esignal: strategy id:%d;  sig_id:%d; exchange:%d; symbol:%s; open_volume:%d; buy_price:%f; close_volume:%d; sell_price:%f; sig_act:%d; sig_openclose:%d; orig_sig_id:%d",
	//				module_name_, sigs[i].st_id, sigs[i].sig_id,
	//				sigs[i].exchange, sigs[i].symbol, sigs[i].open_volume, sigs[i].buy_price,
	//				sigs[i].close_volume, sigs[i].sell_price, sigs[i].sig_act, sigs[i].sig_openclose, sigs[i].orig_sig_id); 
	}
}


int32_t Strategy::GetId()
{
	return this->setting_.config.st_id;
}

const char* Strategy::GetContract()
{
	return this->setting_.config.symbols[0].name;
}

exchange_names Strategy::GetExchange()
{
	return this->setting_.config.symbols[0].exchange;
}

int32_t Strategy::GetMaxPosition()
{
	return this->setting_.config.symbols[0].max_pos;
}

const char* Strategy::GetSoFile()
{
	return this->setting_.file.c_str();
}

long Strategy::GetLocalOrderID(int32_t sig_id)
{
	return sigid_localorderid_map_table_[sig_id];
}

bool Strategy::Freeze(unsigned short sig_openclose, unsigned short int sig_act, int32_t updated_vol)
{
	// 开仓限制要使用多空仓位的差值，锁仓部分不算
	if (sig_openclose==alloc_position_effect_t::open_&& sig_act==signal_act_t::buy){
		position_.frozen_open_long += updated_vol;
	} else if (sig_openclose==alloc_position_effect_t::open_&& sig_act==signal_act_t::sell){
		position_.frozen_open_short += updated_vol;
	} 

	if (sig_openclose==alloc_position_effect_t::close_&& sig_act==signal_act_t::buy){
		position_.frozen_close_short += updated_vol;
	}
	else if (sig_openclose==alloc_position_effect_t::close_&& sig_act==signal_act_t::sell){
		position_.frozen_close_long += updated_vol;
	}

	clog_debug("[%s] Freeze: strategy id:%d; current long:%d; current short:%d; \
				frozen_close_long:%d; frozen_close_short:%d; frozen_open_long:%d; \
				frozen_open_short:%d; ",
				module_name_, setting_.config.st_id, position_.cur_long, position_.cur_short,
				position_.frozen_close_long, position_.frozen_close_short,
				position_.frozen_open_long, position_.frozen_open_short);
}

bool Strategy::Deferred(int sig_id, unsigned short sig_openclose, 
			unsigned short int sig_act, int32_t vol, int32_t& updated_vol)
{
	bool result = false;
	updated_vol = 0;

	if (sig_openclose==alloc_position_effect_t::open_&& sig_act==signal_act_t::buy){
		if (position_.frozen_open_long==0){
			updated_vol = GetMaxPosition() - position_.cur_long + position_.cur_short;
			result = false;
		} else { result = true; }
	}
	else if (sig_openclose==alloc_position_effect_t::open_&& sig_act==signal_act_t::sell){
		if (position_.frozen_open_short==0){
			updated_vol = GetMaxPosition() - position_.cur_short + position_.cur_long;
			result = false;
		} else { result = true; }
	} else if (sig_openclose==alloc_position_effect_t::close_&& sig_act==signal_act_t::buy){
		if (position_.frozen_close_short==0){
			updated_vol = GetMaxPosition()+ position_.cur_short - position_.cur_long;
			if (updated_vol >  position_.cur_short) updated_vol =  position_.cur_short;
			result = false;
		} else { result = true; }
	}
	else if (sig_openclose==alloc_position_effect_t::close_&& sig_act==signal_act_t::sell){
		if (position_.frozen_close_long==0){
			updated_vol = GetMaxPosition() + position_.cur_long - position_.cur_short;
			if (updated_vol >  position_.cur_long) updated_vol =  position_.cur_long;
			result = false;
		} else { result = true; }
	}
	else{ 
		clog_error("[%s] Deferred: strategy id:%d; act:%d; sig_openclose:%d",
				module_name_, setting_.config.st_id, sig_act, sig_openclose); 
	}

	if (updated_vol > vol) updated_vol = vol; 

	clog_debug("[%s] Deferred: strategy id:%d; signal id:%d; current long:%d; current short:%d; "
				"frozen_close_long:%d; frozen_close_short:%d; frozen_open_long:%d; "
				"frozen_open_short:%d; updated vol:%d",
				module_name_, setting_.config.st_id, sig_id, position_.cur_long, position_.cur_short,
				position_.frozen_close_long, position_.frozen_close_short,
				position_.frozen_open_long, position_.frozen_open_short, updated_vol);

	return result;
} 

void Strategy::PrepareForExecutingSig(long localorderid, const signal_t &sig, int32_t actual_vol)
{
	this->Freeze(sig.sig_openclose, sig.sig_act, actual_vol);

	// get next cursor
	static int32_t cursor = SIGANDRPT_TABLE_SIZE - 1;
	cursor++;
	if ((cursor % SIGANDRPT_TABLE_SIZE ) == 0){
		cursor = 0;
	}
	sig_table_[cursor] = sig;

	// signal response
	memset(&(sigrpt_table_[cursor]), 0, sizeof(signal_resp_t));
	sigrpt_table_[cursor].sig_id = sig.sig_id;
	sigrpt_table_[cursor].sig_act = sig.sig_act;
	strcpy(sigrpt_table_[cursor].symbol, sig.symbol);
	sigrpt_table_[cursor].order_volume = actual_vol;
	if (sig.sig_act==signal_act_t::buy){
		sigrpt_table_[cursor].order_price = sig.buy_price;
	} else if (sig.sig_act==signal_act_t::sell){
		sigrpt_table_[cursor].order_price = sig.sell_price;
	}

	// mapping table
	// sigid_sigandrptidx_map_table_[sig.sig_id] = cursor;
	int32_t counter = GetCounterByLocalOrderID(localorderid);
	localorderid_sigandrptidx_map_table_[counter] = cursor;
	sigid_localorderid_map_table_[sig.sig_id] = localorderid;

	clog_debug("[%s] PrepareForExecutingSig: strategy id:%d; sig id: %d; cursor,%d; LocalOrderID:%ld;",
				module_name_, sig.st_id, sig.sig_id, cursor, localorderid);
}


long Strategy::GetLocalOrderIDByCounter(long counter)
{
    return GetId() + counter * 1000;		
}

int32_t Strategy::GetCounterByLocalOrderID(long local_ord_id)
{
	return (local_ord_id - GetId()) / 1000;
}

void Strategy::FeedTunnRpt(TunnRpt &rpt, int *sig_cnt, signal_t* sigs)
{
	// get signal report by LocalOrderID
	int32_t counter = GetCounterByLocalOrderID(rpt.LocalOrderID);
	int32_t index = localorderid_sigandrptidx_map_table_[counter];
	signal_resp_t& sigrpt = sigrpt_table_[index];
	signal_t& sig = sig_table_[index];

	// update strategy's position
	UpdatePosition(rpt, sig.sig_openclose, sig.sig_act);
	// fill signal position report by tunnel report
	if (rpt.MatchedAmount > 0){
		FillPositionRpt(rpt, pos_cache_);
	}

	// update signal report
	UpdateSigrptByTunnrpt(sigrpt, rpt);

	feed_sig_response(&sigrpt, &pos_cache_.s_pos[0], sig_cnt, sigs);

	clog_debug("[%s] FeedTunnRpt: strategy id:%d; contract:%s; long:%d; short:%d; sig_id:%d; \
				symbol:%s; sig_act:%d; order_volume:%d; order_price:%f; exec_price:%f; \
				exec_volume:%d; acc_volume:%d; status:%d; killed:%d; rejected:%d",
				module_name_, setting_.config.st_id, pos_cache_.s_pos[0].symbol, pos_cache_.s_pos[0].long_volume, 
				pos_cache_.s_pos[0].short_volume, sigrpt.sig_id, sigrpt.symbol,
				sigrpt.sig_act, sigrpt.order_volume, sigrpt.order_price, sigrpt.exec_price,
				sigrpt.exec_volume, sigrpt.acc_volume,sigrpt.status,sigrpt.killed,sigrpt.rejected);

}

bool Strategy::HasFrozenPosition()
{
	if (position_.frozen_open_long > 0 ||
		position_.frozen_open_short > 0 || 
		position_.frozen_close_long > 0 || 
		position_.frozen_close_short > 0){
		return true;
	}else{
		return false;
	}

}

void Strategy::UpdatePosition(const TunnRpt& rpt, unsigned short sig_openclose, unsigned short int sig_act)
{
	if (rpt.MatchedAmount > 0){
		if (sig_openclose==alloc_position_effect_t::open_ && sig_act==signal_act_t::buy){
			position_.cur_long += rpt.MatchedAmount;
			position_.frozen_open_long -= rpt.MatchedAmount;
		}
		else if (sig_openclose==alloc_position_effect_t::open_ && sig_act==signal_act_t::sell){
			position_.cur_short += rpt.MatchedAmount;
			position_.frozen_open_short -= rpt.MatchedAmount;
		}
		else if (sig_openclose==alloc_position_effect_t::close_ && sig_act==signal_act_t::buy){
			position_.cur_short -= rpt.MatchedAmount;
			position_.frozen_close_short -= rpt.MatchedAmount;
		}
		else if (sig_openclose==alloc_position_effect_t::close_ && sig_act==signal_act_t::sell){
			position_.cur_long -= rpt.MatchedAmount;
			position_.frozen_close_long -= rpt.MatchedAmount;
		}
	} //end if (rpt.MatchedAmount > 0)

	if (rpt.OrderStatus==USTP_FTDC_OS_AllTraded ||
		rpt.OrderStatus==USTP_FTDC_OS_PartTradedNotQueueing ||
		rpt.OrderStatus==USTP_FTDC_OS_Canceled){
		if (sig_openclose==alloc_position_effect_t::open_ && sig_act==signal_act_t::buy){
			position_.frozen_open_long = 0;
		}
		else if (sig_openclose==alloc_position_effect_t::open_ && sig_act==signal_act_t::sell){
			position_.frozen_open_short = 0;
		}
		else if (sig_openclose==alloc_position_effect_t::close_ && sig_act==signal_act_t::buy){
			position_.frozen_close_short = 0;
		}
		else if (sig_openclose==alloc_position_effect_t::close_ && sig_act==signal_act_t::sell){
			position_.frozen_close_long = 0;
		}
	}

	clog_debug("[%s] UpdatePosition: strategy id:%d; current long:%d; current short:%d; \
				frozen_close_long:%d; frozen_close_short:%d; frozen_open_long:%d; \
				frozen_open_short:%d; ",
				module_name_, setting_.config.st_id, position_.cur_long, position_.cur_short,
				position_.frozen_close_long, position_.frozen_close_short,
				position_.frozen_open_long, position_.frozen_open_short);
}

void Strategy::FillPositionRpt(const TunnRpt& rpt, position_t &pos)
{
	pos.s_pos[0].long_volume = position_.cur_long;
	pos.s_pos[0].short_volume = position_.cur_short;
	pos.s_pos[0].changed = 1;
}
void Strategy::UpdateSigrptByTunnrpt(signal_resp_t& sigrpt,const  TunnRpt& tunnrpt)
{
	sigrpt.exec_volume = tunnrpt.MatchedAmount;
	sigrpt.acc_volume += tunnrpt.MatchedAmount;

	if (tunnrpt.OrderStatus == USTP_FTDC_OS_Canceled){
		sigrpt.killed = sigrpt.order_volume - sigrpt.acc_volume;
	}else{ sigrpt.killed = 0; }

	sigrpt.rejected = 0; 
	sigrpt.error_no = tunnrpt.ErrorID;

	if (tunnrpt.OrderStatus==USTP_FTDC_OS_Canceled ||
		tunnrpt.OrderStatus==USTP_FTDC_OS_PartTradedNotQueueing){
		sigrpt.status = if_sig_state_t::SIG_STATUS_CANCEL;
	}
	else if(tunnrpt.OrderStatus == USTP_FTDC_OS_AllTraded){
		sigrpt.status = if_sig_state_t::SIG_STATUS_SUCCESS;
	}
	else if(tunnrpt.OrderStatus==USTP_FTDC_OS_PartTradedQueueing){
		sigrpt.status = if_sig_state_t::SIG_STATUS_PARTED;
	}
	else if(tunnrpt.OrderStatus == USTP_FTDC_OS_NoTradeQueueing || 
			tunnrpt.OrderStatus == USTP_FTDC_OS_NoTradeNotQueueing){
		sigrpt.status = if_sig_state_t::SIG_STATUS_ENTRUSTED;
	}
	else{
		// log error
	}
}

void Strategy::LoadPosition()
{
	pos_calc* pos_calc_ins = pos_calc::instance();

	memset(&position_, 0, sizeof(position_));
	string sett_cont = GetSymbol();
	string stra = GetSoFile();
	string cont = "";
	if (pos_calc_ins->exists(stra)){
		pos_calc_ins->get_pos(stra, position_.cur_long, position_.cur_short, cont);
	} else {
		position_.cur_long = 0;
		position_.cur_short = 0;
		cont = sett_cont;
	}
	if(sett_cont != cont){
		clog_warning("[%s] pos_calc error:strategy ID(%d); pos contract(%s); setting contract(%s)",
			GetId(), cont.c_str(), sett_cont.c_str());
	}
}

void Strategy::SavePosition()
{
	char buffer[1024];
	sprintf (buffer, "%d;%s;%s;%d;%d",GetId(), GetSoFile(),
		GetContract(), position_.cur_long, position_.cur_short); 

	string fname = this->GetSoFile();
	fname += ".pos";
	std::ofstream outfile (fname);
	outfile.write (buffer, strlen(buffer));
	outfile.close();
}

const char * Strategy::GetSymbol()
{
	return setting_.config.symbols[0].name;
}


void Strategy::WriteStrategyLog1()
{
}
