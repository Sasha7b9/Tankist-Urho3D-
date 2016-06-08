#pragma once


static const StringHash E_CLIENTOBJECTID("ClientObjectID");
static const int MSG_CNTRL = 32;


static const StringHash P_ID("ID");


typedef unsigned int uint;
typedef unsigned char uint8;


#define SAFE_DELETE(x)  \
    if(x) delete x;     \
    x = nullptr;
