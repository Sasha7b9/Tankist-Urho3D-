#pragma once


#include "Socket.h"


class NewServer
{
public:
    NewServer();

    void Init(pFuncVV funcOnConnect, pFuncVV funcOnDisconnect, pFuncVU8pCI funcOnRecieve);
    bool Bind(u_short port);
    bool SendMessage(uint8 numMessage, char* data, int size);
    void Close();

private:
    SocketServerTCP socket;
};
