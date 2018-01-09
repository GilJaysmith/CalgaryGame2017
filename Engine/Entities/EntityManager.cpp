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
	m_AllEntities.erase(entity);
	delete entity;
}
