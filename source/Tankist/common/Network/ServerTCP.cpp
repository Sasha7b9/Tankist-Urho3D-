#include <stdafx.h>


#include "ServerTCP.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const int SIZE_BUFFER = 1024 * 10;

//                      type msg  length
//  Structure message: |    0    |   1   |   2   |   3   |   4   |  ...............
//  Samle              |    3    |   10  |   0   |   0   |   0   |  b0 | b1 | b2 | b3 | b4 | b5 | b6 | b7 | b8 | b9 |


struct ClientData
{
    ClientData() : server(nullptr), stateRecieve(WAIT_MSG) {};
    ClientData(void *serv, int num) : numClient(num), server(serv), recvBytes(0), stateRecieve(WAIT_MSG) {};
    uint8 data[SIZE_BUFFER];
    int numClient;
    StateRecieve stateRecieve;
    int recvBytes;              // Number all accepted bytes - type message + 4 + length buffer
    int lengthBuffer;           // length message without type and another one byte  (10 in sample)
    uint8 typeMessage;
    void *server;
};


static HashMap<void*, HashMap<int, ClientData>> datas;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void CallbackOnConnect(void *server, int numClient, char *address, int port)
{
    if (datas.Find(server) == datas.End())      // If map for this server does not exist
    {
        HashMap<int, ClientData> newServer;
        datas[server] = newServer;              // Create new map for this server
    }

    datas[server][numClient] = ClientData(server, numClient);

    ((NewServer*)server)->param.funcOnConnect(numClient, address, port);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void ProcessNextByte(ClientData &data, uint8 b)
{
    if(data.stateRecieve == WAIT_MSG)
    {
        data.typeMessage = b;
        data.recvBytes = 1;
        data.stateRecieve = RECIEVE_MSG;
        data.lengthBuffer = 0;
    }
    else if(data.stateRecieve == RECIEVE_MSG)
    {
        if(data.recvBytes < 5)
        {
            data.lengthBuffer += ((int)b) << (data.recvBytes - 1);
            data.recvBytes++;
            if(data.recvBytes == 5 && data.lengthBuffer == 0)
            {
                ((NewServer*)data.server)->param.funcOnRecieve(data.numClient, data.typeMessage, data.data, 0);
                data.stateRecieve = WAIT_MSG;
            }
        }
        else
        {
            data.data[data.recvBytes - 2] = b;
            data.recvBytes++;

            if(data.recvBytes - 2 == data.lengthBuffer)
            {
                ((NewServer*)data.server)->param.funcOnRecieve(data.numClient, data.typeMessage, data.data, data.lengthBuffer);
                data.stateRecieve = WAIT_MSG;
            }
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void CallbackOnReceive(void *server, int numClient, void *buffer, int size)
{
    ClientData data = datas[server][numClient];

    uint8 *pointer = (uint8*)buffer;

    for(int i = 0; i < size; i++)
    {
        ProcessNextByte(data, *pointer);
        pointer++;
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void CallbackOnDisconnect(void *server, int numClient)
{
    datas[server].Erase(numClient);

    ((NewServer*)server)->param.funcOnDisconnect(numClient);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NewServer::NewServer()
{
    socket = new SocketServerTCP();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void NewServer::Init(ServerParam *servParam)
{
    param = *servParam;

    socketParam.funcOnConnect = CallbackOnConnect;
    socketParam.funcOnDisconnect = CallbackOnDisconnect;
    socketParam.funcOnReceive = CallbackOnReceive;
    socketParam.sizeBuffer = SIZE_BUFFER;
    socketParam.server = (void*)this;

    socket->Init(&socketParam);

    socket->Listen(servParam->port);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool NewServer::SendMessage(int numClient, uint8 typeMessage, char* data, int size)
{
    socket->Transmit((const char*)&numClient, 4);
    socket->Transmit((const char*)&typeMessage, 1);
    socket->Transmit(data, size);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void NewServer::Close()
{
    socket->Close();
}
