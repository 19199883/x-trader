#include <thread>         // std::thread
#include "quote_interface_shfe_my.h"

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
		 my_data =  l1_md_producer_->GetLastData(efh_data->m_symbol, l1_md_last_index_);
		if(NULL != my_data){			
			// from level1
			my_data->UpperLimitPrice =	  InvalidToZeroD(my_data->UpperLimitPrice);
			my_data->LowerLimitPrice =	  InvalidToZeroD(my_data->LowerLimitPrice);
			my_data->HighestPrice =		  InvalidToZeroD(my_data->HighestPrice);
			my_data->LowestPrice =		  InvalidToZeroD(my_data->LowestPrice);
			my_data->OpenPrice =		  InvalidToZeroD(my_data->OpenPrice);
			my_data->ClosePrice =		  InvalidToZeroD(my_data->ClosePrice);
			my_data->PreClosePrice =	  InvalidToZeroD(my_data->PreClosePrice);			
			my_data->PreOpenInterest =	  InvalidToZeroD(my_data->PreOpenInterest);
			my_data->SettlementPrice =	  InvalidToZeroD(my_data->SettlementPrice);
			my_data->PreSettlementPrice = InvalidToZeroD(my_data->PreSettlementPrice);			
			my_data->PreDelta =		InvalidToZeroD(my_data->PreDelta);
			my_data->CurrDelta =	InvalidToZeroD(my_data->CurrDelta);
			
			// the below is from sfh_lev2
			my_data->LastPrice =	InvalidToZeroD(efh_data->m_last_px);															
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

			// TODO: log
			char buffer[2048];
			clog_info("[%s] send data:%s", ShfeLev2Formater::Format(*my_data,buffer));

			if (lev2_data_handler_ != NULL) { lev2_data_handler_(my_data); }

#ifdef PERSISTENCE_ENABLED 
			timeval t;
			gettimeofday(&t, NULL);
			p_my_shfe_lev2_data_save_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, &my_data);
#endif
		}
		else
		{
			clog_warning("[%s] can not find lev1 for:%s",efh_data->m_symbol);
		}
	}
}

void MYQuoteData::SetQuoteDataHandler(std::function<void(CDepthMarketDataField*)> quote_handler)
{
	clog_warning("[%s] SetQuoteDataHandler invoked.", module_name_);
	lev2_data_handler_ = quote_handler;
}

void MYQuoteData::ProcL1MdData(int32_t index)
{
	l1_md_last_index_ = index;
}

