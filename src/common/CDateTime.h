#ifndef CDATETIME_H
#define CDATETIME_H

#include <sys/time.h>
#include <chrono>

#include "config.h"
#include "stdtype.h"

class CDateTime
{
public:
    CDateTime();
    CDateTime(const CDateTime& other);
    CDateTime& operator=(const CDateTime& other);
    CDateTime(const string& time);
    CDateTime(const time_t& t);
    bool operator < (const CDateTime& other) const;
    bool operator > (const CDateTime& other) const;
    bool operator <= (const CDateTime& other) const;
    bool operator >= (const CDateTime& other) const;
    bool operator == (const CDateTime& other) const;
    long int operator- (const CDateTime& start) const;

    long int secsTo(const CDateTime& dataTime);
    long int minuteTo(const CDateTime& dataTime);
    long int hourTo(const CDateTime& dataTime);
    long int dayTo(const CDateTime& dataTime);
    CDateTime& addSecs(long int sec);
    CDateTime& addMinute(long int min);
    CDateTime& addHour(long int min);
    CDateTime& addDay(long int day);
    int whichWeekday();
    string toString();
    static string getCurrenTime();

public:
    int m_year;
    int m_month;
    int m_day;
    int m_hour;
    int m_minute;
    int m_second;

    time_t m_nowtime;
    struct tm* m_timeinfo;
};

// 微秒级
class CMicroTime
{
public:
    CMicroTime();

    long microsec();
    long sec();
    int minuteswest();
    int dsttime();

private:
    struct timeval m_tv;
    struct timezone m_tz;
};

// 返回微秒
UInt4 GettickCount();
// 返回秒
Int4 GetMillSec();
// 时间秒值
Int4 GetMillSeconds();

// 纳秒级
class CNanoTime
{
public:
    // CLOCK_REALTIME 系统当前时间，从1970年1.1日算起
    // CLOCK_MONOTONIC 系统的启动时间，不能被设置
    // CLOCK_PROCESS_CPUTIME_ID 本进程运行时间
    // CLOCK_THREAD_CPUTIME_ID 本线程运行时间
    CNanoTime() {
        clock_gettime(CLOCK_REALTIME, &ts);
    }
    long nanosec() {
        return ts.tv_nsec;
    }
    long sec() {
        return ts.tv_sec;
    }
    // 获取系统当前时间毫秒数
    static long int currentMillisec() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
private:
    struct timespec ts;
};

#endif // CDateTime_H
