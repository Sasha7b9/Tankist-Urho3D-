#include <stdafx.h>


#include "ProgressBar.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ProgressBar::ProgressBar(Context *context) : UIElement(context)
{
    sprite = new lSprite(context);
    sprite->SetSize((int)width, (int)height);
    AddChild(sprite);

    Font *font = gResourceCache->GetResource<Font>("Fonts/Anonymous Pro.ttf");

    text = new Text(context);
    text->SetFont(font);
    text->SetFixedSize((int)height, (int)height);
    text->SetPosition((int)width + 3, 2);
    AddChild(text);

    textParameters = new Text(context);
    textParameters->SetFont(font);
    textParameters->SetFixedSize((int)height, 100);
    textParameters->SetPosition(-100, -30);
    AddChild(textParameters);

    textBytes = new Text(context);
    textBytes->SetFont(font);
    textBytes->SetFixedSize((int)height, 100);
    textBytes->SetPosition(-100, -60);
    AddChild(textBytes);

    SetWidth(sprite->GetWidth() + text->GetWidth());

    DrawProgress();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
ProgressBar::~ProgressBar()
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void ProgressBar::SetParameters(float progress, float timePassed, float timeElapsed, float speed, String currentFile)
{
    this->currentFile = currentFile;
    this->progress = progress;
    this->timePassed = timePassed;
    this->timeElapsed = timeElapsed;
    this->speed = speed;

    DrawProgress();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void ProgressBar::SetBytes(int all, int recieved)
{
    bytesAll = all;
    bytesRecieved = recieved;

    DrawProgress();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void ProgressBar::DrawProgress()
{
    sprite->Clear(Color::GRAY);
    sprite->FillRectangle(0, 0, (int)(width * progress), (int)height, Color::BLUE);

    char buffer[1000];
    sprintf_s(buffer, "%5.1f%%", progress * 100.0f);

    text->SetText(buffer);

    sprintf_s(buffer, "speed %5.1f kbps, time: passed %5.1f s, elapsed %5.1f s", speed / 1024.0f, timePassed, timeElapsed);

    textParameters->SetText(buffer);

    sprintf_s(buffer, "MBytes: all - %5.1f, recieved - %5.1f %s", bytesAll / 1024.0f / 1024.0f, bytesRecieved / 1024.0f / 1024.0f, currentFile.CString());

    textBytes->SetText(buffer);
}
