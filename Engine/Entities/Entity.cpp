#include "Engine/Pch.h"

#include "Engine/Entities/Component.h"
#include "Engine/Entities/ComponentRegistry.h"
#include "Engine/Entities/Entity.h"
#include "Engine/Entities/EntityManager.h"

#include "sdks/libyaml/include/yaml-cpp/yaml.h"

Entity* Entity::CreateEntity(const std::string& entity_type)
{
	std::string yaml_filename = "data/entities/" + entity_type + ".yaml";
	YAML::Node yaml = YAML::LoadFile(yaml_filename);

	Entity* entity = EntityManager::CreateEntity();
	for (auto behaviour : yaml["behaviours"])
	{
		Component* new_component = ComponentRegistry::CreateComponent(entity, behaviour.first.as<std::string>(), behaviour.second);
		entity->AddComponent(new_component);
	}
	return entity;
}

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

void Entity::OnUpdate(const Time& elapsed_time)
{
	for (auto component : m_Components)
	{
		component->OnUpdate(elapsed_time);
	}
}

void Entity::AddTag(const std::string& tag)
{
	EntityManager::AddTag(this, tag);
}

void Entity::RemoveTag(const std::string& tag)
{
	EntityManager::RemoveTag(this, tag);
}
