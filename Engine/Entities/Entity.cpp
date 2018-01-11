#include "Pch.h"

#include "Entities/Component.h"
#include "Entities/Entity.h"
#include "Entities/EntityManager.h"

Entity::Entity()
{
}

Entity::~Entity()
{
}

void Entity::AddComponent(Component * component)
{
	m_Components.push_back(component);
}

int Entity::OnMessage(Message * message)
{
	int components_responding_to_message = 0;

	for (auto component : m_Components)
	{
		if (component->OnMessage(message))
		{
			++components_responding_to_message;
		}
	}

	return components_responding_to_message;
}

void Entity::OnUpdate(float elapsed_time)
{
	for (auto component : m_Components)
	{
		component->OnUpdate(elapsed_time);
	}
}

void Entity::AddTag(const std::string& tag)
{
	EntityManager::GetSingleton()->AddTag(this, tag);
}

void Entity::RemoveTag(const std::string& tag)
{
	EntityManager::GetSingleton()->RemoveTag(this, tag);
}
