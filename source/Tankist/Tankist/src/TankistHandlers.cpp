/* (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by */
#include <stdafx.h>
#include "Tankist.h"
#include "GlobalVars.h"
#include "defines.h"
#include "Objects/Tank.h"


#define SEND_CONTROL(k, ctrl)   if(key == k && !repeat) { gClient->MessageControl(ctrl, CTRL_ON); }
#define SEND_CONTROL_MODECAMERA(k, mode, ctrl) if(key == k && mode) { gClient->MessageControl(ctrl, CTRL_ON); }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Tankist::HandleKeyDown(StringHash, VariantMap& eventData)
    {
#ifdef WIN32
    using namespace KeyDown;

    int key = eventData[P_KEY].GetInt();
    bool repeat = eventData[P_REPEAT].GetBool();

    if(!gUI->GetFocusElement())
    {
        bool modeShooter = gCamera->GetMode() == ModeShooter;
        bool modeCommander = gCamera->GetMode() == ModeCommander;

        SEND_CONTROL(KEY_W, CTRL_FORWARD)
        else SEND_CONTROL(KEY_S, CTRL_BACK)
        else SEND_CONTROL(KEY_A, CTRL_LEFT)
        else SEND_CONTROL(KEY_D, CTRL_RIGHT)
        else SEND_CONTROL_MODECAMERA(KEY_KP_2, modeShooter, CTRL_TRUNK_DOWN)
        else SEND_CONTROL_MODECAMERA(KEY_KP_2, modeCommander, CTRL_TRUNK_DOWN_FAST)
        else SEND_CONTROL_MODECAMERA(KEY_KP_8, modeShooter, CTRL_TRUNK_UP)
        else SEND_CONTROL_MODECAMERA(KEY_KP_8, modeCommander, CTRL_TRUNK_UP_FAST)
        else SEND_CONTROL_MODECAMERA(KEY_E, modeShooter, CTRL_TOWER_RIGHT)
        else SEND_CONTROL_MODECAMERA(KEY_KP_6, modeShooter, CTRL_TOWER_RIGHT)
        else SEND_CONTROL_MODECAMERA(KEY_E, modeCommander, CTRL_TOWER_RIGHT_FAST)
        else SEND_CONTROL_MODECAMERA(KEY_KP_6, modeCommander, CTRL_TOWER_RIGHT_FAST)
        else SEND_CONTROL_MODECAMERA(KEY_Q, modeShooter, CTRL_TOWER_LEFT)
        else SEND_CONTROL_MODECAMERA(KEY_KP_4, modeShooter, CTRL_TOWER_LEFT)
        else SEND_CONTROL_MODECAMERA(KEY_Q, modeCommander, CTRL_TOWER_LEFT_FAST)
        else SEND_CONTROL_MODECAMERA(KEY_KP_4, modeCommander, CTRL_TOWER_LEFT_FAST)
    }

    // Close console (if open) or exit when ESC is pressed
    if(key == KEY_ESCAPE)
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
    else if(key == KEY_RETURN)
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
    else if(key == KEY_F1)
    {
        gConsole->Toggle();
    }

    // Toggle debug HUD with F2
    else if(key == KEY_F2)
    {
        if(gDebugHud->GetMode() == 0 || gDebugHud->GetMode() == DEBUGHUD_SHOW_MEMORY)
        {
            gDebugHud->SetMode(DEBUGHUD_SHOW_ALL);
        }
        else
        {
            gDebugHud->SetMode(DEBUGHUD_SHOW_NONE);
        }
    }
    else if(key == KEY_F3)
    {
        if(gDebugHud->GetMode() == 0 || gDebugHud->GetMode() == DEBUGHUD_SHOW_ALL)
        {
            gDebugHud->SetMode(DEBUGHUD_SHOW_MEMORY);
        }
        else
        {
            gDebugHud->SetMode(DEBUGHUD_SHOW_NONE);
        }
    }

    else if(key == KEY_KP_ENTER)
    {
        gAudioCapturer->Pause(false);
    }

    else if(key == KEY_KP_PLUS)
    {
        gCamera->IncFov();
    }

    else if(key == KEY_KP_MINUS)
    {
        gCamera->DecFov();
    }

    else if(key == KEY_KP_MULTIPLY)
    {
        gCamera->DefaultFov();
    }

    else if(key == KEY_SPACE)
    {
        gCamera->SetCameraMode(ModeShooter, gScene->GetNode(gClient->trunkID));
    }

    else if(key == KEY_F9)
    {
        gCamera->SetCameraMode(ModeCommander, gScene->GetNode(gClient->towerID));
    }

    else if(key == KEY_CTRL)
    {
        if(!eventData[P_REPEAT].GetBool())
        {
            gGame->Shot();
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
        else if(key == KEY_F12)
        {
            UIElement *instr = gUI->GetRoot()->GetChild(INSTRUCTION);
            instr->SetVisible(!instr->IsVisible());
        }
    }
#endif
}

#define SEND_CONTROL_OFF(k, ctrl) if(key == k) { gClient->MessageControl(ctrl, CTRL_OFF); }
#define SEND_CONTROL_MODECAMERA_OFF(k, mode, ctrl) if(key == k && mode) { gClient->MessageControl(ctrl, CTRL_OFF); }

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::HandleKeyUp(StringHash, VariantMap& eventData)
{
#ifdef WIN32
    using namespace KeyDown;

    int key = eventData[P_KEY].GetInt();

    bool modeShooter = gCamera->GetMode() == ModeShooter;
    bool modeCommander = gCamera->GetMode() == ModeCommander;

    SEND_CONTROL_OFF(KEY_A, CTRL_LEFT)
    else SEND_CONTROL_OFF(KEY_D, CTRL_RIGHT)
    else SEND_CONTROL_MODECAMERA_OFF(KEY_KP_2, modeShooter, CTRL_TRUNK_DOWN)
    else SEND_CONTROL_MODECAMERA_OFF(KEY_KP_2, modeCommander, CTRL_TRUNK_DOWN_FAST)
    else SEND_CONTROL_MODECAMERA_OFF(KEY_KP_8, modeShooter, CTRL_TRUNK_UP)
    else SEND_CONTROL_MODECAMERA_OFF(KEY_KP_8, modeCommander, CTRL_TRUNK_UP_FAST)
    else SEND_CONTROL_MODECAMERA_OFF(KEY_E, modeShooter, CTRL_TOWER_RIGHT)
    else SEND_CONTROL_MODECAMERA_OFF(KEY_KP_6, modeShooter, CTRL_TOWER_RIGHT)
    else SEND_CONTROL_MODECAMERA_OFF(KEY_E, modeCommander, CTRL_TOWER_RIGHT_FAST)
    else SEND_CONTROL_MODECAMERA_OFF(KEY_KP_6, modeCommander, CTRL_TOWER_RIGHT_FAST)
    else SEND_CONTROL_MODECAMERA_OFF(KEY_Q, modeShooter, CTRL_TOWER_LEFT)
    else SEND_CONTROL_MODECAMERA_OFF(KEY_KP_4, modeShooter, CTRL_TOWER_LEFT)
    else SEND_CONTROL_MODECAMERA_OFF(KEY_Q, modeCommander, CTRL_TOWER_LEFT_FAST)
    else SEND_CONTROL_MODECAMERA_OFF(KEY_KP_4, modeCommander, CTRL_TOWER_LEFT_FAST)

    if(key == KEY_KP_ENTER)
    {
        gAudioCapturer->Pause(true);
    }
#endif
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::HandlePostUpdate(StringHash, VariantMap &)
{
    gCounters->Update();

    static float prevTime = 0.0f;

    float curTime = gTime->GetElapsedTime();

    if((curTime - prevTime) > 1.0f)
    {
        gClient->RequestSystemInformation();
        prevTime = curTime;
    }

    if(gTypeApplication == Type_Client)
    {
        UpdateCamera();
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


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::HandlePostRenderUpdate(StringHash, VariantMap&)
{
    if(gDebugRenderer && gScene->GetComponent<PhysicsWorld>())
    {
        gScene->GetComponent<PhysicsWorld>()->DrawDebugGeometry(true);

        PODVector<Node*> nodes;
        gScene->GetChildren(nodes, true);
        for(Node *node : nodes)
        {
            if(node->GetName() == "damper")
            {
                CollisionShape *shape = node->GetComponent<CollisionShape>();
                if(shape)
                {
                    shape->DrawDebugGeometry(gDebugRenderer, true);
                }
            }

            if(node->GetName() == "Tank")
            {
                Tank *tank = node->GetComponent<Tank>();
                if(tank)
                {
                    tank->Logging();
                }
            }
        }
    }
}
