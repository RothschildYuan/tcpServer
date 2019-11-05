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
    static void ReInitLoggerFile();

public:
	//构造函数  
    CLogger() {}

    virtual ~CLogger();

    // brief 写入日志信息之前先写入的源代码文件名, 行号, 函数名
    // param log_rank 日志的等级
    // param line 日志发生的行号
    // param function 日志发生的函数
    static std::ostream& start(const std::string&, const int line, const std::string& function);
    static std::ostream& start();

private:
	// 根据等级获取相应的日志输出流
    static std::ostream& getStream();
    static std::ofstream m_log_file_stream;                   // 日志文件的输出流
	static std::string m_log_file_name;                       // 日志文件的名
};

#define __DEBUG__ 3  //

#if __DEBUG__ == 1

//根据不同等级进行用不同的输出流进行读写
#define Qdebug   CLogger::start(__FILE__, __LINE__, __FUNCTION__)

#elif __DEBUG__ == 2

//根据不同等级进行用不同的输出流进行读写
#define Qdebug   CLogger::start()

#elif __DEBUG__ == 3

//根据不同等级进行用不同的输出流进行读写
#define Qdebug   std::cout

#elif __DEBUG__ == 4

//根据不同等级进行用不同的输出流进行读写
#define Qdebug std::ostream(NULL)

#endif

#define print0(msg) Qdebug << msg << std::endl;
#define print1(format, ...) // dbg_msg(format, ##__VA_ARGS__)
#define print2(format, ...) // dbg_msg(format, ##__VA_ARGS__)
#define print3(format, ...) dbg_msg_file(__FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
void dbg_msg(const char* format, ...);
void dbg_msg_file(const char* file, unsigned int line, const char* func, const char* format, ...);

// 利用日志进行各种宏检查
#define CHECK(a)  if (!(a)) { LOG(ERRORLOG) << " CHECK failed " << std::endl << a << " = " << (a) << std::endl;}

#define CHECK_NOTNULL(a)  if (NULL == (a)) {LOG(ERRORLOG) << " CHECK_NOTNULL failed "<< a << " == NULL " << std::endl; }

#define CHECK_NULL(a)  if (NULL != (a)) {LOG(ERRORLOG) << " CHECK_NULL failed " << std::endl<< a << " != NULL " << std::endl;}

#define CHECK_EQ(a, b)  if (!((a) == (b))) {LOG(ERRORLOG) << " CHECK_EQ failed " << std::endl << a << " = " << (a) << std::endl << b << " = " << (b) << std::endl;}

#define CHECK_NE(a, b)  if (!((a) != (b))) {LOG(ERRORLOG) << " CHECK_NE failed " << std::endl << a << " = " << (a) << std::endl << b << " = " << (b) << std::endl; }

#define CHECK_LT(a, b) if (!((a) < (b))) {LOG(ERRORLOG) << " CHECK_LT failed " << a << " = " << (a) << std::endl << b << " = " << (b) << std::endl; }

#define CHECK_GT(a, b) if (!((a) > (b))) {LOG(ERRORLOG) << " CHECK_GT failed " << std::endl  << a << " = " << (a) << std::endl<< b << " = " << (b) << std::endl;  }

#define CHECK_LE(a, b)  if (!((a) <= (b))) {LOG(ERRORLOG) << " CHECK_LE failed " << std::endl << a << " = " << (a) << std::endl << b << " = " << (b) << std::endl; }

#define CHECK_GE(a, b)  if (!((a) >= (b))) {LOG(ERRORLOG) << " CHECK_GE failed " << std::endl<< a << " = " << (a) << std::endl << b << " = " << (b) << std::endl;}

#define CHECK_DOUBLE_EQ(a, b)  do {CHECK_LE((a), (b)+0.000000000000001L);CHECK_GE((a), (b)-0.000000000000001L); } while (0)

#define SAFE_DELETE(p) if(p != NULL) { delete p; p = NULL;}

#endif  
