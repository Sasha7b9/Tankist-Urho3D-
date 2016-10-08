//
// Copyright (c) 2008-2016 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/Constraint.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>

#include "Vehicle.h"
#include "VehicleDemo.h"

#include <Urho3D/DebugNew.h>

const float CAMERA_DISTANCE = 10.0f;

URHO3D_DEFINE_APPLICATION_MAIN(VehicleDemo)

VehicleDemo::VehicleDemo(Context* context) :
    Sample(context)
{
    // Register factory and attributes for the Vehicle component so it can be created via CreateComponent, and loaded / saved
    Vehicle::RegisterObject(context);
}

void VehicleDemo::Start()
{
    // Execute base class startup
    Sample::Start();

    // Create static scene content
    CreateScene();

    // Create the controllable vehicle
    CreateVehicle();

    // Subscribe to necessary events
    SubscribeToEvents();

    // Set the mouse mode to use in the sample
    Sample::InitMouseMode(MM_RELATIVE);
}

void VehicleDemo::CreateScene()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    cache->AddResourceDir(GetSubsystem<FileSystem>()->GetProgramDir() + "TankistData");

    scene_ = new Scene(context_);

    scene_->CreateComponent<DebugRenderer>();

    // Create scene subsystem components
    scene_->CreateComponent<Octree>();
    scene_->CreateComponent<PhysicsWorld>();

    PhysicsWorld *physics = scene_->GetComponent<PhysicsWorld>();
    //physics->SetInterpolation(true);
    //physics->SetInternalEdge(true);
    //physics->SetSplitImpulse(true);
    physics->SetFps(100);

    // Create camera and define viewport. We will be doing load / save, so it's convenient to create the camera outside the scene,
    // so that it won't be destroyed and recreated, and we don't have to redefine the viewport on load
    cameraNode_ = new Node(context_);
    Camera* camera = cameraNode_->CreateComponent<Camera>();
    camera->SetFarClip(500.0f);
    GetSubsystem<Renderer>()->SetViewport(0, new Viewport(context_, scene_, camera));

    // Create static scene content. First create a zone for ambient lighting and fog control
    Node* zoneNode = scene_->CreateChild("Zone");
    Zone* zone = zoneNode->CreateComponent<Zone>();
    zone->SetAmbientColor(Color(0.15f, 0.15f, 0.15f));
    zone->SetFogColor(Color(0.5f, 0.5f, 0.7f));
    zone->SetFogStart(300.0f);
    zone->SetFogEnd(500.0f);
    zone->SetBoundingBox(BoundingBox(-2000.0f, 2000.0f));

    // Create a directional light with cascaded shadow mapping
    Node* lightNode = scene_->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.3f, -0.5f, 0.425f));
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetCastShadows(true);
    light->SetShadowBias(BiasParameters(0.00025f, 0.5f));
    light->SetShadowCascade(CascadeParameters(10.0f, 50.0f, 200.0f, 0.0f, 0.8f));
    light->SetSpecularIntensity(0.5f);

    // Create heightmap terrain with collision
    Node* terrainNode = scene_->CreateChild("Terrain");
    terrainNode->SetPosition(Vector3::ZERO);
    Terrain* terrain = terrainNode->CreateComponent<Terrain>();
    terrain->SetPatchSize(64);
    terrain->SetSpacing(Vector3(2.0f, 0.1f, 2.0f)); // Spacing between vertices and vertical resolution of the height map
    terrain->SetSmoothing(true);
    terrain->SetHeightMap(cache->GetResource<Image>("Textures/HeightMap.png"));
    terrain->SetMaterial(cache->GetResource<Material>("Materials/Terrain.xml"));
    // The terrain consists of large triangles, which fits well for occlusion rendering, as a hill can occlude all
    // terrain patches and other objects behind it
    terrain->SetOccluder(true);

    RigidBody* body = terrainNode->CreateComponent<RigidBody>();
    body->SetCollisionLayer(2); // Use layer bitmask 2 for static geometry
    //body->SetContactProcessingThreshold(1.0f);
    body->SetRestitution(0.0f);
    body->SetFriction(1.0f);
    //body->SetCcdRadius(10000.0f);
    //body->SetCcdMotionThreshold(10000.0f);
    CollisionShape* shape = terrainNode->CreateComponent<CollisionShape>();
    shape->SetTerrain();

    // Create 1000 mushrooms in the terrain. Always face outward along the terrain normal
    const unsigned NUM_MUSHROOMS = 1000;
    for (unsigned i = 0; i < NUM_MUSHROOMS; ++i)
    {
        Node* objectNode = scene_->CreateChild("Mushroom");
        Vector3 position(Random(2000.0f) - 1000.0f, 0.0f, Random(2000.0f) - 1000.0f);
        position.y_ = terrain->GetHeight(position) - 0.1f;
        objectNode->SetPosition(position);
        // Create a rotation quaternion from up vector to terrain normal
        objectNode->SetRotation(Quaternion(Vector3::UP, terrain->GetNormal(position)));
        objectNode->SetScale(3.0f);
        StaticModel* object = objectNode->CreateComponent<StaticModel>();
        object->SetModel(cache->GetResource<Model>("Models/Mushroom.mdl"));
        object->SetMaterial(cache->GetResource<Material>("Materials/Mushroom.xml"));
        object->SetCastShadows(true);

        RigidBody* body = objectNode->CreateComponent<RigidBody>();
        body->SetCollisionLayer(2);
        CollisionShape* shape = objectNode->CreateComponent<CollisionShape>();
        shape->SetTriangleMesh(object->GetModel(), 0);
    }

    return;

    const unsigned NUM_BOXES = 200;

    float y = 20.0f;

    for(unsigned i = 0; i < NUM_BOXES; i++)
    {
        Node *boxNode = scene_->CreateChild("Box");
        //Vector3 position(Random(2000.0f) - 1000.0f, 20.0f, Random(2000.0f) - 1000.0f);
        Vector3 position(0.0f, y, 0.0f);
        y += 1.0f;
        boxNode->SetPosition(position);
        Vector3 scale(2.0f, 0.25f, 2.0f);
        boxNode->SetScale(scale);
        StaticModel* boxObject = boxNode->CreateComponent<StaticModel>();
        boxObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
        boxObject->SetMaterial(cache->GetResource<Material>("Materials/StoneEnvMapSmall.xml"));
        boxObject->SetCastShadows(true);

        // Create RigidBody and CollisionShape components like above. Give the RigidBody mass to make it movable
        // and also adjust friction. The actual mass is not important; only the mass ratios between colliding
        // objects are significant
        RigidBody* body = boxNode->CreateComponent<RigidBody>();
        body->SetCollisionLayer(2);
        body->SetMass(10000000.0f);
        body->SetFriction(1.0f);
        //body->SetCcdRadius(2.1f / 2.0f);
        //body->SetCcdMotionThreshold(0.25f);
        CollisionShape* shape = boxNode->CreateComponent<CollisionShape>();
        shape->SetBox(Vector3::ONE);
    }
}


