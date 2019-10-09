#ifndef MY_QUOTE_INTERFACE_SHFE_MY_H_
#define MY_QUOTE_INTERFACE_SHFE_MY_H_

#include <sys/types.h>
#include <sys/time.h>
#include <set>
#include <string>
#include <functional>   // std::bind
#include "quote_datatype_shfe_my.h"
#include "quote_datatype_shfe_deep.h"
#include "quote_datatype_level1.h"
#include "l1md_producer.h"
#include "fulldepthmd_producer.h"
#include "vrt_value_obj.h"
#include "quote_cmn_save.h"
#include "my_cmn_util_funcs.h"
#include "quote_cmn_utility.h"
#include "quote_cmn_save.h"


using namespace std;

class DLL_PUBLIC MYQuoteData
{
	public:

		MYQuoteData(EfhLev2Producer *efhLev2_producer, L1MDProducer *l1_md_producer);
		~MYQuoteData();

		void SetQuoteDataHandler(std::function<void(CDepthMarketDataField *)> quote_handler);
		// business logic
		void ProcL1MdData(int32_t index);
		void ProcEfhLev2Data(int32_t index);

		QuoteDataSave<CDepthMarketDataField> *p_my_shfe_md_save_;
	private:
		// 禁止拷贝和赋值
		MYQuoteData(const MYQuoteData & other);
		MYQuoteData operator=(const MYQuoteData & other);

		void Send(MYShfeMarketData *data);
	
		MYShfeMarketData target_data_;
	
	    EfhLev2Producer* efhLev2Producer_;
		L1MDProducer* l1_md_producer_;
		int32_t l1_md_last_index_;
	
	private:
		std::string ToString(const CDepthMarketDataField &d);

		void Send(const char* contract);
		const char *module_name_;  

	    // 数据处理函数对象
	    std::function<void(CDepthMarketDataField *)> lev2_data_handler_;
};

#endif  //MY_QUOTE_INTERFACE_SHFE_MY_H_
