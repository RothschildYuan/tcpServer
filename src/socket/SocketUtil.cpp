#include "SocketUtil.h"

void perror_exit(const char* errmsg) {
    perror(errmsg);
//    exit(-1);
}

void SocketInit()
{
#ifdef WIN32
    WSADATA  Ws;    //Init Windows Socket
    Int4 Ret = WSAStartup(MAKEWORD(2,2), &Ws);
    if(Ret != 0) {
        printf("WSAStartup fail \r\n");
    } else {
        printf("WSAStartup success! \r\n");
    }
#endif
}

void SocketDestroy()
{
#ifdef WIN32
    WSACleanup();
#endif
}

int Epoll_create(int num) {
    int g_epd = -1;
#ifdef __linux__
    g_epd = epoll_create(num);
    if(g_epd == -1) {
        perror_exit("epoll_create error");
    }
#endif
    return g_epd;
}

int Epoll_wait(int __epfd, struct epoll_event *__events, int __maxevents, int __timeout) {
Restart:
    int ret = epoll_wait(__epfd, __events, __maxevents, __timeout);
    if(ret == -1) {
        if(errno == EINTR) {
            goto Restart;
        } else {
            perror_exit("Epoll_wait error");
        }
    }
    return ret;
}

int Socket(int domain, int type, int protocol){
    int res = socket(domain, type, protocol);
    if (res == INVALID_SOCKET){//-1
        perror_exit("Socket error");
    }
    return res;
}

int Bind(int sockfd, const struct sockaddr_in *addr, int addrlen){
    int res = bind(sockfd, (const struct sockaddr *)addr, (socklen_t)addrlen);
    if (res == SOCKET_ERROR){//0,-1
        Close(sockfd);
        perror_exit("Bind error");
    }
    return res;
}

int  Listen(int sockfd, int backlog){
    int res = listen(sockfd, backlog);
    if (res == SOCKET_ERROR){//0,-1
        Close(sockfd);
        perror_exit("Listen error");
    }
    return res;
}

int Connect(int sockfd, const struct sockaddr_in *addr, int addrlen){
    int res = connect(sockfd, (const struct sockaddr *)addr, (socklen_t)addrlen);
    if (res == SOCKET_ERROR){//0.-1
        Close(sockfd);
        perror_exit("Connect error");
    }
    return res;
}

int Accept(int sockfd, struct sockaddr_in *addr, int *addrlen){
    int res;
again:
    res = accept(sockfd, (struct sockaddr *)addr, (socklen_t*)addrlen);
    if (res == INVALID_SOCKET){
#ifdef  __linux__
        if((errno == ECONNABORTED) || (errno == EINTR)) {
            goto again;
        }
#endif
        perror_exit("Accept error");
    }
    return res;
}

int Write(int fd, const void *buf, int count){
    int res = send(fd, buf, count, 0);
    if (res == SOCKET_ERROR){ //-1
        perror_exit("Write error");
    }
    return res;
}

int Read(int fd, void *buf, int count){
    int res = 0;
again:
    res = recv(fd, buf, count, 0);
    if (res == SOCKET_ERROR){// -1
#ifdef __linux__
        if (errno == EINTR) {
            goto again;
        }
#endif
        perror_exit("Read error");
    }
    return res;
}

int Close(int fd){  //0,-1
#ifdef  WIN32
    int res = closesocket(fd);
#else
    int res = close(fd);
#endif
    if (res == SOCKET_ERROR){
        return res;
    }
    return res;
}

struct in_addr Inet_addr(const char* cp){
    struct in_addr i_addr;
#ifdef  WIN32
    i_addr.S_un.S_addr = inet_addr(cp);
#else
    if (inet_pton(AF_INET, cp, &i_addr.s_addr) != 1){
        perror_exit("Inet_addr error");
    }
#endif
    return i_addr;
}

int Recvfrom (int __fd, void * __buf, size_t __n, int __flags, struct sockaddr_in * __addr, int * __addr_len) {
    int ret = recvfrom(__fd,  __buf,  __n, __flags, (struct sockaddr *)__addr, (socklen_t *) __addr_len);
    if(ret == SOCKET_ERROR) {
        perror_exit("Sendto error");
    }
    return ret;
}

