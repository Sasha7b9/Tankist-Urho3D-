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

#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/Constraint.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>

#include <Bullet/BulletDynamics/ConstraintSolver/btSliderConstraint.h>

#include "Vehicle.h"

Vehicle::Vehicle(Context* context) :
    LogicComponent(context),
    steering_(0.0f)
{
    // Only the physics update event is needed: unsubscribe from the rest for optimization
    SetUpdateEventMask(USE_FIXEDUPDATE);
}

void Vehicle::RegisterObject(Context* context)
{
    context->RegisterFactory<Vehicle>();

    URHO3D_ATTRIBUTE("Controls Yaw", float, controls_.yaw_, 0.0f, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Controls Pitch", float, controls_.pitch_, 0.0f, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Steering", float, steering_, 0.0f, AM_DEFAULT);
    // Register wheel node IDs as attributes so that the wheel nodes can be reaquired on deserialization. They need to be tagged
    // as node ID's so that the deserialization code knows to rewrite the IDs in case they are different on load than on save
    URHO3D_ATTRIBUTE("Front Left Node", unsigned, frontLeftID_, 0, AM_DEFAULT | AM_NODEID);
    URHO3D_ATTRIBUTE("Front Right Node", unsigned, frontRightID_, 0, AM_DEFAULT | AM_NODEID);
    URHO3D_ATTRIBUTE("Rear Left Node", unsigned, rearLeftID_, 0, AM_DEFAULT | AM_NODEID);
    URHO3D_ATTRIBUTE("Rear Right Node", unsigned, rearRightID_, 0, AM_DEFAULT | AM_NODEID);
}

void Vehicle::ApplyAttributes()
{
    // This function is called on each Serializable after the whole scene has been loaded. Reacquire wheel nodes from ID's
    // as well as all required physics components
    Scene* scene = GetScene();

    frontLeft_ = scene->GetNode(frontLeftID_);
    frontRight_ = scene->GetNode(frontRightID_);
    rearLeft_ = scene->GetNode(rearLeftID_);
    rearRight_ = scene->GetNode(rearRightID_);
    hullBody_ = node_->GetComponent<RigidBody>();

    GetWheelComponents();
}

void Vehicle::FixedUpdate(float timeStep)
{
    float newSteering = 0.0f;
    float accelerator = 0.0f;

    if (controls_.buttons_ & CTRL_LEFT)
        newSteering = -1.0f;
    if (controls_.buttons_ & CTRL_RIGHT)
        newSteering = 1.0f;
    if (controls_.buttons_ & CTRL_FORWARD)
        accelerator = 1.0f;
    if (controls_.buttons_ & CTRL_BACK)
        accelerator = -0.5f;

    if (newSteering != 0.0f)
    {
        frontLeftBody_->Activate();
        frontRightBody_->Activate();
        steering_ = steering_ * 0.95f + newSteering * 0.05f;
    }
    else
        steering_ = steering_ * 0.8f + newSteering * 0.2f;

    Quaternion steeringRot(steering_ * MAX_WHEEL_ANGLE, 0, 0);
    frontLeftAxis_->SetOtherAxis(steeringRot * Vector3::DOWN);
    frontRightAxis_->SetOtherAxis(steeringRot * Vector3::UP);

    Quaternion hullRot = hullBody_->GetRotation();
    if (accelerator != 0.0f)
    {
        Vector3 torqueVec = Vector3(ENGINE_POWER * accelerator, 0.0f, 0.0f);

        frontLeftBody_->ApplyTorque(hullRot * steeringRot * torqueVec);
        frontRightBody_->ApplyTorque(hullRot * steeringRot * torqueVec);
        rearLeftBody_->ApplyTorque(hullRot * torqueVec);
        rearRightBody_->ApplyTorque(hullRot * torqueVec);
    }

    Vector3 localVelocity = hullRot.Inverse() * hullBody_->GetLinearVelocity();
    hullBody_->ApplyForce(hullRot * Vector3::DOWN * Abs(localVelocity.z_) * DOWN_FORCE);
}

