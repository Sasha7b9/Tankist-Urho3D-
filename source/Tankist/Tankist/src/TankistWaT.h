#pragma once


enum TypeConnection
{
    Connection_None,
    Connection_Server,
    Connection_Client
};


class Vehicle;


class TankistWaT : public Application
{

#pragma warning(push)
#pragma warning(disable:4640)
    URHO3D_OBJECT(TankistWaT, Application);
#pragma warning(pop)

public:
    TankistWaT(Context* context);
    
    virtual void Setup();
    virtual void Start();
    virtual void Stop();
    
private:
    void CreateScene();

#ifndef _SERVER_
    void CreateUI();
    void MoveCamera();
    void SetWindowTitleAndIcon();
    void CreateConsoleAndDebugHud();

    void HandleKeyDown(StringHash eventType, VariantMap& eventData);
#endif
    void SubscribeToEvents();

    void HandlePostUpdate(StringHash eventType, VariantMap &eventData);
    void HandlePhysicsPreStep(StringHash eventType, VariantMap &eventData);

    // Client functions
    void ConnectToServer();

    // Server functions
    void HandleNewConnection(StringHash eventType, VariantMap &eventData);
    void HandleCloseConnection(StringHash eventType, VariantMap &eventData);

    Vehicle* CreateVehicle();

    HashMap<Connection*, WeakPtr<Vehicle>> serverObjects;

    TankistWaT(TankistWaT const&) : Application(nullptr) {};
    TankistWaT operator=(TankistWaT const &) {};
};
