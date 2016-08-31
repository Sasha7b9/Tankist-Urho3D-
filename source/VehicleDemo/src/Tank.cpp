
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

#include "Tank.h"

Tank::Tank(Context* context) :
    LogicComponent(context)
{
    // Only the physics update event is needed: unsubscribe from the rest for optimization
    SetUpdateEventMask(USE_FIXEDUPDATE);
}

void Tank::RegisterObject(Context* context)
{
    context->RegisterFactory<Tank>();

    URHO3D_ATTRIBUTE("Controls Yaw", float, controls_.yaw_, 0.0f, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Controls Pitch", float, controls_.pitch_, 0.0f, AM_DEFAULT);
    // Register wheel node IDs as attributes so that the wheel nodes can be reaquired on deserialization. They need to be tagged
    // as node ID's so that the deserialization code knows to rewrite the IDs in case they are different on load than on save
}

void Tank::ApplyAttributes()
{
    // This function is called on each Serializable after the whole scene has been loaded. Reacquire wheel nodes from ID's
    // as well as all required physics components
    Scene* scene = GetScene();

    hullBody_ = node_->GetComponent<RigidBody>();

    //GetWheelComponents();
}

void Tank::FixedUpdate(float timeStep)
{
    float accelerator = 0.0f;

    if(controls_.buttons_ & CTRL_LEFT)
    {
        for(int i = 0; i < 5; i++)
        {
            wheelBodyLeft[i]->SetLinearVelocity(Vector3::ZERO);
        }
    }
    if(controls_.buttons_ & CTRL_RIGHT)
    {
        for(int i = 0; i < 5; i++)
        {
            wheelBodyRight[i]->SetLinearVelocity(Vector3::ZERO);
        }
    }

    if(controls_.buttons_ & CTRL_FORWARD)
    {
        accelerator = 1.0f;
    }
    if(controls_.buttons_ & CTRL_BACK)
    {
        accelerator = -0.5f;
    }

    Quaternion hullRot = hullBody_->GetRotation();

    if (accelerator != 0.0f)
    {
        Vector3 torqueVec = Vector3(ENGINE_POWER * accelerator, 0.0f, 0.0f);

        if(!(controls_.buttons_ & CTRL_LEFT))
        {
            for(int i = 0; i < 5; i++)
            {
                wheelBodyLeft[i]->ApplyTorque(hullRot * torqueVec);
            }
        }
        else
        {
            for(int i = 0; i < 5; i++)
            {
                wheelBodyLeft[i]->ApplyTorque(hullRot * -torqueVec);
            }
        }

        if(!(controls_.buttons_ & CTRL_RIGHT))
        {
            for(int i = 0; i < 5; i++)
            {
                wheelBodyRight[i]->ApplyTorque(hullRot * torqueVec);
            }
        }
        else
        {
            for(int i = 0; i < 5; i++)
            {
                wheelBodyRight[i]->ApplyTorque(hullRot * -torqueVec);
            }
        }
    }

    Vector3 localVelocity = hullRot.Inverse() * hullBody_->GetLinearVelocity();
    hullBody_->ApplyForce(hullRot * Vector3::DOWN * Abs(localVelocity.z_) * DOWN_FORCE);

    if(controls_.buttons_ & CTRL_STOP)
    {
        for(int i = 0; i < 5; i++)
        {
            wheelBodyLeft[i]->SetLinearVelocity(Vector3::ZERO);
            wheelBodyRight[i]->SetLinearVelocity(Vector3::ZERO);
        }
    }
}

void Tank::Init()
{
    // This function is called only from the main program when initially creating the vehicle, not on scene load
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    StaticModel* hullObject = node_->CreateComponent<StaticModel>();
    hullBody_ = node_->CreateComponent<RigidBody>();
    CollisionShape* hullShape = node_->CreateComponent<CollisionShape>();

    float sizeX = 3.5f;
    float sizeY = 1.0f;
    float sizeZ = 6.0f;

    node_->SetScale(Vector3(sizeX, sizeY, sizeZ));
    hullObject->SetModel(cache->GetResource<Model>("Models/Tank/Body.mdl"));
    hullObject->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));
    hullObject->SetCastShadows(true);
    hullShape->SetBox(Vector3::ONE);
    hullBody_->SetMass(40.0f);
    hullBody_->SetLinearDamping(0.2f); // Some air resistance
    hullBody_->SetAngularDamping(0.5f);
    hullBody_->SetCollisionLayer(1);

    Vector<WeakPtr<RigidBody>> damperBodyLeft(5);
    Vector<WeakPtr<RigidBody>> damperBodyRight(5);

    for(int i = 0; i < 5; i++)
    {
        wheelBodyLeft[i] = new RigidBody(context_);
        wheelBodyRight[i] = new RigidBody(context_);
    }

    float x = 0.47f;
    float y = -0.3f;
    float z = 0.4f;
    float dZ = 0.2f;

    for(int i = 0; i < 5; i++)
    {
        InitDamper("damper", {-x, y, z}, damperBodyLeft[i]);
        InitDamper("damper", {x, y, z}, damperBodyRight[i]);
        z -= dZ;
    }
    
    x = -0.6f;
    y = 2.0f;
    z = 0.0f;

    for(int i = 0; i < 5; i++)
    {
        InitWheel("wheel", Vector3(x, y, z), wheelBodyLeft[i], damperBodyLeft[i]);
        y = -y;
        InitWheel("wheel", Vector3(x, y, z), wheelBodyRight[i], damperBodyRight[i]);
        y = -y;
    }
}

