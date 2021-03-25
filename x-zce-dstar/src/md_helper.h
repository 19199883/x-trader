#ifndef MD_HELPER_H_
#define MD_HELPER_H_

#include <sys/types.h>
#include <sys/time.h>
#include <set>
#include <string>
#include <functional>   // std::bind
#include "l2md_producer.h"
#include "lev1_producer.h"
#include "vrt_value_obj.h"
#include "quote_cmn_save.h"
#include "quote_cmn_utility.h"
#include "quote_datatype_czce_level2.h"
#include "quote_cmn_save.h"
#include "ThostFtdcMdApi.h"

#ifdef PERSISTENCE_ENABLED 
	#define L1_DOMINANT_MD_BUFFER_SIZE 5120
#else
	#define L1_DOMINANT_MD_BUFFER_SIZE 1024
#endif

using namespace std;

class MdHelper
{
	public:

		MdHelper(L2MDProducer* l2_md_producer, 
				Lev1Producer *lev1Producer);
		~MdHelper();

		void SetQuoteDataHandler(
					std::function<void(ZCEL2QuotSnapshotField_MY*)> quote_handler);
		void ProcL1MdData(int32_t index);
		void ProcL2Data(int32_t index);

	private:
		// 禁止拷贝和赋值
		MdHelper(const MdHelper & other);
		MdHelper operator=(const MdHelper & other);
	
		void Convert(const StdQuote5 &lev2Data,
					CThostFtdcDepthMarketDataField *lev1Data,
					ZCEL2QuotSnapshotField_MY &myData);
		ZCEL2QuotSnapshotField_MY target_data_;
	
		std::string ToString(const ZCEL2QuotSnapshotField_MY* p);

	    L2MDProducer* l2_md_producer_;
		Lev1Producer * l1_md_producer_;
		/*
		 * 获取指定合约最新的一档行情。
		 */
		CThostFtdcDepthMarketDataField* GetData(const char *contract);
		CThostFtdcDepthMarketDataField md_buffer_[L1_DOMINANT_MD_BUFFER_SIZE] ;
	
	private:
		const char *module_name_;  

	    // 数据处理函数对象
	    std::function<void(ZCEL2QuotSnapshotField_MY*)> mymd_handler_;
		QuoteDataSave<ZCEL2QuotSnapshotField_MY> *p_md_save_;
};

#endif  //MD_HELPER_H_
