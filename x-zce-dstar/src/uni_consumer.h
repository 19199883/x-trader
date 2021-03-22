
#ifndef __UNI_CONSUMER_H__
#define __UNI_CONSUMER_H__

#include <functional>
#include <array>
#include <string>
#include <list>
#include <unordered_map>
#include "vrt_value_obj.h"
#include "strategy.h"
#include "lev1_producer.h"
#include "l2md_producer.h"
#include "tunn_rpt_producer.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "moduleloadlibrarylinux.h"
#include "loadlibraryproxy.h"
#include "compliance.h"

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

class UniConsumer
{
	public:
		UniConsumer(struct vrt_queue  *queue, 
					Lev1Producer *l1md_producer, 
					L2MDProducer *l2md_producer, 
					TunnRptProducer *tunn_rpt_producer);
		~UniConsumer();

		void Start();
		void Stop();

	private:
		// lic
		bool check_lic();
		bool legal_;

		bool running_;
		const char* module_name_;  
		struct vrt_consumer *consumer_;
		L2MDProducer *l2_md_producer_;
		Lev1Producer *l1_md_producer_;
		TunnRptProducer *tunn_rpt_producer_;
		CLoadLibraryProxy *pproxy_;
		int32_t strategy_counter_;

		Strategy stra_table_[MAX_STRATEGY_COUNT];

		// key: strategy id; value: index of strategy in stra_table_
		int32_t straid_straidx_map_table_[MAX_STRATEGY_COUNT];

		std::list<StrategySetting> strategy_settings_;
		StrategySetting CreateStrategySetting(const TiXmlElement *ele);
		void ParseConfig();
		void CreateStrategies();
		void GetKeys(const char* contract, int &key1, int &key2);
		int32_t GetEmptyNode();

		// business logic
		void ProcL2QuoteSnapshot(ZCEL2QuotSnapshotField_MY* md);
		void FeedL2QuoteSnapshot(int32_t straidx);
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
		int32_t pending_signals_[MAX_STRATEGY_COUNT][MAX_PENDING_SIGNAL_COUNT];

		/*
		 * 1.缓存所有的委托报告信息，用于撤单使用。
		 * 访问一个委托单的报告，通过该委托单的LocalOrderID的counter作为数组下标
		 */
		TunnRpt tunnrpt_table_[RPT_BUFFER_SIZE];

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

		Compliance compliance_;
		
};

#endif

