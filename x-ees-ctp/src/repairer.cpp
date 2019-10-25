#include <exception>      // std::exception
#include <string.h>
#include "repairer.h"
#include "quote_cmn_utility.h"

 std::string repairer::ToString(const MDPack &d) {
	//  clog_debug("[%s] (server:%d) MDPack Data: instrument:%s islast:%d"
	//			  "seqno:%d direction:%c count: %d",module_name_,
	//			  this->server_,
	//			  d.instrument, 
	//			  (int)d.islast, 
	//			  d.seqno, 
	//			  d.direction, d.count);
	  for(int i = 0; i < d.count; i++) {
//	      clog_debug("[%s] (server:%d) price%d: %lf, volume%d: %d",module_name_,
//			  this->server_, i, d.data[i].price, i, d.data[i].volume);
	  }
	 return "";
}

repairer::repairer(FullDepthMDProducer *full_depth_md_producer)
:module_name_("repairer")
{
	full_depth_md_producer_ = full_depth_md_producer;
	this->working_ = false;
	this->seq_no_ = -1;
	this->victim_ = "";
}

// done
bool repairer::lose_pkg(MDPackEx* data)
{
	if (-1 == seq_no_){// specially process for the first UPD data
		return false;
	}else{
		int new_sn = data->content.seqno / 10;

		if ((seq_no_+1) != new_sn){
//			clog_debug("[%s] (server:%d)package loss:sn from %d to %d", 
//				module_name_,server_,seq_no_, new_sn);
		}
	
		return (seq_no_+1) != new_sn;
	}
}

// done
bool repairer::find_start_point(MDPackEx* data)
{
	bool found = false;

	if (working_){
		throw logic_error("wrong invoke find_start_point!");
	}

	if (lose_pkg(data)) this->victim_ = "";

	if ("" == victim_){
		if (SHFE_FTDC_D_Buy==data->content.direction){ victim_ = data->content.instrument; }
		found = false;
	}else{
		if (IsEqualContract((char*)data->content.instrument,(char*)victim_.c_str())){ found = false; }
		else{
			working_ = true;
			victim_ = "";
			found = true; 
		}
	}

	return found;
}

// ignore the contracts not in dominant contract list
void repairer::pull_ready_data()
{
	bool ready_data_found = false;
	bool damaged = false;

	if (!sell_queue_.Empty()){
		while (!buy_queue_.Empty()){
			int sell_index = sell_queue_.Front();
			MDPackEx* sell = full_depth_md_producer_->GetData(sell_index);
			int buy_index = buy_queue_.Front();
			MDPackEx* buy = full_depth_md_producer_->GetData(buy_index);
			if (strcmp(sell->content.instrument,buy->content.instrument)>0){
				buy_queue_.PopFront();
			}else if (IsEqualContract(sell->content.instrument,buy->content.instrument)){
				MDPackEx *new_data = buy;
				if (new_data->damaged) damaged = true;
				if(full_depth_md_producer_->IsDominant(buy->content.instrument)){
					ready_queue_.PushBack(buy_index);
				}
				buy_queue_.PopFront();
				ready_data_found = true;
			}else{ break;}
		}

		// add all sell data into ready queue
		if (ready_data_found ){
			while (!sell_queue_.Empty()){
				int sell_index = sell_queue_.Front();
				MDPackEx *new_data = full_depth_md_producer_->GetData(sell_index);
				if (new_data->damaged) damaged = true;
				if(full_depth_md_producer_->IsDominant(new_data->content.instrument)){
					ready_queue_.PushBack(sell_index);
				}
				sell_queue_.PopFront();
			}

			if (!ready_queue_.Empty() && damaged){ // flag damaged data
				int back_index = ready_queue_.Back();
				MDPackEx *back_data = full_depth_md_producer_->GetData(back_index);
				char *instrument = back_data->content.instrument;
				int idx = ready_queue_.Size()-1;
				while (idx >= 0){
					int ready_index = ready_queue_.At(idx);
					MDPackEx *redy_data = full_depth_md_producer_->GetData(ready_index);
					if(IsEqualContract(redy_data->content.instrument,instrument)){
						redy_data->damaged = true;
					}
					idx--;
				} // while (idx >= 0)
			} // if (damaged)
		} // if (ready_data_found )

		this->sell_queue_.Clear();
	} // end if (!this->sell_queue_.empty()){
}

