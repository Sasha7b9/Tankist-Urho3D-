/* (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by */
#include <stdafx.h>
#include <Urho3D/UI/Window.h>
#include "CameraUni.h"
#include "Sight.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CameraUni::CameraUni(Context *context) : Object(context)
{
    sight = new Sight(context);
    gUIRoot->AddChild(sight);
    sight->SetVisible(false);
    this->node = gScene->CreateChild("CameraNode", LOCAL);
    camera = this->node->CreateComponent<Camera>();
    camera->SetFarClip(5000.0f);
    camera->SetFov(45.0f);
    //camera->SetFillMode(FILL_WIREFRAME);
    GetSubsystem<Renderer>()->SetViewport(0, new Viewport(context_, gScene, camera));
    node->SetPosition({0.0f, 0.0f, 50.0f});
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void CameraUni::SetupViewport()
{
    node = gScene->CreateChild("CameraNode", LOCAL);
    node->CreateComponent<Camera>()->SetFarClip(5000.0f);

    node->SetPosition(Vector3(0.0f, 0.0f, 0.0f));

    SharedPtr<Viewport> viewport(new Viewport(context_, gScene, node->GetComponent<Camera>()));
    GetSubsystem<Renderer>()->SetViewport(0, viewport);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void CameraUni::MoveFromMouse()
{
    const float MOUSE_SENSITIVITY = 0.1f;

    if (!gUI->GetCursor()->IsVisible())
    {
        IntVector2 mouseMove = gInput->GetMouseMove();
        yaw += MOUSE_SENSITIVITY * mouseMove.x_;
        pitch += MOUSE_SENSITIVITY * mouseMove.y_;
        pitch = Clamp(pitch, 1.0f, 90.0f);
        node->SetRotation(Quaternion(pitch, yaw, 0.0f));
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
bool CameraUni::SetCameraMode(CameraMode mode_, Node *node_)
{
    if(node_ && mode_ != mode)
    {
        if(mode == ModeCommander)
        {
            rotationCommaner = this->node->GetRotation();
        }

        mode = mode_;

        if(mode == ModeOverview)
        {
            node_->SetPosition({0.0f, 0.0f, 50.0f});
        } 
        else if(mode == ModeCommander)
        {
            node_->AddChild(this->node);
            this->node->SetPosition(Vector3(0.0f, 20.0f, -50.0f));
            //this->node->SetPosition(Vector3(0.0f, 50.0f, -100.0f));
            camera->SetFov(45.0f);
            this->node->SetRotation(Quaternion(0.0f, 0.0f, 0.0f));
            sight->SetVisible(false);
        }
        else if(mode == ModeShooter)
        {
            node_->AddChild(this->node);
            this->node->SetPosition(Vector3::UP * 1.75f + Vector3::BACK * 3);
            camera->SetFov(4.5f);
            this->node->SetRotation(Quaternion(-90.0f, 0.0f, 0.0f));
            sight->SetVisible(true);
        }
    }

    return node_ != 0;
}
