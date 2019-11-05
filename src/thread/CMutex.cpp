#include "CMutex.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

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

#define MutexCondWait(a, b)             pthread_cond_wait(a, b)
#define MutexCondSignal(a)              pthread_cond_signal(a)
#define MutexCondBroadcast(a)           pthread_cond_broadcast(a)

CMutex::CMutex(bool is_process)
{
    MutexLockAttrInit(&m_mutexattr);
    if(is_process == true) {
        MutexLockAttrSetpshared(&m_mutexattr, PTHREAD_PROCESS_SHARED);
    }
    MutexLockInit(&m_lock, &m_mutexattr);
}

CMutex::~CMutex() {
    MutexLockAttrDestroy(&m_mutexattr);
    MutexLockDestroy(&m_lock);
}

int CMutex::lock() {
    return MutexLock(&m_lock);
}

int CMutex::unlock() {
    return MutexUnlock(&m_lock);
}

int CMutex::trylock() {
    return MutexTrylock(&m_lock);
}

}
