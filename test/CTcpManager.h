#ifndef CTCPMANAGER_H
#define CTCPMANAGER_H

#include "config.h"
#include "stdtype.h"
#include "CServerConnector.h"

class CTcpManager
{
private:
    CTcpManager() {}
    CTcpManager(const CTcpManager&);
    CTcpManager operator=(const CTcpManager&);

    static CTcpManager*   m_instance;
    static ACTMutex       m_insMutex;
    map<string, CServerConnector*> m_mapConns;
public:
    static CTcpManager* GetInstance();
    void add(const string& park_id, CServerConnector* conn);
    int remove(const string& park_id);
    void remove(const int sock);
    CServerConnector* find(const string& park_id);
};

#endif // CTcpManager_H
