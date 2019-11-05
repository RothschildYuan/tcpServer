#include "CCond.h"

namespace ACT {

#define MutexLockAttrInit(a)            pthread_mutexattr_init(a);
#define MutexLockAttrDestroy(a)         pthread_mutexattr_destroy(a);
#define MutexLockAttrSetpshared(a, b)   pthread_mutexattr_setpshared(a, b)

#define MutexLock(a)                    pthread_mutex_lock(a)
#define MutexUnlock(a)                  pthread_mutex_unlock(a)
#define MutexTrylock(a)                 pthread_mutex_trylock(a)
#define MutexLockInit(a, b)             pthread_mutex_init(a, b)
#define MutexLockDestroy(a)             pthread_mutex_destroy(a)

#define MutexCondInit(a, b)             pthread_cond_init(a, b)
#define MutexCondDestroy(a)             pthread_cond_destroy(a)
#define MutexCondAttrInit(a)            pthread_condattr_init(a)
#define MutexCondAttrDestroy(a)         pthread_condattr_destroy(a)
#define MutexCondAttrSetpshared(a, b)   pthread_condattr_setpshared(a, b)

#define MutexCondWait(a, b)             pthread_cond_wait(a, b)
#define MutexCondSignal(a)              pthread_cond_signal(a)
#define MutexCondBroadcast(a)           pthread_cond_broadcast(a)

CCond::CCond(bool is_process)
{
    MutexCondAttrInit(&m_condattr);
    if(is_process == true) {
        MutexCondAttrSetpshared(&m_condattr, PTHREAD_PROCESS_SHARED);
    }
    MutexCondInit(&m_cond, &m_condattr);
}

CCond::~CCond() {
    MutexCondAttrDestroy(&m_condattr);
    MutexCondDestroy(&m_cond);
}


int CCond::wait(CMutex* pmutex) {
    if(pmutex == nullptr) {
        return -1;
    }
    return MutexCondWait(&m_cond, &pmutex->m_lock);
}

int CCond::wait(unique_lock<CMutex> &pmutex)
{
    return wait(pmutex.getLock());
}

int CCond::notify_one() {
    return MutexCondSignal(&m_cond);
}

int CCond::notify_all() {
    return MutexCondBroadcast(&m_cond);
}

}
