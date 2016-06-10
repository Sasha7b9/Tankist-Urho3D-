#include <stdafx.h>


#include "Server.h"
#include "../../common/CommonDefines.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Server::Server(Context *context) : Object(context)
{
    SubscribeToEvent(E_CLIENTCONNECTED, URHO3D_HANDLER(Server, HandleClientConnected));
    SubscribeToEvent(E_CLIENTDISCONNECTED, URHO3D_HANDLER(Server, HandleClientDisconnectd));
    SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(Server, HandleNetworkMessage));
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

    VariantMap &eData = GetEventDataMap();
    eData[NewConnection::P_CONNECT] = newConnection;

    SendEvent(E_NEWCONNECTION, eData);

    char message[] = "Tankist WaT server";
    newConnection->SendMessage(0, true, true, VectorBuffer(message, (uint)strlen(message)));

    numClients++;

    SendMessageChat("Connected " + newConnection->GetAddress() + ", all - " + String(numClients));
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleNetworkMessage(StringHash, VariantMap &eventData)
{
    using namespace Urho3D::NetworkMessage;

    Connection *newConnection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());

    int msgID = eventData[P_MESSAGEID].GetInt();

    if (msgID == MSG_CHAT)
    {
        const PODVector<uint8> &data = eventData[P_DATA].GetBuffer();
        MemoryBuffer msg(data);
        String text = msg.ReadString();

        SendMessageChat(newConnection->GetAddress() + " : " + text);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleClientDisconnectd(StringHash, VariantMap &eventData)
{
    using namespace Urho3D::ClientDisconnected;

    VariantMap &eData = GetEventDataMap();
    eData[CloseConnection::P_CONNECT] = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    SendEvent(E_CLOSECONNECTION, eData);

    // Send message in chat

    Connection *conn = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());

    numClients--;

    SendMessageChat("Disconnected " + conn->GetAddress() + ", all - " + String(numClients));
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::SendMessageChat(const String &msg)
{
    VectorBuffer buffer;
    buffer.WriteString(msg);
    Vector<SharedPtr<Connection>> connections = gNetwork->GetClientConnections();
    for (Connection *conn : connections)
    {
        conn->SendMessage(MSG_CHAT, true, true, buffer);
    }

    URHO3D_LOGINFOF("Chat message : %s", msg.CString());
}
