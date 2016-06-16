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
NewClient::NewClient()
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void NewClient::Init(pFuncVU8pVI funcOnRecieve)
{
    socket.Init(CallbackOnRecieve);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool NewClient::Connect(char *address, u_short port)
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool NewClient::SendMessage(uint8 numMessage, char* data, int size)
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void NewClient::Close()
{
    socket.Close();
}
