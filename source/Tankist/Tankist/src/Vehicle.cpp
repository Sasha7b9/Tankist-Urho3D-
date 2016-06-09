#include <stdafx.h>


#include "Vehicle.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vehicle::Vehicle(Context* context) :
    LogicComponent(context),
    steering(0.0f)
{
    // Only the physics update event is needed: unsubscribe from the rest for optimization
    SetUpdateEventMask(USE_FIXEDUPDATE);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Vehicle::RegisterObject(Context* context)
{
    context->RegisterFactory<Vehicle>();

    URHO3D_ATTRIBUTE("Controls Yaw", float, controls.yaw_, 0.0f, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Controls Pitch", float, controls.pitch_, 0.0f, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Steering", float, steering, 0.0f, AM_DEFAULT);
    // Register wheel node IDs as attributes so that the wheel nodes can be reaquired on deserialization. They need to be tagged
    // as node ID's so that the deserialization code knows to rewrite the IDs in case they are different on load than on save
    URHO3D_ATTRIBUTE("Front Left Node", int, frontLeftID, 0, AM_DEFAULT | AM_NODEID);
    URHO3D_ATTRIBUTE("Front Right Node", int, frontRightID, 0, AM_DEFAULT | AM_NODEID);
    URHO3D_ATTRIBUTE("Rear Left Node", int, rearLeftID, 0, AM_DEFAULT | AM_NODEID);
    URHO3D_ATTRIBUTE("Rear Right Node", int, rearRightID, 0, AM_DEFAULT | AM_NODEID);
    URHO3D_ATTRIBUTE("Tower Node", int, towerID, 0, AM_DEFAULT | AM_NODEID);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Vehicle::FixedUpdate(float timeStep)
{
    if (!frontLeftAxis)
    {
        return;
    }

    float newSteering = 0.0f;
    float accelerator = 0.0f;

    // Read controls
    if(controls.buttons_ & CTRL_LEFT)
    {
        newSteering = -1.0f;
    }
    if(controls.buttons_ & CTRL_RIGHT)
    {
        newSteering = 1.0f;
    }
    if(controls.buttons_ & CTRL_FORWARD)
    {
        accelerator = 1.5f;
    }
    if(controls.buttons_ & CTRL_BACK)
    {
        accelerator = -0.5f;
    }

    // When steering, wake up the wheel rigidbodies so that their orientation is updated
    if (newSteering != 0.0f)
    {
        frontLeftBody->Activate();
        frontRightBody->Activate();
        steering = steering * 0.95f + newSteering * 0.05f;
    }
    else
    {
        steering = steering * 0.8f + newSteering * 0.2f;
    }

    // Set front wheel angles
    Quaternion steeringRot(0, steering * MAX_WHEEL_ANGLE, 0);
    frontLeftAxis->SetOtherAxis(steeringRot * Vector3::LEFT);
    frontRightAxis->SetOtherAxis(steeringRot * Vector3::RIGHT);

    Quaternion hullRot = hullBody->GetRotation();
    if (accelerator != 0.0f)
    {
        // Torques are applied in world space, so need to take the vehicle & wheel rotation into account
        Vector3 torqueVec = Vector3(ENGINE_POWER * accelerator, 0.0f, 0.0f);

        //frontLeftBody->ApplyTorque(hullRot * steeringRot * torqueVec);
        //frontRightBody->ApplyTorque(hullRot * steeringRot * torqueVec);
        rearLeftBody->ApplyTorque(hullRot * torqueVec);
        rearRightBody->ApplyTorque(hullRot * torqueVec);
    }

    // Apply downforce proportional to velocity
    Vector3 localVelocity = hullRot.Inverse() * hullBody->GetLinearVelocity();
    hullBody->ApplyForce(hullRot * Vector3::DOWN * Abs(localVelocity.z_) * DOWN_FORCE);

    if(controls.buttons_ & CTRL_TOWER_RIGHT)
    {
        pitchTower += SPEED_TOWER_ROTATION * timeStep;
        nodeTower->SetRotation(Quaternion(pitchTower, Vector3::UP));
    }

    if(controls.buttons_ & CTRL_TOWER_LEFT)
    {
        pitchTower -= SPEED_TOWER_ROTATION * timeStep;
        nodeTower->SetRotation(Quaternion(pitchTower, Vector3::UP));
    }

    if(controls.buttons_ & CTRL_TRUNK_DOWN)
    {
        RotateTrunk(-SPEED_TRUNK_ROTATION * timeStep);
    }

    if(controls.buttons_ & CTRL_TRUNK_UP)
    {
        RotateTrunk(SPEED_TRUNK_ROTATION * timeStep);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Vehicle::Init()
{
    StaticModel* hullObject = node_->CreateComponent<StaticModel>();
    hullBody = node_->CreateComponent<RigidBody>();
    CollisionShape* hullShape = node_->CreateComponent<CollisionShape>();

    node_->SetScale(Vector3(1.5f, 1.0f, 3.0f));
    hullObject->SetModel(gResourceCache->GetResource<Model>("Models/Box.mdl"));
    hullObject->SetMaterial(gResourceCache->GetResource<Material>("Materials/Stone.xml"));
    hullObject->SetCastShadows(true);
    hullShape->SetBox(Vector3::ONE);
    hullBody->SetMass(4.0f);
    hullBody->SetLinearDamping(0.2f); // Some air resistance
    hullBody->SetAngularDamping(0.5f);
    hullBody->SetCollisionLayer(1);

    InitWheel("FrontLeft", Vector3(-0.6f, -0.4f, 0.3f), frontLeft, frontLeftID);
    InitWheel("FrontRight", Vector3(0.6f, -0.4f, 0.3f), frontRight, frontRightID);
    InitWheel("RearLeft", Vector3(-0.6f, -0.4f, -0.3f), rearLeft, rearLeftID);
    InitWheel("RearRight", Vector3(0.6f, -0.4f, -0.3f), rearRight, rearRightID);

    InitTower();

    GetWheelComponents();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Vehicle::InitWheel(const String& name, const Vector3& offset, WeakPtr<Node>& wheelNode, unsigned& wheelNodeID)
{
    wheelNode = GetScene()->CreateChild(name);
    wheelNode->SetPosition(node_->LocalToWorld(offset));
    wheelNode->SetRotation(node_->GetRotation() * (offset.x_ >= 0.0 ? Quaternion(0.0f, 0.0f, -90.0f) :
        Quaternion(0.0f, 0.0f, 90.0f)));
    wheelNode->SetScale(Vector3(0.8f, 0.5f, 0.8f));

    wheelNodeID = wheelNode->GetID();

    StaticModel* wheelObject = wheelNode->CreateComponent<StaticModel>();
    RigidBody* wheelBody = wheelNode->CreateComponent<RigidBody>();
    CollisionShape* wheelShape = wheelNode->CreateComponent<CollisionShape>();
    Constraint* wheelConstraint = wheelNode->CreateComponent<Constraint>();

    wheelObject->SetModel(gResourceCache->GetResource<Model>("Models/Cylinder.mdl"));
    wheelObject->SetMaterial(gResourceCache->GetResource<Material>("Materials/Stone.xml"));
    wheelObject->SetCastShadows(true);
    wheelShape->SetSphere(1.0f);
    wheelBody->SetFriction(10.0f);
    wheelBody->SetMass(1.0f);
    wheelBody->SetLinearDamping(0.2f); // Some air resistance
    wheelBody->SetAngularDamping(0.75f); // Could also use rolling friction
    wheelBody->SetCollisionLayer(1);
    wheelConstraint->SetConstraintType(CONSTRAINT_HINGE);
    wheelConstraint->SetOtherBody(GetComponent<RigidBody>()); // Connect to the hull body
    wheelConstraint->SetWorldPosition(wheelNode->GetPosition()); // Set constraint's both ends at wheel's location
    wheelConstraint->SetAxis(Vector3::UP); // Wheel rotates around its local Y-axis
    wheelConstraint->SetOtherAxis(offset.x_ >= 0.0 ? Vector3::RIGHT : Vector3::LEFT); // Wheel's hull axis points either left or right
    wheelConstraint->SetLowLimit(Vector2(-180.0f, 0.0f)); // Let the wheel rotate freely around the axis
    wheelConstraint->SetHighLimit(Vector2(180.0f, 0.0f));
    wheelConstraint->SetDisableCollision(true); // Let the wheel intersect the vehicle hull
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Vehicle::InitTower()
{
    nodeTower = node_->CreateChild("Tower");
    nodeTower->SetPosition({0.0f, 0.8f, 0.0f});
    float scaleHor = 0.8f;
    nodeTower->SetScale(Vector3(scaleHor, 0.75f, scaleHor));

    towerID = nodeTower->GetID();

    StaticModel *towerObject = nodeTower->CreateComponent<StaticModel>();
    Constraint *towerConstraint = nodeTower->CreateComponent<Constraint>();

    towerObject->SetModel(gResourceCache->GetResource<Model>("Models/Cylinder.mdl"));
    towerObject->SetMaterial(gResourceCache->GetResource<Material>("Materials/Stone.xml"));
    towerObject->SetCastShadows(true);

    towerConstraint->SetAxis(Vector3::UP);
    towerConstraint->SetOtherAxis(Vector3::ZERO);
    towerConstraint->SetLowLimit(Vector2(-180.0f, 0.0f));
    towerConstraint->SetHighLimit(Vector2(180.0f, 0.0f));
    towerConstraint->SetDisableCollision(false);

    InitTrunk();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Vehicle::InitTrunk()
{
    nodeTrunk = nodeTower->CreateChild("Trunk");
    nodeTrunk->SetPosition({0.0f, 0.75f, 0.4f});
    //nodeTrunk->Translate({0.0f, 1.5f, 0.0f});
    
    RotateTrunk(-90.0f);

    float scaleHor = 0.23f;
    nodeTrunk->SetScale({scaleHor, 1.5f, scaleHor});

    trunkID = nodeTrunk->GetID();

    StaticModel *trunkObject = nodeTrunk->CreateComponent<StaticModel>();
    Constraint *trunkConstraint = nodeTrunk->CreateComponent<Constraint>();

    trunkObject->SetModel(gResourceCache->GetResource<Model>("Models/Cylinder.mdl"));
    trunkObject->SetMaterial(gResourceCache->GetResource<Material>("Materials/Stone.xml"));
    trunkObject->SetCastShadows(true);

    trunkConstraint->SetAxis(Vector3::UP);
    trunkConstraint->SetOtherAxis(Vector3::ZERO);
    trunkConstraint->SetDisableCollision(false);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Vehicle::RotateTrunk(float delta)
{
    yawTrunk += delta;
    if(yawTrunk < yawTrunkMin)
    {
        yawTrunk = yawTrunkMin;
        return;
    }
    if(yawTrunk > yawTrunkMax)
    {
        yawTrunk = yawTrunkMax;
        return;
    }

    Quaternion rotate(delta, Vector3::LEFT);

    nodeTrunk->RotateAround({0.0f, 0.0f, 0.4f}, rotate, Urho3D::TS_PARENT);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Vehicle::GetWheelComponents()
{
    if (frontLeft && frontRight && rearLeft && rearRight)
    {
        frontLeftAxis = frontLeft->GetComponent<Constraint>();
        frontRightAxis = frontRight->GetComponent<Constraint>();
        frontLeftBody = frontLeft->GetComponent<RigidBody>();
        frontRightBody = frontRight->GetComponent<RigidBody>();
        rearLeftBody = rearLeft->GetComponent<RigidBody>();
        rearRightBody = rearRight->GetComponent<RigidBody>();
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Vehicle::Delete()
{
    frontLeftBody->GetNode()->Remove();
    frontRightBody->GetNode()->Remove();
    rearLeftBody->GetNode()->Remove();
    rearRightBody->GetNode()->Remove();
    hullBody->GetNode()->Remove();
}
