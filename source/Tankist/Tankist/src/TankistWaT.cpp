#include <stdafx.h>


#include "Vehicle.h"
#include "TankistWaT.h"
#include "GlobalVars.h"

#pragma warning(disable:4100)
URHO3D_DEFINE_APPLICATION_MAIN(TankistWaT)
#pragma warning(default:4100)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TankistWaT::TankistWaT(Context* context) :
    Application(context)
{
    Vehicle::RegisterObject(context);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void TankistWaT::Setup()
{
    const Vector<String> argumensts = Urho3D::GetArguments();

#ifdef _UNIVERSAL_
    gTypeConnection = (argumensts.Size() > 0 && argumensts[0] == "-server") ? Connection_Server : Connection_Client;
#elif  _CLIENT_
    gTypeConnection = Connection_Client;
#elif _SERVER_
    gTypeConnection = Connection_Server;
#endif

    gNetwork = GetSubsystem<Network>();
    gFileSystem = GetSubsystem<FileSystem>();

#ifndef _SERVER_
    gUI = GetSubsystem<UI>();
    gInput = GetSubsystem<Input>();
    gRenderer = GetSubsystem<Renderer>();
#endif

    gResourceCache = GetSubsystem<ResourceCache>();
    gTime = GetSubsystem<Time>();

#ifndef _SERVER_
    if (gTypeConnection == Connection_Client)
    {
        gCamera = new CameraUni(context_);
    }
#endif

#ifdef _SERVER_
    engineParameters_["Headless"] = true;
#endif

    engineParameters_["WindowTitle"] = gTypeConnection == Connection_Server ? "Tankist WaT server" : "Tankist WaT";
    engineParameters_["LogName"] = gFileSystem->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
    engineParameters_["FullScreen"] = false;
    engineParameters_["Sound"] = false;
    engineParameters_["WindowWidth"] = 640;
    engineParameters_["WindowHeight"] = 480;
    //engineParameters_["WindowPositionY"] = 20;
    //engineParameters_["WindowPositionX"] = gTypeConnection == Connection_Server ? 20 : 700;

    // Construct a search path to find the resource prefix with two entries:
    // The first entry is an empty path which will be substituted with program/bin directory -- this entry is for binary when it is still in build tree
    // The second and third entries are possible relative paths from the installed program/bin directory to the asset directory -- these entries are for binary when it is in the Urho3D SDK installation location
    if (!engineParameters_.Contains("ResourcePrefixPaths"))
    {
        engineParameters_["ResourcePrefixPaths"] = ";../share/Resources;../share/Urho3D/Resources";
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void TankistWaT::Start()
{
    gLog = new Log(context_);
    gLog->SetLevel(LOG_INFO);
    gLog->Open("client.log");

#ifndef _SERVER_
    gGraphics = GetSubsystem<Graphics>();

    SetWindowTitleAndIcon();

    CreateConsoleAndDebugHud();

    CreateUI();
#endif

    CreateScene();

    SubscribeToEvents();

    if (gTypeConnection == Connection_Server)
    {
#ifndef _SERVER_
        gCamera = new CameraUni(context_);
        gCamera->SetupViewport();
#endif
        gServer = new Server(context_);
        gServer->Start(SERVER_PORT);
    }
    else
    {
#ifndef _SERVER_
        gClient = new Client(context_);
        gClient->ConnectToServer();
#endif
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void TankistWaT::Stop()
{
    engine_->DumpResources(true);
    gLog->Write(0, "out");
    gLog->Close();

#ifndef _SERVER_
    SAFE_DELETE(gCamera);
#endif

    SAFE_DELETE(gLog);

#ifndef _SERVER_
    SAFE_DELETE(gClient);
#endif

    SAFE_DELETE(gServer);
    SAFE_DELETE(gScene);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void TankistWaT::CreateScene()
{
    #include "CreateScene.inl"
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void TankistWaT::SubscribeToEvents()
{
#ifndef _SERVER_
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(TankistWaT, HandleKeyDown));
#endif

    SubscribeToEvent(E_PHYSICSPRESTEP, URHO3D_HANDLER(TankistWaT, HandlePhysicsPreStep));
    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(TankistWaT, HandlePostUpdate));

    if (gTypeConnection == Connection_Server)
    {
        SubscribeToEvent(E_NEWCONNECTION, URHO3D_HANDLER(TankistWaT, HandleNewConnection));
        SubscribeToEvent(E_CLOSECONNECTION, URHO3D_HANDLER(TankistWaT, HandleCloseConnection));

        gNetwork->RegisterRemoteEvent(E_CLIENTOBJECTID);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void TankistWaT::HandlePhysicsPreStep(StringHash, VariantMap &)
{
    // Client
    if (gTypeConnection == Connection_Client)
    {
#ifndef _SERVER_
        Connection *serverConnection = gNetwork->GetServerConnection();

        if (serverConnection)
        {
            Controls controls;

            if (!gUI->GetFocusElement())
            {
                controls.Set(CTRL_FORWARD, gInput->GetKeyDown('W'));
                controls.Set(CTRL_BACK, gInput->GetKeyDown('S'));
                controls.Set(CTRL_LEFT, gInput->GetKeyDown('A'));
                controls.Set(CTRL_RIGHT, gInput->GetKeyDown('D'));
            }

            serverConnection->SetControls(controls);
            //serverConnection->SetPosition(cameraNode->GetPosition());
        }
#endif
    }
    // Server
    else
    {
        if (gNetwork->IsServerRunning())
        {
            const Vector<SharedPtr<Connection>> &connections = gNetwork->GetClientConnections();

            for (uint i = 0; i < connections.Size(); ++i)
            {
                Connection *connection = connections[i];
                WeakPtr<Vehicle> vehicle = serverObjects[connection];
                if (!vehicle)
                {
                    continue;
                }

                const Controls &controls = connection->GetControls();

                vehicle->controls.Set(CTRL_FORWARD, ((controls.buttons_ & CTRL_FORWARD) > 0));
                vehicle->controls.Set(CTRL_BACK, ((controls.buttons_ & CTRL_BACK) > 0));
                vehicle->controls.Set(CTRL_LEFT, ((controls.buttons_ & CTRL_LEFT) > 0));
                vehicle->controls.Set(CTRL_RIGHT, ((controls.buttons_ & CTRL_RIGHT) > 0));
            }
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void TankistWaT::HandlePostUpdate(StringHash, VariantMap &)
{
#ifndef _SERVER_
    MoveCamera();
#endif
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef _SERVER_
void TankistWaT::CreateUI()
{
    UIElement* root = gUI->GetRoot();
    XMLFile* uiStyle = gResourceCache->GetResource<XMLFile>("UI/DefaultStyle.xml");
    // Set style to the UI root so that elements will inherit it
    root->SetDefaultStyle(uiStyle);

    // Create a Cursor UI element because we want to be able to hide and show it at will. When hidden, the mouse cursor will
    // control the camera, and when visible, it can interact with the login UI
    SharedPtr<Cursor> cursor(new Cursor(context_));
    cursor->SetStyleAuto(uiStyle);
    gUI->SetCursor(cursor);
    // Set starting position of the cursor at the rendering window center
    cursor->SetPosition(gGraphics->GetWidth() / 2, gGraphics->GetHeight() / 2);
}
#endif


//---------------------------------------------------------------------------------------------------------------------------------------------------
Vehicle* TankistWaT::CreateVehicle()
{
    Node* vehicleNode = gScene->CreateChild("Vehicle");
    vehicleNode->SetPosition(Vector3(0.0f, 5.0f, 15.0f));

    Vehicle *vehicle = vehicleNode->CreateComponent<Vehicle>();
    vehicle->Init();

    return vehicle;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef _SERVER_
void TankistWaT::MoveCamera()
{
    gUI->GetCursor()->SetVisible(!gInput->GetMouseButtonDown(MOUSEB_RIGHT));

    gCamera->MoveFromMouse();

    if (gTypeConnection == Connection_Client)
    {
        static bool cameraIsAttached = false;

        if (!cameraIsAttached)
        {
            cameraIsAttached = gClient->AttachCameraToNode();
        }
    }
}
#endif

//---------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef _SERVER_
void TankistWaT::SetWindowTitleAndIcon()
{
    Image* icon = gResourceCache->GetResource<Image>("Textures/UrhoIcon.png");
    gGraphics->SetWindowIcon(icon);
    //gGraphics->SetWindowTitle("Tankist WaT");
}
#endif


//---------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef _SERVER_
void TankistWaT::CreateConsoleAndDebugHud()
{
    XMLFile* xmlFile = gResourceCache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    Console* console = engine_->CreateConsole();
    gConsole = GetSubsystem<Console>();
    console->SetDefaultStyle(xmlFile);
    console->GetBackground()->SetOpacity(0.8f);

    gDebugHud = engine_->CreateDebugHud();
    gDebugHud->SetDefaultStyle(xmlFile);
}
#endif


//---------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef _SERVER_
void TankistWaT::HandleKeyDown(StringHash /*eventType*/, VariantMap& eventData)
{
    using namespace Urho3D::KeyDown;

    int key = eventData[P_KEY].GetInt();

    // Close console (if open) or exit when ESC is pressed
    if (key == KEY_ESC)
    {
        if (gConsole->IsVisible())
        {
            gConsole->SetVisible(false);
        }
        else
        {
            engine_->Exit();
        }
    }

    // Toggle console with F1
    else if (key == KEY_F1)
        gConsole->Toggle();

    // Toggle debug HUD with F2
    else if (key == KEY_F2)
    {
        if (gDebugHud->GetMode() == 0 || gDebugHud->GetMode() == DEBUGHUD_SHOW_ALL_MEMORY)
        {
            gDebugHud->SetMode(DEBUGHUD_SHOW_ALL);
        }
        else
        {
            gDebugHud->SetMode(DEBUGHUD_SHOW_NONE);
        }
    }
    else if (key == KEY_F3)
    {
        if (gDebugHud->GetMode() == 0 || gDebugHud->GetMode() == DEBUGHUD_SHOW_ALL)
        {
            gDebugHud->SetMode(DEBUGHUD_SHOW_ALL_MEMORY);
        }
        else
        {
            gDebugHud->SetMode(DEBUGHUD_SHOW_NONE);
        }
    }

    // Common rendering quality controls, only when UI has no focused element
    else if (!gUI->GetFocusElement())
    {
        // Texture quality
        if (key == '1')
        {
            int quality = gRenderer->GetTextureQuality();
            ++quality;
            if (quality > QUALITY_HIGH)
            {
                quality = QUALITY_LOW;
            }
            gRenderer->SetTextureQuality(quality);
        }

        // Material quality
        else if (key == '2')
        {
            int quality = gRenderer->GetMaterialQuality();
            ++quality;
            if (quality > QUALITY_HIGH)
            {
                quality = QUALITY_LOW;
            }
            gRenderer->SetMaterialQuality(quality);
        }

        // Specular lighting
        else if (key == '3')
        {
            gRenderer->SetSpecularLighting(!gRenderer->GetSpecularLighting());
        }

        // Shadow rendering
        else if (key == '4')
        {
            gRenderer->SetDrawShadows(!gRenderer->GetDrawShadows());
        }

        // Shadow map resolution
        else if (key == '5')
        {
            int shadowMapSize = gRenderer->GetShadowMapSize();
            shadowMapSize *= 2;
            if (shadowMapSize > 2048)
            {
                shadowMapSize = 512;
            }
            gRenderer->SetShadowMapSize(shadowMapSize);
        }

        // Shadow depth and filtering quality
        else if (key == '6')
        {
            ShadowQuality quality = gRenderer->GetShadowQuality();
            quality = (ShadowQuality)(quality + 1);
            if (quality > SHADOWQUALITY_BLUR_VSM)
            {
                quality = SHADOWQUALITY_SIMPLE_16BIT;
            }
            gRenderer->SetShadowQuality(quality);
        }

        // Occlusion culling
        else if (key == '7')
        {
            bool occlusion = gRenderer->GetMaxOccluderTriangles() > 0;
            occlusion = !occlusion;
            gRenderer->SetMaxOccluderTriangles(occlusion ? 5000 : 0);
        }

        // Instancing
        else if (key == '8')
        {
            gRenderer->SetDynamicInstancing(!gRenderer->GetDynamicInstancing());
        }

        // Take screenshot
        else if (key == '9')
        {
            Image screenshot(context_);
            gGraphics->TakeScreenShot(screenshot);
            // Here we save in the Data folder with date and time appended
            screenshot.SavePNG(gFileSystem->GetProgramDir() + "Data/Screenshot_" +
                               Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_') + ".png");
        }
    }
}
#endif


//---------------------------------------------------------------------------------------------------------------------------------------------------
void TankistWaT::HandleNewConnection(StringHash, VariantMap &eventData)
{
    Connection *newConnection = static_cast<Connection*>(eventData[NewConnection::P_CONNECT].GetPtr());

    Vehicle *vehicle = CreateVehicle();
    serverObjects[newConnection] = vehicle;

    VariantMap remoteEventData;
    remoteEventData[P_ID] = vehicle->GetNode()->GetID();
    newConnection->SendRemoteEvent(E_CLIENTOBJECTID, true, remoteEventData);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void TankistWaT::HandleCloseConnection(StringHash, VariantMap &eventData)
{
    Connection *connection = static_cast<Connection*>(eventData[CloseConnection::P_CONNECT].GetPtr());
    Vehicle *object = serverObjects[connection];
    if (object)
    {
        object->Delete();
    }
    serverObjects.Erase(connection);
}
