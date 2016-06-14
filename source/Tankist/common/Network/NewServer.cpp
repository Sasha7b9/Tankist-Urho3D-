#include "NewServer.h"


#include <Urho3D/Container/HashMap.h>


using Urho3D::HashMap;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const int SIZE_BUFFER = 1024;

enum StateRecieve
{
    WAIT_MSG,
    RECIEVE_MSG
};

//                      type msg  length
//  Structure message: |    0    |   1   | ...............
//  Samle              |    3    |   10  | b0 | b1 | b2 | b3 | b4 | b5 | b6 | b7 | b8 | b9 |


struct ClientData
{
    ClientData(int num, NewServer *serv) : numClient(num), server(serv), recvBytes(0), stateRecieve(WAIT_MSG) {};
    uint8 data[SIZE_BUFFER];
    int numClient;
    StateRecieve stateRecieve;
    int recvBytes;              // Number all accepted bytes - type message + 1 + length buffer
    int lengthBuffer;           // length message without type and another one byte  (10 in sample)
    uint8 typeMessage;
    NewServer *server;
};


static HashMap<NewServer*, HashMap<int, ClientData>> datas;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void CallbackOnConnect(NewServer *server, int numClient, char *address, int port)
{


    datas[server] = ServerData(numClient, server);
    server->funcOnConnect(numClient, address, port);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void ProcessNextByte(SocketData &data, uint8 b)
{
    if(data.stateRecieve == WAIT_MSG)
    {
        data.typeMessage = b;
        data.recvBytes = 1;
        data.stateRecieve = RECIEVE_MSG;
    }
    else if(data.stateRecieve == RECIEVE_MSG)
    {
        if(data.recvBytes == 1)
        {
            data.lengthBuffer = b;
            data.recvBytes = 2;

            if(data.lengthBuffer == 0)
            {
                data.server->funcOnRecieve(data.numClient, data.typeMessage, data.data, 0);
                data.stateRecieve = WAIT_MSG;
            }
        }
        else
        {
            data.data[data.recvBytes - 2] = b;
            data.recvBytes++;

            if(data.recvBytes - 2 == data.lengthBuffer)
            {
                data.server->funcOnRecieve(data.numClient, data.typeMessage, data.data, data.lengthBuffer);
                data.stateRecieve = WAIT_MSG;
            }
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void CallbackOnReceive(NewServer *server, int numClient, uint8 *buffer, int size)
{
    ServerData &servData = datas[server];
    SocketData &sockData = servData.datas[numClient];

    uint8 *pointer = buffer;

    for(int i = 0; i < size; i++)
    {
        ProcessNextByte(sockData, *pointer);
        pointer++;
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void CallbackOnDisconnect(NewServer *server, int numClient)
{
    ServerData &servData = datas[server];
    servData.datas.Erase(numClient);

    server->funcOnDisconnect(numClient);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
NewServer::NewServer()
{
    socket = new SocketServerTCP();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void NewServer::Init(ServerParam *servParam)
{
    socketParam.funcOnConnect = servParam->funcOnConnect;
    socketParam.funcOnDisconnect = servParam->funcOnDisconnect;
    socketParam.funcOnReceive = (pFuncVIpCI)(&NewServer::FuncOnRecieve);
    socketParam.sizeBuffer = 1024;

    //funcOnReceive = servParam->funcOnReceive;

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


//---------------------------------------------------------------------------------------------------------------------------------------------------
void NewServer::FuncOnRecieve(int numClient, char* data, int size)
{

}
