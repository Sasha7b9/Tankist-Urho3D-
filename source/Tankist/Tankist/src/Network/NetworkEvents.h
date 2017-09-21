#pragma once


/// Сообщение чата
static const int MSG_CHAT = 32;
/// Запрос пинга. Возвращает float
static const int MSG_PING = 33;
/// \brief Запрос полной загрузки процессора сервера - [0...1] ([0...2]/2) для 2-процессорного сервера, [0...1] ([0...4]/4) для 4-процессорного
/// сервера. Возвращает float
static const int MSG_LOAD_CPU = 34;
/// Запрос числа подключенных клиентов, возвращает float
static const int MSG_NUM_CLIENTS = 35;

static const int MSG_SERVER_SPEED = 36;

static const int MSG_VOICE_CHAT = 37;
/// Сообщение для передачи управления на сервер
static const int MSG_CONTROL = 38;


static const StringHash E_CLIENTOBJECTID("ClientObjectID");

static const StringHash P_ID_TOWER("ID_TOWER");

static const StringHash P_ID_TRUNK("ID_TRUNK");

static const StringHash P_ID_CASE("ID_CASE");
/// Это ID ноды танка, который сервер посылает клиенту
static const StringHash P_ID_NODE("ID_NODE");

static const StringHash E_STRING_MESSAGE("StringMessage");

static const StringHash P_STRING_VAR("STRING_VAR");

static const StringHash P_STRING_VALUE("STRING_VALUE");

static const StringHash E_SHOOT("Shoot");

// Соообщения клиента
