#include "guava_quote.h"

void guava_quote::on_receive_message(const char* buff, unsigned int len)
{
	if (!m_ptr_event)
	{
		return;
	}

	if (len < sizeof(efh3_lev2) )
	{
		return;
	}

	efh3_lev2* ptr_data = (efh3_lev2*)buff;
	m_ptr_event->on_receive_quote(ptr_data);
}

