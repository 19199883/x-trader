/*!****************************************************************************
 @note   Copyright (coffee), 2005-2014, Shengli Tech. Co., Ltd.
 @file   guava_quote.h
 @date   2014/8/27   12:44
 @author zhou.hu
 
 @brief   

 @note 
******************************************************************************/
#pragma once

#include "stdint.h"

#define MIRP_HEADER_LENGTH 24
#define MIRP_FIELD_HEADER_LENGTH 4

namespace mirp
{
#pragma pack(push, 1)

	/**
	 * MIRP¿¿¿¿¿¿24¿¿¿¿
	 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
	 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
	 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿ID¿¿¿¿¿¿¿¿¿¿¿¿¿
	 *
	 */
	struct mirp_header_t
	{
		/*
		 * ¿¿¿¿¿¿¿¿¿¿¿¿¿4¿¿4¿
		 * ¿¿¿¿¿4¿(Flag&0x0F)¿¿¿¿
		 * ¿¿¿
		 * ¿¿¿¿¿¿1¿¿4¿¿¿¿¿¿(¿
		 * ¿4¿,Flag&0x10)¿¿¿¿¿¿¿¿¿
		 * ¿¿¿¿¿¿0¿¿¿¿¿¿¿¿¿¿¿
		 * ¿¿¿¿¿¿¿
		 * ¿1¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 * ¿¿¿¿¿3¿¿¿¿¿¿¿¿¿
		*/
		uint8_t Flag; 

		/*
		 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 * ¿¿¿¿
		 */
		int8_t TypeID;	

		/*
		 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 */
		uint16_t Length;  


		/**
		 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 * ¿¿¿¿
		 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 * ¿¿¿
		**/
		int32_t PacketNo; 

		/*
		 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 */
		int16_t TopicID;  

		/*
		 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 * ¿¿¿¿
		 *
		 */
		uint16_t SnapMillisec;  

		/*
		 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 */
		int32_t SnapNo;  

		/*
		 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 * ¿¿¿¿
		 */
		uint32_t SnapTime;  

		/*
		 * ¿1980¿01¿01¿¿¿¿¿¿¿¿¿
		 * ¿¿¿¿¿¿¿¿¿¿
		 */
		uint16_t CommPhaseNo;  

		/*
		 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 * ¿¿¿¿¿¿¿
		 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 * ¿0¿¿¿¿¿¿¿¿¿¿¿¿¿1¿
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
		heartbeat =		0x00,

		// ¿¿¿¿¿¿¿¿
		Incremental =	0x01,
	};

	enum mirp_message_fieldid_t
	{
		// ¿¿¿¿¿¿¿
		center_switch	=	0x0032,

		// ¿¿¿¿¿¿¿¿
		contract_header =	0x0003,

		// ¿¿¿¿¿¿
		price_change_list = 0x1001,

		// ¿¿¿¿¿
		match_summary =		0x1002,

		// ¿¿¿¿
		high_price =		0x1011,

		// ¿¿¿¿
		low_price =			0x1012,

		// ¿¿¿¿
		open_price =		0x1013,

		// ¿¿¿¿
		close_price =		0x1014,

		// ¿¿¿¿¿
		upper_limit_price = 0x1015,
			
		// ¿¿¿¿¿
		lower_limit_price = 0x1016,

		// ¿¿¿¿
		settle_price =		0x1017,

		// ¿¿¿¿
		curr_dalta =		0x1018,
	};

	/*
	 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿ID¿¿¿¿¿¿¿¿¿¿
	 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
	 * ¿¿¿¿¿¿¿¿¿¿FieldSize¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
	 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
	 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
	 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
	 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
	 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿
	 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
	 *
	 */
	struct field_header_t
	{
		// ¿ID¿¿¿¿¿¿¿¿¿¿¿¿
		int16_t FieldID; 

		// ¿¿¿¿¿
		int16_t FieldSize; 
	};

	/*
	 * ¿¿¿¿¿¿¿¿(FieldID=0x0003)¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
	 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
	 *
	 */
	struct contract_header_field_t
	{
		// ¿¿¿¿¿
		VInt InstrumentNo; 

		// ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		VInt ChangeNo; 
	};

	/*
	 * ¿¿¿¿¿¿(FieldID=0x1001)¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
	 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
	 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
	 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
	 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
	 * ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
	 */
	struct price_change_list_field_t
	{
		/*
		 * ¿¿¿¿¿
		 *	¿¿¿¿¿¿
		 *  ¿1¿¿¿¿
		 *  ¿2¿¿¿¿
		 *  ¿3¿¿¿¿¿
		 */
		char EventType[1];

		/*
		*	¿¿¿¿¿
		*	¿¿¿¿¿¿
		*	¿0¿¿bid	
		*	¿1¿¿ask¿	
		*/	
		char MDEntryType[1];
			
		/*
		 * *¿¿¿¿¿¿1¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		 */
		// VInt PriceLevel;

		/*
		* ¿¿¿¿¿
		* ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
		* ¿¿¿¿¿¿¿¿¿¿¿n¿¿¿¿¿¿¿ (¿¿¿¿¿¿¿+n*¿¿¿¿¿¿¿¿¿
		 */
		// VInt PriceOffset;
		
		// ¿¿¿
		// VInt Volume; ¿
	};

	class mirp_formatter
	{
		public:
			static void format(mirp_header *header, char*buff)
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
