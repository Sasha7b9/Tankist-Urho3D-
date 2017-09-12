#include <stdafx.h>
#include "ServerTCP.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const int SIZE_BUFFER = 1024 * 10;

//                      type msg  length
//  Structure message: |    0    |   1   |   2   |   3   |   4   |  ...............
//  Sample             |    3    |   10  |   0   |   0   |   0   |  b0 | b1 | b2 | b3 | b4 | b5 | b6 | b7 | b8 | b9 |


struct ClientData
{
    ClientData() : stateRecieve(WAIT_MSG), server(nullptr) {};
    ClientData(void *serv, SOCKET num) : numClient(num), stateRecieve(WAIT_MSG), recvBytes(0), server(serv) {};
    uint8 data[SIZE_BUFFER];
    SOCKET numClient;
    StateRecieve stateRecieve;
    int recvBytes;              // Number all accepted bytes - type message + 4 + length buffer
    BitSet32 lengthBuffer;      // length message without type and another one byte  (10 in sample)
    uint8 typeMessage;
    void *server;
};


static HashMap<void*, HashMap<SOCKET, ClientData>> datas;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void CallbackOnConnect(void *server, SOCKET numClient, char *address, uint16 port)
{
    if (datas.Find(server) == datas.End())      // If map for this server does not exist
    {
        HashMap<SOCKET, ClientData> newServer;
        datas[server] = newServer;              // Create new map for this server
    }

    datas[server][numClient] = ClientData(server, numClient);

    ((ServerTCP*)server)->param.funcOnConnect(numClient, address, port);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void ProcessNextByte(ClientData &data, uint8 b)
{
    if(data.stateRecieve == WAIT_MSG)
    {
        data.typeMessage = b;
        data.recvBytes = 1;
        data.stateRecieve = RECIEVE_MSG;
        data.lengthBuffer.data32 = 0;
    }
    else if(data.stateRecieve == RECIEVE_MSG)
    {
        if(data.recvBytes < 5)
        {
            data.lengthBuffer.b[data.recvBytes - 1] = b;
            data.recvBytes++;
            if(data.recvBytes == 5 && data.lengthBuffer.data32 == 0)
            {
                ((ServerTCP*)data.server)->param.funcOnRecieve(data.numClient, data.typeMessage, data.data, 0);
                data.stateRecieve = WAIT_MSG;
            }
        }
        else
        {
            data.data[data.recvBytes - 5] = b;
            data.recvBytes++;

            if(data.recvBytes - 5 == data.lengthBuffer.data32)
            {
                ((ServerTCP*)data.server)->param.funcOnRecieve(data.numClient, data.typeMessage, data.data, data.lengthBuffer.data32);
                data.stateRecieve = WAIT_MSG;
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void CallbackOnReceive(void *server, SOCKET numClient, void *buffer, int size)
{
    ClientData &data = datas[server][numClient];

    uint8 *pointer = (uint8*)buffer;

    for(int i = 0; i < size; i++)
    {
        ProcessNextByte(data, *pointer);
        pointer++;
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void CallbackOnDisconnect(void *server, SOCKET numClient)
{
    datas[server].Erase(numClient);

    ((ServerTCP*)server)->param.funcOnDisconnect(numClient);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ServerTCP::ServerTCP()
{
    socket = new SocketServerTCP();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
bool ServerTCP::Init(const ServerParam &servParam)
{
    param = servParam;

    socketParam.funcOnConnect = CallbackOnConnect;
    socketParam.funcOnDisconnect = CallbackOnDisconnect;
    socketParam.funcOnReceive = CallbackOnReceive;
    socketParam.sizeBuffer = SIZE_BUFFER;
    socketParam.server = (void*)this;

    if (!socket->Init(&socketParam))
    {
        LOG_ERROR("Can not init socket");
        return false;
    }

    if (!socket->Listen(servParam.port))
    {
        LOG_ERROR("Can not connect socket to port");
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void ServerTCP::SendMessage(SOCKET numClient, uint8 typeMessage, void* data, uint size)
{
    send(numClient, (char*)&typeMessage, 1, 0);
    send(numClient, (char*)&size, 4, 0);
    send(numClient, (char*)data, (int)size, 0);

    gCounters->AddServerOut(1 + 4 + (int)size);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void ServerTCP::Close()
{
    socket->Close();
}
