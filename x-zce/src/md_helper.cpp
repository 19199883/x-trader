#include <ctime>
#include <ratio>
#include <chrono>
#include <thread>         // std::thread
#include "md_helper.h"

using std::chrono::system_clock;

MdHelper::MdHelper(L2MDProducer *l2_md_producer, TapMDProducer *l1_md_producer)
: l2_md_producer_(l2_md_producer), l1_md_producer_(l1_md_producer), 
  module_name_("MdHelper")
{
	clog_warning("[%s] L1_DOMINANT_MD_BUFFER_SIZE:%d;",module_name_,L1_DOMINANT_MD_BUFFER_SIZE);
	for(int i = 0; i < L1_DOMINANT_MD_BUFFER_SIZE; i++){
		TapAPIQuoteWhole &tmp = md_buffer_[i];
		strcpy(tmp.Contract.Commodity.CommodityNo, "");
		strcpy(tmp.Contract.ContractNo1, "");
	}


#ifdef PERSISTENCE_ENABLED 
    p_md_save_ = new QuoteDataSave<ZCEL2QuotSnapshotField_MY>("czce_level2", 
		CZCE_LEVEL2_QUOTE_TYPE);
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
	// discard option
	if(strlen(md->instrument) > 6)
	{
		return;
	}

	bool dominant = l2_md_producer_->IsDominant(md->instrument);
	clog_info("[test] StdQuote5 rev [%s]dominant:%d contract:%s, time:%s %d", 
				module_name_, 
				dominant, 
				md->instrument, 
				md->updateTime, 
				md->updateMS);

	// 抛弃非主力合约
	if(!dominant) return;

	clog_info("[test] ProcL2Data StdQuote5 contract:%s, idx:%d, turnover:%f", md->instrument,index,md->turnover);

	l1_md =  GetData(md->instrument); // md->instrument, e.g. SR1801
	if(NULL != l1_md){
		clog_info("[test] [%s] ProcL2Data L1 contract:%s%s, time:%s, turnover:%f", module_name_, 
			l1_md->Contract.Commodity.CommodityNo, l1_md->Contract.ContractNo1, l1_md->DateTimeStamp, l1_md->QTotalTurnover);

		Convert(*md, l1_md, target_data_);
		if (mymd_handler_ != NULL) mymd_handler_(&target_data_);

		clog_info("[test] ProcL2Data send [%s] contract:%s, time:%s", module_name_, 
				target_data_.ContractID, target_data_.TimeStamp);

#ifdef PERSISTENCE_ENABLED 
		timeval t;
		gettimeofday(&t, NULL);
		p_md_save_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, &target_data_);
#endif
	}
	else
	{
		clog_error("[%s] ProcL2Data: %s; L1 is null.", module_name_, md->instrument);
	}
}

