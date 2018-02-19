#pragma once

#include "Engine/Entities/Component.h"
#include "Engine/Entities/UpdatePass.h"

class VehicleInputHandler;

class VehicleInputComponent : public Component
{
public:
	static Component* CreateComponent(Entity* owner, const YAML::Node& properties);

public:
	virtual ~VehicleInputComponent();

	virtual bool OnMessage(Message*) override;
	virtual void OnUpdate(const Time& elapsed_time, UpdatePass::TYPE update_pass) override;

protected:
	VehicleInputComponent(Entity* owner, const YAML::Node& properties);

	virtual void OnSetActive(bool active) {}

	VehicleInputHandler* m_InputHandler;
};