#pragma once


#include "lSprite.h"


class ProgressBar : public UIElement
{

#pragma warning(push)
#pragma warning(disable:4640)
    URHO3D_OBJECT(ProgressBar, UIElement);
#pragma warning(pop)

public:
    ProgressBar(Context *context);
    ~ProgressBar();

    // progress = [0.0f...1.0f]
    void SetProgress(float progress);
    float GetProgress() { return progress; }

private:
    ProgressBar& operator=(const ProgressBar&)
    {};
    ProgressBar(const ProgressBar&) : UIElement(nullptr)
    {};

    SharedPtr<lSprite> sprite;
    SharedPtr<Text> text;

    float width = 300.0f;
    float height = 20.0f;
    float progress = 0.0f;

    void DrawProgress();
};