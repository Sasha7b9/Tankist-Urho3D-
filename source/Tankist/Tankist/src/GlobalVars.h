#pragma once
#include "Tankist.h"
#include "Objects/CameraUni.h"
#include "Network/Client.h"
#include "Network/Chat.h"
#include "Network/Server.h"
#include "Game.h"
#include "Network/ChatLog.h"
#include "Audio/AudioCapturer.h"
#include "GUI/WindowGameESC.h"
#include "GUI/WindowSettings.h"
#include "Settings.h"
#include "common/Counters.h"
#include "GUI/GameGUI.h"


// For server and client
extern Context *gContext;
extern Log *gLog;
extern Network *gNetwork;
extern FileSystem *gFileSystem;
extern ResourceCache *gCache;
extern Scene *gScene;
extern Time *gTime;

extern Server *gServer;
extern TypeApplication gTypeApplication;


// For client only
extern UI *gUI;
extern UIElement *gUIRoot;
extern Input *gInput;
extern Renderer *gRenderer;
extern Graphics *gGraphics;
extern Console *gConsole;
extern DebugHud *gDebugHud;
extern Audio *gAudio;
extern Localization *gLocale;
extern DebugRenderer *gDebugRenderer;
extern PhysicsWorld *gPhysicsWorld;

extern CameraUni *gCamera;
extern Client *gClient;

extern String gIPAddress;
extern unsigned short gNumPort;

extern Tankist *gTankist;
extern Chat *gChat;
extern Game *gGame;
extern ChatLog *gChatLog;
extern AudioCapturer *gAudioCapturer;
extern WindowGameESC *gWindowGameESC;
extern WindowSettings *gWindowSettings;
extern Settings *gSet;
extern Counters *gCounters;
extern GameGUI *gGameGUI;
