#pragma once


class Chat : public Object
{

#pragma warning(push)
#pragma warning(disable:4640)
    URHO3D_OBJECT(Chat, Object)
#pragma warning(pop)

public:
    Chat(Context *context);

    bool IsActive();
    void SetActive(bool active);
    void PressEnter();
    void AddMessage(const String &message);

private:

    SharedPtr<Text> historyText;
    SharedPtr<LineEdit> messageEdit;
    Vector<String> messages;

    Chat(Chat const&) : Object(nullptr) {};
    Chat operator=(Chat const&) {};
};