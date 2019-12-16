#include "CServerConnectListen.h"

#include "CServerSocket.h"
#include "CServerConnector.h"
#include "CApplication.h"
#include "CServerRWListen.h"
#include "CDateTime.h"
#include "SocketUtil.h"
#include "CLogger.h"

CServerConnectListen::CServerConnectListen(CServerSocket *tSerSocket, bool isDetach)
    :CBaseThread(isDetach)
{
    m_psServer = tSerSocket;
}

void CServerConnectListen::execute()
{
    if(m_psServer == nullptr) {
        throw std::logic_error("m_psServer cannot nullptr, please set m_psServer");
    }

    // 创建 tcp socket，设置超时，epoll 事件监听，超时，非阻塞，并对其进行绑定，监听
    m_psServer->m_handle = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    m_epd = Epoll_create(EPOLL_SIZE);
    m_psServer->eventAdd(m_epd, EPOLLIN);
    m_psServer->setSockTimeout(70, 70);
    m_psServer->setNonBlock(true);
    m_psServer->setReuseAddr();
    m_psServer->setRecvBufSize(1024*1024);
    m_psServer->setSendBufSize(1024*1024);
    // (在closesocket()调用,但是还有数据没发送完毕的时候容许逗留)
    // (容许逗留的时间为0秒)
    m_psServer->setLinger(true, 0);
    m_psServer->setSockAddr();
    Bind(m_psServer->m_handle, &(m_psServer->m_addr), sizeof(RSockAddrIn));
    Listen(m_psServer->m_handle, 127);
    m_psServer->m_connCount = 0;
    for (Int4 i = 0; i <= 20000; i++)
    {
        if (CApplication::GetInstance()->isTerminate()) {
            return ;
        }
        // 启动连接事件监听
        if (m_psServer->m_isActive) {
            break;
        }
        Sleep(20);
    }

    if (!m_psServer->m_isActive) {
        return;
    }
    CApplication::GetInstance()->threadInc();
    while (!m_isExited)
    {
        if (CApplication::GetInstance()->isTerminate()) {
            break;
        }
        if (!m_psServer->m_isActive) {
            break;
        }
        // 添加连接，如果没有新的连接，则一直阻塞着
        if (!addConnect())
        {
            usleep(10);
        }
    }
    CApplication::GetInstance()->threadDec();
    if (m_isDetach) {
        delete this;
    }
}

int CServerConnectListen::addConnect() {
    struct epoll_event eps[EPOLL_SIZE];
    // 监听连接事件
    int readn = Epoll_wait(m_epd, eps, EPOLL_SIZE, -1);
    print1("CServerConnectListen readn = %d\n", readn);
    if(readn == -1) {
        perror_exit("epoll_wait error");
    }
    if(readn > 1) {
        print1("CServerConnectListen::addConnect epoll_wait error readn: %d\n\n\n", readn);
        exit(-1);
    }
    for(int i = 0;i<readn;++i) {
        if(eps[i].events & EPOLLIN) {
            int count = 1024*10;
            // 取连接监听数最少的 epoll 反应堆线程
            CServerRWListen* tmp = nullptr;
            for(CServerRWListen* it : m_psServer->m_psRWListion) {
                if(it->m_nCount < 50) {
                    tmp = it;
                    break;
                }
                if(count > it->getConnCount()) {
                    count = it->getConnCount();
                    tmp = it;
                }
            }
            if(tmp != nullptr) {
                // 把新的新创建的连接添加到该 epoll 反应堆中
                tmp->addConnect(m_psServer->m_handle);
            }
        }
    }
    return 0;
}

void CServerConnectListen::updateConnector(UInt4 NowTime_u4i)
{
    for(CServerRWListen* it : m_psServer->m_psRWListion) {
        // 处理各 epoll 线程的超时连接
        it->updateConnector(NowTime_u4i);
        Sleep(20);
    }
}
