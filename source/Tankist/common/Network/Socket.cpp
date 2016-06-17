#include <stdafx.h>

#include "Socket.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SocketClientTCP::SocketClientTCP()
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
SocketClientTCP::~SocketClientTCP()
{
    Close();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool SocketClientTCP::Init(TypeSocket type, pFuncVpVpVI funcOnRecieve, void *clientTCP)
{
    this->type = type;
    this->clientTCP = clientTCP;

    if(type == Socket_Synch && funcOnRecieve)
    {
        LOG_ERROR("Error create SocketClientTCP - an synch type socket funcOnRecieve it isn't used");
        return false;
    }

    if(type == Socket_Asynch && funcOnRecieve == 0)
    {
        LOG_ERROR("Error create SocketClientTCP - an asynch type socket has to use funcOnRecieve");
        return false;
    }


    this->funcOnRecieve = funcOnRecieve;

    if(WSAStartup(0x202, (WSADATA*)&buff[0]))
    {
        LOG_ERRORF("Winsock not initialized with error %d", WSAGetLastError());
        return false;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock == INVALID_SOCKET)
    {
        LOG_ERRORF("ocket() error %d", WSAGetLastError());
        return false;
    }

    return true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void CallbackOnRecieve(int sock, void *buffer, int sizeBuffer, bool &run, pFuncVpVpVI funcOnRecieve, void *clientTCP)
{
    while(run)
    {
        int numBytes = recv((SOCKET)sock, (char*)buffer, sizeBuffer, 0);
        funcOnRecieve(clientTCP, buffer, numBytes);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool SocketClientTCP::Connect(const char *address, uint16 port)
{
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(port);

    if(inet_addr(address) != INADDR_NONE)
    {
        destAddr.sin_addr.s_addr = inet_addr(address);
    }
    else
    {
        LOG_ERRORF("Invalid address %s", address);
        return false;
    }

    if(connect(sock, (sockaddr*)&destAddr, sizeof(destAddr)))
    {
        LOG_ERRORF("Connect error %d", WSAGetLastError());
        return false;
    }

    if(type == Socket_Asynch)
    {
        t = new std::thread(CallbackOnRecieve, sock, (void*)buff, 1024, run, funcOnRecieve, clientTCP);
    }

    return true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SocketClientTCP::Transmit(void *data, int size)
{
    send(sock, (char*)data, size, 0);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
int SocketClientTCP::Recieve(char *buffer, int sizeBuffer)
{
    if(type == Socket_Asynch)
    {
        LOG_ERROR("Call Recieve method for asynch socket");
        return 0;
    }

    return recv(sock, buffer, sizeBuffer, 0);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SocketClientTCP::Close()
{
    run = false;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
SocketServerTCP::SocketServerTCP()
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool SocketServerTCP::Init(SocketParam *sockParam)
{
    this->sockParam = sockParam;

#ifdef WIN32

    if(WSAStartup(0x0202, (WSADATA*) &buff[0]))
    {
        LOG_ERRORF("Error WSAStartup %d", WSAGetLastError());
        return false;
    }

    sockServer = (int)socket(AF_INET, SOCK_STREAM, 0);
    if (sockServer == 0)
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
static void ExchangeTaks(int sock, SocketParam *sockParam)
{
    char *buffer = new char[sockParam->sizeBuffer];

#ifdef WIN32

    while (sockParam->run)
    {
        int numBytes = recv((SOCKET)sock, buffer, (int)sockParam->sizeBuffer, 0);
        if(numBytes == SOCKET_ERROR)
        {
            sockParam->funcOnDisconnect(sockParam->server, sock);
            break;
        }
        sockParam->funcOnReceive(sockParam->server, sock, buffer, numBytes);
    }

    closesocket((SOCKET)sock);

#else

    while(run)
    {
        int numBytes = recv(sock, buffer, sizeBuffer, 0);
        if(numBytes == 0)
        {
            funcOnDisconnect(sock);
            break;
        }
        funcOnRecieve(buffer, numBytes);
    }

    closesocket(sock);

#endif

    delete[] buffer;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void AcceptTask(int sockServer, SocketParam *sockParam)
{
    std::thread *t;

    sockaddr_in addrClient;
    int lenClient = sizeof(addrClient);

#ifdef WIN32

    while(sockParam->run)
    {
        SOCKET newSock = accept((SOCKET)sockServer, (sockaddr*)&addrClient, &lenClient);

        if(newSock != INVALID_SOCKET)
        {
            uint longAddr = addrClient.sin_addr.S_un.S_addr;
            char buffAddr[30];
            sprintf_s(buffAddr, 19, "%d.%d.%d.%d", longAddr & 0xff, (longAddr >> 8) & 0xff, (longAddr >> 16) & 0xff, (longAddr >> 24) & 0xff);

            t = new std::thread(ExchangeTaks, (int)newSock, sockParam);

            sockParam->funcOnConnect(sockParam->server, (int)newSock, buffAddr, addrClient.sin_port);
        }
        else
        {
            LOG_ERRORF("Failed accept() with error %d", WSAGetLastError());
        }
    }

#else

    while(run)
    {
        int newSock = accept(sockServer, (sockaddr*)&addrClient, &lenClient);
        if(newsock < 0)
        {
            LOG_ERRORF("accept() failed: %d", errno)
        }
        else
        {
            t = new std::thread(ExchangeTaks, newSock, run, funcOnDisconnect, funcOnRecieve, sizeBuffer);
            funcOnConnect(newSock, addrClient.sin_addr.s_addr, addrClient.sin_port);
        }
    }

#endif  // WIN32
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool SocketServerTCP::Listen(uint16 port)
{
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

#ifdef WIN32

    if(bind((SOCKET)sockServer, (sockaddr*)&address, sizeof(address)))
    {
        LOG_ERRORF("Error bind() %d", WSAGetLastError());
        WSACleanup();
        return false;
    }

    if(listen((SOCKET)sockServer, 100))
    {
        LOG_ERRORF("Error listen() %d", WSAGetLastError());
        closesocket((SOCKET)sockServer);
        WSACleanup();
        return false;
    }

#else

    if(bind(sockServer, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        LOG_ERROR1("bind() failed: %d", errno);
        return false;
    }

    listen(sockServer, 100);

#endif  // WIN32

    sockParam->run = true;

    t = new std::thread(AcceptTask, sockServer, sockParam);

    return true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SocketServerTCP::Transmit(const void *data, uint size)
{
    int numBytes = send((SOCKET)sockServer, (const char*)data, (int)size, 0);

    LOG_INFOF("socket = %d, Transferred %d, pass %d, error %d", sockServer, size, numBytes, WSAGetLastError());
}
