#if !defined(MY_QUOTE_INTERFACE_SHFE_MY_H_)
#define MY_QUOTE_INTERFACE_SHFE_MY_H_

#include <set>
#include <string>
#include <functional>   // std::bind
#include "quote_datatype_shfe_my.h"
#include "quote_datatype_shfe_deep.h"
#include "quote_datatype_level1.h"
#include "fulldepthmd_producer.h"
#include "l1md_producer.h"

using namespace std;

// 订阅的合约，集合类型
typedef std::set<std::string> SubscribeContracts;

struct Uniconfig
{
	// disruptor yield strategy
	char yield[20];
};

class DLL_PUBLIC MYQuoteData
{
public:

    /**
     * @param subscribe_contracts 需要订阅的合约集合。
	 * (指针为空指针，或集合为空，将返回行情接口接收到的所有合约数据)
     * @param provider_config_file 行情的配置文件名
     */
    MYQuoteData(const SubscribeContracts *subscribe,const string &provider_config);

    /**
     * @param quote_handler 行情处理的函数对象。shfe quote of my（xh提供）
     */
    void SetQuoteDataHandler(std::function<void(const SHFEQuote *)> quote_handler);
    void SetQuoteDataHandler(std::function<void(const CDepthMarketDataField *)> quote_handler);
    void SetQuoteDataHandler(std::function<void(const MYShfeMarketData *)> quote_handler);
	void SetQuoteDataHandler(std::function<void(const int *)> quote_handler){}
    ~MYQuoteData();

	void Start();
	void Stop();

private:
    // 禁止拷贝和赋值
    MYQuoteData(const MYQuoteData & other);
    MYQuoteData operator=(const MYQuoteData & other);

	void Send(MYShfeMarketData *data);

	bool running_;
	thread *consumer_thread_;
	const char* module_name_;  
	MYShfeMarketData target_md_;

	// 假设最多支持10个全挡数据服务器
	repairer repairers_[10];
	int server_;
	int seq_no_;

	/*
	 * somethings related with diruptor 
	 */
	struct vrt_consumer *consumer_;
	struct vrt_queue *queue_;
	FILE *clog_fp_;/*文件指针*/
	FullDepthMDProducer* fulldepth_md_producer_;
	L1MDProducer* l1_md_producer_;

	void ParseConfig();

	// business logic
	void ProcL1MdData(int32_t index);
	void ProcFullDepthData(int32_t index);
	Uniconfig config_;
private:
	/*
	 * 使用full_depth_data填充target
	 *
	 */ 
	void FillFullDepthInfo();
	/*
	 * 将用于生成MYShfeMarketData过程中使用的成员数据重置成初始状态
	 */
	void Reset();
	/*
	 * 存储买方向MDPackEx数据在FullDepthProcuder缓冲区的索引。
	 */	
	MDPackEx* buy_data_buffer_[1200];
	int buy_data_cursor_;
	MDPackEx* sell_data_buffer_[1200];
	int sell_data_cursor_;

	/*
	 * data:已经通过ProcFullDepthData函数填充了全挡数据内容 
	 * contract:数据所属的合约
	 * 该函数对data(已经通过ProcFullDepthData函数填充了全挡数据内容)
	 * 填充对用合约的最新一档行情，然后发送给订阅者
	 */
	void Send(const char* contract)
    // 数据处理函数对象
    std::function<void(const MYShfeMarketData *)> fulldepthmd_handler_;
};

#endif  //MY_QUOTE_INTERFACE_SHFE_MY_H_
