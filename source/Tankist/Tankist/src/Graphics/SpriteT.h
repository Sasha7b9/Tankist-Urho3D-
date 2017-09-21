#pragma once
#include "ImageT.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteT : public Sprite
{
    URHO3D_OBJECT(SpriteT, Sprite);

public:
    SpriteT(Context *context);
    ~SpriteT() = default;

    void SetSize(int width, int height);

    void Clear(const Color &color);

    void FillRectangle(int x, int y, int width, int height, const Color &color);

private:
    SpriteT(SpriteT const&) : Sprite(nullptr) {};
    SpriteT operator=(SpriteT const &) {};

    SharedPtr<ImageT> image;
};
