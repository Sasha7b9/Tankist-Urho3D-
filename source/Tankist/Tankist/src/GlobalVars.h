#pragma once


// For server and client
extern Urho3D::Log *gLog;
extern Urho3D::Network *gNetwork;
extern Urho3D::FileSystem *gFileSystem;
extern Urho3D::ResourceCache *gResourceCache;
extern Urho3D::Scene *gScene;
extern Urho3D::Time *gTime;

extern Server *gServer;
extern TypeApplication gTypeApplication;


// For client only
extern Urho3D::UI *gUI;
extern Urho3D::UIElement *gUIRoot;
extern Urho3D::Input *gInput;
extern Urho3D::Renderer *gRenderer;
extern Urho3D::Graphics *gGraphics;
extern Urho3D::Console *gConsole;
extern Urho3D::DebugHud *gDebugHud;

extern CameraUni *gCamera;
extern Client *gClient;

extern String gIPAddress;
extern unsigned short gNumPort;

extern Urho3D::Context *gContext;
