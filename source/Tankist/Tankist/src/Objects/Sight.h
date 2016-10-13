#pragma once


class Sight : public Window
{
#pragma warning(push)
#pragma warning(disable:4640)
    URHO3D_OBJECT(Sight, Window);
#pragma warning(pop)

public:
    Sight(Context *context);

private:
    Sight(Sight const&) : Window(nullptr) {};
    Sight operator=(Sight const&) {};
};