void VehicleDemo::AddNewCube()
{
    static float prevTime = 0.0;
    static float timePrevCube = 0.0f;

    static int counter = 0;

    static float x = -100.0f;
    static float z = -100.0f;

    float delta = GetSubsystem<Time>()->GetElapsedTime() - prevTime;

    prevTime = GetSubsystem<Time>()->GetElapsedTime();

    if(delta > 0.02f)
    {
        return;
    }

    if(prevTime - timePrevCube < 0.1f)
    {
        //return;
    }

    timePrevCube = prevTime;

    for(unsigned i = 0; i < 1; i++)
    {
        ResourceCache* cache = GetSubsystem<ResourceCache>();

        prevTime = GetSubsystem<Time>()->GetElapsedTime();

        Node *boxNode = scene_->CreateChild("Box");
        //Vector3 position(Random(2000.0f) - 1000.0f, 20.0f, Random(2000.0f) - 1000.0f);
        Vector3 position(Random(-50.0f, 50.0f), 50.0f, Random(-50.0f, 50.0f));
        boxNode->SetPosition(position);
        boxNode->SetRotation(Quaternion(Random(100.0f), Random(100.0f), Random(100.0f)));
        Vector3 scale(0.2f, 2.0f, 2.0f);
        boxNode->SetScale(scale);
        StaticModel* boxObject = boxNode->CreateComponent<StaticModel>();
        boxObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
        boxObject->SetMaterial(cache->GetResource<Material>("Materials/StoneEnvMapSmall.xml"));
        boxObject->SetCastShadows(true);

        // Create RigidBody and CollisionShape components like above. Give the RigidBody mass to make it movable
        // and also adjust friction. The actual mass is not important; only the mass ratios between colliding
        // objects are significant
        RigidBody* body = boxNode->CreateComponent<RigidBody>();
        body->SetCollisionLayer(2);
        body->SetMass(1000.0f);
        body->SetFriction(1.0f);
        body->SetCcdRadius(2.1f / 2.0f);
        body->SetCcdMotionThreshold(0.2f);
        CollisionShape* shape = boxNode->CreateComponent<CollisionShape>();
        shape->SetBox(Vector3::ONE);

        x += 5.0f;

        if(x == 100.0f && z != 100.0f)
        {
            x = -100.0f;
            z += 5.0f;
        }

        URHO3D_LOGINFOF("%d", counter++);
    }
}


