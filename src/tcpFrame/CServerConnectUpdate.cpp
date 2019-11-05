#include "CServerConnectUpdate.h"
#include "CServerConnectListen.h"
#include "CServerSocket.h"
#include "CApplication.h"
//#include "common.h"
#include "CDateTime.h"

CServerConnectUpdate::CServerConnectUpdate(CServerSocket *tSerSocket, bool isDetach)
    : CBaseThread(isDetach)
{
    m_psServer = tSerSocket;
}

void CServerConnectUpdate::execute()
{
    if(m_psServer == nullptr) {
        throw std::logic_error("m_psServer cannot nullptr, please set m_psServer");
    }

    UInt4 MilliTime_u4;
    CApplication::GetInstance()->threadInc();
    sleep(60*4);
    while (!m_isExited)
    {
        if (CApplication::GetInstance()->isTerminate()) {
            break;
        }
        if (!m_psServer->m_isActive) {
            break;
        }
        MilliTime_u4 = GettickCount();
        // 清理超时连接
        m_psServer->m_psConnListion->updateConnector(MilliTime_u4);
        sleep(3);
    }
    CApplication::GetInstance()->threadDec();
    if (m_isDetach)
        delete this;
}
