/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   UdpDataType.h
 * Author: ZhangZhi
 *
 * Created on 2019年8月6日, 下午12:13
 */

#ifndef UDPDATATYPE_H
#define UDPDATATYPE_H

#include "TapAPICommDef.h"

#pragma pack(push, 1)

	//UDP数据帧头结构
	struct TapAPIUdpHead
	{
		TAPIUINT32						PackageFlag;
		TAPIUINT16		                ProtocolCode;
		TAPIUINT16                      Version;
		TAPIUINT32                      Sequence;
	} ;

	const TAPIUINT32					UDP_Package_Flag = 0x22E95CA7;

	//链路认证请求命令
	const TAPIUINT16					CMD_UDPAuth_Req = 0x0001;
	//链路认证应答命令
	const TAPIUINT16					CMD_UDPAuth_Rsp = 0x0002;

	struct TapAPIUdpAuthReq
	{
		TAPIINT64                       AuthCode;
		TAPISTR_20                      UserNo;
	} ;

	//心跳请求命令
	const TAPIUINT16			CMD_UDPHeartBeat_Req = 0x0003;
	//心跳应答命令
	const TAPIUINT16			CMD_UDPHeartBeat_Rsp = 0x0004;

	//报单请求命令
	const TAPIUINT16			CMD_UDPOrderInsert_Req = 0x0101;
	//报单应答命令
	const TAPIUINT16			CMD_UDPOrderInsert_Rsp = 0x0102;



	//客户下单请求结构简化

	struct TapAPIUdpOrderInsertReq
	{
		TAPISTR_20							AccountNo;						//客户资金帐号 21

		TAPICHAR							Contract[16];                   //合约编码  16
		TAPICHAR							Contract2[16];                  //合约编码  16
		TAPIOrderTypeType					OrderType;						//委托类型 1
		TAPITimeInForceType					TimeInForce;					//委托有效类型 1
		TAPISideType						OrderSide;						//买入卖出 1
		TAPIPositionEffectType				PositionEffect;					//开平标志1 1
		TAPIHedgeFlagType					HedgeFlag;						//投机保值 1
		TAPIREAL64							OrderPrice;						//委托价格1 8
		TAPIUINT32							OrderQty;						//委托数量 4
		TAPIREAL64							OrderPrice2;					//委托价格2 8
		TAPIUINT32							OrderQty2;						//委托数量2 4

		TAPICHAR							ClientOrderNo[16];				//客户端本地委托编号 16
		TAPICHAR							UpperChannelNo[11];				//上手通道号 11
	} ;

	//撤单请求命令
	const TAPIUINT16			CMD_UDPOrderDelete_Req = 0x0103;
	//撤单应答命令
	const TAPIUINT16			CMD_UDPOrderDelete_Rsp = 0x0104;

	//客户撤单请求结构

	struct TapAPIUdpOrderDeleteReq
	{
		TAPICHAR							OrderNo[21];						//委托编码
	} ;
#pragma pack(pop)
    
#endif /* UDPDATATYPE_H */

