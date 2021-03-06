#ifndef __L1MD_PRODUCER_H__
#define __L1MD_PRODUCER_H__

#include <functional>
#include <sys/types.h>
#include <sys/time.h>
#include <array>
#include <string>
#include <thread>         
#include <chrono>        
#include "vrt_value_obj.h"
#include "mdclient.h"
#include "quote_datatype_level1.h"
#include "quote_cmn_utility.h"
#include "quote_cmn_save.h"
#include "efh_sf_api.h"

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
	char mcIp[30];
	int mcPort;
	char contracts_file[500];
	char yield[20];				// disruptor yield strategy
	char efh_sf_eth[20];		// EES行情服务组播
	char mcLocalIp[20];		// EES行情服务组播
	unsigned short mcLocalPort; // EES行情服务组播
	char userid[20];			// EES user id
	char password[20];			// EES password
};

class L1MDProducerHelper
{
	public:
		/*
		 * 获取指定合约的最新行情。
		 * 从行情缓存的最新位置向前查找最多查找主力合约个数Deep位置，中途找到则立即返回
		 */
		static CDepthMarketDataField* GetLastDataImp(const char *contract, int32_t last_index,
			CDepthMarketDataField *buffer, int32_t buffer_size,int32_t traverse_count);
};

#ifdef FEMAS_TOPSPEED_QUOTE
class L1MDProducer : public CMdclientSpi
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
		virtual void OnRtnDepthMarketData(CDepthMarketDataField *pDepthMarketData);

		void ToString(CDepthMarketDataField &data);

		// lic
		CDepthMarketDataField* GetLastDataForIllegaluser(const char *contract);

	private:
		/*
		 * 与API相关
		 */
		void InitMDApi();
		CMdclientApi *api_;

		/*
		 * 与逻辑处理相关
		 */
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
		char dominant_contracts_[MAX_CONTRACT_COUNT][10];
		int max_traverse_count_;
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
#endif


#ifdef EES_UDP_TOPSPEED_QUOTE
#include "EESQuoteApi.h"
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

class L1MDProducer : public EESQuoteEvent
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
		virtual void OnEqsConnected();
		virtual void OnEqsDisconnected();
		virtual void OnLoginResponse(bool bSuccess, const char* pReason);
		virtual void OnSymbolRegisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess);
		virtual void OnQuoteUpdated(EesEqsIntrumentType chInstrumentType, 
					EESMarketDepthQuoteData* pDepthQuoteData);

		void ToString(CDepthMarketDataField &data);
		//
		// lic
		CDepthMarketDataField* GetLastDataForIllegaluser(const char *contract);

	private:
		/*
		 * 与API相关
		 */
		void InitMDApi();
		EESQuoteApi *LoadQuoteApi();
		void UnloadQuoteApi();
		void *m_handle;
		EESQuoteApi *api_;
		funcDestroyEESQuoteApi m_distoryFun;

		/*
		 * 与逻辑处理相关
		 */
		void RalaceInvalidValue_EES(EESMarketDepthQuoteData &d, CDepthMarketDataField &data_dest);
		int32_t Push(const CDepthMarketDataField& md);
		struct vrt_producer  *producer_;
		CDepthMarketDataField md_buffer_[L1MD_BUFFER_SIZE];
		int32_t l1data_cursor_;
		bool ended_;

		/*
		 * check whether the given contract is dominant.
		 */
		bool IsDominant(const char *contract);
		char dominant_contracts_[MAX_CONTRACT_COUNT][10];
		int max_traverse_count_;
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
#endif


#ifdef EES_EFH_SF_TOPSPEED_QUOTE

class L1MDProducer : public EESQuoteEvent
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
		void Rev(const struct guava_udp_normal* p_quote);

		void ToString(CDepthMarketDataField &data);

		static L1MDProducer *This;

	private:
		/*
		 * 与API相关
		 */
		void InitMDApi();
		struct sl_efh_quote* p_SlEfh;
		
		/*
		 * 与逻辑处理相关
		 */
		void RalaceInvalidValue_EES(guava_udp_normal&d);
		int32_t Push(const CDepthMarketDataField& md);
		struct vrt_producer  *producer_;
		CDepthMarketDataField md_buffer_[L1MD_BUFFER_SIZE];
		int32_t l1data_cursor_;
		bool ended_;

		/*
		 * check whether the given contract is dominant.
		 */
		bool IsDominant(const char *contract);
		char dominant_contracts_[MAX_CONTRACT_COUNT][10];
		int max_traverse_count_;
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
#endif
#endif // __L1MD_PRODUCER_H__
