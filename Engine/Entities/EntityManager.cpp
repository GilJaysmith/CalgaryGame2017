#include "Pch.h"

#include "Entities/Entity.h"
#include "Entities/EntityManager.h"

EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
}

Entity * EntityManager::CreateEntity()
{
	Entity* new_entity = new Entity();
	m_AllEntities.insert(new_entity);
	return new_entity;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	m_EntitiesToDelete.push_back(entity);
}

void EntityManager::AddTag(Entity* entity, const std::string& tag)
{
	m_EntityTags[tag].insert(entity);
}

void EntityManager::RemoveTag(Entity* entity, const std::string& tag)
{
	auto it = m_EntityTags.find(tag);
	if (it != m_EntityTags.end())
	{
		it->second.erase(entity);
		if (!it->second.size())
		{
			m_EntityTags.erase(it);
		}
	}
}

void EntityManager::BroadcastMessage(Message* message)
{
	for (auto it : m_AllEntities)
	{
		it->OnMessage(message);
	}
}

void EntityManager::BroadcastMessageToTag(Message* message, const std::string& tag)
{
	auto it = m_EntityTags.find(tag);
	if (it != m_EntityTags.end())
	{
		for (auto entity : it->second)
		{
			entity->OnMessage(message);
		}
	}
}

void EntityManager::DestroyPendingEntities()
{
	for (auto entity : m_EntitiesToDelete)
	{
		ActuallyDestroyEntity(entity);
	}
	m_EntitiesToDelete.clear();
}

void EntityManager::ActuallyDestroyEntity(Entity* entity)
{
	m_AllEntities.erase(entity);
	for (auto it : m_EntityTags)
	{
		it.second.erase(entity);
	}
	delete entity;
}