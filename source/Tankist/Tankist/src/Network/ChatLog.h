#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ChatLog : public Object
{
    URHO3D_OBJECT(ChatLog, Object)

public:
    ChatLog(Context *context);
    ~ChatLog();

    void WriteMessage(const String &message);

private:
    File *file = nullptr;

    ChatLog(ChatLog const&) : Object(nullptr) {};
    ChatLog operator=(ChatLog const&) {};
};
