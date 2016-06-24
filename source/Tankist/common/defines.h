#pragma once

#define URHO3D_LOGGING

#pragma warning(push)
#pragma warning(disable:4251)

#include <Urho3D/Math/StringHash.h>
#include <Urho3D/Container/Str.h>

#pragma warning(pop)

#define ADDR_SERVER "188.120.239.61"


static const Urho3D::StringHash E_CLIENTOBJECTID("ClientObjectID");
static const int MSG_CHAT = 32;         // Chat message
static const int MSG_PING = 33;         // Request ping, return float
static const int MSG_LOAD_CPU = 34;     // Request full load cpu - [0...1] ([0...2]/2) for 2-cpu server, [0...1] ([0...4]/4) for 4-cpu server, return float
static const int MSG_NUM_CLIENTS = 35;  // Request number connected clients, return float
static const int MSG_SERVER_SPEED = 36;
static const int MSG_VOICE_CHAT = 37;


static const Urho3D::StringHash P_ID_TOWER("ID_TOWER");
static const Urho3D::StringHash P_ID_TRUNK("ID_TRUNK");
static const Urho3D::StringHash P_ID_CASE("ID_CASE");
static const Urho3D::String INSTRUCTION("tInstructions");


typedef unsigned int    uint;
typedef unsigned short  uint16;
typedef unsigned char   uint8;

typedef void    (*pFuncVV)(void);
typedef void    (*pFuncVS)(SOCKET);
typedef void    (*pFuncVpVI)(void*, int);
typedef void    (*pFuncVpVS)(void*, SOCKET);
typedef void    (*pFuncVpCI)(char*, int);
typedef void    (*pFuncVU8pCI)(uint8, char*, int);
typedef void    (*pFuncVIpCI)(int, char*, int);
typedef void    (*pFuncVSpCU16)(SOCKET, char*, uint16);
typedef void    (*pFuncVpVIpCI)(void*, int, char*, int);
typedef void    (*pFuncVpVSpCU16)(void*, SOCKET, char*, uint16);
typedef void    (*pFuncVIU8pCI)(int, uint8, char*, int);
typedef void    (*pFuncVIU8pU8I)(int, uint8, uint8*, int);
typedef void    (*pFuncVpVIpVI)(void*, int, void*, int);
typedef void    (*pFuncVpVSpVI)(void*, SOCKET, void*, int);
typedef void    (*pFuncVSU8pVI)(SOCKET, uint8, void*, int);
typedef void    (*pFuncVpC)(char*);
typedef void    (*pFuncVU8pVI)(uint8, void*, int);
typedef void    (*pFuncVpVpVI)(void*, void*, int);


#define PORT_GAME       30000   // Using for game
#define PORT_UPDATE     1235    // Using for updates
#define PORT_CHAT       30002   // Using for chat
#define PORT_VOICE_CHAT 30003   // Using for voice chat


enum StateRecieve
{
    WAIT_MSG,
    RECIEVE_MSG
};


union BitSet32
{
    int data32;
    uint8 b[4];
};


#define SAFE_DELETE(x)  \
    if(x) delete x;     \
    x = nullptr;

#define LOG_INFO(message) Urho3D::Log::Write(Urho3D::LOG_INFO, message + Urho3D::ToString("  .  .  .  .  .  .  .  .  .  .  %s:%d", __FILE__, __LINE__));
#define LOG_INFOF(format, ...) Urho3D::Log::Write(Urho3D::LOG_INFO, Urho3D::ToString(format, ##__VA_ARGS__) + Urho3D::ToString("  .  .  .  .  .  .  .  .  .  .  %s:%d", __FILE__, __LINE__));

#define LOG_DEBUG(message) Urho3D::Log::Write(Urho3D::LOG_DEBUG, message + Urho3D::ToString("  .  .  .  .  .  .  .  .  .  .  %s:%d", __FILE__, __LINE__));
#define LOG_DEBUGF(format, ...) Urho3D::Log::Write(Urho3D::LOG_DEBUG, Urho3D::ToString(format, ##__VA_ARGS__) + Urho3D::ToString("  .  .  .  .  .  .  .  .  .  .  %s:%d", __FILE__, __LINE__));

#define LOG_WARNING(message) Urho3D::Log::Write(Urho3D::LOG_WARNING, message + Urho3D::ToString("  .  .  .  .  .  .  .  .  .  .  %s:%d", __FILE__, __LINE__));
#define LOG_WARNINGF(format, ...) Urho3D::Log::Write(Urho3D::LOG_WARNING, Urho3D::ToString(format, ##__VA_ARGS__) + Urho3D::ToString("  .  .  .  .  .  .  .  .  .  .  %s:%d", __FILE__, __LINE__));

#define LOG_ERROR(message) Urho3D::Log::Write(Urho3D::LOG_ERROR, message + Urho3D::ToString("  .  .  .  .  .  .  .  .  .  .  %s:%d", __FILE__, __LINE__));
#define LOG_ERRORF(format, ...) Urho3D::Log::Write(Urho3D::LOG_ERROR, Urho3D::ToString(format, ##__VA_ARGS__) + Urho3D::ToString("  .  .  .  .  .  .  .  .  .  .  %s:%d", __FILE__, __LINE__));
    
