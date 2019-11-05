#include "CWorkThread.h"

#include "CThreadPool.h"
#include "CLogger.h"

CWorkThread::CWorkThread(CThreadPool* threadPool)
{
    m_pool = threadPool;
}

void CWorkThread::execute()
{
    CThreadPool* pool = m_pool;
    CWorkTask task;
    while (true) {
        // 刚创建出线程，等待任务队列里有任务，否则阻塞等待任务队列里有任务后再唤醒接收任务
        pool->m_mutex.lock();
        // queue_size == 0 说明没有任务，调 wait 阻塞在条件变量上, 若有任务，跳过该while
        while (pool->getQueueSize() == 0 && !pool->m_isExited) {
            print2("thread 0x%x is waiting\n", pthread_self());
            pool->m_queue_not_empty.wait(&pool->m_mutex);
            // 清除指定数目的空闲线程，如果要结束的线程个数大于0，结束线程
            if (pool->m_wait_exit_thr_num > 0) {
                pool->m_wait_exit_thr_num--;
                // 如果线程池里线程个数大于最小值时可以结束当前线程
                if (pool->getLiveThreadnum() > pool->m_min_thr_num) {
                    print2("thread 0x%x is exiting\n", pthread_self());
                    pool->m_live_thr_num--;
                    pool->m_mutex.unlock();
                    pthread_exit(nullptr);
                }
            }
        }
        // 如果指定了true，要关闭线程池里的每个线程，自行退出处理
        if (pool->m_isExited) {
            pool->m_mutex.unlock();
            print2("thread 0x%x is exiting\n", pthread_self());
            // 线程自行结束
            pthread_exit(nullptr);
        }
        //从任务队列里获取任务, 是一个出队操作
        task = pool->m_task_queue[pool->m_queue_front];
        pool->m_task_queue[pool->m_queue_front].reset();
        // 出队，模拟环形队列
        pool->queueFrontInc();
        print2("\n CWorkThread::execute m_queue_size : %d\n", pool->getQueueSize());
        //通知可以有新的任务添加进来
        pool->m_queue_not_full.notify_all();
        //任务取出后，立即将 线程池琐 释放
        pool->m_mutex.unlock();
        //执行任务
        print2("thread 0x%x start working\n", pthread_self());
        //忙状态线程数变量琐, 忙状态线程数+1
        pool->busyThreadnumInc();
        //执行回调函数任务
        task.exec();
        //任务结束处理
        print2("thread 0x%x end working\n", pthread_self());
        //处理掉一个任务，忙状态数线程数-1
        pool->busyThreadnumDec();
    }
    pthread_exit(nullptr);
}
