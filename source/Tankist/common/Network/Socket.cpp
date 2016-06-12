#include "Socket.h"

#pragma warning(push)
#pragma warning(disable:4100 4251 4266 4275 4365 4625 4626 4640)

#include <Urho3D/IO/Log.h>

#pragma warning(pop)

#include <thread>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SocketClientTCP::SocketClientTCP()
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool SocketClientTCP::Init()
{
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
bool SocketServerTCP::Init(pFuncVIpCI funcOnConnect, pFuncVI funcOnDisconnect, pFuncVI funcOnRecieve, void *buffer, int sizeBuffer)
{
    this->funcOnConnect = funcOnConnect;
    this->funcOnDisconnect = funcOnDisconnect;
    this->funcOnRecieve = funcOnRecieve;
    this->buffer = buffer;
    this->sizeBuffer = sizeBuffer;

#ifdef WIN32

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
static void ExchangeTaks(int sock, pFuncVI funcOnDisconnect, pFuncVU8pCI funcOnRecieve, void *buffer, int size)
{
#ifdef WIN32


#else

    while(true)
    {
        int numBytes = recv(sock, buffer, size, 0);
        if(numBytes == 0)
        {
            funcOnDisconnect(sock);
            return;
        }
        else
        {
            pFuncVU8
        }
    }

#endif
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void AcceptTask(bool &run, pFuncVIpCI funcOnConnect, pFuncVI funcOnDisconnect, pFuncVI funcOnRecieve, void *buffer, int sizeBuffer)
{
#ifdef WIN32

#else

    sockaddr_in addrClient;
    int lenClient = sizeof(addrClient);

    while(run)
    {
        int newSock = accept(sockServer, (sockaddr*)&addrClient, &lenClient);
        if(newsock < 0)
        {
            LOG_ERROR1("accept() failed: %d", errno)
        }
        else
        {

        }
    }

#endif  // WIN32
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool SocketServerTCP::Listen(u_short port)
{
#ifdef WIN32


#else

    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sockServer, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        LOG_ERROR1("bind() failed: %d", errno);
        return false;
    }

    listen(sockServer, 100);

    run = true;

    std::thread t(AcceptTask, run, funcOnConnect, funcOnDisconnect, funconf);

#endif  // WIN32
}
