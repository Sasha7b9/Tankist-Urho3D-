#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "defines.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SocketParam
{
    pFuncVpVIpCU16  funcOnConnect;          // *ServerTCP, num client(socket), address, port
    pFuncVpVI       funcOnDisconnect;       // *ServerTCP, num client(socket)
    pFuncVpVIpVI    funcOnReceive;          // *ServerTCP, num client, num recieved bytes
    size_t          sizeBuffer;
    void            *server;
    bool            run;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SocketClientTCP
{
public:
    SocketClientTCP();

    bool Init(pFuncVpVI funcOnRecieve);
    bool Connect(const char *address, u_short port);
    void Transmit(const char *data, int size);
    int Recieve(char *buffer, int sizeBuffer);
    void Close();

private:
    char buff[1024];

#ifdef WIN32
    SOCKET      sock;
    sockaddr_in destAddr;
    pFuncVpVI   funcOnRecieve;
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
    bool Listen(uint16 port);
    void Transmit(const void *data, uint size);
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
