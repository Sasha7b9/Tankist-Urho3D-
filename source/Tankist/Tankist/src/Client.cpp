#include <stdafx.h>


#include "Client.h"


//---------------------------------------------------------------------------------------------------------------------------------------------------
Client::Client(Context *context) : Object(context)
{
    SubscribeToEvent(E_CLIENTOBJECTID, URHO3D_HANDLER(Client, HandleClientObjectID));
    SubscribeToEvent(E_SERVERCONNECTED, URHO3D_HANDLER(Client, HandleServerConnected));
    SubscribeToEvent(E_CONNECTFAILED, URHO3D_HANDLER(Client, HandleConnectFiled));
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
    return gCamera->AttachToNode(gScene->GetNode(nodeID), Vector3::UP * 2 + Vector3::BACK * 2);
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
