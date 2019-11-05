![](./res/tcpServer.png)

**tcpServer 是用c++编写的 tcp服务框架， API调用 异常简洁**

# Features
* 使用c++11新特性，Tcp 框架目前仅支持 linux 版
* 基于 epoll 线程池实现，设置有任务队列任务线程池（默认不开启，需要人工开启设置）

**WARNING: **
> 目前 tcp 框架仅支持 linux 版，使用 epoll 实现

# Building
```
make
```

# Test
测试前需要修改 test/main.cpp 中的数据库账号密码
```
cd test  
make  
./TcpTest
```

# Usage
使用libevent-cpp能够非常方便的创建一个http服务器，比如如下例子创建一个简单的静态http文件服务器
```c++
# CTcpServer.h
#include "CServerSocket.h"

class CTcpServer
{
public:
    CTcpServer(int port = 10002);
    void start();
    void stop();
private:
    CServerSocket server;
};


# CTcpServer.cpp
#include "CTcpServer.h"

#include "CServerConnector.h"

// socket 连接创建回调函数
static
void connectNotify(CServerConnector *CServerConnector_ti)
{
    if (!CServerConnector_ti)
        return;
    cout << "ip " <<  CServerConnector_ti->host() << endl;
}

// socket 连接关闭销毁回调函数
static
void closeNotify(CServerConnector *CServerConnector_ti)
{
    cout << "closeNotify " <<  CServerConnector_ti->host() << endl;
}

// socket 读业务操作回调函数
static
void readNotify(void *arg)
{
    cout << "readNotify" << endl;
    CServerConnector *CServerConnector_ti = (CServerConnector *)arg;
    vector<shared_ptr<char> > sockdata = CServerConnector_ti->getSockDatas();

    Sleep(10);

    for(shared_ptr<char> it : sockdata) {
        char* buf = it.get();
        size_t readlen = strlen(buf);
        
        /*
         * 实现业务逻辑
         */
        
        cout << " len: " <<readlen<< "\nrecv: **"<<buf<<"**\n\n";
		// 响应 client 数据
        CServerConnector_ti->sendBuf(buf, readlen);
    }
}

CTcpServer::CTcpServer(int port)
{
	// 设置端口号
    server.setPort(port);
	// 设置 tcp 超时断开连接
    server.setTimeout(600 * 1000);
	// 设置连接操作回调函数，断开连接操作回调函数，读事件操作回调函数，
    server.setNotify(connectNotify, closeNotify, readNotify);
	// 设置任务处理线程池 5~15 个线程数范围，任务队列里最多容量 5000 个任务
    server.setThreadPoolRange(5, 15, 5000);
}


# main.cpp
#include <iostream>

#include "CTcpServer.h"

int main()
{
	// 申明 TCP 服务对象，然后开启服务
    CTcpServer tcpServ(8185);
    tcpServ.start();
    while (1) {
        sleep(1000);
    }
    return 0;
}
```
