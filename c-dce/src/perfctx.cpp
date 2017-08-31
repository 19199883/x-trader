/*
 * perfctx.cpp
 *
 *  Created on: 24 May, 2016
 *      Author: root
 */

#include "perfctx.h"

//map<int, high_resolution_clock::time_point> perf_ctx::t0_;
high_resolution_clock::time_point perf_ctx::t0_ ;
int perf_ctx::cur_sig_cnt_= 0;
int perf_ctx::cur_data_cnt_ = 0;
std::mutex perf_ctx::mtx;
//map<int, vector<int>> perf_ctx::stra_data_map_;
//map<int, map<int,int>> perf_ctx::stra_sig_data_map_;

perf_ctx::perf_ctx() {
	// TODO Auto-generated constructor stub

}

perf_ctx::~perf_ctx() {
	// TODO Auto-generated destructor stub
}

void perf_ctx::insert_t0(int data_cnt){
	if(cur_sig_cnt_ < MAX_SIG_CNT){
		std::lock_guard<std::mutex> lck (mtx);
		cur_data_cnt_ = data_cnt;
		t0_ = high_resolution_clock::now();
	}
}

//void perf_ctx::create_map_for_stra(int stra_id){
//	if(cur_sig_cnt_ < MAX_SIG_CNT){
//		stra_data_map_[stra_id].push_back(cur_data_cnt_);
//	}
//}
//
//void perf_ctx::pop_data_without_sig(int stra_id){
//	if(cur_sig_cnt_ < MAX_SIG_CNT){
//		vector<int> &tmp = stra_data_map_[stra_id];
//		if(tmp.size() > 0){
//			tmp.erase(tmp.begin());
//		}
//	}
//}
//
//void perf_ctx::pop_data_with_sig(int stra_id,int sig_id){
//	if(cur_sig_cnt_ < MAX_SIG_CNT){
//		vector<int> &tmp = stra_data_map_[stra_id];
//		if(tmp.size() > 0){
//			stra_sig_data_map_[stra_id][sig_id] = tmp[0];
//			tmp.erase(tmp.begin());
//		}
//	}
//}

int perf_ctx::calcu_latency(int stra_id,int sig_id){
	cur_sig_cnt_++;
	if(cur_sig_cnt_ < MAX_SIG_CNT){
		std::lock_guard<std::mutex> lck (mtx);
//		int data_cnt = stra_sig_data_map_[stra_id][sig_id];
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		return (t1.time_since_epoch().count() - t0_.time_since_epoch().count()) / 1000;
	}else{
		return -1;
	}
}
