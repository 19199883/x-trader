#ifndef SHFE_MY_DATA_MANAGER_H_
#define SHFE_MY_DATA_MANAGER_H_

#include <map>
#include <vector>
#include <pthread.h>
#include "quote_cmn_config.h"
#include "quote_datatype_shfe_my.h"
#include "quote_datatype_shfe_deep.h"
#include "quote_datatype_level1.h"
#include "quote_cmn_utility.h"

class MYShfeMDHandler
{
public:
	virtual void OnMYShfeMDData(MYShfeMarketData *data) = 0;
};

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

	DepthDataQueueOfCode data_depth_;
    volatile bool running_flag_;
    static void * ProcessThread(MYShfeMDManager *);
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
};

bool MYShfeMDManager::GetLeftCode(std::string &left_code)
{
    bool ret = false;
    if (buy_first_idx_ < buy_codes_.size()){
        ret =true;
        left_code = buy_codes_[buy_first_idx_];
    }

    return ret;
}
bool MYShfeMDManager::GetPrevCode( const std::string &cur_code, std::string &prev_code )
{
	bool ret = false;
	if (buy_first_idx_ < buy_codes_.size() && cur_code > buy_codes_[buy_first_idx_]){
		ret =true;
		prev_code = buy_codes_[buy_first_idx_];
	}

	return ret;
}
void MYShfeMDManager::PushNewBuyDirCode( const std::string &cur_code)
{
	if (buy_codes_.empty()){
		buy_codes_.push_back(cur_code);
		return;
	}

	if (cur_code != buy_codes_.back()){
		buy_codes_.push_back(cur_code);
		return;
	}
}

void MYShfeMDManager::PopFirstCode( const std::string &cur_code)
{
	if (buy_first_idx_<buy_codes_.size() && cur_code==buy_codes_[buy_first_idx_]){
		++buy_first_idx_;
	}
}
	
#endif // SHFE_MY_DATA_MANAGER_H_
