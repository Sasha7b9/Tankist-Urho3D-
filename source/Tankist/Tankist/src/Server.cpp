#include <stdafx.h>


#include "Server.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Server::Server(Context *context) : Object(context)
{
    SubscribeToEvent(E_CLIENTCONNECTED, URHO3D_HANDLER(Server, HandleClientConnected));
    SubscribeToEvent(E_CLIENTDISCONNECTED, URHO3D_HANDLER(Server, HandleClientDisconnectd));
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::Start(unsigned short port)
{
    gNetwork->StartServer(port);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleClientConnected(StringHash, VariantMap &eventData)
{
    using namespace Urho3D::ClientConnected;

    Connection *newConnection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());

    newConnection->SetScene(gScene);

    static int counter = 0;
    URHO3D_LOGINFOF("Client is connected %d", counter++);

    VariantMap &eData = GetEventDataMap();
    eData[NewConnection::P_CONNECT] = newConnection;

    SendEvent(E_NEWCONNECTION, eData);

    char message[] = "Tankist WaT server";
    newConnection->SendMessage(0, true, true, VectorBuffer(message, (uint)strlen(message)));
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleClientDisconnectd(StringHash, VariantMap &eventData)
{
    using namespace Urho3D::ClientDisconnected;

    VariantMap &eData = GetEventDataMap();
    eData[CloseConnection::P_CONNECT] = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    SendEvent(E_CLOSECONNECTION, eData);
}
