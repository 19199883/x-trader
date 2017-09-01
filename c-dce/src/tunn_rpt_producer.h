#ifndef __TUNN_RPT_PRODUCER_H__
#define __TUNN_RPT_PRODUCER_H__

#include <array>
#include <string>
#include "vrt_value_obj.h"
#include "my_trade_tunnel_api.h"

/*
 * 10 power of 2
 */
#define RPT_BUFFER_SIZE 4096

using namespace std;

struct TunnRpt
{
	SerialNoType			LocalOrderID;                 ///< 本地委托号
	TMyTnlOrderEntrustType	OrderStatus;                  ///< 委托状态
	VolumeType				MatchedAmount;                ///< 成交数量
	double                  MatchedPrice;                 ///< 成交价格
	int32_t                 ErrorID;                      ///< 错误ID
	VolumeType				CancelAmount;                 ///< 撤单数量
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
		int ReqOrderInsert(const T_PlaceOrder *pReq);
		// 撤单操作请求
		int ReqOrderAction(const T_CancelOrder *pReq);

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
		MYExchangeInterface *api_;
		//委托下单响应
		void OnRspInsertOrder(const T_OrderRespond *ord_res, const T_PositionData *posstruct);
		//委托撤单响应
		void OnRspCancelOrder(const T_CancelRespond *canel_res);
		//委托回报
		void OnRtnOrder(const T_OrderReturn *ord_rtn, const T_PositionData * pos);
		//成交回报
		void OnRtnMatchedInfo(const T_TradeReturn *trade_rtn, const T_PositionData *pos);

private:
    void ParseConfig();
	int32_t Push(const TunnRpt& rpt);

	struct vrt_producer  *producer_;
	std::array<TunnRpt, RPT_BUFFER_SIZE> rpt_buffer_;
	my_xchg_cfg config_;
	const char * module_name_;  
	bool ended_;
};

#endif
