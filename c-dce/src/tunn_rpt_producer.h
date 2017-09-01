#ifndef __TUNN_RPT_PRODUCER_H__
#define __TUNN_RPT_PRODUCER_H__

#include <array>
#include <string>
#include "vrt_value_obj.h"
#include "my_trade_tunnel_api.h"

using namespace std;

// need to be changed
// 集成Myexchange

/*
 * 10 power of 2
 */
#define RPT_BUFFER_SIZE 4096

using namespace std;

struct TunnRpt
{
	TX1FtdcLocalOrderIDType               LocalOrderID;                 ///< 本地委托号
	TX1FtdcOrderAnswerStatusType          OrderStatus;                  ///< 委托状态
	TX1FtdcAmountType                     MatchedAmount;                ///< 成交数量
	TX1FtdcPriceType                      MatchedPrice;                 ///< 成交价格
	TX1FtdcErrorIDType                    ErrorID;                      ///< 错误ID
	TX1FtdcAmountType                     CancelAmount;                 ///< 撤单数量
};

class TunnRptProducer
{
	public:
		TunnRptProducer(struct vrt_queue  *queue);
		~TunnRptProducer();

		/*
		 * things relating to X1 Api
		 */
		// 下发指令接口
		int ReqOrderInsert(CX1FtdcInsertOrderField *p);
		// 撤单操作请求
		int ReqOrderAction(CX1FtdcCancelOrderField *p);

		/*
		 * things relating to x-trader internal logic
		 */
		long NewLocalOrderID(int32_t strategyid);
		const char* GetAccount();
		TunnRpt* GetRpt(int32_t index);
		int32_t GetStrategyID(TunnRpt& rpt);

		void End();

	private:
		/*
		 * things relating to MyExchange
		 */
		MYExchangeInterface *channel;

		//委托下单响应
		virtual void OnRspInsertOrder(struct CX1FtdcRspOperOrderField *pf, struct CX1FtdcRspErrorField *pe);
		//委托撤单响应
		virtual void OnRspCancelOrder(struct CX1FtdcRspOperOrderField *pf, struct CX1FtdcRspErrorField *pe);
		//错误回报
		virtual void OnRtnErrorMsg(struct CX1FtdcRspErrorField *pf);
		//成交回报
		virtual void OnRtnMatchedInfo(struct CX1FtdcRspPriMatchInfoField *pf);
		//委托回报
		virtual void OnRtnOrder(struct CX1FtdcRspPriOrderField *pf);
		//撤单回报
		virtual void OnRtnCancelOrder(struct CX1FtdcRspPriCancelOrderField *pf);

private:
    void ParseConfig();
    void ReqLogin();

	struct vrt_producer  *producer_;
	std::array<TunnRpt, RPT_BUFFER_SIZE> rpt_buffer_;
	my_xchg_cfg config_;
	const char * module_name_;  
	bool ended_;

	/*
	 * things relating to counter API
	 */
	int32_t Push(const TunnRpt& rpt);

};

#endif
