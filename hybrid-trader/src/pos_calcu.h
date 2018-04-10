#ifndef POS_CALC_H 
#define POS_CALC_H

#include <tinyxml.h>
#include <tinystr.h>
#include <tinyxml.h>
#include <tinystr.h>
#include <map>
#include <list>
#include <thread>         // std::thread
#include <mutex>          // std::mutex, std::lock_guard
#include <iostream>     // std::cin, std::cout
#include <string>
#include <vector>
#include <fstream>      // std::ifstream

using namespace std;

class pos_calc
{
	public:
		static pos_calc* instance();

		static void destroy_instance();

		bool exists(string &stra){
			string pos_file = stra + ".pos";
			ifstream is;
			is.open (pos_file);
			if (is) {
				return true;
			}else{
				return false;
			}
			is.close();
		}

		void get_pos(const string &stra, const string contract, int &long_pos, int &short_pos, string &cont)
		{
			// TODO: to be tested
			string pos_file = stra + ".pos";
			char buf[1024];
			std::ifstream is;
			is.open (pos_file);
			if(is){
				while(is.getline(buf, sizeof(buf))){
					string line = buf;
					int cur_pos = 0;
					int next_pos = 0;

					next_pos = line.find(';', cur_pos);
					string stra_id = line.substr(cur_pos, next_pos-cur_pos);
					
					cur_pos = next_pos + 1;
					next_pos = line.find(';', cur_pos);
					stra = line.substr(cur_pos, next_pos-cur_pos);
					
					cur_pos = next_pos + 1;
					next_pos = line.find(';', cur_pos);
					cont_tmp = line.substr(cur_pos, next_pos-cur_pos);
					if(cont_tmp==contract){
						cur_pos = next_pos + 1;
						next_pos = line.find(';', cur_pos);
						long_pos = stoi(line.substr(cur_pos, next_pos-cur_pos));
						cur_pos = next_pos + 1;
						short_pos = stoi(line.substr(cur_pos));
						break;
					}
				} // end while(is.getline(buf, sizeof(buf)))
			} // end if(is)

			is.close();
		}

		/* 
		 * pos_sum:key,contract; value,total position
		 * para:
		 *		pos_sum: key-contract; value-(long position, short position)
		 *
		 */
		void sum(map<string,vector<int>> &pos_sum)
		{
			string pos_file = "pos_sum.pos";
			char buf[1024];
			std::ifstream is;
			is.open (pos_file);
			if (is) {
				while(is.getline(buf, sizeof(buf))){
					string line = buf;
					int cur_pos = 0;
					int next_pos = 0;
					next_pos = line.find(';', cur_pos);
					string cont = line.substr(cur_pos, next_pos-cur_pos);
					
					if(pos_sum.count(cont)<=0){ pos_sum[cont] = vector<int>(2,0); }

					cur_pos = next_pos + 1;
					next_pos = line.find(';', cur_pos);
					pos_sum[cont][0] = stoi(line.substr(cur_pos, next_pos-cur_pos));

					cur_pos = next_pos + 1;
					pos_sum[cont][1] = stoi(line.substr(cur_pos));
				}
			}

			is.close();
		}

		/*
		 * get list of strategy names
		 */
		void get_stras(list<string> &stras)
		{
			string pos_file = "stra_names.pos";
			char buf[1024];
			std::ifstream is;
			is.open (pos_file);
			if (is) {
				if (is.getline(buf, sizeof(buf))){
					string line = buf;
					if (!line.empty()) line += ",";
					int cur_pos = 0;
					int next_pos = 0;
					while (string::npos != (next_pos=line.find(',', cur_pos))){
						string cont = line.substr(cur_pos, next_pos-cur_pos);
						stras.push_back(cont);
						cur_pos = next_pos + 1;
					}
				}
			}

			is.close();
		}

	private:
		pos_calc(){ }

		static pos_calc* ins_; 
		static	mutex mtx_ins_;
};

#endif /* POS_CALC_H */
