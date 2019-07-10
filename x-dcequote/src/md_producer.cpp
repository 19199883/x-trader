#include <functional>   // std::bind
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "md_producer.h"
#include "quote_cmn_utility.h"

using namespace std;
using namespace std::placeholders;
using namespace DFITC_L2;
using boost::asio::ip::udp;


MDProducer::MDProducer()
:module_name_("MDProducer"), io_service_()
{
//	memset(valid_conn_, 0, sizeof(valid_conn_));
//	for(int i=0; i<MAX_CONN_COUNT; i++){
//		socks_.push_back(tcp::socket(io_service_));
//	}
	
	ended_ = false;
	clog_warning("[%s] MD_BUFFER_SIZE: %d;", module_name_, MD_BUFFER_SIZE);

	ParseConfig();

	fflush (Log::fp);
}

void MDProducer::ParseConfig()
{
	TiXmlDocument config = TiXmlDocument("x-trader.config");
    config.LoadFile();
    TiXmlElement *RootElement = config.RootElement();    

	// yield strategy
    TiXmlElement *comp_node = RootElement->FirstChildElement("Disruptor");
	if (comp_node != NULL){
		strcpy(config_.yield, comp_node->Attribute("yield"));
	} else { clog_error("[%s] x-trader.config error: Disruptor node missing.", module_name_); }

	// addr
    TiXmlElement *fdmd_node = RootElement->FirstChildElement("Md");
	if (fdmd_node != NULL){
		config_.addr = fdmd_node->Attribute("addr");
		strcpy(config_.user, fdmd_node->Attribute("user"));
		strcpy(config_.pwd, fdmd_node->Attribute("pwd"));
		this->broadcast_port_ = atoi(fdmd_node->Attribute("broadcast_port"));
		strcpy(broadcast_ip_, fdmd_node->Attribute("broadcast_ip"));

	} else { 
		clog_error("[%s] x-shmd.config error: FulldepthMd node missing.", module_name_); 
	}

	// contracts file
    TiXmlElement *contracts_file_node = RootElement->FirstChildElement("Subscription");
	if (contracts_file_node != NULL){
		strcpy(config_.contracts_file, contracts_file_node->Attribute("contracts"));
	} else { clog_error("[%s] x-shmd.config error: Subscription node missing.", module_name_); }

	size_t ipstr_start = config_.addr.find("//")+2;
	size_t ipstr_end = config_.addr.find(":",ipstr_start);
	strcpy(config_.ip, config_.addr.substr(ipstr_start,ipstr_end-ipstr_start).c_str());
	config_.port = stoi(config_.addr.substr(ipstr_end+1));
}

MDProducer::~MDProducer()
{	
	DELETE_CONNECTOR(api_);
	delete sock_;
	delete broadcast_endpoint_ ;
}

int MDProducer::InitMDApi()
{
	//std::thread(&MDProducer::Server, this).detach();
	Server();
	
	api_ = NEW_CONNECTOR();
	if(NULL==api_){
		clog_error("[%s] CreateUdpFD failed.",module_name_);
	}
	// 0为udp接收行情，1为tcp接收行情，2为多播接收行情
	int err = api_->Connect((char*)config_.addr.c_str(), this, 2); 
	clog_error("[%s] addr:%s.Connect: %d",module_name_, config_.addr.c_str(), err);

    return 0;
}

void  MDProducer::OnConnected()
{
	clog_warning("[%s] OnConnected.",module_name_);

	struct DFITCUserLoginField data;
	strcpy(data.accountID, config_.user);
	strcpy(data.passwd, config_.pwd);
	int err = api_->ReqUserLogin(&data);
	clog_warning("[%s] user:%s; pwd:%s. ReqUserLogin: %d", 
				module_name_,
				this->config_.user,
				this->config_.pwd,
				err);
}

void  MDProducer::OnDisconnected(int pReason)
{
	clog_warning("[%s] OnDisconnected: %d", module_name_, pReason);
}

void  MDProducer::OnRspUserLogin(struct ErrorRtnField * pErrorField)
{
	clog_warning("[%s]  OnRspUserLogin ErrorID: %d; ErrorMsg: %s", 
				module_name_,
			 	pErrorField->ErrorID,
				pErrorField->ErrorMsg);

	//api_->SubscribeAll();
}

void  MDProducer::OnRspUserLogout(struct ErrorRtnField * pErrorField)
{
	clog_warning("[%s]  OnRspUserLogout ErrorID: %d; ErrorMsg: %s", 
				module_name_,
			 	pErrorField->ErrorID,
				pErrorField->ErrorMsg);
}

void  MDProducer::OnRspSubscribeMarketData(struct ErrorRtnField * pErrorField)
{
	clog_warning("[%s]  OnRspSubscribeMarketData ErrorID: %d; ErrorMsg: %s", 
				module_name_,
			 	pErrorField->ErrorID,
				pErrorField->ErrorMsg);
}

void  MDProducer::OnRspUnSubscribeMarketData(ErrorRtnField * pErrorField)
{
	clog_warning("[%s]  OnRspUnSubscribeMarketData ErrorID: %d; ErrorMsg: %s", 
				module_name_,
			 	pErrorField->ErrorID,
				pErrorField->ErrorMsg);
}

void  MDProducer::OnRspSubscribeAll(struct ErrorRtnField * pErrorField)
{
	clog_warning("[%s]  OnRspSubscribeAll ErrorID: %d; ErrorMsg: %s", 
				module_name_,
			 	pErrorField->ErrorID,
				pErrorField->ErrorMsg);
}

