#include <stdafx.h>


#include "Server.h"


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

    Connection *connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());

    int msgID = eventData[P_MESSAGEID].GetInt();

    const PODVector<uint8> &data = eventData[P_DATA].GetBuffer();
    MemoryBuffer msg(data);

    VectorBuffer buffer;

    if (msgID == MSG_CHAT)
    {
        String text = msg.ReadString();
        SendMessageChat(connection->GetAddress() + " : " + text);
    }
#ifndef WIN32
    else if(msgID == MSG_PING)
    {
        int rez = system(String(String("ping ") + connection->GetAddress() + String(" -c 1 > out.ping")).CString());
        File file(gContext, "out.ping", Urho3D::FILE_READ);
        Vector<String> list = file.ReadLine().Split(' ');
        file.Close();

        int i = 0;
        for(; i < list.Size(); i++)
        {
            if(list[i] == "ms")
            {
                break;
            }
        }

        URHO3D_LOGINFOF("i ms = %d, rez = %d", i, rez);

        if(rez != -1 && i != 0)
        {
            Vector<String> l = list[i - 1].Split('=');
            buffer.WriteFloat(ToFloat(l[l.Size() - 1]));
            connection->SendMessage(MSG_PING, true, true, buffer);
        }
    }
    else if(msgID == MSG_LOAD_CPU)
    {
        uint numCPU = Urho3D::GetNumPhysicalCPUs();
        int rez = system("uptime > out.uptime");
        File file(gContext, "out.uptime", Urho3D::FILE_READ);
        Vector<String> list = file.ReadLine().Split(' ');
        file.Close();
        buffer.WriteFloat(rez == -1 ? 0.0f : (ToFloat(list[list.Size() - 3]) / (float)numCPU));
        connection->SendMessage(MSG_LOAD_CPU, true, true, buffer);
    }
#endif
    else if(msgID == MSG_NUM_CLIENTS)
    {
        buffer.WriteInt(numClients);
        connection->SendMessage(MSG_NUM_CLIENTS, true, true, buffer);
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
