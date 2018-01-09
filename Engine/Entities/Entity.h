#pragma once

#include <vector>

#include "Math/Types.h"

class Component;
class Message;

class Entity
{
public:

	void AddComponent(Component* component);
	int OnMessage(Message* message);

	Math::Vector3 GetTranslation() const;
	Math::Vector3 GetRotation() const;

protected:

	friend class EntityManager;
	Entity();
	~Entity();

	std::vector<Component*> m_Components;
	Math::Vector3 m_Translation;
	Math::Vector3 m_Rotation;
};
