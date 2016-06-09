#pragma once


static const StringHash E_CLIENTOBJECTID("ClientObjectID");
static const int MSG_CNTRL = 32;


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
