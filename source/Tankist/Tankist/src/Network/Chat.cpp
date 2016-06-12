#pragma warning(push)
#pragma warning(disable:4100 4251 4266 4275 4365 4625 4626 4640)

#include <Urho3D/UI/UIElement.h>
#include <Urho3D/UI/Font.h>

using Urho3D::UIElement;
using Urho3D::Font;

#include "Chat.h"

#include "GlobalVars.h"

#pragma warning(pop)

#include "defines.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Chat::Chat(Context *context) : Object(context)
{
    SharedPtr<UIElement> container(gUIRoot->CreateChild<UIElement>());
    container->SetFixedSize(200, 300);
    container->SetPosition(0, 50);
    container->SetLayoutMode(Urho3D::LM_VERTICAL);
    container->SetStyleAuto();

    Font *font = gResourceCache->GetResource<Font>("Fonts/Anonymous Pro.ttf");

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
    if(text.Empty())
    {
        SetActive(false);
    }
    else
    {
        gClient->TranslateMessage(text);
        messageEdit->SetText("");
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Chat::AddMessage(const String &message)
{
    messages.Push(message);

    do
    {
        if(historyText->GetHeight() > 0.8 * gUIRoot->GetHeight())
        {
            messages.Erase(0);
        }

        String allRows;
        for(uint i = 0; i < messages.Size(); i++)
        {
            allRows += messages[i] + "\n";
        }

        historyText->SetText(allRows);

    } while(historyText->GetHeight() > 0.8 * gUIRoot->GetHeight());
}
