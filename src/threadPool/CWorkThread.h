#ifndef CWORKTHREAD_H
#define CWORKTHREAD_H

#include "CBaseThread.h"

class CThreadPool;

class CWorkThread : public CBaseThread
{
    CWorkThread(const CWorkThread&);
    CWorkThread& operator=(const CWorkThread&);
public:
    CWorkThread(CThreadPool* threadPool);
    virtual void execute();

private:
    CThreadPool* m_pool;
};

#endif // CWorkThread_H
