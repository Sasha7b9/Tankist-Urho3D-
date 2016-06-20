#pragma once


class WindowSettings : public Object
{

#pragma warning(push)
#pragma warning(disable:4640)
    URHO3D_OBJECT(WindowSettings, Object)
#pragma warning(pop)

public:
    WindowSettings(Context *context);

    void Hide();
    void Show();

private:
    WindowSettings(WindowSettings const&) : Object(nullptr) {};
    WindowSettings operator=(WindowSettings const&) {};

    SharedPtr<UIElement> window;

    void FillDropDownListResolutions();

    void HandleButtonApplyChanges(StringHash, VariantMap&);
    void HandleButtonReturnToGame(StringHash, VariantMap&);
    void HandleCheckBoxFullscreen(StringHash, VariantMap&);
};
