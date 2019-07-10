// done
#ifndef __UNI_CONSUMER_H__
#define __UNI_CONSUMER_H__


#include <functional>
#include <array>
#include <string>
#include <list>
#include <unordered_map>
#include "vrt_value_obj.h"
#include "tap_md_producer.h"
#include "l2md_producer.h"
#include <tinyxml.h>
#include <tinystr.h>
#include "moduleloadlibrarylinux.h"
#include <utility>
#include "loadlibraryproxy.h"
#include <boost/asio.hpp>
#include <mutex>          // std::mutex, std::lock_guard
#include <vector>
#include "YaoQuote.h"

using boost::asio::ip::tcp;

// 允许的最大客户端连接数
#define MAX_CONN_COUNT 10

struct Uniconfig
{
	// disruptor yield strategy
	char yield[20];
};

class UniConsumer
{
	public:
		UniConsumer(struct vrt_queue  *queue, 
					TapMDProducer *l1md_producer, 
					L2MDProducer *l2md_producer);
		~UniConsumer();

		void Start();
		void Stop();
		

	private:
		void Server();

		bool running_;
		const char* module_name_;  
		struct vrt_consumer *consumer_;
		L2MDProducer *l2_md_producer_;
		TapMDProducer *l1_md_producer_;
		void ParseConfig();

		// business logic
		void ProcL2QuoteSnapshot(YaoQuote* md);
		void FeedL2QuoteSnapshot(int32_t straidx);
		Uniconfig config_;
		
		// yao quote
		boost::asio::io_service io_service_;
		int port_;
		std::vector<tcp::socket> socks_;
		// 记录连接是否有效。
		// 位置与socks一一对应，1-有效；0-无效：
		int valid_conn_[MAX_CONN_COUNT];
		std::mutex mtx_;
};

#endif

