#include "CServerConnector.h"

#include "CServerSocket.h"
#include "CLogger.h"
#include "SocketUtil.h"

CServerConnector::CServerConnector() : CSocket()
{
    m_server        = nullptr;
    m_heartTime     = 0;
    m_readlen       = -1;
}

CServerConnector::CServerConnector(CServerSocket *Parent_ti) : CSocket()
{
    m_server        = Parent_ti;
    m_heartTime     = 0;
    m_readlen       = -1;
}

CServerConnector::~CServerConnector()
{
    clear(true);
}

int CServerConnector::sendBuf(const char *str, size_t len) {
    if(m_isActive == false || m_handle <= 0) {
        return -1;
    }
    return send(m_handle, str, len, 0);
}

int CServerConnector::sendBuf(const string &str) {
    return sendBuf(str.c_str(), str.length());
}

vector<shared_ptr<char> > CServerConnector::getSockDatas() {
    vector<shared_ptr<char> > tmp = m_sockDatas;
    m_sockDatas.clear();
    return tmp;
}

void CServerConnector::clearCache() {
    m_sockDatas.clear();
}

int CServerConnector::readSocket() {
    char* ptr = (char*)calloc(1, m_readlen+128);
    if(ptr == nullptr) {
        return -1;
    }
    int ret = readBuf(ptr, m_readlen+128);
    if(ret <= 0) {
        SFree(ptr)
        return ret;
    }
    shared_ptr<char> pchar(ptr, [](char* pstr)->void { SFree(pstr); });
    m_sockDatas.push_back(pchar);
    return m_sockDatas.size();
}

int CServerConnector::readBuf(char *str, size_t len)
{
    if(m_isActive == false || m_handle <= 0) {
        return -1;
    }
    return recv(m_handle, str, len, 0);
}

int CServerConnector::readlen() const
{
    return m_readlen;
}

void CServerConnector::clear(bool ShutDown_b1i)
{
    m_mutex.lock();
    if (ShutDown_b1i) {
        if (m_handle != INVALID_SOCKET)
        {
            shutdown(m_handle, SHUT_WR);// SHUT_RDWR, SHUT_RD, SHUT_WR
        }
    }

    if (m_handle != INVALID_SOCKET)
    {
        Close(m_handle);
        m_handle = INVALID_SOCKET;
    }
    m_isActive = false;
    m_mutex.unlock();
}

Int4 CServerConnector::receiveLength()
{
    Int4 Length_n4;
    m_mutex.lock();
    do {
        if (!m_isActive) {
            print1("receiveLength m_isActive error m_handle : %d\n", m_handle);
            Length_n4 = -3;
            break;
        }

        if (m_handle == INVALID_SOCKET) {
            print1("receiveLength INVALID_SOCKET error m_handle :  %d\n", m_handle);
            Length_n4 = -2;
            break;
        }

        Int4 Result = Ioctlsocket(m_handle, FIONREAD, &Length_n4);
        if (Result != 0)
        {
            Length_n4 = -1;
            break;
        }
    } while(0);
    m_mutex.unlock();
    return Length_n4;
}

void CServerConnector::eventAdd(int epfd, int events)
{
    int op;
    struct epoll_event epv = {0, {0}};
    m_mutex.lock();
    epv.data.ptr = this;
    epv.events = events | EPOLLET;
    if(m_isActive == true) {
        op = EPOLL_CTL_MOD;
    } else {
        op = EPOLL_CTL_ADD;
        m_isActive = true;
    }

    if(epoll_ctl(epfd, op, m_handle, &epv) < 0) {
        print1("CServerConnector event add failed [fd=%d], op=%d, events[%d]\n", m_handle, op, epv.events);
    } else {
        print1("CServerConnector event add OK [fd=%d], op=%d, events[%d]\n", m_handle, op, epv.events);
    }
    m_mutex.unlock();
}

void CServerConnector::eventDel(int epfd) {
    struct epoll_event epv = {0, {0}};
    m_mutex.lock();
    epv.data.ptr = this;
    epv.events = 0;
    int op = EPOLL_CTL_DEL;
    m_isActive = false;

    if(epoll_ctl(epfd, op, m_handle, &epv) < 0) {
        print1("event del failed [fd=%d], op=%d, events[%d]\n", m_handle, op, epv.events);
    } else {
        print1("event del OK [fd=%d], op=%d, events[%d]\n", m_handle, op, epv.events);
    }
    m_mutex.unlock();
}