void Tank::InitDamper(const String& name, const Vector3& offset, WeakPtr<RigidBody>& damperBody)
{
    ResourceCache *cache = GetSubsystem<ResourceCache>();

    WeakPtr<Node> damperNode(GetScene()->CreateChild(name));
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

    damperBody->SetFriction(0.0f);
    damperBody->SetMass(2.0f);
    damperBody->SetCollisionLayer(1);
    damperBody->DisableMassUpdate();

    damperConstaraint->SetConstraintType(CONSTRAINT_SLIDER);
    damperConstaraint->SetOtherBody(damperBody);
    damperConstaraint->SetRotation(Quaternion(0.0f, 0.0f, 90.0f));
    damperConstaraint->SetWorldPosition(damperNode->GetPosition());
    damperConstaraint->SetDisableCollision(true);

    btSliderConstraint *bulletConstraint = (btSliderConstraint*)damperConstaraint->GetConstraint();

    bulletConstraint->setDampingLimLin(1.0f);
    bulletConstraint->setSoftnessLimLin(0.5f);
    bulletConstraint->setRestitutionLimLin(0.5f);

    damperConstaraint->SetLowLimit({-0.25f, 0.0f});
    damperConstaraint->SetHighLimit({0.25f, 0.0f});

    damperConstaraint->SetCFM(0.5f);
    damperConstaraint->SetERP(0.5f);
}

void Tank::InitWheel(const String& name, const Vector3& offset, WeakPtr<RigidBody>& wheelBody, WeakPtr<RigidBody>& damperBody)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    // Note: do not parent the wheel to the hull scene node. Instead create it on the root level and let the physics
    // constraint keep it together
    WeakPtr<Node> wheelNode(GetScene()->CreateChild(name));
    Node *node = damperBody->GetNode();
    wheelNode->SetPosition(node->LocalToWorld(offset));
    
    wheelNode->SetScale(Vector3(0.8f, 0.5f, 0.8f));

    StaticModel* wheelObject = wheelNode->CreateComponent<StaticModel>();
    wheelBody = wheelNode->CreateComponent<RigidBody>();
    CollisionShape* wheelShape = wheelNode->CreateComponent<CollisionShape>();
    Constraint* wheelConstraint = wheelNode->CreateComponent<Constraint>();

    wheelObject->SetModel(cache->GetResource<Model>("Models/Cylinder.mdl"));
    wheelObject->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));
    wheelObject->SetCastShadows(true);

    wheelShape->SetSphere(1.0f);

    wheelBody->SetFriction(1.0f);
    //wheelBody->SetRollingFriction(0.5f);
    wheelBody->SetMass(0.5f);
    //wheelBody->SetLinearDamping(0.2f);
    //wheelBody->SetAngularDamping(0.75f);
    //wheelBody->SetCollisionLayer(1);

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
