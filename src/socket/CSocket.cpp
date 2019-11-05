#include "CSocket.h"

#include "SocketUtil.h"

CSocket::CSocket()
{
    SocketInit();
    memset(&m_addr, 0, sizeof(m_addr));
    m_port          = 0;
    m_handle        = INVALID_SOCKET;
    m_isActive      = false;
    m_host          = "0.0.0.0";
}

CSocket::~CSocket()
{
    SocketDestroy();
}

// 设置地址重用
int CSocket::setReuseAddr() {
    return SetReuseAddr(m_handle);
}

// (在closesocket()调用,但是还有数据没发送完毕的时候容许逗留)
// (容许逗留的时间为0秒)
int CSocket::setLinger(bool is_linger, size_t sec) {
    return SetLinger(m_handle, is_linger, sec);
}

//设置非阻塞
int CSocket::setNonBlock(bool is_nonblock) {
    return SetNonBlock(m_handle, is_nonblock);
}

int CSocket::setSockTimeout(int read_sec, int write_sec)
{
    return SetSockTimeout(m_handle, read_sec, write_sec);
}

int CSocket::setRecvBufSize(UInt4 nSize)
{
    return SetRecvBufSize(m_handle, nSize);
}

int CSocket::getRecvBufSize()
{
    return GetRecvBufSize(m_handle);
}

int CSocket::setSendBufSize(UInt4 nSize)
{
    return SetSendBufSize(m_handle, nSize);
}

int CSocket::getSendBufSize()
{
    return GetSendBufSize(m_handle);
}

int CSocket::setSockAddr(UInt2 family) {
    m_addr.sin_family = family;
    m_addr.sin_port = htons(m_port);
    m_addr.sin_addr = Inet_addr(m_host.c_str());
    // m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    return 0;
}

string CSocket::host() const
{
    return m_host;
}

void CSocket::setHost(const string &host)
{
    m_host = host;
}

string& CSocket::setHost()
{
    m_host = inet_ntoa(m_addr.sin_addr);
    return m_host;
}

UInt2 CSocket::port() const
{
    return m_port;
}

void CSocket::setPort(const UInt2 &port)
{
    m_port = port;
}

int CSocket::handle() const
{
    return m_handle;
}

void CSocket::setHandle(int handle)
{
    m_handle = handle;
}

bool CSocket::isActive() const
{
    return m_isActive;
}

void CSocket::setIsActive(const bool &isActive)
{
    m_isActive = isActive;
}
