PROGRAM = libtcp.so             # 最后的可执行文件

# 通配当前目录下.cpp 和 .c文件
CFILES := $(shell find src -mindepth 1 -maxdepth 4 -name "*.c")
CXXFILES := $(shell find src -mindepth 1 -maxdepth 4 -name "*.cpp")
# 输入文件
SOURCES := $(CFILES) $(CXXFILES)

#OBJFILES := $(CXXFILES:src/%.cpp=%) $(CFILES:src/%.c=%)
#OBJS := $(OBJFILES:%=obj/%.o)

#SOURCE  := $(wildcard *.c) $(wildcard *.cpp)
OBJS    += $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))      #把$(SOURCES)中的变量符合后缀是.cpp的全部替换成.o

GXX = g++ 
GCC = gcc 
LINK = -shared -fPIC

# CFLAGS  = -c -ggdb -Wall -DBUILD_TIME="\"`date`\"" -DDEBUG
COMFLAGS  = -g -o2 -DDEBUG -Wall -Wextra 

CFLAGS  = $(COMFLAGS) 
CFLAGS += --std=c99

CCFLAGS  = $(COMFLAGS)
CCFLAGS += -std=c++11  
CCFLAGS += -Wno-deprecated	
CCFLAGS += -D linux
CCFLAGS += 

#-g: 	在对象文件中生成调试信息
#-o2: 	编译等级
#-I: 	添加头文件的搜索路径
#-o: 	输出到指定文件
#-c: 	编译源码, 但是不链接
#-D:	添加宏定义
#-Wall 	打开gcc的所有警告
#-Wextra：打印出更多的警告信息，比开启 -Wall 打印的还多
#-Werror，它要求gcc将所有的警告当成错误进行处理
#-pedantic 源文件中使用扩展语法的地方将产生相应的警告信息

INCLUDES  = -I ./ -I ./include -I ./src/common -I ./src/socket 
INCLUDES += -I ./src/tcpFrame -I ./src/thread -I ./src/threadPool

LIBS += -L -lutil -lpthread -ldl -lrt

$(PROGRAM):$(OBJS)
	$(GXX) $(LINK) $^ $(INCLUDE) $(CCFLAGS) $(LIBS) -o $@

%.o: %.cpp
	$(GXX) -fPIC -c $< $(CCFLAGS) $(INCLUDES) -o $@

%.o: %.c
	$(GCC) -fPIC -c $< $(CFLAGS) $(INCLUDES) -o $@
	
.PHONY: install clean cleanall
install: $(PROGRAM)
	cp $(PROGRAM) ./bin/
clean : 
	rm -rf $(PROGRAM)
cleanall : 
	rm -rf $(PROGRAM) $(OBJS) ./test/*.o
