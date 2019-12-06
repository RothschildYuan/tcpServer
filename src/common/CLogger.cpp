#include "CLogger.h"

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "config.h"
#include "stdtype.h"
//#include "Common.h"
#include "stdarg.h"

namespace {

string getLocalDataFile(int32_t num = 0){
    time_t t = time(nullptr) - num * 86400;
    //Printf("%s\n",ctime(&t));
    struct tm* pt = localtime(&t);
    char buf[256] = {0};
    sprintf(buf,"./%4d-%02d-%02d.log",1900+pt->tm_year,1+pt->tm_mon,pt->tm_mday);
    return string(buf);
}

}

void CLogger::ReInitLoggerFile()
{
    m_log_file_name = getLocalDataFile();
    if(m_log_file_stream.is_open()) {
        m_log_file_stream.close();
    }
    m_log_file_stream.open(m_log_file_name.c_str() ,ios::out | ios::app);
}

CLogger::CLogger() {
    m_log_file_name = getLocalDataFile();                       // 日志文件的名
    m_log_file_stream.open(m_log_file_name.c_str() ,ios::out | ios::app);
}

CLogger::~CLogger(){
    getStream() << endl << flush;
    m_log_file_stream.close();
}

ostream& CLogger::getStream(){
    return m_log_file_stream.is_open() ? m_log_file_stream : cout;
}

ostream& CLogger::start(const string& filename, const int line, const string& function)
{
    if(m_log_file_name != getLocalDataFile()) {
        ReInitLoggerFile();
    }
    (void)filename;
    (void)line;
    (void)function;
	time_t tm;
	time(&tm);
    char time_string[128] = { 0 };
    strncpy(time_string, ctime(&tm), strlen(ctime(&tm))-1);
    //time_string[strlen(ctime(&tm))-1] = 0;
    return getStream() << time_string << " file: (" << filename << ") function:(" << function << ")  line: " << line << "   log:   "<< flush;
}

ostream& CLogger::start()
{
    if(m_log_file_name != getLocalDataFile()) {
        ReInitLoggerFile();
    }
    return getStream()<<flush;
}

void dbg_msg(const char* format, ...) {
    va_list va;
    va_start(va,format);
    char buf[1024*10] = {0};
    vsnprintf(buf, sizeof(buf), format, va);
    va_end(va);
    Qdebug << buf << endl;
}

void dbg_msg_file(const char* file, unsigned int line, const char* func, const char* format, ...) {
    va_list va;
    va_start(va,format);
    char buf[1024*10] = {0};
    snprintf(buf, sizeof(buf),"file: %s, line: (%u), func: <%s>  ", file, line, func);
    long int len = strlen(buf);
    vsnprintf(buf + len, sizeof(buf)-len, format, va);//
    va_end(va);
    Qdebug << buf << endl;
}
