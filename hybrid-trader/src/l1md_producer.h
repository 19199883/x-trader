#ifndef __L1MD_PRODUCER_H__
#define __L1MD_PRODUCER_H__

#include <functional>
#include <array>
#include <string>
#include <thread>         
#include <chrono>        
#include "vrt_value_obj.h"
#include "mdclient.h"
#include "quote_datatype_level1.h"

///////////////////
// TODO: new
#include <string>
#include <sstream>
#include <list>
#include "ThostFtdcMdApi.h"

#include <boost/function.hpp>

#include "quote_interface_level1.h"
#include "quote_datatype_level1.h"
#include "quote_cmn_utility.h"
#include "quote_cmn_config.h"
#include "quote_cmn_save.h"
////////////////////////////

/*
 * 10 power of 2
 */
#define L1MD_BUFFER_SIZE 1000 

/*
 * 识Level行情处于未接收数居前的未知位置
 */
#define L1MD_NPOS -1 

using namespace std;

struct L1MDConfig 
{
	string addr;
	char ip[30];
	int port;
	char contracts_file[500];
	char yield[20]; // disruptor yield strategy
};

class L1MDProducerHelper
{
	public:
		/*
		 * 获取指定合约的最新行情。
		 * 从行情缓存的最新位置向前查找最多查找主力合约个数Deep位置，中途找到则立即返回
		 */
		static CDepthMarketDataField* GetLastDataImp(const char *contract, int32_t last_index,
			CDepthMarketDataField *buffer, int32_t buffer_size, int32_t dominant_contract_count);
};

class L1MDProducer : public CThostFtdcMdSpi
{
	public:
		L1MDProducer(struct vrt_queue  *queue);
		~L1MDProducer();

		/*
		 * 与逻辑处理相关
		 */
		CDepthMarketDataField* GetData(int32_t index);

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
	    virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	    ///取消订阅行情应答
	    virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	    // 行情应答
	    virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField
		*pDepthMarketData);

	    // 针对用户请求的出错通知
	    virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
		bool bIsLast);

		// TODO: need to be modified
		virtual void OnRtnDepthMarketData(CDepthMarketDataField *pDepthMarketData);
		void ToString(CDepthMarketDataField &data);

	private:
		/*
		 * 与API相关
		 */
		CThostFtdcMdApi *api_;
    		char ** pp_instruments_;
    		std::string instruments_;
		void InitMDApi();
		
		// save assistant object
	    	QuoteDataSave<CDepthMarketDataField> *p_save_;


		/*
		 * 与逻辑处理相关
		 */
		// TODO: new		
		void RalaceInvalidValue_CTP(CThostFtdcDepthMarketDataField &d);
    		CDepthMarketDataField Convert(const CThostFtdcDepthMarketDataField &ctp_data);

		void RalaceInvalidValue_Femas(CDepthMarketDataField &d);
		int32_t Push(const CDepthMarketDataField& md);
		struct vrt_producer  *producer_;
		CDepthMarketDataField md_buffer_[L1MD_BUFFER_SIZE];
		int32_t l1data_cursor_;
		bool ended_;

		/*
		 * check whether the given contract is dominant.
		 */
		bool IsDominant(const char *contract);
		int32_t dominant_contract_count_;
		char dominant_contracts_[20][10];

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
