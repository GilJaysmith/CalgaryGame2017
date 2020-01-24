#pragma once

#include "Engine/Entities/Component.h"


class Entity;
class Time;

namespace physx
{
	class PxRigidActor;
}

class PhysicsComponent : public Component
{
public:
	static Component* CreateComponent(Entity* owner, const YAML::Node& properties);

public:
	virtual ~PhysicsComponent();

	bool OnMessage(Message*) override;
	void OnUpdate(const Time& elapsed_time, UpdatePass::TYPE update_pass) override;
	void OnTransformUpdated() override;

protected:
	PhysicsComponent(Entity* owner, const YAML::Node& properties);

	virtual void OnSetActive(bool active) override;

	physx::PxRigidActor* m_Actor;
};