#pragma once


#include "Socket.h"


class NewClient
{
public:
    NewClient();

    void Init(pFuncVU8pCI funcOnRecieve);
    bool Connect(char *address, u_short port);
    bool SendMessage(uint8 numMessage, char* data, int size);
    void Close();

private:
    SocketClientTCP socket;
};
