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
	efh3_lev2* data = efhLev2Producer_->GetData(index);

	CDepthMarketDataField* l1_md = NULL;
	if(l1_md_last_index_ != L1MD_NPOS){
		 l1_md =  l1_md_producer_->GetLastData(contract, l1_md_last_index_);
	}
	if(NULL != l1_md){
		// TODO: here
		memcpy(&target_data_, l1_md, sizeof(CDepthMarketDataField));

		if (fulldepthmd_handler_ != NULL) { fulldepthmd_handler_(&target_data_); }

#ifdef PERSISTENCE_ENABLED 
		timeval t;
		gettimeofday(&t, NULL);
		p_my_shfe_md_save_->OnQuoteData(t.tv_sec * 1000000 + t.tv_usec, &target_data_);
#endif
	}
}

void MYQuoteData::SetQuoteDataHandler(std::function<void(MYShfeMarketData*)> quote_handler)
{
	clog_warning("[%s] SetQuoteDataHandler invoked.", module_name_);
	fulldepthmd_handler_ = quote_handler;
}

void MYQuoteData::ProcL1MdData(int32_t index)
{
	// TODO: here
	l1_md_last_index_ = index;
	CDepthMarketDataField* md = l1_md_producer_->GetData(index);
}

