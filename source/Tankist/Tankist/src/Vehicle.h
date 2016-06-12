#pragma once


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma warning(push)        
#pragma warning(disable:4100 4251 4266 4275 4365 4625 4626 4640)

#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Input/Controls.h>
#include <Urho3D/Physics/Constraint.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Core/Context.h>

using namespace Urho3D;

#pragma warning(pop)


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const int CTRL_FORWARD      = 1 << 0;
const int CTRL_BACK         = 1 << 1;
const int CTRL_LEFT         = 1 << 2;
const int CTRL_RIGHT        = 1 << 3;
const int CTRL_TOWER_LEFT   = 1 << 4;
const int CTRL_TOWER_RIGHT  = 1 << 5;
const int CTRL_TRUNK_UP     = 1 << 6;
const int CTRL_TRUNK_DOWN   = 1 << 7;

const float YAW_SENSITIVITY = 0.1f;
const float ENGINE_POWER = 5.0f;
const float DOWN_FORCE = 10.0f;
const float MAX_WHEEL_ANGLE = 22.5f;
const float SPEED_TOWER_ROTATION = 8.0f;
const float SPEED_TRUNK_ROTATION = 10.0f;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vehicle component, responsible for physical movement according to controls.
class Vehicle : public Urho3D::LogicComponent
{

#pragma warning(push)
#pragma warning(disable:4640)
    URHO3D_OBJECT(Vehicle, Urho3D::LogicComponent)
#pragma warning(pop)

public:
    /// Construct.
    Vehicle(Urho3D::Context* context);
    
    /// Register object factory and attributes.
    static void RegisterObject(Context* context);
    
    /// Handle physics world update. Called by LogicComponent base class.
    virtual void FixedUpdate(float timeStep);
    
    /// Initialize the vehicle. Create rendering and physics components. Called by the application.
    void Init();

    /// Movement controls.
    Controls controls;

    void Delete();

    unsigned towerID;
    
private:
    /// Initialize a wheel and remember its scene node and ID.
    void InitWheel(const String& name, const Vector3& offset, WeakPtr<Node>& wheelNode, unsigned& wheelNodeID);
    /// Acquire wheel components from wheel scene nodes.
    void GetWheelComponents();

    void InitTower();

    void InitTrunk();

    void RotateTrunk(float delta);

    // Wheel scene nodes.
    WeakPtr<Node> frontLeft;
    WeakPtr<Node> frontRight;
    WeakPtr<Node> rearLeft;
    WeakPtr<Node> rearRight;
    WeakPtr<Node> nodeTower;
    WeakPtr<Node> nodeTrunk;
    
    // Steering axle constraints.
    WeakPtr<Constraint> frontLeftAxis;
    WeakPtr<Constraint> frontRightAxis;
    
    // Hull and wheel rigid bodies.
    WeakPtr<RigidBody> hullBody;
    WeakPtr<RigidBody> frontLeftBody;
    WeakPtr<RigidBody> frontRightBody;
    WeakPtr<RigidBody> rearLeftBody;
    WeakPtr<RigidBody> rearRightBody;
    WeakPtr<RigidBody> towerBody;
    
    // IDs of the wheel scene nodes for serialization.
    unsigned frontLeftID;
    unsigned frontRightID;
    unsigned rearLeftID;
    unsigned rearRightID;
    unsigned trunkID;

    float pitchTower = 0.0f;

    float yawTrunkMin = -110.0f;
    float yawTrunkMax = -35.0f;
    float yawTrunk = 0.0f;
    
    /// Current left/right steering amount (-1 to 1.)
    float steering;

    Vehicle(Vehicle const&) : LogicComponent(nullptr) {};
    Vehicle operator=(Vehicle const&) {};
};
