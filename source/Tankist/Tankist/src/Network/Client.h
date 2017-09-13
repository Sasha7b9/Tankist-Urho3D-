#pragma once
#include "Objects/Tank.h"


class ThreadConnection;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Client : public Object
{
friend class ThreadConnection;

URHO3D_OBJECT(Client, Object)

public:
    Client(Context *context);
    ~Client();

    bool ConnectToServer();
    void Disconnect();
    bool AttachCameraToNode();

    // Request ping, load cpu, number clients
    void RequestSystemInformation();

    void MessageControl(Control control, TypeControl type);

    unsigned towerID = 0;
    unsigned trunkID = 0;
    unsigned caseID = 0;

private:
    void HandleStringMessage(StringHash, VariantMap&);
    void HandleClientObjectID(StringHash, VariantMap &);
    void HandleServerConnected(StringHash, VariantMap &);
    void HandleServerDisconnected(StringHash, VariantMap &);
    void HandleConnectFailed(StringHash, VariantMap &);
    void HandleNetworkMessage(StringHash, VariantMap &);
    void HandleClientConnected(StringHash, VariantMap &);

    bool serverIsConnected = false;

    float timePing = 0.0f;

    Client(Client const&) : Object(nullptr) {};
    Client operator=(Client const&) {};
};
