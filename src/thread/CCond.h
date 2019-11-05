#ifndef CCOND_H
#define CCOND_H

#include <CMutex.h>

namespace ACT {

#define MutexLock_t                     pthread_mutex_t
#define MutexLockAttr_t                 pthread_mutexattr_t

#define MutexCond_t                     pthread_cond_t
#define MutexCondAttr_t                 pthread_condattr_t

class CCond
{
    CCond(const CCond&);
    CCond& operator=(const CCond&);
public:
    /**
     * @brief CCond         条件变量
     * @param is_process    是否支持进程间条件变量
     */
    CCond(bool is_process = false);
    virtual ~CCond();

    /**
     * @brief wait          等待阻塞
     * @param pmutex        互斥锁
     * @return              0 成功，其他失败
     */
    int wait(CMutex* pmutex);

    /**
     * @brief wait          等待阻塞（为了兼容 stl 互斥锁 的写法）
     * @param pmutex        互斥锁
     * @return              0 成功，其他失败
     */
    int wait(unique_lock<CMutex>& pmutex);

    /**
     * @brief notify_one    仅激活一个线程
     * @return              0 成功，其他失败
     */
    int notify_one();

    /**
     * @brief notify_all    激活所有线程
     * @return              0 成功，其他失败
     */
    int notify_all();

private:

    MutexCond_t     m_cond;
    MutexCondAttr_t m_condattr;
};

}

#endif // CCond_H
