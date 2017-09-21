/* (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by */
#include <stdafx.h>
#include "SpriteT.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SpriteT::SpriteT(Context *context) : Sprite(context)
{
    image = new ImageT(context);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
SpriteT::~SpriteT()
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SpriteT::SetSize(int width, int height)
{
    SetFixedSize(width, height);
    image->SetSize(width, height);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SpriteT::Clear(const Color &color)
{
    FillRectangle(0, 0, GetWidth(), GetHeight(), color);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void SpriteT::FillRectangle(int x, int y, int width, int height, const Color &color)
{
    image->FillRectangle(x, y, width, height, color);

    SharedPtr<Texture2D> texture(new Texture2D(context_));
    texture->SetSize(image->GetWidth(), image->GetHeight(), D3DFMT_X8R8G8B8);
    texture->SetData((SharedPtr<Image>)image);

    SetTexture(texture);
    SetFullImageRect();
}
