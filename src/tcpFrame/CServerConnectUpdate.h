#ifndef CSERVERCONNECTUPDATE_H
#define CSERVERCONNECTUPDATE_H

#include "CBaseThread.h"

class CServerSocket;

class CServerConnectUpdate : public CBaseThread
{
private:
    CServerSocket *m_psServer;
public:
    CServerConnectUpdate(CServerSocket *tSerSocket, bool isDetach);
    void execute() override;
};

#endif // CServerConnectUpdate_H
