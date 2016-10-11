#include <stdafx.h>


#include "Client.h"
#include "Network/NetworkEvents.h"


//---------------------------------------------------------------------------------------------------------------------------------------------------
Client::Client(Context *context) : Object(context)
{
    gNetwork->RegisterRemoteEvent(E_CLIENTOBJECTID);
    SubscribeToEvent(E_CLIENTOBJECTID, URHO3D_HANDLER(Client, HandleClientObjectID));
    gNetwork->RegisterRemoteEvent(E_STRING_MESSAGE);
    SubscribeToEvent(E_STRING_MESSAGE, URHO3D_HANDLER(Client, HandleStringMessage));
    //gNetwork->RegisterRemoteEvent(E_SHOOT);

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
    towerID = 0;

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
        towerID = 0;
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Client::HandleClientObjectID(StringHash, VariantMap &eventData)
{
    towerID = eventData[P_ID_TOWER].GetUInt();
    trunkID = eventData[P_ID_TRUNK].GetUInt();
    caseID = eventData[P_ID_CASE].GetUInt();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Client::HandleStringMessage(StringHash, VariantMap& eventData)
{
    String var = eventData[P_STRING_VAR].GetString();
    String value = eventData[P_STRING_VALUE].GetString();

    gGameGUI->SetVar(var, value);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool Client::AttachCameraToNode()
{
    return gCamera->SetCameraMode(ModeCommander, gScene->GetNode(towerID));
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


        gTankist->SetBytesInPerSec(connection->GetBytesInPerSec() + gCounters->GetClientIn());
        gTankist->SetBytesOutPerSec(connection->GetBytesOutPerSec() + gCounters->GetClientOut());
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
    gPhysicsWorld = gScene->GetComponent<PhysicsWorld>();
    LOG_INFOF("gPhysicsWorld = %d", gPhysicsWorld);
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
