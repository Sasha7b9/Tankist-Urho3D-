#include "Socket.h"

#pragma warning(push)
#pragma warning(disable:4100 4251 4265 4266 4267 4275 4312 4365 4571 4625 4626 4640)

#include <Urho3D/IO/Log.h>

#pragma warning(pop)


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
bool SocketServerTCP::Init(pFuncVIpCI funcOnConnect, pFuncVI funcOnDisconnect, pFuncVpCI funcOnRecieve, size_t sizeBuffer)
{
    this->funcOnConnect = funcOnConnect;
    this->funcOnDisconnect = funcOnDisconnect;
    this->funcOnRecieve = funcOnRecieve;
    this->sizeBuffer = sizeBuffer;

#ifdef WIN32

    if(WSAStartup(0x0202, (WSADATA*) &buff[0]))
    {
        LOG_ERROR1("Error WSAStartup %d", WSAGetLastError());
        return false;
    }

    sockServer = (int)socket(AF_INET, SOCK_STREAM, 0);
    if (sockServer == 0)
    {
        LOG_ERROR1("Erorr socket() %d", WSAGetLastError());
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
static void ExchangeTaks(int sock, bool run, pFuncVI funcOnDisconnect, pFuncVpCI funcOnRecieve, size_t sizeBuffer)
{
    char *buffer = new char[sizeBuffer];

#ifdef WIN32

    while (run)
    {
        int numBytes = recv((SOCKET)sock, buffer, (int)sizeBuffer, 0);

        if(numBytes == SOCKET_ERROR)
        {
            funcOnDisconnect(sock);
            break;
        }

        funcOnRecieve(buffer, numBytes);
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
static void AcceptTask(int sockServer, bool &run, pFuncVIpCI funcOnConnect, pFuncVI funcOnDisconnect, pFuncVpCI funcOnRecieve, int sizeBuffer)
{
    std::thread *t;

    sockaddr_in addrClient;
    int lenClient = sizeof(addrClient);

#ifdef WIN32

    while(run)
    {
        SOCKET newSock = accept((SOCKET)sockServer, (sockaddr*)&addrClient, &lenClient);

        HOSTENT *hst = gethostbyaddr((char*)&addrClient.sin_addr.s_addr, 4, AF_INET);

        if (hst)
        {

            LOG_INFO1("%s connect", hst->h_name);

            t = new std::thread(ExchangeTaks, (int)newSock, run, funcOnDisconnect, funcOnRecieve, sizeBuffer);

            funcOnConnect((int)newSock, hst->h_name, addrClient.sin_port);
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
bool SocketServerTCP::Listen(u_short port)
{
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

    run = true;

    std::thread t(AcceptTask, sockServer, run, funcOnConnect, funcOnDisconnect, funcOnRecieve, sizeBuffer);
}
