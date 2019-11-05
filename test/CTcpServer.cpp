#include "CTcpServer.h"

#include "CTcpManager.h"
#include "CServerConnector.h"

static
void connectNotify(CServerConnector *CServerConnector_ti)
{
    if (!CServerConnector_ti)
        return;
    cout << "ip " <<  CServerConnector_ti->host() << endl;
}

static
void closeNotify(CServerConnector *CServerConnector_ti)
{
    cout << "closeNotify " <<  CServerConnector_ti->host() << endl;
    CTcpManager::GetInstance()->remove(CServerConnector_ti->handle());
}

static
void readNotify(void *arg)
{
    cout << "readNotify" << endl;
    CServerConnector *CServerConnector_ti = (CServerConnector *)arg;
    vector<shared_ptr<char> > sockdata = CServerConnector_ti->getSockDatas();

    Sleep(10);

    for(shared_ptr<char> it : sockdata) {
        char* buf = it.get();
        size_t readlen = strlen(buf);
        cout << " len: " <<readlen<< "\nrecv: **"<<buf<<"**\n\n";
        CServerConnector_ti->sendBuf(buf, readlen);
    }

//    size_t bufsiz = CServerConnector_ti->readlen() + 10;
//    char* buf = (char*)alloca(bufsiz);
//    memset(buf, 0, bufsiz);
//    sleep(1);
//    int ret = CServerConnector_ti->readBuf(buf, bufsiz);
//    cout << ret <<" len: " <<CServerConnector_ti->readlen()<< "\nrecv: **"<<buf<<"**\n\n";
//    if(ret < 64 || ret > 9999) {
//        return;
//    }
//    CServerConnector_ti->sendBuf(buf, ret);
//    CTcpManager::GetInstance()->add(dispose.getParkId(), CServerConnector_ti);
}

CTcpServer::CTcpServer(int port)
{
    server.setPort(port);
    server.setTimeout(600 * 1000);
    server.setNotify(connectNotify, closeNotify, readNotify);
    server.setThreadPoolRange(5, 15, 5000);
}

void CTcpServer::start()
{
    server.open(2);
}

void CTcpServer::stop()
{
    server.clear();
}
