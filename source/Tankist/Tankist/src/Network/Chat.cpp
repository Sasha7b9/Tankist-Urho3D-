#include <stdafx.h>
#include "Chat.h"
#include "NetworkEvents.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Chat::Chat(Context *context, Type type) : Object(context)
{
    this->type = type;

    if (type == Chat_Client)
    {
        SharedPtr<UIElement> container(gUIRoot->CreateChild<UIElement>());
        container->SetFixedSize(200, 300);
        container->SetPosition(0, 30);
        container->SetLayoutMode(LM_VERTICAL);
        container->SetStyleAuto();

        Font *font = gCache->GetResource<Font>("Fonts/CRL.ttf");

        historyText = container->CreateChild<Text>();
        historyText->SetColor(Color::WHITE);
        historyText->SetFont(font, 10);
        historyText->SetMaxHeight(100);
        historyText->SetMaxWidth(300);
        historyText->SetWordwrap(true);

        messageEdit = container->CreateChild<LineEdit>();
        messageEdit->SetStyleAuto();
        messageEdit->SetFixedHeight(18);
        messageEdit->SetVisible(false);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void CallbackClientOnRecieve(uint8 typeMessage, void *buffer, int sizeBuffer)
{
    if(typeMessage == MSG_CHAT)
    {
        gChat->AddMessage(String((char*)buffer, (uint)sizeBuffer));
    }
    else if(typeMessage == MSG_VOICE_CHAT)
    {
#ifdef WIN32
        void *buf = gAudioCapturer->OPUS_Decode(buffer, &sizeBuffer);
        gAudioCapturer->PlayData(buf, (uint)sizeBuffer);
#endif
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
bool Chat::Connect(const char *address, uint16 port)
{
    if (type == Chat_Client)
    {
        client.Init(CallbackClientOnRecieve);
        if(client.Connect(address, port))
        {
            URHO3D_LOGINFOF("Connected chat on %s:%d", address, (int)port);
            return true;
        }

        LOG_ERRORF("Can not connect chat on %s:%d", address, (int)port);
    }
    else
    {
        LOG_WARNING("Call Chat::Client() for server");
    }

    return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
bool Chat::IsActive()
{
    return messageEdit->HasFocus();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Chat::SetActive(bool active)
{
    messageEdit->SetVisible(active);
    messageEdit->SetFocus(active);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Chat::PressEnter()
{
    String text = messageEdit->GetText();
    if (text.Empty())
    {
        SetActive(false);
    }
    else
    {
        client.SendMessage(MSG_CHAT, (void*)text.CString(), text.Length());
        messageEdit->SetText("");
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Chat::SendToServer(const String &message)
{
    client.SendMessage(MSG_CHAT, (void*)message.CString(), message.Length());
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Chat::SendAudioData(void *buffer, uint sizeBuffer)
{
    client.SendMessage(MSG_VOICE_CHAT, buffer, sizeBuffer);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Chat::UpdateChat()
{
    if(messages.Size() == 0)
    {
        return;
    }


    do
    {
        if(historyText->GetHeight() > 0.8f * gUIRoot->GetHeight())
        {
            messages.Erase(0);
        }
        String allRows;
        for(uint i = 0; i < messages.Size(); i++)
        {
            allRows += messages[i] + "\n";
        }

        historyText->SetText(allRows);

    } while(historyText->GetHeight() > 0.8f * gUIRoot->GetHeight());
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Chat::AddMessage(const String &message)
{
    messages.Push(message);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct DataClient
{
    DataClient() : clientID(0), port(0)
    {
        address[0] = '\0';
    };
    DataClient(SOCKET id, char *addr, uint16 p) : clientID(id), port(p)
    {
#ifdef WIN32
        strcpy_s(address, 19, addr);
#else
        strcpy(address, addr);
#endif

    };
    SOCKET clientID;
    char address[20];
    uint16 port;

    bool operator!=(const DataClient &right)
    {
        return  (clientID != right.clientID) ||
                (port != right.port) ||
                (strcmp(address, right.address) != 0);
    }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------
static Vector<DataClient> clients;

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void ServerCallbackOnConnect(SOCKET clientID, char *address, uint16 port)
{
    URHO3D_LOGINFOF("Chat from %s:%d connected", address, (int)port);

    clients.Push(DataClient(clientID, address, port));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void ServerCallbackOnRecieve(SOCKET clientID, uint8 typeMessage, void *data, int sizeData)
{
    if(typeMessage == MSG_CHAT)
    {
        String message((char*)data, (uint)sizeData);
        gChat->SendToAll(MSG_CHAT, message);
    }
    else if(typeMessage == MSG_VOICE_CHAT)
    {
        String message((char*)data, (uint)sizeData);
        gChat->SendToAllExcept(MSG_VOICE_CHAT, message, clientID);
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
static void ServerCallbackOnDisconnect(SOCKET clientID)
{
    LOG_FUNC_ENTER

    String message;

    for (DataClient &client : clients)
    {
        if (client.clientID == clientID)
        {
            clients.Remove(client);
            message = String(client.address) + String(" leave");
            break;
        }
    }
    gChat->SendToAll(MSG_CHAT, message);

    LOG_FUNC_LEAVE
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
bool Chat::Listen(uint16 port)
{
    if (type == Chat_Server)
    {
        ServerParam param;
        param.funcOnConnect = ServerCallbackOnConnect;
        param.funcOnDisconnect = ServerCallbackOnDisconnect;
        param.funcOnRecieve = ServerCallbackOnRecieve;
        param.port = port;

        if(server.Init(param))
        {
            URHO3D_LOGINFOF("Started chat on port %d", (int)port);
            return true;
        }

        LOG_ERRORF("Can not start chat on port %d", (int)port);
    }
    else
    {
        LOG_WARNING("Call Listen() for Chat in client mode");
    }

    return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Chat::SendToAll(uint8 typeMessage, const String &message)
{
    if(typeMessage == MSG_CHAT)
    {
        gChatLog->WriteMessage(message);
    }

    for (DataClient &cl : clients)
    {
        server.SendMessage(cl.clientID, typeMessage, (void*)message.CString(), message.Length());
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Chat::SendToAllExcept(uint8 typeMessage, const String &message, SOCKET except)
{
    for(DataClient &cl : clients)
    {
        if(cl.clientID != except)
        {
            server.SendMessage(cl.clientID, typeMessage, (void*)message.CString(), message.Length());
        }
    }
}
