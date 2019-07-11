#ifndef __MD_PRODUCER_H__
#define __MD_PRODUCER_H__

#include <functional>
#include <array>
#include <thread>         
#include <chrono>        
#include <string>        
#include "vrt_value_obj.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "DFITCL2Api.h"
#include <boost/asio.hpp>

/*
 * 10 power of 2
 */
#define MD_BUFFER_SIZE 1000 

using namespace std;
using namespace DFITC_L2;
using boost::asio::ip::udp;

struct Mdconfig
{
	string addr;
	char ip[30];
	int port;
	char contracts_file[500];
	char yield[20]; // disruptor yield strategy
	char user[20];
	char pwd[20];
};

enum EDataType
{
    eMDBestAndDeep = 0,
    eArbiBestAndDeep,
    eMDTenEntrust,
    eMDRealTimePrice,
    eMDOrderStatistic,
    eMDMarchPriceQty,
};

class MDProducer : public DFITC_L2::DFITCL2Spi
{
	public:
		MDProducer();
		~MDProducer();
		
		void End();
		int InitMDApi();
		
		void OnConnected();
		void OnDisconnected(int pReason);
		void OnRspUserLogin(struct ErrorRtnField * pErrorField);
		void OnRspUserLogout(struct ErrorRtnField * pErrorField);
		void OnRspSubscribeMarketData(struct ErrorRtnField * pErrorField);
		void OnRspUnSubscribeMarketData(ErrorRtnField * pErrorField);
		void OnRspSubscribeAll(struct ErrorRtnField * pErrorField);
		void OnRspUnSubscribeAll(struct ErrorRtnField * pErrorField);
		void OnRspModifyPassword(struct ErrorRtnField * pErrorField);
		void OnHeartBeatLost();

		void OnBestAndDeep(MDBestAndDeep * const pQuote, UINT4 SequenceNo);
		void OnArbi(MDBestAndDeep * const pQuote, UINT4 SequenceNo);
		void OnTenEntrust(MDTenEntrust * const pQuote, UINT4 SequenceNo);
		void OnRealtime(MDRealTimePrice * const pQuote, UINT4 SequenceNo);
		void OnOrderStatistic(MDOrderStatistic * const pQuote, UINT4 SequenceNo);
		void OnMarchPrice(MDMarchPriceQty * const pQuote, UINT4 SequenceNo);

	private:		
		const char *module_name_;  

		DFITCL2Api * api_;

		bool ended_;
		Mdconfig config_;
		void ParseConfig();		

	
		void Server();
		boost::asio::io_service io_service_;
		int broadcast_port_;
		char broadcast_ip_[20];
		udp::socket *sock_;
		udp::endpoint *broadcast_endpoint_;
};

#endif
