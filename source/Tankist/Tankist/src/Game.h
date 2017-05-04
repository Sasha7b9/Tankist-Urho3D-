#pragma once


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Objects/Tank.h"

enum StateGame
{
    InProcess,
    State_WindowGameESC
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Game : public Object
{

#pragma warning(push)
#pragma warning(disable:4640)
    URHO3D_OBJECT(Game, Object);
#pragma warning(pop)

public:
    Game(Context *context);

    void Start();
    void ClientDisconnected(Connection *connection);
    Tank* ClientConnected(Connection *connection);
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

    HashMap<Connection*, WeakPtr<Tank>> objects;

    void SubscribeToEvents();

    void HandlePhysicsPreStep(StringHash, VariantMap&);
    void HandleCommandControl(StringHash, VariantMap&);

    Tank* CreateTank();

    StateGame state = InProcess;

    Controls controls;
};