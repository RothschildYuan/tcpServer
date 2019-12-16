#ifndef CSERVERRWLISTEN_H
#define CSERVERRWLISTEN_H

#include "CBaseThread.h"
#include "config.h"
#include "stdtype.h"

class CServerSocket;
class CServerConnector;
class CThreadPool;
class CServerRWListen : public CBaseThread
{
    friend class CServerSocket;
    friend class CServerConnectListen;
private:
    CServerSocket *m_psServer;
public:
    CServerRWListen(CServerSocket *tSerSocket, bool isDetach = false);
    void execute() override;
private:
    /**
     * @brief eventSet      把新创建的连接添加的 epoll 反应堆中监听
     * @param cConn         新创建的连接对象
     * @return              0 成功，其他失败
     */
    int eventSet(CServerConnector *cConn);

    /**
     * @brief eventDel      从 epoll 反应堆中删除指定的连接
     * @param cConn         新创建的连接对象
     * @return              0 成功，其他失败
     */
    int eventDel(CServerConnector* cConn);

    /**
     * @brief addConnect    新创建的连接
     * @param handle        tcp 服务端监听句柄
     * @return              0 成功，其他失败
     */
    int addConnect(int handle);

    /**
     * @brief updateConnector   处理超时连接
     * @param NowTime_u4i       设置当前事件撮，单位 毫秒
     */
    void updateConnector(UInt4 NowTime_u4i);

    /**
     * @brief processHandle
     * @return
     */
    int processHandle();

    /**
     * @brief getConnCount  返回 epoll 监听的连接数
     * @return              < 0 失败，其他成功
     */
    int getConnCount();
private:
    int                     m_epd;      // epoll 返回堆句柄
    atomic_int              m_nCount;   // epoll 返回堆监听的连接数
    list<CServerConnector*> m_listConns;// tcp 连接队列，连接可循环使用，不用经常 delete，new 降低资源消耗
    ACTMutex                m_connMutex;

    const int EPOLL_SIZE = 1024*10;
};

#endif // CServerRWLISTEN_H
