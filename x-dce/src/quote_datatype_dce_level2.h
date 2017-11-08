#pragma once

#ifndef DLL_PUBLIC
#define DLL_PUBLIC  __attribute__ ((visibility("default")))
#endif

#include <string>

#pragma pack(push)
#pragma pack(8)

////////////////////////////////////////////////
///MDBestAndDeep：最优与五档深度行情
////////////////////////////////////////////////
struct DLL_PUBLIC MDBestAndDeep_MY
{
    char Type;
    unsigned int Length;                         //报文长度
    unsigned int Version;                        //版本从1开始
    unsigned int Time;                           //预留字段
    char Exchange[3];                    //交易所
    char Contract[80];                   //合约代码
    int SuspensionSign;                 //停牌标志
    float LastClearPrice;                 //昨结算价
    float ClearPrice;                     //今结算价
    float AvgPrice;                       //成交均价
    float LastClose;                      //昨收盘
    float Close;                          //今收盘
    float OpenPrice;                      //今开盘
    unsigned int LastOpenInterest;               //昨持仓量
    unsigned int OpenInterest;                   //持仓量
    float LastPrice;                      //最新价
    unsigned int MatchTotQty;                    //成交数量
    double Turnover;                       //成交金额
    float RiseLimit;                      //最高报价
    float FallLimit;                      //最低报价
    float HighPrice;                      //最高价
    float LowPrice;                       //最低价
    float PreDelta;                       //昨虚实度
    float CurrDelta;                      //今虚实度

    float BuyPriceOne;              //买入价格1
    unsigned int BuyQtyOne;                //买入数量1
    unsigned int BuyImplyQtyOne;           //买1推导量
    float BuyPriceTwo;
    unsigned int BuyQtyTwo;
    unsigned int BuyImplyQtyTwo;
    float BuyPriceThree;
    unsigned int BuyQtyThree;
    unsigned int BuyImplyQtyThree;
    float BuyPriceFour;
    unsigned int BuyQtyFour;
    unsigned int BuyImplyQtyFour;
    float BuyPriceFive;
    unsigned int BuyQtyFive;
    unsigned int BuyImplyQtyFive;
    float SellPriceOne;             //卖出价格1
    unsigned int SellQtyOne;               //买出数量1
    unsigned int SellImplyQtyOne;          //卖1推导量
    float SellPriceTwo;
    unsigned int SellQtyTwo;
    unsigned int SellImplyQtyTwo;
    float SellPriceThree;
    unsigned int SellQtyThree;
    unsigned int SellImplyQtyThree;
    float SellPriceFour;
    unsigned int SellQtyFour;
    unsigned int SellImplyQtyFour;
    float SellPriceFive;
    unsigned int SellQtyFive;
    unsigned int SellImplyQtyFive;

    char GenTime[13];                    //行情产生时间
    unsigned int LastMatchQty;                   //最新成交量
    int InterestChg;                    //持仓量变化
    float LifeLow;                        //历史最低价
    float LifeHigh;                       //历史最高价
    double Delta;                          //delta
    double Gamma;                          //gama
    double Rho;                            //rho
    double Theta;                          //theta
    double Vega;                           //vega
    char TradeDate[9];                   //行情日期
    char LocalDate[9];

    // HH:MM:SS.mmm
    std::string GetQuoteTime() const
    {
        return GenTime;
    }
};

////////////////////////////////////////////////
///MDBestAndDeep：arbi深度行情
////////////////////////////////////////////////
struct DLL_PUBLIC MDArbi_MY
{
    char Type;
    unsigned int Length;                         //报文长度
    unsigned int Version;                        //版本从1开始
    unsigned int Time;                           //预留字段
    char Exchange[3];                    //交易所
    char Contract[80];                   //合约代码
    int SuspensionSign;                 //停牌标志
    float LastClearPrice;                 //昨结算价
    float ClearPrice;                     //今结算价
    float AvgPrice;                       //成交均价
    float LastClose;                      //昨收盘
    float Close;                          //今收盘
    float OpenPrice;                      //今开盘
    unsigned int LastOpenInterest;               //昨持仓量
    unsigned int OpenInterest;                   //持仓量
    float LastPrice;                      //最新价
    unsigned int MatchTotQty;                    //成交数量
    double Turnover;                       //成交金额
    float RiseLimit;                      //最高报价
    float FallLimit;                      //最低报价
    float HighPrice;                      //最高价
    float LowPrice;                       //最低价
    float PreDelta;                       //昨虚实度
    float CurrDelta;                      //今虚实度

    float BuyPriceOne;              //买入价格1
    unsigned int BuyQtyOne;                //买入数量1
    unsigned int BuyImplyQtyOne;           //买1推导量
    float BuyPriceTwo;
    unsigned int BuyQtyTwo;
    unsigned int BuyImplyQtyTwo;
    float BuyPriceThree;
    unsigned int BuyQtyThree;
    unsigned int BuyImplyQtyThree;
    float BuyPriceFour;
    unsigned int BuyQtyFour;
    unsigned int BuyImplyQtyFour;
    float BuyPriceFive;
    unsigned int BuyQtyFive;
    unsigned int BuyImplyQtyFive;
    float SellPriceOne;             //卖出价格1
    unsigned int SellQtyOne;               //买出数量1
    unsigned int SellImplyQtyOne;          //卖1推导量
    float SellPriceTwo;
    unsigned int SellQtyTwo;
    unsigned int SellImplyQtyTwo;
    float SellPriceThree;
    unsigned int SellQtyThree;
    unsigned int SellImplyQtyThree;
    float SellPriceFour;
    unsigned int SellQtyFour;
    unsigned int SellImplyQtyFour;
    float SellPriceFive;
    unsigned int SellQtyFive;
    unsigned int SellImplyQtyFive;

