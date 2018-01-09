#include "Pch.h"

#include "Entities/Component.h"
#include "Entities/Entity.h"

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

