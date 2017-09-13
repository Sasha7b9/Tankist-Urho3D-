/* (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by */
#include <stdafx.h>
#include "DropDownListButtons.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DropDownListButtons::DropDownListButtons(Context *context, UIElement *parent, char *nameDDLB) : Object(context)
{
    UIElement *ddlb = parent->GetChild(nameDDLB, true);

    Button *btnLess = (Button*)ddlb->GetChild("ButtonLess", true);
    Button *btnMore = (Button*)ddlb->GetChild("ButtonMore", true);
    
    SubscribeToEvent(btnLess, E_RELEASED, URHO3D_HANDLER(DropDownListButtons, HandleButtonLess));
    SubscribeToEvent(btnMore, E_RELEASED, URHO3D_HANDLER(DropDownListButtons, HandleButtonMore));

    list = (DropDownList*)ddlb->GetChild("DropDownList", true);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void DropDownListButtons::AddItem(const String &item)
{
    Text *text = list->CreateChild<Text>();
    text->SetFont(gCache->GetResource<Font>("Fonts/CRL.ttf"), 15);
    text->SetText(item);
    list->AddItem(text);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
uint DropDownListButtons::CurrentItem(String &item)
{
    item = ((Text*)list->GetSelectedItem())->GetText();

    return list->GetSelection();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void DropDownListButtons::SetCurrentItem(const String &item)
{
    for(uint i = 0; i < list->GetNumItems(); i++)
    {
        if(((Text*)list->GetItem(i))->GetText() == item)
        {
            list->SetSelection(i);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void DropDownListButtons::HandleButtonLess(StringHash, VariantMap&)
{
    uint numItems = list->GetNumItems();
    uint selection = list->GetSelection();

    uint newSelection = (selection == numItems - 1) ? 0 : (selection + 1);

    list->SetSelection(newSelection);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void DropDownListButtons::HandleButtonMore(StringHash, VariantMap&)
{
    uint numItems = list->GetNumItems();
    uint selection = list->GetSelection();

    uint newSelection = (selection == 0) ? (numItems - 1) : (selection - 1);

    list->SetSelection(newSelection);
}
