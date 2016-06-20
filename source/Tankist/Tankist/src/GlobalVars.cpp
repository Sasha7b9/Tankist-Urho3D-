#include <stdafx.h>


#include <GlobalVars.h>


// For server and client
Log *gLog = nullptr;
Network *gNetwork = nullptr;
FileSystem *gFileSystem = nullptr;
ResourceCache *gCache = nullptr;
Scene *gScene = nullptr;
Time *gTime = nullptr;

Server *gServer = nullptr;
TypeApplication gTypeApplication = Type_None;


// For client only
UI *gUI = nullptr;
UIElement *gUIRoot = nullptr;
Input *gInput = nullptr;
Renderer *gRenderer = nullptr;
Graphics *gGraphics = nullptr;
Console *gConsole = nullptr;
DebugHud *gDebugHud = nullptr;
Audio *gAudio = nullptr;
Localization *gLocale = nullptr;

CameraUni *gCamera = nullptr;
Client *gClient = nullptr;

String gIPAddress = "";
unsigned short gNumPort = 0;

Context *gContext = nullptr;

Tankist *gTankist = nullptr;
Chat *gChat = nullptr;
Game *gGame = nullptr;
ChatLog *gChatLog = nullptr;
AudioCapturer *gAudioCapturer = nullptr;
WindowGameESC *gWindowGameESC = nullptr;
WindowSettings *gWindowSettings = nullptr;
