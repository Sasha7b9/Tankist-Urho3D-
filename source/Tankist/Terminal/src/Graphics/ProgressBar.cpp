#include <stdafx.h>


#include "ProgressBar.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ProgressBar::ProgressBar(Context *context) : UIElement(context)
{
    sprite = new lSprite(context);
    sprite->SetSize((int)width, (int)height);
    AddChild(sprite);

    text = new Text(context);
    text->SetFont(gResourceCache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 13);
    text->SetFixedSize((int)height, (int)height);
    text->SetPosition((int)width + 3, 2);
    AddChild(text);

    SetWidth(sprite->GetWidth() + text->GetWidth());

    DrawProgress();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
ProgressBar::~ProgressBar()
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void ProgressBar::SetProgress(float progress)
{
    this->progress = progress;
    DrawProgress();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void ProgressBar::DrawProgress()
{
    sprite->Clear(Color::GRAY);
    sprite->FillRectangle(0, 0, (int)(width * progress), (int)height, Color::BLUE);

    char buffer[10];
    sprintf_s(buffer, "%5.1f%%", progress * 100.0f);

    text->SetText(buffer);
}
