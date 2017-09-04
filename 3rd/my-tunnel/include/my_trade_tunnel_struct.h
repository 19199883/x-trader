// 茂源交易通道，业务数据结构

#ifndef _MY_TRADE_TUNNEL_STRUCT_H_
#define _MY_TRADE_TUNNEL_STRUCT_H_

#include "my_trade_tunnel_data_type.h"
#include <string>
#include <vector>

/////////////////////////////////////////////////////////////////////////
/// 期货 报单/撤单，请求/回报 相关数据结构
/////////////////////////////////////////////////////////////////////////

/// 下单请求结构
struct DLL_PUBLIC T_PlaceOrder
{
	SerialNoType serial_no;   				// 报单请求序列号
	StockCodeType stock_code;  				// 合约代码
	double limit_price; 					// 委托价格
	TMyTnlDirectionType direction;   		// 买卖方向
	TMyTnlOffsetType open_close;  			// 开平标识
	TMyTnlHedgeFlagType speculator;  		// 投机套保标志
	VolumeType volume;      				// 手数
	TMyTnlOrderPriceType order_kind;  		// 委托价格属性
	TMyTnlOrderEntrustType order_type;  	// 报单委托类型

	// Improvement
	char exchange_type;						// 交易所编码
};

/// 下单的响应回报结构
struct DLL_PUBLIC T_OrderRespond
{
	SerialNoType serial_no;  				// 报单请求序列号
	int error_no;							// 错误号，0：正常；其它：异常
	EntrustNoType entrust_no;				// 交易所委托号
	TMyTnlOrderStatusType entrust_status;	// 委托状态

	// Improvement
	char exchange_type;
};

/// 撤单请求结构
struct DLL_PUBLIC T_CancelOrder
{
	SerialNoType serial_no;					// 撤单请求序列号
	StockCodeType stock_code;				// 合约代码
	TMyTnlDirectionType direction;			// 买卖方向
	TMyTnlOffsetType open_close;			// 开平标识
	TMyTnlHedgeFlagType speculator;			// 投机套保标志
	VolumeType volume;						// 报单量
	double limit_price;						// 报单价格
	EntrustNoType entrust_no;				// 待撤单的交易所委托号
	SerialNoType org_serial_no;				// 待撤报单的序列号

	// Improvement
	char exchange_type;						// 交易所编码
};

/// 撤单响应回报结构
struct DLL_PUBLIC T_CancelRespond
{
	SerialNoType serial_no;					// 撤单请求序列号
	int error_no;							// 错误号，0：正常；其它：异常
	EntrustNoType entrust_no;				// 交易所委托号
	TMyTnlOrderStatusType entrust_status;	// 撤单操作状态（已报、拒绝，只能为这两种状态中的一个）
};

/// 报单状态回报结构
struct DLL_PUBLIC T_OrderReturn
{
	SerialNoType serial_no;					// 报单请求序列号
	StockCodeType stock_code;				// 合约代码
	EntrustNoType entrust_no;				// 交易所委托号
	TMyTnlOrderStatusType entrust_status;	// 委托状态
	TMyTnlDirectionType direction;			// 买卖方向
	TMyTnlOffsetType open_close;			// 开平标识
	TMyTnlHedgeFlagType speculator;			// 投机套保标志
	VolumeType volume;						// 报单的原始手数
	double limit_price;						// 委托价格

	// added for myexchange
	VolumeType volume_remain;				// 剩余未成交的手数

	// Improvement
	char exchange_type;						// 交易所编码
};

/// 成交回报结构
struct DLL_PUBLIC T_TradeReturn
{
	SerialNoType serial_no;					// 报单请求序列号
	EntrustNoType entrust_no;				// 交易所委托号
	int business_volume;					// 成交数据
	double business_price;					// 成交价格
	int business_no;						// 成交序号

	StockCodeType stock_code;				// 合约代码
	TMyTnlDirectionType direction;			// 买卖方向
	TMyTnlOffsetType open_close;			// 开平标识

	// Improvement
	char exchange_type;						// 交易所编码
};

/// 仓位回报结构 （和报单/成交回报一起返回）
struct DLL_PUBLIC T_PositionData
{
	mutable int update_flag;				// 仓位是否更新标识; 0:no; 1:yes
	int long_position;						// 总多仓
	double long_avg_price;					// average price of long position
	int short_position;						// 总空仓
	double short_avg_price;					// average price of short position

