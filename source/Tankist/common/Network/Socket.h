#pragma once


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "defines.h"

#pragma warning(push)
#pragma warning(disable:4265 4571 4625 4626)

#include <thread>

#pragma warning(pop)

#ifdef WIN32

#ifndef AF_IPX

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

#endif  // AF_IPX

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#endif  // WIN32


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SocketParam
{
    pFuncVIpCI  funcOnConnect;          // num client(socket), address, port
    pFuncVI     funcOnDisconnect;       // num client(socket)
    pFuncVIpCI  funcOnReceive;          // num client, num recieved bytes
    size_t      sizeBuffer;
    bool        run;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SocketClientTCP
{
public:
    SocketClientTCP();

    bool Init();
    bool Connect(const char *address, u_short port);
    void Transmit(const char *data, int size);
    int Recieve(char *buffer, int sizeBuffer);
    void Close();

private:
    char buff[1024];

#ifdef WIN32
    SOCKET sock;
    sockaddr_in destAddr;
#endif
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SocketServerTCP
{
public:
    SocketServerTCP();

    // funcOnConnect call on connect new client
    // funcOnDisconnect call on disconnect client
    // funcOnRecieve call on recieved data
    // buffer - buffer for recieved data
    // sizeBuffer - size of buffer
    bool Init(SocketParam *sockParam);
    bool Listen(u_short port);
    void Transmit(const char *data, int size);
    void Close()
    {
        if(sockParam)
        {
            sockParam->run = false;
            sockParam = nullptr;
        }
    }

private:
    char buff[1024];
    SocketParam *sockParam = nullptr;
    sockaddr_in address;
    int sockServer;             // Using for Windows - (SOCKET)sockServer
};
