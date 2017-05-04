#include "stdafx.h"


#include "SocketServerTCP.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SocketServerTCP::SocketServerTCP()
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool SocketServerTCP::Init(SocketParam *sockParam_)
{
    sockParam = sockParam_;

#ifdef WIN32

    if(WSAStartup(0x0202, (WSADATA*)&buff[0]))
    {
        LOG_ERRORF("Error WSAStartup %d", WSAGetLastError());
        return false;
    }

    sockServer = socket(AF_INET, SOCK_STREAM, 0);
    if(sockServer == 0)
    {
        LOG_ERRORF("Erorr socket() %d", WSAGetLastError());
        WSACleanup();
        return false;
    }

#else

    sockServer = socket(AF_INET, SOCK_STREAM, 0);
    if(sockServer < 0)
    {
        LOG_ERROR("Can not create socket");
        return false;
    }


#endif  // WIN32

    return true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void ExchangeTaks(SOCKET sock, SocketParam *sockParam)
{
    char *buffer = new char[sockParam->sizeBuffer];

    while(sockParam->run)
    {
        int numBytes = recv(sock, buffer, (int)sockParam->sizeBuffer, 0);
        gCounters->AddServerIn(numBytes);
#ifdef WIN32
        if(numBytes == SOCKET_ERROR)
#else
        if(numBytes == 0)
#endif
        {
            sockParam->funcOnDisconnect(sockParam->server, sock);
            break;
        }
        sockParam->funcOnReceive(sockParam->server, sock, buffer, numBytes);
    }

    closesocket(sock);

    delete[] buffer;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void AcceptTask(SOCKET sockServer, SocketParam *sockParam)
{
    std::thread *t;

    sockaddr_in addrClient;
#ifdef WIN32
    int lenClient = sizeof(addrClient);
#else
    socklen_t lenClient = sizeof(addrClient);
#endif

    while(sockParam->run)
    {
        SOCKET newSock = accept(sockServer, (sockaddr*)&addrClient, &lenClient);
#ifdef WIN32
        if(newSock != INVALID_SOCKET)
#else
        if(newSock >= 0)
#endif
        {
            char buffAddr[30];
#ifdef WIN32
            uint longAddr = addrClient.sin_addr.S_un.S_addr;
            sprintf_s(buffAddr, 19,
#else
            uint longAddr = addrClient.sin_addr.s_addr;
            sprintf(buffAddr,
#endif
                              "%d.%d.%d.%d", longAddr & 0xff, (longAddr >> 8) & 0xff, (longAddr >> 16) & 0xff, (longAddr >> 24) & 0xff);

            t = new std::thread(ExchangeTaks, (int)newSock, sockParam);

            sockParam->funcOnConnect(sockParam->server, newSock, buffAddr, addrClient.sin_port);
        }
        else
        {
            LOG_ERRORF("Failed accept() with error %d", WSAGetLastError());
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool SocketServerTCP::Listen(uint16 port)
{
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sockServer, (sockaddr*)&address, sizeof(address)))
    {
        LOG_ERRORF("Error bind() %d", WSAGetLastError());
#ifdef WIN32
        WSACleanup();
#endif
        return false;
    }

    if(listen(sockServer, 100))
    {
        LOG_ERRORF("Error listen() %d", WSAGetLastError());
        closesocket(sockServer);
#ifdef WIN32
        WSACleanup();
#endif
        return false;
    }

    sockParam->run = true;

    t = new std::thread(AcceptTask, sockServer, sockParam);

    return true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SocketServerTCP::Transmit(const void *data, uint size)
{
    int numBytes = send(sockServer, (const char*)data, (int)size, 0);
    gCounters->AddServerOut(numBytes);

    //LOG_INFOF("socket = %d, Transferred %d, pass %d, error %d", sockServer, size, numBytes, WSAGetLastError());
}