void Vehicle::Init()
{
    // This function is called only from the main program when initially creating the vehicle, not on scene load
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    StaticModel* hullObject = node_->CreateComponent<StaticModel>();
    hullBody_ = node_->CreateComponent<RigidBody>();
    CollisionShape* hullShape = node_->CreateComponent<CollisionShape>();

    node_->SetScale(Vector3(1.5f, 1.0f, 4.0f));
    hullObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
    hullObject->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));
    hullObject->SetCastShadows(true);
    hullShape->SetBox(Vector3::ONE);
    hullBody_->SetMass(10.0f);
    hullBody_->SetLinearDamping(0.2f); // Some air resistance
    hullBody_->SetAngularDamping(0.5f);
    hullBody_->SetCollisionLayer(1);

    float x = 0.7f;
    float y = -0.5f;
    float z = 0.3f;

    WeakPtr<RigidBody> damperBodyFrontLeft;
    WeakPtr<RigidBody> damperBodyFrontRight;
    WeakPtr<RigidBody> damperBodyRearLeft;
    WeakPtr<RigidBody> damperBodyRearRight;
    
    InitDamper("damperFrontLeft", {-x, y, z}, damperFrontLeft, damperBodyFrontLeft);
    InitDamper("damperFrontRight", {x, y, z}, damperFrontRight, damperBodyFrontRight);
    InitDamper("damperRearLeft", {-x, y, -z}, damperRearLeft, damperBodyRearLeft);
    InitDamper("damperRearRight", {x, y, -z}, damperRearRight, damperBodyRearRight);

    x = -0.6f;
    y = 2.0f;
    z = 0.0f;
    
    /*
    InitWheel("FrontLeft", Vector3(-x, y, z), frontLeft_, frontLeftID_, hullBody_);
    InitWheel("FrontRight", Vector3(x, y, z), frontRight_, frontRightID_, hullBody_);
    InitWheel("RearLeft", Vector3(-x, y, -z), rearLeft_, rearLeftID_, hullBody_);
    InitWheel("RearRight", Vector3(x, y, -z), rearRight_, rearRightID_, hullBody_);
    */
    
    
    InitWheel("FrontLeft", Vector3(x, y, z), frontLeft_, frontLeftID_, damperBodyFrontLeft);
    InitWheel("FrontRight", Vector3(x, -y, z), frontRight_, frontRightID_, damperBodyFrontRight);
    InitWheel("RearLeft", Vector3(x, y, -z), rearLeft_, rearLeftID_, damperBodyRearLeft);
    InitWheel("RearRight", Vector3(x, -y, -z), rearRight_, rearRightID_, damperBodyRearRight);
    

    GetWheelComponents();
}

void Vehicle::InitDamper(const String& name, const Vector3& offset, WeakPtr<Node>& damperNode, WeakPtr<RigidBody>& damperBody)
{
    ResourceCache *cache = GetSubsystem<ResourceCache>();

    damperNode = GetScene()->CreateChild(name);
    damperNode->SetPosition(node_->LocalToWorld(offset));
    damperNode->SetRotation(Quaternion(0.0f, 0.0f, 90.0f));

    float scaleX = 1.5f;
    float scaleY = 0.2f;
    float scaleZ = 0.2f;

    damperNode->SetScale({scaleX, scaleY, scaleZ});

    StaticModel *damperObject = damperNode->CreateComponent<StaticModel>();
    damperBody = damperNode->CreateComponent<RigidBody>();
    CollisionShape *damperShape = damperNode->CreateComponent<CollisionShape>();
    Constraint *damperConstaraint = hullBody_->GetNode()->CreateComponent<Constraint>();

    damperObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
    damperObject->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));
    damperObject->SetCastShadows(true);

    damperShape->SetSphere(0.2f);
    damperShape->SetPosition(Vector3(-0.5f, 0.0f, 0.0f));

    damperBody->SetFriction(1.0f);
    damperBody->SetMass(5.0f);
    damperBody->SetCollisionLayer(1);
    damperBody->DisableMassUpdate();

    damperConstaraint->SetConstraintType(CONSTRAINT_SLIDER);
    damperConstaraint->SetOtherBody(damperBody);
    damperConstaraint->SetRotation(Quaternion(0.0f, 0.0f, 90.0f));
    damperConstaraint->SetWorldPosition(damperNode->GetPosition());
    damperConstaraint->SetDisableCollision(true);

    btSliderConstraint *bulletConstraint = (btSliderConstraint*)damperConstaraint->GetConstraint();

    //bulletConstraint->setDampingLimLin(0.9f);
    bulletConstraint->setSoftnessLimLin(0.5f);
    bulletConstraint->setRestitutionLimLin(0.5f);

    damperConstaraint->SetLowLimit({-0.5f, 0.0f});
    damperConstaraint->SetHighLimit({0.5f, 0.0f});

    damperConstaraint->SetCFM(0.1f);
    damperConstaraint->SetERP(0.3f);
}

