#include "Socket.h"

#pragma warning(push)
#pragma warning(disable:4100 4251 4266 4275 4365 4625 4626 4640)

#include <Urho3D/IO/Log.h>

#pragma warning(pop)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SocketClient::SocketClient()
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool SocketClient::Init()
{
    if(WSAStartup(0x202, (WSADATA*)&buff[0]))
    {
        URHO3D_LOGERRORF("Winsock not initialized with error %d", WSAGetLastError());
        return false;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock == INVALID_SOCKET)
    {
        URHO3D_LOGERRORF("socket() error %d", WSAGetLastError());
        return false;
    }

    return true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool SocketClient::Connect(const char *address, u_short port)
{
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(port);

    if(inet_addr(address) != INADDR_NONE)
    {
        destAddr.sin_addr.s_addr = inet_addr(address);
    }
    else
    {
        URHO3D_LOGERRORF("Invalid address %s", address);
        return false;
    }

    if(connect(sock, (sockaddr*)&destAddr, sizeof(destAddr)))
    {
        URHO3D_LOGERRORF("Connect error %d", WSAGetLastError());
        return false;
    }

    return true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SocketClient::Transmit(const char *data, int size)
{
    send(sock, data, size, 0);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
int SocketClient::Recieve(char *buffer, int sizeBuffer)
{
    return recv(sock, buffer, sizeBuffer, 0);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SocketClient::Close()
{

}
