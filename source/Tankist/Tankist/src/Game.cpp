#include "Game.h"


#include <Urho3D/Physics/PhysicsEvents.h>

using namespace Urho3D;

#include "GlobalVars.h"
#include "defines.h"


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
            Controls controls;

            if(!gUI->GetFocusElement())
            {
                controls.Set(CTRL_FORWARD, gInput->GetKeyDown('W'));
                controls.Set(CTRL_BACK, gInput->GetKeyDown('S'));
                controls.Set(CTRL_LEFT, gInput->GetKeyDown('A'));
                controls.Set(CTRL_RIGHT, gInput->GetKeyDown('D'));
                controls.Set(CTRL_TOWER_RIGHT, gInput->GetKeyDown('E') | gInput->GetKeyDown(KEY_KP_6));
                controls.Set(CTRL_TOWER_LEFT, gInput->GetKeyDown('Q') | gInput->GetKeyDown(KEY_KP_4));
                controls.Set(CTRL_TRUNK_DOWN, gInput->GetKeyDown(KEY_KP_2));
                controls.Set(CTRL_TRUNK_UP, gInput->GetKeyDown(KEY_KP_8));
            }

            serverConnection->SetControls(controls);
            //serverConnection->SetPosition(cameraNode->GetPosition());
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
                Connection *connection = connections[i];
                WeakPtr<Vehicle> vehicle = objects[connection];
                if(!vehicle)
                {
                    continue;
                }

                const Controls &controls = connection->GetControls();

                vehicle->controls.Set(CTRL_FORWARD, ((controls.buttons_ & CTRL_FORWARD) > 0));
                vehicle->controls.Set(CTRL_BACK, ((controls.buttons_ & CTRL_BACK) > 0));
                vehicle->controls.Set(CTRL_LEFT, ((controls.buttons_ & CTRL_LEFT) > 0));
                vehicle->controls.Set(CTRL_RIGHT, ((controls.buttons_ & CTRL_RIGHT) > 0));
                vehicle->controls.Set(CTRL_TOWER_LEFT, ((controls.buttons_ & CTRL_TOWER_LEFT) != 0));
                vehicle->controls.Set(CTRL_TOWER_RIGHT, ((controls.buttons_ & CTRL_TOWER_RIGHT) != 0));
                vehicle->controls.Set(CTRL_TRUNK_UP, ((controls.buttons_ & CTRL_TRUNK_UP) != 0));
                vehicle->controls.Set(CTRL_TRUNK_DOWN, ((controls.buttons_ & CTRL_TRUNK_DOWN) != 0));
            }
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Game::ClientDisconnected(Connection *connection)
{
    Vehicle *object = objects[connection];
    if(object)
    {
        object->Delete();
    }
    objects.Erase(connection);

#ifdef _WINDOWS

    gTankist->Exit();

#endif
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
Vehicle* Game::ClientConnected(Connection *connection)
{
    Vehicle *vehicle = CreateVehicle();
    objects[connection] = vehicle;
    return vehicle;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
Vehicle* Game::CreateVehicle()
{
    Node* vehicleNode = gScene->CreateChild("Vehicle");
    vehicleNode->SetPosition(Vector3(0.0f, 5.0f, 15.0f));

    Vehicle *vehicle = vehicleNode->CreateComponent<Vehicle>();
    vehicle->Init();

    return vehicle;
}
