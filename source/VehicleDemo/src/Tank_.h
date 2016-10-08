
#pragma once

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Scene/LogicComponent.h>

namespace Urho3D
{

class Constraint;
class Node;
class RigidBody;

}

using namespace Urho3D;

const int CTRL_FORWARD = 1 << 0;
const int CTRL_BACK = 1 << 1;
const int CTRL_LEFT = 1 << 2;
const int CTRL_RIGHT = 1 << 3;
const int CTRL_STOP = 1 << 4;

const float YAW_SENSITIVITY = 0.1f;
const float ENGINE_POWER = 10.0f;
const float DOWN_FORCE = 10.0f;
const float MAX_WHEEL_ANGLE = 22.5f;

/// Tank component, responsible for physical movement according to controls.
class Tank : public LogicComponent
{
    URHO3D_OBJECT(Tank, LogicComponent)

public:
    /// Construct.
    Tank(Context* context);
    ~Tank();
    
    /// Register object factory and attributes.
    static void RegisterObject(Context* context);
    
    /// Perform post-load after deserialization. Acquire the components from the scene nodes.
    virtual void ApplyAttributes();
    /// Handle physics world update. Called by LogicComponent base class.
    virtual void FixedUpdate(float timeStep);
    
    /// Initialize the vehicle. Create rendering and physics components. Called by the application.
    void Init();
    
    /// Movement controls.
    Controls controls_;

    WeakPtr<RigidBody> hullBody_;
    
private:
    /// Initialize a wheel and remember its scene node and ID.
    void InitWheel(const String& name, const Vector3& offset, WeakPtr<RigidBody>& wheelBody, WeakPtr<RigidBody>& damperBody);
    void InitDamper(const String& name, const Vector3& offset, WeakPtr<RigidBody>& damperBody);
    
    // Wheel scene nodes.
    WeakPtr<RigidBody> wheelBodyLeft[5];
    WeakPtr<RigidBody> wheelBodyRight[5];
};
