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

using namespace std;
using namespace std::chrono;

Strategy::Strategy()
: module_name_("Strategy"),lock_log_(ATOMIC_FLAG_INIT)
{
	valid_ = false;
	cursor_ = 0;

	pfn_init_;
	pfn_feedl2quotesnapshot_ = NULL;
	pfn_feedsignalresponse_ = NULL;
	pfn_destroy_ = NULL;
	pfn_feedinitposition_ = NULL;
	pfn_destroy_ = NULL;

	pproxy_ = NULL;

	log_ = vector<strat_out_log>(MAX_LINES_FOR_LOG);
	log_w_ = vector<strat_out_log>(MAX_LINES_FOR_LOG);
	log_cursor_ = 0;
	pfDayLogFile_ = NULL;
	lock_log_.test_and_set();
	thread_log_ = new std::thread(&Strategy::WriteLogImp,this);
	log_ended_ = false;
	log_write_count_ = 0;
}

void Strategy::End(void)
{
	if (valid_) SavePosition();

	WriteLog(true);
	fclose(pfDayLogFile_);

	if (this->pfn_destroy_ != NULL){
		//pfn_destroy_ ();
	}
}

Strategy::~Strategy(void)
{
	if(thread_log_!=NULL && !thread_log_->joinable()){
		delete thread_log_;
		thread_log_ = NULL; 
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

	pfn_feedl2quotesnapshot_ = (FeedL2QuoteSnapshot_ptr)pproxy_->findObject(
					this->setting_.file, STRATEGY_METHOD_FEED_MD_L2QUOTESNAPSHOT);

	pfn_feedinitposition_ = (FeedInitPosition_ptr)pproxy_->findObject(
				this->setting_.file, STRATEGY_METHOD_FEED_INIT_POSITION);

	pfn_feedsignalresponse_ = (FeedSignalResponse_ptr)pproxy_->findObject(
				this->setting_.file, STRATEGY_METHOD_FEED_SIG_RESP);

	pfn_destroy_ = (Destroy_ptr)pproxy_->findObject(
				this->setting_.file, STRATEGY_METHOD_FEED_DESTROY );
	
	string model_log = generate_log_name(setting_.config.log_name);
	strcpy(setting_.config.log_name, model_log.c_str());
	setting_.config.log_id = setting_.config.st_id;

	pfDayLogFile_ = fopen (setting_.config.log_name, "w");
	WriteLogTitle();

	LoadPosition();
	
	memset(&(pos_cache_.s_pos[0]), 0, sizeof(symbol_pos_t));
	strcpy(pos_cache_.s_pos[0].symbol, GetSymbol());
	pos_cache_.symbol_cnt = 1;
	FillPositionRpt(pos_cache_);

	string sym_log_name = generate_log_name(setting_.config.symbols[0].symbol_log_name);
	strcpy(setting_.config.symbols[0].symbol_log_name, sym_log_name.c_str());

	int err = 0;
	this->pfn_init_(&this->setting_.config, &err, log_.data()+log_cursor_);
	log_cursor_++;
	if(log_cursor_ == MAX_LINES_FOR_LOG) WriteLog(false); 

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

	this->pfn_feedinitposition_(&init_pos, log_.data()+log_cursor_);
	log_cursor_++;
	if(log_cursor_ == MAX_LINES_FOR_LOG) WriteLog(false); 
}

void Strategy::FeedMd(ZCEL2QuotSnapshotField_MY* md, int *sig_cnt, signal_t* sigs)
{
	*sig_cnt = 0;
	this->pfn_feedl2quotesnapshot_(md, sig_cnt, sigs, log_.data()+log_cursor_);
	log_cursor_++;
	if(log_cursor_ == MAX_LINES_FOR_LOG) WriteLog(false);

	for (int i = 0; i < *sig_cnt; i++ ){
		sigs[i].st_id = this->GetId();
	}
}

void Strategy::feed_sig_response(signal_resp_t* rpt, symbol_pos_t *pos, int *sig_cnt, signal_t* sigs)
{
	*sig_cnt = 0;
	this->pfn_feedsignalresponse_(rpt, pos, sig_cnt, sigs, log_.data()+log_cursor_);
	log_cursor_++;
	if(log_cursor_ == MAX_LINES_FOR_LOG) WriteLog(false);

	for (int i = 0; i < *sig_cnt; i++ ){
		sigs[i].st_id = GetId();
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
}

int Strategy::GetVol(const signal_t &sig)
{
	int32_t vol = 0;
	if (sig.sig_openclose == alloc_position_effect_t::open_){
		vol = sig.open_volume;
	} else if (sig.sig_openclose == alloc_position_effect_t::close_){
		vol = sig.close_volume;
	} else{
	}

	return vol;
}

int Strategy::GetAvailableVol(int sig_id, unsigned short sig_openclose, unsigned short int sig_act, int32_t vol)
{
	int updated_vol = 0;

	if (sig_openclose==alloc_position_effect_t::open_&& sig_act==signal_act_t::buy){
		if (position_.frozen_open_long==0){
			updated_vol = GetMaxPosition() - position_.cur_long + position_.cur_short;
		} 
	}
	else if (sig_openclose==alloc_position_effect_t::open_&& sig_act==signal_act_t::sell){
		if (position_.frozen_open_short==0){
			updated_vol = GetMaxPosition() - position_.cur_short + position_.cur_long;
		} 
	} else if (sig_openclose==alloc_position_effect_t::close_&& sig_act==signal_act_t::buy){
		if (position_.frozen_close_short==0){
			updated_vol = GetMaxPosition()+ position_.cur_short - position_.cur_long;
			if (updated_vol >  position_.cur_short) updated_vol =  position_.cur_short;
		} 
	}
	else if (sig_openclose==alloc_position_effect_t::close_&& sig_act==signal_act_t::sell){
		if (position_.frozen_close_long==0){
			updated_vol = GetMaxPosition() + position_.cur_long - position_.cur_short;
			if (updated_vol >  position_.cur_long) updated_vol =  position_.cur_long;
		}
	} else{ }

	if (updated_vol > vol) updated_vol = vol; 

	return updated_vol;
} 


bool Strategy::Deferred(int sig_id, unsigned short sig_openclose, unsigned short int sig_act)
{
	bool result = false;

	if (sig_openclose==alloc_position_effect_t::open_&& sig_act==signal_act_t::buy){
		if (position_.frozen_open_long==0){
			result = false;
		} else { result = true; }
	}
	else if (sig_openclose==alloc_position_effect_t::open_&& sig_act==signal_act_t::sell){
		if (position_.frozen_open_short==0){
			result = false;
		} else { result = true; }
	} else if (sig_openclose==alloc_position_effect_t::close_&& sig_act==signal_act_t::buy){
		if (position_.frozen_close_short==0){
			result = false;
		} else { result = true; }
	}
	else if (sig_openclose==alloc_position_effect_t::close_&& sig_act==signal_act_t::sell){
		if (position_.frozen_close_long==0){
			result = false;
		} else { result = true; }
	} else{ }

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
	sigrpt_table_[cursor_].sig_id = sig.sig_id;
	sigrpt_table_[cursor_].sig_act = sig.sig_act;
	strcpy(sigrpt_table_[cursor_].symbol, sig.symbol);
	if (sig.sig_act==signal_act_t::buy){
		sigrpt_table_[cursor_].order_price = sig.buy_price;
	} else if (sig.sig_act==signal_act_t::sell){
		sigrpt_table_[cursor_].order_price = sig.sell_price;
	}

	cursor_++;
}

void Strategy::PrepareForExecutingSig(long localorderid, const signal_t &sig, int32_t actual_vol)
{
	int32_t cursor = sigid_sigidx_map_table_[sig.sig_id];
	this->Freeze(sig.sig_openclose, sig.sig_act, actual_vol);
	sigrpt_table_[cursor].order_volume = actual_vol;
	// mapping table
	// sigid_sigandrptidx_map_table_[sig.sig_id] = cursor;
	int32_t counter = GetCounterByLocalOrderID(localorderid);
	localorderid_sigandrptidx_map_table_[counter] = cursor;
	sigid_localorderid_map_table_[sig.sig_id] = localorderid;
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

	// 使MatchedAmount为最后一次的成交量
	int32_t lastqty = rpt.MatchedAmount - sigrpt.acc_volume;
	// update strategy's position
	UpdatePosition(lastqty,rpt, sig.sig_openclose, sig.sig_act);
	// fill signal position report by tunnel report
	if (lastqty > 0){
		FillPositionRpt(pos_cache_);
	}

	// update signal report
	UpdateSigrptByTunnrpt(lastqty,sigrpt, rpt);

	feed_sig_response(&sigrpt, &pos_cache_.s_pos[0], sig_cnt, sigs);
}

bool Strategy::HasFrozenPosition()
{
	if (position_.frozen_open_long > 0 ||
		position_.frozen_open_short > 0 || 
		position_.frozen_close_long > 0 || 
		position_.frozen_close_short > 0){
		return true;
	}else{ return false; }
}

void Strategy::UpdatePosition(int32_t lastqty,const TunnRpt& rpt, unsigned short sig_openclose, 
			unsigned short int sig_act)
{
	if (lastqty > 0){
		if (sig_openclose==alloc_position_effect_t::open_ && sig_act==signal_act_t::buy){
			position_.cur_long += lastqty;
			position_.frozen_open_long -= lastqty;
		}else if (sig_openclose==alloc_position_effect_t::open_ && sig_act==signal_act_t::sell){
			position_.cur_short += lastqty;
			position_.frozen_open_short -= lastqty;
		}else if (sig_openclose==alloc_position_effect_t::close_ && sig_act==signal_act_t::buy){
			position_.cur_short -= lastqty;
			position_.frozen_close_short -= lastqty;
		}else if (sig_openclose==alloc_position_effect_t::close_ && sig_act==signal_act_t::sell){
			position_.cur_long -= lastqty;
			position_.frozen_close_long -= lastqty;
		}
	} //end if (rpt.MatchedAmount > 0)

	if (rpt.OrderStatus==TAPI_ORDER_STATE_CANCELED ||
		rpt.OrderStatus==TAPI_ORDER_STATE_LEFTDELETED ||
		rpt.OrderStatus==TAPI_ORDER_STATE_FAIL){
		if (sig_openclose==alloc_position_effect_t::open_ && sig_act==signal_act_t::buy){
			position_.frozen_open_long = 0;
		}else if (sig_openclose==alloc_position_effect_t::open_ && sig_act==signal_act_t::sell){
			position_.frozen_open_short = 0;
		}else if (sig_openclose==alloc_position_effect_t::close_ && sig_act==signal_act_t::buy){
			position_.frozen_close_short = 0;
		}else if (sig_openclose==alloc_position_effect_t::close_ && sig_act==signal_act_t::sell){
			position_.frozen_close_long = 0;
		}
	}
}

void Strategy::FillPositionRpt(position_t &pos)
{
	pos.s_pos[0].long_volume = position_.cur_long;
	pos.s_pos[0].short_volume = position_.cur_short;
	pos.s_pos[0].changed = 1;
}
void Strategy::UpdateSigrptByTunnrpt(int32_t lastqty,signal_resp_t& sigrpt,const  TunnRpt& tunnrpt)
{
	if(lastqty > 0){
		sigrpt.exec_price = tunnrpt.OrderMatchPrice;
		sigrpt.exec_volume = lastqty;
		sigrpt.acc_volume += lastqty;
	}
	
	if (tunnrpt.OrderStatus==TAPI_ORDER_STATE_CANCELED ||
		tunnrpt.OrderStatus==TAPI_ORDER_STATE_LEFTDELETED){
		sigrpt.killed = sigrpt.order_volume - sigrpt.acc_volume;
	}else{ sigrpt.killed = 0; }

	if (tunnrpt.OrderStatus==TAPI_ORDER_STATE_FAIL) sigrpt.rejected = sigrpt.order_volume;
	else sigrpt.rejected = 0; 

	sigrpt.error_no = tunnrpt.ErrorID;

	if (tunnrpt.OrderStatus==TAPI_ORDER_STATE_CANCELED ||
		tunnrpt.OrderStatus==TAPI_ORDER_STATE_LEFTDELETED){
		sigrpt.status = if_sig_state_t::SIG_STATUS_CANCEL;
	}else if(tunnrpt.OrderStatus==TAPI_ORDER_STATE_FINISHED){
		sigrpt.status = if_sig_state_t::SIG_STATUS_SUCCESS;
	}else if(tunnrpt.OrderStatus==TAPI_ORDER_STATE_PARTFINISHED){
		sigrpt.status = if_sig_state_t::SIG_STATUS_PARTED;
	}else if(tunnrpt.OrderStatus==TAPI_ORDER_STATE_FAIL){
		sigrpt.status = if_sig_state_t::SIG_STATUS_REJECTED;
	}else if(tunnrpt.OrderStatus==TAPI_ORDER_STATE_ACCEPT){
		sigrpt.status = if_sig_state_t::SIG_STATUS_ENTRUSTED;
	}else{ }
}

void Strategy::LoadPosition()
{
	memset(&position_, 0, sizeof(position_));
	string sett_cont = GetSymbol();
	string stra = GetSoFile();
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


void Strategy::WriteLog(bool isEnded)
{
	log_ended_ = isEnded;
	log_w_.swap(log_);
	log_write_count_ = log_cursor_ ;
	lock_log_.clear();
	log_cursor_ = 0;
}
void Strategy::WriteLogTitle()
{
	// title
	fprintf (pfDayLogFile_, "exch_time  contract  n_tick  price  vol  bv1  bp1  sp1  sv1  amt  ");
	fprintf (pfDayLogFile_, "oi buy_price  sell_price  open_vol  close_vol  ");
	fprintf (pfDayLogFile_, "long_pos  short_pos  total_ordervol  total_cancelvol order_count cancel_count ");
	fprintf (pfDayLogFile_, "cash live total_vol max_dd max_net_pos max_side_pos ");
	for(int i=0; i< 11; i++)
	{
		fprintf(pfDayLogFile_,"sig%d ", i);
	}
	fprintf(pfDayLogFile_,"sig11\n");
}

void Strategy::WriteLogImp()
{
	while(!log_ended_){
		while (lock_log_.test_and_set()) {
			std::this_thread::sleep_for (std::chrono::seconds(10));
		}

		// content
		for(int i = 0; i < log_write_count_; i++){
			WriteOne(pfDayLogFile_, log_w_.data()+i);
		}
	}
}

void Strategy::WriteOne(FILE *pfDayLogFile, struct strat_out_log *pstratlog)
{
	if(0==pstratlog->exch_time) return;

    fprintf(pfDayLogFile,"%d %6s %d %14.2f %d ",
            pstratlog->exch_time,
            pstratlog->contract,
            pstratlog->n_tick,
            pstratlog->price,
            pstratlog->vol);

    fprintf(pfDayLogFile,"%d %12.4f %12.4f %d %ld %ld ",
            pstratlog->bv1,
            pstratlog->bp1,
            pstratlog->sp1,
            pstratlog->sv1,
            pstratlog->amt,
            pstratlog->oi);

    fprintf(pfDayLogFile,"%12.4f %12.4f %d %d ",
            pstratlog->buy_price,
            pstratlog->sell_price,
            pstratlog->open_vol,
            pstratlog->close_vol);

    fprintf(pfDayLogFile,"%d %d %d %d %d %d ",
            pstratlog->long_pos,
            pstratlog->short_pos,
            pstratlog->tot_ordervol,
            pstratlog->tot_cancelvol,
            pstratlog->order_cnt,
            pstratlog->cancel_cnt);

    fprintf(pfDayLogFile,"%16.2f %16.2f %d %16.2f %d %d ",
            pstratlog->cash,
            pstratlog->live,
            pstratlog->tot_vol,
            pstratlog->max_dd,
            pstratlog->max_net_pos,
            pstratlog->max_side_pos);

    for(int i=0; i< 11; i++)
    {
        fprintf(pfDayLogFile,"%0.2f ", pstratlog->sig[i]);
    }
    fprintf(pfDayLogFile,"%0.2f\n", pstratlog->sig[11]);
}

