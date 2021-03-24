#pragma once

#include <functional>
#include <sys/types.h>
#include <sys/time.h>
#include <array>
#include <string>
#include <thread>         
#include <chrono>        
#include "vrt_value_obj.h"
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
	char is_multicast[20];		// CTP quote
};

class Lev1Producer : public CThostFtdcMdSpi
{
	public:
		Lev1Producer(struct vrt_queue  *queue);
		~Lev1Producer();

		/*
		 * 与逻辑处理相关
		 */
		CThostFtdcDepthMarketDataField* GetData(int32_t index);

		void End();

		void Start();

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

		void ToString(CThostFtdcDepthMarketDataField *data);

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
		void ReplaceInvalidValue(CThostFtdcDepthMarketDataField *data);
		int32_t Push(const CThostFtdcDepthMarketDataField & md);
		struct vrt_producer  *producer_;
		CThostFtdcDepthMarketDataField md_buffer_[L1MD_BUFFER_SIZE];
		int32_t l1data_cursor_;
		bool ended_;

		char dominant_contracts_[MAX_CONTRACT_COUNT][10];
		int  contract_count_;

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