int Sendto (int __fd, const void *__buf, size_t __n, int __flags, struct sockaddr_in * __addr, int __addr_len) {
    int ret = sendto(__fd, __buf, __n,  __flags, (struct sockaddr *)__addr, (socklen_t)__addr_len);
    if(ret == SOCKET_ERROR) {
        perror_exit("Sendto error");
    }
    return ret;
}

int Setsockopt (int __fd, int __level, int __optname,const void *__optval, int __optlen) {
    int res = setsockopt ( __fd,  __level,  __optname, __optval, (socklen_t)__optlen);
    if(res == -1) {
        perror_exit("Setsockopt error");
    }
    return res;
}

int Getsockopt (int __fd, int __level, int __optname, void *__optval, int* __optlen)
{
    int res = getsockopt(__fd, __level, __optname, __optval, (socklen_t *)__optlen);
    if(res == -1) {
        perror_exit("Setsockopt error");
    }
    return res;
}

int SetSockTimeout(int fd, int read_sec, int write_sec)
{
    struct timeval send_timeval;
    struct timeval recv_timeval;
    if(fd <= 0) return -1;
    send_timeval.tv_sec = write_sec<0?0:write_sec;
    send_timeval.tv_usec = 0;
    recv_timeval.tv_sec = read_sec<0?0:read_sec;
    recv_timeval.tv_usec = 0;
    if(Setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &send_timeval, sizeof(send_timeval)) == -1)
    {
        return -1;
    }
    if(Setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeval, sizeof(recv_timeval)) == -1)
    {
        return -1;
    }
    return 0;
}

int SetSendTimeout(int fd, int write_sec)
{
    struct timeval send_timeval;
    if(fd <= 0) return -1;
    send_timeval.tv_sec = write_sec<0?0:write_sec;
    send_timeval.tv_usec = 0;
    return Setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &send_timeval, sizeof(send_timeval));
}

int SetRecvTimeout(int fd, int read_sec)
{
    struct timeval recv_timeval;
    if(fd <= 0) return -1;
    recv_timeval.tv_sec = read_sec<0?0:read_sec;
    recv_timeval.tv_usec = 0;
    return Setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeval, sizeof(recv_timeval));
}

// 设置端口重用 / 地址重用
int SetReuseAddr(int handle) {
    int ret = 0;
    int opt = 1;
    ret = Setsockopt(handle,SOL_SOCKET,SO_REUSEADDR,(void *)&opt,sizeof(opt));
    if(ret != 0) {
        return ret;
    }
    ret = Setsockopt(handle,SOL_SOCKET,SO_REUSEPORT,(void *)&opt,sizeof(opt));
    return ret;
}

// (在closesocket()调用,但是还有数据没发送完毕的时候容许逗留)
// (容许逗留的时间为0秒)
int SetLinger(int handle, bool is_linger, size_t sec) {
    struct linger TmpLinger;
    TmpLinger.l_onoff = is_linger;
    TmpLinger.l_linger = sec;
    return Setsockopt(handle, SOL_SOCKET, SO_LINGER,(char *) &TmpLinger, sizeof(TmpLinger));
}

int  SetNonBlock(int handle, bool is_nonblock) {
    int flags = 0;
#ifdef  WIN32
    uint64_t argp = is_nonblock;
    flags = ioctlsocket(handle, FIONBIO, &argp);
#else
    flags = fcntl(handle, F_GETFL, 0);
    flags = (is_nonblock == true) ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
    flags = fcntl(handle, F_SETFL, flags);
#endif
    return flags;
}

// 设置探测分节
int SetKeepAlive(int handle, int keepIdle, int keepInterval, int keepCount) {
    int on = 1;
    int res = Setsockopt(handle, SOL_SOCKET, SO_KEEPALIVE, (void *)&on, sizeof(on));
    if(res == -1) {
        return -1;
    }
    // 探测分节时间间隔，系统默认两小时
    if(keepIdle <= 0) {
        keepIdle = 60;
    }
    // 当没收到响应时，重新发送探测数据的间隔时间，系统默认 75 s
    if(keepInterval <= 0) {
        keepInterval = 5;
    }
    // 当没收到响应时，重新发送探测数据的次，系统默认 9 次
    if(keepCount <= 0) {
        keepCount = 3;
    }
    res = Setsockopt(handle, SOL_TCP, TCP_KEEPIDLE, (void *)&keepIdle, sizeof(keepIdle));
    if(res == -1) {
        return -1;
    }
    res = Setsockopt(handle, SOL_TCP,TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
    if(res == -1) {
        return -1;
    }
    return Setsockopt(handle,SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));
}

