/* 
 * 1.设置要调试的策略so：修改main函数中的sofile变量，并把so放到执行文件所在目录
 * 2. 设置策略交易的合约，手数：修改main函数中的contract，position变量
 * 3. 设置行情内容：修改InitData函数的md变量
 * 4. 设置通道回报内容：修改tunn_rpt函数的md变量
 * 5. 设置ev文件：修改main函数中的ev变量，并把ev放到相应目录，如：例子中，需放到ev子目录中
 * 6. 策略日志：策略日志会存放在log子目录中
 *
 *
 *
 *
 *
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <dlfcn.h>
#include <string>
#include "strategy.h"
#include "st_signal.h"
#include "quote_datatype_czce_level2.h"
#include "TapTradeAPIDataType.h"

using namespace std;

#define SIG_BUFFER_SIZE 100

// 以根据调试需要修改状态，成交量等
// 态定义，见TapTradeAPIDataType.h
//
struct TunnRpt tunn_rpt;
ZCEL2QuotSnapshotField_MY md;

void InitData()
{
	tunn_rpt.SessionID		 = 0;
	tunn_rpt.LocalOrderID	 = 1;								///< 本地委托号
	tunn_rpt.OrderStatus	 = TAPI_ORDER_STATE_PARTFINISHED;    ///< 委托状态
	tunn_rpt.MatchedAmount	 = 10;								///< 成交数量
	tunn_rpt.OrderMatchPrice = 12;								///< 成交价1
	tunn_rpt.ErrorID		 = 1;								///< 错误ID
	tunn_rpt.ServerFlag		 = 0;								///< 服务器标识
	//tunn_rpt.OrderNo		 = 1;								///< 委托编码

	// 情数据，可以根据调试需要，修改相应的字段值
	md.PreSettle = 10.99;       /*前结算价格*/
	md.PreClose = 3;        /*前收盘价格*/
	md.PreOpenInterest = 9;    /*昨日空盘量*/
	md.OpenPrice = 3;       /*开盘价*/
	md.HighPrice= 9;       /*最高价*/
	md.LowPrice =4;        /*最低价*/
	md.LastPrice = 33;       /*最新价*/
	md.BidPrice[0] = 8;     /*买入价格 下标从0开始*/
	md.BidPrice[1] = 8;     /*买入价格 下标从0开始*/
	md.BidPrice[2] = 8;     /*买入价格 下标从0开始*/
	md.BidPrice[3] = 8;     /*买入价格 下标从0开始*/
	md.BidPrice[4] = 8;     /*买入价格 下标从0开始*/
	md.AskPrice[0] = 4;     /*卖出价 下标从0开始*/
	md.AskPrice[1] = 4;     /*卖出价 下标从0开始*/
	md.AskPrice[2] = 4;     /*卖出价 下标从0开始*/
	md.AskPrice[3] = 4;     /*卖出价 下标从0开始*/
	md.AskPrice[4] = 4;     /*卖出价 下标从0开始*/
	md.BidLot[0] = 4;          /*买入数量 下标从0开始*/
	md.BidLot[1] = 4;          /*买入数量 下标从0开始*/
	md.BidLot[2] = 4;          /*买入数量 下标从0开始*/
	md.BidLot[3] = 4;          /*买入数量 下标从0开始*/
	md.BidLot[4] = 4;          /*买入数量 下标从0开始*/
	md.AskLot[0] = 4;          /*卖出数量 下标从0开始*/
	md.AskLot[1] = 4;          /*卖出数量 下标从0开始*/
	md.AskLot[2] = 4;          /*卖出数量 下标从0开始*/
	md.AskLot[3] = 4;          /*卖出数量 下标从0开始*/
	md.AskLot[4] = 4;          /*卖出数量 下标从0开始*/
	md.TotalVolume = 8;        /*总成交量*/
	md.OpenInterest = 444;       /*持仓量*/
	md.ClosePrice = 8.9;      /*收盘价*/
	md. SettlePrice = 9.7;     /*结算价*/
	md.AveragePrice = 9.6;    /*均价*/
	md.LifeHigh = 7.9;        /*历史最高成交价格*/
	md.LifeLow = 3.5;         /*历史最低成交价格*/
	md.HighLimit = 44.3;       /*涨停板*/
	md.LowLimit = 8;        /*跌停板*/
	md.TotalBidLot = 30;        /*委买总量*/
	md.TotalAskLot = 33;        /*委卖总量*/
	strcpy(md.TimeStamp, "2014-02-03 13:23:45");		//时间：如2014-02-03 13:23:45
	strcpy(md.ContractID, "SR801");    /*合约编码*/
}

void CreateStrategySetting(StrategySetting &setting, string &sofile, 
	string &contract, int position, string &ev)
{
	setting.config.st_id = 1;
	setting.file = sofile;
	setting.config.log_id = setting.config.st_id *10 + 0;
	strcpy(setting.config.log_name, "log");
	setting.config.iv_id = setting.config.st_id *10 + 1;
	strcpy(setting.config.iv_name, "iv_name");
	setting.config.ev_id = setting.config.st_id *10 + 2;
	strcpy(setting.config.ev_name, ev.c_str());

	setting.config.symbols_cnt = 1;
	symbol_t &tmp = setting.config.symbols[0];
	tmp.max_pos = position;
	strcpy(tmp.name, contract.c_str());
	tmp.symbol_log_id = 10000;
}


int main(/*int argc, const char **argv*/)
{
	InitData();
	// 以根据调试需要，修改合约，仓位等值
	string sofile = "st301t_zzsm_day";	// 策略so文件名，不包括扩展名
	string contract = "SR805";		// 合约	
	int32_t position = 10;			// 手数
	string ev = "ev/ev.txt";		// 

	// 始化策略设置
	StrategySetting setting;
	CreateStrategySetting(setting, sofile, contract, position, ev);
	// 加载策略
	CLoadLibraryProxy *pproxy = CLoadLibraryProxy::CreateLoadLibraryProxy();
	pproxy->setModuleLoadLibrary(new CModuleLoadLibraryLinux());
	pproxy->setBasePathLibrary(pproxy->getexedir());

	signal_t sig_buffer[SIG_BUFFER_SIZE];
	Strategy strategy;
	int sig_cnt = 0;
	// 初始化策略
	strategy.Init(setting, pproxy);
	// 推送行情
	strategy.FeedMd(&md, &sig_cnt, sig_buffer);
	// 推送回报
	strategy.FeedTunnRpt(tunn_rpt, &sig_cnt, sig_buffer);

	return 0;
}



