#ifndef CAPPLICATION_H
#define CAPPLICATION_H

#include "stdtype.h"

class CApplication
{
    CApplication();
    CApplication(const CApplication&);
    CApplication& operator=(const CApplication&);
public:
    static CApplication *GetInstance();

    // 增加程序线程数
    void threadInc();
    // 减少程序线程数
    void threadDec();
    // 获取程序线程数
    UInt4 getThreadCount();
    // 是否终止程序
    bool isTerminate();
    // 终止程序
    void terminate();
    // 是否所有线程都退出
    bool isAllThreadExited();

private:
    int m_threadCount;  // 激活线程数
    bool m_terminated;  // 是否终止进程
    ACTMutex m_mutex;
};

#endif  
