#include <iostream>

#include "CTcpServer.h"

using namespace std;

int main()
{
    CTcpServer tcpServ(8185);
    tcpServ.start();
    while (1) {
        sleep(1000);
    }
    return 0;
}