void VehicleDemo::CreateVehicle()
{
    Node* vehicleNode = scene_->CreateChild("Vehicle");
    vehicleNode->SetPosition(Vector3(0.0f, 5.0f, 0.0f));

    // Create the tank logic component
    tank_ = vehicleNode->CreateComponent<Vehicle>();
    // Create the rendering and physics components
    tank_->Init();
}

void VehicleDemo::SubscribeToEvents()
{
    // Subscribe to Update event for setting the vehicle controls before physics simulation
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(VehicleDemo, HandleUpdate));

    // Subscribe to PostUpdate event for updating the camera position after physics simulation
    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(VehicleDemo, HandlePostUpdate));

    // Unsubscribe the SceneUpdate event from base class as the camera node is being controlled in HandlePostUpdate() in this sample
    UnsubscribeFromEvent(E_SCENEUPDATE);

    SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(VehicleDemo, HandlePostRenderUpdate));
}

void VehicleDemo::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    //AddNewCube();

    using namespace Update;

    Input* input = GetSubsystem<Input>();

    if (tank_)
    {
        UI* ui = GetSubsystem<UI>();

        // Get movement controls and assign them to the vehicle component. If UI has a focused element, clear controls
        if (!ui->GetFocusElement())
        {
            tank_->controls_.Set(CTRL_FORWARD, input->GetKeyDown(KEY_W));
            tank_->controls_.Set(CTRL_BACK, input->GetKeyDown(KEY_S));
            tank_->controls_.Set(CTRL_LEFT, input->GetKeyDown(KEY_A));
            tank_->controls_.Set(CTRL_RIGHT, input->GetKeyDown(KEY_D));
//            tank_->controls_.Set(CTRL_STOP, input->GetKeyDown(KEY_KP_ENTER));

            // Add yaw & pitch from the mouse motion or touch input. Used only for the camera, does not affect motion
            if (touchEnabled_)
            {
                for (unsigned i = 0; i < input->GetNumTouches(); ++i)
                {
                    TouchState* state = input->GetTouch(i);
                    if (!state->touchedElement_)    // Touch on empty space
                    {
                        Camera* camera = cameraNode_->GetComponent<Camera>();
                        if (!camera)
                            return;

                        Graphics* graphics = GetSubsystem<Graphics>();
                        tank_->controls_.yaw_ += TOUCH_SENSITIVITY * camera->GetFov() / graphics->GetHeight() * state->delta_.x_;
                        tank_->controls_.pitch_ += TOUCH_SENSITIVITY * camera->GetFov() / graphics->GetHeight() * state->delta_.y_;
                    }
                }
            }
            else
            {
                tank_->controls_.yaw_ += (float)input->GetMouseMoveX() * YAW_SENSITIVITY;
                tank_->controls_.pitch_ += (float)input->GetMouseMoveY() * YAW_SENSITIVITY;
            }
            // Limit pitch
            tank_->controls_.pitch_ = Clamp(tank_->controls_.pitch_, 0.0f, 80.0f);

            // Check for loading / saving the scene
            if (input->GetKeyPress(KEY_F5))
            {
                File saveFile(context_, GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Scenes/VehicleDemo.xml",
                    FILE_WRITE);
                scene_->SaveXML(saveFile);
            }
            if (input->GetKeyPress(KEY_F7))
            {
                File loadFile(context_, GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Scenes/VehicleDemo.xml", FILE_READ);
                scene_->LoadXML(loadFile);
                // After loading we have to reacquire the weak pointer to the Vehicle component, as it has been recreated
                // Simply find the vehicle's scene node by name as there's only one of them
                Node* vehicleNode = scene_->GetChild("Vehicle", true);
                if (vehicleNode)
                    tank_ = vehicleNode->GetComponent<Vehicle>();
            }
        }
        else
            tank_->controls_.Set(CTRL_FORWARD | CTRL_BACK | CTRL_LEFT | CTRL_RIGHT, false);
    }
}

