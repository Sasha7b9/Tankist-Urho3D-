#include <stdafx.h>


#include "CameraUni.h"
#include "Sight.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CameraUni::CameraUni(Context *context) : Object(context)
{
    sight = new Sight(context);
    gUIRoot->AddChild(sight);
    sight->SetVisible(false);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void CameraUni::SetupViewport()
{
    node = gScene->CreateChild("CameraNode", LOCAL);
    Camera *camera = node->CreateComponent<Camera>();
    camera->SetFarClip(300.0f);

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
bool CameraUni::SetMode(CameraMode mode, Node *node, const Vector3 &shift)
{
    if(node && mode != this->mode)
    {
        if(camera == nullptr)
        {
            this->node = gScene->CreateChild("CameraNode", LOCAL);
            camera = this->node->CreateComponent<Camera>();
            camera->SetFarClip(5000.0f);
            GetSubsystem<Renderer>()->SetViewport(0, new Viewport(context_, gScene, camera));
        }

        if(this->mode == ModeCommander)
        {
            rotationCommaner = this->node->GetRotation();
        }

        this->mode = mode;

        if(mode == ModeCommander)
        {
            node->AddChild(this->node);
            this->node->SetPosition(shift);
            camera->SetFov(45.0f);
            this->node->SetRotation(Quaternion(0.0f, 0.0f, 0.0f));
            sight->SetVisible(false);
        }
        else if(mode == ModeShooter)
        {
            node->AddChild(this->node);
            this->node->SetPosition(shift);
            camera->SetFov(4.5f);
            this->node->SetRotation(Quaternion(-90.0f, 0.0f, 0.0f));
            sight->SetVisible(true);
        }
    }

    return node != 0;
}
