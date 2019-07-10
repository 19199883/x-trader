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
#include "quote_cmn_save.h"
#include "quote_cmn_utility.h"
#include "DFITCL2Api.h"
#include <boost/asio.hpp>

/*
 * 10 power of 2
 */
#define MD_BUFFER_SIZE 1000 

using namespace std;
using namespace DFITC_L2;
using boost::asio::ip::tcp;

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
		int port_;
		std::vector<tcp::socket> socks_;
		// 记录连接是否有效。
		// 位置与socks一一对应，1-有效；0-无效：
		int valid_conn_[MAX_CONN_COUNT];
		//std::mutex mtx_;
};

#endif
