#ifndef REPAIRER_H
#define REPAIRER_H
#include <string>
#include <queue>
#include <deque>
#include "quote_datatype_shfe_my.h"
#include "quote_datatype_shfe_deep.h"

using namespace std;

class MDPackEx
{
	public:
		MDPackEx(): damaged(false)
		{
		}

		MDPackEx(MDPack &cur_content): damaged(false)
		{
			this->content = cur_content;
		}

		MDPack content;
		bool damaged;
};

/*
 * this class repaires data damaged for udp package loss.
 * for more detail to see 上期全挡行情(my_quote_shfe_my_jr_ex-spd_lib).docx
 */
class repairer
{
	public:
		repairer();

		/* receive data from UDP. repaires data if it is damaged
		 */
		void rev(MDPack &data);

		/*		 
		 * get next avaialble data
		 * return null if no available data
		 *
		 */
		MDPackEx next(bool &empty);
		//
		// udp server id
		int server_;

		/*		 
		 * find normal data start point when system starts
		 */
		bool find_start_point(MDPack &data);

		/* check whether package loss occurs
		 * return true if package loss occurs; otherwise, false
		 */
		bool lose_pkg(MDPack &data);

		void print_queue();

		/* 
		 */
		void normal_proc_buy_data(MDPack &data);
		void repair_buy_data(MDPack &data);
		void normal_proc_sell_data(MDPack &data);
		void repair_sell_data(MDPack &data);
		void proc_pkg_loss(MDPack &data);

		/*
		 * pull data ready to send from buy/sell queue
		 */
		void pull_ready_data();

		/* 
		 * flag un-integrity data with damaged
		 */
		void flag_damaged_data();

		 std::string ToString(const MDPack &d);

		// record the contract of victim data when package loss occurs
		string victim_;

		deque<MDPackEx> buy_queue_;
		deque<MDPackEx> sell_queue_;

		// save data available for sending
		deque<MDPackEx> ready_queue_;

		// initial value is false; it will become true when normal data start point is found.
		bool working_;
		// record current serial number of UDP data	
		int seq_no_;

};


#endif // REPAIRER_H

