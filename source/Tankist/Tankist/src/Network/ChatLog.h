#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ChatLog : public Object
{
#pragma warning(push)
#pragma warning(disable:4640)
    URHO3D_OBJECT(ChatLog, Object)
#pragma warning(pop)

public:
    ChatLog(Context *context);
    ~ChatLog();

    void WriteMessage(const String &message);

private:
    File *file = nullptr;

    ChatLog(ChatLog const&) : Object(nullptr) {};
    ChatLog operator=(ChatLog const&) {};
};
