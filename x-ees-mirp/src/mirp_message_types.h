/*!****************************************************************************
 @note   Copyright (coffee), 2005-2014, Shengli Tech. Co., Ltd.
 @file   guava_quote.h
 @date   2014/8/27   12:44
 @author zhou.hu
 
 @brief   

 @note 
******************************************************************************/
#pragma once

#include <stdio.h>
#include "stdint.h"

#define MIRP_HEADER_LENGTH 24
#define MIRP_FIELD_HEADER_LENGTH 4

namespace mirp
{
#pragma pack(push, 1)

	/**
	 * MIRP真真真24真真
	 * 真真真真真真真真真真真真真真真真真真�
	 * 真真真真真真真真
	 * 真真真真真真真真真真�ID真真真真真真�
	 *
	 */
	struct mirp_header_t
	{
		/*
		 * 真真真真真真�4真4�
		 * 真真�4�(Flag&0x0F)真真
		 * 真�
		 * 真真真1真4真真真(�
		 * �4�,Flag&0x10)真真真真�
		 * 真真真0真真真真真�
		 * 真真真�
		 * �1真真真真真真真真
		 * 真真�3真真真真�
		*/
		uint8_t Flag; 

		/*
		 * 真真真真真真真真真
		 * 真真
		 */
		int8_t TypeID;	

		/*
		 * 真真真真真真真
		 */
		uint16_t Length;  


		/**
		 * 真真真真真真真真真
		 * 真真
		 * 真真真真真真真真真
		 * 真�
		**/
		int32_t PacketNo; 

		/*
		 * 真真真真真真真真真
		 */
		int16_t TopicID;  

		/*
		 * 真真真真真真真真
		 * 真真真真真真真真�
		 * 真真
		 *
		 */
		uint16_t SnapMillisec;  

		/*
		 * 真真真真真真真真
		 * 真真真真真真真真真
		 */
		int32_t SnapNo;  

		/*
		 * 真真真真真真真�
		 * 真真真真真真真真�
		 * 真真
		 */
		uint32_t SnapTime;  

		/*
		 * �1980�01�01真真真真�
		 * 真真真真真
		 */
		uint16_t CommPhaseNo;  

		/*
		 * 真真真真真真真真�
		 * 真真真�
		 * 真真真真真真真真�
		 * �0真真真真真真�1�
		 */
		int8_t CenterChangeNo; 

		/*
	     * 真真�
		 */
		int8_t Reserved;  
	};

	enum mirp_message_type_t
	{
		// 真真
		heartbeat =		0x00,

		// 真真真真
		Incremental =	0x01,
	};

	enum mirp_message_fieldid_t
	{
		// 真真真�
		center_switch	=	0x0032,

		// 真真真真
		contract_header =	0x0003,

		// 真真真
		price_change_list = 0x1001,

		// 真真�
		match_summary =		0x1002,

		// 真真
		high_price =		0x1011,

		// 真真
		low_price =			0x1012,

		// 真真
		open_price =		0x1013,

		// 真真
		close_price =		0x1014,

		// 真真�
		upper_limit_price = 0x1015,
			
		// 真真�
		lower_limit_price = 0x1016,

		// 真真
		settle_price =		0x1017,

		// 真真
		curr_dalta =		0x1018,
	};

	/*
	 * 真真真真真真真真真真真真真�ID真真真真真
	 * 真真真真真真真真�
	 * 真真真真真FieldSize真真真真真真真真真真真�
	 * 真真真真真真真真
	 * 真真真真真真真真真真真真真真真真真真真
	 * 真真真真真真真�
	 * 真真真真真真真真真真真真真真真真真真真�
	 * 真真真真真真真
	 * 真真真真真真真真�
	 *
	 */
	struct field_header_t
	{
		// �ID真真真真真真
		int16_t FieldID; 

		// 真真�
		int16_t FieldSize; 
	};

	/*
	 * 真真真真(FieldID=0x0003)真真真真真真真真真真
	 * 真真真真真真真�
	 *
	 */
	struct contract_header_field_t
	{
		// 真真�
	//	VInt InstrumentNo; 

	//	// 真真真真真真真真真真真真真真真真真
	//	VInt ChangeNo; 
	};

	/*
	 * 真真真(FieldID=0x1001)真真真真真真真真真真真
	 * 真真真真真真真�
	 * 真真真真真真真真真真真真真真真真真真
	 * 真真真真真真真�
	 * 真真真真真真真真真真真真真真真真真真
	 * 真真真真真真真�
	 */
	struct price_change_list_field_t
	{
		/*
		 * 真真�
		 *	真真真
		 *  �1真真
		 *  �2真真
		 *  �3真真�
		 */
		char EventType[1];

		/*
		*	真真�
		*	真真真
		*	�0真bid	
		*	�1真ask�	
		*/	
		char MDEntryType[1];
			
		/*
		 * *真真真1真真真真真真真真真真真
		 */
		// VInt PriceLevel;

		/*
		* 真真�
		* 真真真真真真真真真真真真真真真真真真�
		* 真真真真真�n真真真� (真真真�+n*真真真真�
		 */
		// VInt PriceOffset;
		
		// 真�
		// VInt Volume; �
	};

	class mirp_formatter
	{
		public:
			static void format(mirp_header_t *header, char*buff)
			{
				sprintf (buff,
					"mirp_header "
					"Flag:%u; " 
					"TypeID:%d; "
					"Length:%u; "
					"PacketNo:%d; "
					"TopicID:%d; "
					"SnapMillisec:%u; "
					"SnapNo:%d; "
					"SnapTime:%u; "
					"CommPhaseNo:%u; "
					"CenterChangeNo:%d; "
					"Reserved:%d; ",
					header-> Flag, 
					header-> TypeID,	
					header-> Length,  
					header-> PacketNo, 
					header-> TopicID, 
					header-> SnapMillisec,
					header-> SnapNo,
					header-> SnapTime,  
					header-> CommPhaseNo,
					header-> CenterChangeNo,
					header-> Reserved);

			}
	};

#pragma pack(pop)
}
