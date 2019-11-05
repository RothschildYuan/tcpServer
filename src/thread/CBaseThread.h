#ifndef CBASETHREAD_H
#define CBASETHREAD_H

#include "config.h"
#include "stdtype.h"

class CBaseThread
{
public:
    CBaseThread(bool isDetach = true);
    virtual ~CBaseThread();

    /**
     * @brief start         start 线程启动函数
     */
    void start();

    /**
     * @brief execute       start 线程启动函数，默认执行该函数，需要子对象继承函数
     */
    virtual void execute() = 0;

    /**
     * @brief close         关闭线程
     */
    virtual void close();

    /**
     * @brief getThread     获取线程 id
     * @return
     */
    UInt8 getThread();

private:

    /**
     * @brief threadProc    pthread_create 回调函数，把回调函数引导 execute() 函数上
     * @param arg
     * @return
     */
    static void* threadProc(void *arg);

protected:
    bool    m_isExited;
    bool    m_isDetach;//自动释放的标志
    UInt8   m_threadID;
};

#endif // BASETHREAD_H
