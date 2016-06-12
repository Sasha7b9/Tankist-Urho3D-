#pragma once


#include <Urho3D/Scene/Node.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Core/Object.h>

using Urho3D::Node;
using Urho3D::Vector3;
using Urho3D::Object;


class CameraUni : public Object
{
#pragma warning(push)
#pragma warning(disable:4640)
    URHO3D_OBJECT(CameraUni, Object)
#pragma warning(pop)

public:
    CameraUni(Context *context);

    void MoveFromMouse();
    bool AttachToNode(Node *node, const Vector3 &shift);    // For client
    void SetupViewport();                                   // For server

private:
    float yaw = 0.0f;
    float pitch = 0.0f;
    SharedPtr<Node> node;

    CameraUni(CameraUni const&) : Object(nullptr) {};
    CameraUni operator=(CameraUni const&) {};
};