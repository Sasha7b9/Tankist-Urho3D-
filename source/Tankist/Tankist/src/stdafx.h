#pragma once

#pragma warning(push)
#pragma warning(disable:4100)
#pragma warning(disable:4266)
#pragma warning(disable:4365)
#pragma warning(disable:4625)
#pragma warning(disable:4626)
#pragma warning(disable:4640)

#define URHO3D_LOGGING

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/Core/Thread.h>

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/DebugHud.h>

#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/Graphics/Zone.h>

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Input/Input.h>

#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/IO/MemoryBuffer.h>

#include <Urho3D/Network/Network.h>
#include <Urho3D/Network/NetworkEvents.h>

#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/Constraint.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/PhysicsEvents.h>

#include <Urho3D/Resource/ResourceCache.h>

#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/LogicComponent.h>

#include <Urho3D/UI/UI.h>

#include <Urho3D/DebugNew.h>

#include <windowsx.h>

#pragma warning(pop)


using Urho3D::Application;

using Urho3D::BiasParameters;
using Urho3D::BoundingBox;

using Urho3D::Camera;
using Urho3D::CascadeParameters;
using Urho3D::CollisionShape;
using Urho3D::Color;
using Urho3D::Connection;
using Urho3D::Console;
using Urho3D::Constraint;
using Urho3D::Context;
using Urho3D::Controls;
using Urho3D::Cursor;

using Urho3D::DebugHud;

using Urho3D::File;
using Urho3D::FileSystem;

using Urho3D::Graphics;

using Urho3D::Input;
using Urho3D::IntVector2;
using Urho3D::Image;

using Urho3D::HashMap;

using Urho3D::Log;
using Urho3D::LogicComponent;
using Urho3D::Light;

using Urho3D::Material;
using Urho3D::MemoryBuffer;
using Urho3D::Model;

using Urho3D::Network;
using Urho3D::Node;

using Urho3D::Object;
using Urho3D::Octree;

using Urho3D::PhysicsWorld;
using Urho3D::PODVector;

using Urho3D::Quaternion;

using Urho3D::Renderer;
using Urho3D::ResourceCache;
using Urho3D::RigidBody;

using Urho3D::Scene;
using Urho3D::ShadowQuality;
using Urho3D::SharedPtr;
using Urho3D::StaticModel;
using Urho3D::String;
using Urho3D::StringHash;

using Urho3D::Terrain;
using Urho3D::Time;
using Urho3D::Thread;

using Urho3D::VariantMap;
using Urho3D::Vector;
using Urho3D::Vector2;
using Urho3D::Vector3;
using Urho3D::VectorBuffer;
using Urho3D::Viewport;

using Urho3D::WeakPtr;

using Urho3D::UI;
using Urho3D::UIElement;

using Urho3D::XMLFile;

using Urho3D::Zone;

using Urho3D::Clamp;
using Urho3D::Random;
using Urho3D::GetVariantType;
using Urho3D::Abs;

using Urho3D::LOCAL;
using Urho3D::LIGHT_DIRECTIONAL;
using Urho3D::SHADOWQUALITY_BLUR_VSM;
using Urho3D::SHADOWQUALITY_SIMPLE_16BIT;
using Urho3D::CONSTRAINT_HINGE;

using Urho3D::USE_FIXEDUPDATE;
using Urho3D::DEBUGHUD_SHOW_ALL_MEMORY;
using Urho3D::DEBUGHUD_SHOW_ALL;
using Urho3D::DEBUGHUD_SHOW_NONE;
using Urho3D::LOG_INFO;
using Urho3D::MOUSEB_RIGHT;
using Urho3D::KEY_ESC;
using Urho3D::KEY_F1;
using Urho3D::KEY_F2;
using Urho3D::KEY_F3;
using Urho3D::QUALITY_HIGH;
using Urho3D::QUALITY_LOW;
using Urho3D::AM_DEFAULT;
using Urho3D::AM_NODEID;

using Urho3D::E_KEYDOWN;
using Urho3D::E_PHYSICSPRESTEP;
using Urho3D::E_POSTUPDATE;
using Urho3D::E_SERVERCONNECTED;
using Urho3D::E_NETWORKMESSAGE;
using Urho3D::E_CLIENTCONNECTED;
using Urho3D::E_CONNECTFAILED;
using Urho3D::E_CLIENTDISCONNECTED;


#include "defines.h"

//For client only
#include "CameraUni.h"
#include "Client.h"

#include "Server.h"
#include "Tankist.h"
#include "GlobalVars.h"

