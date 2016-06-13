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

private:
    void HandleClientObjectID(StringHash eventType, VariantMap &eventData);
    void HandleServerConnected(StringHash eventType, VariantMap &eventData);
    void HandleConnectFiled(StringHash eventType, VariantMap &eventData);
    void HandleNetworkMessage(StringHash eventType, VariantMap &eventData);

    unsigned nodeID = 0;

    bool serverIsConnected = false;

    float timePing = 0.0f;

    Client(Client const&) : Object(nullptr) {};
    Client operator=(Client const&) {};
};