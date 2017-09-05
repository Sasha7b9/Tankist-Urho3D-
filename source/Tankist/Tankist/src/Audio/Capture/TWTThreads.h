#pragma once

#include <windows.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Signal
{
public:
    explicit Signal(int count = 1);
    ~Signal();

    void Trigger(int index = 0)
    {
        SetEvent(signalEvent[index]);
    }

    HANDLE GetEvetnHandle(int index = 0)
    {
        return signalEvent[index];
    }

    int Wait(int time = -1);

private:
    int signalCount;
    HANDLE signalEvent[4];
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Thread
{
public:
    typedef void ThreadProc(const Thread*, void *);

private:
    ThreadProc  *threadProc;
    void        *threadCookie;
    Signal      *threadSignal;
    volatile bool threadComplete;
};

