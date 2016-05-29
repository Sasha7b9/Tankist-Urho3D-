#pragma once


// Windows version for local network
//#define _UNIVERSAL_ 1


static const unsigned short SERVER_PORT = 1234;

static const StringHash E_CLIENTOBJECTID("ClientObjectID");

static const StringHash P_ID("ID");


typedef unsigned int uint;


#define SAFE_DELETE(x)  \
    if(x) delete x;     \
    x = nullptr;