void MdHelper::Convert(const StdQuote5 &other, TapAPIQuoteWhole *tap_data,
	ZCEL2QuotSnapshotField_MY &data)
{
	if(tap_data != NULL){ // contents from level1 
		data.PreSettle = InvalidToZeroD(tap_data->QPreSettlePrice);	/*前结算价格*/
		data.PreClose = InvalidToZeroD(tap_data->QPreClosingPrice);	/*前收盘价格*/
		data.PreOpenInterest = (int)tap_data->QPrePositionQty;		/*previous days's positions */
		data.OpenPrice = InvalidToZeroD(tap_data->QOpeningPrice);	/*开盘价*/
		data.HighPrice = InvalidToZeroD(tap_data->QHighPrice);	    /*最高价*/
		data.LowPrice = InvalidToZeroD(tap_data->QLowPrice);	        /*最低价*/
		data.ClosePrice = InvalidToZeroD(tap_data->QClosingPrice);	    /*收盘价*/
		data.SettlePrice = InvalidToZeroD(tap_data->QSettlePrice);	/*结算价*/
		data.HighLimit = InvalidToZeroD(tap_data->QLimitUpPrice);	/*涨停板*/
		data.LowLimit = InvalidToZeroD(tap_data->QLimitDownPrice);	/*跌停板*/
		data.LifeHigh = InvalidToZeroD(tap_data->QHisHighPrice);	/*历史最高成交价格*/
		data.LifeLow = InvalidToZeroD(tap_data->QHisLowPrice);	/*历史最低成交价格*/
		data.AveragePrice = InvalidToZeroD(tap_data->QAveragePrice);	/*均价*/
		data.OpenInterest = (int)tap_data->QPositionQty;	/*持仓量*/
		strcpy(data.ContractID, tap_data->Contract.Commodity.CommodityNo);		/*合约编码*/
		strcpy(data.ContractID + 2, tap_data->Contract.ContractNo1);		/*合约编码*/
	}
	 
	//时间：如2014-02-03 13:23:45   
	system_clock::time_point today = system_clock::now();
	std::time_t tt = system_clock::to_time_t ( today );
	strftime(data.TimeStamp, sizeof(data.TimeStamp), "%Y-%m-%d %H:%M:%S",localtime(&tt));
	strcpy(data.TimeStamp+11,other.updateTime);
	strcpy(data.TimeStamp+19,".");
	sprintf(data.TimeStamp+20,"%03d", 0/*other.updateMS*/); // 策略需要该时间字段.因当前行情的updateMS存储的是递增的值（不是时间的毫秒部分），故使用0代替

	data.TotalBidLot = (int)other.buyv;	/*委买总量*/
	data.TotalAskLot = (int)other.sellv;	/*委卖总量*/

	// 
	data.SettlePrice = other.turnover; // SettlePrice暂时存储成交金额(StdQuote5.turnover)（黄志平用）
	data.TotalVolume = other.volume;
	data.ContractIDType = 0;			/*合约类型 0->目前应该为0， 扩充：0:期货,1:期权,2:组合*/
	data.LastPrice = InvalidToZeroD(other.price);		/*最新价*/
	data.BidPrice[0] = InvalidToZeroD(other.bidPrice1);     /*买入价格 下标从0开始*/
	data.BidPrice[1] = InvalidToZeroD(other.bidPrice2);     /*买入价格 下标从0开始*/
	data.BidPrice[2] = InvalidToZeroD(other.bidPrice3);     /*买入价格 下标从0开始*/
	data.BidPrice[3] = InvalidToZeroD(other.bidPrice4);     /*买入价格 下标从0开始*/	
	data.BidPrice[4] = InvalidToZeroD(other.bidPrice5);     /*买入价格 下标从0开始*/

	data.AskPrice[0] = InvalidToZeroD(other.askPrice1);     /*卖出价 下标从0开始*/
	data.AskPrice[1] = InvalidToZeroD(other.askPrice2);     /*卖出价 下标从0开始*/
	data.AskPrice[2] = InvalidToZeroD(other.askPrice3);     /*卖出价 下标从0开始*/
	data.AskPrice[3] = InvalidToZeroD(other.askPrice4);     /*卖出价 下标从0开始*/
	data.AskPrice[4] = InvalidToZeroD(other.askPrice5);     /*卖出价 下标从0开始*/
	data.BidLot[0] = other.bidVolume1;          /*买入数量 下标从0开始*/
	data.BidLot[1] = other.bidVolume2;          /*买入数量 下标从0开始*/
	data.BidLot[2] = other.bidVolume3;          /*买入数量 下标从0开始*/
	data.BidLot[3] = other.bidVolume4;          /*买入数量 下标从0开始*/
	data.BidLot[4] = other.bidVolume5;          /*买入数量 下标从0开始*/

	data.AskLot[0] = other.askVolume1;          /*卖出数量 下标从0开始*/
	data.AskLot[1] = other.askVolume2;          /*卖出数量 下标从0开始*/
	data.AskLot[2] = other.askVolume3;          /*卖出数量 下标从0开始*/
	data.AskLot[3] = other.askVolume4;          /*卖出数量 下标从0开始*/
	data.AskLot[4] = other.askVolume5;          /*卖出数量 下标从0开始*/
}

void MdHelper::SetQuoteDataHandler(std::function<void(ZCEL2QuotSnapshotField_MY*)> quote_handler)
{
	clog_warning("[%s] SetQuoteDataHandler invoked.", module_name_);
	mymd_handler_= quote_handler;
}

