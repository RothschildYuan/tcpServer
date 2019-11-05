#ifndef CADJUSTTHREAD_H
#define CADJUSTTHREAD_H

#include "CBaseThread.h"

class CThreadPool;

class CAdjustThread : public CBaseThread
{
    CAdjustThread(const CAdjustThread&);
    CAdjustThread& operator=(const CAdjustThread&);
public:
    CAdjustThread(CThreadPool* threadPool);
    virtual void execute();

private:
    CThreadPool* m_pool;
};

#endif // CADJUSTTHREAD_H
