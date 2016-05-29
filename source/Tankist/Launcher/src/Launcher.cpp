#include <stdafx.h>


#include "Launcher.h"
#include "Graphics/lSprite.h"
#include "NetworkThread.h"


#pragma warning(disable:4100)
URHO3D_DEFINE_APPLICATION_MAIN(TerminalTWT)
#pragma warning(default:4100)


NetworkThread thread;


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
    gFileSystem = GetSubsystem<FileSystem>();

    engineParameters_["WindowTitle"] = "Tankist WaT";
    engineParameters_["LogName"] = gFileSystem->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
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
    gLog = new Log(context_);
    gLog->SetLevel(Urho3D::LOG_INFO);
    gLog->Open("launcher.log");
    gGraphics = GetSubsystem<Graphics>();

    CreateUI();

    MakeWindow(StateWindow::Start);

    SubscribeToEvents();

    gContext = context_;
    thread.Run();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void TerminalTWT::Stop()
{
    gLog->Write(0, "out");
    gLog->Close();
    thread.Stop();
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
void TerminalTWT::MakeWindow(StateWindow state)
{
    stateWindow = state;

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

    NetworkThread::State stateThread = thread.GetState();

    if(stateThread == NetworkThread::DownloadFile)
    {
        if(stateWindow == StateWindow::Start)
        {
            MakeWindow(StateWindow::UpdateInProcess);
        }
        else
        {
            progressBar->SetBytes(thread.GetBytesAll(), thread.GetBytesRecieved());
            progressBar->SetParameters(thread.GetPercents() / 100.0f, thread.GetPassedTime(), thread.GetElapsedTime(), thread.GetSpeed());
        }
    }
    else if(stateThread == NetworkThread::ConnectClose)
    {
        if(stateWindow == StateWindow::UpdateInProcess)
        {
            MakeWindow(StateWindow::UpdateComplete);
        }
    }
}
