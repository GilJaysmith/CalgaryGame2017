#pragma once

#include <string>
#include <vector>

class Component;
class Message;

class Entity
{
public:

	void AddComponent(Component* component);
	int OnMessage(Message* message);
	void OnUpdate(float elapsed_time);

	glm::mat4 GetTransform() const { return m_Transform;  }
	void SetTransform(const glm::mat4& transform) { m_Transform = transform; }

	void AddTag(const std::string& tag);
	void RemoveTag(const std::string& tag);

protected:

	friend class EntityManager;
	Entity();
	~Entity();

	std::vector<Component*> m_Components;
	glm::mat4 m_Transform;
};
