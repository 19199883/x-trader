/*!****************************************************************************
 @note   Copyright (coffee), 2005-2014, Shengli Tech. Co., Ltd.
 @file   guava_quote.h
 @date   2014/8/27   12:44
 @author zhou.hu
 
 @brief   本文件是GUAVA行情组播接口的示例程序

 @note 
******************************************************************************/
#pragma once


class mirp_header
{
public:
	virtual ~mirp_header() {}

	/*
	* 靠靠靠靠靠靠�4靠4靠靠靠4�(Flag&0x0F)靠靠靠�
	* 靠靠靠1靠4靠靠靠(靠4�,Flag&0x10)靠靠靠靠�
	* 靠靠靠0靠靠靠靠靠靠靠靠靠
	* �1靠靠靠靠靠靠靠靠靠靠�3靠靠靠靠�
	*/
	uint8_t Flag uInt8; 

	/*
	 * 靠靠靠靠靠靠靠靠靠靠靠
	 */
	int8_t TypeID;	

	/*
	 * 靠靠靠靠靠靠靠
	 */
	uint16_t Length;  


	/**
	  * 靠靠靠靠靠靠靠靠靠靠靠
	  *		* 靠靠靠靠靠靠靠靠靠靠�
	**/
	int32_t PacketNo; 

	/*
	 * 靠靠靠靠靠靠靠靠靠
	 */
	int16_t TopicID;  

	/*
	 * 靠靠靠靠靠靠靠靠靠靠靠靠靠靠靠靠靠靠�
	 *
	 */
	uint16_t SnapMillisec;  

	/*
	 * 靠靠靠靠靠靠靠靠靠靠靠靠靠靠靠靠靠
	 */
	int32_t SnapNo;  

	/*
	 * 靠靠靠靠靠靠靠靠靠靠靠靠靠靠靠靠靠靠
	 */
	uInt32 SnapTime;  

	/*
	 * �1980�01�01靠靠靠靠靠靠靠靠靠�
	 */
	uInt16 CommPhaseNo;  

	/*
	 * * 靠靠靠靠靠靠靠靠靠靠靠靠
	 *	* 靠靠靠靠靠靠靠靠靠0靠靠靠靠靠靠�1�
	 */
	int8_t CenterChangeNo; 

	/*
	 * 靠靠�
	 */
	int8_t Reserved;  
};
