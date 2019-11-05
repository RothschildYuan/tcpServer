#include "CTcpManager.h"

#include "CServerConnector.h"

CTcpManager* CTcpManager::m_instance = nullptr;
ACTMutex     CTcpManager::m_insMutex;

CTcpManager* CTcpManager::GetInstance()
{
    if(!m_instance) {
        m_insMutex.lock();
        if(!m_instance) {
            m_instance = new CTcpManager();
        }
        m_insMutex.unlock();
    }
    return m_instance;
}

void CTcpManager::add(const string& park_id, CServerConnector* conn)
{
    m_mapConns[park_id] = conn;
}

int CTcpManager::remove(const string& park_id)
{
    int ret = 0;
    map<string, CServerConnector*>::iterator index = m_mapConns.find(park_id);
    if(index != m_mapConns.end())
    {
        m_mapConns.erase(index);
    }
    else
    {
        ret = -1;
    }
    return ret;
}

void CTcpManager::remove(const int sock)
{
    for(map<string, CServerConnector*>::iterator it = m_mapConns.begin(); it != m_mapConns.end(); ++it) {
        CServerConnector* tmp = it->second;
        if(tmp->handle() == sock) {
            m_mapConns.erase(it);
            break;
        }
    }
}

CServerConnector* CTcpManager::find(const string& park_id)
{
    CServerConnector* ret = nullptr;
    map<string, CServerConnector*>::iterator index = m_mapConns.find(park_id);
    if(index != m_mapConns.end())
    {
        ret = m_mapConns[park_id];
    }
    return ret;
}
