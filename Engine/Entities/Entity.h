#pragma once

#include <string>
#include <vector>

#include "Engine/GameStates/Time.h"

#include "sdks/libyaml/include/yaml-cpp/yaml.h"

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
	void OnUpdate(const Time& elapsed_time);

	glm::mat4 GetTransform() const { return m_Transform;  }
	void SetTransform(const glm::mat4& transform) { m_Transform = transform; }

	void AddTag(const std::string& tag);
	void RemoveTag(const std::string& tag);

protected:
	std::vector<Component*> m_Components;
	glm::mat4 m_Transform;
};
