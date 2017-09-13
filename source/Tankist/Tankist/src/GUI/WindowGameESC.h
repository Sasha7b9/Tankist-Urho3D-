#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WindowGameESC : public Object
{
    URHO3D_OBJECT(WindowGameESC, Object)

public:
    WindowGameESC(Context *context);

    void Hide();
    void Show();

private:
    WindowGameESC(WindowGameESC const&) : Object(nullptr) {};
    WindowGameESC operator=(WindowGameESC const&) {};

    SharedPtr<UIElement> window;

    void HandleButtonExit(StringHash, VariantMap&);
    void HandleButtonSettings(StringHash, VariantMap&);
    void HandleButtonLanguage(StringHash, VariantMap&);
};
