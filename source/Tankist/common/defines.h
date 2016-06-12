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


static const Urho3D::StringHash P_ID("ID");
static const Urho3D::String INSTRUCTION("instructionText");


typedef unsigned int uint;
typedef unsigned char uint8;

typedef void    (*pFuncVV)(void);
typedef void    (*pFuncVI)(int);
typedef void    (*pFuncVpCI)(char*, int);
typedef void    (*pFuncVU8pCI)(uint8, char*, int);
typedef void    (*pFuncVIpCI)(int, char*, int);


#define PORT_GAME       30000   // Using for game
#define PORT_UPDATE     1235    // Using for updates
#define PORT_CHAT       30002   // Using for chat
#define PORT_VOICE_CHAT 30003   // Using for voice chat


#define SAFE_DELETE(x)  \
    if(x) delete x;     \
    x = nullptr;

//---------------------------------------------------------------------------------------------------------------------------------------------------
#define LOG_INFO(x)     \
    URHO3D_LOGINFOF("%s:%d", __FILE__, __LINE__);   \
    URHO3D_LOGINFO(x)

#define LOG_INFO1(format, y)                         \
    URHO3D_LOGINFOF("%s:%d", __FILE__, __LINE__);    \
    URHO3D_LOGINFOF(format, y)

//---------------------------------------------------------------------------------------------------------------------------------------------------
#define LOG_DEBUG1(format, x)   \
    URHO3D_LOGINFOF("%s:%d", __FILE__, __LINE__);   \
    URHO3D_LOGDEBUGF(format, x)

//---------------------------------------------------------------------------------------------------------------------------------------------------
#define LOG_WARNING(x)  \
    URHO3D_LOGINFOF("%s:%d", __FILE__, __LINE__);   \
    URHO3D_LOGWARNING(x)

//---------------------------------------------------------------------------------------------------------------------------------------------------
#define LOG_ERROR(x)    \
    URHO3D_LOGINFOF("%s:%d", __FILE__, __LINE__);   \
    URHO3D_LOGERROR(x)

#define LOG_ERROR1(format, x)    \
    URHO3D_LOGINFOF("%s:%d", __FILE__, __LINE__);   \
    URHO3D_LOGERRORF(format, x)

#define LOG_ERROR2(format, x, y)   \
    URHO3D_LOGINFOF("%s:%d", __FILE__, __LINE__);   \
    URHO3D_LOGERRORF(format, x, y)
    
