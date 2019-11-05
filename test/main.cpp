#include <iostream>

#include "CTcpServer.h"

using namespace std;

int main()
{
    // 申明 TCP 服务对象，然后开启服务
    CTcpServer tcpServ(8185);
    tcpServ.start();
    while (1) {
        sleep(1000);
    }
    return 0;
}