// done
void repairer::normal_proc_buy_data(int index)
{
	MDPackEx* data = full_depth_md_producer_->GetData(index);
	if (buy_queue_.Empty()){
		if (!sell_queue_.Empty()){
//			clog_debug("[%s] (server:%d)normal_proc_buy_data,"
//				"error(sell queue in NOT empty),sn:%d",
//				module_name_,server_, data->content.seqno);
		}
	}else{
		int32_t back_index = buy_queue_.Back();
		MDPackEx* back_data =  full_depth_md_producer_->GetData(back_index);
		if (strcmp(data->content.instrument, back_data->content.instrument)>=0){ // in one patch
			if (!sell_queue_.Empty()){
				clog_error("[%s] (server:%d)normal_proc_buy_data,"
					"error(sell queue in NOT empty),sn:%d",
					module_name_,this->server_, data->content.seqno);
			}
		}else{ // cross more than one patch
			// pull ready data
			pull_ready_data();	
			// remove un-integrity data from buy queue
			buy_queue_.Clear();
		}
	}

	// add new data into buy queue
	buy_queue_.PushBack(index);
}

// done
void repairer::repair_buy_data(int index)
{
	MDPackEx* data = full_depth_md_producer_->GetData(index);
	if (!IsEqualContract((char*)victim_.c_str(),data->content.instrument)){
		normal_proc_buy_data(index);
		this->victim_ = "";
	}
	else{ /* discard victim data */ }
}

// done
void repairer::normal_proc_sell_data(int index)
{
	MDPackEx* data = full_depth_md_producer_->GetData(index);
	sell_queue_.PushBack(index);
	if (data->content.count<MAX_PAIR || data->content.islast){
		pull_ready_data(); 
	} 
}

// done
void repairer::repair_sell_data(int index)
{
	MDPackEx* data = full_depth_md_producer_->GetData(index);

	if (!IsEqualContract((char*)victim_.c_str(),data->content.instrument)){
		if (!sell_queue_.Empty()) {
			clog_error("[%s] (server:%d)repair_sell_data,error, sell queue should"
				"be emptyr,sn:%d,victim:%s",module_name_, this->server_, data->content.seqno,victim_.c_str());
		}

		sell_queue_.PushBack(index);
		if (data->content.count < MAX_PAIR){ pull_ready_data(); }

		this->victim_ = "";
	}
	else{ /* discard victim data*/ }
}

// done
void repairer::flag_damaged_data()
{
	if (!buy_queue_.Empty()){
		int index = buy_queue_.Back();
		MDPackEx* data = full_depth_md_producer_->GetData(index);
		if (data->content.count==MAX_PAIR){ data->damaged = true; }
	}

	if (!sell_queue_.Empty()){
		int index = sell_queue_.Back();
		MDPackEx* data = full_depth_md_producer_->GetData(index);
		if (data->content.count==MAX_PAIR){ data->damaged = true; }
	}
}

// done
void repairer::print_queue()
{
	int buy_size = this->buy_queue_.Size();
	//for (int i=0; i<size; i++){
	//	MY_LOG_DEBUG("(server:%d)buy queue:seqno:%d",this->server_, this->buy_queue_[i].content.seqno);
	//}


	int sell_size = this->sell_queue_.Size();
	//for (int i=0; i<size; i++){
	//	MY_LOG_DEBUG("(server:%d)sell queue:seqno:%d",this->server_, this->sell_queue_[i].content.seqno);
	//}

	int ready_size = this->ready_queue_.Size();
	//for (int i=0; i<size; i++){
	//	MY_LOG_DEBUG("(server:%d)ready queue:seqno:%d",this->server_, this->ready_queue_[i].content.seqno);
	//}
}

