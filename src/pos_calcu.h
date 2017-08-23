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

		bool enabled(){ return enabled_; }

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

		void get_pos(string &stra, int &long_pos, int &short_pos, string &cont)
		{
			string pos_file = stra + ".pos";
			char buf[1024];
			std::ifstream is;
			is.open (pos_file);
			if (is) {
				is.getline(buf, sizeof(buf));
				string line = buf;
				
				int cur_pos = 0;
				int next_pos = 0;
				next_pos = line.find(';', cur_pos);
				string stra_id = line.substr(cur_pos, next_pos-cur_pos);
				
				cur_pos = next_pos + 1;
				next_pos = line.find(';', cur_pos);
				// commented by wangying on 2017-03-23
				// stra = line.substr(cur_pos, next_pos-cur_pos);
				
				cur_pos = next_pos + 1;
				next_pos = line.find(';', cur_pos);
				cont = line.substr(cur_pos, next_pos-cur_pos);

				cur_pos = next_pos + 1;
				next_pos = line.find(';', cur_pos);
				long_pos = stoi(line.substr(cur_pos, next_pos-cur_pos));

				cur_pos = next_pos + 1;
				short_pos = stoi(line.substr(cur_pos));

			}

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
		pos_calc(){
			TiXmlDocument config = TiXmlDocument("trasev.config");
			config.LoadFile();
			TiXmlElement *RootElement = config.RootElement();
			const char * enabled_att = RootElement->Attribute("pos_calc");
			if (NULL == enabled_att){ // if there is not pos_calc attribute, false is default value of eanbled field
						this->enabled_ = false;
			}else{
				string enabled_str = enabled_att;
				if (0 == strcmp ("on",enabled_str.c_str())){
					this->enabled_ = true;
				}else{
					this->enabled_ = false;
				}
			}
		}

		bool enabled_;
		static pos_calc* ins_; 
		static	mutex mtx_ins_;
};

#endif /* POS_CALC_H */
