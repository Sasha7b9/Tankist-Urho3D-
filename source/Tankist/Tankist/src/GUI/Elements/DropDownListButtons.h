#pragma once


class DropDownListButtons : public Object
{

#pragma warning(push)
#pragma warning(disable:4640)
    URHO3D_OBJECT(DropDownListButtons, Object)
#pragma warning(pop)

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
