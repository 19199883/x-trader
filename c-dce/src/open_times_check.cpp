#include "open_times_check.h"
#include <string>
#include <unordered_map>
#include <mutex>
#include <pthread.h>

#include "const_define.h"
#include "my_cmn_log.h"
#include "my_cmn_util_funcs.h"

using namespace std;
using namespace OpenTimesCheck;

enum
{
    IF_IDX = 0,
    IH_IDX = 1,
    IC_IDX = 2,
    IDX_SIZE,
};
struct OpenVolumes
{
    int vol[IDX_SIZE];

    OpenVolumes()
    {
        vol[IF_IDX] = vol[IH_IDX] = vol[IC_IDX] = 0;
    }

    OpenVolumes &operator =(int v)
    {
        vol[IF_IDX] = vol[IH_IDX] = vol[IC_IDX] = v;
        return *this;
    }
    OpenVolumes &operator +=(int v)
    {
        vol[IF_IDX] += v;
        vol[IH_IDX] += v;
        vol[IC_IDX] += v;
        return *this;
    }
};

//typedef pthread_spinlock_t MyMutex;
//typedef _TTSpinLockGuard MyLock;
typedef std::mutex MyMutex;
typedef std::lock_guard<std::mutex> MyLock;

typedef unordered_map<string, int> AccountOpenThresholdSet;
static AccountOpenThresholdSet s_max_open_speculate; // 1199;
static AccountOpenThresholdSet s_max_open_arbitrage; // 9999;
static AccountOpenThresholdSet s_max_open_total; // 9999;

typedef unordered_map<string, OpenVolumes> AccountOpenVolSet;
static AccountOpenVolSet cur_open_speculate;
static AccountOpenVolSet cur_open_arbitrage;
static AccountOpenVolSet cur_open_total;

static MyMutex s_open_times_mutex;

inline void IncOpenTimes(const std::string &account, int volumn, char hedge_flag, int idx)
{
    cur_open_total[account].vol[idx] += volumn;
    if (hedge_flag == CONST_SHFLAG_TOU)
    {
        cur_open_speculate[account].vol[idx] += volumn;
    }
    if (hedge_flag == CONST_SHFLAG_TAO)
    {
        cur_open_arbitrage[account].vol[idx] += volumn;
    }
}

inline void DecOpenTimes(const std::string &account, int volumn, char hedge_flag, int idx)
{
    cur_open_total[account].vol[idx] -= volumn;
    if (hedge_flag == CONST_SHFLAG_TOU)
    {
        cur_open_speculate[account].vol[idx] -= volumn;
    }
    if (hedge_flag == CONST_SHFLAG_TAO)
    {
        cur_open_arbitrage[account].vol[idx] -= volumn;
    }
}

inline bool NeedCheck(
    char exchange_code,         // 交易所编码
    char open_close_flag        // 开平方向
    )
{
    // 仅中金所有开仓限制 20130806
    return exchange_code == CONST_EXCHCODE_CFFEX
        && open_close_flag == CONST_ENTRUST_OPEN;
}

// 初始化
void OpenTimesCheck::Init(
    const char * account,       // account
    int max_open_speculate,     // 最大投机手数
    int max_open_arbitrage,     // 最大套利手数
    int max_open_total          // 最大总手数
    )
{
    //pthread_spin_init(&s_open_times_mutex, 0);

    s_max_open_speculate[string(account)] = max_open_speculate;
    s_max_open_arbitrage[string(account)] = max_open_arbitrage;
    s_max_open_total[string(account)] = max_open_total;

    cur_open_speculate[string(account)] = 0;
    cur_open_arbitrage[string(account)] = 0;
    cur_open_total[string(account)] = 0;

    TNL_LOG_WARN("s_max_open_speculate: %d; s_max_open_arbitrage: %d: s_max_open_total: %d",
        s_max_open_speculate[string(account)],
        s_max_open_arbitrage[string(account)],
        s_max_open_total[string(account)]);
}

// 开仓量初始化（品种当前的开仓量）
void ComplianceCheck_SetOpenVolume(
    const char * account,       // account
    char exchange_code,         // 交易所编码
    const char * product,       // 品种
    char hedge_flag,            // 投机套保类型
    int open_volume             // 开仓数量
    )
{
    if (exchange_code == CONST_EXCHCODE_CFFEX)
    {
        MyLock lock(s_open_times_mutex);

        int idx = IF_IDX;
        if (product[1] == 'H') idx = IH_IDX;
        if (product[1] == 'C') idx = IC_IDX;

        cur_open_total[account].vol[idx] = open_volume;
        if (hedge_flag == CONST_SHFLAG_TOU)
        {
            cur_open_speculate[account].vol[idx] = open_volume;
        }
        if (hedge_flag == CONST_SHFLAG_TAO)
        {
            cur_open_arbitrage[account].vol[idx] = open_volume;
        }

        TNL_LOG_DEBUG("[SetOpenVolume] %s-%s:%d", account, product, cur_open_speculate[account].vol[idx]);
    }
}

