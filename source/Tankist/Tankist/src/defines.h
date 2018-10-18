#pragma once

#pragma warning(push)
#pragma warning(disable:4251 4668)

#include <Urho3D/Math/StringHash.h>
#include <Urho3D/Container/Str.h>

#pragma warning(pop)


using namespace Urho3D;

#define ADDR_SERVER "188.120.239.61"

static const String INSTRUCTION("tInstructions");

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

#define LOG_FUNC_ENTER Log::Write(__FILE__, __LINE__, LOG_INFO, ToString("%s enter", __FUNCTION__));
#define LOG_FUNC_LEAVE Log::Write(__FILE__, __LINE__, LOG_INFO, ToString("%s leave", __FUNCTION__));
