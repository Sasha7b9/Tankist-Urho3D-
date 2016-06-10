#ifdef WIN32

#include <stdafx.h>

#endif

#ifndef WIN32

#pragma warning(push)
#pragma warning(disable:4365)
#pragma warning(disable:4625)
#pragma warning(disable:4626)
#pragma warning(disable:4640)

#include "defines.h"

#include <Urho3D/Network/NetworkEvents.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Variant.h>
#include <Urho3D/Math/StringHash.h>
#include <Urho3D/Container/Str.h>
#include <Urho3D/IO/VectorBuffer.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/IO/VectorBuffer.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Network/Connection.h>

using Urho3D::Context;
using Urho3D::Object;
using Urho3D::StringHash;
using Urho3D::VariantMap;
using Urho3D::String;
using Urho3D::VectorBuffer;
using Urho3D::File;
using Urho3D::PODVector;
using Urho3D::VectorBuffer;
using Urho3D::MemoryBuffer;
using Urho3D::Connection;

#include "Server.h"

#include <GlobalVars.h>

#pragma warning(pop)

#endif



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Server::Server(Context *context) : Object(context)
{
    SubscribeToEvent(Urho3D::E_CLIENTCONNECTED, URHO3D_HANDLER(Server, HandleClientConnected));
    SubscribeToEvent(Urho3D::E_CLIENTDISCONNECTED, URHO3D_HANDLER(Server, HandleClientDisconnectd));
    SubscribeToEvent(Urho3D::E_NETWORKMESSAGE, URHO3D_HANDLER(Server, HandleNetworkMessage));
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
    else if(msgID == MSG_LOAD_CPU)
    {
#ifndef WIN32
        uint numCPU = Urho3D::GetNumPhysicalCPUs();
        int rez = system("uptime > out.uptime");
        File file(gContext, "out.uptime", Urho3D::FILE_READ);
        Vector<String> list = file.ReadLine().Split(' ');
        file.Close();
        buffer.WriteFloat(rez == -1 ? 0.0f : (ToFloat(list[list.Size() - 3]) / (float)numCPU));
        connection->SendMessage(MSG_LOAD_CPU, true, true, buffer);
#endif
    }
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
