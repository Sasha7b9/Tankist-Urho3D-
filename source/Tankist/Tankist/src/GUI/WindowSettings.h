#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WindowSettings : public Object
{
    URHO3D_OBJECT(WindowSettings, Object)

public:
    WindowSettings(Context *context);

    void Hide();

    void Show();

private:
    WindowSettings(WindowSettings const&) : Object(nullptr) {};
    WindowSettings operator=(WindowSettings const&) {};

    SharedPtr<UIElement> window;

    DropDownListButtons *ddlbResolution = nullptr;

    Governor *gGameVolume = nullptr;

    Governor *gChatVolume = nullptr;

    Governor *gMicLevel = nullptr;

    void FillDropDownListResolutions();

    void FillControlsFromSettings();

    void HandleButtonApplyChanges(StringHash, VariantMap&);

    void HandleButtonReturnToGame(StringHash, VariantMap&);

    void HandleChangedScreenMode(StringHash, VariantMap&);
};
