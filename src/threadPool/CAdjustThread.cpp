#include "cAdjustThread.h"

#include "CThreadPool.h"
#include "config.h"
#include "CLogger.h"

#define DEFAULT_TIME 3                 //10s检测一次
#define MIN_WAIT_TASK_NUM 10            //如果queue_size > MIN_WAIT_TASK_NUM 添加新的线程到线程池
#define DEFAULT_THREAD_VARY 10          //每次创建和销毁线程的个数

CAdjustThread::CAdjustThread(CThreadPool* threadPool)
{
    m_pool = threadPool;
}

void CAdjustThread::execute()
{
    if(m_pool == nullptr) {
        throw std::logic_error("m_pool == nullptr");
    }
    while (!m_pool->m_isExited) {
        sleep(DEFAULT_TIME);                                    //定时 对线程池管理
        int queue_size = m_pool->getQueueSize();                      // 关注 任务数
        print2("\n\n CAdjustThread::execute size : %d\n\n", queue_size);
        int live_thr_num = m_pool->getLiveThreadnum();                  // 存活 线程数
        int busy_thr_num = m_pool->getBusyThreadnum();                  // 忙着的线程数
        int min_wait_task_num = m_pool->m_queue_max_size / 20;
        // 创建新线程 算法： 任务数大于最小线程池个数, 且存活的线程数少于最大线程个数时 如：30>=10 && 40<100
        if (queue_size >= min_wait_task_num && live_thr_num < m_pool->m_max_thr_num) {
            m_pool->m_mutex.lock();
            //一次增加 DEFAULT_THREAD 个线程
            for (int i = 0, add = 0; i < m_pool->m_max_thr_num
                 && add < DEFAULT_THREAD_VARY
                 && m_pool->getLiveThreadnum() < m_pool->m_max_thr_num; i++)
            {
                m_pool->addTaskThread();
                add++;
            }
            m_pool->m_mutex.unlock();
        }
        // 销毁多余的空闲线程 算法：忙线程X2 小于 存活的线程数 且 存活的线程数 大于 最小线程数时
        if ((busy_thr_num * 2) < live_thr_num  &&  live_thr_num > m_pool->m_min_thr_num) {
            // 一次销毁DEFAULT_THREAD个线程, 隨機10個即可 // 要销毁的线程数 设置为10
            m_pool->setwaitExitThrNum(DEFAULT_THREAD_VARY);
            for (int i = 0; i < m_pool->getwaitExitThrNum(); i++) {
                // 通知处在空闲状态的线程, 他们会自行终止
                m_pool->m_queue_not_empty.notify_one();
            }
        }
    }
}
