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

    const int DEFAULT_TIME          = 3;                 //10s检测一次
    const int MIN_WAIT_TASK_NUM     = 10;            //如果queue_size > MIN_WAIT_TASK_NUM 添加新的线程到线程池
    const int DEFAULT_THREAD_VARY   = 10;          //每次创建和销毁线程的个数
};

#endif // CADJUSTTHREAD_H
