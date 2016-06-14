#pragma once


#include "Network/NewClient.h"
#include "Network/NewServer.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ChatClient
{
public:
    void Connect(char *address, int port, pFuncVpC funcOnMessage);
    void SendMessage(char *message);

private:
    NewClient client;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ChatServer
{
public:
    void Listen(int port);

private:
    NewServer server;

    struct DataClient
    {
        int clientID;
        char *address;
        int port;
    };

    HashMap<int, DataClient> clients;
};
