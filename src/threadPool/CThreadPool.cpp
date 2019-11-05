#include "CThreadPool.h"

#include "config.h"
#include "CAdjustThread.h"
#include "CWorkThread.h"
#include "CLogger.h"

CThreadPool::CThreadPool(Int4 min_thr_num, Int4 max_thr_num, Int4 queue_max_size)
{
    m_npriority         = 3;

    m_min_thr_num       = min_thr_num;
    m_max_thr_num       = max_thr_num;
    m_busy_thr_num      = 0;
    m_live_thr_num      = min_thr_num;               // 活着的线程数 初值=最小线程数
    m_wait_exit_thr_num = 0;
    m_queue_size        = 0;                         // 有0个产品
    m_queue_max_size    = queue_max_size;
    m_queue_front       = 0;
    m_queue_rear        = 0;
    m_isExited          = false;                     // 不关闭线程池

    // 队列开辟空间
    m_task_queue = nullptr;
    m_adjustThread  = nullptr;
    m_taskThreads.reserve(max_thr_num);
}

CThreadPool::~CThreadPool() {
    m_isExited = true;
    for (Int4 i = 0; i < m_live_thr_num; i++) {
        // 通知所有的空闲线程
        m_queue_not_empty.notify_all();
    }
    if (m_task_queue) {
        SDeletes(m_task_queue);
    }
    // 先销毁管理线程
    SDelete(m_adjustThread);
    for(auto & it : m_taskThreads) {
        SDelete(it);
    }
    m_taskThreads.clear();

    SDeletes(m_task_queue);
}

void CThreadPool::execute()
{
    // 队列开辟空间
    m_task_queue = new CWorkTask[m_queue_max_size];
    if (m_task_queue == nullptr) {
        throw std::bad_alloc();
    }

    // 启动 max_thr_num 个 work thread // 根据最大线程上限数， 给工作线程数组开辟空间, 并清零
    for (Int4 i = 0; i < m_min_thr_num; i++) {
        CWorkThread* thread = new CWorkThread(this);
        thread->start();
        m_taskThreads.push_back(thread);
        stringstream ss;
        ss << "start work thread " << std::hex <<thread->getThread()<<endl;
        print1(ss.str().c_str());
    }

    // 启动管理者线程
    m_adjustThread = new CAdjustThread(this);
    m_adjustThread->start();
    stringstream ss;
    ss << "start adjust thread " << std::hex <<m_adjustThread->getThread()<<endl;
    print1(ss.str().c_str());
}

void CThreadPool::setPrioCount(UInt2 npriority) {
    m_npriority = npriority;
}

Int4 CThreadPool::addTask(CallBackFunc function, void *arg)
{
    if(m_task_queue == nullptr) {
        throw std::logic_error("CThreadPool has not execute()");
    }

    if(function == nullptr || arg == nullptr) {
        return -1;
    }

    m_mutex.lock();
    // ==为真，队列已经满， 调wait阻塞
    while (m_queue_size == m_queue_max_size && !m_isExited)
    {
        m_queue_not_full.wait(&m_mutex);
    }
    if (m_isExited) {
        m_mutex.unlock();
        return 0;
    }
    // 清空 工作线程 调用的回调函数 的参数arg
    //添加任务到任务队列里
    m_task_queue[m_queue_rear].setFunc(function, arg);
    // 队尾指针移动, 模拟环形
    queueRearInc();
    print2("m_queue_size : \n", getQueueSize());
    //添加完任务后，队列不为空，唤醒线程池中 等待处理任务的线程
    m_queue_not_empty.notify_one();
    m_mutex.unlock();
    return 0;
}

Int4 CThreadPool::getBusyThreadnum()
{
    return m_busy_thr_num.load();
}

void CThreadPool::busyThreadnumInc()
{
    ++m_busy_thr_num;
}

void CThreadPool::busyThreadnumDec()
{
    --m_busy_thr_num;
}

void CThreadPool::queueFrontInc()
{
    m_queue_front = (m_queue_front + 1) % m_queue_max_size;
    m_queue_size--;
}

void CThreadPool::queueRearInc()
{
    m_queue_rear = (m_queue_rear + 1) % m_queue_max_size;
    m_queue_size++;
}

Int4 CThreadPool::getLiveThreadnum() {
    return m_live_thr_num.load();
}

Int4 CThreadPool::getQueueSize() {
    return m_queue_size.load();
}

Int4 CThreadPool::getwaitExitThrNum() const
{
    return m_wait_exit_thr_num.load();
}

void CThreadPool::setwaitExitThrNum(Int4 wait_exit_thr_num)
{
    m_wait_exit_thr_num.store(wait_exit_thr_num);
}

bool CThreadPool::isThreadAlive(pthread_t tid)
{
    Int4 kill_rc = pthread_kill(tid, 0);     //发0号信号，测试线程是否存活
    if (kill_rc == ESRCH) {
        return false;
    }
    return true;
}