	// TODO: Improvement for securities
	int today_buy_volume;					// 今天的总买量
	double today_aver_price_buy;			// 今天的买平均价格
	int today_sell_volume;					// 今天的总卖量
	double today_aver_price_sell;			// 今天卖平均价格

	// Improvement
	char exchange_type;						// 交易所编码

	T_PositionData()
	{
		update_flag = 0;
		long_position = 0;
		long_avg_price = 0;
		short_position = 0;
		short_avg_price = 0;
		today_buy_volume = 0;
		today_aver_price_buy = 0;
		today_sell_volume = 0;
		today_aver_price_sell = 0;
	}
};

/////////////////////////////////////////////////////////////////////////
/// 仓位，报单，成交，合约，等相关查询接口的请求/回报数据结构
/////////////////////////////////////////////////////////////////////////

/// 查询持仓的接口（当前没有参数，查询账号的所有持仓）
struct DLL_PUBLIC T_QryPosition
{
};
struct DLL_PUBLIC PositionDetail
{
	StockCodeType stock_code;				// 合约代码
	TMyTnlDirectionType direction;			// 仓位方向
	VolumeType position;					// 今持仓量, (当前的仓位)
	double position_avg_price;				// 今日开仓均价
	VolumeType yestoday_position;			// 昨持仓量
	double yd_position_avg_price;			// 昨日开仓均价

	// TODO: Improvement
	int today_buy_volume;					// 今天的总买量
	double today_aver_price_buy;			// 今天的买平均价格
	int today_sell_volume;					// 今天的总卖量
	double today_aver_price_sell;			// 今天卖平均价格
	
	// TODO:
	char exchange_type;						// 交易所编码
};
struct DLL_PUBLIC T_PositionReturn
{
	int error_no;							// 错误号，0：正常；其它：异常
	std::vector<PositionDetail> datas;		// 持仓集合
};

/// 查询报单明细的接口（当前没有参数，查询账号当天所有报单）
struct DLL_PUBLIC T_QryOrderDetail
{
};
struct DLL_PUBLIC OrderDetail
{
	StockCodeType stock_code;				// 合约代码
	EntrustNoType entrust_no;				// 委托号
	TMyTnlOrderPriceType order_kind;  		// 委托价格属性
	TMyTnlDirectionType direction;			// 买卖方向
	TMyTnlOffsetType open_close;			// 开平标识
	TMyTnlHedgeFlagType speculator;			// 投机套保标志
	TMyTnlOrderStatusType entrust_status;	// 委托状态
	double limit_price; 					// 价格
	VolumeType volume;      				// 报单手数
	VolumeType volume_traded;  				// 今成交数量
	VolumeType volume_remain;  				// 剩余未成交的手数
};
struct DLL_PUBLIC T_OrderDetailReturn
{
	int error_no;							// 错误号，0：正常；其它：异常
	std::vector<OrderDetail> datas;			// 报单明细集合
};

/// 查询成交明细的接口（当前没有参数，查询账号当天所有成交）
struct DLL_PUBLIC T_QryTradeDetail
{
};
struct DLL_PUBLIC TradeDetail
{
	StockCodeType stock_code;				// 合约代码
	EntrustNoType entrust_no;				// 委托号
	TMyTnlDirectionType direction;			// 买卖方向
	TMyTnlOffsetType open_close;			// 开平标识
	TMyTnlHedgeFlagType speculator;			// 投机套保标志
	double trade_price; 					// 成交价格
	VolumeType trade_volume;  				// 成交数量
	char trade_time[9];						// 成交时间
};
struct DLL_PUBLIC T_TradeDetailReturn
{
	int error_no;							// 错误号，0：正常；其它：异常
	std::vector<TradeDetail> datas;			// 成交明细集合
};

/// 查询合约信息的接口（当前没有参数，查询所有合约信息）
struct DLL_PUBLIC T_QryContractInfo
{
};
struct DLL_PUBLIC ContractInfo
{
	StockCodeType stock_code;				// 合约代码
	char    TradeDate[9];					// 交易日
	char    ExpireDate[9];					// 到期日

};
struct DLL_PUBLIC T_ContractInfoReturn
{
	int error_no;							// 错误号，0：正常；其它：异常
	std::vector<ContractInfo> datas;		// 合约信息集合
};

