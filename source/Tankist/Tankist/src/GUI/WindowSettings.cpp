#include "stdafx.h"


#include "WindowSettings.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WindowSettings::WindowSettings(Context *context) : Object(context)
{
    window = gUI->LoadLayout(gCache->GetResource<XMLFile>("UI/Elements/WindowSettings.xml"));
    gUIRoot->AddChild(window);

    int posX = gUIRoot->GetWidth() / 2 - window->GetWidth() / 2;
    int posY = gUIRoot->GetHeight() / 2 - window->GetHeight() / 2;

    window->SetPosition(posX, posY);

    Hide();

    SubscribeToEvent(E_SCREENMODE, URHO3D_HANDLER(WindowSettings, HandleChangedScreenMode));

    Button *bApplyChanges = (Button*)window->GetChild("bApplyChanges", true);
    SubscribeToEvent(bApplyChanges, E_RELEASED, URHO3D_HANDLER(WindowSettings, HandleButtonApplyChanges));

    Button *bReturnToGame = (Button*)window->GetChild("bReturnToGame", true);
    SubscribeToEvent(bReturnToGame, E_RELEASED, URHO3D_HANDLER(WindowSettings, HandleButtonReturnToGame));

    ddlbResolution = new DropDownListButtons(context_, window, "ddlbResolution");

    gGameVolume = new Governor(context, window, "gGameVolume");

    gChatVolume = new Governor(context, window, "gChatVolume");

    gMicLevel = new Governor(context, window, "gLevelMic");

    FillDropDownListResolutions();

    FillControlsFromSettings();

    window->SetOpacity(0.75f);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowSettings::FillDropDownListResolutions()
{
    PODVector<IntVector3> resolutions = gGraphics->GetResolutions(0);   // \todo –азобратьс€ с несколькими мониторами

    for(IntVector3 &resolution : resolutions)
    {
        String res = String(resolution.x_) + " x " + String(resolution.y_);
        ddlbResolution->AddItem(res);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowSettings::FillControlsFromSettings()
{
    CheckBox *chbFullscreen = (CheckBox*)window->GetChild("chbFullscreen", true);

    chbFullscreen->SetChecked(gSet->Get(FULLSCREEN));

    String currentResolution = String(gSet->Get(WINDOW_WIDTH)) + " x " + String(gSet->Get(WINDOW_HEIGHT));
    ddlbResolution->SetCurrentItem(currentResolution);

    gGameVolume->SetValue(gSet->Get(VOLUME_GAME));
    gChatVolume->SetValue(gSet->Get(VOLUME_CHAT));
    gMicLevel->SetValue(gSet->Get(MIC_LEVEL));
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowSettings::Hide()
{
    window->SetVisible(false);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowSettings::Show()
{
    window->SetVisible(true);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowSettings::HandleButtonApplyChanges(StringHash, VariantMap&)
{
    // Screen mode
    bool fullscreen = gSet->Get(FULLSCREEN);
    int width = gSet->Get(WINDOW_WIDTH);
    int height = gSet->Get(WINDOW_HEIGHT);

    CheckBox *chbFullscreen = (CheckBox*)window->GetChild("chbFullscreen", true);
    bool newFullscreen = chbFullscreen->IsChecked();

    String resolution;
    ddlbResolution->CurrentItem(resolution);
    Vector<String> numbers = resolution.Split(' ');

    int newWidth = ToInt(numbers[0]);
    int newHeight = ToInt(numbers[2]);

    if(fullscreen != newFullscreen || width != newWidth || height != newHeight)
    {
        if(newFullscreen != fullscreen)
        {
            gGraphics->ToggleFullscreen();
        }

        gSet->Set(FULLSCREEN, newFullscreen);
        gSet->Set(WINDOW_WIDTH, newWidth);
        gSet->Set(WINDOW_HEIGHT, newHeight);

        gGraphics->SetMode(newWidth, newHeight);

        if(!gGraphics->GetFullscreen())
        {
            IntVector2 desktopResolution = gGraphics->GetDesktopResolution(0);

            int posX = desktopResolution.x_ / 2 - gSet->Get(WINDOW_WIDTH) / 2;
            int posY = desktopResolution.y_ / 2 - gSet->Get(WINDOW_HEIGHT) / 2;

            gGraphics->SetWindowPosition(posX, posY);
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowSettings::HandleButtonReturnToGame(StringHash, VariantMap&)
{
    Hide();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void WindowSettings::HandleChangedScreenMode(StringHash, VariantMap&)
{
    using namespace ScreenMode;

    int width = gUIRoot->GetWidth();
    int height = gUIRoot->GetHeight();

    window->SetPosition(width / 2 - window->GetWidth() / 2, height / 2 - window->GetHeight() / 2);
}
