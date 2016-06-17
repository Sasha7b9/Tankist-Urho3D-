#include <stdafx.h>

#include "Socket.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SocketClientTCP::SocketClientTCP()
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool SocketClientTCP::Init(pFuncVpVI funcOnRecieve)
{
    this->funcOnRecieve = funcOnRecieve;

    if(WSAStartup(0x202, (WSADATA*)&buff[0]))
    {
        LOG_ERROR1("Winsock not initialized with error %d", WSAGetLastError());
        return false;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock == INVALID_SOCKET)
    {
        LOG_ERROR1("ocket() error %d", WSAGetLastError());
        return false;
    }

    return true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool SocketClientTCP::Connect(const char *address, u_short port)
{
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(port);

    if(inet_addr(address) != INADDR_NONE)
    {
        destAddr.sin_addr.s_addr = inet_addr(address);
    }
    else
    {
        LOG_ERROR1("Invalid address %s", address);
        return false;
    }

    if(connect(sock, (sockaddr*)&destAddr, sizeof(destAddr)))
    {
        LOG_ERROR1("Connect error %d", WSAGetLastError());
        return false;
    }

    return true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SocketClientTCP::Transmit(const char *data, int size)
{
    send(sock, data, size, 0);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
int SocketClientTCP::Recieve(char *buffer, int sizeBuffer)
{
    return recv(sock, buffer, sizeBuffer, 0);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SocketClientTCP::Close()
{

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
        LOG_ERROR1("Error WSAStartup %d", WSAGetLastError());
        return false;
    }
    else
    {
        LOG_INFO("WSAStartup is ok!");
    }

    sockServer = (int)socket(AF_INET, SOCK_STREAM, 0);
    if (sockServer == 0)
    {
        LOG_ERROR1("Erorr socket() %d", WSAGetLastError());
        WSACleanup();
        return false;
    }
    else
    {
        LOG_INFO("socket() is ok!");
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

        HOSTENT *hst = gethostbyaddr((char*)&addrClient.sin_addr.s_addr, 4, AF_INET);

        if (hst)
        {

            LOG_INFO1("%s connect", hst->h_name);

            t = new std::thread(ExchangeTaks, (int)newSock, sockParam);

            sockParam->funcOnConnect(sockParam->server, (int)newSock, hst->h_name, addrClient.sin_port);
        }
        else
        {
            LOG_ERROR1("accept() error %d", WSAGetLastError());
        }
    }

#else

    while(run)
    {
        int newSock = accept(sockServer, (sockaddr*)&addrClient, &lenClient);
        if(newsock < 0)
        {
            LOG_ERROR1("accept() failed: %d", errno)
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
    LOG_INFO2("%s(%d)", __FUNCTION__, port);

    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

#ifdef WIN32

    if(bind((SOCKET)sockServer, (sockaddr*)&address, sizeof(address)))
    {
        LOG_ERROR1("Error bind() %d", WSAGetLastError());
        WSACleanup();
        return false;
    }

    if(listen((SOCKET)sockServer, 100))
    {
        LOG_ERROR1("Error listen() %d", WSAGetLastError());
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

    std::thread *t = new std::thread(AcceptTask, sockServer, sockParam);

    return true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SocketServerTCP::Transmit(const void *data, uint size)
{
    send((SOCKET)sockServer, (const char*)data, (int)size, 0);
}