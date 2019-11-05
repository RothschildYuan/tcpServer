#ifndef CSERVERCONNECTOR_H
#define CSERVERCONNECTOR_H

#include "stdtype.h"
#include "CSocket.h"

class CServerSocket;

class CServerConnector : public CSocket
{
    friend class CServerSocket;
    friend class CServerRWListen;
public:
    CServerConnector();
    CServerConnector(CServerSocket *Parent_ti);
    ~CServerConnector();

    /**
     * @brief sendBuf       发送数据
     * @param str           字符串内容
     * @param len           长度
     * @return              >= 0 成功，其他失败
     */
    int sendBuf(const char* str, size_t len);

    /**
     * @brief sendBuf       发送数据
     * @param str           字符串
     * @return              >= 0 成功，其他失败
     */
    int sendBuf(const string& str);

    /**
     * @brief getSockDatas
     * @return              返回不会失败
     */
    vector<shared_ptr<char>> getSockDatas();

protected:

    /**
     * @brief clearCache    清理系统中读取 socket 数据后的缓存
     */
    void clearCache();

    /**
     * @brief readSocket    读取 socket 中的数据
     * @return              <= 0 失败，> 0 返回 读取到的数据总条数
     */
    int readSocket();

    /**
     * @brief readBuf       读取数据
     * @param str           字符串内容
     * @param len           长度
     * @return              >= 0 成功，其他失败
     */
    int readBuf(char* str, size_t len);

    /**
     * @brief readlen       滑动窗口数据量
     * @return              >= 0 成功，其他失败
     */
    int readlen() const;

protected:

    /**
     * @brief clear         关闭客户端连接的套接字
     * @param ShutDown_b1i  close 句柄时，是否先调用 shutdown 函数
     */
    void clear(bool ShutDown_b1i);

    /**
     * @brief receiveLength 查看连接客户端 tcp 滑动窗口数据量
     * @return              >= 成功，其他失败
     */
    Int4 receiveLength();

    /**
     * @brief eventAdd      事件fd 添加到 epoll 接点上
     * @param epfd          epoll 句柄
     * @param events        事件类型
     */
    void eventAdd(int epfd, int events);

    /**
     * @brief eventDel      事件fd 从 epoll 接点删除
     * @param epfd          epoll 句柄
     */
    void eventDel(int epfd);

//    void getData<>

protected:

    CServerSocket*  m_server;
    ACTMutex        m_mutex;
    int             m_readlen;
    UInt4           m_heartTime;//发送心跳时间 //心跳间隔上限，毫秒
    vector<shared_ptr<char>> m_sockDatas;
};

#endif // CServerCONNECTOR_H
