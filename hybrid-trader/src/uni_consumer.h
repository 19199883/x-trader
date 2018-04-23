
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
#include <tinyxml.h>
#include <tinystr.h>
#include "moduleloadlibrarylinux.h"
#include "loadlibraryproxy.h"
#include "compliance.h"
#include "l1md_producer.h"
#include "tunn_rpt_producer.h"
#include "ThostFtdcTraderApi.h"
#include "my_trade_tunnel_data_type.h"

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

class FemasFieldConverter
{
	public:
		static void InitNewOrder(Tunnconfig &cfg)
		{
			memset(&new_order_, 0, sizeof(new_order_));

			new_order_.RequestID = 0;
			strncpy(new_order_.BrokerID, cfg.brokerid.c_str(), sizeof(TThostFtdcBrokerIDType));
			strncpy(new_order_.InvestorID, cfg.investorid.c_str(), sizeof(TThostFtdcInvestorIDType));
			strncpy(new_order_.UserID, cfg.userid.c_str(), sizeof(TThostFtdcUserIDType));
			new_order_.OrderPriceType = THOST_FTDC_OPT_LimitPrice; 
			new_order_.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
			// 有效期类型
			new_order_.TimeCondition = THOST_FTDC_TC_GFD;
			// 成交量类型
			new_order_.VolumeCondition = THOST_FTDC_VC_AV;
			new_order_.ContingentCondition = THOST_FTDC_CC_Immediately;
			// 强平原因
			new_order_.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
		}

		static CThostFtdcInputOrderField* Convert(const signal_t& sig, 
					long localorderid,int32_t vol)
		{
			strncpy(new_order_.InstrumentID, sig.symbol, sizeof(TThostFtdcInstrumentIDType));
			snprintf(new_order_.OrderRef, sizeof(TThostFtdcOrderRefType), 
						"%08lld", localorderid); // 8位，左填充0，最大支持99999个信号
			//	exchange field
			/*if(MY_TNL_EC_SHFE==sig.exchange){
				strncpy(new_order_.ExchangeID, MY_TNL_EXID_SHFE, sizeof(TThostFtdcExchangeIDType));
			}else if(MY_TNL_EC_DCE==sig.exchange){
				strncpy(new_order_.ExchangeID, MY_TNL_EXID_DCE, sizeof(TThostFtdcExchangeIDType));
			}
			if(new_order_.InstrumentID[0]=='s' && new_order_.InstrumentID[1]=='c'){
				strncpy(new_order_.ExchangeID, MY_TNL_EXID_INE, sizeof(TThostFtdcExchangeIDType));
			}*/

			if (sig.sig_act == signal_act_t::buy){
				new_order_.LimitPrice = sig.buy_price;
				new_order_.Direction = THOST_FTDC_D_Buy;
			}else if (sig.sig_act == signal_act_t::sell){
				new_order_.LimitPrice = sig.sell_price;
				new_order_.Direction = THOST_FTDC_D_Sell;
			}else{
				 clog_error("[%s] do support Direction value:%d; sig id:%d", "FemasFieldConverter",
					new_order_.Direction, sig.sig_id); 
			}

			if (sig.sig_openclose == alloc_position_effect_t::open_){
				new_order_.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
			}else if (sig.sig_openclose == alloc_position_effect_t::close_){
				new_order_.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;
			}
			new_order_.VolumeTotalOriginal = vol;

			return &new_order_;
		}

		static void InitCancelOrder(Tunnconfig &cfg)
		{
			memset(&cancel_order_, 0, sizeof(cancel_order_));

			// 原报单交易所标识
			strncpy(cancel_order_.BrokerID, cfg.brokerid.c_str(), sizeof(TThostFtdcBrokerIDType));
			strncpy(cancel_order_.InvestorID, cfg.investorid.c_str(), sizeof(TThostFtdcInvestorIDType));
			strncpy(cancel_order_.UserID, cfg.userid.c_str(), sizeof(TThostFtdcUserIDType));
			// order.OrderSysID);
			cancel_order_.ActionFlag = THOST_FTDC_AF_Delete;
			cancel_order_.LimitPrice = 0;
			cancel_order_.VolumeChange = 0;
		}

		static CThostFtdcInputOrderActionField*  Convert(const char*instrument, long local_order_id,
					long ori_local_ord_id)
		{
			// TODO:
			if(instrument[0]=='s' && instrument[1]=='c'){
				strncpy(cancel_order_.ExchangeID, MY_TNL_EXID_INE, sizeof(TThostFtdcExchangeIDType));
			}else{
				strncpy(cancel_order_.ExchangeID, MY_TNL_EXID_SHFE, sizeof(TThostFtdcExchangeIDType));
			}

			// 8位，左填充0，最大支持99999个信号
			// TODO:
			//snprintf(cancel_order_.OrderActionRef,sizeof(TThostFtdcOrderRefType),"%08lld", local_order_id);
			// wangying 8位，左填充0，最大支持99999个信号
			snprintf(cancel_order_.OrderRef, sizeof(TThostFtdcOrderRefType),"%08lld", ori_local_ord_id);

			// TODO: to be done
			// strncpy(cancle_order.InstrumentID, req->stock_code, sizeof(TThostFtdcInstrumentIDType));
//			cancle_order.FrontID = org_order_info->front_id;
//			cancle_order.SessionID = org_order_info->session_id;
			//strncpy(cancle_order.ExchangeID, CTPFieldConvert::ExchCodeToExchName(exch_code),
			//		sizeof(TThostFtdcExchangeIDType));
			//CTPFieldConvert::SysOrderIDToCTPFormat(req->entrust_no, cancle_order.OrderSysID);

			return &cancel_order_;
		}

	private:
		static CThostFtdcInputOrderField new_order_;
		static CThostFtdcInputOrderActionField cancel_order_;

};

class UniConsumer
{
	public:
		UniConsumer(struct vrt_queue *queue,L1MDProducer *l1_md_producer,TunnRptProducer *tunn_rpt_producer);
		~UniConsumer();

		void Start();
		void Stop();

		// TODO:
		int ReqOrderInsertTest();

	private:
		atomic<bool> running_;
		const char* module_name_;  
		struct vrt_consumer *consumer_;
		L1MDProducer *l1_md_producer_;
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

		// key: strategy id; value: index of strategy in stra_table_
		int32_t straid_straidx_map_table_[STRA_TABLE_SIZE];

		std::list<StrategySetting> strategy_settings_;
		StrategySetting CreateStrategySetting(const TiXmlElement *ele);
		void ParseConfig();
		void CreateStrategies();
		void GetKeys(const char* contract, int &key1, int &key2);
		int32_t GetEmptyNode();

		// business logic
		void ProcShfeMarketData(int32_t index);
		void ProcDceMarketData(int32_t index);
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

