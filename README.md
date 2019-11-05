![](./res/tcpServer.png)

**tcpServer 是用c++编写的 tcp服务框架， API调用 异常简洁**

# Features
* 使用c++11新特性，支持可变模块参数传参，传参简洁
* 目前支持 SQL 预处理执行和直接执行两种操作
* 最底层使用数据库连接池进行封装，上层从连接池中取数据库连接句柄，实现数据库连接复用，提高效率
* 获取数据库连接句柄前都会对连接是否正常进行判断，提高框架稳定性
* 使用 CDBQuery 操作类前，必须先设置 CDBPool 连接池类的账号密码，并创建连接，首次创建的连接池作为默认连接池使用
* CDBPool 连接池在创建前可以设置最大连接数和最小连接数，默认 3~10 个连接，然后由底层动态调整连接数实际数量，不足就创建，过多就删除释放
* 当前实现了数据量操作的五种赋值方式，下面 sqlSelect() 用例有五种赋值方式的示范说明
* 支持Linux和windows（目前所有文件都是跨平台的，后续添加windows编译文件）

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

static
void connectNotify(CServerConnector *CServerConnector_ti)
{
    if (!CServerConnector_ti)
        return;
    cout << "ip " <<  CServerConnector_ti->host() << endl;
}

static
void closeNotify(CServerConnector *CServerConnector_ti)
{
    cout << "closeNotify " <<  CServerConnector_ti->host() << endl;
}

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
        CServerConnector_ti->sendBuf(buf, readlen);
    }
}

CTcpServer::CTcpServer(int port)
{
    server.setPort(port);
    server.setTimeout(600 * 1000);
    server.setNotify(connectNotify, closeNotify, readNotify);
    server.setThreadPoolRange(5, 15, 5000);
}

void CTcpServer::start()
{
    server.open(2);
}

void CTcpServer::stop()
{
    server.clear();
}


# main.cpp
#include <iostream>

#include "CTcpServer.h"

int main()
{
    CTcpServer tcpServ(8185);
    tcpServ.start();
    while (1) {
        sleep(1000);
    }
    return 0;
}
```
