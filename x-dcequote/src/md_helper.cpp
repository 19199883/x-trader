// done
#include <ctime>
#include <ratio>
#include <chrono>
#include <thread>         // std::thread
#include "md_helper.h"

using std::chrono::system_clock;

MdHelper::MdHelper(L2MDProducer *l2_md_producer, TapMDProducer *l1_md_producer) 
	 :  l2_md_producer_(l2_md_producer), 
		l1_md_producer_(l1_md_producer), 
		module_name_("MdHelper")
{
	clog_warning("[%s] MAX_CONTACT_COUNT:%d;",
				module_name_,
				MAX_CONTRACT_COUNT);
	for(int i = 0; i < MAX_CONTRACT_COUNT; i++){
		TapAPIQuoteWhole &tmp = md_buffer_[i];
		strcpy(tmp.Contract.Commodity.CommodityNo, "");
		strcpy(tmp.Contract.ContractNo1, "");
	}


#ifdef PERSISTENCE_ENABLED 
    p_md_save_ = new QuoteDataSave<YaoQuote>( "yao_quote", YAO_QUOTE_TYPE);
#endif
}

MdHelper::~MdHelper()
{
#ifdef PERSISTENCE_ENABLED 
    if (p_md_save_) delete p_md_save_;
#endif

	clog_warning("[%s] ~MdHelper invoked.", module_name_);
}

void MdHelper::ProcL2Data(int32_t index)
{
	TapAPIQuoteWhole* l1_md = NULL;

	StdQuote5* md = l2_md_producer_->GetData(index);

	clog_info("[test] ProcL2Data StdQuote5 contract:%s, idx:%d, turnover:%f", 
				md->instrument,
				index,
				md->turnover);

	l1_md =  GetData(md->instrument); // md->instrument, e.g. SR1801
	if(NULL != l1_md){
		clog_info("[test] [%s] ProcL2Data L1 contract:%s%s, time:%s, turnover:%f", 
					module_name_, 
					l1_md->Contract.Commodity.CommodityNo, 
					l1_md->Contract.ContractNo1, 
					l1_md->DateTimeStamp, 
					l1_md->QTotalTurnover);

		Convert(*md, l1_md, target_data_);
		if (mymd_handler_ != NULL) mymd_handler_(&target_data_);

		clog_info("[%s] send [%s]", module_name_, ToString(&target_data_).c_str());

#ifdef PERSISTENCE_ENABLED 
		timeval t;
		gettimeofday(&t, NULL);
		p_md_save_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, &target_data_);
#endif
	}
	else{ clog_error("[%s] ProcL2Data: L1 is null.", module_name_); }
}

int MdHelper::GetIntTime(const char *timestr)
{
	char buffer[30];
	//时间：如2014-02-03 13:23:45   
	strncpy(buffer, timestr+11, 2);	// hour
	strncpy(buffer+2, timestr+14, 2);		// min
	strncpy(buffer+4, timestr+17, 2);		// sec
	strncpy(buffer+6, timestr+20, 3);		// sec
	buffer[9] = 0;

	int int_time = atoi(buffer);
	if(int_time < 40000000)
	{
		int_time += 240000000;
	}

	return int_time;

}