void MdHelper::ProcL1MdData(int32_t index)
{
	TapAPIQuoteWhole *new_l1_md =  l1_md_producer_->GetData(index);

	TapAPIQuoteWhole *old_l1_md = NULL;
	for(int i = 0; i < L1_DOMINANT_MD_BUFFER_SIZE; i++){
		TapAPIQuoteWhole &tmp = md_buffer_[i];
		if(IsEmptyString(tmp.Contract.ContractNo1)){ // 空字符串表示已到了缓存中第一个未使用的缓存项
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

	for(int i = 0; i < L1_DOMINANT_MD_BUFFER_SIZE; i++){
		TapAPIQuoteWhole &tmp = md_buffer_[i];
		if(IsEmptyString(tmp.Contract.ContractNo1)){ // 空字符串表示已到了缓存中第一个未使用的缓存项
			break;
		}

		if(IsEqualSize4(contract, 
						tmp.Contract.Commodity.CommodityNo, 
						tmp.Contract.ContractNo1))
		{ // contract: e.g. SR1801
			data = &tmp; 
			break;
		}
	}

	return data;
}

std::string MdHelper::ToString(const ZCEL2QuotSnapshotField_MY * p)
{
	char buf[10240];
	if (p) {
		snprintf(buf, sizeof(buf), "structName=ZCEL2QuotSnapshotField_MY\n"
			"\tContractID=%s\n"
			"\tTimeStamp=%s\n"
			"\tPreSettle=%f\n"
			"\tPreClose=%f\n"
			"\tPreOpenInterest=%i\n"
			"\tOpenPrice=%f\n"
			"\tHighPrice=%f\n"
			"\tLowPrice=%f\n"
			"\tLastPrice=%f\n"
			"\tTotalVolume=%i\n"
			"\tOpenInterest=%i\n"
			"\tClosePrice=%f\n"
			"\tSettlePrice=%f\n"
			"\tAveragePrice=%f\n"
			"\tLifeHigh=%f\n"
			"\tLifeLow=%f\n"
			"\tHighLimit=%f\n"
			"\tLowLimit=%f\n"
			"\tTotalBidLot=%i\n"
			"\tTotalAskLot=%i\n"



			"\tBidLot1=%i\n"
			"\tBidLot2=%i\n"
			"\tBidLot3=%i\n"
			"\tBidLot4=%i\n"
			"\tBidLot5=%i\n"

			"\tAskLot1=%i\n"
			"\tAskLot2=%i\n"
			"\tAskLot3=%i\n"
			"\tAskLot4=%i\n"
			"\tAskLot5=%i\n"

			"\tBidPrice1=%f\n"
			"\tBidPrice2=%f\n"
			"\tBidPrice3=%f\n"
			"\tBidPrice4=%f\n"
			"\tBidPrice5=%f\n"

			"\tAskPrice1=%f\n"
			"\tAskPrice2=%f\n"
			"\tAskPrice3=%f\n"
			"\tAskPrice4=%f\n"
			"\tAskPrice5=%f\n",
			p->ContractID,                     ///< 合约编码         
			p->TimeStamp,
			p->PreSettle,                   ///< 前结算价格
			p->PreClose,                 ///< 前收盘价格
			p->PreOpenInterest,                    ///昨日空盘量
			p->OpenPrice,                   ///< 开盘价
			p->HighPrice,                 ///< 最高价
			p->LowPrice,                   ///< 最低价
			p->LastPrice,        ///< 最新价
			p->TotalVolume,                   ///
			p->OpenInterest,                ///< 持仓量
			p->ClosePrice,                  ///< 收盘价
			p->SettlePrice,                ///< 结算价
			p->AveragePrice,                ///< 均价
			p->LifeHigh,                ///< 历史最高成交价格
			p->LifeLow,                ///< 历史最低成交价格
			p->HighLimit,                ///< 涨停板
			p->LowLimit,                ///< 跌停板
			p->TotalBidLot,                ///< 委卖总量
			p->TotalAskLot,                ///< 委卖总量

			p->BidLot[0],                
			p->BidLot[1],                
			p->BidLot[2],                
			p->BidLot[3],                
			p->BidLot[4],                

			p->AskLot[0],                
			p->AskLot[1],                
			p->AskLot[2],                
			p->AskLot[3],                
			p->AskLot[4],                

			p->BidPrice[0],              
			p->BidPrice[1],              
			p->BidPrice[2],              
			p->BidPrice[3],              
			p->BidPrice[4],              

			p->AskPrice[0],              
			p->AskPrice[1],              
			p->AskPrice[2],              
			p->AskPrice[3],              
			p->AskPrice[4]              

		);
	}
	return buf;
}
