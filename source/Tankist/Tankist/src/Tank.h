#pragma once


// Команды управления
URHO3D_EVENT(E_COMMAND_CONTROL, CommandControl)
{
    URHO3D_PARAM(P_KEY, Command);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const int CTRL_FORWARD          = 1 << 0;
const int CTRL_BACK             = 1 << 1;
const int CTRL_LEFT             = 1 << 2;
const int CTRL_RIGHT            = 1 << 3;
const int CTRL_TOWER_LEFT       = 1 << 4;
const int CTRL_TOWER_RIGHT      = 1 << 5;
const int CTRL_TRUNK_UP         = 1 << 6;
const int CTRL_TRUNK_DOWN       = 1 << 7;
const int CTRL_TOWER_LEFT_FAST  = 1 << 8;
const int CTRL_TOWER_RIGHT_FAST = 1 << 9;
const int CTRL_TRUNK_UP_FAST    = 1 << 10;
const int CTRL_TRUNK_DOWN_FAST  = 1 << 11;
const int CTRL_STOP             = 1 << 12;

const float YAW_SENSITIVITY = 0.1f;
const float ENGINE_POWER = 500.0f;
const float DOWN_FORCE = 1.0f;
const float MAX_WHEEL_ANGLE = 22.5f;
const float SPEED_TOWER_ROTATION = 2.0f;
const float SPEED_TRUNK_ROTATION = 2.5f;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tank component, responsible for physical movement according to controls.
class Tank : public LogicComponent
{

#pragma warning(push)
#pragma warning(disable:4640)
    URHO3D_OBJECT(Tank, LogicComponent);
#pragma warning(pop)

public:
    /// Construct.
    Tank(Context* context);
    
    /// Register object factory and attributes.
    static void RegisterObject(Context* context);
    
    /// Handle physics world update. Called by LogicComponent base class.
    virtual void FixedUpdate(float timeStep);
    
    /// Initialize the tank. Create rendering and physics components. Called by the application.
    void Init();

    /// Movement controls.
    Controls controls;

    void Delete();

    unsigned towerID;
    unsigned trunkID;

    void DrawDebugGeometry_();
    void Logging();

    int speed = 0;
    
private:
    void InitTower();

    void InitTrunk();

    void RotateTrunk(float delta);

    // Hull and wheel rigid bodies.
    WeakPtr<RigidBody> hullBody;
    bool created = false;

    WeakPtr<Node> nodeTower;
    WeakPtr<Node> nodeTrunk;

    float pitchTower = 0.0f;

    float yawTrunkMin = -110.0f;
    float yawTrunkMax = -35.0f;
    float yawTrunk = 0.0f;
    
    /// Current left/right steering amount (-1 to 1.)
    float steering;

    Tank(Tank const&) : LogicComponent(nullptr) {};
    Tank operator=(Tank const&){};

    float scaleNode = 0.5f;

    const int maxSpeed = 10;
    const int minSpeed = -5;
};
