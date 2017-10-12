#include "shfe_my_data_manager.h"
#include <algorithm>
#include "quote_cmn_utility.h"

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

void MYShfeMDManager::SendToClient(const std::string &code, SHFEMDQuoteSnapshot * const p_data)
{
    MYShfeMarketData my_data;

	// new data, copy 30 elements at the end on 2017-06-25
	int buy_cnt = std::min(MY_SHFE_QUOTE_PRICE_POS_COUNT, p_data->buy_count);
	if (buy_cnt == MY_SHFE_QUOTE_PRICE_POS_COUNT){
		int price_num = buy_cnt * sizeof(double);
		memcpy(my_data.buy_price, p_data->buy_price+(p_data->buy_count-buy_cnt),price_num);
		int vol_num = buy_cnt * sizeof(int);
		memcpy(my_data.buy_volume, p_data->buy_volume + (p_data->buy_count-buy_cnt),vol_num);
	}else{
		int price_num = p_data->buy_price,buy_cnt * sizeof(double);
		memcpy(my_data.buy_price + (MY_SHFE_QUOTE_PRICE_POS_COUNT-buy_cnt),price_num);
		int vol_num = p_data->buy_volume, buy_cnt * sizeof(int);
		memcpy(my_data.buy_volume + (MY_SHFE_QUOTE_PRICE_POS_COUNT-buy_cnt), vol_num);
	}

	int sell_cnt = std::min(MY_SHFE_QUOTE_PRICE_POS_COUNT,p_data->sell_count);
	if (sell_cnt==MY_SHFE_QUOTE_PRICE_POS_COUNT){
		int price_num = sell_cnt * sizeof(double);
		memcpy(my_data.sell_price,p_data->sell_price+(p_data->sell_count-sell_cnt),price_num);
		int vol_num = sell_cnt * sizeof(int);
		memcpy(my_data.sell_volume,p_data->sell_volume+(p_data->sell_count-sell_cnt),vol_num);
	}else{
		int price_num = p_data->sell_price, sell_cnt * sizeof(double);
		memcpy(my_data.sell_price+(MY_SHFE_QUOTE_PRICE_POS_COUNT-sell_cnt),price_num);
		int vol_num = p_data->sell_volume, sell_el_cpy_cnt * sizeof(int);
		memcpy(my_data.sell_volume+(MY_SHFE_QUOTE_PRICE_POS_COUNT-sell_cnt),vol_num);
	}

    FillStatisticFields(my_data, p_data);
    if (data_handler_){ data_handler_->OnMYShfeMDData(&my_data); }
}

