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


class SocketServerTCP;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class NewServer
{
public:
    NewServer();

    void Init();
    bool Bind(u_short port);
    //bool SendMessage(uint8 numMessage, char* data, int size);
    void Close();

    void FuncOnConnect(int, char*, int);

private:
    SocketServerTCP socket;
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
    bool Init(NewServer::pFuncVIpCI funcOnConnect, pFuncVI funcOnDisconnect, pFuncVpCI funcOnRecieve, size_t sizeBuffer);
    bool Listen(u_short port);
    void Transmit(const char *data, int size);
    void Close()
    {
        run = false;
    }

private:
    char buff[1024];

    pFuncVIpCI funcOnConnect;   // num client(socket), address, port
    pFuncVI funcOnDisconnect;   // num client(socket)
    pFuncVpCI funcOnRecieve;    // num recieved bytes
    size_t sizeBuffer;
    bool run = false;

    sockaddr_in address;

    int sockServer;             // Using for Windows - (SOCKET)sockServer
};
