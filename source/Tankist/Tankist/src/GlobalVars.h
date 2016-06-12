#pragma once

#pragma warning(push)
#pragma warning(disable:4100)
#pragma warning(disable:4640)

#include <Urho3D/IO/Log.h>
#include <Urho3D/Network/Network.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Graphics/Renderer.h>

#include "Tankist.h"
#include "CameraUni.h"
#include "Network/Client.h"
#include "Network/Chat.h"
#include "Network/Server.h"


#pragma warning(pop)

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

extern Tankist *gTankist;

extern Chat *gChat;
