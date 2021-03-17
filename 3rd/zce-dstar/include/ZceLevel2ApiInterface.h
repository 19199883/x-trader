/* 
 * File:   ZceLevel2ApiInterface.h
 * Author: zz_online
 *
 * Created on 2014年7月1日, 下午3:13
 */

#ifndef ZCELEVEL2APIINTERFACE_H
#define	ZCELEVEL2APIINTERFACE_H

#include "ZceLevel2ApiStruct.h"

#ifdef _WIN32
#define ZCEL2QUOTE_CALL __cdecl
#define ZCEL2QUOTE_API extern "C" _declspec(dllexport)
#else
#define ZCEL2QUOTE_CALL
#define ZCEL2QUOTE_API extern "C"
#endif

class IZCEL2QuoteClient
{
public:
	virtual ZCEINT4 ZCEL2QUOTE_CALL GetAPIVersion()=0;
	virtual	ZCEINT4 ZCEL2QUOTE_CALL SetService(const char* IP,int Port)=0;
	virtual	ZCEINT4 ZCEL2QUOTE_CALL ConnectService()=0;
	virtual	ZCEINT4 ZCEL2QUOTE_CALL Login(const ZCEL2QuotReqLoginField* UserMsg)=0;
	virtual ZCEINT4 ZCEL2QUOTE_CALL Logout()=0;
};

class IZCEL2QuoteNotify
{
public:
	virtual void ZCEL2QUOTE_CALL OnRspLogin(const ZCEL2QuotRspLoginField& loginMsg)=0;
	virtual void ZCEL2QUOTE_CALL OnRecvQuote(const ZCEL2QuotSnapshotField& snapshot)=0;
	virtual void ZCEL2QUOTE_CALL OnRecvCmbQuote(const ZCEL2QuotCMBQuotField& snapshot)=0;
	virtual void ZCEL2QUOTE_CALL OnConnectClose()=0;
};

//---------------- API 接口函数
/*
	功能：创建IZCEL2QuoteClient的接口对象。
	参数：IZCEL2QuoteNotify派生类对象指针
	返回值：创建成功，返回实现了ITapQuoteAPI接口的对象指针；创建失败，返回NULL。 
 */
ZCEL2QUOTE_API IZCEL2QuoteClient* ZCEL2QUOTE_CALL CreateZCEL2QuotClient(IZCEL2QuoteNotify * notify);
/*
	功能：销毁通过CreateZCEQuotClient函数创建的IZCEL2QuoteClient 对象。
	参数：IZCEL2QuoteClient 对象指针。
	返回值：
 */
ZCEL2QUOTE_API void ZCEL2QUOTE_CALL DelZCEL2QuotClient(IZCEL2QuoteClient* APIclient);


#endif	/* ZCELEVEL2APIINTERFACE_H */

