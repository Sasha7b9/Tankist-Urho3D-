#pragma once


#include "Network/Socket.h"


class NetworkThread : public Thread
{
public:

    enum State
    {
        Init,
        DownloadFiles,
        ConnectClose
    };

    virtual void ThreadFunction();

    State GetState()
    {
        return state;
    }

    float GetPassedTime()
    {
        return gTime->GetElapsedTime() - startTime;
    }

    float GetPercents()
    {
        return percents;
    }

    float GetSpeed()
    {
        return speed;
    }

    float GetElapsedTime()
    {
        return elapsedTime;
    }

    int GetBytesAll()
    {
        return bytesAll;
    }

    int GetBytesRecieved()
    {
        return bytesRecieved;
    }

    String GetCurrentFile()
    {
        return currentFile;
    }

private:
    State state = Init;
    //SOCKET sock;
    SocketClientTCP socket;
    float startTime = 0.0f;
    float percents = 0.0f;
    float speed = 0.0f;
    float elapsedTime = 0.0f;
    int bytesAll = 0;
    int bytesRecieved = 0;
    String currentFile;

    int GetFile(const char *nameIn, const char *nameOut = 0);
    void SendToSocket(const String &message);
};