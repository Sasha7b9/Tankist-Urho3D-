#include <stdafx.h>

#include "Chat.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Chat::Chat(Context *context, Type type) : Object(context)
{
    this->type = type;

    if (type == Chat_Client)
    {
        SharedPtr<UIElement> container(gUIRoot->CreateChild<UIElement>());
        container->SetFixedSize(200, 300);
        container->SetPosition(0, 30);
        container->SetLayoutMode(Urho3D::LM_VERTICAL);
        container->SetStyleAuto();

        Font *font = gCache->GetResource<Font>("Fonts/CRL.ttf");

        historyText = container->CreateChild<Text>();
        historyText->SetColor(Urho3D::Color::WHITE);
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
    DataClient(int id, char *addr, uint16 p) : clientID(id), port(p)
    {
        strcpy_s(address, 19, addr);
    };
    int clientID;
    char address[20];
    uint16 port;

    bool operator!=(const DataClient &right)
    {
        return  (clientID != right.clientID) ||
                (port != right.port) ||
                (strcmp(address, right.address) != 0);
    }
};


static Vector<DataClient> clients;


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void ServerCallbackOnConnect(int clientID, char *address, uint16 port)
{
    URHO3D_LOGINFOF("Chat from %s:%d connected", address, (int)port);

    clients.Push(DataClient(clientID, address, port));
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void ServerCallbackOnRecieve(int /*clientID*/, uint8 typeMessage, void *data, int sizeData)
{
    if(typeMessage == MSG_CHAT)
    {
        String message((char*)data, (uint)sizeData);
        gChat->SendToAll(message);
    }
    else if(typeMessage == MSG_VOICE_CHAT)
    {
        void *buf = gAudioCapturer->OPUS_Decode(data, &sizeData);

        gAudioCapturer->PlayData(buf, (uint)sizeData);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
static void ServerCallbackOnDisconnect(int clientID)
{
    LOG_INFOF("%s enter");

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
    gChat->SendToAll(message);

    LOG_INFOF("%s leave");
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
void Chat::SendToAll(const String &message)
{
    gChatLog->WriteMessage(message);

    for (DataClient &client : clients)
    {
        server.SendMessage(client.clientID, MSG_CHAT, (void*)message.CString(), message.Length());
    }
}
