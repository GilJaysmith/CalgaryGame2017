#pragma once

#include "Engine/Entities/Component.h"

class Entity;
class Time;

namespace DebugDraw
{
	class IDebugObject;
}

namespace physx
{
	class PxRigidActor;
	class PxShape;
}

class CheckpointComponent : public Component
{
public:
	static Component* CreateComponent(Entity* owner, const YAML::Node& properties);

public:
	virtual ~CheckpointComponent();

	bool OnMessage(Message*) override;

protected:
	CheckpointComponent(Entity* owner, const YAML::Node& properties);

	physx::PxRigidActor* m_Actor;
	DebugDraw::IDebugObject* m_DebugObject;
};