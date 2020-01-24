#pragma once

#include "Engine/Entities/Component.h"


class Entity;
class Mesh;
class RenderableMesh;

class RenderComponent : public Component
{
public:
	static Component* CreateComponent(Entity* owner, const YAML::Node& properties);

public:
	virtual ~RenderComponent();

	bool OnMessage(Message*) override;
	void OnUpdate(const Time& elapsed_time, UpdatePass::TYPE update_pass) override;

protected:
	RenderComponent(Entity* owner, const YAML::Node& properties);

	virtual void OnSetActive(bool active) override;

	RenderableMesh* m_Renderable;
};
