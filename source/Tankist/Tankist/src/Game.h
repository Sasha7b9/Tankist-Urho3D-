#pragma once


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Vehicle.h"


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
    Vehicle* ClientConnected(Connection *connection);

private:
    Game(Game const&) : Object(nullptr) {};
    Game operator=(Game const&) {};

    HashMap<Connection*, WeakPtr<Vehicle>> objects;

    void SubscribeToEvents();

    void HandlePhysicsPreStep(StringHash eventType, VariantMap &eventData);

    Vehicle* CreateVehicle();
};