#include "CDateTime.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <sstream>
#include <iostream>
#include <vector>


CDateTime::CDateTime() {
    time( &m_nowtime );
    m_timeinfo = localtime( &m_nowtime );

    this->m_year       = m_timeinfo->tm_year + 1900;
    this->m_month      = m_timeinfo->tm_mon + 1;
    this->m_day        = m_timeinfo->tm_mday;
    this->m_hour       = m_timeinfo->tm_hour;
    this->m_minute     = m_timeinfo->tm_min;
    this->m_second     = m_timeinfo->tm_sec;
}

CDateTime::CDateTime(const CDateTime& other) {
    this->m_year       = other.m_year;
    this->m_month      = other.m_month;
    this->m_day        = other.m_day;
    this->m_hour       = other.m_hour;
    this->m_minute     = other.m_minute;
    this->m_second     = other.m_second;
    this->m_nowtime    = other.m_nowtime;
}

CDateTime& CDateTime::operator=(const CDateTime& other) {
    this->m_year       = other.m_year;
    this->m_month      = other.m_month;
    this->m_day        = other.m_day;
    this->m_hour       = other.m_hour;
    this->m_minute     = other.m_minute;
    this->m_second     = other.m_second;
    this->m_nowtime    = other.m_nowtime;
    return *this;
}

CDateTime::CDateTime(const string& time) {
    struct tm pt;
    if(time.length() == 19) {
        char buf[5] = {0};
        strncpy(buf, time.c_str(), 4);
        pt.tm_year = atoi(buf) - 1900;

        bzero(buf, sizeof(buf));
        strncpy(buf, time.c_str()+5, 2);
        pt.tm_mon = atoi(buf) - 1;

        bzero(buf, sizeof(buf));
        strncpy(buf, time.c_str()+8, 2);
        pt.tm_mday = atoi(buf);

        bzero(buf, sizeof(buf));
        strncpy(buf, time.c_str()+11, 2);
        pt.tm_hour = atoi(buf);

        bzero(buf, sizeof(buf));
        strncpy(buf, time.c_str()+14, 2);
        pt.tm_min = atoi(buf);

        bzero(buf, sizeof(buf));
        strncpy(buf, time.c_str()+17, 2);
        pt.tm_sec = atoi(buf);
    }
    m_nowtime = mktime(&pt);
}

CDateTime::CDateTime(const time_t &t)
{
    m_nowtime = t;
}

bool CDateTime::operator < (const CDateTime& other) const
{
    if(m_year < other.m_year) {
        return true;
    }
    if(m_month < other.m_month) {
        return true;
    }
    if(m_day < other.m_day) {
        return true;
    }
    if(m_hour < other.m_hour) {
        return true;
    }
    if(m_minute < other.m_minute) {
        return true;
    }
    if(m_second < other.m_second) {
        return true;
    }
    return false;
}

bool CDateTime::operator > (const CDateTime& other) const
{
    if(m_year > other.m_year) {
        return true;
    }
    if(m_month > other.m_month) {
        return true;
    }
    if(m_day > other.m_day) {
        return true;
    }
    if(m_hour > other.m_hour) {
        return true;
    }
    if(m_minute > other.m_minute) {
        return true;
    }
    if(m_second > other.m_second) {
        return true;
    }
    return false;
}

bool CDateTime::operator <= (const CDateTime& other) const
{
    return !operator > (other);
}

bool CDateTime::operator >= (const CDateTime& other) const
{
    return operator < (other);
}

bool CDateTime::operator == (const CDateTime& other) const
{
    if(m_year != other.m_year) {
        return false;
    }
    if(m_month != other.m_month) {
        return false;
    }
    if(m_day != other.m_day) {
        return false;
    }
    if(m_hour != other.m_hour) {
        return false;
    }
    if(m_minute != other.m_minute) {
        return false;
    }
    if(m_second != other.m_second) {
        return false;
    }
    return true;
}

long int CDateTime::operator- (const CDateTime& start) const
{
    return m_nowtime - start.m_nowtime;
}

long int CDateTime::secsTo(const CDateTime& dataTime)
{
    return m_nowtime - dataTime.m_nowtime;
}

long int CDateTime::minuteTo(const CDateTime& dataTime)
{
    return secsTo(dataTime) / 60;
}

long int CDateTime::hourTo(const CDateTime& dataTime)
{
    return minuteTo(dataTime)  / 60;
}

long int CDateTime::dayTo(const CDateTime& dataTime)
{
    return hourTo(dataTime)  / 24;
}

CDateTime& CDateTime::addSecs(long int sec)
{
    m_nowtime += sec;
    m_timeinfo = localtime( &m_nowtime );

    this->m_year       = m_timeinfo->tm_year + 1900;
    this->m_month      = m_timeinfo->tm_mon + 1;
    this->m_day        = m_timeinfo->tm_mday;
    this->m_hour       = m_timeinfo->tm_hour;
    this->m_minute     = m_timeinfo->tm_min;
    this->m_second     = m_timeinfo->tm_sec;
    return *this;
}

CDateTime& CDateTime::addMinute(long int min)
{
    return addSecs(min * 60);
}

CDateTime& CDateTime::addHour(long int min)
{
    return addMinute(min * 60);
}

CDateTime& CDateTime::addDay(long int day)
{
    return addHour(day * 24);
}

int CDateTime::whichWeekday() {
    return m_timeinfo->tm_wday;
}

string CDateTime::toString() {
    char buf[64] = {0};
    m_timeinfo = localtime(&m_nowtime);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", m_timeinfo);
    return buf;
}

string CDateTime::getCurrenTime(){
    CDateTime curtime;
    return curtime.toString();
}

////////////////////////////////////////////////////////////////////////////////////////////

CMicroTime::CMicroTime() {
    gettimeofday(&m_tv, &m_tz);
}

long CMicroTime::microsec() {
    return m_tv.tv_usec;
}

long CMicroTime::sec() {
    return m_tv.tv_sec;
}

int CMicroTime::minuteswest() {
    return m_tz.tz_minuteswest;
}

int CMicroTime::dsttime() {
    return m_tz.tz_dsttime;
}

// 返回微秒
UInt4 GettickCount()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

// 返回秒
Int4 GetMillSec()
{
    int xtu = GettickCount() % 1000;
    return xtu;
}

// 时间秒值
Int4 GetMillSeconds()
{
    time_t nowtime;
    struct tm *timeinfo;
    time( &nowtime );
    timeinfo = localtime( &nowtime );
    int xts = timeinfo->tm_hour;
    int xtf = timeinfo->tm_min;
    int xtm = timeinfo->tm_sec;
    return xts * 60 * 60 * 1000 + xtf * 60 * 1000 + xtm * 1000;
}

