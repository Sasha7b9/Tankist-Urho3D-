#pragma once


enum CameraMode
{
    ModeNone,
    ModeCommander,
    ModeShooter,
    ModeDriver
};


class Sight;


class CameraUni : public Object
{
#pragma warning(push)
#pragma warning(disable:4640)
    URHO3D_OBJECT(CameraUni, Object)
#pragma warning(pop)

public:
    CameraUni(Context *context);

    void MoveFromMouse();
    bool SetMode(CameraMode mode, Node *node, const Vector3 &shift);    // For client
    void SetupViewport();                                               // For server
    void IncFov()
    {
        camera->SetFov(camera->GetFov() / 1.1f);
    }
    void DecFov()
    {
        camera->SetFov(camera->GetFov() * 1.1f);
    }
    void DefaultFov()
    {
        camera->SetFov(45.0f);
    }
    CameraMode GetMode()
    {
        return mode;
    }

private:
    float yaw = 0.0f;
    float pitch = 0.0f;
    SharedPtr<Node> node;
    Camera *camera;
    CameraMode mode = ModeNone;
    Quaternion rotationCommaner;
    Sight *sight = nullptr;

    CameraUni(CameraUni const&) : Object(nullptr) {};
    CameraUni operator=(CameraUni const&) {};
};