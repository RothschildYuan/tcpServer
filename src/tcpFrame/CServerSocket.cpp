#include "CServerSocket.h"

#include "CApplication.h"
#include "CDateTime.h"
#include "CThreadPool.h"
#include "CServerConnectUpdate.h"
#include "CServerConnectListen.h"
#include "CServerRWListen.h"
#include "CServerConnector.h"
#include "SocketUtil.h"
#include "CLogger.h"

CServerSocket::CServerSocket() : CSocket(),
    m_connEventCB(nullptr),
    m_disconnEventCB(nullptr),
    m_readEventCB(nullptr),
    m_psUpdate(nullptr),
    m_psConnListion(nullptr)
{
    m_psRWListion.clear();
    m_connCount     = 0;
    m_timeout       = 600000;
    m_port          = 8899;
    m_thrPool       = nullptr;    // 默认不启动线程池
    print1("CServerSocket \n");
}

CServerSocket::~CServerSocket()
{
    if (m_isActive) {
        clear();
    }
}

bool CServerSocket::open(UInt4 nthreadCount)
{
    // 判断是否设置 连接，断开连接，读操作的回调函数
    if(m_connEventCB == nullptr
            || m_disconnEventCB == nullptr
            || m_readEventCB == nullptr)
    {
        throw std::logic_error("connEventCB，disconnEventCB，readEventCB cannot nullptr, "
                               "please set connEventCB，disconnEventCB，readEventCB");
    }

    bool bResult = m_isActive;
    // 如果该服务已激活，则不能重新进行再次激活
    if (m_isActive)
        return bResult;
    m_connCount = 0xFFFF;

    // 开启连接事件监听线程
    m_psConnListion = new CServerConnectListen(this, true);
    m_psConnListion->start();

    for (UInt4 i = 0; i <= 5000; i++)
	{
        if (m_connCount != 0xFFFF)
        {
            // 激活 tcp 服务，其他线程跟着启动服务
            m_isActive = true;
			break;
        }
        Sleep(20);
	}

    // 如果设置线程池，则启动处理任务线程池
    if(m_thrPool != nullptr) {
        m_thrPool->execute();
    }

    if (m_isActive)
    {
        // 开启超时连接管理线程
        m_psUpdate = new CServerConnectUpdate(this, true);
        m_psUpdate->start();

        for (UInt4 i = 0; i < nthreadCount; i++)
        {
            // 开启 epoll 返应堆，监听事件读写操作线程，（若不设置任务线程池，该线程负责处理任务）
            CServerRWListen *psEvent = new CServerRWListen(this, true);
            psEvent->start();
            m_psRWListion.push_back(psEvent);
        }
    }
    return m_isActive;
}

void CServerSocket::clear()
{
    if (!m_isActive) {
        return;
    }
    // 关闭连接事件监听线程
    if (m_psConnListion != nullptr)
    {
        m_psConnListion->close();
        m_psConnListion = nullptr;
    }

    // 关闭超时连接管理线程
    if (m_psUpdate != nullptr)
    {
        m_psUpdate->close();
        m_psUpdate = nullptr;
    }

    // 关闭 epoll 返应堆，监听事件读写操作线程
    for(CServerRWListen *it : m_psRWListion) {
        it->close();
        it = nullptr;
    }
    m_psRWListion.clear();
    m_isActive = false;
}

void CServerSocket::setNotify(NotifyEventCB Connect_oi,
                                  NotifyEventCB Disconnect_oi,
                                  NotifyEventReadCB Read_oi)
{
    m_connEventCB       = Connect_oi;
    m_disconnEventCB    = Disconnect_oi;
    m_readEventCB       = Read_oi;
}

void CServerSocket::setThreadPool(CThreadPool *thrPool)
{
    if(m_thrPool != nullptr) {
        SDelete(m_thrPool);
    }
    m_thrPool = thrPool;
}

void CServerSocket::setThreadPoolRange(Int4 min_thr_num,
                                       Int4 max_thr_num,
                                       Int4 queue_max_size)
{
    CThreadPool* thrPool = new CThreadPool(min_thr_num, max_thr_num, queue_max_size);
    setThreadPool(thrPool);
}

void CServerSocket::eventAdd(int epfd, int events)
{
    int op;
    // 设置 epoll 监听事件
    struct epoll_event epv = {0, {0}};
    epv.data.ptr = this;
    epv.events = events;
    if(m_isActive == true) {
        op = EPOLL_CTL_MOD;
    } else {
        op = EPOLL_CTL_ADD;
        m_isActive = true;
    }

    // 把监听事件添加到 epoll 反应堆中
    if(epoll_ctl(epfd, op, m_handle, &epv) < 0) {
        print1("CServerSocket event add failed [fd=%d], op=%d, events[%d]\n", m_handle, op, events);
    } else {
        print1("CServerSocket event add OK [fd==%d], op=%d, events[%d]\n", m_handle, op, events);
    }
}

void CServerSocket::setTimeout(const UInt4 &timeout)
{
    m_timeout = timeout;
}
