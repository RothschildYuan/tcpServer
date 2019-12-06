/****************************************************************************** 
 
            版权所有 (C), 2016-2017, 深圳市沃土科技有限公司 
 
 ****************************************************************************** 
    文 件 名 : QLogger.h
    版 本 号 : V1.0 
    作    者 : 钱旺 
    生成日期 : 2017年9月9日 
    功能描述 : 打印日志类头文件 
    修改历史 : 
******************************************************************************/ 
#ifndef  __CLOGGER_H_
#define  __CLOGGER_H_

#include <iostream>  
#include <iomanip>  
#include <fstream>  
#include <string>  
#include <cstdlib>  
#include <stdint.h>

using std::string;
using std::ostream;
using std::ofstream;

// 日志文件的类型
typedef enum log_rank {
	INFOLOG,
	WARNLOG,
	ERRORLOG,
	FATALLOG
}log_rank_t;

//日志系统类
class CLogger {
    // brief 初始化日志文件
    // param info_log_filename 信息文件的名字
    // param warn_log_filename 警告文件的名字
    // param error_log_filename 错误文件的名字
    void ReInitLoggerFile();
    //构造函数
    CLogger();

public:
    static CLogger& getInstance() {
        static CLogger ins;
        return ins;
    }

    virtual ~CLogger();

    // brief 写入日志信息之前先写入的源代码文件名, 行号, 函数名
    // param log_rank 日志的等级
    // param line 日志发生的行号
    // param function 日志发生的函数
    ostream& start(const string&, const int line, const string& function);
    ostream& start();

private:
	// 根据等级获取相应的日志输出流
    ostream& getStream();
    ofstream m_log_file_stream;                   // 日志文件的输出流
    string m_log_file_name;                       // 日志文件的名
};

#define __DEBUG__ 3  //

#if __DEBUG__ == 1

//根据不同等级进行用不同的输出流进行读写
#define Qdebug   CLogger::getInstance().start(__FILE__, __LINE__, __FUNCTION__)

#elif __DEBUG__ == 2

//根据不同等级进行用不同的输出流进行读写
#define Qdebug   CLogger::getInstance().start()

#elif __DEBUG__ == 3

//根据不同等级进行用不同的输出流进行读写
#define Qdebug   cout

#elif __DEBUG__ == 4

//根据不同等级进行用不同的输出流进行读写
#define Qdebug ostream(NULL)

#endif

#define print0(msg) Qdebug << msg << endl;
#define print1(format, ...) // dbg_msg(format, ##__VA_ARGS__)
#define print2(format, ...) // dbg_msg(format, ##__VA_ARGS__)
#define print3(format, ...) dbg_msg_file(__FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
void dbg_msg(const char* format, ...);
void dbg_msg_file(const char* file, unsigned int line, const char* func, const char* format, ...);

// 利用日志进行各种宏检查
#define CHECK(a)  if (!(a)) { LOG(ERRORLOG) << " CHECK failed " << endl << a << " = " << (a) << endl;}

#define CHECK_NOTNULL(a)  if (NULL == (a)) {LOG(ERRORLOG) << " CHECK_NOTNULL failed "<< a << " == NULL " << endl; }

#define CHECK_NULL(a)  if (NULL != (a)) {LOG(ERRORLOG) << " CHECK_NULL failed " << endl<< a << " != NULL " << endl;}

#define CHECK_EQ(a, b)  if (!((a) == (b))) {LOG(ERRORLOG) << " CHECK_EQ failed " << endl << a << " = " << (a) << endl << b << " = " << (b) << endl;}

#define CHECK_NE(a, b)  if (!((a) != (b))) {LOG(ERRORLOG) << " CHECK_NE failed " << endl << a << " = " << (a) << endl << b << " = " << (b) << endl; }

#define CHECK_LT(a, b) if (!((a) < (b))) {LOG(ERRORLOG) << " CHECK_LT failed " << a << " = " << (a) << endl << b << " = " << (b) << endl; }

#define CHECK_GT(a, b) if (!((a) > (b))) {LOG(ERRORLOG) << " CHECK_GT failed " << endl  << a << " = " << (a) << endl<< b << " = " << (b) << endl;  }

#define CHECK_LE(a, b)  if (!((a) <= (b))) {LOG(ERRORLOG) << " CHECK_LE failed " << endl << a << " = " << (a) << endl << b << " = " << (b) << endl; }

#define CHECK_GE(a, b)  if (!((a) >= (b))) {LOG(ERRORLOG) << " CHECK_GE failed " << endl<< a << " = " << (a) << endl << b << " = " << (b) << endl;}

#define CHECK_DOUBLE_EQ(a, b)  do {CHECK_LE((a), (b)+0.000000000000001L);CHECK_GE((a), (b)-0.000000000000001L); } while (0)

#define SAFE_DELETE(p) if(p != NULL) { delete p; p = NULL;}

#endif  
