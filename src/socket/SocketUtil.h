/****************************************************************************** 
 
            版权所有 (C), 2016-2017, 深圳市沃土科技有限公司 
 
 ****************************************************************************** 
    文 件 名 : QSocketError.h
    版 本 号 : V1.0 
    作    者 : 钱旺 
    生成日期 : 2017年9月9日 
    功能描述 : socket跨平台、错误处理类头文件
    修改历史 : 
******************************************************************************/ 
#ifndef SOCKETBASE_H_
#define SOCKETBASE_H_

#include "config.h"
#include "stdtype.h"

void perror_exit(const char* errmsg);

// 程序启动后调用，才可以使用socket
void SocketInit();

// 程序退出前调用，释放socket资源
void SocketDestroy();

// 创建 epoll 根节点
int Epoll_create(int num);

int Epoll_wait(int __epfd, struct epoll_event *__events, int __maxevents, int __timeout);

// 创建 socket
int Socket(int domain, int type, int protocol);

// socket 绑定地址
int Bind(int sockfd, const struct sockaddr_in *addr, int addrlen);

// 设置监听 默认 127
int Listen(int sockfd, int backlog);

// 连接 socket
int Connect(int sockfd, const struct sockaddr_in *addr, int addrlen);

// 接收 socket 连接
int Accept(int sockfd, struct sockaddr_in *addr, int *addrlen);

// 写数据
int Write(int fd, const void *buf, int count);

// 读数据
int Read(int fd, void *buf, int count);

// 关闭 socket
int Close(int fd);

struct in_addr Inet_addr(const char* cp);

// UDP 协议读数据
int Recvfrom (int __fd, void * __buf, size_t __n, int __flags, struct sockaddr_in * __addr, int * __addr_len);

// UDP 协议写数据
int Sendto (int __fd, const void *__buf, size_t __n, int __flags, struct sockaddr_in * __addr, int __addr_len);

int Setsockopt (int __fd, int __level, int __optname,const void *__optval, int __optlen);
int Getsockopt (int __fd, int __level, int __optname, void *__optval, int* __optlen);

// 设置超时
int SetSockTimeout(int fd, int read_sec, int write_sec);
int SetSendTimeout(int fd, int write_sec);
int SetRecvTimeout(int fd, int read_sec);

// 设置端口重用 / 地址重用
int SetReuseAddr(int handle);

// (在closesocket()调用,但是还有数据没发送完毕的时候容许逗留)
// (容许逗留的时间为0秒)
int SetLinger(int handle, bool is_linger, size_t sec);

//设置非阻塞
int SetNonBlock(int handle, bool is_nonblock = false);

// 设置探测分节
// keepIdle:        探测分节时间间隔，系统默认两小时
// keepInterval:    当没收到响应时，重新发送探测数据的间隔时间，系统默认 75 s
// keepCount:       当没收到响应时，重新发送探测数据的次，系统默认 9 次
int SetKeepAlive(int handle, int keepIdle = 60, int keepInterval = 5, int keepCount = 3);

// 设置接收端滑动窗口大小
Int4 SetRecvBufSize(int handle, UInt4 Size_u4i);

// 获取接收端滑动窗口大小
Int4 GetRecvBufSize(int handle);

// 设置发送端滑动窗口大小
Int4 SetSendBufSize(int handle, UInt4 Size_u4i);

// 获取发送端滑动窗口大小
Int4 GetSendBufSize(int handle);

// 获取缓存域数据长度
int Ioctlsocket(int handle, int request, void* arg);

#ifdef __linux__

/*参三: 应该读取的字节数*/                          //socket 4096  readn(cfd, buf, 4096)   nleft = 4096-1500
ssize_t Readn(int fd, void *vptr, size_t n);

ssize_t Writen(int fd, const void *vptr, size_t n);

// readline --- fgets 传出参数 vptr
ssize_t Readline(int fd, void *vptr, size_t maxlen);

#endif

#endif
