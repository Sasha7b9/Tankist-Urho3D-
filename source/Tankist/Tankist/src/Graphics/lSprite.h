#pragma once


#include "lImage.h"


class lSprite : public Sprite
{
    URHO3D_OBJECT(lSprite, Sprite);

public:
    lSprite(Context *context);
    ~lSprite();

    void SetSize(int width, int height);
    void Clear(const Color &color);
    void FillRectangle(int x, int y, int width, int height, const Color &color);

private:
    lSprite(lSprite const&) : Sprite(nullptr) {};
    lSprite operator=(lSprite const &) {};

    SharedPtr<lImage> image;
};