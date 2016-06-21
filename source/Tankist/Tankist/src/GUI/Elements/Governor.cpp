#include "stdafx.h"


#include "Governor.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Governor::Governor(Context *context, UIElement *parent, char *nameGovernor, int min, int max) : Object(context)
{
    this->min = min;
    this->max = max;

    UIElement *governor = parent->GetChild(nameGovernor, true);

    Button *btnLess = (Button*)governor->GetChild("ButtonLess", true);
    Button *btnMore = (Button*)governor->GetChild("ButtonMore", true);
    btnDirect = (Button*)governor->GetChild("ButtonDirect", true);
    slider = (Button*)governor->GetChild("Slider", true);
    tValue = (Text*)governor->GetChild("TextValue", true);

    scale = (float)(btnDirect->GetWidth() - slider->GetWidth()) / (max - min);

    SubscribeToEvent(btnLess, Urho3D::E_CLICK, URHO3D_HANDLER(Governor, HandleButtonLess));
    SubscribeToEvent(btnMore, Urho3D::E_CLICK, URHO3D_HANDLER(Governor, HandleButtonMore));
    SubscribeToEvent(slider, Urho3D::E_CLICK, URHO3D_HANDLER(Governor, HandleMouseDown));
    SubscribeToEvent(Urho3D::E_CLICKEND, URHO3D_HANDLER(Governor, HandleMouseUp));
    SubscribeToEvent(Urho3D::E_MOUSEMOVE, URHO3D_HANDLER(Governor, HandleMouseMove));

    SetValue(min);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
int Governor::GetValue()
{
    return value;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Governor::SetValue(int value)
{
    if(value < min)
    {
        value = min;
    }
    else if(value > max)
    {
        value = max;
    }

    this->value = value;
    UpdateSlider();
    UpdateText();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Governor::Inc()
{
    if(value < max)
    {
        value++;
    }

    UpdateSlider();
    UpdateText();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Governor::Dec()
{
    if(value > min)
    {
        value--;
    }

    UpdateSlider();
    UpdateText();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Governor::HandleButtonLess(StringHash, VariantMap&)
{
    Dec();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Governor::HandleButtonMore(StringHash, VariantMap&)
{
    Inc();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Governor::HandleMouseDown(StringHash, VariantMap&)
{
    mousePressed = true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Governor::HandleMouseMove(StringHash, VariantMap &eventData)
{
    if(mousePressed)
    {
        using namespace Urho3D::MouseMove;

        int newPos = slider->GetPosition().x_ + eventData[P_DX].GetInt();

        if(newPos < 0)
        {
            newPos = 0;
        }
        else if(newPos > btnDirect->GetWidth() - slider->GetWidth())
        {
            newPos = btnDirect->GetWidth() - slider->GetWidth();
        }

        slider->SetPosition(newPos, 0);

        value = (int)(slider->GetPosition().x_ / scale);

        UpdateText();
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Governor::HandleMouseUp(StringHash, VariantMap&)
{
    mousePressed = false;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Governor::UpdateSlider()
{
    slider->SetPosition((int)(value * scale), 0);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Governor::UpdateText()
{
    tValue->SetText(String(value));
}
