/* (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by */
#include <stdafx.h>
#include "NetworkEvents.h"



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Server::Server(Context *context) : Object(context)
{
    //gNetwork->RegisterRemoteEvent(E_CLIENTOBJECTID);
    gNetwork->RegisterRemoteEvent(E_SHOOT);
    SubscribeToEvent(E_SHOOT, URHO3D_HANDLER(Server, HandleShoot));

    SubscribeToEvent(E_CLOSECONNECTION, URHO3D_HANDLER(Server, HandleCloseConnection));

    SubscribeToEvent(E_CLIENTCONNECTED, URHO3D_HANDLER(Server, HandleClientConnected));
    SubscribeToEvent(E_SERVERCONNECTED, URHO3D_HANDLER(Server, HandleServerConnected));
    SubscribeToEvent(E_SERVERDISCONNECTED, URHO3D_HANDLER(Server, HandleServerDisconnected));
    SubscribeToEvent(E_CONNECTFAILED, URHO3D_HANDLER(Server, HandleConnectFailed));
    SubscribeToEvent(E_CLIENTDISCONNECTED, URHO3D_HANDLER(Server, HandleClientDisconnected));
    SubscribeToEvent(E_CLIENTIDENTITY, URHO3D_HANDLER(Server, HandleClientIdentity));
    SubscribeToEvent(E_CLIENTSCENELOADED, URHO3D_HANDLER(Server, HandleClientSceneLoaded));
    SubscribeToEvent(E_NETWORKMESSAGE, URHO3D_HANDLER(Server, HandleNetworkMessage));
//    SubscribeToEvent(E_NETWORKUPDATE, URHO3D_HANDLER(Server, HandleNetworkUpdate));
//    SubscribeToEvent(E_NETWORKUPDATESENT, URHO3D_HANDLER(Server, HandleNetworkUpdateSent));
    SubscribeToEvent(E_NETWORKSCENELOADFAILED, URHO3D_HANDLER(Server, HandleNetworkSceneLoadFailed));
    SubscribeToEvent(E_REMOTEEVENTDATA, URHO3D_HANDLER(Server, HandleRemoteEventData));

    SubscribeToEvent(E_PHYSICSPOSTSTEP, URHO3D_HANDLER(Server, HandlePhysicsPostStep));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::Start(unsigned short port)
{
    gNetwork->StartServer(port);
}


static Vector<Node*> boxNodes;
static const String ATTR_TIME_DEATH = "TimeDeath";


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleShoot(StringHash, VariantMap& eventData)
{
    uint trunkID = eventData[P_ID_TRUNK].GetUInt();
    Node* nodeTrunk = gScene->GetNode(trunkID);

    Node *boxNode = gScene->CreateChild("SmallBox", REPLICATED);
    boxNode->SetVar(ATTR_TIME_DEATH, Variant(gTime->GetElapsedTime() + 60.0f));
    boxNode->SetPosition(nodeTrunk->GetWorldPosition() + nodeTrunk->GetWorldRotation() * Vector3::UP * 1.0f);
    boxNode->SetScale(0.5f);

    StaticModel *boxObject = boxNode->CreateComponent<StaticModel>();
    boxObject->SetModel(gCache->GetResource<Model>("Models/Box.mdl"));
    boxObject->SetMaterial(gCache->GetResource<Material>("Materials/StoneEnvMapSmall.xml"));
    boxObject->SetCastShadows(true);

    RigidBody *body = boxNode->CreateComponent<RigidBody>();
    body->SetMass(100.0f);
    body->SetFriction(0.75f);
    CollisionShape *shape = boxNode->CreateComponent<CollisionShape>();
    shape->SetBox(Vector3::ONE);

    const float OBJECT_VELOCITY = 250.0f;

    body->SetLinearVelocity(nodeTrunk->GetWorldRotation() * Vector3::UP * OBJECT_VELOCITY);

    boxNodes.Push(boxNode);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandlePhysicsPostStep(StringHash, VariantMap&)
{
    for(Node *node : boxNodes)
    {
        RigidBody *body = node->GetComponent<RigidBody>();
        if(body->GetLinearVelocity() == Vector3::ZERO || body->GetPosition().y_ < -100.0f || node->GetVar(ATTR_TIME_DEATH).GetFloat() < gTime->GetElapsedTime())
        {
            boxNodes.Remove(node);
            gScene->RemoveChild(node);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleClientConnected(StringHash, VariantMap &eventData)
{
    LOG_INFO("New client connected");

    using namespace ClientConnected;

    Connection *newConnection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());

    newConnection->SetScene(gScene);

    Vehicle* tank = gGame->ClientConnected(newConnection);

    tanks.Push(tank);
    connections.Push(newConnection);

    VariantMap remoteEventData;
    //remoteEventData[P_ID_TOWER] = tank->towerID;
//    remoteEventData[P_ID_TRUNK] = tank->trunkID;
    remoteEventData[P_ID_CASE] = 0;
    newConnection->SendRemoteEvent(E_CLIENTOBJECTID, true, remoteEventData);

    numClients++;

    gChat->SendToAll(MSG_CHAT, newConnection->GetAddress() + " enter");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::SendStringMessage(Vehicle *tank, const String &var, const String &value)
{
    VariantMap eventData;
    eventData[P_STRING_VAR] = var;
    eventData[P_STRING_VALUE] = value;

    Connection *connection = 0;
    for(uint i = 0; i < tanks.Size(); i++)
    {
        if(tanks[i] == tank)
        {
            connection = connections[i];
            break;
        }
    }

    if(connection)
    {
        connection->SendRemoteEvent(E_STRING_MESSAGE, false, eventData);
    }
    else
    {
        LOG_INFOF("Can not message - not find *tank");
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleNetworkMessage(StringHash, VariantMap &eventData)
{
    using namespace NetworkMessage;

    Connection *connection = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());

    int msgID = eventData[P_MESSAGEID].GetInt();

    const PODVector<uint8> &data = eventData[P_DATA].GetBuffer();
    MemoryBuffer msg(data);

    VectorBuffer buffer;

    if(msgID == MSG_PING)
    {
        connection->SendMessage(MSG_PING, true, true, buffer);
    }
    else if(msgID == MSG_LOAD_CPU)
    {
#ifndef WIN32
        // uint numCPU = GetNumPhysicalCPUs(); On virtual server not work
        int rez = system("uptime > out.uptime");
        File file(gContext, "out.uptime", FILE_READ);
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
        Vector<SharedPtr<Connection>> connections_ = gNetwork->GetClientConnections();

        float speedIn = 0.0f;
        float speedOut = 0.0f;

        for(Connection *conn : connections_)
        {
            speedIn += conn->GetBytesInPerSec();
            speedOut += conn->GetBytesOutPerSec();
        }

        buffer.WriteFloat(speedIn + gCounters->GetServerIn());
        buffer.WriteFloat(speedOut + gCounters->GetServerOut());
        connection->SendMessage(MSG_SERVER_SPEED, true, true, buffer);
    }
    else if(msgID == MSG_CONTROL)
    {
        Control cntrl = (Control)msg.ReadInt();
        TypeControl type = (TypeControl)msg.ReadInt();
        
        for(uint i = 0; i < connections.Size(); i++)
        {
            if(connection == connections[i])
            {
                Vehicle *tank = tanks[i];
//                tank->SetControl(cntrl, type);
                break;
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleClientDisconnected(StringHash, VariantMap &eventData)
{
    using namespace ClientDisconnected;

    VariantMap &eData = GetEventDataMap();
    eData[CloseConnection::P_CONNECT] = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());
    SendEvent(E_CLOSECONNECTION, eData);

    // Send message in chat

    Connection *conn = static_cast<Connection*>(eventData[P_CONNECTION].GetPtr());

    numClients--;

    gChat->SendToAll(MSG_CHAT, conn->GetAddress() + " leave");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleControlMessage(StringHash, VariantMap&)
{

}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleCloseConnection(StringHash, VariantMap &eventData)
{
    Connection *connection = static_cast<Connection*>(eventData[CloseConnection::P_CONNECT].GetPtr());

    gGame->ClientDisconnected(connection);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleServerConnected(StringHash, VariantMap &)
{
    LOG_INFOF("%s", __FUNCTION__);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleServerDisconnected(StringHash, VariantMap &)
{
    LOG_INFOF("%s", __FUNCTION__);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleConnectFailed(StringHash, VariantMap &)
{
    LOG_INFOF("%s", __FUNCTION__);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleClientIdentity(StringHash, VariantMap &)
{

}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleClientSceneLoaded(StringHash, VariantMap &)
{

}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleNetworkUpdate(StringHash, VariantMap &)
{
    LOG_INFOF("%s", __FUNCTION__);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleNetworkUpdateSent(StringHash, VariantMap &)
{
    LOG_INFOF("%s", __FUNCTION__);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleNetworkSceneLoadFailed(StringHash, VariantMap &)
{
    LOG_INFOF("%s", __FUNCTION__);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Server::HandleRemoteEventData(StringHash, VariantMap &)
{
    LOG_INFOF("%s", __FUNCTION__);
}
