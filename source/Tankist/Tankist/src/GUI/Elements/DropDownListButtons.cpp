#include "stdafx.h"


#include "DropDownListButtons.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DropDownListButtons::DropDownListButtons(Context *context, UIElement *parent, char *nameDDLB) : Object(context)
{
    UIElement *ddlb = parent->GetChild(nameDDLB, true);
}
