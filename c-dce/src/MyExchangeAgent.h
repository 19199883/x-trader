/*
 * MyExchangeAgent.h
 *
 *  Created on: Jun 18, 2015
 *      Author: root
 */

#ifndef MYEXCHANGEAGENT_H_
#define MYEXCHANGEAGENT_H_

#include <string>
#include <map>
#include "../my_exchange.h"
#include "my_trade_tunnel_api.h"

namespace trading_channel_agent {

using namespace std;


typedef MYTunnelInterface *CreateTradeTunnel_t(const std::string &tunnel_config_file);
typedef void DestroyTradeTunnel_t(MYTunnelInterface *);

typedef MYExchangeInterface *CreateExchange_t(const struct my_xchg_cfg &future_exchange_config_file);
typedef void DestroyExchange_t(MYExchangeInterface *);

class MyExchangeFactory {
public:
	static MYExchangeInterface * create(string ex_so_file,struct my_xchg_cfg & tunnel_conf);
	static void destroy(string ex_so_file,MYExchangeInterface * ex);

private:
	static map<string,void*> so_handles_;
	static map<string,DestroyExchange_t*> destroies_;
};

} /* namespace trading_channel_agent */

#endif /* MYEXCHANGEAGENT_H_ */
