#include "stdafx.h"


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

    sockClient = (int)socket(AF_INET, SOCK_STREAM, 0);

    if((SOCKET)sockClient == INVALID_SOCKET)
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

    if(connect((SOCKET)sockClient, (sockaddr*)&destAddr, sizeof(destAddr)))
    {
        LOG_ERRORF("Connect error %d", WSAGetLastError());
        return false;
    }

    if(type == Socket_Asynch)
    {
        t = new std::thread(CallbackOnRecieve, sockClient, (void*)buff, 1024, run, funcOnRecieve, clientTCP);
    }

    return true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SocketClientTCP::Transmit(void *data, int size)
{
    send((SOCKET)sockClient, (char*)data, size, 0);
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
