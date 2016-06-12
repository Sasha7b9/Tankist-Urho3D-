#pragma once

#define URHO3D_LOGGING

#pragma warning(push)
#pragma warning(disable:4251)

#include <Urho3D/Math/StringHash.h>
#include <Urho3D/Container/Str.h>

#pragma warning(pop)

#define UPDATE_ADDR "188.120.239.61"
#define UPDATE_PORT 1235


static const Urho3D::StringHash E_CLIENTOBJECTID("ClientObjectID");
static const int MSG_CHAT = 32;         // Chat message
static const int MSG_PING = 33;         // Request ping, return float
static const int MSG_LOAD_CPU = 34;     // Request full load cpu - [0...1] ([0...2]/2) for 2-cpu server, [0...1] ([0...4]/4) for 4-cpu server, return float
static const int MSG_NUM_CLIENTS = 35;  // Request number connected clients, return float
static const int MSG_SERVER_SPEED = 36;


static const Urho3D::StringHash P_ID("ID");
static const Urho3D::String INSTRUCTION("instructionText");


typedef unsigned int uint;
typedef unsigned char uint8;

typedef void (*pFuncVV)(void);


#define PORT_GAME       30000   // Using for game
#define PORT_UPDATE     30001   // Using for updates
#define PORT_CHAT       30002   // Using for chat
#define PORT_VOICE_CHAT 30003   // Using for voice chat


#define SAFE_DELETE(x)  \
    if(x) delete x;     \
    x = nullptr;

//#define DISABLE_WARNINGS_BEGIN      \

