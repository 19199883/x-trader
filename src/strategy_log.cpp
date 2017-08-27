#include "strategy_log.h"

int StrategyLog::log1_cnt_ = 0;
int StrategyLog::log2_cnt_ = 0;
struct Log1Ex StrategyLog::log1_[8388608];
struct Log2Ex StrategyLog::log2_[8388608];

void StrategyLog::Log1(int strategy_id, struct Log1 &content)
{
	log1_[log1_cnt_].strategy_id = strategy_id;
	log1_[log1_cnt_].content =content;
	log1_cnt_++;
}

void StrategyLog::Log2(int strategy_id, struct Log2 &content)
{
	log2_[log2_cnt_].strategy_id = strategy_id;
	log2_[log2_cnt_].content =content;
	log2_cnt_++;
}

void StrategyLog::Save()
{
	// TODO:
}