// done
void repairer::proc_pkg_loss(MDPackEx* data)
{
	victim_ = data->content.instrument;
	flag_damaged_data();

	if (SHFE_FTDC_D_Buy==data->content.direction){
		if (!buy_queue_.Empty()){
			int buy_index = buy_queue_.Back();
			MDPackEx *buy_data = full_depth_md_producer_->GetData(buy_index);
			if (strcmp(data->content.instrument,buy_data->content.instrument)<0){ // cross more than one frame
				// pull out previous frame of usable data
				pull_ready_data();	
				// clear previous frame of unusable data
				buy_queue_.Clear();
				sell_queue_.Clear();
			}
		} // end if (!this->buy_queue_.empty())

	}// end if (SHFE_FTDC_D_Buy==data.direction)
	else if (SHFE_FTDC_D_Sell==data->content.direction){
		if (!sell_queue_.Empty()){
			int sell_index = sell_queue_.Back();
			MDPackEx *sell_data = full_depth_md_producer_->GetData(sell_index);
			if (strcmp(data->content.instrument,sell_data->content.instrument)<0){ // corss more than frame
				// pull out frevious frame of usable data
				pull_ready_data();	
				// clear previous frame of unsable data
				buy_queue_.Clear();
				sell_queue_.Clear();
			}else if (strcmp(data->content.instrument,sell_data->content.instrument)>=0){ // in one patch
				pull_ready_data();	
			}
		}
	} // end else if (SHFE_FTDC_D_Sell==data.direction)
}

// done
MDPackEx *repairer::next(bool &empty)
{
	MDPackEx *data = NULL;
	empty = true;

	if (!ready_queue_.Empty()){
		int index = ready_queue_.Front();
		data = full_depth_md_producer_->GetData(index);
		empty = false;
		ready_queue_.PopFront();
	}

	return data;
}

// 
// entrance of repairer class
// done
void repairer::rev(int index)
{
	MDPackEx* data = full_depth_md_producer_->GetData(index);

	//ToString(data->content);
	int new_svr = data->content.seqno % 10;
	if(new_svr != server_){
		throw logic_error("rev: server changed!");
	}

	int new_sn = data->content.seqno / 10;
	if ((seq_no_!=-1) && new_sn != seq_no_+1) {
		//clog_debug("[%s] seq no from %d to %d, packet loss",module_name_, seq_no_,new_sn);
	}

	if (!working_){ // find normal data start point
		find_start_point(data);

		if (working_){ // the first normal UDP data as start point
			if (SHFE_FTDC_D_Buy==data->content.direction){
				normal_proc_buy_data(index);
			}else if (SHFE_FTDC_D_Sell==data->content.direction){
				normal_proc_sell_data(index);
			}
		}
	}else{ // enter receiving process
		if (lose_pkg(data)){ // enter package loss process procedure
			proc_pkg_loss(data);
		}// end package loss process procedure
		else{ // enter package non-loss procedure
			if("" == victim_){ // normal data process procedure 
				if(SHFE_FTDC_D_Buy==data->content.direction){
					normal_proc_buy_data(index);
				}
				else if (SHFE_FTDC_D_Sell==data->content.direction){
					normal_proc_sell_data(index);
				}
			}else{ // enter data repaireing procedure
				if (SHFE_FTDC_D_Buy==data->content.direction){
					repair_buy_data(index);
				}else if (SHFE_FTDC_D_Sell==data->content.direction){
					repair_sell_data(index);
				}
			} // end data repaireing procedure
		} // end package non-loss procedure
	} // end enter receiving process

	seq_no_ = new_sn;
}

