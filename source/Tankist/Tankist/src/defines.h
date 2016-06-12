#pragma once

#define URHO3D_LOGGING

#pragma warning(push)
#pragma warning(disable:4251)

#include <Urho3D/Math/StringHash.h>

#pragma warning(pop)

using Urho3D::StringHash;


static const StringHash E_CLIENTOBJECTID("ClientObjectID");
static const int MSG_CHAT = 32;         // Chat message
static const int MSG_PING = 33;         // Request ping, return float
static const int MSG_LOAD_CPU = 34;     // Request full load cpu - [0...1] ([0...2]/2) for 2-cpu server, [0...1] ([0...4]/4) for 4-cpu server, return float
static const int MSG_NUM_CLIENTS = 35;  // Request number connected clients, return float
static const int MSG_SERVER_SPEED = 36;


static const StringHash P_ID("ID");


typedef unsigned int uint;
typedef unsigned char uint8;


#define PORT_GAME       30000   // Using for game
#define PORT_UPDATE     30001   // Using for updates
#define PORT_CHAT       30002   // Using for chat
#define PORT_VOICE_CHAT 30003   // Using for voice chat


#define SAFE_DELETE(x)  \
    if(x) delete x;     \
    x = nullptr;
