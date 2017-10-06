#ifndef SHFE_MY_DATA_MANAGER_H_
#define SHFE_MY_DATA_MANAGER_H_

#include <map>
#include <vector>
#include <pthread.h>

#include "quote_cmn_config.h"

#include "quote_datatype_shfe_my.h"
#include "quote_datatype_shfe_deep.h"
#include "quote_datatype_level1.h"
//
#include "quote_cmn_utility.h"

class MYShfeMDHandler
{
public:
	virtual void OnMYShfeMDData(MYShfeMarketData *data) = 0;
	virtual ~MYShfeMDHandler(){}
};

struct SHFEMDQuoteSnapshot
{
	SHFEMDQuoteSnapshot()
	{
		this->damaged = false;
	}

	int		buy_count;
	int		sell_count;

	double *buy_price;
	int    *buy_volume;
	double *sell_price;
	int    *sell_volume;

	// wangying, repairer,total sell volume
	bool damaged;
};

typedef std::map<std::string, int> CodeIndex;
typedef std::vector<SHFEQuote> MBLDataCollection;
typedef std::vector<CDepthMarketDataField> DepthDataQueue;
typedef std::map<std::string, DepthDataQueue> DepthDataQueueOfCode;

class MYShfeMDManager
{
public:
    MYShfeMDManager(const ConfigData &cfg);
    ~MYShfeMDManager();
    void OnMBLData(const CShfeFtdcMBLMarketDataField * const pdata, bool last_flag);
	void OnDepthMarketData(const CDepthMarketDataField * const pdata);

	void SetDataHandler(MYShfeMDHandler * data_handler)
	{
		data_handler_ = data_handler;
	}

private:
    const ConfigData &cfg_;

    pthread_mutex_t mbl_mutex_;
    MBLDataCollection data_in_;
    MBLDataCollection data_handle_;
	pthread_mutex_t depth_mutex_;
	DepthDataQueueOfCode data_depth_;

    volatile bool running_flag_;
    pthread_attr_t data_process_thread_attr_;
    pthread_t data_process_thread_;
    static void * ProcessThread(MYShfeMDManager *);

    // data manager interface
    SHFEMDQuoteSnapshot **p_quote_buffer_;
    CodeIndex code_index_;
    int code_count_max;
    int last_code_index;
    int price_position_count_max;
    SHFEMDQuoteSnapshot *GetDataCache(const std::string &code);
	SHFEMDQuoteSnapshot *PushDataToBuffer(const std::string &cur_code, MBLDataCollection::iterator &p);

	void SendToClient(const std::string &code, SHFEMDQuoteSnapshot * const p_data);

	// TODO: new data
	std::string ToString(const MYShfeMarketData &d);

	MYShfeMDHandler *data_handler_;

	// for checking data finish status
	std::vector<std::string> buy_codes_;
	std::size_t buy_first_idx_;
	inline bool GetLeftCode(std::string &left_code);
	inline bool GetPrevCode(const std::string &cur_code, std::string &prev_code);
	inline void PushNewBuyDirCode(const std::string &cur_code);
	inline void PopFirstCode(const std::string &cur_code);
	inline void ResetBuyCode()
	{
		buy_first_idx_ = 0;
		buy_codes_.clear();
	}
	
	/*
	 * check whether previous frame ends.
	 * return true if previous frame ends; otherwise, false
	 */
	inline bool FrameCutShort(const std::string &cur_code);
};

bool MYShfeMDManager::GetLeftCode(std::string &left_code)
{
    bool ret = false;
    if (buy_first_idx_ < buy_codes_.size())
    {
        ret =true;
        left_code = buy_codes_[buy_first_idx_];
    }

    return ret;
}
bool MYShfeMDManager::GetPrevCode( const std::string &cur_code, std::string &prev_code )
{
	bool ret = false;
	if (buy_first_idx_ < buy_codes_.size() && cur_code > buy_codes_[buy_first_idx_])
	{
		ret =true;
		prev_code = buy_codes_[buy_first_idx_];
	}

	return ret;
}
void MYShfeMDManager::PushNewBuyDirCode( const std::string &cur_code)
{
	if (buy_codes_.empty())
	{
		buy_codes_.push_back(cur_code);
		return;
	}

	if (cur_code != buy_codes_.back())
	{
		buy_codes_.push_back(cur_code);
		return;
	}
}

void MYShfeMDManager::PopFirstCode( const std::string &cur_code)
{
	if (buy_first_idx_ < buy_codes_.size() && cur_code == buy_codes_[buy_first_idx_])
	{
		++buy_first_idx_;
	}
}
	
bool MYShfeMDManager::FrameCutShort(const std::string &cur_code)
{
	if (buy_codes_.empty()){
		return false;
	}
	else{
		if (cur_code < buy_codes_.back()){// frame cut short due to package loss
			return true;
		}
		else{ return false; }
	}
}

#endif // SHFE_MY_DATA_MANAGER_H_
