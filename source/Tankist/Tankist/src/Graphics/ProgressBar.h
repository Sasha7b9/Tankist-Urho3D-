#pragma once


#include "lSprite.h"


class ProgressBar : public UIElement
{
    URHO3D_OBJECT(ProgressBar, UIElement);

public:
    ProgressBar(Context *context);
    ~ProgressBar();

    // progress = [0.0f...1.0f]
    float GetProgress() { return progress; }
    void SetParameters(float progress, float timePassed, float timeElapsed, float speed, String currentFile);
    void SetBytes(int all, int recieved);

private:
    ProgressBar& operator=(const ProgressBar&)
    {};
    ProgressBar(const ProgressBar&) : UIElement(nullptr)
    {};

    SharedPtr<lSprite> sprite;
    SharedPtr<Text> textPercents;
    SharedPtr<Text> text;

    float width = 300.0f;
    float height = 20.0f;
    float progress = 0.0f;

    float timePassed = 0.0f;
    float timeElapsed = 0.0f;
    float speed = 0.0f;

    int bytesAll = 0;
    int bytesRecieved = 0;

    String currentFile;

    void DrawProgress();
};