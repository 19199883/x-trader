/* 
* File:   ZceLevel2ApiStruct.h
* Author: zz_online
*
* Created on 2014年7月1日, 下午2:46
*/


#ifndef ZCELEVEL2APISTRUCT_H
#define	ZCELEVEL2APISTRUCT_H

typedef int				ZCEINT4;
typedef unsigned int	ZCEUINT4;
typedef unsigned char	ZCEBYTE;
typedef double			ZCEFLOAT;
typedef char			ZCECHAR;

#pragma  pack(push, 4)
//API涉及的结构定义

//用户登录请求
struct ZCEL2QuotReqLoginField
{
	ZCECHAR		UserNo[20];			//用户号
	ZCECHAR		Password[20];		//密码
};

//登录应答结构
struct ZCEL2QuotRspLoginField
{
	ZCECHAR		UserNo[20];			//用户号
	ZCEINT4		ErrorCode;			//错误码：0表示成功
	ZCECHAR		ErrorText[200];		//错误信息，成功时不返回信息
	ZCECHAR		TimeStamp[24];		//时间：如2014-02-03 13:23:45.000
};

//行情全文
struct ZCEL2QuotSnapshotField
{
	ZCECHAR		ContractID[50];		/*合约编码*/
	ZCEBYTE		ContractIDType;		/*合约类型 0->目前应该为0， 扩充：0:期货,1:期权,2:组合*/
	ZCEFLOAT	PreSettle;			/*前结算价格*/
	ZCEFLOAT	PreClose;			/*前收盘价格*/
	ZCEUINT4	PreOpenInterest;	/*昨日空盘量*/
	ZCEFLOAT	OpenPrice;			/*开盘价*/
	ZCEFLOAT	HighPrice;			/*最高价*/
	ZCEFLOAT	LowPrice;			/*最低价*/
	ZCEFLOAT	LastPrice;			/*最新价*/
	ZCEFLOAT	BidPrice[5];		/*买入价格*/
	ZCEFLOAT	AskPrice[5];		/*卖出价*/
	ZCEUINT4	BidLot[5];			/*买入数量*/
	ZCEUINT4	AskLot[5];			/*卖出数量*/
	ZCEUINT4	TotalVolume;		/*总成交量*/
	ZCEUINT4	OpenInterest;		/*持仓量*/
	ZCEFLOAT	ClosePrice;			/*收盘价*/
	ZCEFLOAT	SettlePrice;		/*结算价*/
	ZCEFLOAT	AveragePrice;		/*均价*/
	ZCEFLOAT	LifeHigh;			/*历史最高成交价格*/
	ZCEFLOAT	LifeLow;			/*历史最低成交价格*/
	ZCEFLOAT	HighLimit;			/*涨停板*/
	ZCEFLOAT	LowLimit;			/*跌停板*/
	ZCEUINT4	TotalBidLot;		/*委买总量*/
	ZCEUINT4	TotalAskLot;		/*委卖总量*/
	ZCECHAR		TimeStamp[24];		//时间：如2014-02-03 13:23:45
};

//组合行情
struct ZCEL2QuotCMBQuotField
{
	ZCEBYTE 	CmbType;			/*组合类型 1 -- SPD, 2 -- IPS*/
	ZCECHAR		ContractID[50];		/*组合合约编码,分隔符'-'*/
	ZCEFLOAT	BidPrice;			/*买入价格*/
	ZCEFLOAT	AskPrice;			/*卖出价*/
	ZCEUINT4	BidLot;				/*买入数量*/
	ZCEUINT4	AskLot;				/*卖出数量*/
	ZCEUINT4	VolBidLot;			/*总买入数量*/
	ZCEUINT4	VolAskLot;			/*总卖出数量*/
	ZCECHAR		TimeStamp[24];		//时间：如2008-02-03 13:23:45
};

//错误代码定义
const ZCEINT4 ZCEErrCode_Success			= 0;	//启动数据接收失败
const ZCEINT4 ZCEErrCode_ConnectFailed		= 1;	//启动数据接收失败
const ZCEINT4 ZCEErrCode_StartRecv			= 2;	//启动数据接收失败
const ZCEINT4 ZCEErrCode_LogpNull			= 3;	//登录结构指针为空
const ZCEINT4 ZCEErrCode_FailInitNetwork	= 4;	//初始化网络环境失败
const ZCEINT4 ZCEErrCode_ReachMaxSrvNum		= 5;	//不能设置更多的服务器地址
const ZCEINT4 ZCEErrCode_SrvAddrNull		= 6;	//未设置服务地址
const ZCEINT4 ZCEErrCode_SrvClose			= 7;	//服务器连接关闭
const ZCEINT4 ZCEErrCode_FailedNegotiate	= 8;	//链路协商失败
const ZCEINT4 ZCEErrCode_SendFailed			= 9;	//数据发送失败,可能连接未建立
const ZCEINT4 ZCEErrCode_IncorrectUser		= 10;	//登录失败，用户名或密码错误
const ZCEINT4 ZCEErrCode_ReachMaxLoginNum	= 11;	//登录失败，超出最大允许登陆数
#pragma pack(pop)

// added by wangying on 20161028, jinrui future company, udp level2
#pragma pack(push,8)
struct StdQuote5
{
	char instrument[32];
	char updateTime[9];
	// millisecond component of update time
	int updateMS;
	// last price
	double price;
	// cumulate quantity
	int volume;
	double turnover;
	int openinterest;
	double askPrice1;
	int askVolume1;
	double bidPrice1;
	int bidVolume1;
	double askPrice2;
	int askVolume2;
	double bidPrice2;
	int bidVolume2;
	double askPrice3;
	int askVolume3;
	double bidPrice3;
	int bidVolume3;
	double askPrice4;
	int askVolume4;
	double bidPrice4;
	int bidVolume4;
	double askPrice5;
	int askVolume5;
	double bidPrice5;
	int bidVolume5;
	unsigned long long totalbid;
	unsigned long long totalask;		 
};

#pragma pack(pop)

#endif	/* ZCELEVEL2APISTRUCT_H */

