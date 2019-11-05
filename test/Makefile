PROGRAM = TcpTest             # 最后的可执行文件

SOURCES  = $(wildcard *.cpp)          # 通配当前目录下.cpp文件

OBJS    += $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))      #把$(SOURCES)中的变量符合后缀是.cpp的全部替换成.o

GXX = g++ 
GCC = gcc 

CFLAGS  = -g -o2 -DDEBUG -Wall -Wextra 

CCFLAGS  = -std=c++11 -g -o2 -DDEBUG -Wall -Wextra 
CCFLAGS  += -Wno-deprecated	
CCFLAGS  += -D linux 
CCFLAGS  += 

INCLUDE = -I ./ -I ../include -I ./common -I ./sqlConnPool 
INCLUDE += -I ./include-internal -I ./include-internal/mysql -I ./include/postgresql

LIB += -L. ../libtcp.so
LIB += -L -lutil -lpthread -ldl -lrt

$(PROGRAM):$(OBJS)
	$(GXX) $(LINK) $^ $(INCLUDE) $(CCFLAGS) $(LIB) -o $@

%.o:%.cpp
	$(GXX) -fPIC -c $< $(CCFLAGS) $(INCLUDE) -o $@

%.o:%.c
	$(GCC) -fPIC -c $< $(CFLAGS) $(INCLUDE) -o $@
	
.PHONY: install clean cleanall
install: $(PROGRAM)
	cp $(PROGRAM) ./bin/
clean : 
	rm -rf $(PROGRAM)
cleanall : 
	rm -rf $(PROGRAM)  *.o ./test/*.o ./*.log 