void MdHelper::Convert(const StdQuote5 &other, 
			TapAPIQuoteWhole *tap_data, 
			YaoQuote &data)
{
	data.feed_type = FeedTypes::CzceLevel2;
	data.exchange = YaoExchanges::YCZCE;

	// TODO: yao debug	
	clog_warning("[%s] other.updateTime other.updateMS: %s %d; data.TimeStamp: %s", 
				module_name_, 
				other.updateTime,
				other.updateMS, 
				tap_data->DateTimeStamp);

	// TODO: yao. debug 需要调试看具体的值是什么样的
	data.int_time = GetIntTime(tap_data->DateTimeStamp);

	data.last_px = InvalidToZeroD(other.price);				/*最新价*/
	data.bp_array[0] = InvalidToZeroD(other.bidPrice1);     /*买入价格 下标从0开始*/
	data.bp_array[1] = InvalidToZeroD(other.bidPrice2);     /*买入价格 下标从0开始*/
	data.bp_array[2] = InvalidToZeroD(other.bidPrice3);     /*买入价格 下标从0开始*/
	data.bp_array[3] = InvalidToZeroD(other.bidPrice4);     /*买入价格 下标从0开始*/	
	data.bp_array[4] = InvalidToZeroD(other.bidPrice5);     /*买入价格 下标从0开始*/

	data.ap_array[0] = InvalidToZeroD(other.askPrice1);     /*卖出价 下标从0开始*/
	data.ap_array[1] = InvalidToZeroD(other.askPrice2);     /*卖出价 下标从0开始*/
	data.ap_array[2] = InvalidToZeroD(other.askPrice3);     /*卖出价 下标从0开始*/
	data.ap_array[3] = InvalidToZeroD(other.askPrice4);     /*卖出价 下标从0开始*/
	data.ap_array[4] = InvalidToZeroD(other.askPrice5);     /*卖出价 下标从0开始*/

	data.bv_array[0] = other.bidVolume1;          /*买入数量 下标从0开始*/
	data.bv_array[1] = other.bidVolume2;          /*买入数量 下标从0开始*/
	data.bv_array[2] = other.bidVolume3;          /*买入数量 下标从0开始*/
	data.bv_array[3] = other.bidVolume4;          /*买入数量 下标从0开始*/
	data.bv_array[4] = other.bidVolume5;          /*买入数量 下标从0开始*/

	data.av_array[0] = other.askVolume1;          /*卖出数量 下标从0开始*/
	data.av_array[1] = other.askVolume2;          /*卖出数量 下标从0开始*/
	data.av_array[2] = other.askVolume3;          /*卖出数量 下标从0开始*/
	data.av_array[3] = other.askVolume4;          /*卖出数量 下标从0开始*/
	data.av_array[4] = other.askVolume5;          /*卖出数量 下标从0开始*/

	data.total_vol = other.volume; //	总成交量
	data.total_notional = other.turnover; // 存储成交金额(StdQuote5.turnover)
	data.total_buy_ordsize = (int)other.buyv;	/*委买总量*/
	data.total_sell_ordsize = (int)other.sellv;	/*委卖总量*/

	if(tap_data != NULL){ // contents from level1 
		strcpy(data.symbol, tap_data->Contract.Commodity.CommodityNo);		/*合约编码*/
		strcpy(data.symbol + 2, tap_data->Contract.ContractNo1);		/*合约编码*/
		data.pre_close_px = InvalidToZeroD(tap_data->QPreClosingPrice);	/*前收盘价格*/
		data.pre_settle_px = InvalidToZeroD(tap_data->QPreSettlePrice);	/*前结算价格*/
		data.pre_open_interest = (int)tap_data->QPrePositionQty;		/*previous days's positions */
		data.open_interest = (int)tap_data->QPositionQty;	/*持仓量*/
		data.open_px = InvalidToZeroD(tap_data->QOpeningPrice);	/*开盘价*/
		data.high_px = InvalidToZeroD(tap_data->QHighPrice);	    /*最高价*/
		data.low_px = InvalidToZeroD(tap_data->QLowPrice);	        /*最低价*/
		data.avg_px = InvalidToZeroD(tap_data->QAveragePrice);	/*均价*/
		data.upper_limit_px = InvalidToZeroD(tap_data->QLimitUpPrice);	/*涨停板*/
		data.lower_limit_px = InvalidToZeroD(tap_data->QLimitDownPrice);	/*跌停板*/
		data.close_px = InvalidToZeroD(tap_data->QClosingPrice);	    /*收盘价*/
		data.settle_px = InvalidToZeroD(tap_data->QSettlePrice);	/*结算价*/
		
		// data.implied_bid_size // yao 不需要
		//data.implied_ask_size  // yao 不需要
		//data.weighted_buy_px	 // yao 不需要	
	}
}

void MdHelper::SetQuoteDataHandler(std::function<void(YaoQuote*)> quote_handler)
{
	clog_warning("[%s] SetQuoteDataHandler invoked.", module_name_);
	mymd_handler_= quote_handler;
}

void MdHelper::ProcL1MdData(int32_t index)
{
	TapAPIQuoteWhole *new_l1_md =  l1_md_producer_->GetData(index);

	TapAPIQuoteWhole *old_l1_md = NULL;
	for(int i = 0; i < MAX_CONTRACT_COUNT; i++){
		TapAPIQuoteWhole &tmp = md_buffer_[i];
		if(strcmp(tmp.Contract.ContractNo1, "") == 0){ // 空字符串表示已到了缓存中第一个未使用的缓存项
			old_l1_md = &tmp; 
			break;
		}

		if(strcmp(new_l1_md->Contract.Commodity.CommodityNo, tmp.Contract.Commodity.CommodityNo) == 0 &&
			strcmp(new_l1_md->Contract.ContractNo1, tmp.Contract.ContractNo1) == 0){ // contract: e.g. SR1801
			old_l1_md = &tmp; 
			break;
		}
	}

	*old_l1_md = *new_l1_md;

		clog_info("[%s] ProcL1MdData invoked. contract:%s%s", 
					module_name_, 
					new_l1_md->Contract.ContractNo1,
					new_l1_md->Contract.Commodity.CommodityNo);
}

TapAPIQuoteWhole* MdHelper::GetData(const char *contract)
{
	TapAPIQuoteWhole* data = NULL;

	for(int i = 0; i < MAX_CONTRACT_COUNT; i++){
		TapAPIQuoteWhole &tmp = md_buffer_[i];
		if(strcmp(tmp.Contract.ContractNo1, "") == 0){ // 空字符串表示已到了缓存中第一个未使用的缓存项
			break;
		}

		if(IsEqualSize4(contract, tmp.Contract.Commodity.CommodityNo, tmp.Contract.ContractNo1)){ // contract: e.g. SR1801
			data = &tmp; 
			break;
		}
	}

	return data;
}

std::string MdHelper::ToString(const YaoQuote *quote)
{
	return "";
}