void CThreadPool::addTaskThread() {
    CWorkThread* task = new CWorkThread(this);
    task->start();
    m_taskThreads.push_back(task);
    m_live_thr_num++;
}

#define NUM -1

#if NUM == 0
#define TASKSIZE 10000
// 测试
//  线程池中的线程，模拟处理业务
void *process(void *arg)
{
    int* param = (int*)arg;
    print2("thread 0x%x working on task \n\n", pthread_self());
    Sleep(1000);
    return nullptr;
}

int main() {
    // 创建线程池，池里最小3个线程，最大100，队列最大100
    CThreadPool pool(3, 23,TASKSIZE * 3);
    pool.execute();
    sleep(1);
    print2("pool inited\n";
    int *num = (int *)calloc(TASKSIZE, sizeof(int));
    int i;
    for (i = 0; i < TASKSIZE; i++) {
        num[i]=i+1;
        print2("add task %d\n", i);
        pool.addTask(process, &num[i]);     /* 向线程池中添加任务 */
    }
    sleep(30);                                     /* 等子线程完成任务 */
    for (i = 0; i < TASKSIZE; i++) {
        num[i]=i+1;
        print2("add task %d\n", i);
        pool.addTask(process, &num[i]);     /* 向线程池中添加任务 */
    }
    sleep(30);
    for (i = 0; i < TASKSIZE; i++) {
        num[i]=i+1;
        print2("add task  %d\n", i);
        pool.addTask(process, &num[i]);     /* 向线程池中添加任务 */
    }
    while(1) {
        sleep(1000);
    }
    return 0;
}

#elif NUM == 2

// 虚函数表原理
class ClassA
{
public:
    ClassA() {}
    virtual void a() { printf("ClassA::a()\n"); }
    virtual void b() { printf("ClassA::b()\n"); }
    virtual void c() { printf("ClassA::c()\n"); }
};

class ClassB : public ClassA
{
public:
    ClassB() {}
    virtual void a() { printf("ClassB::a()\n"); }
    virtual void b() { printf("ClassB::b()\n"); }
    virtual void c() { printf("ClassB::c()\n"); }
};

class ClassC : public ClassA
{
public:
    ClassC() {}
    virtual void a1() { printf("ClassC::a()\n"); }
    virtual void b1() { printf("ClassC::b()\n"); }
    virtual void c1() { printf("ClassC::c()\n"); }
};

typedef void (*FUNC)();

int main() {

    ClassA* a = new ClassA();
    ClassA* b = new ClassB();
    ClassA* c = new ClassC();
    FUNC fun;
    printf("addr: %p\n", a);
    printf("addr: %x\n", *(int*)a);
    int addr = *(int*)a;
    printf("addr: %x\n", addr);
//    fun = (FUNC)(long*)(*((long*)*(long*)a));
//    fun();

//    long* len = (long*)*(long*)a;
//    long add[3] = {0};
//    for(int i = 0; i<3; ++i) {
//        add[i] = len[i];
//        printf("add[i]: %lx\n",add[i]);
//    }
//    fun = (FUNC)(long*)(add[0]);
//    fun();
//    fun = (FUNC)(long*)(add[1]);
//    fun();
//    fun = (FUNC)(long*)(add[2]);
//    fun();

//    fun = (FUNC)(long*)*((long*)*(long*)a+0);
//    fun();
//    fun = (FUNC)(long*)*((long*)*(long*)a+1);
//    fun();
//    fun = (FUNC)(long*)*((long*)*(long*)a+2);
//    fun();
//    printf("size: %lu\n",sizeof(long*));

//    fun = (FUNC)(long*)*((long*)*(long*)b+0);
//    fun();
//    fun = (FUNC)(long*)*((long*)*(long*)b+1);
//    fun();
//    fun = (FUNC)(long*)*((long*)*(long*)b+2);
//    fun();

//    fun = (FUNC)(long*)*((long*)*(long*)c+0);
//    fun();
//    fun = (FUNC)(long*)*((long*)*(long*)c+1);
//    fun();
//    fun = (FUNC)(long*)*((long*)*(long*)c+2);
//    fun();
//    fun = (FUNC)(long*)*((long*)*(long*)c+3);
//    fun();
//    fun = (FUNC)(long*)*((long*)*(long*)c+4);
//    fun();
//    fun = (FUNC)(long*)*((long*)*(long*)c+5);
//    fun();

    ClassA* a1 = new ClassA();
    ClassA* a2 = new ClassA();
    ClassA* a3 = new ClassA();
    ClassA* a4 = new ClassA();
    ClassA* a5 = new ClassA();
    printf("a1: %ld\n",*(long*)a1);
    printf("a2: %ld\n",*(long*)a2);
    printf("a3: %ld\n",*(long*)a3);
    printf("a4: %ld\n",*(long*)a4);
    printf("a5: %ld\n",*(long*)a5);

    return 0;
}

#endif
