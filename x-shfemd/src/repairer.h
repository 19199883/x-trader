#ifndef _REPAIRER_H_
#define _REPAIRER_H_

#include <string>
#include "quote_datatype_shfe_my.h"
#include "quote_datatype_shfe_deep.h"
#include "my_int_deque.h"
#include "fulldepthmd_producer.h"
#include "vrt_value_obj.h"

using namespace std;

/*
 * this class repaires data damaged for udp package loss.
 * for more detail to see 上期全挡行情(my_quote_shfe_my_jr_ex-spd_lib).docx
 */
// TODO: dequeue to be replaced
class repairer
{
	public:
		repairer(FullDepthMDProducer *full_depth_md_producer);

		/* receive data from UDP. repaires data if it is damaged
		 */
		void rev(int32_t index);

		/*		 
		 * get next avaialble data
		 * return null if no available data
		 *
		 */
		MDPackEx* next(bool &empty);

	private:
		//
		// udp server id
		int server_;

		/*		 
		 * find normal data start point when system starts
		 */
		bool find_start_point(MDPackEx* data);

		/* check whether package loss occurs
		 * return true if package loss occurs; otherwise, false
		 */
		bool lose_pkg(MDPackEx* data);

		void print_queue();

		/* 
		 */
		void normal_proc_buy_data(int index);
		void repair_buy_data(int index);
		void normal_proc_sell_data(int index);
		void repair_sell_data(int index);
		void proc_pkg_loss(MDPackEx* data);

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

		MyIntDeque buy_queue_;
		MyIntDeque sell_queue_;

		// save data available for sending
		MyIntDeque ready_queue_;

		// initial value is false; it will become true when normal data start point is found.
		bool working_;
		// record current serial number of UDP data	
		int seq_no_;
		const char * module_name_;  
		FullDepthMDProducer *full_depth_md_producer_;

};

#endif // REPAIRER_H

