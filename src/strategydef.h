#include <sstream>
#include <stdio.h>

using namespace std;

Strategy::Strategy()
{
}

Strategy::~Strategy(void)
{
	finalize();
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

void Strategy::init(model_setting setting)
{
	this->setting_ = _setting;
	this->pproxy_ = CLoadLibraryProxy::CreateLoadLibraryProxy();

	pfn_init_ = (Init_ptr)(_pproxy->findObject(this->setting.file,STRATEGY_METHOD_INIT));
	if (!pfn_init_){
		LOG4CXX_ERROR(log4cxx::Logger::getRootLogger(),
				"invoking init_f unsuccessfully when loading model," << "error code锟斤拷" << errno);
	}

	pfn_feedbestanddeep = (FeedBestAndDeep_ptr)(_pproxy->findObject(
					this->setting.file,STRATEGY_METHOD_FEED_MD_BESTANDDEEP ));
	if (!pfn_feedbestanddeep){
		LOG4CXX_WARN(log4cxx::Logger::getRootLogger(),"invoking FeedSPIFQuote unsuccessfully," << "error code:" << errno);
	}

	pfn_feedorderstatistic_ = (FeedOrderStatistic_ptr)(_pproxy->findObject(
					this->setting.file,STRATEGY_METHOD_FEED_MD_ORDERSTATISTICS));
	if (!pfn_feedorderstatistic_){
		LOG4CXX_WARN(log4cxx::Logger::getRootLogger(),
				"invoking FeedCFQuote unsuccessfully," << "error code:" << errno);
	}

	pfn_feedinitposition_ = (FeedInitPosition_ptr)(
			_pproxy->findObject(this->setting.file, STRATEGY_METHOD_FEED_INIT_POSITION));
	if (!pfn_feedinitposition_ ){
		LOG4CXX_WARN(log4cxx::Logger::getRootLogger(),
				"creating FeedInitPositionF failed" << ",error code:" << errno);
	}

	pfn_feedsignalresponse_ = (FeedSignalResponse_ptr)(
		_pproxy->findObject(this->setting.file, STRATEGY_METHOD_FEED_SIG_RESP));
	if (!pfn_feedsignalresponse_){
		LOG4CXX_WARN(log4cxx::Logger::getRootLogger(),
				"invoking FeedSignalResponseF unsuccessfully" << ",error code:" << errno);
	}

	pfn_destroy_ = (Destroy_ptr)(
			_pproxy->findObject(
				this->setting.file, STRATEGY_METHOD_FEED_DESTROY ));
	if (!pfn_destroy_){
		LOG4CXX_WARN(log4cxx::Logger::getRootLogger(),
			"invoking FeedTimeEventF unsuccessfully" << ",error code:" << errno);
	}

	LOG4CXX_INFO(log4cxx::Logger::getRootLogger(),
			"load model " << this->setting.name << "successfully.");

	string model_log = generate_log_name(setting.config_.log_name);
	strcpy(setting.config_.log_name, model_log.c_str());
	setting.config_.log_id = setting.config_.st_id;

	int err = 0;
	this->init_imp(&this->setting.config_, &err);
	if (err < 0){
		LOG4CXX_ERROR(log4cxx::Logger::getRootLogger(), "failed to feed configuration, err no:" << err);
	}
}

void Strategy::trace(string title,symbol_pos_t *pos)
{
	LOG4CXX_TRACE(log4cxx::Logger::getRootLogger(),
			title << " long_price:" << pos->long_price
			<< " long_volume:" << pos->long_volume
			<< " short_price:" << pos->short_price
			<< " short_volume:" << pos->short_volume
			<< " symbol:" << pos->symbol
			<< "buy_volume:" <<  pos->today_buy_volume<< ","
			<< "today_aver_price_buy:" <<  pos->today_aver_price_buy<< ","
			<< "today_sell_volume:" <<  pos->today_sell_volume<< ","
			<< "today_aver_price_sell:" <<  pos->today_aver_price_sell<< ","
			<< " changed:" << pos->changed
	);

}

void Strategy::feed_init_position(strategy_init_pos_t *data,int *sig_cnt, signal_t *sig_out)
{
	*sig_cnt = 0;
	this->feed_init_position_imp(data,sig_cnt, sig_out);
	for (int cur_idx=0; cur_idx<*sig_cnt; cur_idx++ ){
		sig_out[cur_idx].st_id = this->setting.id;
		LOG4CXX_WARN(log4cxx::Logger::getRootLogger(),
				"feed_init_position do NOT support parameter 'sig_out'. ");
	}
}

void Strategy::feed_spif_quote(SPIFQuoteT &quote_ptr,signal_t* signals,int &signals_size)
{
	this->trace(quote_ptr);
	this->feed_spif_quote_imp(quote_ptr,signals,signals_size);
	for (int cur_idx=0; cur_idx<signals_size; cur_idx++ ){
		signals[cur_idx].st_id = this->setting.id;
	}
}

void Strategy::feed_cf_quote(CFQuoteT &quote_ptr,signal_t* signals,int &signals_size)
{
	signals_size = 0;
	this->feed_cf_quote_imp(quote_ptr,signals,signals_size);
	for (int cur_idx=0; cur_idx<signals_size; cur_idx++ ){
		signals[cur_idx].st_id = this->setting.id;
	}
}


void Strategy::finalize(void)
{
	if (this->DestroyF != NULL){
		LOG4CXX_TRACE(log4cxx::Logger::getRootLogger(),"destroy" << this->setting.id << "...");
		DestroyF();
		LOG4CXX_TRACE(log4cxx::Logger::getRootLogger(),this->setting.id << " destroyed");
	}

	if (_pproxy != NULL){
		_pproxy->deleteObject(this->setting.file);
		_pproxy->DeleteLoadLibraryProxy();
		_pproxy = NULL;
	}

	LOG4CXX_INFO(log4cxx::Logger::getRootLogger(),"unload model so successfully:" << this->setting.file);
}


void Strategy::init_imp(st_config_t *config, int *ret_code)
{
	try{
		this->InitFAddr(config, ret_code);
	}catch(exception& ex){
		LOG4CXX_ERROR(log4cxx::Logger::getRootLogger(),"init_imp throw exception:" <<  ex.what());
	} // end try{
}

void Strategy::feed_init_position_imp(strategy_init_pos_t *data, int *sig_cnt, signal_t *sig_out)
{
	this->FeedInitPositionF(data, sig_cnt, sig_out);
	trace(data);
}

void Strategy::feed_spif_quote_imp(SPIFQuoteT &quote_ptr,signal_t* signals,int &signals_size)
{
	try	{
		signals_size = 0;

		// maint.                                                                                       
		if(maintenance::enabled()){                                                                                     
			string contract = pending_quote_dao<SPIFQuoteT>::get_symbol(&quote_ptr);
			contract += "(invoking spif)";                                                                          
			maintenance::log(contract);                                                                                 
		}  

		this->FeedSPIFQuoteF(&quote_ptr,&signals_size,signals);
		// maint.                                                                                       
		if(maintenance::enabled()){                                                                                     
			string contract = pending_quote_dao<SPIFQuoteT>::get_symbol(&quote_ptr);
			contract += "(invoked spif)"; 
			maintenance::log(contract);                                                                                 
		}  

		if (signals_size > 0){
			trace("feed_spif_quote_imp:",signals,signals_size);
		}
	}
	catch(exception& ex){
		LOG4CXX_ERROR(log4cxx::Logger::getRootLogger(),"FeedQuoteF throw exception:" <<  ex.what());
	}
}

void Strategy::feed_cf_quote_imp(CFQuoteT &quote_ptr,signal_t* signals,int &signals_size)
{
	try	{
			signals_size = 0;

			// maint.                                                                                       
			if(maintenance::enabled()){
				string contract = pending_quote_dao<CFQuoteT>::get_symbol(&quote_ptr); 
				contract += "(invoking cf)";                                                                          
				maintenance::log(contract);
			}  

			this->FeedCFQuoteF(&quote_ptr,&signals_size,signals);

			// maint.                                                                                       
			if(maintenance::enabled()){ 
				string contract = pending_quote_dao<CFQuoteT>::get_symbol(&quote_ptr);
				contract += "(invoked cf)";                                                                          
				maintenance::log(contract);
			}  

			if (signals_size > 0){
				trace("feed_cf_quote_imp:",signals,signals_size);
			}
		}
		catch(exception& ex){
			LOG4CXX_ERROR(log4cxx::Logger::getRootLogger(),"FeedQuoteF throw exception:" <<  ex.what());
		}
}

void Strategy::trace(string title,signal_t* signals,const int &signals_size)
{
	int count = 0;
	int len = signals_size;
	LOG4CXX_TRACE(log4cxx::Logger::getRootLogger(),"count:" << len);
	for (; count<len; count++ ){
		LOG4CXX_TRACE(log4cxx::Logger::getRootLogger(),
				title
				<< "exchange:" <<  signals[count].exchange << ","
				<< "orig_sig_id:" <<  signals[count].orig_sig_id << ","
				<<"symbol:"<<  signals[count].symbol << ","
				<< "sig_act:"<<  signals[count].sig_act << ","
				<<"sig_id:" <<  signals[count].sig_id << ","
				<< "st_id:" <<  signals[count].st_id << ","
				<< "buy price:" <<  signals[count].buy_price << ","
				<< "open volume:" <<  signals[count].open_volume << ","
				<< "sell price:" <<  signals[count].sell_price << ","
				<< "close volume:" <<  signals[count].close_volume << ","
				<< "cancel_pattern:" <<  signals[count].cancel_pattern << ","
				<< "sig_openclose:" <<  signals[count].sig_openclose << ","
				<< "instr:" <<  signals[count].instr << ","
				);
	}
}

void Strategy::trace(signal_resp_t* rpt)
{
	LOG4CXX_TRACE(log4cxx::Logger::getRootLogger(),
			"feed rpt:aver_price," <<  rpt->aver_price << ","
			<< "error_no," <<  rpt->error_no << ","
			<<"exec_price,"<<  rpt->exec_price << ","
			<< "exec_volume," <<  rpt->exec_volume << ","
			<< "sig_act,"<<  rpt->sig_act << ","
			<<"sig_id," <<  rpt->sig_id << ","
			<< "status," <<  rpt->status << ","
			<< "killed," <<  rpt->killed << ","
			<< "symbol," <<  rpt->symbol<< ","
			<< "acc_volume," <<  rpt->acc_volume<< ","
			<< "order_volume," <<  rpt->order_volume<< ","
			);

}

void Strategy::trace(strategy_init_pos_t *data)
{
	LOG4CXX_WARN(log4cxx::Logger::getRootLogger(),
		 this->setting.file << "," "init pos,today position, " << "symbol_cnt:" <<  data->_today_pos.symbol_cnt);
	for(int i=0; i<data->_today_pos.symbol_cnt; i++){
		LOG4CXX_WARN(log4cxx::Logger::getRootLogger(),
				"Cont(" <<  data->_today_pos.s_pos[i].symbol<< ","
				<< "L:" <<  data->_today_pos.s_pos[i].long_volume<< ","
				<< "S:" <<  data->_today_pos.s_pos[i].short_volume<< ")");
	}
}

void Strategy::trace(const SPIFQuoteT &data)
{
//	LOG4CXX_TRACE(log4cxx::Logger::getRootLogger(),
//			 "exchange:" <<  data.InstrumentID << ","
//			<< "BidPrice1:" <<  data.BidPrice1 << ","
//			<< "BidVolume1:" <<  data.BidVolume1 << ","
//			<< "AskPrice1:" <<  data.AskPrice1 << ","
//			<< "AskVolume1:" <<  data.AskVolume1 << ","
//			);

}

void Strategy::trace(const T_RtnForQuote &data)
{
	LOG4CXX_TRACE(log4cxx::Logger::getRootLogger(),
			 "feed_quote_notify,for_quote_id:" <<  data.for_quote_id << ","
			<< "stock_code:" <<  data.stock_code << ","
			);

}

void Strategy::trace(pending_order_t *pending_ord)
{
	int count = 0;
	int pos_size = pending_ord->req_cnt;
	for (; count<pos_size; count++ ){
		LOG4CXX_TRACE(log4cxx::Logger::getRootLogger(),
			"pending orders"<<"," <<  pending_ord->pending_req[count].symbol << ","
			<<  this->setting.id << ","
			<<  pending_ord->pending_req[count].direct << ","
			<<  pending_ord->pending_req[count].price << ","
			<<  pending_ord->pending_req[count].volume << ",");
	}
}

void Strategy ::feed_sig_response(signal_resp_t* rpt,symbol_pos_t *pos,
		pending_order_t *pending_ord,signal_t* sigs,int &sigs_len)
{
	trace(rpt);
	trace(pending_ord);
	trace("position:",pos);

	sigs_len = 0;
	this->feed_sig_response_imp(rpt,pos,pending_ord,sigs,sigs_len);
	for (int cur_idx=0; cur_idx<sigs_len; cur_idx++ ){
		sigs[cur_idx].st_id = this->setting.id;
	}
}

void Strategy::feed_sig_response_imp(signal_resp_t* rpt,symbol_pos_t *pos,
		pending_order_t *pending_ord,signal_t* sigs,int &sigs_len)
{
	try	{
		signal_resp_t tmp = *rpt;
		sigs_len = 0;

		LOG4CXX_TRACE(log4cxx::Logger::getRootLogger(),"begin feed_sig_response_imp:" <<  this->setting.id);

		this->FeedSignalResponseF(&tmp,pos,pending_ord,&sigs_len,sigs);

		LOG4CXX_TRACE(log4cxx::Logger::getRootLogger(),"end feed_sig_response_imp:" <<  this->setting.id);

		if (sigs_len > 0){
			trace("feed_sig_response_imp:",sigs,sigs_len);
		}
	}
	catch(exception& ex){
		LOG4CXX_ERROR(log4cxx::Logger::getRootLogger(),"feed_sig_response_imp throw exception:" <<  ex.what());
	}
}

