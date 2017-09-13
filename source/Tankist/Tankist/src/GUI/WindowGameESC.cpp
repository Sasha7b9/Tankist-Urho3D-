/* (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by */
#include <stdafx.h>
#include "WindowGameESC.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WindowGameESC::WindowGameESC(Context *context) : Object(context)
{
    window = gUI->LoadLayout(gCache->GetResource<XMLFile>("UI/Elements/WindowGameESC.xml"));
    gUIRoot->AddChild(window);

    int posX = gUIRoot->GetWidth() / 2 - window->GetWidth() / 2;
    int posY = gUIRoot->GetHeight() / 2 - window->GetHeight() / 2;

    window->SetPosition(posX, posY);

    Hide();

    Button *bExit = (Button*)window->GetChild("bExit", true);
    SubscribeToEvent(bExit, E_RELEASED, URHO3D_HANDLER(WindowGameESC, HandleButtonExit));

    Button *bSettings = (Button*)window->GetChild("bSettings", true);
    SubscribeToEvent(bSettings, E_RELEASED, URHO3D_HANDLER(WindowGameESC, HandleButtonSettings));

    Button *bLanguage = (Button*)window->GetChild("bLanguage", true);
    SubscribeToEvent(bLanguage, E_RELEASED, URHO3D_HANDLER(WindowGameESC, HandleButtonLanguage));

    window->SetOpacity(0.75f);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowGameESC::Hide()
{
    window->SetVisible(false);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowGameESC::Show()
{
    window->SetVisible(true);
    window->SetBringToFront(true);
    window->SetFocus(true);
    window->BringToFront();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowGameESC::HandleButtonExit(StringHash, VariantMap&)
{
    gTankist->Exit();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowGameESC::HandleButtonLanguage(StringHash, VariantMap&)
{
    gSet->Set(LANGUAGE, gLocale->GetLanguage() == String("ru") ? 0 : 1);
    gLocale->SetLanguage(gSet->Get(LANGUAGE) == 0 ? "en" : "ru");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowGameESC::HandleButtonSettings(StringHash, VariantMap&)
{
    Hide();
    gWindowSettings->Show();
}
