#ifndef CONFIG_H
#define CONFIG_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <fcntl.h>
#include <stdint.h>
#include <assert.h>
#include <signal.h>
#include <math.h>

#ifdef __linux__

    #include <ifaddrs.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <sys/poll.h>
    #include <sys/select.h>
    #include <sys/epoll.h>
    #include <sys/ioctl.h>
    #include <sys/wait.h>
	#include <sys/time.h>
    #include <sys/timerfd.h>
    #include <sys/eventfd.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <net/if.h>
    #include <errno.h>
    #include <netdb.h>
    #include <iconv.h>

    #include <ctype.h>
    #include <strings.h>
    #include <pthread.h>

    #define Sleep(x)        usleep((x) * 1000)

#else

    //#include <WinSock2.h>
    #include <windows.h>
	#include<direct.h>
	#include<io.h>
    #include <sys/utime.h>

    #pragma comment(lib,"Ws2_32.lib")
    #define socklen_t int
    
    #define _CRT_SECURE_NO_WARNINGS
    #pragma warning(disable:4996)

    #define F_OK			0
    #define access(t, x)	_access(t, x)
    #define mkdir(t, x)		_mkdir((t))
    #define sleep(x)		Sleep((x) * 1000)
    #define snprintf		_snprintf
    #define open            _open
    #define read            _read
    #define close           _close
    #define strdup          _strdup
    #define strcasecmp      _stricmp
    #define strncasecmp     _strnicmp

inline char * strcasestr(const char* src,const char* need){
    if(NULL == src || NULL == need) {
        return NULL;
    }
    size_t len = strlen(need);
    int low = tolower(*need);
    char* cp = (char*)src;
    while(*cp){
        if(!tolower(*cp) - low) {
            if(strncasecmp(cp, need, len) == 0) {
                return cp;
            }
        }
        cp++;
    }
    return NULL;
}

#endif

// PropertyBuilderByName 用于生成类的成员变量
// 并生成set和get方法
// type 为变量类型
// access_permission 为变量的访问权限(public, priavte, protected)
// 指针浅拷贝
#define PropertyBuilderByName(type, name, access_permission)\
    access_permission:                              \
        type m_##name;                              \
    public:                                         \
    inline void set##name(const type v) {           \
        m_##name = v;                               \
    }                                               \
    inline type get##name() const {                 \
        return m_##name;                            \
    }

// 指针深拷贝
#define PropertyPointerBuilderByName(type, name, access_permission)\
    access_permission:                              \
        type* m_##name;                             \
    public:                                         \
        inline void set##name(const type* v){       \
            if(m_##name) {                          \
                memcpy(m_##name, v, sizeof(type));  \
            } else {                                \
                m_##name = new type(*v);            \
            }                                       \
        }                                           \
        inline type* get##name() const {            \
            return m_##name;                        \
        }

#ifdef __cplusplus /* 如果采用了C++，如下代码使用C编译器 */
    extern "C" { /* 如果没有采用C++，顺序预编译 */
#endif
/* 采用C编译器编译的C语言代码段 */
#ifdef __cplusplus /* 结束使用C编译器 */
    }
#endif

#endif // CONFIG_H
