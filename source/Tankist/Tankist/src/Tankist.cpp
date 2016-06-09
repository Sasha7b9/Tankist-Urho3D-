#include <stdafx.h>


#include "Vehicle.h"
#include "Tankist.h"
#include "GlobalVars.h"
#include "../../common/CommonFunctions.h"

#pragma warning(disable:4100)
URHO3D_DEFINE_APPLICATION_MAIN(Tankist)
#pragma warning(default:4100)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const String INSTRUCTION("instructionText");


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Tankist::Tankist(Context* context) :
    Application(context)
{
    Vehicle::RegisterObject(context);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::Setup()
{
    Vector<String> argumensts = Urho3D::GetArguments();

    if (!ParseArguments(argumensts, gTypeApplication, gIPAddress, gNumPort))
    {
#ifdef _WINDOWS
        URHO3D_LOGERRORF("To rum application type tankist.exe -{client|server} [-ip:xxx.xxx.xxx.xxx] -port:xxx");
#else
        URHO3D_LOGERRORF("To rum application type ./tankist -{client|server} [-ip:xxx.xxx.xxx.xxx] -port:xxx");
#endif
        exit = true;
    }

    gContext = context_;

    gLog = new Log(context_);
    gLog->SetLevel(Urho3D::LOG_DEBUG);

    if (gTypeApplication == Type_Client)
    {
        gLog->Open("client.log");
    }
    else
    {
        gLog->Open("server.log");
    }

    gNetwork = GetSubsystem<Network>();
    gFileSystem = GetSubsystem<FileSystem>();
    gResourceCache = GetSubsystem<ResourceCache>();
    gTime = GetSubsystem<Time>();

    if (gTypeApplication == Type_Client)
    {
        gUI = GetSubsystem<UI>();
        gInput = GetSubsystem<Input>();
        gRenderer = GetSubsystem<Renderer>();
        gCamera = new CameraUni(context_);
    }

    engineParameters_["Headless"] = gTypeApplication == Type_Server;
    engineParameters_["WindowTitle"] = "Tankist WaT";
    engineParameters_["LogName"] = gFileSystem->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
    engineParameters_["FullScreen"] = false;
    engineParameters_["Sound"] = false;
    engineParameters_["WindowWidth"] = 1024;
    engineParameters_["WindowHeight"] = 768;
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
void Tankist::Start()
{
    if (gTypeApplication == Type_Client)
    {
        gGraphics = GetSubsystem<Graphics>();

        SetWindowTitleAndIcon();

        CreateConsoleAndDebugHud();

        CreateUI();
    }

    CreateScene();

    SubscribeToEvents();

    if (gTypeApplication == Type_Server)
    {
        gServer = new Server(context_);
        gServer->Start(gNumPort);

        CreateListFiles();
    }
    
    if (gTypeApplication == Type_Client)
    {
        gCamera = new CameraUni(context_);
        gCamera->SetupViewport();

        gClient = new Client(context_);
        gClient->ConnectToServer();

        CreateInstructions();
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::Stop()
{
    if (gClient)
    {
        gClient->Disconnect();
    }

    engine_->DumpResources(true);
    engine_->DumpProfiler();
    engine_->DumpMemory();
    gLog->Write(0, "out");
    gLog->Close();

    SAFE_DELETE(gCamera);
    SAFE_DELETE(gLog);
    SAFE_DELETE(gClient);
    SAFE_DELETE(gServer);
    SAFE_DELETE(gScene);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::CreateScene()
{
    #include "CreateScene.inl"
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::SubscribeToEvents()
{
    if (gTypeApplication == Type_Client)
    {
        SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Tankist, HandleKeyDown));
    }

    SubscribeToEvent(E_PHYSICSPRESTEP, URHO3D_HANDLER(Tankist, HandlePhysicsPreStep));
    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(Tankist, HandlePostUpdate));

    if (gTypeApplication == Type_Server)
    {
        SubscribeToEvent(E_NEWCONNECTION, URHO3D_HANDLER(Tankist, HandleNewConnection));
        SubscribeToEvent(E_CLOSECONNECTION, URHO3D_HANDLER(Tankist, HandleCloseConnection));

        gNetwork->RegisterRemoteEvent(E_CLIENTOBJECTID);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::HandlePhysicsPreStep(StringHash, VariantMap &)
{
    // Client
    if (gTypeApplication == Type_Client)
    {
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
                controls.Set(CTRL_TOWER_RIGHT, gInput->GetKeyDown('E') | gInput->GetKeyDown(Urho3D::KEY_KP_6));
                controls.Set(CTRL_TOWER_LEFT, gInput->GetKeyDown('Q') | gInput->GetKeyDown(Urho3D::KEY_KP_4));
                controls.Set(CTRL_TRUNK_DOWN, gInput->GetKeyDown(Urho3D::KEY_KP_2));
                controls.Set(CTRL_TRUNK_UP, gInput->GetKeyDown(Urho3D::KEY_KP_8));
            }

            serverConnection->SetControls(controls);
            //serverConnection->SetPosition(cameraNode->GetPosition());
        }
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
                vehicle->controls.Set(CTRL_TOWER_LEFT, ((controls.buttons_ & CTRL_TOWER_LEFT) != 0));
                vehicle->controls.Set(CTRL_TOWER_RIGHT, ((controls.buttons_ & CTRL_TOWER_RIGHT) != 0));
                vehicle->controls.Set(CTRL_TRUNK_UP, ((controls.buttons_ & CTRL_TRUNK_UP) != 0));
                vehicle->controls.Set(CTRL_TRUNK_DOWN, ((controls.buttons_ & CTRL_TRUNK_DOWN) != 0));
            }
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::HandlePostUpdate(StringHash, VariantMap &)
{
    if (gTypeApplication == Type_Client)
    {
        MoveCamera();
    }

    if (exit)
    {
        engine_->Exit();
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::CreateUI()
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


//---------------------------------------------------------------------------------------------------------------------------------------------------
Vehicle* Tankist::CreateVehicle()
{
    Node* vehicleNode = gScene->CreateChild("Vehicle");
    vehicleNode->SetPosition(Vector3(0.0f, 5.0f, 15.0f));

    Vehicle *vehicle = vehicleNode->CreateComponent<Vehicle>();
    vehicle->Init();

    return vehicle;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::MoveCamera()
{
    if (gTypeApplication != Type_Server)
    gUI->GetCursor()->SetVisible(!gInput->GetMouseButtonDown(MOUSEB_RIGHT));

    gCamera->MoveFromMouse();

    if (gTypeApplication == Type_Client)
    {
        static bool cameraIsAttached = false;

        if (!cameraIsAttached)
        {
            cameraIsAttached = gClient->AttachCameraToNode();
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::SetWindowTitleAndIcon()
{
    Image* icon = gResourceCache->GetResource<Image>("Textures/UrhoIcon.png");
    gGraphics->SetWindowIcon(icon);
    //gGraphics->SetWindowTitle("Tankist WaT");
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::CreateConsoleAndDebugHud()
{
    XMLFile* xmlFile = gResourceCache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    Console* console = engine_->CreateConsole();
    gConsole = GetSubsystem<Console>();
    console->SetDefaultStyle(xmlFile);
    console->GetBackground()->SetOpacity(0.8f);

    gDebugHud = engine_->CreateDebugHud();
    gDebugHud->SetDefaultStyle(xmlFile);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::HandleKeyDown(StringHash /*eventType*/, VariantMap& eventData)
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
        else if(key == Urho3D::KEY_F12)
        {
            UIElement *instr = gUI->GetRoot()->GetChild(INSTRUCTION);
            instr->SetVisible(!instr->IsVisible());
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::HandleNewConnection(StringHash, VariantMap &eventData)
{
    Connection *newConnection = static_cast<Connection*>(eventData[NewConnection::P_CONNECT].GetPtr());

    Vehicle *vehicle = CreateVehicle();
    serverObjects[newConnection] = vehicle;

    VariantMap remoteEventData;
    remoteEventData[P_ID] = vehicle->GetNode()->GetID();
    newConnection->SendRemoteEvent(E_CLIENTOBJECTID, true, remoteEventData);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::HandleCloseConnection(StringHash, VariantMap &eventData)
{
    Connection *connection = static_cast<Connection*>(eventData[CloseConnection::P_CONNECT].GetPtr());
    Vehicle *object = serverObjects[connection];
    if (object)
    {
        object->Delete();
    }
    serverObjects.Erase(connection);

#ifdef _WINDOWS

    engine_->Exit();

#endif
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool Tankist::ParseArguments(Vector<String> &arguments, TypeApplication &type, String &address, unsigned short &port)
{
    if (arguments.Size() == 0)
    {
        return false;
    }

    if (arguments[0] == "-server" && arguments.Size() == 2)
    {
        if (arguments[1].Length() > 6 && GetNumPort(arguments[1], port))     // "-port:"
        {
            type = Type_Server;
            return true;
        }
    }

    if (arguments[0] == "-client" && arguments.Size() == 3)
    {
        if (arguments[1].Length() > 9)   // "-address:"
        {
            address = arguments[1].Substring(9);

            if (GetNumPort(arguments[2], port))
            {
                type = Type_Client;
                return true;
            }
        }
    }

    return false;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool Tankist::GetNumPort(String &str, unsigned short &port)
{
    String strPort = str.Substring(6);

    long int numPort = strtol(strPort.CString(), 0, 10);

    if (numPort == 0L)
    {
        return false;
    }

    port = (unsigned short)numPort;

    return true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::CreateInstructions()
{
    Text *instructionText = gUI->GetRoot()->CreateChild<Text>();
    instructionText->SetText("Press F12 to help");
    instructionText->SetFont(gResourceCache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 10);

    instructionText->SetHorizontalAlignment(Urho3D::HA_LEFT);
    instructionText->SetVerticalAlignment(Urho3D::VA_TOP);
    instructionText->SetPosition(0, 0);

    instructionText = gUI->GetRoot()->CreateChild<Text>(INSTRUCTION);
    instructionText->SetText(
        "W,A,S,D,Q,E,NUM_4,NUM_8,NUM_6,NUM_2 - controls\n"
        "ESC - exit"
        );
    instructionText->SetFont(gResourceCache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 15);
    instructionText->SetTextAlignment(Urho3D::HA_CENTER);
    instructionText->SetHorizontalAlignment(Urho3D::HA_CENTER);
    instructionText->SetVerticalAlignment(Urho3D::VA_CENTER);
    instructionText->SetPosition(0, gUI->GetRoot()->GetHeight() / 4);
    instructionText->SetVisible(false);
}

