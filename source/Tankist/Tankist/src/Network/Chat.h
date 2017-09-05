#pragma once
#include "Network/ClientTCP.h"
#include "Network/ServerTCP.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Chat : public Object
{
#pragma warning(push)
#pragma warning(disable:4640)
    URHO3D_OBJECT(Chat, Object)
#pragma warning(pop)
public:

    enum Type
    {
        Chat_Client,
        Chat_Server
    };

    Chat(Context *context, Type type);

    // Functions server
    bool Listen(uint16 port);
    void SendToAll(uint8 typeMessage, const String &message);
    void SendToAllExcept(uint8 typeMessage, const String &message, SOCKET except);

    // Functions client
    bool Connect(const char *address, uint16 port);
    bool IsActive();
    void SetActive(bool active);
    void PressEnter();
    void AddMessage(const String &message);     // Show message on display
    void SendToServer(const String &message);
    void UpdateChat();
    void SendAudioData(void *buffer, uint sizeBuffer);

private:
    Type type;

    // For server
    ServerTCP server;

    // For client
    ClientTCP client;
    SharedPtr<Text> historyText;
    SharedPtr<LineEdit> messageEdit;
    Vector<String> messages;

    Chat(Chat const&) : Object(nullptr) {};
    Chat operator=(Chat const&) {};
};
