#pragma once


#include "lImage.h"


class lSprite : public Sprite
{

#pragma warning(push)
#pragma warning(disable:4640)
    URHO3D_OBJECT(lSprite, Sprite);
#pragma warning(pop)

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