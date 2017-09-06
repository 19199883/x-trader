#include "cancel_times_check.h"
#include <string>
#include <map>
#include <mutex>
#include <pthread.h>
#include <unordered_map>
#include "const_define.h"
#include "my_cmn_log.h"
#include "my_cmn_util_funcs.h"

using namespace std;

//typedef pthread_spinlock_t MyMutex;
//typedef _TTSpinLockGuard MyLock;
typedef std::mutex MyMutex;
typedef std::lock_guard<std::mutex> MyLock;

typedef pair<string, string> AccCode;
typedef map<AccCode, int> AccCodeCancelTimes;
typedef AccCodeCancelTimes::iterator AccCodeCancelTimesIt;
typedef AccCodeCancelTimes::const_iterator AccCodeCancelTimesConstIt;

typedef unordered_map<string, int> AccountCancelTimesSet;
static AccountCancelTimesSet s_warn_threshold;     // 480
static AccountCancelTimesSet s_max_cancel_times;   // 490

static AccCodeCancelTimes s_secutity_cancel_times;

#define CS_INIT 0
#define CS_FAILED 1
#define CS_SUCCESS 2
typedef pair<string, OrderRefDataType> AccountOrderType;
typedef map<AccountOrderType, int> OrderCancelState;
typedef OrderCancelState::iterator OrderCancelStateIt;
static OrderCancelState s_order_cancel_state;

static MyMutex s_cancel_times_mutex;

void CancelTimesCheck::Init(
    const char * account,       // account
    int warn_threthold,			// 撤单警告阈值
    int max_cancel_times		// 最大撤单次数
    )
{
    //pthread_spin_init(&s_cancel_times_mutex, 0);

    s_warn_threshold[string(account)] = warn_threthold;
    s_max_cancel_times[string(account)] = max_cancel_times;

    TNL_LOG_WARN("s_warn_threshold: %d; s_max_cancel_times: %d",
        s_warn_threshold[string(account)],
        s_max_cancel_times[string(account)]);
}

// 撤单次数（合约当前的撤单次数）
void ComplianceCheck_SetCancelTimes(
    const char * account,       // account
    const char * code,          // 证券代码
    int cancel_times            // 撤单次数
    )
{
    if (!code || !account)
    {
        return;
    }

    try
    {
        MyLock lock(s_cancel_times_mutex);

        AccCodeCancelTimesIt it = s_secutity_cancel_times.find(make_pair(string(account), string(code)));
        if (it == s_secutity_cancel_times.end())
        {
            it = s_secutity_cancel_times.insert(std::make_pair(make_pair(string(account), string(code)), cancel_times)).first;
        }
        else
        {
            it->second = cancel_times;
        }
        TNL_LOG_DEBUG("[InitCancelTimes] %s-%s:%d", account, code, it->second);
    }
    catch (std::exception &ex)
    {
        TNL_LOG_ERROR("exception occured, desc: %s", ex.what());
    }
    catch (...)
    {
        TNL_LOG_ERROR("unknown exception occured!");
    }
}

RESULT_TYPE CancelTimesCheck::TryReqOrderAction(
    const char * account,       // account
    const char * code, 			// 证券代码
    OrderRefDataType order_ref  // 报单引用
    )
{
    RESULT_TYPE ret = COMPLIANCE_CHECK_RESULT_SUCCESS;
    if (!code || !account)
    {
        return ret;
    }

    try
    {
        MyLock lock(s_cancel_times_mutex);

        AccCodeCancelTimesIt it = s_secutity_cancel_times.find(make_pair(string(account), string(code)));
        if (it == s_secutity_cancel_times.end())
        {
            it = s_secutity_cancel_times.insert(std::make_pair(make_pair(string(account), string(code)), 0)).first;
        }

        if (it->second >= s_max_cancel_times[string(account)])
        {
            ret = COMPLIANCE_CHECK_RESULT_CANCEL_EXCEED_LIMIT;
        }
        else
        {
            if (it->second >= s_warn_threshold[string(account)])
            {
                ret = COMPLIANCE_CHECK_RESULT_CANCEL_TIMES_REACH_WARN_THRETHOLD;
            }
            else if (it->second == s_warn_threshold[string(account)] - 1)
            {
                ret = COMPLIANCE_CHECK_RESULT_CANCEL_TIMES_EQUAL_WARN_THRETHOLD;
            }

            OrderCancelStateIt it_os = s_order_cancel_state.find(make_pair(string(account), order_ref));
            if (it_os == s_order_cancel_state.end())
            {
                ++it->second;
                s_order_cancel_state.insert(std::make_pair(make_pair(string(account), order_ref), CS_INIT));
            }

            TNL_LOG_DEBUG("[TryReqOrderAction] %s-%s cancel times:%d, order_ref:%ld", account, code, it->second, order_ref);
        }
    }
    catch (std::exception &ex)
    {
        TNL_LOG_ERROR("exception occured, desc: %s", ex.what());
    }
    catch (...)
    {
        TNL_LOG_ERROR("unknown exception occured!");
    }

    return ret;
}

void CancelTimesCheck::OnOrderCancelFailed(
    const char * account,       // account
    const char * code, 			// 证券代码
    OrderRefDataType order_ref  // 报单引用
    )
{
    if (!code || !account)
    {
        return;
    }

    MyLock lock(s_cancel_times_mutex);

    AccCodeCancelTimesIt it = s_secutity_cancel_times.find(make_pair(string(account), string(code)));
    if (it != s_secutity_cancel_times.end())
    {
        OrderCancelStateIt it_os = s_order_cancel_state.find(make_pair(string(account), order_ref));
        if (it_os != s_order_cancel_state.end() && it_os->second == CS_INIT)
        {
            --it->second;
            it_os->second = CS_FAILED;
        }
        TNL_LOG_DEBUG("[OnOrderCancelFailed] %s-%s cancel times:%d, order_ref:%ld", account, code, it->second, order_ref);
    }
}

void CancelTimesCheck::OnOrderCanceled(
    const char * account,       // account
    const char * code,          // 证券代码
    OrderRefDataType order_ref  // 报单引用
    )
{
    if (!code || !account)
    {
        return;
    }

    MyLock lock(s_cancel_times_mutex);

    AccCodeCancelTimesIt it = s_secutity_cancel_times.find(make_pair(string(account), string(code)));
    if (it != s_secutity_cancel_times.end())
    {
        OrderCancelStateIt it_os = s_order_cancel_state.find(make_pair(string(account), order_ref));
        if (it_os != s_order_cancel_state.end())
        {
            if (it_os->second == CS_INIT)
            {
                it_os->second = CS_SUCCESS;
            }
            else if (it_os->second == CS_FAILED)
            {
                ++it->second;
                it_os->second = CS_SUCCESS;
            }
        }
        TNL_LOG_DEBUG("[OnOrderCanceled] %s-%s cancel times:%d, order_ref:%ld", account, code, it->second, order_ref);
    }
}

// 达到告警阈值，禁止开仓
bool CancelTimesCheck::ReachWarnThreshold(
    const char * account,       // account
    const char * code 			// 证券代码
    )
{
    if (!code || !account)
    {
        return false;
    }

    MyLock lock(s_cancel_times_mutex);

    AccCodeCancelTimesConstIt it = s_secutity_cancel_times.find(make_pair(string(account), string(code)));
    if (it != s_secutity_cancel_times.end())
    {
        return it->second >= s_warn_threshold[string(account)];
    }

    return false;
}

