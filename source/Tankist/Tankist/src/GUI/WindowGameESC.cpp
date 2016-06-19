#include "stdafx.h"


#include "WindowGameESC.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WindowGameESC::WindowGameESC(Context *context) : Object(context)
{
    window = gUI->LoadLayout(gResourceCache->GetResource<XMLFile>("UI/Elements/WindowGameESC.xml"));
    gUIRoot->AddChild(window);

    int posX = gUIRoot->GetWidth() / 2 - window->GetWidth() / 2;
    int posY = gUIRoot->GetHeight() / 2 - window->GetWidth() / 2;

    window->SetPosition(posX, posY);

    Hide();

    Button *bExit = (Button*)window->GetChild("bExit", true);
    SubscribeToEvent(bExit, Urho3D::E_RELEASED, URHO3D_HANDLER(WindowGameESC, HandleButtonExit));

    Button *bOptions = (Button*)window->GetChild("bOptions", true);
    SubscribeToEvent(bOptions, Urho3D::E_RELEASED, URHO3D_HANDLER(WindowGameESC, HandleButtonOptions));

    Button *bLanguage = (Button*)window->GetChild("bLanguage", true);
    SubscribeToEvent(bLanguage, Urho3D::E_RELEASED, URHO3D_HANDLER(WindowGameESC, HandleButtonLanguage));
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
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowGameESC::HandleButtonExit(StringHash, VariantMap&)
{
    gTankist->Exit();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowGameESC::HandleButtonLanguage(StringHash, VariantMap&)
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowGameESC::HandleButtonOptions(StringHash, VariantMap&)
{

}
