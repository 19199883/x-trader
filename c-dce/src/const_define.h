#ifndef CONSTDEFINE_H_
#define CONSTDEFINE_H_

/// 常量，和通道中定义一致，无需再次映射处理
//买卖方向
const char CONST_ENTRUST_BUY = '0';   //买入
const char CONST_ENTRUST_SELL = '1';  //卖出

//开平仓标志
const char CONST_ENTRUST_OPEN = '0';  //开仓
const char CONST_ENTRUST_CLOSE = '1';  //平仓
const char CONST_ENTRUST_CLOSETODAY = '2';  //平今仓
const char CONST_ENTRUST_CLOSEYESTERDAY = '3'; //平昨仓 20121017 caodl 增加

//投保标志
const char CONST_SHFLAG_TOU = '0';  //投机
const char CONST_SHFLAG_BAO = '1';  //保值
const char CONST_SHFLAG_TAO = '2';  //套利  //20130125 caodl 增加

//价格类型
const char CONST_ENTRUSTKIND_XJ   = '0';  //限价
const char CONST_ENTRUSTKIND_SJ   = '1';  //市价
const char CONST_ENTRUSTKIND_ZYJ  = '2';  //最优价

//报单类型
const char CONST_ENTRUSTKIND_NORMAL = '0';  //普通
const char CONST_ENTRUSTKIND_FAK    = '1';  //FAK
const char CONST_ENTRUSTKIND_FOK    = '2';  //FOK

//交易所标志
const char CONST_EXCHCODE_SHFE  = 'A'; //上海期货
const char CONST_EXCHCODE_DCE   = 'B';  //大连期货
const char CONST_EXCHCODE_CZCE  = 'C';  //郑州期货
const char CONST_EXCHCODE_CFFEX = 'G';  //中金所
const char CONST_EXCHCODE_SZEX  = '0';  //深交所
const char CONST_EXCHCODE_SHEX  = '1';  //上交所

#endif // CONSTDEFINE_H_
