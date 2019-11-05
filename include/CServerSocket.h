#ifndef __CSERVERSOCKET_TCP__
#define __CSERVERSOCKET_TCP__

#include "stdtype.h"
#include "CSocket.h"

class CThreadPool;
class CServerConnector;
class CServerConnectUpdate;
class CServerConnectListen;
class CServerRWListen;

typedef void (*NotifyEventCB)(CServerConnector *conn);
typedef void (*NotifyEventReadCB)(void *);

class CServerSocket : public CSocket
{
    friend class CServerConnectUpdate;
    friend class CServerConnector;
    friend class CServerConnectListen;
    friend class CServerRWListen;
public:
    CServerSocket();
    ~CServerSocket();

    /**
     * @brief open          开启 tcp 框架服务
     * @param nthreadCount  开启 epoll 线程数
     * @return              true 成功，其他失败
     */
    bool open(UInt4 nthreadCount = 1);

    /**
     * @brief clear         关闭 tcp 服务
     */
    void clear();

    /**
     * @brief setNotify     设置 tcp 操作回调函数
     * @param Connect_oi    连接操作事件回调
     * @param Disconnect_oi 断开连接操作事件回调
     * @param Read_oi       读操作事件回调
     */
    void setNotify(NotifyEventCB Connect_oi,
                  NotifyEventCB Disconnect_oi,
                  NotifyEventReadCB Read_oi);

    /**
     * @brief setThreadPool 设置处理任务队列的线程池，不设置，默认就不使用线程池任务队列
     * @param thrPool       传入线程池对象
     */
    void setThreadPool(CThreadPool* thrPool);

    /**
     * @brief setThreadPoolRange    设置处理任务队列的线程池线程数范围，不设置，默认就不使用线程池任务队列
     * @param min_thr_num           线程最小数
     * @param max_thr_num           线程最大数
     * @param queue_max_size        任务队列容量
     */
    void setThreadPoolRange(Int4 min_thr_num,
                            Int4 max_thr_num,
                            Int4 queue_max_size = 5000);

    /**
     * @brief setTimeout    设置超时时长 ,连接长时间无事件，则删除
     * @param timeout       毫秒
     */
    void setTimeout(const UInt4 &timeout);

private:

    /**
     * @brief eventAdd  添加事件到 epoll 监听节点进行事件监听
     * @param epfd      epoll 节点
     * @param events    事件类型
     */
    void eventAdd(int epfd, int events);

private:
    //回调
    NotifyEventCB       m_connEventCB;          // 连接操作事件回调
    NotifyEventCB       m_disconnEventCB;       // 断开连接操作事件回调
    NotifyEventReadCB   m_readEventCB;          // 读操作事件回调

    UInt2                       m_connCount;    // 用于启动时，线程间同步启动处理
    UInt4                       m_timeout;      // 超时 毫秒,连接长时间无事件，则删除
    CServerConnectUpdate*       m_psUpdate;     // 判断超时连接，如果连接长时间无操作，则断开，关闭，删除该连接
    CServerConnectListen*       m_psConnListion;// 连接监听线程（仅监听，并创建 tcp 连接）
    vector<CServerRWListen*>    m_psRWListion;  // epoll 反应堆线程
    CThreadPool*                m_thrPool;      // 线程池，默认不启动
};

#endif
