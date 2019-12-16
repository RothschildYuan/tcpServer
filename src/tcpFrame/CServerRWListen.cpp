#include "CServerRWListen.h"

#include "CServerSocket.h"
#include "CApplication.h"
#include "CServerConnectListen.h"
#include "CServerConnector.h"
#include "CLogger.h"
#include "CDateTime.h"
#include "SocketUtil.h"
#include "CThreadPool.h"

CServerRWListen::CServerRWListen(CServerSocket *tSerSocket, bool isDetach)
    :CBaseThread(isDetach)
{
    m_psServer  = tSerSocket;
    m_epd       = 0;
    m_nCount.store(0);
}

int CServerRWListen::eventSet(CServerConnector* cConn) {
    if(cConn == nullptr) {
        return 0;
    }
    // (在closesocket()调用,但是还有数据没发送完毕的时候容许逗留)
    // (容许逗留的时间为0秒)
    cConn->setLinger(true, 0);
    cConn->setNonBlock(true);
    m_connMutex.lock();
    cConn->eventAdd(m_epd, EPOLLIN);
    ++m_nCount;
    m_connMutex.unlock();
    // 调用 tcp 连接回调函数，以供上层使用
    m_psServer->m_connEventCB(cConn);
    return 0;
}

int CServerRWListen::eventDel(CServerConnector* cConn) {
    if(cConn == nullptr) {
        return 0;
    }
    // 更新最近的事件操作时间，连接长时间无事件操作需要被删除
    cConn->m_heartTime = GettickCount();
    // 查看缓存中的数据长度
    int len = cConn->m_readlen = cConn->receiveLength();
    if (len > 0)
    {
        print1("receiveLength : %d\n", len);
        return len;
    }
    if(len < 0) {
        print1("CServerRWListen error receiveLength : %d\n", len);
        if(len == -2) {
            return len;
        }
    }
    // 从 epoll 反应堆中删除监听事件
    m_connMutex.lock();
    cConn->eventDel(m_epd);
    --m_nCount;
    m_connMutex.unlock();
    // 调用 tcp 断开连接回调函数，以供上层使用
    m_psServer->m_disconnEventCB(cConn);

    cConn->clear(true);
    return len;
}

int CServerRWListen::processHandle() {
    // 如果 epoll 没有监听任何事件
    if(getConnCount() == 0) {
        Sleep(100);
        return 0;
    }
    print1("CServerRWListen count : %d  m_listConns.size:  %d\n", getConnCount(), m_listConns.size());
    CServerConnector* event = nullptr;
    struct epoll_event eps[EPOLL_SIZE];
    // epoll 监听读写事件
    int readn = Epoll_wait(m_epd, eps, EPOLL_SIZE, -1);
    print1("CServerRWListen readn = %d\n", readn);
    if(readn == -1) {
        perror_exit("epoll_wait error");
    }
    for(int i = 0;i<readn;++i) {
        event = (CServerConnector*)eps[i].data.ptr;
        if(eps[i].events & EPOLLIN) {
            // read
            // 查看 tcp 连接滑动窗口中的数据，> 0 则调用用户设置的读事件回调函数
            int ret = eventDel(event);
            if(ret > 0) {
                ret = event->readSocket();
                if(ret <= 0
                        || ret > 1) {
                    continue;
                }
                if(m_psServer->m_thrPool == nullptr) {
                    m_psServer->m_readEventCB(event);
                } else {
                    // 如果设置了任务线程池，则把任务添加进任务队列中
                    m_psServer->m_thrPool->addTask(m_psServer->m_readEventCB, event);
                }
            }
        }
//        if(eps[i].events & EPOLLOUT) {
//            // write
//            if(m_psServer->m_writeEventCB != nullptr) {
//                m_psServer->m_writeEventCB(event);
//            }
//        }
    }
    return 0;
}

int CServerRWListen::getConnCount() {
    return m_nCount.load();
}

int CServerRWListen::addConnect(int handle) {
    // read
    CServerConnector *Connector_t = nullptr;
    m_connMutex.lock();
    // 从现有的 tcp 连接队列中查询未使用的 tcp 连接对象，并循环使用
    for(CServerConnector* it : m_listConns) {
        if(it->isActive() == false) {
            Connector_t = it;
            break;
        }
    }
    m_connMutex.unlock();
    // 如不存在未使用的 tcp 连接对接，则创建新的 tcp 连接，并把 tcp 连接添加到连接队列中
    if (Connector_t == nullptr)
    {
        Connector_t = new CServerConnector(m_psServer);
        m_connMutex.lock();
        m_listConns.push_back(Connector_t);
        m_connMutex.unlock();
    }
    int SockAddrSize_n4 = sizeof(RSockAddrIn);
    // 创建 socket 连接
    int cfd = Accept(handle, &Connector_t->m_addr, &SockAddrSize_n4);
    if (cfd == INVALID_SOCKET){
        print1("Accept error == INVALID_SOCKET socket = %d\n", cfd);
        throw std::logic_error("Accept error == INVALID_SOCKET");
        return -1;
    }
    if (cfd < 0){
        print1("Accept error < 0 socket = %d\n", cfd);
        throw std::logic_error("Accept error < 0");
        return -1;
    }
    Connector_t->setHandle(cfd);
    string host = Connector_t->setHost();
    print1("socket =  %d host: %s\n", cfd, host.c_str());
    Connector_t->m_heartTime = GettickCount();
    Connector_t->setIsActive(false);
    // 把连接放入 epoll 进行监听
    return eventSet(Connector_t);
}

void CServerRWListen::updateConnector(UInt4 NowTime_u4i)
{
    int decr;
    m_connMutex.lock();
    for(CServerConnector* iter : m_listConns) {
        if(iter->isActive() == false) {
            continue;
        }
        m_connMutex.unlock();
        decr = NowTime_u4i - iter->m_heartTime;
        if (decr <= 0){
            iter->m_heartTime = NowTime_u4i;
            m_connMutex.lock();
            continue;
        }
        // 连接超时，则删除该连接，并从 epoll 中删除该事件
        if (decr >= (int)m_psServer->m_timeout)
        {
            m_connMutex.lock();
            iter->eventDel(m_epd);
            --m_nCount;
            m_connMutex.unlock();
            m_psServer->m_disconnEventCB(iter);
            iter->clear(true);
        }
        m_connMutex.lock();
    }
    m_connMutex.unlock();
}

void CServerRWListen::execute()
{
    if(m_psServer == nullptr) {
        throw std::logic_error("m_psServer cannot nullptr, please set m_psServer");
    }

    // 创建 epoll 反应堆
    m_epd = Epoll_create(EPOLL_SIZE);
    for (Int4 i = 0; i <= 20000; i++)
    {
        if (CApplication::GetInstance()->isTerminate()) {
            return ;
        }
        if (m_psServer->isActive()) {
            break;
        }
        Sleep(20);
    }
    if (!m_psServer->isActive()) {
        return;
    }
    CApplication::GetInstance()->threadInc();
    while (!m_isExited)
    {
        if (CApplication::GetInstance()->isTerminate()) {
            break;
        }
        if (!m_psServer->isActive()) {
            break;
        }
        // 执行 epoll 监听事件
        if (!processHandle())
        {
            Sleep(10);
        }
    }
    CApplication::GetInstance()->threadDec();
    if (m_isDetach) {
        delete this;
    }
}
