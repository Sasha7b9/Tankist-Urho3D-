#include <stdafx.h>

#include "Tankist.h"
#include "GlobalVars.h"
#include "defines.h"
#include "Vehicle.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tankist::HandleKeyDown(StringHash /*eventType*/, VariantMap& eventData)
{
#ifdef WIN32
    using namespace Urho3D::KeyDown;

    int key = eventData[P_KEY].GetInt();

    // Close console (if open) or exit when ESC is pressed
    if(key == Urho3D::KEY_ESC)
    {
        if(gConsole->IsVisible())
        {
            gConsole->SetVisible(false);
        }
        else
        {
            if(gGame->GetState() == InProcess)
            {
                gWindowGameESC->Show();
                gGame->SetState(State_WindowGameESC);
            }
            else
            {
                gWindowGameESC->Hide();
                gGame->SetState(InProcess);
            }
        }
    }
    else if(key == Urho3D::KEY_RETURN)
    {
        if(!gConsole->IsVisible())
        {
            if(!gChat->IsActive())
            {
                gChat->SetActive(true);
            }
            else
            {
                gChat->PressEnter();
            }
        }
    }

    // Toggle console with F1
    else if(key == Urho3D::KEY_F1)
    {
        gConsole->Toggle();
    }

    // Toggle debug HUD with F2
    else if(key == Urho3D::KEY_F2)
    {
        if(gDebugHud->GetMode() == 0 || gDebugHud->GetMode() == Urho3D::DEBUGHUD_SHOW_ALL_MEMORY)
        {
            gDebugHud->SetMode(Urho3D::DEBUGHUD_SHOW_ALL);
        }
        else
        {
            gDebugHud->SetMode(Urho3D::DEBUGHUD_SHOW_NONE);
        }
    }
    else if(key == Urho3D::KEY_F3)
    {
        if(gDebugHud->GetMode() == 0 || gDebugHud->GetMode() == DEBUGHUD_SHOW_ALL)
        {
            gDebugHud->SetMode(DEBUGHUD_SHOW_ALL_MEMORY);
        }
        else
        {
            gDebugHud->SetMode(DEBUGHUD_SHOW_NONE);
        }
    }

    // Common rendering quality controls, only when UI has no focused element
    else if(!gUI->GetFocusElement())
    {
        // Texture quality
        if(key == '1')
        {
            int quality = gRenderer->GetTextureQuality();
            ++quality;
            if(quality > QUALITY_HIGH)
            {
                quality = QUALITY_LOW;
            }
            gRenderer->SetTextureQuality(quality);
        }

        // Material quality
        else if(key == '2')
        {
            int quality = gRenderer->GetMaterialQuality();
            ++quality;
            if(quality > QUALITY_HIGH)
            {
                quality = QUALITY_LOW;
            }
            gRenderer->SetMaterialQuality(quality);
        }

        // Specular lighting
        else if(key == '3')
        {
            gRenderer->SetSpecularLighting(!gRenderer->GetSpecularLighting());
        }

        // Shadow rendering
        else if(key == '4')
        {
            gRenderer->SetDrawShadows(!gRenderer->GetDrawShadows());
        }

        // Shadow map resolution
        else if(key == '5')
        {
            int shadowMapSize = gRenderer->GetShadowMapSize();
            shadowMapSize *= 2;
            if(shadowMapSize > 2048)
            {
                shadowMapSize = 512;
            }
            gRenderer->SetShadowMapSize(shadowMapSize);
        }

        // Shadow depth and filtering quality
        else if(key == '6')
        {
            ShadowQuality quality = gRenderer->GetShadowQuality();
            quality = (ShadowQuality)(quality + 1);
            if(quality > SHADOWQUALITY_BLUR_VSM)
            {
                quality = SHADOWQUALITY_SIMPLE_16BIT;
            }
            gRenderer->SetShadowQuality(quality);
        }

        // Occlusion culling
        else if(key == '7')
        {
            bool occlusion = gRenderer->GetMaxOccluderTriangles() > 0;
            occlusion = !occlusion;
            gRenderer->SetMaxOccluderTriangles(occlusion ? 5000 : 0);
        }

        // Instancing
        else if(key == '8')
        {
            gRenderer->SetDynamicInstancing(!gRenderer->GetDynamicInstancing());
        }

        // Take screenshot
        else if(key == '9')
        {
            Image screenshot(context_);
            gGraphics->TakeScreenShot(screenshot);
            // Here we save in the Data folder with date and time appended
            screenshot.SavePNG(gFileSystem->GetProgramDir() + "Data/Screenshot_" +
                               Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_') + ".png");
        }
        else if(key == Urho3D::KEY_F12)
        {
            UIElement *instr = gUI->GetRoot()->GetChild(INSTRUCTION);
            instr->SetVisible(!instr->IsVisible());
        }
    }
#endif
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::HandlePostUpdate(StringHash, VariantMap &)
{
    static float prevTime = 0.0f;

    float curTime = gTime->GetElapsedTime();

    if((curTime - prevTime) > 1.0f)
    {
        gClient->RequestSystemInformation();
        prevTime = curTime;
    }

    if(gTypeApplication == Type_Client)
    {
        MoveCamera();
    }

    if(exit)
    {
        engine_->Exit();
    }

    if(gChat)
    {
        gChat->UpdateChat();
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::HandleLanguageChanged(StringHash, VariantMap&)
{
    PODVector<UIElement*> elements;
    gUIRoot->GetChildren(elements, true);

    for(UIElement *element : elements)
    {
        if(element->GetType() == "Text")
        {
            String name = element->GetName();
            if(!name.Empty() && name != "DDL_Placeholder_Text" && name != "LE_Text" &&
               name != "TextValue"                                                          // This controls for information
               )
            {
                String text = gLocale->Get(name);
                if(!text.Empty())
                {
                    ((Text*)element)->SetText(text);
                }
            }
        }
    }
}
