// 茂源交易通道，业务数据类型

#ifndef _MY_TRADE_TUNNEL_DATA_TYPE_H_
#define _MY_TRADE_TUNNEL_DATA_TYPE_H_

#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <vector>

#ifdef WIN32
#define DLL_PUBLIC
#else
#ifndef DLL_PUBLIC
#define DLL_PUBLIC __attribute__ ((visibility ("default")))
#endif
#endif

/////////////////////////////////////////////////////////////////////////
/// StockCodeType 合约代码/股票代码数据类型
/////////////////////////////////////////////////////////////////////////
typedef char StockCodeType[64];

/////////////////////////////////////////////////////////////////////////
/// SerialNoType 请求序号类型
/////////////////////////////////////////////////////////////////////////
typedef long SerialNoType;

/////////////////////////////////////////////////////////////////////////
/// VolumeType 报单量类型
/////////////////////////////////////////////////////////////////////////
typedef long VolumeType;

/////////////////////////////////////////////////////////////////////////
/// EntrustNoType 交易所委托号类型
/////////////////////////////////////////////////////////////////////////
typedef long EntrustNoType;

/////////////////////////////////////////////////////////////////////////
/// TMyTnlDirectionType 买卖方向类型
/////////////////////////////////////////////////////////////////////////
///买入
#define MY_TNL_D_BUY				'0'
///卖出
#define MY_TNL_D_SELL				'1'

typedef char TMyTnlDirectionType;

/////////////////////////////////////////////////////////////////////////
/// TMyTnlOffsetType 开平仓标志类型
/////////////////////////////////////////////////////////////////////////
///开仓
#define MY_TNL_D_OPEN				'0'
///平仓
#define MY_TNL_D_CLOSE				'1'
///平今仓
#define MY_TNL_D_CLOSETODAY         '2'
///平昨仓
#define MY_TNL_D_CLOSEYESTERDAY     '3'

typedef char TMyTnlOffsetType;

/////////////////////////////////////////////////////////////////////////
// TMyTnlOrderPriceType 委托价格属性类型
/////////////////////////////////////////////////////////////////////////
///限价
#define MY_TNL_OPT_LIMIT_PRICE		'0'
///市价
#define MY_TNL_OPT_ANY_PRICE		'1'
///最优价
#define MY_TNL_OPT_BEST_PRICE		'2'
///五档价
#define MY_TNL_OPT_FIVELEVEL_PRICE	'3'

typedef char TMyTnlOrderPriceType;

/////////////////////////////////////////////////////////////////////////
///TMyTnlHedgeFlagType 投机套保标志类型
/////////////////////////////////////////////////////////////////////////
///投机
#define MY_TNL_HF_SPECULATION		'0'
///套保
#define MY_TNL_HF_HEDGE				'1'
///套利
#define MY_TNL_HF_ARBITRAGE			'2'

typedef char TMyTnlHedgeFlagType;

/////////////////////////////////////////////////////////////////////////
/// TMyTnlOrderEntrustType 报单委托类型
/////////////////////////////////////////////////////////////////////////
///普通
#define MY_TNL_HF_NORMAL			'0'
///FAK
#define MY_TNL_HF_FAK				'1'
///FOK
#define MY_TNL_HF_FOK				'2'

typedef char TMyTnlOrderEntrustType;

/////////////////////////////////////////////////////////////////////////
/// TMyTnlOrderStatusType 报单的委托状态类型
/////////////////////////////////////////////////////////////////////////
///未报
#define MY_TNL_OS_UNREPORT			'0'
///已经报入
#define MY_TNL_OS_REPORDED			'a'
///部分成交
#define MY_TNL_OS_PARTIALCOM		'p'
///全部成交
#define MY_TNL_OS_COMPLETED			'c'
///等待撤除
#define MY_TNL_OS_WITHDRAWING		'f'
///错误委托
#define MY_TNL_OS_ERROR				'e'
///已经撤销
#define MY_TNL_OS_WITHDRAWED		'd'
///内部撮合中 （myexchange 新增状态）
#define MY_TNL_OS_INNERMATCHING		'm'

typedef char TMyTnlOrderStatusType;