void  MDProducer::OnRspUnSubscribeAll(struct ErrorRtnField * pErrorField)
{
	clog_warning("[%s]  OnRspUnSubscribeAll ErrorID: %d; ErrorMsg: %s", 
				module_name_,
			 	pErrorField->ErrorID,
				pErrorField->ErrorMsg);
}

void  MDProducer::OnRspModifyPassword(struct ErrorRtnField * pErrorField)
{
}

void  MDProducer::OnHeartBeatLost()
{
	clog_warning("[%s]  OnHeartBeatLost", module_name_);
}

void  MDProducer::OnBestAndDeep(MDBestAndDeep * const p, UINT4 SequenceNo)
{
	char buffer[8192];
	int total = 1 + sizeof(MDBestAndDeep);
	//sock_->send_to(boost::asio::buffer(buffer, total), *broadcast_endpoint_);
	
	clog_warning("[%s] OnBestAndDeep sn:%d", module_name_, SequenceNo);

	// TODO:
	//for(int i=0; i< MAX_CONN_COUNT; i++){		
	//	{
	//		//std::lock_guard<std::mutex> lck (mtx_);
	//		if(0 == valid_conn_[i]) continue;
	//	}
	//	
	//	try{			
	//		 boost::system::error_code error;	
	//		 buffer[0] = EDataType::eMDBestAndDeep;
	//		 memcpy(buffer+1, p, sizeof(MDBestAndDeep));
	//		 boost::asio::write(socks_[i], boost::asio::buffer(buffer, total), error);			  
	//		 if (error){
	//			 valid_conn_[i] = 0;
	//			clog_warning("[%s] write error: %d", module_name_, error); 			
	//		 }
	//	}
	//	catch (std::exception& e){
	//		valid_conn_[i] = 0;
	//		clog_warning("[%s] send error:%s", module_name_, e.what()); 			
	//	}
	//}
	
}
void  MDProducer::OnArbi(MDBestAndDeep * const pQuote, UINT4 SequenceNo)
{
	clog_warning("[%s] OnArbi  sn:%d", module_name_, SequenceNo);
}

void  MDProducer::OnTenEntrust(MDTenEntrust * const pQuote, UINT4 SequenceNo)
{
	clog_warning("[%s] OnTenEntrust sn:%d", module_name_, SequenceNo);
}

void  MDProducer::OnRealtime(MDRealTimePrice * const pQuote, UINT4 SequenceNo)
{
	clog_warning("[%s] OnRealtime  sn:%d", module_name_, SequenceNo);
}

void  MDProducer::OnOrderStatistic(MDOrderStatistic * const p, UINT4 SequenceNo)
{
	char buffer[8192];
	int total = 1 + sizeof(MDOrderStatistic);
	buffer[0] = EDataType::eMDOrderStatistic;
	 memcpy(buffer+1, p, sizeof(MDOrderStatistic));
	//sock_->send_to(boost::asio::buffer(buffer, total), *broadcast_endpoint_);
	
	clog_warning("[%s] OnOrderStatistic sn:%d", module_name_, SequenceNo);

	// TODO:
//	for(int i=0; i< MAX_CONN_COUNT; i++){		
//		{
//			// std::lock_guard<std::mutex> lck (mtx_);
//			if(0 == valid_conn_[i]) continue;
//		}
//		
//		try{			
//			 boost::system::error_code error;	
//			buffer[0] = EDataType::eMDOrderStatistic;
//			 memcpy(buffer+1, p, sizeof(MDOrderStatistic));
//			 boost::asio::write(socks_[i], boost::asio::buffer(buffer, total), error);			  
//			 if (error){
//				 valid_conn_[i] = 0;
//				clog_warning("[%s] write error: %d", module_name_, error); 			
//			 }
//		}
//		catch (std::exception& e){
//			valid_conn_[i] = 0;
//			clog_warning("[%s] send error:%s", module_name_, e.what()); 			
//		}
//	}	
}

void  MDProducer::OnMarchPrice(MDMarchPriceQty * const pQuote, UINT4 SequenceNo)
{
	clog_warning("[%s] OnMarchPrice sn:%d", module_name_, SequenceNo);
}

void MDProducer::End()
{
	if(!ended_){
		ended_ = true;		
		clog_warning("[%s] End exit", module_name_);
	}
}

void MDProducer::Server()
{
	// Server binds to any address and any port.
	sock_ = new udp::socket(io_service_, udp::endpoint(udp::v4(), 0));
	sock_->set_option(boost::asio::socket_base::broadcast(true));
	 broadcast_endpoint_ = new udp::endpoint(boost::asio::ip::address_v4::from_string(broadcast_ip_), broadcast_port_);

	//boost::asio::io_context io_context;
//	boost::asio::ip::address addr;
//	addr = addr.from_string("172.18.80.4"); 
//	udp::endpoint local_endpoint(udp::v4(), port_);
//	local_endpoint.address(addr);
//  udp::acceptor a(io_service_, (udp::v4(), port_));
//  while(!ended_)
//  {	
//	int i = 0;
//	for(; i<MAX_CONN_COUNT; i++){
//		if(0 == valid_conn_[i]) break;
//	}
//	if(i < MAX_CONN_COUNT){
//		a.accept(socks_[i]);
//		{
//			// std::lock_guard<std::mutex> lck (mtx_);
//			valid_conn_[i] = 1;
//		}
//	}
//	else{
//		clog_warning("[%s] socks_ is full.", module_name_);
//	}
//  }
}
