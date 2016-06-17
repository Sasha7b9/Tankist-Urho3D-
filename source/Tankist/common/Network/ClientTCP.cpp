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
static void ProcessingNextByte(uint8 byte)
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
                
            }
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void CallbackOnRecieve(void *buffer, int sizeBuffer)
{
    uint8 *pointer = (uint8*)buffer;

    for(int i = 0; i < sizeBuffer; i++)
    {
        ProcessingNextByte(*pointer);
        pointer++;
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ClientTCP::ClientTCP()
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void ClientTCP::Init(pFuncVU8pVI /*funcOnRecieve*/)
{

    socket.Init(CallbackOnRecieve);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool ClientTCP::Connect(const char * /*address*/, uint16 /*port*/)
{
    return false;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void ClientTCP::SendMessage(uint8 /*numMessage*/, void* /*data*/, uint /*size*/)
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void ClientTCP::Close()
{
    socket.Close();
}
