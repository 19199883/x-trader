
#ifndef __UNI_CONSUMER_H__
#define __UNI_CONSUMER_H__

#include <functional>
#include <array>
#include <string>
#include <list>
#include <unordered_map>
#include "vrt_value_obj.h"
#include "strategy.h"
#include "md_producer.h"
#include "tunn_rpt_producer.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "moduleloadlibrarylinux.h"
#include "loadlibraryproxy.h"
#include "compliance.h"

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

class X1FieldConverter
{
	public:
		static void InitNewOrder(const char *account)
		{
			memset(&new_order_, 0, sizeof(CX1FtdcInsertOrderField));

			strncpy(new_order_.AccountID, account, sizeof(TX1FtdcAccountIDType));
			new_order_.OrderType = X1_FTDC_LIMITORDER; 
			new_order_.OrderProperty = X1_FTDC_SP_NON;
			new_order_.InsertType = X1_FTDC_BASIC_ORDER;        //委托类别,默认为普通订单
			new_order_.InstrumentType = X1FTDC_INSTRUMENT_TYPE_COMM;      //合约类型, 期货
			new_order_.Speculator = X1_FTDC_SPD_SPECULATOR;
		}

		static CX1FtdcInsertOrderField*  Convert(const signal_t& sig,long localorderid,int32_t vol)
		{
			new_order_.LocalOrderID = localorderid;
			new_order_.RequestID = localorderid;
			strncpy(new_order_.InstrumentID, sig.symbol, sizeof(TX1FtdcInstrumentIDType));

			if (sig.sig_act == signal_act_t::buy){
				new_order_.InsertPrice = sig.buy_price;
				new_order_.BuySellType = X1_FTDC_SPD_BUY;
			}else if (sig.sig_act == signal_act_t::sell){
				new_order_.InsertPrice = sig.sell_price;
				new_order_.BuySellType = X1_FTDC_SPD_SELL;
			}else{
				 clog_warning("[%s] do support BuySellType value:%d; sig id:%d", "X1FieldConverter",
					new_order_.BuySellType, sig.sig_id); 
			}

			if (sig.sig_openclose == alloc_position_effect_t::open_){
				new_order_.OpenCloseType = X1_FTDC_SPD_OPEN;
			}else if (sig.sig_openclose == alloc_position_effect_t::close_){
				new_order_.OpenCloseType = X1_FTDC_SPD_CLOSE;
			}else{
				clog_warning("[%s] do support sig_openclose value:%d; sig id:%d", "X1FieldConverter",
				sig.sig_openclose, sig.sig_id); 
			}

			new_order_.OrderAmount = vol;

			return &new_order_;
		}

	private:
		static CX1FtdcInsertOrderField new_order_;
};

class UniConsumer
{
	public:
		UniConsumer(struct vrt_queue  *queue, MDProducer *md_producer,
					TunnRptProducer *tunn_rpt_producer);
		~UniConsumer();

		void Start();
		void Stop();

	private:
		bool running_;
		const char* module_name_;  
		struct vrt_consumer *consumer_;
		MDProducer *md_producer_;
		TunnRptProducer *tunn_rpt_producer_;
		CLoadLibraryProxy *pproxy_;
		int32_t strategy_counter_;

		Strategy stra_table_[STRA_TABLE_SIZE];

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
		void ProcBestAndDeep(int32_t index);
		void FeedBestAndDeep(int32_t straidx);
		void ProcOrderStatistic(int32_t index);
		void ProcSigs(Strategy &strategy, int32_t sig_cnt, signal_t *sigs);
		void ProcTunnRpt(int32_t index);
		void CancelOrder(Strategy &strategy,signal_t &sig);
		void PlaceOrder(Strategy &strategy, const signal_t &sig);
		bool CancelPendingSig(Strategy &strategy, int32_t ori_sigid);
		signal_t sig_buffer_[SIG_BUFFER_SIZE];
		Uniconfig config_;

		/*
		 *		 * pending_signals_[st_id][n]:pending_signals_[st_id]存储st_id
		 *				 * 策略待处理的信号的信号id。-1表示无效
		 *						 *
		 *								 */
		int32_t pending_signals_[STRA_TABLE_SIZE][MAX_PENDING_SIGNAL_COUNT];

#ifdef COMPLIANCE_CHECK
		Compliance compliance_;
#endif
		
};

#endif

