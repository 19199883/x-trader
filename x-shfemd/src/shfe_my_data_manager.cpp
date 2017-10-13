static void FillStatisticFields(MYShfeMarketData &des_data, SHFEMDQuoteSnapshot * const src_data)
{
    des_data.buy_total_volume = 0;
    des_data.buy_weighted_avg_price = 0;
    double sum_pv = 0;

	if (!src_data->damaged){
		for (int i = 0; i < src_data->buy_count; ++i){
			des_data.buy_total_volume += src_data->buy_volume[i];
			sum_pv += (src_data->buy_volume[i] * src_data->buy_price[i]);
		}
	}else{
		des_data.buy_total_volume = 0;
	}
    if (des_data.buy_total_volume > 0){
        des_data.buy_weighted_avg_price = sum_pv / des_data.buy_total_volume;
    }

    des_data.sell_total_volume = 0;
    des_data.sell_weighted_avg_price = 0;
    sum_pv = 0;
	if (!src_data->damaged){
		for (int i = 0; i < src_data->sell_count; ++i){
			des_data.sell_total_volume += src_data->sell_volume[i];
			sum_pv += (src_data->sell_volume[i] * src_data->sell_price[i]);
		}
	}else{des_data.sell_total_volume = 0; }
    if (des_data.sell_total_volume > 0){
        des_data.sell_weighted_avg_price = sum_pv / des_data.sell_total_volume;
    }
}

