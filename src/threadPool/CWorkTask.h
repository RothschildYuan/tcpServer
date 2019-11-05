#ifndef CWORKTASK_H
#define CWORKTASK_H

typedef void (*CallBackFunc)(void *);

// 各子线程任务结构体
class CWorkTask
{
public:
    CWorkTask();
    CWorkTask(CallBackFunc func, void* arg);
    CWorkTask(const CWorkTask& oth);
    CWorkTask& operator=(const CWorkTask& oth);

    /**
     * @brief setFunc   设置工作任务内容
     * @param func      回调函数
     * @param arg       回调操作参数
     */
    void setFunc(CallBackFunc func, void* arg);

    /**
     * @brief reset     清空回调函数和参数内容
     */
    void reset();

    /**
     * @brief exec      执行回调操作
     */
    void exec();

private:
    CallBackFunc    m_func;       // 函数指针，回调函数
    void*           m_arg;            // 上面函数的参数
};

//template<typename CallBackFunc,typename _Type>
//class CWorkTask
//{
//public:
//    CWorkTask() {
//        m_func  = nullptr;
//        m_arg   = nullptr;
//    }
//    CWorkTask(CallBackFunc func, _Type* arg) {
//        setFunc(func, arg);
//    }
//    CWorkTask(const CWorkTask& oth) {
//        m_func  = oth.m_func;
//        m_arg   = oth.m_arg;
//    }
//    CWorkTask& operator=(const CWorkTask& oth) {
//        m_func  = oth.m_func;
//        m_arg   = oth.m_arg;
//        return *this;
//    }
//    void setFunc(CallBackFunc func, _Type* arg) {
//        m_func  = func;
//        m_arg   = arg;
//    }
//    void reset() {
//        m_func  = nullptr;
//        m_arg   = nullptr;
//    }
//    void exec() {
//        if(m_func == nullptr || m_arg == nullptr ) {
//            return;
//        }
//        m_func(m_arg);
//    }

//private:
//    CallBackFunc    m_func;       // 函数指针，回调函数
//    _Type*          m_arg;        // 上面函数的参数
//};

#endif // CWorkTask_H
