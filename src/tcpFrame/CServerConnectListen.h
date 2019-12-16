#ifndef CSERVERCONNECTLISTEN_H
#define CSERVERCONNECTLISTEN_H

#include "CBaseThread.h"
#include "config.h"
#include "stdtype.h"

class CServerSocket;
class CServerRWListen;

class CServerConnectListen : public CBaseThread
{
    friend class CServerSocket;
    friend class CServerRWListen;
    friend class CServerConnectUpdate;
private:
    CServerSocket *m_psServer;
public:
    CServerConnectListen(CServerSocket *tSerSocket, bool isDetach = false);
    void execute() override;
private:

    /**
     * @brief addConnect    // 监听连接事件，如果有连接事件，则进行连接创建
     * @return              // 0 成功，其他失败
     */
    int addConnect();

    /**
     * @brief updateConnector   // 清理长时间无事件的连接
     * @param NowTime_u4i       // 当前事件，单位 毫秒
     */
    void updateConnector(UInt4 NowTime_u4i);

private:
    int m_epd; // epoll 反应堆句柄

    const int EPOLL_SIZE = 10;
};

#endif // CServerCONNECTLISTEN_H
