#include "CApplication.h"

CApplication::CApplication()
{
    m_threadCount = 0;
    m_terminated = false;
}

CApplication *CApplication::GetInstance(){
    static CApplication m_instance;
    return &m_instance;
}

void CApplication::threadInc()
{
    m_mutex.lock();
    m_threadCount ++;
    m_mutex.unlock();
}

void CApplication::threadDec()
{
    m_mutex.lock();
    m_threadCount --;
    m_mutex.unlock();
}

UInt4 CApplication::getThreadCount()
{
    int Result = 0;
    m_mutex.lock();
    Result = m_threadCount;
    m_mutex.unlock();
    return Result;
}

bool CApplication::isTerminate()
{
    return m_terminated;
}

void CApplication::terminate()
{
    m_terminated = true;
}

bool CApplication::isAllThreadExited()
{    
    bool Result_b1 = false;
    m_mutex.lock();
    Result_b1 = (m_threadCount > 0 ? false : true);
    m_mutex.unlock();
    return Result_b1;
}
