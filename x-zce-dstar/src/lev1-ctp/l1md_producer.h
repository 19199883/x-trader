#pragma once

#include <functional>
#include <sys/types.h>
#include <sys/time.h>
#include <array>
#include <string>
#include <thread>         
#include <chrono>        
#include "vrt_value_obj.h"
#include "quote_datatype_level1.h"
#include "quote_cmn_utility.h"
#include "quote_cmn_save.h"

#include "ThostFtdcMdApi.h"


/*
 * 10 power of 2
 */
#define L1MD_BUFFER_SIZE 32768 

/*
 * 识Level行情处于未接收数居前的未知位置
 */
#define L1MD_NPOS -1 

using namespace std;

struct L1MDConfig 
{
	char mcIp[30];
	int mcPort;
	char contracts_file[500];
	char yield[20];				// disruptor yield strategy
	char efh_sf_eth[20];		// EES行情服务组播
	char mcLocalIp[20];		// EES行情服务组播
	unsigned short mcLocalPort; // EES行情服务组播
	char userid[20];			// EES user id
	char password[20];			// EES password
	char is_multicast[20];		// CTP quote
};

class L1MDProducerHelper
{
	public:
		/*
		 * 获取指定合约的最新行情。
		 * 从行情缓存的最新位置向前查找最多查找主力合约个数Deep位置，中途找到则立即返回
		 */
		static CDepthMarketDataField* GetLastDataImp(const char *contract, 
					int32_t last_index, 
					CDepthMarketDataField *buffer, 
					int32_t buffer_size);
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
		///// added by wangying
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
		virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
		// 针对用户请求的出错通知
		virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

		void ToString(CDepthMarketDataField &data);

		// lic
		CDepthMarketDataField* GetLastDataForIllegaluser(const char *contract);

		/*
		 * check whether the given contract is dominant.
		 */
		bool IsDominant(const char *contract);
	private:
		/*
		 * 与API相关
		 */
		void InitMDApi();
		CThostFtdcMdApi *api_;
		char * pp_instruments_[700];
		bool is_multicast_;

		/*
		 * 与逻辑处理相关
		 */
		void RalaceInvalidValue_Femas(CDepthMarketDataField &d);
		int32_t Push(const CDepthMarketDataField& md);
		struct vrt_producer  *producer_;
		CDepthMarketDataField md_buffer_[L1MD_BUFFER_SIZE];
		int32_t l1data_cursor_;
		bool ended_;
		void Convert(CDepthMarketDataField &quote_level1, const CThostFtdcDepthMarketDataField &ctp_data);
		CDepthMarketDataField quote_level1_;

		char dominant_contracts_[MAX_CONTRACT_COUNT][10];
		int  contract_count_;

		QuoteDataSave<CDepthMarketDataField> *p_level1_save_;

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