/////////////////////////////////////////////////////////////////////////
/// TMyTnlExchangeCodeType 茂源交易所编码 类型
/////////////////////////////////////////////////////////////////////////
///上海期货
#define MY_TNL_EC_SHFE              'A'
#define MY_TNL_EXID_SHFE            "SHFE"
///大连期货
#define MY_TNL_EC_DCE               'B'
#define MY_TNL_EXID_DCE             "DCE"
///郑州期货
#define MY_TNL_EC_CZCE              'C'
#define MY_TNL_EXID_CZCE            "CZCE"
///中金所
#define MY_TNL_EC_CFFEX             'G'
#define MY_TNL_EXID_CFFEX           "CFFEX"
///深交所
#define MY_TNL_EC_SZEX              '0'
#define MY_TNL_EXID_SZEX            "SZE"
///上交所
#define MY_TNL_EC_SHEX              '1'
#define MY_TNL_EXID_SHEX            "SSE"
///港交所
#define MY_TNL_EC_HKEX              '2'
#define MY_TNL_EXID_HKEX            "SEHK"
///上海黄金
#define MY_TNL_EC_SHGOLD            'D'
#define MY_TNL_EXID_SHGOLD          "SHGOLD"
/// SGX
#define MY_TNL_EC_SGX               'S'
#define MY_TNL_EXID_SGX             "SGX"

typedef char TMyTnlExchangeCodeType;

// 功能命令的错误码定义
namespace TUNNEL_ERR_CODE
{
const int RESULT_FAIL = -1;                        //执行功能失败
const int RESULT_SUCCESS = 0;                      //执行功能成功
const int UNSUPPORTED_FUNCTION_NO = 1;             //无此功能号
const int NO_VALID_CONNECT_AVAILABLE = 2;          //交易通道未连接期货商
const int ERROR_REQUEST = 3;                       //错误的请求指令
const int CFFEX_EXCEED_LIMIT = 4;                  //股指期货累计开仓超出最大限制
const int POSSIBLE_SELF_TRADE = 5;                 //可能自成交
const int CANCEL_TIMES_REACH_WARN_THRETHOLD = 6;   //撤单次数达到告警阈值
const int CANCEL_REACH_LIMIT = 7;                  //撤单次数达到上限
const int OPEN_EQUAL_LIMIT = 8;						//开仓数正好等于阈值
const int CANCEL_EQUAL_LIMIT = 9;					//撤单数正好等于阈值

const int UNSUPPORTED_FUNCTION = 100;              //不支持的功能
const int NO_PRIVILEGE = 101;                      //无此权限
const int NO_TRADING_RIGHT = 102;                  //没有报单交易权限
const int NO_VALID_TRADER_AVAILABLE = 103;         //该交易席位未连接到交易所
const int MARKET_NOT_OPEN = 104;                   //该席位目前没有处于开盘状态
const int CFFEX_OVER_REQUEST = 105;                //交易所未处理请求超过许可数
const int CFFEX_OVER_REQUEST_PER_SECOND = 106;     //交易所每秒发送请求数超过许可数
const int SETTLEMENT_INFO_NOT_CONFIRMED = 107;     //结算结果未确认

const int INSTRUMENT_NOT_FOUND = 200;              //找不到合约
const int INSTRUMENT_NOT_TRADING = 201;            //合约不能交易
const int BAD_FIELD = 202;                         //报单字段有误
const int BAD_ORDER_ACTION_FIELD = 203;            //错误的报单操作字段
const int DUPLICATE_ORDER_REF = 204;               //不允许重复报单
const int DUPLICATE_ORDER_ACTION_REF = 205;        //不允许重复撤单
const int ORDER_NOT_FOUND = 206;                   //撤单找不到相应报单
const int UNSUITABLE_ORDER_STATUS = 207;           //当前报单状态不允许撤单（报单已成时，不可撤单）
const int CLOSE_ONLY = 208;                        //只能平仓
const int OVER_CLOSE_POSITION = 209;               //平仓量超过持仓量
const int INSUFFICIENT_MONEY = 210;                //资金不足
const int SHORT_SELL = 211;                        //现货交易不能卖空
const int OVER_CLOSETODAY_POSITION = 212;          //平今仓位不足
const int OVER_CLOSEYESTERDAY_POSITION = 213;      //平昨仓位不足
const int PRICE_OVER_LIMIT = 214;                  //委托价格超出涨跌幅限制
const int STOCK_CANUSEAMT_NOTENOUGH = 215;         //证券可用数量不足

const int REACH_UPPER_LIMIT_POSITION = 216;        //报单超量，或报单数量不合法
}

#endif
