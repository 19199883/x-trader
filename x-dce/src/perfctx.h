/*
 * perfctx.h
 *
 *  Created on: 24 May, 2016
 *      Author: root
 */

#ifndef PERFCTX_H_
#define PERFCTX_H_

#include <map>
#include <chrono>
#include <ctime>
#include <ratio>
#include <ctime>
#include <vector>
#include <mutex>          // std::mutex, std::lock_guard

//#define LATENCY_MEASURE

using namespace std;
using namespace std::chrono;

class perf_ctx {
public:
	perf_ctx();
	virtual ~perf_ctx();

	static void insert_t0(int data_cnt);
//	static void create_map_for_stra(int stra_id);
//	static void pop_data_without_sig(int stra_id);
//	static void pop_data_with_sig(int stra_id,int sig_id);
//	static void create_map_for_sig(int stra_id,int data_cnt,int sig_id);
	static int calcu_latency(int stra_id,int data_cnt);

private:
	//static map<int, high_resolution_clock::time_point> t0_;
	static high_resolution_clock::time_point t0_;
	//static map<int, vector<int>> stra_data_map_;
	//static map<int, map<int,int>> stra_sig_data_map_;
	static int cur_data_cnt_;
	static int cur_sig_cnt_;
	static const int MAX_SIG_CNT = 200;
	static std::mutex mtx;
};

#endif /* PERFCTX_H_ */
