#pragma once

#pragma warning(push)
#pragma warning(disable:4100)
#pragma warning(disable:4365)
#pragma warning(disable:4625)
#pragma warning(disable:4626)
#pragma warning(disable:4640)

#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Container/Ptr.h>
#include <Urho3D/Container/Str.h>
#include <Urho3D/Container/Vector.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/LineEdit.h>

#pragma warning(pop)


using Urho3D::Object;
using Urho3D::Context;
using Urho3D::SharedPtr;
using Urho3D::Text;
using Urho3D::LineEdit;
using Urho3D::String;
using Urho3D::Vector;


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