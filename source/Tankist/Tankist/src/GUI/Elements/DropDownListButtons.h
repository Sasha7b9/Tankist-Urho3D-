#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class DropDownListButtons : public Object
{
    URHO3D_OBJECT(DropDownListButtons, Object)

public:
    DropDownListButtons(Context *context, UIElement *parent, char *nameDDLB);

    void AddItem(const String &item);

    uint CurrentItem(String &item);

    void SetCurrentItem(const String &item);

private:

    DropDownListButtons(DropDownListButtons const&) : Object(nullptr) {};
    DropDownListButtons operator=(DropDownListButtons const&) {};


    DropDownList *list;

    void HandleButtonLess(StringHash, VariantMap&);

    void HandleButtonMore(StringHash, VariantMap&);
};
