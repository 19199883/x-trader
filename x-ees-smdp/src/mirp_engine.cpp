#include "mirp_header.h"
#include "guava_quote.h"
#include "vrt_value_obj.h"


guava_quote::guava_quote(void)
{
	m_ptr_event = NULL;
}


guava_quote::~guava_quote(void)
{
}

void guava_quote::on_receive_message(int id, const char* buff, unsigned int len)
{
	if (!m_ptr_event)
	{
		return;
	}

	if (len < sizeof(efh3_lev2) )
	{
		return;
	}

	// TODO: coding here
	// mirp header
	mirp_header_t *header = (mirp_header_t*)buff;
	
	//efh3_lev2* ptr_data = (efh3_lev2*)buff;
	//m_ptr_event->on_receive_quote(ptr_data);
}


bool guava_quote::init(multicast_info cffex, guava_quote_event* p_event)
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

void guava_quote::close()
{
	m_udp.sock_close();
}