    char GenTime[13];                    //行情产生时间
    unsigned int LastMatchQty;                   //最新成交量
    int InterestChg;                    //持仓量变化
    float LifeLow;                        //历史最低价
    float LifeHigh;                       //历史最高价
    double Delta;                          //delta
    double Gamma;                          //gama
    double Rho;                            //rho
    double Theta;                          //theta
    double Vega;                           //vega
    char TradeDate[9];                   //行情日期
    char LocalDate[9];

    // HH:MM:SS.mmm
    std::string GetQuoteTime() const
    {
        return "";
    }
};

////////////////////////////////////////////////
///MDTenEntrust：最优价位上十笔委托
////////////////////////////////////////////////
struct DLL_PUBLIC MDTenEntrust_MY
{
    char Type;                           //行情域标识
    unsigned int Len;
    char Contract[80];                   //合约代码
    double BestBuyOrderPrice;              //价格
    unsigned int BestBuyOrderQtyOne;             //委托量1
    unsigned int BestBuyOrderQtyTwo;             //委托量2
    unsigned int BestBuyOrderQtyThree;           //委托量3
    unsigned int BestBuyOrderQtyFour;            //委托量4
    unsigned int BestBuyOrderQtyFive;            //委托量5
    unsigned int BestBuyOrderQtySix;             //委托量6
    unsigned int BestBuyOrderQtySeven;           //委托量7
    unsigned int BestBuyOrderQtyEight;           //委托量8
    unsigned int BestBuyOrderQtyNine;            //委托量9
    unsigned int BestBuyOrderQtyTen;             //委托量10
    double BestSellOrderPrice;             //价格
    unsigned int BestSellOrderQtyOne;            //委托量1
    unsigned int BestSellOrderQtyTwo;            //委托量2
    unsigned int BestSellOrderQtyThree;          //委托量3
    unsigned int BestSellOrderQtyFour;           //委托量4
    unsigned int BestSellOrderQtyFive;           //委托量5
    unsigned int BestSellOrderQtySix;            //委托量6
    unsigned int BestSellOrderQtySeven;          //委托量7
    unsigned int BestSellOrderQtyEight;          //委托量8
    unsigned int BestSellOrderQtyNine;           //委托量9
    unsigned int BestSellOrderQtyTen;            //委托量10
    char GenTime[13];                    //生成时间

    // HH:MM:SS.mmm
    std::string GetQuoteTime() const
    {
        return "";
    }
};

////////////////////////////////////////////////
///MDOrderStatistic：加权平均以及委托总量行情
////////////////////////////////////////////////
struct DLL_PUBLIC MDOrderStatistic_MY
{
    char Type;                           //行情域标识
    unsigned int Len;
    char ContractID[80];                 //合约号
    unsigned int TotalBuyOrderNum;               //买委托总量
    unsigned int TotalSellOrderNum;              //卖委托总量
    double WeightedAverageBuyOrderPrice;   //加权平均委买价格
    double WeightedAverageSellOrderPrice;  //加权平均委卖价格

    // HH:MM:SS.mmm
    std::string GetQuoteTime() const
    {
        return "";
    }
};

////////////////////////////////////////////////
///MDRealTimePrice：实时结算价
////////////////////////////////////////////////
struct DLL_PUBLIC MDRealTimePrice_MY
{
    char Type;                           //行情域标识
    unsigned int Len;
    char ContractID[80];                 //合约号
    double RealTimePrice;                  //实时结算价

    // HH:MM:SS.mmm
    std::string GetQuoteTime() const
    {
        return "";
    }
};

////////////////////////////////////////////////
///分价位成交：分价位成交
////////////////////////////////////////////////
struct DLL_PUBLIC MDMarchPriceQty_MY
{
    char Type;                           //行情域标识
    unsigned int Len;
    char ContractID[80];                 //合约号
    double PriceOne;                       //价格
    unsigned int PriceOneBOQty;                  //买开数量
    unsigned int PriceOneBEQty;                  //买平数量
    unsigned int PriceOneSOQty;                  //卖开数量
    unsigned int PriceOneSEQty;                  //卖平数量
    double PriceTwo;                       //价格
    unsigned int PriceTwoBOQty;                  //买开数量
    unsigned int PriceTwoBEQty;                  //买平数量
    unsigned int PriceTwoSOQty;                  //卖开数量
    unsigned int PriceTwoSEQty;                  //卖平数量
    double PriceThree;                     //价格
    unsigned int PriceThreeBOQty;                //买开数量
    unsigned int PriceThreeBEQty;                //买平数量
    unsigned int PriceThreeSOQty;                //卖开数量
    unsigned int PriceThreeSEQty;                //卖平数量
    double PriceFour;                      //价格
    unsigned int PriceFourBOQty;                 //买开数量
    unsigned int PriceFourBEQty;                 //买平数量
    unsigned int PriceFourSOQty;                 //卖开数量
    unsigned int PriceFourSEQty;                 //卖平数量
    double PriceFive;                      //价格
    unsigned int PriceFiveBOQty;                 //买开数量
    unsigned int PriceFiveBEQty;                 //买平数量
    unsigned int PriceFiveSOQty;                 //卖开数量
    unsigned int PriceFiveSEQty;                 //卖平数量

    // HH:MM:SS.mmm
    std::string GetQuoteTime() const
    {
        return "";
    }
};

#pragma pack(pop)
