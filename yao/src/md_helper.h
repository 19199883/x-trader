#ifndef MD_HELPER_H_
#define MD_HELPER_H_

#include <sys/types.h>
#include <sys/time.h>
#include <set>
#include <string>
#include <functional>   // std::bind
#include "l2md_producer.h"
//#include "tap_md_producer.h"
#include "vrt_value_obj.h"
#include "quote_cmn_save.h"
#include "quote_cmn_utility.h"
#include "quote_datatype_czce_level2.h"
#include "quote_cmn_save.h"

#ifdef PERSISTENCE_ENABLED 
	#define L1_DOMINANT_MD_BUFFER_SIZE 600
#else
	#define L1_DOMINANT_MD_BUFFER_SIZE 14
#endif

using namespace std;

class MdHelper
{
	public:

		MdHelper(L2MDProducer* l2_md_producer/*, TapMDProducer *l1_md_producer*/);
		~MdHelper();

		void SetQuoteDataHandler(std::function<void(ZCEL2QuotSnapshotField_MY*)> quote_handler);
		void ProcL1MdData(int32_t index);
		void ProcL2Data(int32_t index);
	private:
		// 禁止拷贝和赋值
		MdHelper(const MdHelper & other);
		MdHelper operator=(const MdHelper & other);
	
//		void Convert(const StdQuote5 &other,TapAPIQuoteWhole_MY *tap_data,
//			ZCEL2QuotSnapshotField_MY &data);
		ZCEL2QuotSnapshotField_MY target_data_;
	
		std::string ToString(const ZCEL2QuotSnapshotField_MY* p);

	    L2MDProducer* l2_md_producer_;
		//TapMDProducer * l1_md_producer_;
		/*
		 * contract: 要获取行情的合约, SR1801
		 * 获取指定合约最新的一档行情。
		 * contract: e.g. SR1801
		 */
		//TapAPIQuoteWhole_MY* GetData(const char *contract);
		//TapAPIQuoteWhole_MY md_buffer_[L1_DOMINANT_MD_BUFFER_SIZE] ;
	
	private:
		const char *module_name_;  

	    // 数据处理函数对象
	    std::function<void(ZCEL2QuotSnapshotField_MY*)> mymd_handler_;
		QuoteDataSave<ZCEL2QuotSnapshotField_MY> *p_md_save_;
};

#endif  //MD_HELPER_H_
