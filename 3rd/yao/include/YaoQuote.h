
#ifndef   YAOQUOTE_H_
#define   YAOQUOTE_H_

enum YaoExchanges
{
	YSHFE	= 'A',
	YCFFEX	= 'G',
	YDCE		= 'B',
	YCZCE	= 'C'
};

enum FeedTypes
{
	// 中金所深度行情
	CffexLevel2 = 0,
	//	大商所深度
	DceLevel2 = 1,
	// 大商所OrderStats
	DceOrderStats = 3,
	// 大商所深度与大商所OrderStats拼接
	DceCombine = 27,
	// 上期深度
	ShfeLevel2 = 6,
	// 大商所深度行情
	CzceLevel2 = 7,
	// CTP一档行情
	CTPLevel1 = 12
};

struct YaoQuote
{
	FeedTypes feed_type;				
	char symbol[48];			//	合约代码: 如'm1801','SR801','Au(T+D)'
	YaoExchanges exchange;			//	交易所代码
	//	交易所行情时间(HHMMssmmm), 如：90000306表示09:00:00 306. 0点-3点的数据 +24hrs
	int int_time;				
	float pre_close_px;			//	昨收盘价
	float pre_settle_px;		//	昨结算价
	double pre_open_interest;	//	昨市场总持仓量
	double open_interest;		//	市场总持仓量
	float open_px;				//	开盘价
	float high_px;				//	最高价
	float low_px;				//	最低价
	float avg_px;				//	平均价
	float last_px;				//	最新价
	float bp_array[5];			//	多档买价
	float ap_array[5];			//	多档卖价
	int bv_array[5];			//	多档买量
	int av_array[5];			//	多档卖量
	int total_vol;				//	总成交量
	float total_notional;		//	总成交额
	float upper_limit_px;		//	涨停价
	float lower_limit_px;		//	跌停价
	float close_px;				//	收盘价 只在日盘收盘后的几笔行情有效，其余时间为0
	float settle_px;			//	结算价 只在日盘收盘后的几笔行情有效，其余时间为0
	int implied_bid_size[5];	//	推导买量
	int implied_ask_size[5];	//	推导卖量
	int total_buy_ordsize;		//	总买量 DCE OrderStat quote
	int total_sell_ordsize;		//	总卖量 DCE OrderStat quote
	float weighted_buy_px;		//	平均买价 DCE OrderStat quote
	float weighted_sell_px;		//	平均卖价 DCE OrderStat quote	

};

#endif // YAOQUOTE_H_
