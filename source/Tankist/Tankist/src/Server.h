#pragma once


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
URHO3D_EVENT(E_NEWCONNECTION, NewConnection)
{
    URHO3D_PARAM(P_CONNECT, Connect);
};


URHO3D_EVENT(E_CLOSECONNECTION, CloseConnection)
{
    URHO3D_PARAM(P_CONNECT, Connect);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Server : public Object
{

#pragma warning(push)
#pragma warning(disable:4640)
    URHO3D_OBJECT(Server, Object)
#pragma warning(pop)

public:
    Server(Context *context);

    void Start(unsigned short port);

private:
    void HandleClientConnected(StringHash eventType, VariantMap &eventData);
    void HandleClientDisconnectd(StringHash eventType, VariantMap &eventData);
    void HandleNetworkMessage(StringHash eventType, VariantMap &eventData);

    void SendMessageChat(const String &msg);

    int numClients = 0;

    Server(Server const&) : Object(nullptr) {};
    Server operator=(Server const&) {};
};
