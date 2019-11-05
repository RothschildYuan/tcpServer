#ifndef __CSOCKET_H__
#define __CSOCKET_H__

#include "stdtype.h"
#include "config.h"

typedef struct sockaddr_in RSockAddrIn;

class CSocket
{
    CSocket(const CSocket& oth);
    CSocket& operator=(const CSocket& oth);
public:
    CSocket();
    virtual ~CSocket();

    // 设置地址重用
    int setReuseAddr();
    // (在closesocket()调用,但是还有数据没发送完毕的时候容许逗留)
    // (容许逗留的时间为0秒)
    int setLinger(bool is_linger, size_t sec);
    //设置非阻塞
    int setNonBlock(bool is_nonblock);
    // 设置超时
    int setSockTimeout(int read_sec, int write_sec);
    // 设置接收滑动窗口大小
    int setRecvBufSize(UInt4 nSize = 200 * 1024);
    // 获取接收滑动窗口大小
    int getRecvBufSize();
    // 设置发送滑动窗口大小
    int setSendBufSize(UInt4 nSize = 200 * 1024);
    // 获取发送滑动窗口大小
    int getSendBufSize();

    int setSockAddr(UInt2 family = AF_INET);
    string host() const;
    void setHost(const string &host);
    string &setHost();
    UInt2 port() const;
    void setPort(const UInt2 &port);
    int handle() const;
    void setHandle(int handle);
    bool isActive() const;
    void setIsActive(const bool &isActive);

    RSockAddrIn     m_addr;         // 套接字地址
protected:
    string          m_host;         // 主机地址
    UInt2           m_port;         // 端口号
    int             m_handle;       // 套接字，句柄
    bool            m_isActive;     // 是否激活
};


#endif
