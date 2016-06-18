#include <stdafx.h>


#include "Client.h"


//---------------------------------------------------------------------------------------------------------------------------------------------------
Client::Client(Context *context) : Object(context)
{
    gNetwork->RegisterRemoteEvent(E_CLIENTOBJECTID);
    SubscribeToEvent(E_CLIENTOBJECTID, URHO3D_HANDLER(Client, HandleClientObjectID));

    SubscribeToEvent(Urho3D::E_SERVERCONNECTED, URHO3D_HANDLER(Client, HandleServerConnected));
    SubscribeToEvent(Urho3D::E_SERVERDISCONNECTED, URHO3D_HANDLER(Client, HandleServerDisconnected));
    SubscribeToEvent(Urho3D::E_CONNECTFAILED, URHO3D_HANDLER(Client, HandleConnectFailed));
    SubscribeToEvent(Urho3D::E_CLIENTCONNECTED, URHO3D_HANDLER(Client, HandleClientObjectID));
    SubscribeToEvent(Urho3D::E_CLIENTDISCONNECTED, URHO3D_HANDLER(Client, HandleClientDisconnected));
    SubscribeToEvent(Urho3D::E_CLIENTIDENTITY, URHO3D_HANDLER(Client, HandleClientIdentity));
    SubscribeToEvent(Urho3D::E_CLIENTSCENELOADED, URHO3D_HANDLER(Client, HandleClientSceneLoaded));
    SubscribeToEvent(Urho3D::E_NETWORKMESSAGE, URHO3D_HANDLER(Client, HandleNetworkMessage));
    //SubscribeToEvent(Urho3D::E_NETWORKUPDATE, URHO3D_HANDLER(Client, HandleNetworkUpdate));
    //SubscribeToEvent(Urho3D::E_NETWORKUPDATESENT, URHO3D_HANDLER(Client, HandleNetworkUpdateSent));
    SubscribeToEvent(Urho3D::E_NETWORKSCENELOADFAILED, URHO3D_HANDLER(Client, HandleNetworkSceneLoadFailed));
    SubscribeToEvent(Urho3D::E_REMOTEEVENTDATA, URHO3D_HANDLER(Client, HandleRemoteEventData));
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
void Client::HandleServerDisconnected(StringHash, VariantMap &)
{
    LOG_INFOF("%s", __FUNCTION__);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Client::HandleConnectFailed(StringHash, VariantMap &)
{
    LOG_INFOF("%s", __FUNCTION__);
    float timeStart = gTime->GetElapsedTime();
    ConnectToServer();
    static int count = 0;
    LOG_ERRORF("Failed connection %d, time %f ms", count++, (gTime->GetElapsedTime() - timeStart) * 1000.0f);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Client::HandleNetworkMessage(StringHash, VariantMap &eventData)
{
    using namespace Urho3D::NetworkMessage;

    int msgID = eventData[P_MESSAGEID].GetInt();

    const PODVector<uint8> &data = eventData[P_DATA].GetBuffer();
    MemoryBuffer msg(data);

    if(msgID == MSG_PING)
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
void Client::TranslateMessage(const String &message)
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


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Client::HandleClientConnected(StringHash, VariantMap &)
{
    LOG_INFOF("%s", __FUNCTION__);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Client::HandleClientDisconnected(StringHash, VariantMap &)
{
    LOG_INFOF("%s", __FUNCTION__);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Client::HandleClientIdentity(StringHash, VariantMap &)
{
    LOG_INFOF("%s", __FUNCTION__);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Client::HandleClientSceneLoaded(StringHash, VariantMap &)
{
    LOG_INFOF("%s", __FUNCTION__);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Client::HandleNetworkUpdate(StringHash, VariantMap &)
{
    LOG_INFOF("%s", __FUNCTION__);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Client::HandleNetworkUpdateSent(StringHash, VariantMap &)
{
    LOG_INFOF("%s", __FUNCTION__);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Client::HandleNetworkSceneLoadFailed(StringHash, VariantMap &)
{
    LOG_INFOF("%s", __FUNCTION__);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Client::HandleRemoteEventData(StringHash, VariantMap &)
{
    LOG_INFOF("%s", __FUNCTION__);
}