void Vehicle::InitWheel(const String& name, const Vector3& offset, WeakPtr<Node>& wheelNode, unsigned& wheelNodeID, WeakPtr<RigidBody>& damperBody)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    // Note: do not parent the wheel to the hull scene node. Instead create it on the root level and let the physics
    // constraint keep it together
    wheelNode = GetScene()->CreateChild(name);
    Node *node = damperBody->GetNode();
    wheelNode->SetPosition(node->LocalToWorld(offset));
    
    wheelNode->SetScale(Vector3(0.8f, 0.5f, 0.8f));
    // Remember the ID for serialization
    wheelNodeID = wheelNode->GetID();

    StaticModel* wheelObject = wheelNode->CreateComponent<StaticModel>();
    RigidBody* wheelBody = wheelNode->CreateComponent<RigidBody>();
    CollisionShape* wheelShape = wheelNode->CreateComponent<CollisionShape>();
    Constraint* wheelConstraint = wheelNode->CreateComponent<Constraint>();

    wheelObject->SetModel(cache->GetResource<Model>("Models/Cylinder.mdl"));
    wheelObject->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));
    wheelObject->SetCastShadows(true);

    wheelShape->SetSphere(1.0f);

    wheelBody->SetFriction(1.0f);
    wheelBody->SetMass(0.5f);
    wheelBody->SetLinearDamping(0.2f); // Some air resistance
    wheelBody->SetAngularDamping(0.75f); // Could also use rolling friction
    wheelBody->SetCollisionLayer(1);

    wheelConstraint->SetConstraintType(CONSTRAINT_HINGE);
    wheelConstraint->SetOtherBody(damperBody); // Connect to the hull body
    wheelConstraint->SetWorldPosition(wheelNode->GetPosition()); // Set constraint's both ends at wheel's location
    
    if(offset.x_ < 0.0f)
    {
        wheelNode->SetRotation(offset.y_ >= 0.0f ? Quaternion(0.0f, 0.0f, -90.0f) : Quaternion(0.0f, 0.0f, 90.0f));
        wheelConstraint->SetAxis(Vector3::UP);
        wheelConstraint->SetOtherAxis(offset.y_ < 0.0f ? Vector3::UP : Vector3::DOWN);
    }
    else
    {
        wheelNode->SetRotation(Quaternion(90.0f, 0.0f, 0.0f));
        wheelConstraint->SetAxis(Vector3::LEFT);
        wheelConstraint->SetOtherAxis(Vector3::DOWN);
    }
    

    wheelConstraint->SetLowLimit(Vector2(-180.0f, 0.0f)); // Let the wheel rotate freely around the axis
    wheelConstraint->SetHighLimit(Vector2(180.0f, 0.0f));
    wheelConstraint->SetDisableCollision(true); // Let the wheel intersect the vehicle hull
}

void Vehicle::GetWheelComponents()
{
    frontLeftAxis_ = frontLeft_->GetComponent<Constraint>();
    frontRightAxis_ = frontRight_->GetComponent<Constraint>();
    frontLeftBody_ = frontLeft_->GetComponent<RigidBody>(); 
    frontRightBody_ = frontRight_->GetComponent<RigidBody>();
    rearLeftBody_ = rearLeft_->GetComponent<RigidBody>();
    rearRightBody_ = rearRight_->GetComponent<RigidBody>();
}

