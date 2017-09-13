/* (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by */
#include <stdafx.h>
#include "ClientTCP.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const int SIZE_BUFFER = 1024 * 10;
static uint8 buffer[SIZE_BUFFER];
static StateRecieve stateRecieve = WAIT_MSG;
static uint8 typeMessage = 0;
static int recvBytes = 0;
static BitSet32 lengthBuffer;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void ProcessingNextByte(void *clientTCP, uint8 byte)
{
    if(stateRecieve == WAIT_MSG)
    {
        typeMessage = byte;
        recvBytes = 1;
        stateRecieve = RECIEVE_MSG;
        lengthBuffer.data32 = 0;
    }
    else if(stateRecieve == RECIEVE_MSG)
    {
        if(recvBytes < 5)
        {
            lengthBuffer.b[recvBytes - 1] = byte;
            recvBytes++;
            if(recvBytes == 5 && lengthBuffer.data32 == 0)
            {
                ((ClientTCP*)clientTCP)->FuncOnRecieve(typeMessage, buffer, 0);
                stateRecieve = WAIT_MSG;
            }
        }
        else
        {
            buffer[recvBytes - 5] = byte;
            recvBytes++;

            if(recvBytes - 5 == lengthBuffer.data32)
            {
                ((ClientTCP*)clientTCP)->FuncOnRecieve(typeMessage, buffer, lengthBuffer.data32);
                stateRecieve = WAIT_MSG;
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void CallbackOnRecieve(void *clientTCP, void *buffer_, int sizeBuffer)
{
    uint8 *pointer = (uint8*)buffer_;

    for(int i = 0; i < sizeBuffer; i++)
    {
        ProcessingNextByte(clientTCP, *pointer);
        pointer++;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ClientTCP::ClientTCP()
{

}

//---------------------------------------------------------------------------------------------------------------------------------------------------
bool ClientTCP::Init(pFuncVU8pVI funcOnRecieve)
{
    FuncOnRecieve = funcOnRecieve;
    return socket.Init(SocketClientTCP::Socket_Asynch, CallbackOnRecieve, this);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
bool ClientTCP::Connect(const char *address, uint16 port)
{
    return socket.Connect(address, port);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void ClientTCP::SendMessage(uint8 numMessage, void *data, uint size)
{
    socket.Transmit((void*)&numMessage, 1);
    socket.Transmit((void*)&size, 4);
    socket.Transmit(data, (int)size);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void ClientTCP::Close()
{
    socket.Close();
}
