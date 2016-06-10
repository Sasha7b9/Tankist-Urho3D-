#include <stdafx.h>


#include "Client.h"
#include "../../common/CommonDefines.h"


//---------------------------------------------------------------------------------------------------------------------------------------------------
Client::Client(Context *context) : Object(context)
{
    SubscribeToEvent(E_CLIENTOBJECTID, URHO3D_HANDLER(Client, HandleClientObjectID));
    SubscribeToEvent(E_SERVERCONNECTED, URHO3D_HANDLER(Client, HandleServerConnected));
    SubscribeToEvent(E_CONNECTFAILED, URHO3D_HANDLER(Client, HandleConnectFiled));
    SubscribeToEvent(Urho3D::E_NETWORKMESSAGE, URHO3D_HANDLER(Client, HandleNetworkMessage));
    gNetwork->RegisterRemoteEvent(E_CLIENTOBJECTID);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
Client::~Client()
{
    Disconnect();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool Client::ConnectToServer()
{
    nodeID = 0;

    return GetSubsystem<Network>()->Connect(gIPAddress, gNumPort, gScene);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Client::Disconnect()
{
    Connection *serverConnnection = gNetwork->GetServerConnection();
    if (serverConnnection)
    {
        serverConnnection->Disconnect();
        gScene->Clear(true, false);
        nodeID = 0;
        serverConnnection = false;
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Client::HandleClientObjectID(StringHash, VariantMap &eventData)
{
    nodeID = eventData[P_ID].GetUInt();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool Client::AttachCameraToNode()
{
    return gCamera->AttachToNode(gScene->GetNode(nodeID), Vector3::UP * 1.75f + Vector3::BACK);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Client::HandleServerConnected(StringHash, VariantMap &)
{
    serverIsConnected = true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Client::HandleConnectFiled(StringHash, VariantMap &)
{
    float timeStart = gTime->GetElapsedTime();
    ConnectToServer();
    static int count = 0;
    URHO3D_LOGINFOF("Filed connection %d, time %f ms", count++, (gTime->GetElapsedTime() - timeStart) * 1000.0f);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Client::HandleNetworkMessage(StringHash, VariantMap &eventData)
{
    using namespace Urho3D::NetworkMessage;

    int msgID = eventData[P_MESSAGEID].GetInt();

    if (msgID == MSG_CHAT)
    {
        const PODVector<uint8> &data = eventData[P_DATA].GetBuffer();
        MemoryBuffer msg(data);
        String text = msg.ReadString();
        chatMessages.Push(text);
        gTankist->UpdateMessages();
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Client::SendMessage(const String &message)
{
    VectorBuffer buffer;
    buffer.WriteString(message);
    gNetwork->GetServerConnection()->SendMessage(MSG_CHAT, true, true, buffer);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
String Client::GetStatistics()
{
    Connection *connection = gNetwork->GetServerConnection();



    return String("in = ") + String(connection->GetBytesInPerSec() / 1000.0f) + String(" kB/s\n") +
        String("out = ") + String(connection->GetBytesOutPerSec() / 1000.0f) + String(" kB/s");
}
