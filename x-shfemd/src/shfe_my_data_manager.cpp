#include "shfe_my_data_manager.h"
#include <algorithm>
#include "quote_cmn_utility.h"

std::string MYShfeMDManager::ToString(const MYShfeMarketData &d) {
	MY_LOG_DEBUG("MYShfeMarketData: instrument:%s, data_flag:%d,"
		"buy_total_volume:%d; sell_total_volume:%d; "
		"buy_weighted_avg_price:%lf; sell_weighted_avg_price:%lf",
		d.InstrumentID, d.data_flag, d.buy_total_volume,
		d.sell_total_volume,d.buy_weighted_avg_price,
		d.sell_weighted_avg_price);

	MY_LOG_DEBUG("dir:buy; price, volume");
	for(int i = 0; i < 30; i++) {
		 MY_LOG_DEBUG("price%d: %lf, volume%d: %d\n", i, d.buy_price[i], i, d.buy_volume[i]);
	}

	MY_LOG_DEBUG("dir:sell; price, volume");
	for(int i = 0; i < 30; i++) {
		 MY_LOG_DEBUG("price%d: %lf, volume%d: %d\n", i, d.sell_price[i], i, d.sell_volume[i]);
	}
  
  return "";
}

MYShfeMDManager::MYShfeMDManager(const ConfigData &cfg) : cfg_(cfg) { running_flag_ = true; }
MYShfeMDManager::~MYShfeMDManager() { running_flag_ = false; }
void MYShfeMDManager::OnMBLData(const CShfeFtdcMBLMarketDataField* const pdata, bool last_flag) { }
void* MYShfeMDManager::ProcessThread(MYShfeMDManager* p_mngr)
{
    while (p_mngr->running_flag_){
        for (MBLDataCollection::iterator p = p_mngr->data_handle_.begin(); p != p_mngr->data_handle_.end(); ++p) {
            if (p->isLast) { // snapshot is completed, should send to client
                if (p->field.Volume > 0) { // valid data
                    cur_code = p->field.InstrumentID;
                    (void) p_mngr->PushDataToBuffer(cur_code, p);
                }
                // 帧结束了，将没有发送的合约数据全部发出 // TODO:repairer已覆盖该场景
                bool send_cur_flag = false;
                while (p_mngr->GetLeftCode(prev_code)) {
                    SHFEMDQuoteSnapshot * p_snapshot_prev = p_mngr->GetDataCache(prev_code);
                    p_mngr->SendToClient(prev_code, p_snapshot_prev);
                }
                p_mngr->ResetBuyCode();
                continue;
            }
            if (p->field.Volume > 0) { // 有效数据
                cur_code = p->field.InstrumentID;
                cur_dir = p->field.Direction;
                (void) p_mngr->PushDataToBuffer(cur_code, p);

				if (prev_dir==SHFE_FTDC_D_Buy && 
					cur_dir == SHFE_FTDC_D_Sell) { // 第一个卖方向数据，核对前方是否有涨停合约，该类合约无卖方向数据，应该发出
					// TODO:repairer已覆盖该场景
                    while (p_mngr->GetPrevCode(cur_code, prev_code)) {
                        SHFEMDQuoteSnapshot * p_snapshot_prev = p_mngr->GetDataCache(prev_code);
                        p_mngr->SendToClient(prev_code, p_snapshot_prev);
                    }
                }
                prev_code = cur_code;
                prev_dir = cur_dir;
                continue;
            }
        }
    }
}

SHFEMDQuoteSnapshot *MYShfeMDManager::PushDataToBuffer(const std::string &cur_code,
			MBLDataCollection::iterator &p)
{
    SHFEMDQuoteSnapshot * p_data = GetDataCache(cur_code);
	if (p->field.damaged){
		p->field.Price = 0;
		p->field.Volume = 0;	
	}
	p_data->damaged = p->field.damaged;
    if (p->field.Direction == SHFE_FTDC_D_Buy){
        p_data->buy_price[p_data->buy_count] = p->field.Price;
        p_data->buy_volume[p_data->buy_count] = p->field.Volume;
        ++p_data->buy_count;
    }else{
        p_data->sell_price[p_data->sell_count] = p->field.Price;
        p_data->sell_volume[p_data->sell_count] = p->field.Volume;
        ++p_data->sell_count;
    }
    return p_data;
}

