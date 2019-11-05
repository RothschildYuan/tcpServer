#ifndef CMUTEX_H
#define CMUTEX_H

#include <pthread.h>

namespace ACT {

#define MutexLock_t                     pthread_mutex_t
#define MutexCond_t                     pthread_cond_t
#define MutexLockAttr_t                 pthread_mutexattr_t

class CMutex
{
    friend class CCond;

    CMutex(const CMutex&);
    CMutex& operator=(const CMutex&);
public:
    CMutex(bool is_process = false);
    virtual ~CMutex();

    int lock();
    int unlock();
    int trylock();

private:

    MutexLock_t     m_lock;
    MutexLockAttr_t m_mutexattr;
};

// 为了兼容 stl 互斥锁 的写法
template<typename _T>
class unique_lock
{
public:
    unique_lock(){}
    unique_lock(_T& t) {
        m_lock = &t;
    }
    unique_lock(const unique_lock& t) {
        m_lock = &t;
    }
    unique_lock& operator=(const unique_lock& oth) {
        m_lock = oth.m_lock;
    }
    _T *getLock() const {
        return m_lock;
    }
private:
    _T* m_lock;
};

}

#endif // CMutex_H
