#pragma once


class DropDownListButtons : public Object
{

#pragma warning(push)
#pragma warning(disable:4640)
    URHO3D_OBJECT(DropDownListButtons, Object)
#pragma warning(pop)

public:
    DropDownListButtons(Context *context, UIElement *parent, char *nameDDLB);

private:
    DropDownListButtons(DropDownListButtons const&) : Object(nullptr) {};
    DropDownListButtons operator=(DropDownListButtons const&) {};

    DropDownList *list;
    Button *btnLess;
    Button *btnMore;
};
