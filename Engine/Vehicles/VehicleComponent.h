#pragma once

#include "Engine/Entities/Component.h"


class VehicleComponent : public Component
{
public:
	static Component* CreateComponent(Entity* owner, const YAML::Node& properties);

public:
	virtual ~VehicleComponent();

	virtual bool OnMessage(Message*);
	virtual void OnUpdate(const Time& elapsed_time);

protected:
	VehicleComponent(Entity* owner, const YAML::Node& properties);

	virtual void OnSetActive(bool active) {}
};