#include "stdafx.h"


#include "WindowSettings.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WindowSettings::WindowSettings(Context *context) : Object(context)
{
    window = gUI->LoadLayout(gCache->GetResource<XMLFile>("UI/Elements/WindowSettings.xml"));
    gUIRoot->AddChild(window);

    int posX = gUIRoot->GetWidth() / 2 - window->GetWidth() / 2;
    int posY = gUIRoot->GetHeight() / 2 - window->GetHeight() / 2;

    window->SetPosition(posX, posY);

    Hide();

    Button *bApplyChanges = (Button*)window->GetChild("bApplyChanges", true);
    SubscribeToEvent(bApplyChanges, Urho3D::E_RELEASED, URHO3D_HANDLER(WindowSettings, HandleButtonApplyChanges));

    Button *bReturnToGame = (Button*)window->GetChild("bReturnToGame", true);
    SubscribeToEvent(bReturnToGame, Urho3D::E_RELEASED, URHO3D_HANDLER(WindowSettings, HandleButtonReturnToGame));

    FillDropDownListResolutions();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowSettings::FillDropDownListResolutions()
{
    UIElement *ddlb = window->GetChild("ddlbResolution", true);

    PODVector<UIElement*> elements;
    ddlb->GetChildren(elements, true);

    for(UIElement *element : elements)
    {
        if(element->GetType() == "DropDownList")
        {
            DropDownList* list = (DropDownList*)element;

            PODVector<IntVector2> resolutions = gGraphics->GetResolutions();

            for(IntVector2 &resolution : resolutions)
            {
                String res = String(resolution.x_) + " x " + String(resolution.y_);

                LOG_INFOF("%s", res.CString());

                Text *text = list->CreateChild<Text>();
                text->SetFont(gCache->GetResource<Font>("Fonts/CRL.ttf"), 15);
                text->SetText(res);

                list->AddItem(text);
            }
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowSettings::Hide()
{
    window->SetVisible(false);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowSettings::Show()
{
    window->SetVisible(true);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowSettings::HandleButtonApplyChanges(StringHash, VariantMap&)
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowSettings::HandleButtonReturnToGame(StringHash, VariantMap&)
{
    Hide();
}
