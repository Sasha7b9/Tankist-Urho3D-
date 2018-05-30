#pragma once


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Objects/Vehicle.h"

enum StateGame
{
    InProcess,
    State_WindowGameESC
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Game : public Object
{
    URHO3D_OBJECT(Game, Object);

public:
    Game(Context *context);

    void Start();

    void ClientDisconnected(Connection *connection);

    Vehicle* ClientConnected(Connection *connection);

    StateGame GetState()
    {
        return state;
    }

    void SetState(StateGame state_)
    {
        this->state = state_;
    }

    void Shot();

private:
    Game(Game const&) : Object(nullptr) {};
    Game operator=(Game const&) {};

    HashMap<Connection*, WeakPtr<Vehicle>> objects;

    void SubscribeToEvents();

    void HandlePhysicsPreStep(StringHash, VariantMap&);

    Vehicle* CreateTank();

    StateGame state = InProcess;

    Controls controls;
};
