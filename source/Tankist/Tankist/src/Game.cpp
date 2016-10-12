#include <stdafx.h>


#include "Game.h"
#include "GlobalVars.h"
#include "defines.h"
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
            if(!gUI->GetFocusElement())
            {
                controls.Set(CTRL_LEFT, gInput->GetKeyDown('A'));
                controls.Set(CTRL_RIGHT, gInput->GetKeyDown('D'));
                controls.Set(CTRL_STOP, gInput->GetKeyDown(KEY_KP_ENTER));
                controls.Set(CTRL_TOWER_RIGHT, (gInput->GetKeyDown('E') | gInput->GetKeyDown(KEY_KP_6)) && gCamera->GetMode() == ModeShooter);
                controls.Set(CTRL_TOWER_RIGHT_FAST, (gInput->GetKeyDown('E') || gInput->GetKeyDown(KEY_KP_6)) && gCamera->GetMode() == ModeCommander);
                controls.Set(CTRL_TOWER_LEFT, (gInput->GetKeyDown('Q') | gInput->GetKeyDown(KEY_KP_4)) && gCamera->GetMode() == ModeShooter);
                controls.Set(CTRL_TOWER_LEFT_FAST, (gInput->GetKeyDown('Q') || gInput->GetKeyDown(KEY_KP_4)) && gCamera->GetMode() == ModeCommander);
                controls.Set(CTRL_TRUNK_DOWN, gInput->GetKeyDown(KEY_KP_2) && gCamera->GetMode() == ModeShooter);
                controls.Set(CTRL_TRUNK_DOWN_FAST, gInput->GetKeyDown(KEY_KP_2) && gCamera->GetMode() == ModeCommander);
                controls.Set(CTRL_TRUNK_UP, gInput->GetKeyDown(KEY_KP_8) && gCamera->GetMode() == ModeShooter);
                controls.Set(CTRL_TRUNK_UP_FAST, gInput->GetKeyDown(KEY_KP_8) && gCamera->GetMode() == ModeCommander);
            }

            serverConnection->SetControls(controls);
            //serverConnection->SetPosition(cameraNode->GetPosition());

            controls.buttons_ = 0;
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
                WeakPtr<Tank> tank = objects[connection];
                if(!tank)
                {
                    continue;
                }

                const Controls &controls = connection->GetControls();

                tank->controls.Set(CTRL_FORWARD, ((controls.buttons_ & CTRL_FORWARD) > 0));
                tank->controls.Set(CTRL_BACK, ((controls.buttons_ & CTRL_BACK) > 0));
                tank->controls.Set(CTRL_LEFT, ((controls.buttons_ & CTRL_LEFT) > 0));
                tank->controls.Set(CTRL_RIGHT, ((controls.buttons_ & CTRL_RIGHT) > 0));
                tank->controls.Set(CTRL_STOP, ((controls.buttons_ & CTRL_STOP) > 0));
                tank->controls.Set(CTRL_TOWER_LEFT, ((controls.buttons_ & CTRL_TOWER_LEFT) != 0));
                tank->controls.Set(CTRL_TOWER_RIGHT, ((controls.buttons_ & CTRL_TOWER_RIGHT) != 0));
                tank->controls.Set(CTRL_TRUNK_UP, ((controls.buttons_ & CTRL_TRUNK_UP) != 0));
                tank->controls.Set(CTRL_TRUNK_DOWN, ((controls.buttons_ & CTRL_TRUNK_DOWN) != 0));
                tank->controls.Set(CTRL_TOWER_LEFT_FAST, ((controls.buttons_ & CTRL_TOWER_LEFT_FAST) != 0));
                tank->controls.Set(CTRL_TOWER_RIGHT_FAST, ((controls.buttons_ & CTRL_TOWER_RIGHT_FAST) != 0));
                tank->controls.Set(CTRL_TRUNK_UP_FAST, ((controls.buttons_ & CTRL_TRUNK_UP_FAST) != 0));
                tank->controls.Set(CTRL_TRUNK_DOWN_FAST, ((controls.buttons_ & CTRL_TRUNK_DOWN_FAST) != 0));
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
