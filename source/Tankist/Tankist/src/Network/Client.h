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
    //void TranslateChatMessage(const String &message);

    // Request ping, load cpu, number clients
    void RequestSystemInformation();

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
    void HandleClientDisconnected(StringHash, VariantMap &);
    void HandleClientIdentity(StringHash, VariantMap &);
    void HandleClientSceneLoaded(StringHash, VariantMap &);
    void HandleNetworkUpdate(StringHash, VariantMap &);
    void HandleNetworkUpdateSent(StringHash, VariantMap &);
    void HandleNetworkSceneLoadFailed(StringHash, VariantMap &);
    void HandleRemoteEventData(StringHash, VariantMap &);

    bool serverIsConnected = false;

    float timePing = 0.0f;

    Client(Client const&) : Object(nullptr) {};
    Client operator=(Client const&) {};
};
