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
    if(!created)
    {
        return;
    }

    float accelerator = 0.0f;

    if(controls.buttons_ & CTRL_LEFT)
    {
    }
    if(controls.buttons_ & CTRL_RIGHT)
    {
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
        //Vector3 torqueVec = Vector3(0.0f, 0.0f, ENGINE_POWER * accelerator);
        //Vector3 torqueVec = Vector3(0.0f, ENGINE_POWER * accelerator, 0.0f);
        Vector3 torqueVec = Vector3(ENGINE_POWER * accelerator, 0.0f, 0.0f);

        if(!(controls.buttons_ & CTRL_LEFT))
        {
        }
        else
        {
        }

        if(!(controls.buttons_ & CTRL_RIGHT))
        {
        }
        else
        {
        }
    }

    Vector3 localVelocity = hullRot.Inverse() * hullBody->GetLinearVelocity();

    if(controls.buttons_ & CTRL_STOP)
    {
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
    c = node_->GetWorldPosition();
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

    hullBody->SetMass(0.0f);
    hullBody->SetLinearDamping(0.2f); // Some air resistance
    hullBody->SetAngularDamping(0.2f);
    hullBody->SetCollisionLayer(1);

    String strLeftWheel = "LeftWheel";
    String strRightWheel = "RightWheel";

    InitTower();

    created = true;

    node_->SetScale(node_->GetScale() * scaleNode);
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
}
