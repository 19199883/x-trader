#include <thread>         // std::thread
#include "quote_interface_shfe_my.h"

std::string MYQuoteData::ToString(const CDepthMarketDataField &d) {
	clog_info("MYShfeMarketData: instrument:%s, data_flag:%d,buy_total_volume:%d;"
				"sell_total_volume:%d; buy_weighted_avg_price:%lf; sell_weighted_avg_price:%lf",
				d.InstrumentID, 
				d.data_flag, 
				d.buy_total_volume,
				d.sell_total_volume,
				d.buy_weighted_avg_price,
				d.sell_weighted_avg_price);

  return "";
}

MYQuoteData::MYQuoteData(EfhLev2Producer *efhLev2_producer, L1MDProducer *l1_md_producer)
	: efhLev2Producer_(efhLev2_producer),
	l1_md_producer_(l1_md_producer),
	module_name_("MYQuoteData")
{
	l1_md_last_index_ = L1MD_NPOS;

#ifdef PERSISTENCE_ENABLED 
    p_shfe_lev2_data_save_ = new QuoteDataSave<CDepthMarketDataField>("shfe_lev2_data", MY_SHFE_LEV2_DATA_QUOTE_TYPE);
#endif

}

MYQuoteData::~MYQuoteData()
{

#ifdef PERSISTENCE_ENABLED 
    if (p_shfe_lev2_data_save_) delete p_shfe_lev2_data_save_;
#endif

	clog_warning("[%s] ~MYQuoteData invoked.", module_name_);
}

void MYQuoteData::ProcEfhLev2Data(int32_t index)
{
	efh3_lev2* efh_data = efhLev2Producer_->GetData(index);

	CDepthMarketDataField* my_data = NULL;
	if(l1_md_last_index_ != L1MD_NPOS){
		 my_data =  l1_md_producer_->GetLastData(contract, l1_md_last_index_);
		if(NULL != my_data){			
			my_data->UpperLimitPrice = InvalidToZeroD(my_data->UpperLimitPrice);
			my_data->LowerLimitPrice = InvalidToZeroD(my_data.LowerLimitPrice);
			my_data->HighestPrice = InvalidToZeroD(my_data.HighestPrice);
			my_data->LowestPrice = InvalidToZeroD(my_data.LowestPrice);
			my_data->OpenPrice = InvalidToZeroD(my_data.OpenPrice);
			my_data->ClosePrice = InvalidToZeroD(my_data.ClosePrice);
			my_data->PreClosePrice = InvalidToZeroD(my_data.PreClosePrice);			
			my_data->PreOpenInterest = InvalidToZeroD(my_data.PreOpenInterest);
			my_data->SettlementPrice = InvalidToZeroD(my_data.SettlementPrice);
			my_data->PreSettlementPrice = InvalidToZeroD(my_data.PreSettlementPrice);			
			my_data->PreDelta = InvalidToZeroD(my_data.PreDelta);
			my_data->CurrDelta = InvalidToZeroDmy_datad.CurrDelta);
			
			my_data->LastPrice = InvalidToZeroD(efh_data->m_last_px);															
			my_data->Volume = efh_data->m_last_share;
			my_data->Turnover = InvalidToZeroD(efh_data->m_turnover);				
			strcpy(my_data->UpdateTime,efh_data->m_update_time);
			my_data->UpdateMillisec = efh_data->m_millisecond;
			my_data->OpenInterest = InvalidToZeroD(efh_data->m_open_interest);	
			
			// my_data-> = efh_data->m_symbol_code;
			
			my_data->BidPrice1 =  InvalidToZeroD(efh_data->m_bid_1_px);
			my_data->BidPrice2 =  InvalidToZeroD(efh_data->m_bid_2_px);
			my_data->BidPrice3 =  InvalidToZeroD(efh_data->m_bid_3_px);
			my_data->BidPrice4 =  InvalidToZeroD(efh_data->m_bid_4_px);
			my_data->BidPrice5 =  InvalidToZeroD(efh_data->m_bid_5_px);
			
			my_data->BidVolume1 = efh_data->m_bid_1_share;
			my_data->BidVolume2 = efh_data->m_bid_2_share;
			my_data->BidVolume3 = efh_data->m_bid_3_share;
			my_data->BidVolume4 = efh_data->m_bid_4_share;
			my_data->BidVolume5 = efh_data->m_bid_5_share;
				
			my_data->AskPrice1 =  InvalidToZeroD(efh_data->m_ask_1_px);
			my_data->AskPrice2 =  InvalidToZeroD(efh_data->m_ask_2_px);
			my_data->AskPrice3 =  InvalidToZeroD(efh_data->m_ask_3_px);
			my_data->AskPrice4 =  InvalidToZeroD(efh_data->m_ask_4_px);
			my_data->AskPrice5 =  InvalidToZeroD(efh_data->m_ask_5_px);
			
			my_data->AskVolume1 = efh_data->m_ask_1_share;		
			my_data->AskVolume2 = efh_data->m_ask_2_share;
			my_data->AskVolume3 = efh_data->m_ask_3_share;		
			my_data->AskVolume4 = efh_data->m_ask_4_share;
			my_data->AskVolume5 = efh_data->m_ask_5_share;			

			if (fulldepthmd_handler_ != NULL) { fulldepthmd_handler_(&target_data_); }

#ifdef PERSISTENCE_ENABLED 
			timeval t;
			gettimeofday(&t, NULL);
			p_my_shfe_md_save_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, &target_data_);
#endif
		}
		else
		{
			clog_warning("[%s] can not find lev1 for:%s",data->m_symbol);
		}
	}
}

