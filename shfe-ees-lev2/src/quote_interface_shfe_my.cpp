#include <thread>         // std::thread
#include "quote_interface_shfe_my.h"

MYQuoteData::MYQuoteData(EfhLev2Producer *efhLev2_producer, L1MDProducer *l1_md_producer)
	: efhLev2Producer_(efhLev2_producer),
	l1_md_producer_(l1_md_producer),
	module_name_("MYQuoteData")
{
	l2_md_last_index_ = L2MD_NPOS;

#ifdef PERSISTENCE_ENABLED 
    p_shfe_lev2_data_save_ = new QuoteDataSave<CThostFtdcDepthMarketDataField>("shfe_lev2_data", SHFE_LEV2_DATA_QUOTE_TYPE);
#endif

}

MYQuoteData::~MYQuoteData()
{

#ifdef PERSISTENCE_ENABLED 
    if (p_shfe_lev2_data_save_) delete p_shfe_lev2_data_save_;
#endif

	clog_warning("[%s] ~MYQuoteData invoked.", module_name_);
}

void MYQuoteData::ProcL1Data(int32_t index)
{
	CThostFtdcDepthMarketDataField* l1_data = l1_md_producer_->GetData(index);

	efh3_lev2* l2_data = NULL;
	if(l2_md_last_index_ != L2MD_NPOS)
	{
		 l2_data =  efhLev2Producer_->GetLastData(l1_data->InstrumentID, l2_md_last_index_);
		if(NULL != l2_data)
		{	
			// from level1
			//strcpy(l1_data->UpdateTime, l2_data->m_update_time);
			//l1_data->UpdateMillisec = l2_data->m_millisecond;
			l1_data->UpperLimitPrice =	  InvalidToZeroD(l1_data->UpperLimitPrice);
			l1_data->LowerLimitPrice =	  InvalidToZeroD(l1_data->LowerLimitPrice);
			l1_data->HighestPrice =		  InvalidToZeroD(l1_data->HighestPrice);
			l1_data->LowestPrice =		  InvalidToZeroD(l1_data->LowestPrice);
			l1_data->OpenPrice =		  InvalidToZeroD(l1_data->OpenPrice);
			l1_data->ClosePrice =		  InvalidToZeroD(l1_data->ClosePrice);
			l1_data->PreClosePrice =	  InvalidToZeroD(l1_data->PreClosePrice);			
			l1_data->PreOpenInterest =	  InvalidToZeroD(l1_data->PreOpenInterest);
			l1_data->SettlementPrice =	  InvalidToZeroD(l1_data->SettlementPrice);
			l1_data->PreSettlementPrice = InvalidToZeroD(l1_data->PreSettlementPrice);			
			//l1_data->PreDelta =		  InvalidToZeroD(l1_data->PreDelta);
			//l1_data->CurrDelta =		  InvalidToZeroD(l1_data->CurrDelta);
			l1_data->LastPrice =		  InvalidToZeroD(l1_data->LastPrice);														

			// the below is from sfh_lev2
			l1_data->Volume =					   l2_data->m_last_share;
			l1_data->Turnover =     InvalidToZeroD(l2_data->m_turnover);
			l1_data->OpenInterest = InvalidToZeroD(l2_data->m_open_interest);	
			// l2_datalev2_data_ = l2_data->m_symbol_code;
			l1_data->BidPrice1 =    InvalidToZeroD(l2_data->m_bid_1_px);
			l1_data->BidPrice2 =    InvalidToZeroD(l2_data->m_bid_2_px);
			l1_data->BidPrice3 =    InvalidToZeroD(l2_data->m_bid_3_px);
			l1_data->BidPrice4 =    InvalidToZeroD(l2_data->m_bid_4_px);
			l1_data->BidPrice5 =    InvalidToZeroD(l2_data->m_bid_5_px);
			
			l1_data->BidVolume1 =				   l2_data->m_bid_1_share;
			l1_data->BidVolume2 =				   l2_data->m_bid_2_share;
			l1_data->BidVolume3 =				   l2_data->m_bid_3_share;
			l1_data->BidVolume4 =				   l2_data->m_bid_4_share;
			l1_data->BidVolume5 =				   l2_data->m_bid_5_share;
			
			l1_data->AskPrice1 =    InvalidToZeroD(l2_data->m_ask_1_px);
			l1_data->AskPrice2 =    InvalidToZeroD(l2_data->m_ask_2_px);
			l1_data->AskPrice3 =    InvalidToZeroD(l2_data->m_ask_3_px);
			l1_data->AskPrice4 =    InvalidToZeroD(l2_data->m_ask_4_px);
			l1_data->AskPrice5 =    InvalidToZeroD(l2_data->m_ask_5_px);
			
			l1_data->AskVolume1 =                  l2_data->m_ask_1_share;		
			l1_data->AskVolume2 =                  l2_data->m_ask_2_share;
			l1_data->AskVolume3 =                  l2_data->m_ask_3_share;		
			l1_data->AskVolume4 =                  l2_data->m_ask_4_share;
			l1_data->AskVolume5 =                  l2_data->m_ask_5_share;			

			if (lev2_data_handler_ != NULL) { lev2_data_handler_(l1_data); }

#ifdef PERSISTENCE_ENABLED 
			timeval t;
			gettimeofday(&t, NULL);
			p_shfe_lev2_data_save_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, l1_data);
#endif
		}
		else
		{
			l1_data->BidPrice2 =    InvalidToZeroD(l1_data->BidPrice2);
			l1_data->BidPrice3 =    InvalidToZeroD(l1_data->BidPrice3);
			l1_data->BidPrice4 =    InvalidToZeroD(l1_data->BidPrice4);
			l1_data->BidPrice5 =    InvalidToZeroD(l1_data->BidPrice5);
			
			l1_data->AskPrice2 =    InvalidToZeroD(l1_data->AskPrice2);
			l1_data->AskPrice3 =    InvalidToZeroD(l1_data->AskPrice3);
			l1_data->AskPrice4 =    InvalidToZeroD(l1_data->AskPrice4);
			l1_data->AskPrice5 =    InvalidToZeroD(l1_data->AskPrice5);
			
			if (lev2_data_handler_ != NULL) { lev2_data_handler_(l1_data); }

#ifdef PERSISTENCE_ENABLED 
			timeval t;
			gettimeofday(&t, NULL);
			p_shfe_lev2_data_save_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, l1_data);
#endif
			char buffer[2048];
			clog_warning("[%s] can not find lev2 for:%s", 
						module_name_,  
						ShfeLev2Formater::Format(*l1_data, buffer));
		}
	}
}

void MYQuoteData::SetQuoteDataHandler(std::function<void(CThostFtdcDepthMarketDataField*)> quote_handler)
{
	clog_warning("[%s] SetQuoteDataHandler invoked.", module_name_);
	lev2_data_handler_ = quote_handler;
}

void MYQuoteData::ProcL2Data(int32_t index)
{
	l2_md_last_index_ = index;

	// TODO: commented for debug
	char buffer[2048];
	clog_info("[%s] rev efh3_lev2:%s", 
				module_name_,
				EfhLev2Producer::Format(*efhLev2Producer_->GetData(index), buffer));
}

