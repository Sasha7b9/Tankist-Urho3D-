#include "stdafx.h"


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
    SubscribeToEvent(bExit, Urho3D::E_RELEASED, URHO3D_HANDLER(WindowGameESC, HandleButtonExit));

    Button *bOptions = (Button*)window->GetChild("bOptions", true);
    SubscribeToEvent(bOptions, Urho3D::E_RELEASED, URHO3D_HANDLER(WindowGameESC, HandleButtonOptions));

    Button *bLanguage = (Button*)window->GetChild("bLanguage", true);
    SubscribeToEvent(bLanguage, Urho3D::E_RELEASED, URHO3D_HANDLER(WindowGameESC, HandleButtonLanguage));

    SubscribeToEvent(Urho3D::E_CHANGELANGUAGE, URHO3D_HANDLER(WindowGameESC, HandleLanguageChanged));
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
    gLocale->SetLanguage(gLocale->GetLanguage() == String("ru") ? "en" : "ru");
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowGameESC::HandleButtonOptions(StringHash, VariantMap&)
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowGameESC::HandleLanguageChanged(StringHash, VariantMap&)
{
    PODVector<UIElement*> elements;
    window->GetChildren(elements, true);

    for(UIElement *element : elements)
    {
        if(element->GetType() == "Text")
        {
            String name = element->GetName();
            if(!name.Empty())
            {
                String text = gLocale->Get(name);
                if(!text.Empty())
                {
                    ((Text*)element)->SetText(text);
                }
            }
        }
    }
}
