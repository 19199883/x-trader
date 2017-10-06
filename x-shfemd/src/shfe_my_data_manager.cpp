#include "shfe_my_data_manager.h"

#include <algorithm>
#include "quote_cmn_utility.h"

#define SHFE_MD_CODE_COUNT_INIT 200
#define SHFE_PRICE_COUNT_INIT   600

std::string MYShfeMDManager::ToString(const MYShfeMarketData &d) {
	MY_LOG_DEBUG("MYShfeMarketData: instrument:%s, data_flag:%d,buy_total_volume:%d; sell_total_volume:%d; buy_weighted_avg_price:%lf; sell_weighted_avg_price:%lf",
				d.InstrumentID, d.data_flag, d.buy_total_volume,d.sell_total_volume,d.buy_weighted_avg_price,d.sell_weighted_avg_price);

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

static SHFEMDQuoteSnapshot ** AllocateSnapshotBuffer(int code_count, int price_pos_count)
{
    SHFEMDQuoteSnapshot ** pp = new SHFEMDQuoteSnapshot *[code_count];
    for (int i = 0; i < code_count; ++i)
    {
        pp[i] = new SHFEMDQuoteSnapshot();
        pp[i]->buy_count = 0;
        pp[i]->buy_price = new double[price_pos_count];
        pp[i]->buy_volume = new int[price_pos_count];
        pp[i]->sell_count = 0;
        pp[i]->sell_price = new double[price_pos_count];
        pp[i]->sell_volume = new int[price_pos_count];
    }

    return pp;
}

static void CopySnapshotBuffer(SHFEMDQuoteSnapshot **enlarged_buffer, SHFEMDQuoteSnapshot ** p_old,
    int code_count_old, int price_pos_count_old)
{
    for (int i = 0; i < code_count_old; ++i)
    {
        enlarged_buffer[i]->buy_count = p_old[i]->buy_count;
        memcpy(enlarged_buffer[i]->buy_price, p_old[i]->buy_price, price_pos_count_old * sizeof(double));
        memcpy(enlarged_buffer[i]->buy_volume, p_old[i]->buy_volume, price_pos_count_old * sizeof(int));
        enlarged_buffer[i]->sell_count = p_old[i]->sell_count;
        memcpy(enlarged_buffer[i]->sell_price, p_old[i]->sell_price, price_pos_count_old * sizeof(double));
        memcpy(enlarged_buffer[i]->sell_volume, p_old[i]->sell_volume, price_pos_count_old * sizeof(int));
    }
}

MYShfeMDManager::MYShfeMDManager(const ConfigData &cfg)
    : cfg_(cfg)
{
    pthread_mutex_init(&mbl_mutex_, NULL);
    pthread_mutex_init(&depth_mutex_, NULL);
    data_process_thread_ = 0;
    running_flag_ = true;
    buy_first_idx_ = 0;
    data_handler_ = NULL;

    // allocate buffer
    code_count_max = SHFE_MD_CODE_COUNT_INIT;
    price_position_count_max = SHFE_PRICE_COUNT_INIT;
    p_quote_buffer_ = AllocateSnapshotBuffer(code_count_max, price_position_count_max);
    last_code_index = 0;

    data_in_.reserve(5000);
    data_handle_.reserve(5000);

    pthread_attr_init(&data_process_thread_attr_);
    pthread_attr_setdetachstate(&data_process_thread_attr_, PTHREAD_CREATE_JOINABLE);

    // start threads
    pthread_create(&data_process_thread_, &data_process_thread_attr_, (void * (*)(void *))ProcessThread, (void *)this);
}

MYShfeMDManager::~MYShfeMDManager()
{
    running_flag_ = false;

    if (data_process_thread_ != 0)
    {
        pthread_join(data_process_thread_, NULL);
    }

    pthread_mutex_destroy(&mbl_mutex_);
    pthread_mutex_destroy(&depth_mutex_);
}

void MYShfeMDManager::OnMBLData(const CShfeFtdcMBLMarketDataField* const pdata, bool last_flag)
{

    MYMutexGuard guard(mbl_mutex_);

    if (pdata)
    {
        data_in_.push_back(SHFEQuote(*pdata, last_flag));
    }
    else if (last_flag)
    {
        data_in_.push_back(SHFEQuote(true));
    }
}

inline void ResetSnapshot(SHFEMDQuoteSnapshot* pd)
{
    pd->buy_count = 0;
    pd->sell_count = 0;
}

void* MYShfeMDManager::ProcessThread(MYShfeMDManager* p_mngr)
{
    std::string prev_code;
    char prev_dir = SHFE_FTDC_D_Buy;
    char cur_dir = SHFE_FTDC_D_Buy;
    std::string cur_code;

    while (p_mngr->running_flag_)
    {
        {
            // 数据交换
            MYMutexGuard guard(p_mngr->mbl_mutex_);

            if (!p_mngr->data_in_.empty()) {
                p_mngr->data_in_.swap(p_mngr->data_handle_);
            }
        }

        if (p_mngr->data_handle_.empty()) {
            usleep(20);
            continue;
        }

        for (MBLDataCollection::iterator p = p_mngr->data_handle_.begin(); p != p_mngr->data_handle_.end(); ++p) {
			// wangying, repairer
            if (p->isLast) { // snapshot is completed, should send to client
                if (p->field.Volume > 0) { // valid data
                    cur_code = p->field.InstrumentID;
                    (void) p_mngr->PushDataToBuffer(cur_code, p);
                }

                // 帧结束了，将没有发送的合约数据全部发出
                bool send_cur_flag = false;
                while (p_mngr->GetLeftCode(prev_code)) {
                    SHFEMDQuoteSnapshot * p_snapshot_prev = p_mngr->GetDataCache(prev_code);
                    p_mngr->SendToClient(prev_code, p_snapshot_prev);
                    if (!send_cur_flag && cur_code == prev_code) {// there is only sell dir data for the same instruemnt
                        send_cur_flag = true;
                    }
                }
                if (!send_cur_flag) {
                    SHFEMDQuoteSnapshot * p_snapshot = p_mngr->GetDataCache(cur_code);
                    p_mngr->SendToClient(cur_code, p_snapshot);
                }

                // 重置数据
                p_mngr->ResetBuyCode();

                continue;
            }

            if (p->field.Volume > 0) {
                // 有效数据
                cur_code = p->field.InstrumentID;
                cur_dir = p->field.Direction;

                // 数据记录
                (void) p_mngr->PushDataToBuffer(cur_code, p);

                // 买方向合约记录
                if (cur_dir == SHFE_FTDC_D_Buy) { 
					// repairer 
					if (p_mngr->FrameCutShort(cur_code)) { 
						while (p_mngr->GetLeftCode(prev_code)) {
							// debug
							//MY_LOG_WARN("error:cur_dir == SHFE_FTDC_D_Buy");

							SHFEMDQuoteSnapshot * p_snapshot_prev = p_mngr->GetDataCache(prev_code);
							p_mngr->SendToClient(prev_code, p_snapshot_prev);
						}
					}

					p_mngr->PushNewBuyDirCode(cur_code); 
				}

                else if (prev_dir == SHFE_FTDC_D_Buy && cur_dir == SHFE_FTDC_D_Sell) { // 第一个卖方向数据，核对前方是否有涨停合约，该类合约无卖方向数据，应该发出
                    while (p_mngr->GetPrevCode(cur_code, prev_code)) {
                        SHFEMDQuoteSnapshot * p_snapshot_prev = p_mngr->GetDataCache(prev_code);
                        p_mngr->SendToClient(prev_code, p_snapshot_prev);
                    }
                }
                else if (prev_dir == SHFE_FTDC_D_Sell && p->field.Direction == SHFE_FTDC_D_Sell && prev_code != cur_code) { // 卖方向中间的合约切换，将之前的合约数据发出
                    SHFEMDQuoteSnapshot * p_snapshot_prev = p_mngr->GetDataCache(prev_code);
                    p_mngr->SendToClient(prev_code, p_snapshot_prev);

                    while (p_mngr->GetPrevCode(cur_code, prev_code)) {
						// debug
						//MY_LOG_WARN("error:prev_dir == SHFE_FTDC_D_Sell && p->field.Direction == SHFE_FTDC_D_Sell && prev_code != cur_code");

                        SHFEMDQuoteSnapshot * p_snapshot_prev = p_mngr->GetDataCache(prev_code);
                        p_mngr->SendToClient(prev_code, p_snapshot_prev);
                    }
                }

                prev_code = cur_code;
                prev_dir = cur_dir;
                continue;
            }
            else { // invalid data, no need to handle, shouldn't reach here
                MY_LOG_WARN("invalid data without last flag.");
            }
        }
        p_mngr->data_handle_.clear();
    }

    return NULL;
}

SHFEMDQuoteSnapshot* MYShfeMDManager::GetDataCache(const std::string& code)
{
    CodeIndex::iterator it = code_index_.find(code);
    if (it == code_index_.end())
    {
        // assign new index
        int new_index = last_code_index++;

        // enlarge code count
        if (new_index >= code_count_max)
        {
            MY_LOG_WARN("code count beyond: %d", code_count_max);

            SHFEMDQuoteSnapshot ** p_old = p_quote_buffer_;
            int code_count_old = code_count_max;

            // allocate new space
            code_count_max *= 2;
            p_quote_buffer_ = AllocateSnapshotBuffer(code_count_max, price_position_count_max);

            // copy old data
            CopySnapshotBuffer(p_quote_buffer_, p_old, code_count_old, price_position_count_max);

            // clear old data
            for (int i = 0; i < code_count_old; ++i)
            {
                delete[] p_old[i]->buy_price;
                delete[] p_old[i]->buy_volume;
                delete[] p_old[i]->sell_price;
                delete[] p_old[i]->sell_volume;
            }
            delete[] p_old;
        }

        it = code_index_.insert(std::make_pair(code, new_index)).first;
    }

    if (p_quote_buffer_[it->second]->buy_count >= price_position_count_max
        || p_quote_buffer_[it->second]->sell_count >= price_position_count_max)
    {
        MY_LOG_WARN("price position count beyond: %d", price_position_count_max);

        // enlarge price position size
        SHFEMDQuoteSnapshot ** p_old = p_quote_buffer_;
        int price_pos_count_old = price_position_count_max;

        // allocate new space
        price_position_count_max *= 2;
        p_quote_buffer_ = AllocateSnapshotBuffer(code_count_max, price_position_count_max);

        // copy old data
        CopySnapshotBuffer(p_quote_buffer_, p_old, code_count_max, price_pos_count_old);

        // clear old data
        for (int i = 0; i < code_count_max; ++i)
        {
            delete[] p_old[i]->buy_price;
            delete[] p_old[i]->buy_volume;
            delete[] p_old[i]->sell_price;
            delete[] p_old[i]->sell_volume;
        }
        delete[] p_old;
    }

    return p_quote_buffer_[it->second];
}

SHFEMDQuoteSnapshot *MYShfeMDManager::PushDataToBuffer(const std::string &cur_code, MBLDataCollection::iterator &p)
{
    SHFEMDQuoteSnapshot * p_data = GetDataCache(cur_code);
    if (!p_data)
    {
        return p_data;
    }

	// wangying, repairer, total sell volume, bug dound on 2017-05-11
	// new data on 2017-0625
	// TODO: new data, debug, print
	//MY_LOG_DEBUG("damaged: %d; instrument:%s", p->field.damaged, p->field.InstrumentID);
	if (p->field.damaged){
		p->field.Price = 0;
		p->field.Volume = 0;	
	}

	p_data->damaged = p->field.damaged;

    if (p->field.Direction == SHFE_FTDC_D_Buy)
    {
        p_data->buy_price[p_data->buy_count] = p->field.Price;
        p_data->buy_volume[p_data->buy_count] = p->field.Volume;
        ++p_data->buy_count;
    }
    else
    {
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
		for (int i = 0; i < src_data->buy_count; ++i)
		{
			des_data.buy_total_volume += src_data->buy_volume[i];
			sum_pv += (src_data->buy_volume[i] * src_data->buy_price[i]);
		}
	}
	else{
		des_data.buy_total_volume = 0;
	}
    if (des_data.buy_total_volume > 0)
    {
        des_data.buy_weighted_avg_price = sum_pv / des_data.buy_total_volume;
    }

    des_data.sell_total_volume = 0;
    des_data.sell_weighted_avg_price = 0;
    sum_pv = 0;
	// wangying, repairer, total sell volume
	if (!src_data->damaged){
		for (int i = 0; i < src_data->sell_count; ++i)
		{
			des_data.sell_total_volume += src_data->sell_volume[i];
			sum_pv += (src_data->sell_volume[i] * src_data->sell_price[i]);
		}
	}
	else{
		des_data.sell_total_volume = 0;
	}
    if (des_data.sell_total_volume > 0)
    {
        des_data.sell_weighted_avg_price = sum_pv / des_data.sell_total_volume;
    }
}

void MYShfeMDManager::SendToClient(const std::string &code, SHFEMDQuoteSnapshot * const p_data)
{
    // prepare my data
    MYShfeMarketData my_data;

    memcpy(my_data.InstrumentID, code.c_str(), code.size() + 1);
    //my_data.data_flag = 2;
    my_data.data_flag = 5;
    {
        MYMutexGuard guard(depth_mutex_);
        DepthDataQueueOfCode::iterator it = data_depth_.find(code);
        if (it != data_depth_.end())
        {
            for (DepthDataQueue::const_iterator d_cit = it->second.begin(); d_cit != it->second.end(); ++d_cit)
            {
                memcpy(&my_data, &(*d_cit), sizeof(CDepthMarketDataField));

                if (d_cit + 1 != it->second.end())
                {
					// send immediately when receiving CDepthMarketDataField, so do NOT send data here again.
                }
                else
                {
                    // only combine the latest level one data with mbl data
                    //my_data.data_flag = 3;
                    my_data.data_flag = 6;
                }
            }
            // clear all
            it->second.clear();
        }
    }

	// new data, copy 30 elements at the end on 2017-06-25
	int buy_el_cpy_cnt = std::min(MY_SHFE_QUOTE_PRICE_POS_COUNT, p_data->buy_count);
	if (buy_el_cpy_cnt==MY_SHFE_QUOTE_PRICE_POS_COUNT){
		memcpy(my_data.buy_price,  p_data->buy_price  + (p_data->buy_count - buy_el_cpy_cnt), buy_el_cpy_cnt * sizeof(double));
		memcpy(my_data.buy_volume, p_data->buy_volume + (p_data->buy_count - buy_el_cpy_cnt), buy_el_cpy_cnt * sizeof(int));
	}
	else{
		memcpy(my_data.buy_price  + (MY_SHFE_QUOTE_PRICE_POS_COUNT - buy_el_cpy_cnt), p_data->buy_price , buy_el_cpy_cnt * sizeof(double));
		memcpy(my_data.buy_volume + (MY_SHFE_QUOTE_PRICE_POS_COUNT - buy_el_cpy_cnt), p_data->buy_volume, buy_el_cpy_cnt * sizeof(int));
	}

	int sell_el_cpy_cnt = std::min(MY_SHFE_QUOTE_PRICE_POS_COUNT, p_data->sell_count);
	if (sell_el_cpy_cnt==MY_SHFE_QUOTE_PRICE_POS_COUNT){
		memcpy(my_data.sell_price,  p_data->sell_price  + (p_data->sell_count - sell_el_cpy_cnt), sell_el_cpy_cnt * sizeof(double));
		memcpy(my_data.sell_volume, p_data->sell_volume + (p_data->sell_count - sell_el_cpy_cnt), sell_el_cpy_cnt * sizeof(int));
	}
	else{
		memcpy(my_data.sell_price  + (MY_SHFE_QUOTE_PRICE_POS_COUNT - sell_el_cpy_cnt), p_data->sell_price, sell_el_cpy_cnt * sizeof(double));
		memcpy(my_data.sell_volume + (MY_SHFE_QUOTE_PRICE_POS_COUNT - sell_el_cpy_cnt), p_data->sell_volume, sell_el_cpy_cnt * sizeof(int));
	}

    FillStatisticFields(my_data, p_data);

	// TODO: new data, debug, print
	//ToString(my_data);

    // 发给数据客户
    if (data_handler_)
    {
        data_handler_->OnMYShfeMDData(&my_data);
    }

    //MY_LOG_DEBUG("snapshot, code: %s, buy_count: %d, sell_count: %d", code.c_str(), p_data->buy_count, p_data->sell_count);

    // reset
    ResetSnapshot(p_data);
    PopFirstCode(code);
}

void MYShfeMDManager::OnDepthMarketData(const CDepthMarketDataField * const pdata)
{
    MYMutexGuard guard(depth_mutex_);
    if (pdata)
    {
		MYShfeMarketData my_data;
		// already check that the lengths of Instruments of two class(CDepthMarketDataField and MYShfeMarketData) are equal, it is 31.
		memcpy(my_data.InstrumentID, pdata->InstrumentID, sizeof(my_data.InstrumentID));
		memcpy(&my_data, pdata, sizeof(CDepthMarketDataField));
		my_data.data_flag = 1;
		// 发给数据客户
		if (data_handler_) { 
			data_handler_->OnMYShfeMDData(&my_data); 
		}

        DepthDataQueueOfCode::iterator it = data_depth_.find(pdata->InstrumentID);
        if (it == data_depth_.end())
        {
            it = data_depth_.insert(std::make_pair(pdata->InstrumentID, DepthDataQueue())).first;
        }

        it->second.push_back(*pdata);
    }
}
