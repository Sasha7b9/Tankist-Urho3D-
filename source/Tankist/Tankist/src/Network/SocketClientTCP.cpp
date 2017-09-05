#include <stdafx.h>
#include "SocketClientTCP.h"


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
bool SocketClientTCP::Init(TypeSocket type_, pFuncVpVpVI funcOnRecieve_, void *clientTCP_)
{
    type = type_;
    funcOnRecieve = funcOnRecieve_;
    clientTCP = clientTCP_;

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

#ifdef WIN32
    if(WSAStartup(0x202, (WSADATA*)&buff[0]))
    {
        LOG_ERRORF("Winsock not initialized with error %d", WSAGetLastError());
        return false;
    }
#endif

    sockClient = socket(AF_INET, SOCK_STREAM, 0);

#ifdef WIN32
    if(sockClient == INVALID_SOCKET)
    {
        LOG_ERRORF("socket() error %d", WSAGetLastError());
        return false;
    }
#endif

    return true;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void CallbackOnRecieve(SOCKET sock, void *buffer, int sizeBuffer, bool *run, pFuncVpVpVI funcOnRecieve, void *clientTCP)
{
    while(*run)
    {
        int numBytes = recv(sock, (char*)buffer, sizeBuffer, 0);
        funcOnRecieve(clientTCP, buffer, numBytes);
        gCounters->AddClientIn(numBytes);
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

    if(connect(sockClient, (sockaddr*)&destAddr, sizeof(destAddr)))
    {
        LOG_ERRORF("Connect error %d", WSAGetLastError());
        return false;
    }

    if(type == Socket_Asynch)
    {
        t = new std::thread(CallbackOnRecieve, sockClient, (void*)buff, 1024, &run, funcOnRecieve, clientTCP);
    }

    return true;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void SocketClientTCP::Transmit(void *data, int size)
{
    send((SOCKET)sockClient, (char*)data, size, 0);

    gCounters->AddClientOut(size);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
int SocketClientTCP::Recieve(char *buffer, int sizeBuffer)
{
    if(type == Socket_Asynch)
    {
        LOG_ERROR("Call Recieve method for asynch socket");
        return 0;
    }

    return recv((SOCKET)sockClient, buffer, sizeBuffer, 0);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void SocketClientTCP::Close()
{
    run = false;
}
