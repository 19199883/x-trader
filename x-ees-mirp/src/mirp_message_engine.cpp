#include "mirp_message_types.h"
#include "vrt_value_obj.h"
#include "mirp_message_engine.h"
#include "vint.h"

using namespace mirp;

mirp_message_engine::mirp_message_engine(void)
:module_name_("mirp_message_engine")
{
	m_ptr_event = NULL;
}


mirp_message_engine::~mirp_message_engine(void)
{
}

void mirp_message_engine::on_receive_message(
			int id, 
			const char* buff, 
			unsigned int len)
{
	if (!m_ptr_event) { return; }

	if (len < MIRP_HEADER_LENGTH)
	{
		clog_error("[%s] header length is: %d", module_name_, len);
		return;
	}

	mirp_header_t *header = (mirp_header_t*)buff;
	char log_buff[5120];
	//clog_info("[%s] rev header %s",
	//			module_name_,
	//			mirp_formatter::format(header,log_buff));

	switch (header->TypeID)
	{
		case mirp_message_type_t::heartbeat:
			process_heartbeat_message();
			break;
		case mirp_message_type_t::Incremental:
			process_incremental_message(buff + MIRP_HEADER_LENGTH,header->Length);
			break;
		default:
			clog_error("[%s] unexpected typeid: %d", module_name_, header->TypeID);
			break;
	}
}

bool mirp_message_engine::init(
			multicast_info cffex, 
			mirp_message_engine_event* p_event)
{
	m_udp_conf = cffex;
	m_ptr_event = p_event;
	
	bool ret = m_udp.sock_init(m_udp_conf.m_remote_ip, 
				m_udp_conf.m_remote_port, 
				m_udp_conf.m_local_ip, 
				m_udp_conf.m_local_port, 
				0, 
				this);
	if (!ret) { return false; }

	return true;
}

void mirp_message_engine::close()
{
	m_udp.sock_close();
}

void mirp_message_engine::process_heartbreat_meesage()
{
	clog_info("[%s] heartbeat.", module_name_);
}

void mirp_message_engine::process_incremental_message(
			const char* message_addr, 
			unsigned int message_len)
{
	const char *field_addr = message_addr;

	// 合约增量行情头域
	field_header_t *field_header = (field_header_t*)field_addr;
	do
	{
		switch (field_header->FieldID)
		{
			case mirp_message_fieldid_t::contract_header:
				process_contract_header_field(field_addr + MIRP_FIELD_HEADER_LENGTH);
				break;
			default:
				clog_error("[%s] unexpected fieldid: %d", module_name_, field_header->FieldID);
				break;
		}

		field_addr = field_addr + MIRP_FIELD_HEADER_LENGTH + field_header->FieldSize;

		// TODO: code here
		// remove the below after debugging
		return;
	} while(true); // TODO: coding here

}

void mirp_message_engine::process_contract_header_field(const char* field_body_addr)
{
	uint8_t decoded_bytes = 0;
	int64_t intrument_no = vint::decode_signed_varint((const uint8_t*)field_body_addr, decoded_bytes);
	int64_t change_no = vint::decode_signed_varint((const uint8_t*)data + decoded_bytes, decoded_bytes);
	clog_info("[%s] instrument no:%d; change no:%d", 
				module_name_, 
				instrumen_no,
				change_no);

}
