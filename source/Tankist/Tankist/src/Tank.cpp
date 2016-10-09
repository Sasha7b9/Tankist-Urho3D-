#include <stdafx.h>


#include "Tank.h"

#include <CommonFunctions.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Tank::Tank(Context* context) :
    LogicComponent(context),
    steering(0.0f)
{
    // Only the physics update event is needed: unsubscribe from the rest for optimization
    SetUpdateEventMask(USE_FIXEDUPDATE);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tank::RegisterObject(Context* context)
{
    context->RegisterFactory<Tank>();

    URHO3D_ATTRIBUTE("Controls Yaw", float, controls.yaw_, 0.0f, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Controls Pitch", float, controls.pitch_, 0.0f, AM_DEFAULT);
    URHO3D_ATTRIBUTE("Steering", float, steering, 0.0f, AM_DEFAULT);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tank::FixedUpdate(float timeStep)
{
    return;

    if(!hullBody)
    {
        return;
    }

    float accelerator = 0.0f;

    if(controls.buttons_ & CTRL_LEFT)
    {
        for(int i = 0; i < 5; i++)
        {
            wheelBodyLeft[i]->SetLinearVelocity(Vector3::ZERO);
        }
    }
    if(controls.buttons_ & CTRL_RIGHT)
    {
        for(int i = 0; i < 5; i++)
        {
            wheelBodyRight[i]->SetLinearVelocity(Vector3::ZERO);
        }
    }

    if(controls.buttons_ & CTRL_FORWARD)
    {
        accelerator = 1.0f;
    }
    if(controls.buttons_ & CTRL_BACK)
    {
        accelerator = -0.5f;
    }

    Quaternion hullRot = hullBody->GetRotation();

    if(accelerator != 0.0f)
    {
        Vector3 torqueVec = Vector3(ENGINE_POWER * accelerator, 0.0f, 0.0f);

        if(!(controls.buttons_ & CTRL_LEFT))
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

        if(!(controls.buttons_ & CTRL_RIGHT))
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

    Vector3 localVelocity = hullRot.Inverse() * hullBody->GetLinearVelocity();
    hullBody->ApplyForce(hullRot * Vector3::DOWN * Abs(localVelocity.z_) * DOWN_FORCE);

    if(controls.buttons_ & CTRL_STOP)
    {
        for(int i = 0; i < 5; i++)
        {
            wheelBodyLeft[i]->SetLinearVelocity(Vector3::ZERO);
            wheelBodyRight[i]->SetLinearVelocity(Vector3::ZERO);
        }
    }

    if(controls.buttons_ & CTRL_TOWER_RIGHT)
    {
        pitchTower += SPEED_TOWER_ROTATION * timeStep;
        nodeTower->SetRotation(Quaternion(pitchTower, Vector3::UP));
    }

    if(controls.buttons_ & CTRL_TOWER_RIGHT_FAST)
    {
        pitchTower += SPEED_TOWER_ROTATION * timeStep * 4;
        nodeTower->SetRotation(Quaternion(pitchTower, Vector3::UP));
    }

    if(controls.buttons_ & CTRL_TOWER_LEFT)
    {
        pitchTower -= SPEED_TOWER_ROTATION * timeStep;
        nodeTower->SetRotation(Quaternion(pitchTower, Vector3::UP));
    }

    if(controls.buttons_ & CTRL_TOWER_LEFT_FAST)
    {
        pitchTower -= SPEED_TOWER_ROTATION * timeStep * 4;
        nodeTower->SetRotation(Quaternion(pitchTower, Vector3::UP));
    }

    if(controls.buttons_ & CTRL_TRUNK_DOWN)
    {
        RotateTrunk(-SPEED_TRUNK_ROTATION * timeStep);
    }

    if(controls.buttons_ & CTRL_TRUNK_DOWN_FAST)
    {
        RotateTrunk(-SPEED_TRUNK_ROTATION * timeStep * 4);
    }

    if(controls.buttons_ & CTRL_TRUNK_UP)
    {
        RotateTrunk(SPEED_TRUNK_ROTATION * timeStep);
    }

    if(controls.buttons_ & CTRL_TRUNK_UP_FAST)
    {
        RotateTrunk(SPEED_TRUNK_ROTATION * timeStep * 4);
    }
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tank::Logging()
{
    CollisionShape *shape = node_->GetComponent<CollisionShape>();
    Vector3 c = shape->GetPosition();
    LOG_INFOF("position collision shape %f %f %f", c.x_, c.y_, c.z_);
    c = node_->GetWorldPosition();
    LOG_INFOF("position node %f %f %f", c.x_, c.y_, c.z_);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tank::Init()
{
    AddModelToNode(node_, "Models/Tank/Box001.mdl", {0.0f, 6.5f, -9.0f});

    float x = 6.5f;
    float y = 7.5f;
    float z = -3.1f;

    AddModelToNode(node_, "Models/Tank/Cylinder001.mdl", {-x, y, z});
    AddModelToNode(node_, "Models/Tank/Cylinder002.mdl", {x, y, z});

    StaticModel *hullObject = node_->CreateComponent<StaticModel>();
    hullBody = node_->CreateComponent<RigidBody>();
    CollisionShape* hullShape = node_->CreateComponent<CollisionShape>();

    float scale = 1.0f;
    node_->SetScale({scale, scale, scale});

    hullObject->SetModel(gCache->GetResource<Model>("Models/Tank/Body.mdl"));
    hullObject->SetMaterial(gCache->GetResource<Material>("Models/Tank/DefaultMaterial.xml"));
    hullObject->SetCastShadows(true);

    BoundingBox box = hullObject->GetBoundingBox();
    Vector3 dimensions;
    Vector3 center;
    GetDimensionsCenter(box, dimensions, center, 1.0f);
    hullShape->SetBox(dimensions, center);

    hullBody->SetMass(40.0f);
    hullBody->SetLinearDamping(0.2f); // Some air resistance
    hullBody->SetAngularDamping(0.5f);
    hullBody->SetCollisionLayer(1);

    //hullBody->SetCcdRadius(0.1f);
    //hullBody->SetCcdMotionThreshold(dimensions.y_ / 1000.0f);

    Vector<WeakPtr<RigidBody>> damperBodyLeft(5);
    Vector<WeakPtr<RigidBody>> damperBodyRight(5);

    for(int i = 0; i < 5; i++)
    {
        wheelBodyLeft[i] = new RigidBody(context_);
        wheelBodyRight[i] = new RigidBody(context_);
    }

    x = 10.0f;
    y = 00.0f;
    z = 16.0f;
    float dZ = 8.0f;

    /*
    for(uint i = 0; i < 5; i++)
    {
        InitDamper("damper", {-x, y, z}, damperBodyLeft[i]);
        InitDamper("damper", {x, y, z}, damperBodyRight[i]);
        z -= dZ;
    }

    x = -0.6f;
    y = 2.0f;
    z = 0.0f;

    String strLeftWheel = "LeftWheel";
    String strRightWheel = "RightWheel";

    for(uint i = 0; i < 5; i++)
    {
        InitWheel(&scnTank, strLeftWheel + String(i + 1), Vector3(x, y, z), wheelBodyLeft[i], damperBodyLeft[i]);
        y = -y;
        InitWheel(&scnTank, strRightWheel + String(i + 1), Vector3(x, y, z), wheelBodyRight[i], damperBodyRight[i]);
        y = -y;
    }
    */

    InitTower();
}


void Tank::DrawDebugGeometry_()
{
    if(gDebugRenderer)
    {
        PODVector<Node*> nodes;
        node_->GetChildren(nodes, false);
        for(Node *node : nodes)
        {
            if(node->GetName() == "damper")
            {
                CollisionShape *shape = node->GetComponent<CollisionShape>();
                shape->DrawDebugGeometry(gDebugRenderer, true);
            }
        }
    }
}


void Tank::InitDamper(const String& name, const Vector3& offset, WeakPtr<RigidBody>& damperBody)
{
    WeakPtr<Node> damperNode(GetScene()->CreateChild(name));
    damperNode->SetPosition(node_->LocalToWorld(offset));
    Vector3 s = node_->GetScale();
    damperNode->SetScale(Vector3(s.x_, s.y_ * 5, s.z_));

    StaticModel *damperObject = damperNode->CreateComponent<StaticModel>();
    damperBody = damperNode->CreateComponent<RigidBody>();
    CollisionShape *damperShape = damperNode->CreateComponent<CollisionShape>();

    damperObject->SetModel(gCache->GetResource<Model>("Models/Box.mdl"));
    damperObject->SetMaterial(gCache->GetResource<Material>("Materials/Stone.xml"));
    damperObject->SetCastShadows(true);

    damperShape->SetSphere(1.0f);
    damperShape->SetPosition(Vector3(0.0f, 10.0f, 0.0f));

    damperBody->SetFriction(0.0f);
    damperBody->SetMass(2.0f);
    damperBody->SetCollisionLayer(1);
    damperBody->DisableMassUpdate();

    Constraint *damperConstaraint = hullBody->GetNode()->CreateComponent<Constraint>();

    damperConstaraint->SetConstraintType(Urho3D::CONSTRAINT_SLIDER);
    damperConstaraint->SetOtherBody(damperBody);
    //damperConstaraint->SetRotation(Quaternion(0.0f, 0.0f, 90.0f));
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


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tank::InitWheel(const String& name, const Vector3& offset, WeakPtr<RigidBody>& wheelBody, WeakPtr<RigidBody>& damperBody)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    // Note: do not parent the wheel to the hull scene node. Instead create it on the root level and let the physics
    // constraint keep it together
    WeakPtr<Node> wheelNode(GetScene()->CreateChild(name));
    Node *node = damperBody->GetNode();
    wheelNode->SetPosition(node->LocalToWorld(offset));

    StaticModel* wheelObject = wheelNode->CreateComponent<StaticModel>();
    wheelBody = wheelNode->CreateComponent<RigidBody>();
    CollisionShape* wheelShape = wheelNode->CreateComponent<CollisionShape>();
    Constraint* wheelConstraint = wheelNode->CreateComponent<Constraint>();

    wheelObject->SetModel(gCache->GetResource<Model>(String("Models/Tank/") + name + String(".mdl")));
    wheelObject->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));
    wheelObject->SetCastShadows(true);

    wheelShape->SetSphere(1.0f);

    wheelBody->SetFriction(10.0f);
    wheelBody->SetMass(0.5f);
    wheelBody->SetLinearDamping(0.2f);
    wheelBody->SetAngularDamping(0.75f);
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


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tank::InitTower()
{
    nodeTower = node_->CreateChild("Tower");
    nodeTower->SetPosition({0.0f, 6.8f, 4.0f});

    towerID = nodeTower->GetID();

    StaticModel *towerObject = nodeTower->CreateComponent<StaticModel>();
    Model *model = (Model*)gCache->GetResource<Model>("Models/Tank/Tower.mdl");
    towerObject->SetModel(model);

    Constraint *towerConstraint = nodeTower->CreateComponent<Constraint>();

    //towerObject->SetMaterial(gCache->GetResource<Material>("Models/Tank/DefaultMaterial.xml"));
    towerObject->SetCastShadows(true);

    towerConstraint->SetAxis(Vector3::UP);
    towerConstraint->SetOtherAxis(Vector3::ZERO);
    towerConstraint->SetLowLimit(Vector2(-180.0f, 0.0f));
    towerConstraint->SetHighLimit(Vector2(180.0f, 0.0f));
    towerConstraint->SetDisableCollision(false);

    InitTrunk();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tank::InitTrunk()
{
    nodeTrunk = nodeTower->CreateChild("Trunk");
    nodeTrunk->SetPosition({0.0f, 0.75f, 0.4f});
    //nodeTrunk->Translate({0.0f, 1.5f, 0.0f});
    
    //RotateTrunk(-90.0f);

    trunkID = nodeTrunk->GetID();

    StaticModel *trunkObject = nodeTrunk->CreateComponent<StaticModel>();
    Constraint *trunkConstraint = nodeTrunk->CreateComponent<Constraint>();

    trunkObject->SetModel(gCache->GetResource<Model>("Models/Tank/Trunk.mdl"));
    //trunkObject->SetMaterial(gCache->GetResource<Material>("Models/Tank/DefaultMaterial.xml"));
    trunkObject->SetCastShadows(true);

    trunkConstraint->SetAxis(Vector3::UP);
    trunkConstraint->SetOtherAxis(Vector3::ZERO);
    trunkConstraint->SetDisableCollision(false);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void Tank::RotateTrunk(float delta)
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
void Tank::Delete()
{
    for(uint i = 0; i < 5; i++)
    {
        wheelBodyLeft[i]->GetNode()->Remove();
        wheelBodyRight[i]->GetNode()->Remove();
    }

    hullBody->GetNode()->Remove();
}
