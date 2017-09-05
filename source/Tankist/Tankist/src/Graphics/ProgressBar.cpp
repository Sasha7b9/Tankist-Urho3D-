#include <stdafx.h>


#include "ProgressBar.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ProgressBar::ProgressBar(Context *context) : UIElement(context)
{
    sprite = new lSprite(context);
    sprite->SetSize((int)width, (int)height);
    AddChild(sprite);

    Font *font = gCache->GetResource<Font>("Fonts/Anonymous Pro.ttf");

    textPercents = new Text(context);
    textPercents->SetFont(font);
    textPercents->SetFixedSize((int)height, (int)height);
    textPercents->SetPosition((int)width + 3, 2);
    AddChild(textPercents);

    text = new Text(context);
    text->SetFont(font);
    text->SetPosition(-100, -60);
    AddChild(text);

    SetWidth(sprite->GetWidth() + textPercents->GetWidth());

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

    char buffer[100];
    sprintf_s(buffer, 99, "%5.1f%%", progress * 100.0f);

    textPercents->SetText(String(buffer));

    sprintf_s(buffer, 99, "Size: all %5.1fMB, recieved %5.1fMB\n", bytesAll / 1024.0f / 1024.0f, bytesRecieved / 1024.0f / 1024.0f);
    String str1(buffer);

    sprintf_s(buffer, 99, "Speed %5.1fkB/s, time: passed %5.1fs, elapsed %5.1fs", speed / 1024.0f, timePassed, timeElapsed);
    String str2(buffer);

    text->SetText(currentFile + String("\n") + str1 + str2);
}