void VehicleDemo::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{
    if (!tank_)
        return;

    Node* vehicleNode = tank_->GetNode();

    // Physics update has completed. Position camera behind vehicle
    Quaternion dir(vehicleNode->GetRotation().YawAngle(), Vector3::UP);
    dir = dir * Quaternion(tank_->controls_.yaw_, Vector3::UP);
    dir = dir * Quaternion(tank_->controls_.pitch_, Vector3::RIGHT);

    Vector3 cameraTargetPos = vehicleNode->GetPosition() - dir * Vector3(0.0f, 0.0f, CAMERA_DISTANCE);
    Vector3 cameraStartPos = vehicleNode->GetPosition();

    // Raycast camera against static objects (physics collision mask 2)
    // and move it closer to the vehicle if something in between
    Ray cameraRay(cameraStartPos, cameraTargetPos - cameraStartPos);
    float cameraRayLength = (cameraTargetPos - cameraStartPos).Length();
    PhysicsRaycastResult result;
    scene_->GetComponent<PhysicsWorld>()->RaycastSingle(result, cameraRay, cameraRayLength, 2);
    if (result.body_)
        cameraTargetPos = cameraStartPos + cameraRay.direction_ * (result.distance_ - 0.5f);

    cameraNode_->SetPosition(cameraTargetPos);
    cameraNode_->SetRotation(dir);
}

void VehicleDemo::HandlePostRenderUpdate(StringHash, VariantMap&)
{
    static bool drawDebug = false;

    if(GetSubsystem<Input>()->GetKeyPress(KEY_SPACE))
    {
        drawDebug = !drawDebug;
    }

    if(drawDebug)
    {
        scene_->GetComponent<PhysicsWorld>()->DrawDebugGeometry(false);
        /*
        DebugRenderer *dRender = scene_->GetComponent<DebugRenderer>();
        if(dRender)
        {
            dRender->AddSphere(Sphere(tank_->hullBody_->GetNode()->GetWorldPosition(), 0.25f), Color::YELLOW, false);
            dRender->Render();
        }
        */
    }
}
