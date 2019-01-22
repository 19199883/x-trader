#pragma once

#ifndef DLL_PUBLIC
#define DLL_PUBLIC  __attribute__ ((visibility("default")))
#endif


/*************************************************
172.18.32.255
172.18.33.255  托管网

多播地址：238.0.0.1 
端口1: 26888 - 成交量
端口2: 27888 - 10档行情 交易所产生的10档增删来构建。
端口3：28888 - 即时行情
端口4：29888 - 逐笔委托
************************************************/

/********************************
数据结构： 
1. 十档行情
*************************/
struct marketData
{
    double price;       // 价格
    int size;           // Consolidated trade quantity(现手)
    int numberOfOrders; // 该档位挂单总量 
};

// 合约bid/offer各10档行情数据结构定义
struct depthMarketData
{
    char name[6];              // 合约名
    uint64_t transactTime;     // 时间戳
    struct marketData bid[10]; // buy 10档行情
    struct marketData ask[10]; // sell 10档行情
};

/********************
2. 即时行情,成交汇总
********************/
// 即时行情定义 tradesummary
struct realTimeData
{
    char name[6];          // 合约名
    uint64_t transactTime; // 时间戳-纳秒级别
    double price;          // 价格
    int size;              //  Consolidated trade quantity 成交手数
    int numberOfOrders;    // The total number of real orders per instrument that participated in a match step within a match event
    int updateAction;      // 更新类型, 0:New, 1:Change, 2:Delete, 3:Delete Thru, 4:Delete From, 5:Overlay
    int aggressorSide;     // 买卖方向 0: No Aggressor, 1:Buy, 2,Sell 主卖，卖方大单
};

/******************
3. 逐笔委托
orderbook即时委托是在10档之外的委托，主要行情要用10档
******************/
// 逐笔委托定义
struct orderbookData
{
    char name[6];           // 合约名
    uint64_t transactTime;  // 时间戳-纳秒级别
    uint64_t orderID;       // 订单号
    uint64_t orderPriority; // 订单优先级
    double price;           // 价格
    int orderQty;           // 可视订单量
    int updateAction;       // 更新类型 0:New, 1:Change, 2:Delete, 3:Delete Thru, 4:Delete From, 5:Overlay
    int bookType;           // 48:Bid, 49:Offer, 69:Implied Bid, 70:Implied Offer, 74:Book Reset
};

/*********************
4. 总成交量
*********************/
// 成交量结构定义
struct tradeVolume
{
    char name[6];
    uint64_t transactTime; // 时间戳-纳秒级别
    int volume;            // 成交量
    int updateAction;      // 更新类型 0:New, 1:Change, 2:Delete, 3:Delete Thru, 4:Delete From, 5:Overlay
};

