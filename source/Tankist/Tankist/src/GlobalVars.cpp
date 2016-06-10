#include <stdafx.h>


// For server and client
Urho3D::Log *gLog = nullptr;
Urho3D::Network *gNetwork = nullptr;
Urho3D::FileSystem *gFileSystem = nullptr;
Urho3D::ResourceCache *gResourceCache = nullptr;
Urho3D::Scene *gScene = nullptr;
Urho3D::Time *gTime = nullptr;

Server *gServer = nullptr;
TypeApplication gTypeApplication = Type_None;


// For client only
Urho3D::UI *gUI = nullptr;
Urho3D::UIElement *gUIRoot = nullptr;
Urho3D::Input *gInput = nullptr;
Urho3D::Renderer *gRenderer = nullptr;
Urho3D::Graphics *gGraphics = nullptr;
Urho3D::Console *gConsole = nullptr;
Urho3D::DebugHud *gDebugHud = nullptr;

CameraUni *gCamera = nullptr;
Client *gClient = nullptr;

String gIPAddress = "";
unsigned short gNumPort = 0;

Urho3D::Context *gContext = nullptr;
