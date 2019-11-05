#ifndef CTCPSERVER_H
#define CTCPSERVER_H

#include "CServerSocket.h"

class CTcpServer
{
public:
    CTcpServer(int port = 10002);
    void start();
    void stop();
private:
    CServerSocket server;
};

#endif // CTcpServer_H
