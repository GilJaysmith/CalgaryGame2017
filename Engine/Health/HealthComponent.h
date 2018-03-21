#pragma once

#include "Engine/Entities/Component.h"


class Entity;
class Time;

class HealthComponent : public Component
{
public:
	static Component* CreateComponent(Entity* owner, const YAML::Node& properties);

public:
	virtual ~HealthComponent();

	bool OnMessage(Message*) override;

protected:
	HealthComponent(Entity* owner, const YAML::Node& properties);
};