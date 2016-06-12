#pragma once


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma warning(push)
#pragma warning(disable:4100 4251 4266 4275 4365 4625 4626 4640)

#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Container/HashMap.h>
#include <Urho3D/Network/Connection.h>
#include <Urho3D/Container/Ptr.h>

#pragma warning(pop)

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