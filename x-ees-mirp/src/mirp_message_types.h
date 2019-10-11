/*!****************************************************************************
 @note   Copyright (coffee), 2005-2014, Shengli Tech. Co., Ltd.
 @file   guava_quote.h
 @date   2014/8/27   12:44
 @author zhou.hu
 
 @brief   ±¾ÎÄ¼þÊÇGUAVAÐÐÇé×é²¥½Ó¿ÚµÄÊ¾Àý³ÌÐò

 @note 
******************************************************************************/
#pragma once

#include "stdint.h"

namespace mirp
{
#pragma pack(push, 1)

	/**
	 * MIRP¿¿¿¿¿¿24¿¿¿¿
	 * ¿¿MIRP¿¿¿¿¿¿¿¿¿¿MIRP¿¿¿¿¿¿¿¿¿¿¿¿¿¿MIRP¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
	 * ¿¿
	 *
	 */
	class mirp_header_t
	{
		/*
		* ¿¿¿¿¿¿¿¿¿¿¿¿¿4¿¿4¿¿¿¿¿¿4¿(Flag&0x0F)¿¿¿¿¿¿¿
		* ¿¿¿¿¿¿1¿¿4¿¿¿¿¿¿(¿¿4¿,Flag&0x10)¿¿¿¿¿¿¿¿¿
		* ¿¿¿¿¿¿0¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		* ¿1¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿3¿¿¿¿¿¿¿¿¿
		*/
		uint8_t Flag; 

		/*
		 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 */
		int8_t TypeID;	

		/*
		 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 */
		uint16_t Length;  


		/**
		  * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		  *		* ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		**/
		int32_t PacketNo; 

		/*
		 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 */
		int16_t TopicID;  

		/*
		 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 *
		 */
		uint16_t SnapMillisec;  

		/*
		 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 */
		int32_t SnapNo;  

		/*
		 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 */
		uint32_t SnapTime;  

		/*
		 * ¿1980¿01¿01¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 */
		uint16_t CommPhaseNo;  

		/*
		 * * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 *	* ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿0¿¿¿¿¿¿¿¿¿¿¿¿¿1¿
		 */
		int8_t CenterChangeNo; 

		/*
		 * ¿¿¿¿¿
		 */
		int8_t Reserved;  
	};

	enum mirp_message_type_t
	{
		// ¿¿¿¿
		heartbeat = 0x00,

		// ¿¿¿¿¿¿¿¿
		Incremental = 0x01,
	};

	enum mirp_message_fieldid_t
	{
		// ¿¿¿¿¿¿¿¿
		contract_header = 0x0003,

		// ¿¿¿¿¿¿
		price_change_list = 0x1001,

		// ¿¿¿¿¿
		match_summary = 0x1002,

		// ¿¿¿¿
		high_price = 0x1011,

		// ¿¿¿¿
		low_price = 0x1012,

		// ¿¿¿¿
		open_price = 0x1013,

		// ¿¿¿¿
		close_price = 0x1014,

		// ¿¿¿¿¿
		upper_limit_price = 0x1015,
			
		// ¿¿¿¿¿
		lower_limit_price = 0x1016,

		// ¿¿¿¿
		settle_price =0x1017,

		// ¿¿¿¿
		curr_dalta = 0x1018,


	};
#pragma pack(pop)
}
