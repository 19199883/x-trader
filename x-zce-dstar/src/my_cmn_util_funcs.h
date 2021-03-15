#ifndef COMMONFUNCS_UTIL_FUNCS_H_
#define COMMONFUNCS_UTIL_FUNCS_H_

#include <cstddef>
#include <string>
#include <string.h>

namespace my_cmn
{
    // 20130115_123059  2013-01-15 12:30:59
    std::string GetCurrentDateTimeString();

    // 20130115_123059_500  2013-01-15 12:30:59.500
    std::string GetCurrentDateTimeWithMilisecString();

    // 20130115
    std::string GetCurrentDateString();

    // 12:30:59.500  12:30:59.500
    std::string GetCurrentTimeWithMilisecString();

    // 123059  12:30:59
    int GetCurrentTimeInt();

    // 123059500  12:30:59.500
    int GetCurrentTimeWithMilisecInt();

    // 20130115
    int GetCurrentDateInt();

    template<typename T>
    void MYStringSplit(const char *str, T &fields, char seperator)
    {
        std::size_t prev_pos = 0;
        std::size_t pos = 0;
        for (; pos < strlen(str); ++pos)
        {
            if (*(str + pos) == seperator)
            {
                if (pos > prev_pos) fields.push_back(std::string(str + prev_pos, pos - prev_pos));
                prev_pos = pos + 1;
            }
        }
        if (pos > prev_pos) fields.push_back(std::string(str + prev_pos, pos - prev_pos));
    }
}

namespace
{
// spinlock的RAII封装
class _TTSpinLockGuard
{
public:
    _TTSpinLockGuard(pthread_spinlock_t &lock)
        : lock_obj_(lock)
    {
        pthread_spin_lock(&lock_obj_);
    }
    ~_TTSpinLockGuard()
    {
        pthread_spin_unlock(&lock_obj_);
    }

private:
    _TTSpinLockGuard(const _TTSpinLockGuard&);
    _TTSpinLockGuard &operator=(const _TTSpinLockGuard&);

    // spinlock
    pthread_spinlock_t &lock_obj_;
};

// mutex的RAII封装
class _TTMutexGuard
{
public:
    _TTMutexGuard(pthread_mutex_t &sync_mutex) : sync_mutex_(sync_mutex)
    {
        pthread_mutex_lock(&sync_mutex_);
    }
    ~_TTMutexGuard()
    {
        pthread_mutex_unlock(&sync_mutex_);
    }

private:
    _TTMutexGuard(const _TTMutexGuard&);
    _TTMutexGuard &operator=(const _TTMutexGuard&);

    // 互斥量
    pthread_mutex_t &sync_mutex_;
};
}

#endif  //COMMONFUNCS_UTIL_FUNCS_H_
