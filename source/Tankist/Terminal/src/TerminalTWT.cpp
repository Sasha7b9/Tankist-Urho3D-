#include <stdafx.h>


#include "TerminalTWT.h"
#include "Graphics/lSprite.h"


#pragma warning(disable:4100)
URHO3D_DEFINE_APPLICATION_MAIN(TerminalTWT)
#pragma warning(default:4100)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TerminalTWT::TerminalTWT(Context* context) : Application(context)
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void TerminalTWT::Setup()
{
    gUI = GetSubsystem<UI>();
    gUIRoot = gUI->GetRoot();
    gResourceCache = GetSubsystem<ResourceCache>();
    gInput = GetSubsystem<Input>();
    gTime = GetSubsystem<Time>();

    engineParameters_["WindowTitle"] = "Tankist WaT";
    //engineParameters_["LogName"] = gFileSystem->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
    engineParameters_["FullScreen"] = false;
    engineParameters_["Headless"] = false;
    engineParameters_["Sound"] = false;
    engineParameters_["WindowWidth"] = 640;
    engineParameters_["WindowHeight"] = 480;
    //engineParameters_["WindowPositionY"] = 20;
    //engineParameters_["WindowPositionX"] = 20;

    if (!engineParameters_.Contains("ResourcePrefixPaths"))
    {
        engineParameters_["ResourcePrefixPaths"] = ";../share/Resources;../share/Urho3D/Resources";
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void TerminalTWT::Start()
{
    gGraphics = GetSubsystem<Graphics>();

    CreateUI();

    MakeWindow();

    SubscribeToEvents();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void TerminalTWT::Stop()
{

}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void TerminalTWT::SubscribeToEvents()
{
    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(TerminalTWT, HandlePostUpdate));
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void TerminalTWT::CreateUI()
{
    XMLFile *uiStyle = gResourceCache->GetResource<XMLFile>("UI/DefaultStyle.xml");
    gUIRoot->SetDefaultStyle(uiStyle);

    SharedPtr<Cursor> cursor(new Cursor(context_));
    cursor->SetStyleAuto(uiStyle);
    gUI->SetCursor(cursor);
    cursor->SetPosition(gGraphics->GetWidth() / 2, gGraphics->GetHeight() / 2);

    gInput->SetMouseVisible(true);

    ///
    int width = engineParameters_["WindowWidth"].GetInt();
    int height = engineParameters_["WindowHeight"].GetInt();

    ///
    label = new Text(context_);
    label->SetFont(gResourceCache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 50);
    label->SetColor(Color::YELLOW);
    label->SetText(L"Танкист МК");
    label->SetPosition((width - label->GetWidth()) / 2, 10);
    gUIRoot->AddChild(label);

    ///
    progressBar = new ProgressBar(context_);
    progressBar->SetPosition((width - progressBar->GetWidth()) / 2, height - 50);

    ///
    textVerifyUpdate = new Text(context_);
    textVerifyUpdate->SetFont(gResourceCache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 13);
    textVerifyUpdate->SetColor(Color::GREEN);
    textVerifyUpdate->SetText(L"Проверяю наличие обновлений...");
    textVerifyUpdate->SetPosition((width - textVerifyUpdate->GetWidth()) / 2, height - 100);

    ///
    buttonPlay = new Button(context_);
    buttonPlay->SetStyleAuto();
    SharedPtr<Text> lbl(new Text(context_));
    lbl->SetFont(gResourceCache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 15);
    lbl->SetAlignment(Urho3D::HA_CENTER, Urho3D::VA_CENTER);
    lbl->SetText(L"ИГРАТЬ");
    buttonPlay->AddChild(lbl);
    buttonPlay->SetFixedSize(100, 30);
    buttonPlay->SetPosition((width - buttonPlay->GetWidth()) / 2, height - 100);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void TerminalTWT::MakeWindow()
{
    gUIRoot->RemoveChild(progressBar);
    gUIRoot->RemoveChild(textVerifyUpdate);
    gUIRoot->RemoveChild(buttonPlay);

    if(stateWindow == StateWindow::Start)
    {
        gUIRoot->AddChild(textVerifyUpdate);
    }
    else if(stateWindow == StateWindow::UpdateInProcess)
    {
        gUIRoot->AddChild(progressBar);
    }
    else if(stateWindow == StateWindow::UpdateComplete)
    {
        gUIRoot->AddChild(buttonPlay);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void TerminalTWT::HandlePostUpdate(StringHash , VariantMap &)
{
    static float timeEnterComplete = 0.0f;
    static float timeEnterStart = 0.0f;
    if(stateWindow == StateWindow::Start)
    {
        if(gTime->GetElapsedTime() - timeEnterStart >= 1.0f)
        {
            stateWindow = StateWindow::UpdateInProcess;
            progressBar->SetProgress(0.0f);
            MakeWindow();
        }
    }
    else if(stateWindow == StateWindow::UpdateInProcess)
    {
        static float speed = 1.0f;

        float time = gTime->GetTimeStep();

        float delta = time * speed;

        float progress = progressBar->GetProgress();

        progress += delta;

        if(progress >= 1.0f)
        {
            progressBar->SetProgress(1.0f);
            stateWindow = StateWindow::UpdateComplete;
            MakeWindow();
            timeEnterComplete = gTime->GetElapsedTime();
        }
        else
        {
            progressBar->SetProgress(progress);
        }
    }
    else if(stateWindow == StateWindow::UpdateComplete)
    {
        if(gTime->GetElapsedTime() - timeEnterComplete >= 1.0f)
        {
            stateWindow = StateWindow::Start;
            timeEnterStart = gTime->GetElapsedTime();
            MakeWindow();
        }
    }
}
