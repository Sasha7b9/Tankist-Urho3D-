#include <stdafx.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Server::Server(Context *context) : Object(context)
{
    gNetwork->RegisterRemoteEvent(E_CLIENTOBJECTID);
    SubscribeToEvent(E_CLOSECONNECTION, URHO3D_HANDLER(Server, HandleCloseConnection));

    SubscribeToEvent(Urho3D::E_CLIENTCONNECTED, URHO3D_HANDLER(Server, HandleClientConnected));
    SubscribeToEvent(Urho3D::E_SERVERCONNECTED, URHO3D_HANDLER(Server, HandleServerConnected));
    SubscribeToEvent(Urho3D::E_SERVERDISCONNECTED, URHO3D_HANDLER(Server, HandleServerDisconnected));
    SubscribeToEvent(Urho3D::E_CONNECTFAILED, URHO3D_HANDLER(Server, HandleConnectFailed));
    SubscribeToEvent(Urho3D::E_CLIENTDISCONNECTED, URHO3D_HANDLER(Server, HandleClientDisconnected));
    SubscribeToEvent(Urho3D::E_CLIENTIDENTITY, URHO3D_HANDLER(Server, HandleClientIdentity));
    SubscribeToEvent(Urho3D::E_CLIENTSCENELOADED, URHO3D_HANDLER(Server, HandleClientSceneLoaded));
//    SubscribeToEvent(Urho3D::E_NETWORKMESSAGE, URHO3D_HANDLER(Server, HandleNetworkMessage));
//    SubscribeToEvent(Urho3D::E_NETWORKUPDATE, URHO3D_HANDLER(Server, HandleNetworkUpdate));
//    SubscribeToEvent(Urho3D::E_NETWORKUPDATESENT, URHO3D_HANDLER(Server, HandleNetworkUpdateSent));
    SubscribeToEvent(Urho3D::E_NETWORKSCENELOADFAILED, URHO3D_HANDLER(Server, HandleNetworkSceneLoadFailed));
    SubscribeToEvent(Urho3D::E_REMOTEEVENTDATA, URHO3D_HANDLER(Server, HandleRemoteEventData));
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::Start(unsigned short port)
{
    gNetwork->StartServer(port);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleClientConnected(StringHash, VariantMap &eventData)
{
    LOG_INFO1("%s", __FUNCTION__);

    using namespace Urho3D::ClientConnected;

    Connection *newConnection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());

    newConnection->SetScene(gScene);

    Vehicle* vehicle = gGame->ClientConnected(newConnection);

    VariantMap remoteEventData;
    remoteEventData[P_ID] = vehicle->towerID;
    newConnection->SendRemoteEvent(E_CLIENTOBJECTID, true, remoteEventData);

    numClients++;

    SendMessageChat(newConnection->GetAddress() + " enter");
}

static String prevAddress;


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleNetworkMessage(StringHash, VariantMap &eventData)
{
    LOG_INFO1("%s", __FUNCTION__);

    using namespace Urho3D::NetworkMessage;

    Connection *connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());

    int msgID = eventData[P_MESSAGEID].GetInt();

    const PODVector<uint8> &data = eventData[P_DATA].GetBuffer();
    MemoryBuffer msg(data);

    VectorBuffer buffer;

    if (msgID == MSG_CHAT)
    {
        String text = msg.ReadString();
        String address = connection->GetAddress();

        if(prevAddress != address)
        {
            SendMessageChat(address + ": " + text);
            prevAddress = address;
        }
        else
        {
            SendMessageChat(text);
        }
    }
    else if(msgID == MSG_PING)
    {
        connection->SendMessage(MSG_PING, true, true, buffer);
    }
    else if(msgID == MSG_LOAD_CPU)
    {
#ifndef WIN32
        // uint numCPU = Urho3D::GetNumPhysicalCPUs(); On virtual server not work
        int rez = system("uptime > out.uptime");
        File file(gContext, "out.uptime", Urho3D::FILE_READ);
        Vector<String> list = file.ReadLine().Split(' ');
        file.Close();
        buffer.WriteFloat(rez == -1 ? 0.0f : (ToFloat(list[list.Size() - 3]) / /* (float)numCPU */ 2.0f));
        connection->SendMessage(MSG_LOAD_CPU, true, true, buffer);
#endif
    }
    else if(msgID == MSG_NUM_CLIENTS)
    {
        buffer.WriteInt(numClients);
        connection->SendMessage(MSG_NUM_CLIENTS, true, true, buffer);
    }
    else if(msgID == MSG_SERVER_SPEED)
    {
        Vector<SharedPtr<Connection>> connections = gNetwork->GetClientConnections();

        float speedIn = 0.0f;
        float speedOut = 0.0f;

        for(Connection *conn : connections)
        {
            speedIn += conn->GetBytesInPerSec();
            speedOut += conn->GetBytesOutPerSec();
        }

        buffer.WriteFloat(speedIn);
        buffer.WriteFloat(speedOut);
        connection->SendMessage(MSG_SERVER_SPEED, true, true, buffer);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleClientDisconnected(StringHash, VariantMap &eventData)
{
    LOG_INFO1("%s", __FUNCTION__);

    using namespace Urho3D::ClientDisconnected;

    VariantMap &eData = GetEventDataMap();
    eData[CloseConnection::P_CONNECT] = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    SendEvent(E_CLOSECONNECTION, eData);

    // Send message in chat

    Connection *conn = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());

    numClients--;

    SendMessageChat(conn->GetAddress() + " leave");
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

    gChatLog->WriteMessage(msg);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleCloseConnection(StringHash, VariantMap &eventData)
{
    LOG_INFO1("%s", __FUNCTION__);

    Connection *connection = static_cast<Connection*>(eventData[CloseConnection::P_CONNECT].GetPtr());

    gGame->ClientDisconnected(connection);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleServerConnected(StringHash, VariantMap &)
{
    LOG_INFO1("%s", __FUNCTION__);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleServerDisconnected(StringHash, VariantMap &)
{
    LOG_INFO1("%s", __FUNCTION__);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleConnectFailed(StringHash, VariantMap &)
{
    LOG_INFO1("%s", __FUNCTION__);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleClientIdentity(StringHash, VariantMap &)
{
    LOG_INFO1("%s", __FUNCTION__);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleClientSceneLoaded(StringHash, VariantMap &)
{
    LOG_INFO1("%s", __FUNCTION__);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleNetworkUpdate(StringHash, VariantMap &)
{
    LOG_INFO1("%s", __FUNCTION__);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleNetworkUpdateSent(StringHash, VariantMap &)
{
    LOG_INFO1("%s", __FUNCTION__);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleNetworkSceneLoadFailed(StringHash, VariantMap &)
{
    LOG_INFO1("%s", __FUNCTION__);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleRemoteEventData(StringHash, VariantMap &)
{
    LOG_INFO1("%s", __FUNCTION__);
}