// 设置接收端滑动窗口大小
Int4 SetRecvBufSize(int handle, UInt4 Size_u4i)
{
    if(Size_u4i <= 0) {
        Size_u4i = 200 * 1024;
    }
    UInt4 Len_n4 = sizeof(Size_u4i);
    return Setsockopt(handle, SOL_SOCKET, SO_RCVBUF, &Size_u4i, Len_n4);
}

// 获取接收端滑动窗口大小
Int4 GetRecvBufSize(int handle)
{
    UInt4 BufSize_n4;
    Int4 Len_n4= sizeof(BufSize_n4);
    if (Getsockopt(handle, SOL_SOCKET, SO_RCVBUF,&BufSize_n4, &Len_n4) < 0) {
        return 0;
    }
    return BufSize_n4;
}

// 设置发送端滑动窗口大小
Int4 SetSendBufSize(int handle, UInt4 Size_u4i)
{
    if(Size_u4i <= 0) {
        Size_u4i = 200 * 1024;
    }
    Int4 Len_n4 = sizeof(Size_u4i);
    return Setsockopt(handle, SOL_SOCKET, SO_SNDBUF, &Size_u4i, Len_n4);
}

// 获取发送端滑动窗口大小
Int4 GetSendBufSize(int handle)
{
    UInt4 BufSize_n4;
    Int4 Len_n4= sizeof(BufSize_n4);
    if (Getsockopt(handle, SOL_SOCKET, SO_SNDBUF, &BufSize_n4, &Len_n4) < 0)
    {
        return -1;
    }
    return BufSize_n4;
}

int Ioctlsocket(int handle, int request, void* arg)
{
    int ret = -1;
#ifdef  WIN32
    ret = ioctlsocket(handle, request, (u_long *) arg);
#else
    ret = ioctl(handle, request, arg);
#endif
    return ret;
}

////////////////////////////////////////////////////////////////////////////////

#ifdef  __linux__

// 读一个字节
static ssize_t my_read(int fd, char *ptr);

/*参三: 应该读取的字节数*/                          //socket 4096  readn(cfd, buf, 4096)   nleft = 4096-1500
ssize_t Readn(int fd, void *vptr, size_t n)
{
    size_t  nleft;              //usigned int 剩余未读取的字节数
    ssize_t nread;              //int 实际读到的字节数
    char   *ptr;

    ptr = (char *)vptr;
    nleft = n;                  //n 未读取字节数

    while (nleft > 0) {
        if ((nread = read(fd, ptr, nleft)) < 0) {
            if (errno == EINTR)
                nread = 0;
            else
                return -1;
        } else if (nread == 0)
            break;

        nleft -= nread;   //nleft = nleft - nread
        ptr += nread;
    }
    return n - nleft;
}

ssize_t Writen(int fd, const void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = (const char *)vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;
            else
                return -1;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}

// 读一个字节
static ssize_t my_read(int fd, char *ptr)
{
    static int read_cnt;
    static char *read_ptr;
    static char read_buf[100];

    if (read_cnt <= 0) {
again:
        if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {   //"hello\n"
            if (errno == EINTR)
                goto again;
            return -1;
        } else if (read_cnt == 0)
            return 0;

        read_ptr = read_buf;
    }
    read_cnt--;
    *ptr = *read_ptr++;

    return 1;
}

/*readline --- fgets*/
//传出参数 vptr
ssize_t Readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char    c, *ptr;
    ptr = (char *)vptr;

    for (n = 1; n < (ssize_t)maxlen; n++) {
        if ((rc = my_read(fd, &c)) == 1) {   //ptr[] = hello\n
            *ptr++ = c;
            if (c == '\n')
                break;
        } else if (rc == 0) {
            *ptr = 0;
            return n-1;
        } else
            return -1;
    }
    *ptr = 0;

    return n;
}

#endif
