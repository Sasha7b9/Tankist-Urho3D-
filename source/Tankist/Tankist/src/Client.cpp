#include <stdafx.h>


#include "Client.h"


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

    const PODVector<uint8> &data = eventData[P_DATA].GetBuffer();
    MemoryBuffer msg(data);

    if (msgID == MSG_CHAT)
    {
        String text = msg.ReadString();
        chatMessages.Push(text);
        gTankist->UpdateMessages();
    }
    else if(msgID == MSG_PING)
    {
        gTankist->SetPing(gTime->GetElapsedTime() - timePing);
    }
    else if(msgID == MSG_LOAD_CPU)
    {
        float loadCPU = msg.ReadFloat();
        gTankist->SetLoadCPU(loadCPU);
    }
    else if(msgID == MSG_NUM_CLIENTS)
    {
        int numClients = msg.ReadInt();
        gTankist->SetNumClients(numClients);
    }
    else if(msgID == MSG_SERVER_SPEED)
    {
        float speedIn = msg.ReadFloat();
        float speedOut = msg.ReadFloat();
        gTankist->SetBytesInPerSecServer(speedIn);
        gTankist->SetBytesOutPerSecServer(speedOut);
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
void Client::RequestSystemInformation()
{
    Connection *connection = gNetwork->GetServerConnection();

    if(connection)
    {
        gNetwork->GetServerConnection()->SendMessage(MSG_PING, true, true, VectorBuffer());
        timePing = gTime->GetElapsedTime();
        gNetwork->GetServerConnection()->SendMessage(MSG_LOAD_CPU, true, true, VectorBuffer());
        gNetwork->GetServerConnection()->SendMessage(MSG_NUM_CLIENTS, true, true, VectorBuffer());
        gNetwork->GetServerConnection()->SendMessage(MSG_SERVER_SPEED, true, true, VectorBuffer());


        gTankist->SetBytesInPerSec(connection->GetBytesInPerSec());
        gTankist->SetBytesOutPerSec(connection->GetBytesOutPerSec());
    }
}
