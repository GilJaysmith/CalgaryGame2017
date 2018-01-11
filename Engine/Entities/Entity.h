#pragma once

#include <string>
#include <vector>

#include "Math/Types.h"

class Component;
class Message;

class Entity
{
public:

	void AddComponent(Component* component);
	int OnMessage(Message* message);
	void OnUpdate(float elapsed_time);

	Math::Vector3 GetTranslation() const;
	Math::Vector3 GetRotation() const;

	void AddTag(const std::string& tag);
	void RemoveTag(const std::string& tag);

protected:

	friend class EntityManager;
	Entity();
	~Entity();

	std::vector<Component*> m_Components;
	Math::Vector3 m_Translation;
	Math::Vector3 m_Rotation;
};
