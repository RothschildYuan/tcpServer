#include "CBaseThread.h"

CBaseThread::CBaseThread(bool isDetach)
{
    m_isExited  = false;
    m_isDetach  = isDetach;
//    m_thread    = nullptr;
}

CBaseThread::~CBaseThread() {
//    SDelete(m_thread);
}

void CBaseThread::start()
{
    ACTThread thr(threadProc, this);
    stringstream ss;
    ss << thr.get_id();
    ss >> m_threadID;
//    m_thread = new ACTThread(threadProc, this);
//    m_thread = std::move(ACTThread(threadProc, this));
    if(m_isDetach) {
        thr.detach();
    }
}

void CBaseThread::close()
{
    m_isExited = true;
}

UInt8 CBaseThread::getThread()
{
    return m_threadID;
}

void* CBaseThread::threadProc(void *arg)
{
    CBaseThread *thread_pt = static_cast<CBaseThread *>(arg);
    thread_pt->execute();
    return nullptr;
}
