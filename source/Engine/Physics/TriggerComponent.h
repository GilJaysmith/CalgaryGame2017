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

class TriggerComponent : public Component
{
public:
	static Component* CreateComponent(Entity* owner, const YAML::Node& properties);

public:
	virtual ~TriggerComponent();

	bool OnMessage(Message*) override;

protected:
	TriggerComponent(Entity* owner, const YAML::Node& properties);

	physx::PxRigidActor* m_Actor;
	std::map<physx::PxShape*, DebugDraw::IDebugObject*> m_ShapeToDebugObject;
};