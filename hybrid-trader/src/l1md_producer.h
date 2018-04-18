#ifndef __L1MD_PRODUCER_H__
#define __L1MD_PRODUCER_H__

#include <functional>
#include <array>
#include <string>
#include <thread>         
#include <chrono>        
#include "vrt_value_obj.h"
#include "quote_datatype_level1.h"
#include "quote_datatype_shfe_my.h"
#include "quote_datatype_dce_level2.h"

///////////////////
// TODO: new
#include <sstream>
#include <list>
#include "ThostFtdcMdApi.h"
#include <boost/function.hpp>
#include "quote_cmn_utility.h"
#include "quote_cmn_save.h"
////////////////////////////

/*
 * 10 power of 2
 */
#define L1MD_BUFFER_SIZE 1000 

/*
 * 识Level行情处于未接收数居前的未知位置
 */
#define MD_NPOS -1 

using namespace std;

struct L1MDConfig 
{
	string addr;
	char ip[30];
	int port;
	char contracts_file[500];
	char yield[20]; // disruptor yield strategy
};

class L1MDProducer : public CThostFtdcMdSpi
{
	public:
		L1MDProducer(struct vrt_queue  *queue);
		~L1MDProducer();

		/*
		 * 与逻辑处理相关
		 */
		MYShfeMarketData* GetShfeData(int32_t index);
		MDBestAndDeep_MY* GetDceData(int32_t index);

		/*
		 * contract: 要获取行情的合约
		 * last_index;最新行情在缓存的位置
		 * 获取指定合约最新的一档行情。
		 * 从最新存储的行情位置向前查找，最远向前查找到前边n（主力合约个数）个元素
		 */
		CDepthMarketDataField* GetLastData(const char *contract, int32_t last_index);
		void End();

		/*
		 * 与API相关
		 */
		// 当客户端与交易托管系统建立起通信连接，客户端需要进行登录
    		virtual void OnFrontConnected();

	    // 当客户端与交易托管系统通信连接断开时，该方法被调用
	    virtual void OnFrontDisconnected(int nReason);

	    // 当客户端发出登录请求之后，该方法会被调用，通知客户端登录是否成功
	    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	    ///订阅行情应答
	    virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	    ///取消订阅行情应答
	    virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
			CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	    // 行情应答
	    virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField
			*pDepthMarketData);

	    // 针对用户请求的出错通知
	    virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
			bool bIsLast);

		// TODO: need to be modified
		void ToString(CDepthMarketDataField &data);
		bool IsDominant(const char *contract);

	private:
		/*
		 * 与API相关
		 */
		CThostFtdcMdApi *api_;
		void InitMDApi();
		
#ifdef PERSISTENCE_ENABLED 
		QuoteDataSave<MYShfeMarketData> *p_my_shfe_md_save_;
		QuoteDataSave<MDBestAndDeep_MY> *p_save_best_and_deep_;
#endif

		/*
		 * 与逻辑处理相关
		 */
		void RalaceInvalidValue_CTP(CThostFtdcDepthMarketDataField &d);
    	void Convert(const CThostFtdcDepthMarketDataField &ctp_data,CDepthMarketDataField &l1_data);
		void Convert(const CThostFtdcDepthMarketDataField &ctp_data,MDBestAndDeep_MY &data);
		CDepthMarketDataField l1md_buffer_;
		MYShfeMarketData target_data_;
		MDBestAndDeep_MY dce_data_;

		int32_t Push(const MYShfeMarketData& md);
		int32_t Push(const MDBestAndDeep_MY &md);
		struct vrt_producer  *producer_;
		// TODO: here
		MYShfeMarketData shfe_md_buffer_[L1MD_BUFFER_SIZE];
		int32_t shfe_data_cursor_;
		MDBestAndDeep_MY dce_md_buffer_[L1MD_BUFFER_SIZE];
		int32_t dce_data_cursor_;
		bool ended_;

		int32_t dominant_contract_count_;
		char dominant_contracts_[60][10];

		/*
		 *日志相关
		 */
		const char *module_name_;  

		/*
		 * 配置相关
		 */
		void ParseConfig();
		L1MDConfig config_;
};

#endif
