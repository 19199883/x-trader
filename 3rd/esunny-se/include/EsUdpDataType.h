/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   UdpDataType.h
 * Author: ZhangZhi
 *
 * Created on 2019��8��6��, ����12:13
 */

#ifndef UDPDATATYPE_H
#define UDPDATATYPE_H

#include "TapAPICommDef.h"

#pragma pack(push, 1)

	//UDP����֡ͷ�ṹ
	struct TapAPIUdpHead
	{
		TAPIUINT32						PackageFlag;
		TAPIUINT16		                ProtocolCode;
		TAPIUINT16                      Version;
		TAPIUINT32                      Sequence;
	} ;

	const TAPIUINT32					UDP_Package_Flag = 0x22E95CA7;

	//��·��֤��������
	const TAPIUINT16					CMD_UDPAuth_Req = 0x0001;
	//��·��֤Ӧ������
	const TAPIUINT16					CMD_UDPAuth_Rsp = 0x0002;

	struct TapAPIUdpAuthReq
	{
		TAPIINT64                       AuthCode;
		TAPISTR_20                      UserNo;
	} ;

	//������������
	const TAPIUINT16			CMD_UDPHeartBeat_Req = 0x0003;
	//����Ӧ������
	const TAPIUINT16			CMD_UDPHeartBeat_Rsp = 0x0004;

	//������������
	const TAPIUINT16			CMD_UDPOrderInsert_Req = 0x0101;
	//����Ӧ������
	const TAPIUINT16			CMD_UDPOrderInsert_Rsp = 0x0102;



	//�ͻ��µ�����ṹ��

	struct TapAPIUdpOrderInsertReq
	{
		TAPISTR_20							AccountNo;						//�ͻ��ʽ��ʺ� 21

		TAPICHAR							Contract[16];                   //��Լ����  16
		TAPICHAR							Contract2[16];                  //��Լ����  16
		TAPIOrderTypeType					OrderType;						//ί������ 1
		TAPITimeInForceType					TimeInForce;					//ί����Ч���� 1
		TAPISideType						OrderSide;						//�������� 1
		TAPIPositionEffectType				PositionEffect;					//��ƽ��־1 1
		TAPIHedgeFlagType					HedgeFlag;						//Ͷ����ֵ 1
		TAPIREAL64							OrderPrice;						//ί�м۸�1 8
		TAPIUINT32							OrderQty;						//ί������ 4
		TAPIREAL64							OrderPrice2;					//ί�м۸�2 8
		TAPIUINT32							OrderQty2;						//ί������2 4

		TAPICHAR							ClientOrderNo[16];				//�ͻ��˱���ί�б�� 16
		TAPICHAR							UpperChannelNo[11];				//����ͨ���� 11
	} ;

	//������������
	const TAPIUINT16			CMD_UDPOrderDelete_Req = 0x0103;
	//����Ӧ������
	const TAPIUINT16			CMD_UDPOrderDelete_Rsp = 0x0104;

	//�ͻ���������ṹ

	struct TapAPIUdpOrderDeleteReq
	{
		TAPICHAR							OrderNo[21];						//ί�б���
	} ;
#pragma pack(pop)
    
#endif /* UDPDATATYPE_H */

