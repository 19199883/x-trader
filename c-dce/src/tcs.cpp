void tcs::place_request(const my_order &ord)
{
		T_PlaceOrder chn_ord;
		chn_ord.direction = ord.side;
		chn_ord.limit_price = ord.price;
		chn_ord.open_close = ord.position_effect;
		chn_ord.order_kind = ord.price_type;
		chn_ord.order_type = ord.ord_type;
		chn_ord.serial_no = ord.cl_ord_id;
		chn_ord.speculator = ord.sah_;
		chn_ord.exchange_type = ord.exchange;

		strcpy(chn_ord.stock_code,ord.symbol.c_str());
		chn_ord.volume = ord.volume;

		this->channel->PlaceOrder(&chn_ord);
}

void tcs::cancel_request(const my_order &ord) {
		T_CancelOrder chn_ord;
 		chn_ord.direction = ord.side;
		chn_ord.limit_price = ord.price;
		chn_ord.open_close = ord.position_effect;
		chn_ord.serial_no = ord.cl_ord_id;
		chn_ord.speculator = '0';
		strcpy(chn_ord.stock_code,ord.symbol.c_str());
		chn_ord.volume = ord.volume;
		chn_ord.org_serial_no = ord.orig_cl_ord_id;
		chn_ord.entrust_no = ord.orig_ord_id;
		chn_ord.exchange_type = ord.exchange;

		this->channel->CancelOrder(&chn_ord);
}

state_options tcs::convert_from(char state) {

		if ((char)state == '0' ||
			(char)state == 'n' ||
			(char)state == 's') {
			return state_options::pending_new;
		} else if ((char)state == 'a') {
			return state_options::new_state;
		} else if ((char)state == 'p') {
			return state_options::partial_filled;
		} else if ((char)state == 'c') {
			return state_options::filled;
		} else if ((char)state == 'e' || (char)state == 'q') {
			return state_options::rejected;
		} else if ( state == 'b') {
			return state_options::cancelled;
		} else if ((char)state == 'd' ) {
			return state_options::cancelled;
		} else if ((char)state == 'f') {
			return state_options::pending_cancel;
		} else {
			return state_options::undefined2;
		}
}

bool tcs::is_final(OrderDetail &ord)
{
	if(ord.entrust_status == 'c' 	//全成
		||ord.entrust_status == 'd'	//已撤
		||ord.entrust_status =='e' 	//错误报单
		){
		return false;
	}else{
		return true;
	}
}




