#pragma once


class ThreadConnection;


class Client : public Object
{
friend class ThreadConnection;

#pragma warning(push)
#pragma warning(disable:4640)
    URHO3D_OBJECT(Client, Object)
#pragma warning(pop)

public:
    Client(Context *context);
    ~Client();

    bool ConnectToServer();
    void Disconnect();
    bool AttachCameraToNode();
    void TranslateMessage(const String &message);

    // Request ping, load cpu, number clients
    void RequestSystemInformation();

    unsigned towerID = 0;
    unsigned trunkID = 0;
    unsigned caseID = 0;

private:
    void HandleClientObjectID(StringHash eventType, VariantMap &eventData);
    void HandleServerConnected(StringHash eventType, VariantMap &eventData);
    void HandleServerDisconnected(StringHash eventType, VariantMap &eventData);
    void HandleConnectFailed(StringHash eventType, VariantMap &eventData);
    void HandleNetworkMessage(StringHash eventType, VariantMap &eventData);
    void HandleClientConnected(StringHash eventType, VariantMap &eventData);
    void HandleClientDisconnected(StringHash eventType, VariantMap &eventData);
    void HandleClientIdentity(StringHash eventType, VariantMap &eventData);
    void HandleClientSceneLoaded(StringHash eventType, VariantMap &eventData);
    void HandleNetworkUpdate(StringHash eventType, VariantMap &eventData);
    void HandleNetworkUpdateSent(StringHash eventType, VariantMap &eventData);
    void HandleNetworkSceneLoadFailed(StringHash eventType, VariantMap &eventData);
    void HandleRemoteEventData(StringHash eventType, VariantMap &eventData);

    bool serverIsConnected = false;

    float timePing = 0.0f;

    Client(Client const&) : Object(nullptr) {};
    Client operator=(Client const&) {};
};
