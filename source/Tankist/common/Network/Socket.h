#pragma once


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "defines.h"

#ifdef WIN32

#ifndef AF_IPX

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

#endif

#else

#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SocketClient
{
public:
    SocketClient();
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