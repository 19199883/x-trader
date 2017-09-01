#pragma once
	class tcs
	{
	public:
		tcs_setting setting;

	public:
		tcs(tcs_setting setting);
		~tcs(void);
		void initialize(void);
		void finalize(void);
		void place_request(const my_order &ord);
		void cancel_request(const my_order &ord);

		state_options convert_from(char state);
		void init(const long &model);
	private:
		void rev_ord_response(const T_OrderRespond *, const T_PositionData *);
		void rev_cancel_ord_response(const T_CancelRespond *);
		void rev_ord_return(const T_OrderReturn *, const T_PositionData * pos);
		void rev_trade_return(const T_TradeReturn *, const T_PositionData * pos);

	};
