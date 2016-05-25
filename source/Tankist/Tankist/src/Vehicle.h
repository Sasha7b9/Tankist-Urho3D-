#pragma once


const int CTRL_FORWARD = 1 << 0;
const int CTRL_BACK = 1 << 1;
const int CTRL_LEFT = 1 << 2;
const int CTRL_RIGHT = 1 << 3;

const float YAW_SENSITIVITY = 0.1f;
const float ENGINE_POWER = 10.0f;
const float DOWN_FORCE = 10.0f;
const float MAX_WHEEL_ANGLE = 22.5f;

/// Vehicle component, responsible for physical movement according to controls.
class Vehicle : public LogicComponent
{

#pragma warning(push)
#pragma warning(disable:4640)
    URHO3D_OBJECT(Vehicle, LogicComponent)
#pragma warning(pop)

public:
    /// Construct.
    Vehicle(Context* context);
    
    /// Register object factory and attributes.
    static void RegisterObject(Context* context);
    
    /// Perform post-load after deserialization. Acquire the components from the scene nodes.
    virtual void ApplyAttributes();
    /// Handle physics world update. Called by LogicComponent base class.
    virtual void FixedUpdate(float timeStep);
    
    /// Initialize the vehicle. Create rendering and physics components. Called by the application.
    void Init();

    /// Movement controls.
    Controls controls;

    void Delete();
    
private:
    /// Initialize a wheel and remember its scene node and ID.
    void InitWheel(const String& name, const Vector3& offset, WeakPtr<Node>& wheelNode, unsigned& wheelNodeID);
    /// Acquire wheel components from wheel scene nodes.
    void GetWheelComponents();

    // Wheel scene nodes.
    WeakPtr<Node> frontLeft;
    WeakPtr<Node> frontRight;
    WeakPtr<Node> rearLeft;
    WeakPtr<Node> rearRight;
    
    // Steering axle constraints.
    WeakPtr<Constraint> frontLeftAxis;
    WeakPtr<Constraint> frontRightAxis;
    
    // Hull and wheel rigid bodies.
    WeakPtr<RigidBody> hullBody;
    WeakPtr<RigidBody> frontLeftBody;
    WeakPtr<RigidBody> frontRightBody;
    WeakPtr<RigidBody> rearLeftBody;
    WeakPtr<RigidBody> rearRightBody;
    
    // IDs of the wheel scene nodes for serialization.
    unsigned frontLeftID;
    unsigned frontRightID;
    unsigned rearLeftID;
    unsigned rearRightID;
    
    /// Current left/right steering amount (-1 to 1.)
    float steering;

    Vehicle(Vehicle const&) : LogicComponent(nullptr) {};
    Vehicle operator=(Vehicle const&) {};
};
