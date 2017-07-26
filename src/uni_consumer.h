#ifndef __UNI_CONSUMER_H__
#define __UNI_CONSUMER_H__

#include <functional>
#include <array>
#include <string>
#include <list>
#include <unordered_map>
#include <libcork/core.h>
#include <libcork/helpers/errors.h>
#include <vrt.h>
#include "vrt_value_obj.h"
#include "strategy.h"
#include "md_producer.h"
#include "tunn_rpt_producer.h"

#define STRA_TABLE_SIZE 512 
#define SIG_BUFFER_SIZE 32 

class X1FieldConverter
{
	public:
		static Convert(const signal_t& sig,const char *account, localorderid, CX1FtdcInsertOrderField& x1field)
		{
			strncpy(insert_order.AccountID, account, sizeof(TX1FtdcAccountIDType));
			insert_order.LocalOrderID = localorderid;
			strncpy(insert_order.InstrumentID, sig.symbol, sizeof(TX1FtdcInstrumentIDType));

			if (sig.sig_act == signal_act_t::buy){
				insert_order.InsertPrice = sig.buy_price;
				insert_order.BuySellType = X1_FTDC_SPD_BUY;
			}
			else if (sig.sig_act == signal_act_t::sell){
				insert_order.InsertPrice = sig.sell_price;
				insert_order.BuySellType = X1_FTDC_SPD_SELL;
			}
			else{
				// log
			}

			if (sig.sig_openclose == alloc_position_effect_t::open){
				insert_order.OrderAmount = sig.open_volume;
				insert_order.OpenCloseType = X1_FTDC_SPD_OPEN;
			}
			else if (sig.sig_openclose == alloc_position_effect_t::close){
				insert_order.OrderAmount = sig.close_volume;
				insert_order.OpenCloseType = X1_FTDC_SPD_CLOSE;
			}
			else{
				// log
			}


			if (sig.instr == instr_t::MARKET){
				insert_order.OrderType = X1_FTDC_MKORDER; 
			}
			else{
				insert_order.OrderType = X1_FTDC_LIMITORDER; 
			}

			insert_order.OrderProperty = X1_FTDC_SP_NON;
			insert_order.InsertType = X1_FTDC_BASIC_ORDER;        //委托类别,默认为普通订单
			insert_order.InstrumentType = X1FTDC_INSTRUMENT_TYPE_COMM;      //合约类型, 期货
			insert_order.Speculator = X1_FTDC_SPD_SPECULATOR;
		}
};

class UniConsumer
{
	public:
		UniConsumer(struct vrt_queue  *queue, MDProducer *md_producer,
					TunnRptProducer *tunn_rpt_producer);
		~UniConsumer();

		void start();

	private:
		bool running_;
		const std::string module_name_;  
		struct vrt_consumer *consumer_;
		MDProducer *md_producer_;
		TunnRptProducer *tunn_rpt_producer_;

		std::array<Strategy, STRA_TABLE_SIZE> stra_table_;
		// key: contract; value: indices of strategies in stra_table_
		std::unordered_multimap<std::string, int32_t> cont_straidx_map_table_;
		// key: strategy id; value: index of strategy in stra_table_
		std::unordered_map<int32_t, int32_t> straid_straidx_map_table_;

		std::list<StrategySetting> strategy_settings_;
		StrategySetting CreateStrategySetting(const TiXmlElement *ele);
		void ParseConfig();
		void CreateStrategies();

		// business logic
		void ProcBestAndDeep(int32_t index);
		void FeedBestAndDeep(int32_t straidx);
		void ProcOrderStatistic(int32_t index);
		void ProcPendingSig(int32_t index);
		void ProcSigs(Strategy &strategy, int32_t sig_cnt, signal_t *sigs);
		void ProcTunnRpt(int32_t index);
		void CancelOrder(Strategy &strategy,signal_t &sig);
		void PlaceOrder(Strategy &strategy,signal_t &sig);
		std::array<signal_t, SIG_BUFFER_SIZE> sig_buffer_;
		
};

#endif
