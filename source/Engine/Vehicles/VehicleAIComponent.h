#pragma once

#include "Engine/Entities/Component.h"
#include "Engine/Entities/UpdatePass.h"

class VehicleAIComponent : public Component
{
public:
	static Component* CreateComponent(Entity* owner, const YAML::Node& properties);

public:
	virtual ~VehicleAIComponent();

	virtual bool OnMessage(Message*) override;
	virtual void OnUpdate(const Time& elapsed_time, UpdatePass::TYPE update_pass) override;

protected:
	VehicleAIComponent(Entity* owner, const YAML::Node& properties);
};