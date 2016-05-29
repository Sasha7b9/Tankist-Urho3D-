#include <stdafx.h>


#include "NetworkThread.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetworkThread::ThreadFunction()
{
    static char buff[1024];

    if(WSAStartup(0x202, (WSADATA*)&buff[0]))
    {
        URHO3D_LOGERRORF("Winsock not initialized with error %d", WSAGetLastError());
    }

    SOCKET sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock == INVALID_SOCKET)
    {
        URHO3D_LOGERRORF("socket() error %d", WSAGetLastError());
    }

    sockaddr_in destAddr;
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(UPDATE_PORT);

    if(inet_addr(UPDATE_ADDR) != INADDR_NONE)
    {
        destAddr.sin_addr.s_addr = inet_addr(UPDATE_ADDR);
    }
    else
    {
        URHO3D_LOGERROR("Invalid address");
    }

    if(connect(sock, (sockaddr*)&destAddr, sizeof(destAddr)))
    {
        URHO3D_LOGERRORF("Connect error %d", WSAGetLastError());
    }
    else
    {
        URHO3D_LOGINFO("Connect is ok!");
    }

    send(sock, "version", 7, 0);

    recv(sock, &buff[0], sizeof(buff) - 1, 0);

    URHO3D_LOGINFOF("new version is %s", buff);

    if(strcmp(buff, _version_) != 0)
    {

        send(sock, "get_size", 8, 0);

        recv(sock, &buff[0], sizeof(buff) - 1, 0);

        int size = atoi(buff) / 10;
        bytesAll = size;

        URHO3D_LOGINFOF("size new version %d", size);


        //--------------------------------------------------------------
        state = DownloadFile;

        bytesRecieved = 0;

        File file(gContext, "out.exe", Urho3D::FILE_WRITE);

        send(sock, "get_file", 8, 0);

        percents = 0.0f;

        startTime = gTime->GetElapsedTime();

        while(bytesRecieved < size)
        {
            uint numBytes = (uint)recv(sock, buff, 1024, 0);
            bytesRecieved += numBytes;
            file.Write(buff, numBytes);
            percents = ((float)bytesRecieved / size * 100.0f);
            speed = bytesRecieved / (gTime->GetElapsedTime() - startTime);
            elapsedTime = (size - bytesRecieved) / speed;
        }

        file.Close();

    }

    state = ConnectClose;
}
