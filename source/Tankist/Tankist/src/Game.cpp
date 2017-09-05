#include <stdafx.h>
#include "Game.h"
#include "GlobalVars.h"
#include "common/defines.h"
#include "Network/NetworkEvents.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Game::Game(Context *context) : Object(context)
{

}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Game::Start()
{
    SubscribeToEvents();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Game::SubscribeToEvents()
{
    SubscribeToEvent(E_PHYSICSPRESTEP, URHO3D_HANDLER(Game, HandlePhysicsPreStep));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Game::HandlePhysicsPreStep(StringHash, VariantMap &)
{
    // Client
    if(gTypeApplication == Type_Client)
    {
        Connection *serverConnection = gNetwork->GetServerConnection();

        if(serverConnection)
        {
        }
    }
    // Server
    else
    {
        if(gNetwork->IsServerRunning())
        {
            const Vector<SharedPtr<Connection>> &connections = gNetwork->GetClientConnections();

            for(uint i = 0; i < connections.Size(); ++i)
            {
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Game::ClientDisconnected(Connection *connection)
{
    Tank *object = objects[connection];
    if(object)
    {
        object->Delete();
    }
    objects.Erase(connection);

#ifdef _WINDOWS

    if (objects.Size() == 0)
    {
        gTankist->Exit();
    }

#endif
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
Tank* Game::ClientConnected(Connection *connection)
{
    Tank *tank = CreateTank();
    objects[connection] = tank;
    return tank;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
Tank* Game::CreateTank()
{
    Node* tankNode = gScene->CreateChild("Tank");
    tankNode->SetPosition(Vector3(0.0f, 110.0f, 0.0f));

    Tank *tank = tankNode->CreateComponent<Tank>();
    tank->Init();

    return tank;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Game::Shot()
{
    VariantMap eventData;
    eventData[P_ID_TRUNK] = gClient->trunkID;
    gNetwork->GetServerConnection()->SendRemoteEvent(E_SHOOT, true, eventData);
}