static void FillStatisticFields(MYShfeMarketData &des_data, SHFEMDQuoteSnapshot * const src_data)
{
    des_data.buy_total_volume = 0;
    des_data.buy_weighted_avg_price = 0;
    double sum_pv = 0;

	// wangying, repairer, total sell volume
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
	// wangying, repairer, total sell volume
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
    memcpy(my_data.InstrumentID, code.c_str(), code.size() + 1);
    my_data.data_flag = 5;
	DepthDataQueueOfCode::iterator it = data_depth_.find(code);
	if (it != data_depth_.end()){
		for (DepthDataQueue::const_iterator d_cit=it->second.begin(); d_cit!=it->second.end(); ++d_cit){
			memcpy(&my_data, &(*d_cit), sizeof(CDepthMarketDataField));

			if (d_cit + 1 != it->second.end()){
				// send immediately when receiving CDepthMarketDataField, so do NOT send data here again.
			}else{ // only combine the latest level one data with mbl data
				my_data.data_flag = 6;
			}
		}
		// clear all
		it->second.clear();
	}

	// new data, copy 30 elements at the end on 2017-06-25
	int buy_el_cpy_cnt = std::min(MY_SHFE_QUOTE_PRICE_POS_COUNT, p_data->buy_count);
	if (buy_el_cpy_cnt==MY_SHFE_QUOTE_PRICE_POS_COUNT){
		memcpy(my_data.buy_price,p_data->buy_price+(p_data->buy_count-buy_el_cpy_cnt),
			buy_el_cpy_cnt*sizeof(double));
		memcpy(my_data.buy_volume, p_data->buy_volume+(p_data->buy_count-buy_el_cpy_cnt),
			buy_el_cpy_cnt*sizeof(int));
	}else{
		memcpy(my_data.buy_price+(MY_SHFE_QUOTE_PRICE_POS_COUNT-buy_el_cpy_cnt),
			p_data->buy_price,buy_el_cpy_cnt * sizeof(double));
		memcpy(my_data.buy_volume+(MY_SHFE_QUOTE_PRICE_POS_COUNT-buy_el_cpy_cnt), 
			p_data->buy_volume, buy_el_cpy_cnt*sizeof(int));
	}

	int sell_el_cpy_cnt = std::min(MY_SHFE_QUOTE_PRICE_POS_COUNT,p_data->sell_count);
	if (sell_el_cpy_cnt==MY_SHFE_QUOTE_PRICE_POS_COUNT){
		memcpy(my_data.sell_price,p_data->sell_price+(p_data->sell_count-sell_el_cpy_cnt),
			sell_el_cpy_cnt*sizeof(double));
		memcpy(my_data.sell_volume,p_data->sell_volume+(p_data->sell_count-sell_el_cpy_cnt),
			sell_el_cpy_cnt*sizeof(int));
	}else{
		memcpy(my_data.sell_price+(MY_SHFE_QUOTE_PRICE_POS_COUNT-sell_el_cpy_cnt), 
			p_data->sell_price,sell_el_cpy_cnt*sizeof(double));
		memcpy(my_data.sell_volume+(MY_SHFE_QUOTE_PRICE_POS_COUNT-sell_el_cpy_cnt), 
			p_data->sell_volume,sell_el_cpy_cnt*sizeof(int));
	}

    FillStatisticFields(my_data, p_data);
    // 发给数据客户
    if (data_handler_){ data_handler_->OnMYShfeMDData(&my_data); }
}

void MYShfeMDManager::OnDepthMarketData(const CDepthMarketDataField * const pdata)
{
	MYShfeMarketData my_data;
	memcpy(my_data.InstrumentID, pdata->InstrumentID, sizeof(my_data.InstrumentID));
	memcpy(&my_data, pdata, sizeof(CDepthMarketDataField));
	my_data.data_flag = 1;
	// 发给数据客户
	if (data_handler_) { data_handler_->OnMYShfeMDData(&my_data); }

	DepthDataQueueOfCode::iterator it = data_depth_.find(pdata->InstrumentID);
	if (it == data_depth_.end()){
		it = data_depth_.insert(std::make_pair(pdata->InstrumentID, DepthDataQueue())).first;
	}
	it->second.push_back(*pdata);
}
