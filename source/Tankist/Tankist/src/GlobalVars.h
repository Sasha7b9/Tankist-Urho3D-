#pragma once


#include "Tankist.h"
#include "CameraUni.h"
#include "Network/Client.h"
#include "Network/Chat.h"
#include "Network/Server.h"
#include "Game.h"
#include "ChatLog.h"
#include "Audio/AudioCapturer.h"


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
extern Urho3D::Audio *gAudio;

extern CameraUni *gCamera;
extern Client *gClient;

extern Urho3D::String gIPAddress;
extern unsigned short gNumPort;

extern Urho3D::Context *gContext;

extern Tankist *gTankist;

extern Chat *gChat;

extern Game *gGame;

extern ChatLog *gChatLog;

extern AudioCapturer *gAudioCapturer;

extern UIElement *gWindowGameESC;
