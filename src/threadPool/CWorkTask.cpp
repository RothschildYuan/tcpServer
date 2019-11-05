#include "CWorkTask.h"

CWorkTask::CWorkTask() {
    m_func  = nullptr;
    m_arg   = nullptr;
}

CWorkTask::CWorkTask(CallBackFunc func, void *arg) {
    setFunc(func, arg);
}

CWorkTask::CWorkTask(const CWorkTask &oth) {
    m_func  = oth.m_func;
    m_arg   = oth.m_arg;
}

CWorkTask &CWorkTask::operator=(const CWorkTask &oth) {
    m_func  = oth.m_func;
    m_arg   = oth.m_arg;
    return *this;
}

void CWorkTask::setFunc(CallBackFunc func, void *arg) {
    m_func  = func;
    m_arg   = arg;
}

void CWorkTask::reset() {
    m_func  = nullptr;
    m_arg   = nullptr;
}

void CWorkTask::exec() {
    if(m_func == nullptr || m_arg == nullptr ) {
        return;
    }
    m_func(m_arg);
}
