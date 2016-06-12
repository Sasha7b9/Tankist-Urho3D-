#include <stdafx.h>


#include "Vehicle.h"
#include "Tankist.h"
#include "GlobalVars.h"
#include "../../common/CommonFunctions.h"
#include <Network/Chat.h>

#pragma warning(disable:4100)
URHO3D_DEFINE_APPLICATION_MAIN(Tankist)
#pragma warning(default:4100)


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
        gUIRoot = gUI->GetRoot();
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

    gTankist = this;

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

        gChat = new Chat(gContext);
    }

    gGame = new Game(gContext);
    gGame->Start();
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

    SAFE_DELETE(gGame);
    SAFE_DELETE(gCamera);
    SAFE_DELETE(gLog);
    SAFE_DELETE(gClient);
    SAFE_DELETE(gServer);
    SAFE_DELETE(gScene);
    SAFE_DELETE(gChat);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::CreateScene()
{
    gScene = new Scene(context_);

    gScene->CreateComponent<Octree>(LOCAL);
    gScene->CreateComponent<PhysicsWorld>(LOCAL);

    Node* zoneNode = gScene->CreateChild("Zone", LOCAL);
    Zone* zone = zoneNode->CreateComponent<Zone>();
    zone->SetAmbientColor(Color(0.15f, 0.15f, 0.15f));
    zone->SetFogColor(Color(0.5f, 0.5f, 0.7f));
    zone->SetFogStart(300.0f);
    zone->SetFogEnd(500.0f);
    zone->SetBoundingBox(BoundingBox(-2000.0f, 2000.0f));

    // Create a directional light with cascaded shadow mapping
    Node* lightNode = gScene->CreateChild("DirectionalLight", LOCAL);
    lightNode->SetDirection(Vector3(0.3f, -0.5f, 0.425f));
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetCastShadows(true);
    light->SetShadowBias(BiasParameters(0.00025f, 0.5f));
    light->SetShadowCascade(CascadeParameters(10.0f, 50.0f, 200.0f, 0.0f, 0.8f));
    light->SetSpecularIntensity(0.5f);

    // Create heightmap terrain with collision
    Node* terrainNode = gScene->CreateChild("Terrain");
    terrainNode->SetPosition(Vector3::ZERO);
    Terrain* terrain = terrainNode->CreateComponent<Terrain>();
    terrain->SetPatchSize(64);
    terrain->SetSpacing(Vector3(2.0f, 0.1f, 2.0f)); // Spacing between vertices and vertical resolution of the height map
    terrain->SetSmoothing(false);
    terrain->SetHeightMap(gResourceCache->GetResource<Image>("Textures/HeightMap.png"));
    terrain->SetMaterial(gResourceCache->GetResource<Material>("Materials/Terrain.xml"));
    // The terrain consists of large triangles, which fits well for occlusion rendering, as a hill can occlude all
    // terrain patches and other objects behind it
    terrain->SetOccluder(true);

    RigidBody* body = terrainNode->CreateComponent<RigidBody>();
    body->SetCollisionLayer(2); // Use layer bitmask 2 for static geometry
    CollisionShape* shape = terrainNode->CreateComponent<CollisionShape>();
    shape->SetTerrain();

    // Create 1000 mushrooms in the terrain. Always face outward along the terrain normal
    const unsigned NUM_MUSHROOMS = 1000;
    for(unsigned i = 0; i < NUM_MUSHROOMS; ++i)
    {
        Node* objectNode = gScene->CreateChild("Mushroom", LOCAL);
        Vector3 position(Random(2000.0f) - 1000.0f, 0.0f, Random(2000.0f) - 1000.0f);
        position.y_ = terrain->GetHeight(position) - 0.1f;
        objectNode->SetPosition(position);
        // Create a rotation quaternion from up vector to terrain normal
        objectNode->SetRotation(Quaternion(Vector3::UP, terrain->GetNormal(position)));
        objectNode->SetScale(3.0f);
        StaticModel* object = objectNode->CreateComponent<StaticModel>();
        object->SetModel(gResourceCache->GetResource<Model>("Models/Mushroom.mdl"));
        object->SetMaterial(gResourceCache->GetResource<Material>("Materials/Mushroom.xml"));
        object->SetCastShadows(true);

        RigidBody* body = objectNode->CreateComponent<RigidBody>();
        body->SetCollisionLayer(2);
        CollisionShape* shape = objectNode->CreateComponent<CollisionShape>();
        shape->SetTriangleMesh(object->GetModel(), 0);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::SubscribeToEvents()
{
    if (gTypeApplication == Type_Client)
    {
        SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Tankist, HandleKeyDown));
    }

    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(Tankist, HandlePostUpdate));

    if (gTypeApplication == Type_Server)
    {
        SubscribeToEvent(E_CLOSECONNECTION, URHO3D_HANDLER(Tankist, HandleCloseConnection));

        gNetwork->RegisterRemoteEvent(E_CLIENTOBJECTID);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::CreateUI()
{
    XMLFile* uiStyle = gResourceCache->GetResource<XMLFile>("UI/DefaultStyle.xml");
    // Set style to the UI root so that elements will inherit it
    gUIRoot->SetDefaultStyle(uiStyle);

    // Create a Cursor UI element because we want to be able to hide and show it at will. When hidden, the mouse cursor will
    // control the camera, and when visible, it can interact with the login UI
    SharedPtr<Cursor> cursor(new Cursor(context_));
    cursor->SetStyleAuto(uiStyle);
    gUI->SetCursor(cursor);
    // Set starting position of the cursor at the rendering window center
    cursor->SetPosition(gGraphics->GetWidth() / 2, gGraphics->GetHeight() / 2);

    SharedPtr<UIElement> container(gUIRoot->CreateChild<UIElement>());
    container->SetFixedSize(200, 300);
    container->SetPosition(0, 100);
    container->SetLayoutMode(Urho3D::LM_VERTICAL);
    container->SetStyleAuto();

    statisticsWindow = gUIRoot->CreateChild<Text>();
    statisticsWindow->SetStyleAuto();
    statisticsWindow->SetPosition(gUIRoot->GetWidth() - 200, 0);
    statisticsWindow->SetColor(Urho3D::Color::BLACK);
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
        "Right button mouse - move camera\n"
        "Enter - enter/leave chat\n"
        "ESC - exit"
        );
    instructionText->SetFont(gResourceCache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 15);
    instructionText->SetTextAlignment(Urho3D::HA_CENTER);
    instructionText->SetHorizontalAlignment(Urho3D::HA_CENTER);
    instructionText->SetVerticalAlignment(Urho3D::VA_CENTER);
    instructionText->SetPosition(0, gUI->GetRoot()->GetHeight() / 4);
    instructionText->SetVisible(false);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::UpdateStatisticWindow()
{
    statisticsWindow->SetText(Urho3D::ToString("%d users online\n\n", numClients) +
                              String("speed in = ") + String(bytesInPerSec / 1024.0f) + String(" kB/s\n") +
                              String("speed out = ") + String(bytesOutPerSec / 1024.0f) + String(" kB/s\n") +
                              String("ping = ") + String(ping * 1000.0f) + String(" ms\n\n") +
                              String("load server CPU = ") + String(loadCPU * 100.0f) + " %\n" +
                              String("server in = ") + String(bytesInPerSecServer / 1024.0f) + String(" kB/s\n") +
                              String("server out = ") + String(bytesOutPerSecServer / 1024.0f) + String(" kB/s\n")
                              );
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::SetPing(float pingMS)
{
    this->ping = pingMS;
    UpdateStatisticWindow();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::SetLoadCPU(float loadCPU)
{
    this->loadCPU = loadCPU;
    UpdateStatisticWindow();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::SetNumClients(int numClients)
{
    this->numClients = numClients;
    UpdateStatisticWindow();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::SetBytesInPerSec(float bytesInPerSec)
{
    this->bytesInPerSec = bytesInPerSec;
    UpdateStatisticWindow();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::SetBytesOutPerSec(float bytesOutPerSec)
{
    this->bytesOutPerSec = bytesOutPerSec;
    UpdateStatisticWindow();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::SetBytesInPerSecServer(float bytes)
{
    bytesInPerSecServer = bytes;
    UpdateStatisticWindow();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::SetBytesOutPerSecServer(float bytes)
{
    bytesOutPerSecServer = bytes;
    UpdateStatisticWindow();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tankist::Exit()
{
    engine_->Exit();
}