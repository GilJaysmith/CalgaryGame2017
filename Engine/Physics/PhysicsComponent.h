#pragma once

#include "Engine/Entities/Component.h"

#include "sdks/libyaml/include/yaml-cpp/yaml.h"

class Entity;
class Time;

namespace physx
{
	class PxRigidDynamic;
	class PxRigidStatic;
}

class PhysicsComponent : public Component
{
public:
	static Component* CreateComponent(Entity* owner, const YAML::Node& properties);

public:
	virtual ~PhysicsComponent();

	bool OnMessage(Message*) override;
	void OnUpdate(const Time& elapsed_time) override;

protected:
	PhysicsComponent(Entity* owner, const YAML::Node& properties);

	virtual void OnSetActive(bool active) override;

	physx::PxRigidDynamic* m_Actor;
	physx::PxRigidStatic* m_StaticActor;
};