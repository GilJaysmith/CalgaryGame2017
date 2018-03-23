#pragma once

#include <string>
#include <vector>

#include "Engine/Entities/UpdatePass.h"
#include "Engine/GameStates/Time.h"


class Component;
class Message;
class Time;


class Entity
{
public:
	Entity();
	virtual ~Entity();

	static Entity* CreateEntity(const std::string& entity_type, const glm::mat4& transform);

	void AddComponent(Component* component);
	int OnMessage(Message* message);
	void OnUpdate(const Time& elapsed_time, UpdatePass::TYPE update_pass);

	glm::mat4 GetTransform() const { return m_Transform;  }
	void SetTransform(const glm::mat4& transform) { m_Transform = transform; }

	unsigned int GetEntityId() const { return m_EntityId; }

	void AddTag(const std::string& tag);
	void RemoveTag(const std::string& tag);

	void SetActive(bool active);

protected:
	std::vector<Component*> m_Components;
	glm::mat4 m_Transform;
	unsigned int m_EntityId;
	std::string m_Type;
	static unsigned int s_NextEntityId;
};