void MYQuoteData::SetQuoteDataHandler(std::function<void(MYShfeMarketData*)> quote_handler)
{
	clog_warning("[%s] SetQuoteDataHandler invoked.", module_name_);
	fulldepthmd_handler_ = quote_handler;
}

void MYQuoteData::ProcL1MdData(int32_t index)
{
	l1_md_last_index_ = index;
}

void MYQuoteData::RalaceInvalidValue(CDepthMarketDataField &d)
{
    d.Turnover = InvalidToZeroD(d.Turnover);
    d.LastPrice = InvalidToZeroD(d.LastPrice);
    d.UpperLimitPrice = InvalidToZeroD(d.UpperLimitPrice);
    d.LowerLimitPrice = InvalidToZeroD(d.LowerLimitPrice);
    d.HighestPrice = InvalidToZeroD(d.HighestPrice);
    d.LowestPrice = InvalidToZeroD(d.LowestPrice);
    d.OpenPrice = InvalidToZeroD(d.OpenPrice);
    d.ClosePrice = InvalidToZeroD(d.ClosePrice);
    d.PreClosePrice = InvalidToZeroD(d.PreClosePrice);
    d.OpenInterest = InvalidToZeroD(d.OpenInterest);
    d.PreOpenInterest = InvalidToZeroD(d.PreOpenInterest);

    d.BidPrice1 = InvalidToZeroD(d.BidPrice1);
    d.BidPrice2 = InvalidToZeroD(d.BidPrice2);
    d.BidPrice3 = InvalidToZeroD(d.BidPrice3);
    d.BidPrice4 = InvalidToZeroD(d.BidPrice4);
    d.BidPrice5 = InvalidToZeroD(d.BidPrice5);

	d.AskPrice1 = InvalidToZeroD(d.AskPrice1);
    d.AskPrice2 = InvalidToZeroD(d.AskPrice2);
    d.AskPrice3 = InvalidToZeroD(d.AskPrice3);
    d.AskPrice4 = InvalidToZeroD(d.AskPrice4);
    d.AskPrice5 = InvalidToZeroD(d.AskPrice5);

	d.SettlementPrice = InvalidToZeroD(d.SettlementPrice);
	d.PreSettlementPrice = InvalidToZeroD(d.PreSettlementPrice);

    d.PreDelta = InvalidToZeroD(d.PreDelta);
    d.CurrDelta = InvalidToZeroD(d.CurrDelta);
}