/////////////////////////////////////////////////////////////////////////
/// 做市商相关请求/回报接口的数据结构 （询价、报价、撤报价）
/////////////////////////////////////////////////////////////////////////

///  询价请求数据结构 模型 -> 通道
struct DLL_PUBLIC T_ReqForQuote
{
	SerialNoType serial_no;					// 询价请求序列号
	StockCodeType stock_code;				// 合约代码
};

///  询价响应数据结构 通道 -> 模型
struct DLL_PUBLIC T_RspOfReqForQuote
{
	int error_no;							// 错误号，0：正常；其它：异常
	SerialNoType serial_no;					// 询价请求序列号
};

///  询价通知数据结构 通道 -> 模型
struct DLL_PUBLIC T_RtnForQuote
{
	StockCodeType stock_code;				// 合约代码
	char for_quote_id[21];					// 询价编号
	char for_quote_time[9];					// 询价时间 HH:MM:SS
};

///  报价请求数据结构 模型 -> 通道
struct DLL_PUBLIC T_InsertQuote
{
	SerialNoType serial_no;					// 报价请求序列号
	StockCodeType stock_code;				// 合约代码

	double buy_limit_price;					// 买方向的价格
	VolumeType buy_volume;					// 买方向的手数
	TMyTnlOffsetType buy_open_close;		// 买方向报价的开平标识
	TMyTnlHedgeFlagType buy_speculator;		// 买方向的投机套保标识

	double sell_limit_price;				// 卖方向的价格
	VolumeType sell_volume;					// 卖方向的手数
	char sell_open_close;					// 卖方向报价的开平标识
	char sell_speculator;					// 卖方向的投机套保标识

	char for_quote_id[21];					// 应价编号
};

///  报价应答数据结构 通道 -> 模型
struct DLL_PUBLIC T_InsertQuoteRespond
{
	SerialNoType serial_no;					// 报价请求序列号
	int error_no;							// 错误号，0：正常；其它：异常
	EntrustNoType entrust_no;				// 交易所委托号
	TMyTnlOrderStatusType entrust_status;	// 委托状态
};

///  撤销报价请求数据结构 模型 -> 通道
struct DLL_PUBLIC T_CancelQuote
{
	SerialNoType serial_no;					// 撤销报价请求序列号
	StockCodeType stock_code;				// 合约代码
	EntrustNoType entrust_no;				// 交易所委托号
	SerialNoType org_serial_no;				// 待撤报价的序列号
};

///  撤销报价应答数据结构 通道 -> 模型
struct DLL_PUBLIC T_CancelQuoteRespond
{
	SerialNoType serial_no;					// 撤销报价序列号
	int error_no;							// 错误号，0：正常；其它：异常
	EntrustNoType entrust_no;				// 交易所委托号
	TMyTnlOrderStatusType entrust_status;	// 委托状态
};

///  报价回报数据结构 通道 -> 模型
struct DLL_PUBLIC T_QuoteReturn
{
	SerialNoType serial_no;					// 报价序列号
	StockCodeType stock_code;				// 合约代码
	EntrustNoType entrust_no;				// 交易所委托号
	TMyTnlOrderStatusType entrust_status;	// 委托状态
	TMyTnlDirectionType direction;			// 买卖方向
	TMyTnlOffsetType open_close;			// 开平标识
	TMyTnlHedgeFlagType speculator;			// 投机套保标志
	VolumeType volume;						// 报单的原始手数
	double limit_price;						// 委托价格
	VolumeType volume_remain;				// 剩余未成交的手数
};

///  报价成交回报数据结构  通道 -> 模型
struct DLL_PUBLIC T_QuoteTrade
{
	SerialNoType serial_no;					// 报价请求序列号
	EntrustNoType entrust_no;				// 交易所委托号
	int business_volume;					// 成交数据
	double business_price;					// 成交价格
	int business_no;						// 成交序号
	StockCodeType stock_code;				// 合约代码
	TMyTnlDirectionType direction;			// 买卖方向
	TMyTnlOffsetType open_close;			// 开平标识
};

#endif
