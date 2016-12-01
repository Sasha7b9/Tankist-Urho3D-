#pragma once

#pragma warning(push)
#pragma warning(disable:4100 4127 4251 4263 4264 4265 4266 4275 4312 4365 4571 4625 4626 4640)

#include <Bullet/BulletDynamics/ConstraintSolver/btSliderConstraint.h>

#include <Urho3D/Audio/Audio.h>

#include <Urho3D/Container/Ptr.h>

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/Core/Thread.h>
#include <Urho3D/Core/Variant.h>

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>

#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/StaticModelGroup.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Graphics/VertexBuffer.h>
#include <Urho3D/Graphics/DebugRenderer.h>

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Input/Input.h>

#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/IO/MemoryBuffer.h>

#include <Urho3D/Math/Ray.h>

#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>

#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/Constraint.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/PhysicsEvents.h>

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/Localization.h>
#include <Urho3D/Resource/ResourceEvents.h>

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/LogicComponent.h>

#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/DropDownList.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/Sprite.h>

#include <Urho3D/DebugNew.h>

#include <Urho3D/Urho2D/Sprite2D.h>
#include <Urho3D/Urho2D/StaticSprite2D.h>

#include <thread>

#ifdef WIN32

#pragma comment(lib, "ws2_32.lib")
#include <d3d9types.h>

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#endif

#pragma warning(pop)

#ifndef WIN32
#define SOCKET int
#define closesocket close
#define WSAGetLastError() errno
#endif

#include "defines.h"

using namespace Urho3D;

//For client only
#include "Objects/CameraUni.h"
#include "Network/Client.h"

#include "GUI/Elements/DropDownListButtons.h"
#include "GUI/Elements/Governor.h"
#include "Network/Server.h"
#include "Tankist.h"
#include "GlobalVars.h"
