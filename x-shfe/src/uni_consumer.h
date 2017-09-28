
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
		static void Convert(Tunnconfig &cfg, const signal_t& sig, long localorderid, 
					int32_t vol, CUstpFtdcInputOrderField& insert_order)
		{
			memset(&insert_order, 0, sizeof(insert_order));
			strncpy(insert_order.BrokerID, cfg.brokerid.c_str(), 
						sizeof(TUstpFtdcBrokerIDType));
			strncpy(insert_order.ExchangeID, MY_TNL_EXID_SHFE, sizeof(TUstpFtdcExchangeIDType));
			//strcpy(insert_order.OrderSysID, "");//系统报单编号，填什么内容?
			strncpy(insert_order.InvestorID, cfg.userid.c_str(), sizeof(TUstpFtdcInvestorIDType));
			strncpy(insert_order.UserID, cfg.userid.c_str(), sizeof(TUstpFtdcUserIDType));
			strncpy(insert_order.InstrumentID, sig.symbol, sizeof(TUstpFtdcInstrumentIDType));
			// 8位，左填充0，最大支持99999个信号
			snprintf(insert_order.UserOrderLocalID, sizeof(TUstpFtdcUserOrderLocalIDType), 
						"%08lld", localorderid);
			if (sig.instr == instr_t::MARKET){
				insert_order.OrderPriceType = USTP_FTDC_OPT_AnyPrice; 
			}
			else{
				insert_order.OrderPriceType = USTP_FTDC_OPT_LimitPrice; 
			}

			if (sig.sig_act == signal_act_t::buy){
				insert_order.LimitPrice = sig.buy_price;
				insert_order.Direction = USTP_FTDC_D_Buy;
			}
			else if (sig.sig_act == signal_act_t::sell){
				insert_order.LimitPrice = sig.sell_price;
				insert_order.Direction = USTP_FTDC_D_Sell;
			}else{
				 clog_warning("[%s] do support Direction value:%d; sig id:%d", "FemasFieldConverter",
					insert_order.Direction, sig.sig_id); 
			}

			if (sig.sig_openclose == alloc_position_effect_t::open_){
				insert_order.OffsetFlag = USTP_FTDC_OF_Open;
			}
			else if (sig.sig_openclose == alloc_position_effect_t::close_){
				insert_order.OffsetFlag = USTP_FTDC_OF_CloseToday;
			}
			insert_order.HedgeFlag = USTP_FTDC_CHF_Speculation;
			insert_order.Volume = vol;
			// 有效期类型
			insert_order.TimeCondition = USTP_FTDC_TC_GFD;
			// GTD日期
			//strcpy(insert_order.GTDDate, "");
			// 成交量类型
			insert_order.VolumeCondition = USTP_FTDC_VC_AV;
			// 最小成交量
			//insert_order.MinVolume = 0;
			// 止损价
			//insert_order.StopPrice = 0;
			// 强平原因
			insert_order.ForceCloseReason = USTP_FTDC_FCR_NotForceClose;
			// 自动挂起标志
			//insert_order.IsAutoSuspend = 0;
			//strcpy(insert_order.BusinessUnit,"");
			strncpy(insert_order.UserCustom, cfg.userid.c_str(), sizeof(TUstpFtdcCustomType));
		}

		static void Convert(Tunnconfig &cfg, long local_order_id, 
					long ori_local_ord_id, CUstpFtdcOrderActionField &order)
		{
			memset(&order, 0, sizeof(order));

			// 原报单交易所标识
			strncpy(order.ExchangeID, MY_TNL_EXID_SHFE, sizeof(TUstpFtdcExchangeIDType));
			strncpy(order.BrokerID, cfg.brokerid.c_str(), sizeof(TUstpFtdcBrokerIDType));
			strncpy(order.InvestorID, cfg.userid.c_str(), sizeof(TUstpFtdcInvestorIDType));
			strncpy(order.UserID, cfg.userid.c_str(), sizeof(TUstpFtdcUserIDType));
			// 8位，左填充0，最大支持99999个信号
			snprintf(order.UserOrderActionLocalID, sizeof(order.UserOrderActionLocalID), 
						"%08lld", local_order_id);
			// wangying 8位，左填充0，最大支持99999个信号
			snprintf(order.UserOrderLocalID, sizeof(order.UserOrderLocalID), 
						"%08lld", ori_local_ord_id);
			// order.OrderSysID);
			order.ActionFlag = USTP_FTDC_AF_Delete;
			order.LimitPrice = 0;
			order.VolumeChange = 0;
		}

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
		void ProcShfeMarketData(int32_t index);
		void ProcSigs(Strategy &strategy, int32_t sig_cnt, signal_t *sigs);
		void ProcTunnRpt(int32_t index);
		void CancelOrder(Strategy &strategy,signal_t &sig);
		void PlaceOrder(Strategy &strategy, const signal_t &sig);
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

