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

#define INVALID_CURSOR -1; // 无效的索引

class DLL_PUBLIC MYQuoteData
{
	public:

		MYQuoteData(FullDepthMDProducer *fulldepth_md_producer, L1MDProducer *l1_md_producer);
		~MYQuoteData();

		void SetQuoteDataHandler(std::function<void(MYShfeMarketData *)> quote_handler);
		// business logic
		void ProcL1MdData(int32_t index);
		void ProcFullDepthData(int32_t index);

		QuoteDataSave<MYShfeMarketData> *p_my_shfe_md_save_;
	private:
		// 禁止拷贝和赋值
		MYQuoteData(const MYQuoteData & other);
		MYQuoteData operator=(const MYQuoteData & other);

		void Send(MYShfeMarketData *data);
	
		MYShfeMarketData target_data_;
	
	    FullDepthMDProducer* fulldepth_md_producer_;
		L1MDProducer* l1_md_producer_;
		int32_t l1_md_last_index_;
	
	private:
		std::string ToString(const MYShfeMarketData &d);

		/*
		 * 使用full_depth_data填充target
		 *
		 */ 
		void FillFullDepthInfo();
		/*
		 * 向target_data_填充盘口30档忙方向数据，以及总委买量和均价
		 */
		void FillBuyFullDepthInfo();
		/*
		 * 向target_data_填充盘口30档卖方向数据，以及总委卖量和均价
		 */
		void FillSellFullDepthInfo();
		/*
		 * 将用于生成MYShfeMarketData过程中使用的成员数据重置成初始状态
		 */
		void Reset();
		/*
		 * 存储买方向MDPackEx数据在FullDepthProcuder缓冲区的索引。
		 */	
		CShfeFtdcMBLMarketDataField buy_data_buffer_[300000];
		int buy_write_cursor_; // 记录买队列下一个可写的位置,初始指向数组首位置
		int buy_read_cursor_;  // 记录买队列下一个可读的位置,初始指向数组首位置
		CShfeFtdcMBLMarketDataField sell_data_buffer_[300000];
		int sell_write_cursor_; // 记录买队列下一个可写的位置,初始指向数组首位置
		int sell_read_cursor_;  // 记录买队列下一个可读的位置,初始指向数组首位置
		char cur_contract_[10]; // 记录卖方向正在接收的合约
		char new_contract_[10]; // 记录卖方向正在新收的合约

		/*
		 * 因考虑系统盘中启动的场景，所以当接收到第一个last（data.InstrumentID）
		 * 数据后，行情则进入ready状态，之后接收的数据都是完整的数据帧
		 */
		bool ready_;
	
		/*
		 * data:已经通过ProcFullDepthData函数填充了全挡数据内容 
		 * contract:数据所属的合约
		 * 该函数对data(已经通过ProcFullDepthData函数填充了全挡数据内容)
		 * 填充对用合约的最新一档行情，然后发送给订阅者
		 */
		void Send(const char* contract);
		const char *module_name_;  

	    // 数据处理函数对象
	    std::function<void(MYShfeMarketData *)> fulldepthmd_handler_;
};

#endif  //MY_QUOTE_INTERFACE_SHFE_MY_H_
