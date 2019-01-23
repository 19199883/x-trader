
#ifndef __UNI_CONSUMER_H__
#define __UNI_CONSUMER_H__

#include <functional>
#include <array>
#include <atomic>         // std::atomic
#include <string>
#include <list>
#include <unordered_map>
#include "vrt_value_obj.h"
#include "strategy.h"
#include "tunn_rpt_producer.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "moduleloadlibrarylinux.h"
#include "loadlibraryproxy.h"
#include "compliance.h"
#include "dmd_producer.h"

/*
 * 最多支持策略数量
 */
#define STRA_TABLE_SIZE 100 
#define SIG_BUFFER_SIZE 32 

// key2: stoi(年月)，如1801
#define MAX_STRATEGY_KEY2 3000 

// 品种字符相加：如jd1801，key1: j+d(ascii 值相加
#define MAX_STRATEGY_KEY1 500

// 允许的最大缓存的待处理信号数量
#define MAX_PENDING_SIGNAL_COUNT 10

struct Uniconfig
{
	// disruptor yield strategy
	char yield[20];
};

class EESFieldConverter
{
	public:
		static void InitNewOrder(Tunnconfig &cfg)
		{
			memset(&new_order_, 0, sizeof(new_order_));

			strncpy(new_order_.m_Account, cfg.investorid.c_str(), sizeof(EES_Account));
			new_order_.m_SecType = EES_SecType_fut; 
			new_order_.m_HedgeFlag = EES_HedgeFlag_Speculation;
			new_order_.m_Tif = EES_OrderTif_Day;
		}

		static EES_EnterOrderField * Convert(const signal_t& sig, 
					long localorderid,int32_t vol)
		{
			strncpy(new_order_.m_Symbol, sig.symbol, sizeof(EES_Symbol));
			new_order_.m_ClientOrderToken = localorderid; 
			if(new_order_.m_Symbol[0]=='s' && new_order_.m_Symbol[1]=='c'){
				new_order_.m_Exchange = EES_ExchangeID_ine;
			}else{
				new_order_.m_Exchange = EES_ExchangeID_shfe;
			}
			new_order_.m_Qty = vol;


			if (sig.sig_act == signal_act_t::buy &&
				sig.sig_openclose == alloc_position_effect_t::open_	){
				new_order_.m_Price = sig.buy_price;
				new_order_.m_Side = EES_SideType_open_long;
			}
			if (sig.sig_act == signal_act_t::buy &&
				sig.sig_openclose == alloc_position_effect_t::close_){
				new_order_.m_Price = sig.buy_price;
				new_order_.m_Side = EES_SideType_close_today_short;
			}
			if (sig.sig_act == signal_act_t::sell &&
				sig.sig_openclose == alloc_position_effect_t::open_	){
				new_order_.m_Price = sig.sell_price;
				new_order_.m_Side = EES_SideType_open_short;
			}
			if (sig.sig_act == signal_act_t::sell &&
				sig.sig_openclose == alloc_position_effect_t::close_){
				new_order_.m_Price = sig.sell_price;
				new_order_.m_Side = EES_SideType_close_today_long;
			}
			
			return &new_order_;
		}

		static void InitCancelOrder(Tunnconfig &cfg)
		{
			memset(&cancel_order_, 0, sizeof(cancel_order_));
			strncpy(cancel_order_.m_Account, cfg.investorid.c_str(), sizeof(EES_Account));
		}

		static EES_CancelOrder*  Convert(long ori_sysorderid)
		{
			cancel_order_.m_MarketOrderToken = ori_sysorderid;
			return &cancel_order_;
		}

	private:
		static EES_EnterOrderField new_order_;
		static EES_CancelOrder cancel_order_;

};

class UniConsumer
{
	public:
		UniConsumer(struct vrt_queue  *queue, depthMarketData *md_producer, 
			TunnRptProducer *tunn_rpt_producer);
		~UniConsumer();

		void Start();
		void Stop();

	private:
		atomic<bool> running_;
		const char* module_name_;  
		struct vrt_consumer *consumer_;
		DepthMDProducer *md_producer_;
		TunnRptProducer *tunn_rpt_producer_;
		CLoadLibraryProxy *pproxy_;
		int32_t strategy_counter_;

		Strategy stra_table_[STRA_TABLE_SIZE];

		// strategy log
		void FlushStrategyLog();
		void WriteLogTitle(FILE * pfDayLogFile);
		void WriteLogImp();
		void WriteOne(FILE *pfDayLogFile, struct strat_out_log *pstratlog);
		void WriteStrategyLog(Strategy &strategy);
		std::atomic_flag lock_log_;
		vector<strat_out_log> log_w_;
		std::thread *thread_log_;
		int32_t log_write_count_;
		FILE * pfDayLogFile_;
				
		// lic
		bool check_lic();
		bool legal_;

#if FIND_STRATEGIES == 1
		// unordered_multimap  key: contract; value: indices of strategies in stra_table_
		std::unordered_multimap<std::string, int32_t> cont_straidx_map_table_;
#endif

#if FIND_STRATEGIES == 2
		// two-dimensional array
		int32_t stra_idx_table_[STRA_TABLE_SIZE][STRA_TABLE_SIZE];
		int32_t cont_straidx_map_table_[MAX_STRATEGY_KEY1][MAX_STRATEGY_KEY2];
#endif

		// key: strategy id; value: index of strategy in stra_table_
		int32_t straid_straidx_map_table_[STRA_TABLE_SIZE];

		std::list<StrategySetting> strategy_settings_;
		StrategySetting CreateStrategySetting(const TiXmlElement *ele);
		void ParseConfig();
		void CreateStrategies();
		void GetKeys(const char* contract, int &key1, int &key2);
		int32_t GetEmptyNode();

		// business logic
		void ProcShfeMarketData(depthMarketData* md);
		void ProcSigs(Strategy &strategy, int32_t sig_cnt, signal_t *sigs);
		void ProcTunnRpt(int32_t index);
		void CancelOrder(Strategy &strategy,signal_t &sig);
		void PlaceOrder(Strategy &strategy, const signal_t &sig);
		bool CancelPendingSig(Strategy &strategy, int32_t ori_sigid);
		signal_t sig_buffer_[SIG_BUFFER_SIZE];
		Uniconfig config_;

		/*
		 * pending_signals_[st_id][n]:pending_signals_[st_id]存储st_id
		 * 策略待处理的信号的信号id。-1表示无效
		 *
		 */
		int32_t pending_signals_[STRA_TABLE_SIZE][MAX_PENDING_SIGNAL_COUNT];

#ifdef COMPLIANCE_CHECK
		Compliance compliance_;
#endif
		
};

#endif

