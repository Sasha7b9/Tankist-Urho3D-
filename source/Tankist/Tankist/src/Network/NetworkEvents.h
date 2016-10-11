#pragma once


static const int MSG_CHAT = 32;         // Chat message
static const int MSG_PING = 33;         // Request ping, return float
static const int MSG_LOAD_CPU = 34;     // Request full load cpu - [0...1] ([0...2]/2) for 2-cpu server, [0...1] ([0...4]/4) for 4-cpu server, return float
static const int MSG_NUM_CLIENTS = 35;  // Request number connected clients, return float
static const int MSG_SERVER_SPEED = 36;
static const int MSG_VOICE_CHAT = 37;

static const Urho3D::StringHash E_CLIENTOBJECTID("ClientObjectID");
static const Urho3D::StringHash P_ID_TOWER("ID_TOWER");
static const Urho3D::StringHash P_ID_TRUNK("ID_TRUNK");
static const Urho3D::StringHash P_ID_CASE("ID_CASE");
static const Urho3D::StringHash P_ID_NODE("ID_NODE");       // Это ID ноды танка, который сервер посылает клиенту

static const Urho3D::StringHash E_STRING_MESSAGE("StringMessage");
static const Urho3D::StringHash P_STRING_VAR("STRING_VAR");
static const Urho3D::StringHash P_STRING_VALUE("STRING_VALUE");

static const Urho3D::StringHash E_SHOOT("Shoot");

