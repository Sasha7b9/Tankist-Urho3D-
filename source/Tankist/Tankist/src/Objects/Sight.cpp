/* (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by */
#include <stdafx.h>
#include "Sight.h"
#include "Graphics/lImage.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawTriangle(lImage *image, int x, int y)
{
    image->DrawLine(x - 5, y + 10, x, y, Color::WHITE);
    image->DrawLine(x, y, x + 5, y + 10, Color::WHITE);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
Sight::Sight(Context *context) : Window(context)
{
#ifdef WIN32
    lImage *imageSight = new lImage(context_);
    imageSight->SetSize(gUIRoot->GetWidth(), gUIRoot->GetHeight());
    imageSight->Clear({0.0f, 0.0f, 0.0f, 1.0f});

    SharedPtr<Texture2D> texture(new Texture2D(context_));
    texture->SetSize(imageSight->GetWidth(), imageSight->GetHeight(), D3DFMT_X8R8G8B8);

    SetSize(texture->GetWidth(), texture->GetHeight());

    float radius = gUIRoot->GetHeight() / 2.0f * 0.975f;

    int x0 = gUIRoot->GetWidth() / 2;
    int y0 = gUIRoot->GetHeight() / 2;

    imageSight->DrawCircle((float)x0, (float)y0, radius, {0.0f, 0.0f, 0.0f, 0.0f}, 0.01f);
    imageSight->FillRegion((int)(x0 + 0.5f), (int)(y0 + 0.5f), {0.0f, 0.0f, 0.0f, 0.0f});

    float delta = radius / 100.0f;
    float kDelta = 1.035f;
    float y = -radius / 20.0f;
    for(int i = 0; i < 41; i++)
    {
        if((i % 2) == 1 && i < 10)
        {

        }
        else
        {
            float length = (i % 2) == 1 ? (radius / 15.0f) : (radius / 10.0f);
            imageSight->DrawLine((int)(x0 - length / 2.0f + 0.5f), (int)(y0 + y + 0.5f), (int)(x0 + length / 2.0f + 0.5f), (int)(y0 + y + 0.5f), Color::WHITE);
            if(i % 2 == 0)
            {
                SharedPtr<Text> text(CreateChild<Text>());
                text->SetFont(gCache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 8);
                text->SetText(String(i));
                text->SetMinWidth(20);
                text->SetMaxWidth(20);
                if(i % 4 == 0)
                {
                    text->SetTextAlignment(HA_RIGHT);
                    text->SetPosition((int)(x0 - length / 2.0f - 22), (int)(y0 + y + 0.5f - 5.0f));
                }
                else if(i % 2 == 0)
                {
                    text->SetTextAlignment(HA_LEFT);
                    text->SetPosition((int)(x0 + length / 2.0f + 0.5f + 4.0f), (int)(y0 + y + 0.5f - 5.0f));
                }
            }
        }
        y -= delta;
        delta *= kDelta;
    }

    delta = 60.0f;

    for(int i = 1; i < 5; i++)
    {
        imageSight->DrawLine((int)(x0 - delta * i + delta / 2), (int)y0, (int)(x0 - delta * i + delta / 2), (int)(y0 + 5), Color::WHITE);
        DrawTriangle(imageSight, (int)(x0 - delta * i), (int)(y0));
        imageSight->DrawLine((int)(x0 + delta * i - delta / 2), (int)(y0), (int)(x0 + delta * i - delta / 2), (int)(y0 + 5), Color::WHITE);
        DrawTriangle(imageSight, (int)(x0 + delta * i), (int)y0);
    }

    imageSight->DrawLine((int)(x0 - 20), (int)(y0 + 40), (int)x0, (int)y0, Color::WHITE);
    imageSight->DrawLine(x0, y0, x0 + 20, y0 + 40, Color::WHITE);

    texture->SetData((SharedPtr<Image>)imageSight);

    SetTexture(texture);
    SetBringToBack(true);
    SetFocusMode(FM_NOTFOCUSABLE);
#endif
}
