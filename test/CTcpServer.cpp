#include "CTcpServer.h"

#include "CTcpManager.h"
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
    CTcpManager::GetInstance()->remove(CServerConnector_ti->handle());
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

//    CTcpManager::GetInstance()->add(dispose.getParkId(), CServerConnector_ti);
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

void CTcpServer::start()
{
    // 设置 epoll 线程数
    server.open(2);
}

void CTcpServer::stop()
{
    // 关闭 tcp 服务
    server.clear();
}
