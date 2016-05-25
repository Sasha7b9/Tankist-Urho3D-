#pragma once


#include "Graphics/ProgressBar.h"


enum StateWindow
{
    Start,
    UpdateInProcess,
    UpdateComplete
};


class TerminalTWT : public Application
{

#pragma warning(push)
#pragma warning(disable:4640)
    URHO3D_OBJECT(TerminalTWT, Application);
#pragma warning(pop)

public:
    TerminalTWT(Context* context);

    virtual void Setup();
    virtual void Start();
    virtual void Stop();

private:
    TerminalTWT(TerminalTWT const&) : Application(nullptr) {};
    TerminalTWT operator=(TerminalTWT const &) {};

    StateWindow stateWindow = StateWindow::Start;
    SharedPtr<ProgressBar> progressBar;
    SharedPtr<Text> label;
    SharedPtr<Text> textVerifyUpdate;
    SharedPtr<Button> buttonPlay;

    void CreateUI();
    void MakeWindow();
    void SubscribeToEvents();

    void HandlePostUpdate(StringHash eventType, VariantMap &eventData);
};