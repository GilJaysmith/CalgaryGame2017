#pragma once

#include "Engine/Entities/Component.h"


class Entity;
class Time;

class CollisionResponseComponent : public Component
{
public:
	static Component* CreateComponent(Entity* owner, const YAML::Node& properties);

public:
	virtual ~CollisionResponseComponent();

	bool OnMessage(Message*) override;

protected:
	CollisionResponseComponent(Entity* owner, const YAML::Node& properties);
};