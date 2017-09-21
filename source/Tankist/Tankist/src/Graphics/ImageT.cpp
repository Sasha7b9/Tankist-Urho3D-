/* (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by */
#include <stdafx.h>
#include "ImageT.h"
#include "Core/Math.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ImageT::ImageT(Context *context) :
    Image(context)
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ImageT::SetSize(int width, int height)
{
    Image::SetSize(width, height, 4);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ImageT::RegisterObject(Context* context)
{
    context->RegisterFactory<ImageT>();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ImageT::SetPoint(int x, int y, const Color& color)
{
    if(x < GetWidth() && y < GetHeight())
    {
        SetPixel((int)x, (int)y, color);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ImageT::DrawLine(int x0, int y0, int x1, int y1, const Color &color)
{
    if((x1 - x0) == 0 && (y1 - y0) == 0)
    {
        x0++;
    }

    int x = x0;
    int y = y0;
    int dx = (int)fabs((float)(x1 - x0));
    int dy = (int)fabs((float)(y1 - y0));
    int s1 = Math::Sign(x1 - x0);
    int s2 = Math::Sign(y1 - y0);
    int temp;
    bool exchange;
    if(dy > dx)
    {
        temp = dx;
        dx = dy;
        dy = temp;
        exchange = true;
    }
    else
    {
        exchange = false;
    }
    int e = 2 * dy - dx;
    for(int i = 0; i <= dx; i++)
    {
        SetPoint(x, y, color);
        while(e >= 0)
        {
            if(exchange)
            {
                x += s1;
            }
            else
            {
                y += s2;
            }
            e -= 2 * dx;
        }
        if(exchange)
        {
            y += s2;
        }
        else
        {
            x += s1;
        }
        e += 2 * dy;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ImageT::DrawRectangle(int x, int y, int width, int height, const Color &color)
{
    DrawLine(x, y, x + width, y, color);
    DrawLine(x + width, y, x + width, y + height, color);
    DrawLine(x, y + height, x + width, y + height, color);
    DrawLine(x, y, x, y + height, color);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ImageT::FillRectangle(int x0, int y0, int width, int height, const Color &color)
{
    for(int x = x0; x < x0 + width; x++)
    {
        DrawLine(x, y0, x, y0 + height, color);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ImageT::FillRegion(int x, int y, const Color &color)
{
    /// @todo Посмотреть в интернете, как правильно это делается. https://habrahabr.ru/post/116398/ https://habrahabr.ru/post/116374/

    SetPixel(x, y, color);

    // Чертим горизонтальную линию
    int x0 = x - 1;
    while (x0 > 0 && GetPixel(x0, y) != color)
    {
        SetPixel(x0, y, color);
        --x0;
    }
    x0 = x + 1;
    while (x0 < GetWidth() && GetPixel(x0, y) != color)
    {
        SetPixel(x0, y, color);
        ++x0;
    }

    // Чертим вертикальную линию
    int y0 = y - 1;
    while (y0 > 0 && GetPixel(x, y0) != color)
    {
        SetPixel(x, y0, color);
        --y0;
    }
    y0 = y + 1;
    while (y0 < GetHeight() && GetPixel(x, y0) != color)
    {
        SetPixel(x, y0, color);
        ++y0;
    }

    Replace4Points(x - 1, y - 1, color);

    Replace4Points(x + 1, y - 1, color);

    Replace4Points(x + 1, y + 1, color);

    Replace4Points(x - 1, y + 1, color);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ImageT::Replace4Points(int x, int y, const Color &color)
{
    if(y > 0)                       // upper pixel
    {
        if(GetPixel(x, y - 1) != color)
        {
            SetPixel(x, y - 1, color);
            Replace4Points(x, y - 1, color);
        }
    }
    if(x < GetWidth() - 1)   // rught pixel
    {
        if(GetPixel(x + 1, y) != color)
        {
            SetPixel(x + 1, y, color);
            Replace4Points(x + 1, y, color);
        }
    }
    if(y < GetHeight() - 1)
    {
        if(GetPixel(x, y + 1) != color)
        {
            SetPixel(x, y + 1, color);
            Replace4Points(x, y + 1, color);
        }
    }
    if(x > 0)
    {
        if(GetPixel(x - 1, y) != color)
        {
            SetPixel(x - 1, y, color);
            Replace4Points(x - 1, y, color);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ImageT::FillRegion(int x, int y, const Color &color, const Color &colorBound)
{
    boundingColor = colorBound;

    if(GetPixel(x, y) != colorBound)
    {
        SetPixel(x, y, color);
        Replace4PointsBound(x, y, color);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ImageT::CopyImage(int x0, int y0, ImageT &inImage)
{
    int xMin = x0;
    int xMax = xMin + inImage.GetWidth();
    if (xMax >= GetWidth())
    {
        xMax = GetWidth() - 1;
    }

    int yMin = y0;
    int yMax = yMin + inImage.GetHeight();
    if (yMax >= GetHeight())
    {
        yMax = GetHeight() - 1;
    }

    for (int x = x0; x < xMax; x++)
    {
        for (int y = y0; y < yMax; y++)
        {
            int curX = x - x0;
            int curY = y - y0;
            Color color = inImage.GetPixel(curX, curY);

            if (color.a_ > 0.5f)
            {
                SetPoint(x, y, color);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
#define FILL(a, b)                              \
    Color col = GetPixel(a, b);                 \
    if(col != boundingColor && col != color)    \
    {                                           \
        SetPixel(a, b, color);                  \
        Replace4PointsBound(a, b, color);       \
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ImageT::Replace4PointsBound(int x, int y, const Color &color)
{
    if(y > 0)
    {
        FILL(x, y - 1);
    }
    if(x < GetWidth() - 1)
    {
        FILL(x + 1, y);
    }
    if(y < GetHeight() - 1)
    {
        FILL(x, y + 1);
    }
    if(x > 0)
    {
        FILL(x - 1, y);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ImageT::DrawPolyline(const Color &color, int numPoints, int *xy)
{
    int numLines = numPoints - 1;

    for(int i = 0; i < numLines; i++)
    {
        DrawLine(xy[i * 2], xy[i * 2 + 1], xy[i * 2 + 2], xy[i * 2 + 3], color);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ImageT::DrawCircle(float x, float y, float radius, const Color &color, float step)
{
    for (float angle = 0.0f; angle < 360.0f; angle += step)
    {
        SetPoint((int)(x + Cos(angle) * radius + 0.5f), (int)(y + Sin(angle) * radius + 0.5f), color);
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
IntVector2 ImageT::GetHotSpot() const
{
    return hotSpot;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void ImageT::SetHotSpot(int x, int y)
{
    hotSpot.x_ = x;
    hotSpot.y_ = y;
}
