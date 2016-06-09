#include <stdafx.h>


#include "NetworkThread.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct FileInfo
{
    FileInfo(String c = "", uint s = 0U) : crc32(c), size(s) {};
    String crc32;
    uint size;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void ReadListFiles(char *nameFile, HashMap<String, FileInfo> &map)
{
    map.Clear();

    File file(gContext, nameFile, Urho3D::FILE_READ);

    while(!file.IsEof())
    {
        String str = file.ReadLine();

        Vector<String> data = str.Split(' ');

        if (data.Size() > 3)
        {
            data[0] += ' ';
            data[0] += data[1];
            data[1] = data[2];
            data[2] = data[3];
        }

        FileInfo info = {data[1], Urho3D::ToUInt(data[2])};

        map[data[0]] = info;
    }

    file.Close();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
// ourFiles - existing files
// newFiles - files on server
// listDownloading - files for download
// return size downloading files
static int PrepareListDownloading(HashMap<String, FileInfo> ourFiles, HashMap<String, FileInfo> newFiles, Vector<String> &listDownloading)
{
//    uint numOurFiles = ourFiles.Size();

    uint size = 0U;

    for(HashMap<String, FileInfo>::Iterator i = newFiles.Begin(); i != newFiles.End(); i++)
    {
        String name = i->first_;
        FileInfo info = ourFiles[name];
        if(info.crc32 != i->second_.crc32)
        {
            size += i->second_.size;
            listDownloading.Push(i->first_);
        }
    }

    return (int)size;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void NetworkThread::ThreadFunction()
{
    static char buff[1024];

    if(WSAStartup(0x202, (WSADATA*)&buff[0]))
    {
        URHO3D_LOGERRORF("Winsock not initialized with error %d", WSAGetLastError());
    }

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

    // Get information about new files
    GetFile("files.txt", "files_new.txt");

    HashMap<String, FileInfo> newFiles;

    ReadListFiles("files_new.txt", newFiles);

    // Get information about our files
    HashMap<String, FileInfo> ourFiles;

    ReadListFiles("files.txt", ourFiles);

    // Prepare list files for downloading
    Vector<String> downloadingFiles;

    bytesAll = PrepareListDownloading(ourFiles, newFiles, downloadingFiles);

    // Download new files
    state = DownloadFiles;

    bytesRecieved = 0;

    startTime = gTime->GetElapsedTime();

    for(uint i = 0; i < downloadingFiles.Size(); i++)
    {
        String nameFile = downloadingFiles[i];
        currentFile = nameFile;
        bytesRecieved += GetFile(nameFile.CString());
        percents = ((float)bytesRecieved / bytesAll * 100.0f);
        speed = bytesRecieved / (gTime->GetElapsedTime() - startTime);
        elapsedTime = (bytesAll - bytesRecieved) / speed;
    }

    SendToSocket("close_connection");

    gFileSystem->Delete("files.txt");
    gFileSystem->Delete("files_new.txt");

    state = ConnectClose;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void CreateDirIfAbsent(const char *name)
{
    uint pos = String(name).FindLast('/');

    if(pos == String::NPOS)
    {
        return;
    }

    String nameDir = String(name).Substring(0, pos);

    if(!gFileSystem->DirExists(nameDir))
    {
        gFileSystem->CreateDir(nameDir);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
int NetworkThread::GetFile(const char *nameIn, const char *nameOut)
{
    nameOut = nameOut ? nameOut : nameIn;

    String name = nameIn;
    name.Replace('\\', '/');

    static char buff[1025];

    SendToSocket(String("get_file_size ") + name);

    uint numBytes = (uint)recv(sock, buff, sizeof(buff) - 1, 0);

    buff[numBytes] = '\0';

    int size = atoi(buff);

    SendToSocket(String("get_file ") + name);

    CreateDirIfAbsent(nameOut);

    File file(gContext, nameOut, Urho3D::FILE_WRITE);

    file.Flush();

    int bytesRecv = 0;

    while(bytesRecv < size)
    {
        uint numBytes = (uint)recv(sock, buff, 1024, 0);
        bytesRecv += numBytes;
        file.Write(buff, numBytes);
    }

    file.Close();

    return size;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void NetworkThread::SendToSocket(const String &message)
{
    send(sock, message.CString(), (int)strlen(message.CString()), 0);
}
