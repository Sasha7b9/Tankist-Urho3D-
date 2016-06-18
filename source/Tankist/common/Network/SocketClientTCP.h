#pragma once


class SocketClientTCP
{
public:
    SocketClientTCP();
    ~SocketClientTCP();

    enum TypeSocket
    {
        Socket_Synch,
        Socket_Asynch
    };

    // At type asynch funcOnRecieve has to be nullptr
    bool Init(TypeSocket type, pFuncVpVpVI funcOnRecieve, void *clientTCP);
    bool Connect(const char *address, uint16 port);
    void Transmit(void *data, int size);
    int Recieve(char *buffer, int sizeBuffer);
    void Close();

private:
    char        buff[1024];
    std::thread *t = nullptr;
    TypeSocket  type;
    bool        run = true;
    void        *clientTCP;
    int         sockClient;     // Using for Windows - (SOCKET)sockClient
    sockaddr_in destAddr;
    pFuncVpVpVI funcOnRecieve;
};
