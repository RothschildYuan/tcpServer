#ifndef CTHREADPOOL_H
#define CTHREADPOOL_H

#include "stdtype.h"
#include "CBaseThread.h"
#include "CWorkTask.h"
#include <CMutex.h>
#include <CCond.h>

class CAdjustThread;
class CWorkThread;
class CThreadPool : public CBaseThread
{
    friend class CAdjustThread;
    friend class CWorkThread;

    CThreadPool(const CThreadPool&);
    CThreadPool& operator=(const CThreadPool&);
public:
    CThreadPool(Int4 min_thr_num, Int4 max_thr_num, Int4 queue_max_size = 5000);
    virtual ~CThreadPool();
    virtual void execute();

    /**
     * @brief setPrioCount      设置优先级等级数
     * @param npriority         优先级等级数
     */
    void setPrioCount(UInt2 npriority);

    /**
     * @brief addTask           向线程池中 添加一个任务
     * @param function          回调函数
     * @param arg               回调操作参数
     * @return                  0 成功，其他失败
     */
    Int4 addTask(CallBackFunc function, void *arg);

    /**
     * @brief getBusyThreadnum  忙时线程数
     * @return                  0 成功，其他失败
     */
    Int4 getBusyThreadnum();

    /**
     * @brief busyThreadnumInc  忙时线程数 +1
     */
    void busyThreadnumInc();

    /**
     * @brief busyThreadnumDec  忙时线程数 -1
     */
    void busyThreadnumDec();

    /**
     * @brief queueFrontInc     任务队列头下标 index +1
     */
    void queueFrontInc();

    /**
     * @brief queueRearInc      任务队列尾下标 index +1
     */
    void queueRearInc();

    /**
     * @brief getLiveThreadnum  获取存活线程数
     * @return
     */
    Int4 getLiveThreadnum();

    /**
     * @brief getQueueSize      获取任务队列的任务数
     * @return
     */
    Int4 getQueueSize();

    /**
     * @brief getwaitExitThrNum 获取等待退出线程数（每次退出线程数量）
     * @return                  >=0 成功，其他失败
     */
    Int4 getwaitExitThrNum() const;

    /**
     * @brief setwaitExitThrNum 设置等待退出线程数（每次退出线程数量）
     * @param wait_exit_thr_num 每次等待退出线程数量
     */
    void setwaitExitThrNum(Int4 wait_exit_thr_num);

    /**
     * @brief isThreadAlive     判断线程是否存活
     * @param tid               线程编号
     * @return                  true 成功，其他失败
     */
    bool isThreadAlive(pthread_t tid);

    /**
     * @brief addTaskThread     创建并添加任务线程
     */
    void addTaskThread();

private:
    CAdjustThread*       m_adjustThread;        // 管理线程
    vector<CWorkThread*> m_taskThreads;         // 线程池中各个工作线程

    // 描述线程池相关信息
    ACT::CMutex         m_mutex;                // 用于锁住本结构体
    ACT::CCond          m_queue_not_full;       // 当任务队列满时，添加任务的线程阻塞，等待此条件变量
    ACT::CCond          m_queue_not_empty;      // 任务队列里不为空时，通知等待任务的线程

    Int4                m_min_thr_num;          // 线程池最小线程数
    Int4                m_max_thr_num;          // 线程池最大线程数
    atomic_int          m_live_thr_num;         // 当前存活线程个数
    atomic_int          m_busy_thr_num;         // 忙状态线程个数
    atomic_int          m_wait_exit_thr_num;    // 要销毁的线程个数

    CWorkTask*          m_task_queue;           // 任务队列
    Int4                m_queue_front;          // task_queue队头下标
    Int4                m_queue_rear;           // task_queue队尾下标
    atomic_int          m_queue_size;           // task_queue队中实际任务数
    Int4                m_queue_max_size;       // task_queue队列可容纳任务数上限

    UInt2               m_npriority;
};

#endif // THREADPOOL_H
