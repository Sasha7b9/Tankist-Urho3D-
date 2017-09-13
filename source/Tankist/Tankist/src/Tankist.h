#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define VIEW_MASK_TERRAIN 1 << 10

enum TypeApplication
{
    Type_None,      // Before initialization
    Type_Server,    // Online server
    Type_Client     // Online client
};


class Tank;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Tankist : public Application
{
    URHO3D_OBJECT(Tankist, Application);

public:
    Tankist(Context* context);
    
    virtual void Setup();
    virtual void Start();
    virtual void Stop();

    void SetPing(float pingMS);
    void SetLoadCPU(float loadCPU);
    void SetNumClients(int numClients);
    void SetBytesInPerSec(float bytesInPerSec);
    void SetBytesOutPerSec(float bytesOutPerSec);
    void SetBytesInPerSecServer(float bytes);
    void SetBytesOutPerSecServer(float bytes);
    void Exit();
    
private:
    void CreateScene();
    void CreateInstructions();

    void CreateUI();
    void MoveCamera();
    void SetWindowTitleAndIcon();
    void CreateConsoleAndDebugHud();
    bool ParseArguments(Vector<String> &arguments, TypeApplication &type, String &address, unsigned short &port);
    bool GetNumPort(String &str, unsigned short &port);

    // Handlers
    void HandleKeyDown(StringHash, VariantMap&);
    void HandleKeyUp(StringHash, VariantMap&);
    void HandlePostUpdate(StringHash, VariantMap&);
    void HandleLanguageChanged(StringHash, VariantMap&);
    void HandlePostRenderUpdate(StringHash, VariantMap&);

    void SubscribeToEvents();

    Tankist(Tankist const&) : Application(nullptr) {};
    Tankist operator=(Tankist const &) {};

    bool exit = false;

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
