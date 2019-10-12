#include "mirp_header.h"
#include "guava_quote.h"
#include "vrt_value_obj.h"
#include "mirp_engine.h"

using namespace mirp;

mirp_message_engine::mirp_message_engine(void)
{
	m_ptr_event = NULL;
}


mirp_message_engine::~mirp_message_engine(void)
{
}

void mirp_message_engine::on_receive_message(int id, const char* buff, unsigned int len)
{
	if (!m_ptr_event)
	{
		return;
	}

	if (len < MIRP_HEADER_LENGTH)
	{
		clog_error("[%s] header length is: %d", module_name_, len);
		return;
	}

	// TODO: coding here
	mirp_header_t *header = (mirp_header_t*)buff;
	char log_buff[5120];
	clog_info("[%s] rev header %s",
				module_name_,
				mirp_formatter::format(hdader,log_buff));

	switch (header->TypeID)
	{
		case mirp_message_type_t::heartbeat:
			process_heartbeat();
			break;
		case mirp_message_type_t::Incremental:
			process_incremental(buff + MIRP_HEADER_LENGTH,header->Length);
			break;
		default:
			clog_error("[%s] unexpected typeid: %d", module_name_, header->TypeID);
			break;
	}
	
	
	//efh3_lev2* ptr_data = (efh3_lev2*)buff;
	//m_ptr_event->on_receive_quote(ptr_data);
}


bool mirp_message_engine::init(multicast_info cffex, mirp_message_engine_event* p_event)
{
	m_udp_conf = cffex;
	m_ptr_event = p_event;
	
	bool ret = m_udp.sock_init(m_udp_conf.m_remote_ip, m_udp_conf.m_remote_port, m_udp_conf.m_local_ip, m_udp_conf.m_local_port, 0, this);
	if (!ret)
	{
		return false;
	}

	return true;
}

void mirp_message_engine::close()
{
	m_udp.sock_close();
}

void mirp_message_engine::process_heartbreat()
{
	clog_info("[%s] heartbeat.", module_name_);
}

void mirp_message_engine::process_incremental(const char* buff, unsigned int len)
{
	char *field_addr = buff;

	// TODO: code here
	// 合约增量行情头域
	field_header_t *field_header = (field_header_t*)field_addr;

}
