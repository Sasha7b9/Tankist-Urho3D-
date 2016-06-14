#include <stdafx.h>


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
void Server::HandleClientDisconnectd(StringHash, VariantMap &eventData)
{
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

    URHO3D_LOGINFOF("Chat message : %s", msg.CString());
}
