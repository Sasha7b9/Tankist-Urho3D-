#pragma once


#include "Vehicle.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    void HandleCloseConnection(StringHash eventType, VariantMap &eventData);
    void HandleServerConnected(StringHash eventType, VariantMap &eventData);
    void HandleServerDisconnected(StringHash eventType, VariantMap &eventData);
    void HandleConnectFailed(StringHash eventType, VariantMap &eventData);
    void HandleNetworkMessage(StringHash eventType, VariantMap &eventData);
    void HandleClientConnected(StringHash eventType, VariantMap &eventData);
    void HandleClientDisconnected(StringHash eventType, VariantMap &eventData);
    void HandleClientIdentity(StringHash eventType, VariantMap &eventData);
    void HandleClientSceneLoaded(StringHash eventType, VariantMap &eventData);
    void HandleNetworkUpdate(StringHash eventType, VariantMap &eventData);
    void HandleNetworkUpdateSent(StringHash eventType, VariantMap &eventData);
    void HandleNetworkSceneLoadFailed(StringHash eventType, VariantMap &eventData);
    void HandleRemoteEventData(StringHash eventType, VariantMap &eventData);

    void SendMessageChat(const String &msg);

    int numClients = 0;

    Server(Server const&) : Object(nullptr) {};
    Server operator=(Server const&) {};
};
