#pragma once


#pragma warning(push)
#pragma warning(disable:4100)

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/Container/HashMap.h>
#include <Urho3D/Network/Connection.h>
#include <Urho3D/Container/Ptr.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/LineEdit.h>

#pragma warning(pop)


using Urho3D::Application;
using Urho3D::Vector;
using Urho3D::HashMap;
using Urho3D::Connection;
using Urho3D::WeakPtr;
using Urho3D::SharedPtr;
using Urho3D::Text;
using Urho3D::LineEdit;


enum TypeApplication
{
    Type_None,      // Before initialization
    Type_Server,    // Online server
    Type_Client     // Online client
};


class Vehicle;


class Tankist : public Application
{

#pragma warning(push)
#pragma warning(disable:4640)
    URHO3D_OBJECT(Tankist, Application);
#pragma warning(pop)

public:
    Tankist(Context* context);
    
    virtual void Setup();
    virtual void Start();
    virtual void Stop();

    void UpdateMessages();

    void SetPing(float pingMS);
    void SetLoadCPU(float loadCPU);
    void SetNumClients(int numClients);
    void SetBytesInPerSec(float bytesInPerSec);
    void SetBytesOutPerSec(float bytesOutPerSec);
    void SetBytesInPerSecServer(float bytes);
    void SetBytesOutPerSecServer(float bytes);
    
private:
    void CreateScene();
    void CreateInstructions();

    void CreateUI();
    void MoveCamera();
    void SetWindowTitleAndIcon();
    void CreateConsoleAndDebugHud();
    bool ParseArguments(Vector<String> &arguments, TypeApplication &type, String &address, unsigned short &port);
    bool GetNumPort(String &str, unsigned short &port);

    void HandleKeyDown(StringHash eventType, VariantMap& eventData);

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

    Tankist(Tankist const&) : Application(nullptr) {};
    Tankist operator=(Tankist const &) {};

    bool exit = false;

    SharedPtr<Text> chatHistoryText;
    SharedPtr<LineEdit> messageEdit;

    SharedPtr<Text> statisticsWindow;

    float ping = 0.0f;
    float loadCPU = 0.0f;
    int numClients = 0;
    float bytesInPerSec = 0.0f;
    float bytesOutPerSec = 0.0f;
    float bytesInPerSecServer = 0.0f;
    float bytesOutPerSecServer = 0.0f;

    void UpdateStatisticWindow();
};
