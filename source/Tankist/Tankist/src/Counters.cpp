/* (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by */
#include <stdafx.h>
#include "Counters.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Counters::AddClientIn(int numBytes)
{
    bytesClientIn += numBytes;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Counters::AddClientOut(int numBytes)
{
    bytesClientOut += numBytes;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Counters::AddServerIn(int numbytes)
{
    bytesServerIn += numbytes;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Counters::AddServerOut(int numBytes)
{
    bytesServerOut += numBytes;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Counters::Update()
{
    static float prevTime = 0.0f;
    if(gTime->GetElapsedTime() - prevTime >= 1.0f)
    {
        prevTime = gTime->GetElapsedTime();

        speedClientIn = bytesClientIn;
        speedClientOut = bytesClientOut;
        speedServerIn = bytesServerIn;
        speedServerOut = bytesServerOut;

        bytesClientIn = bytesClientOut = bytesServerIn = bytesServerOut = 0;
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
int Counters::GetClientIn()
{
    return speedClientIn;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
int Counters::GetClientOut()
{
    return speedClientOut;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
int Counters::GetServerIn()
{
    return speedServerIn;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
int Counters::GetServerOut()
{
    return speedServerOut;
}
