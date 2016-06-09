#include <stdafx.h>


#include "CameraUni.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CameraUni::CameraUni(Context *context) : Object(context)
{

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
bool CameraUni::AttachToNode(Node *node, const Vector3 &shift)
{
    if (node)
    {
        this->node = node->CreateChild("CameraNode", LOCAL);
        this->node->SetPosition(shift);
        Camera* camera = this->node->CreateComponent<Camera>();
        camera->SetFarClip(500.0f);
        GetSubsystem<Renderer>()->SetViewport(0, new Viewport(context_, gScene, camera));
        return true;
    }

    return false;
}