// 下单尝试，增加 开仓计数
RESULT_TYPE OpenTimesCheck::TryReqOrderInsert(
    const char * account,       // account
    char exchange_code,         // 交易所编码
    const char * code,          // 证券代码
    int volumn,                 // 数量
    char hedge_flag,            // 投机套保类型
    char open_close_flag,       // 开平方向
    OrderRefDataType * opened_count         // 【输出】导致自成交对手单的报单引用/当前开仓数
    )
{
    if (NeedCheck(exchange_code, open_close_flag))
    {
        int idx = IF_IDX;
        if (code[1] == 'H') idx = IH_IDX;
        if (code[1] == 'C') idx = IC_IDX;

        MyLock lock(s_open_times_mutex);
        TNL_LOG_DEBUG("[TryReqOrderInsert] %s-%s:%d", account, code, cur_open_speculate[account].vol[idx]);

        //检查投机开仓限制
        if (hedge_flag == CONST_SHFLAG_TOU
            && s_max_open_speculate[string(account)] > 0
            && (cur_open_speculate[string(account)].vol[idx] + volumn) > s_max_open_speculate[string(account)])
        {
            *opened_count = cur_open_speculate[string(account)].vol[idx];
            return COMPLIANCE_CHECK_RESULT_OPEN_REACH_EXCEED_LIMIT;
        }
        //检查套利开仓限制
        if (hedge_flag == CONST_SHFLAG_TAO
            && s_max_open_arbitrage[string(account)] > 0
            && (cur_open_arbitrage[string(account)].vol[idx] + volumn) > s_max_open_arbitrage[string(account)])
        {
            *opened_count = cur_open_arbitrage[string(account)].vol[idx];
            return COMPLIANCE_CHECK_RESULT_OPEN_REACH_EXCEED_LIMIT;
        }
        //检查总共开仓限制
        if (s_max_open_total[string(account)] > 0
            && (cur_open_total[string(account)].vol[idx] + volumn) > s_max_open_total[string(account)])
        {
            *opened_count = cur_open_total[string(account)].vol[idx];
            return COMPLIANCE_CHECK_RESULT_OPEN_REACH_EXCEED_LIMIT;
        }

        int ret = COMPLIANCE_CHECK_RESULT_SUCCESS;

        //判断是否刚好到达开仓限制
        if (hedge_flag == CONST_SHFLAG_TOU
            && s_max_open_speculate[string(account)] > 0
            && (cur_open_speculate[string(account)].vol[idx] + volumn) == s_max_open_speculate[string(account)])
        {
            *opened_count = cur_open_speculate[string(account)].vol[idx];
            ret = COMPLIANCE_CHECK_RESULT_OPEN_EQUAL_LIMIT;
        }
        //检查套利开仓限制
        else if (hedge_flag == CONST_SHFLAG_TAO
            && s_max_open_arbitrage[string(account)] > 0
            && (cur_open_arbitrage[string(account)].vol[idx] + volumn) == s_max_open_arbitrage[string(account)])
        {
            *opened_count = cur_open_arbitrage[string(account)].vol[idx];
            ret = COMPLIANCE_CHECK_RESULT_OPEN_EQUAL_LIMIT;
        }
        //检查总共开仓限制
        else if (s_max_open_total[string(account)] > 0
            && (cur_open_total[string(account)].vol[idx] + volumn) == s_max_open_total[string(account)])
        {
            *opened_count = cur_open_total[string(account)].vol[idx];
            ret = COMPLIANCE_CHECK_RESULT_OPEN_EQUAL_LIMIT;
        }

        // 增加开仓计数
        IncOpenTimes(account, volumn, hedge_flag, idx);

        return ret;
    }
    else
    {
        return COMPLIANCE_CHECK_RESULT_SUCCESS;
    }
}

// 下单失败回报，删除该报单/回滚开仓计数
void OpenTimesCheck::OnOrderInsertFailed(
    const char * account,       // account
    char exchange_code,         // 交易所编码
    const char * code,          // 证券代码
    int volumn,                 // 数量
    char hedge_flag,            // 投机套保类型
    char open_close_flag        // 开平方向
    )
{
    if (NeedCheck(exchange_code, open_close_flag))
    {
        int idx = IF_IDX;
        if (code[1] == 'H') idx = IH_IDX;
        if (code[1] == 'C') idx = IC_IDX;

        MyLock lock(s_open_times_mutex);
        DecOpenTimes(account, volumn, hedge_flag, idx);
    }
}

// 报单撤单时，删除该报单/更新开仓计数
void OpenTimesCheck::OnOrderCanceled(
    const char * account,       // account
    char exchange_code,         // 交易所编码
    const char * code,          // 证券代码
    int canceled_volumn,        // 撤单数量
    char hedge_flag,            // 投机套保类型
    char open_close_flag        // 开平方向
    )
{
    if (NeedCheck(exchange_code, open_close_flag))
    {
        int idx = IF_IDX;
        if (code[1] == 'H') idx = IH_IDX;
        if (code[1] == 'C') idx = IC_IDX;

        MyLock lock(s_open_times_mutex);
        DecOpenTimes(account, canceled_volumn, hedge_flag, idx);
    }
}
