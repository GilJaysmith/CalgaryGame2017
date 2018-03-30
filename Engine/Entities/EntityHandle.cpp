#include "Engine/Pch.h"

#include "Engine/Entities/Entity.h"
#include "Engine/Entities/EntityHandle.h"
#include "Engine/Entities/EntityManager.h"


EntityHandle::EntityHandle()
	: m_Entity(nullptr)
	, m_EntityId(0)
{
}

EntityHandle::EntityHandle(Entity* entity)
	: m_Entity(entity)
	, m_EntityId(entity ? entity->GetId() : 0)
{
}

bool EntityHandle::IsValid() const
{
	auto id = EntityManager::GetEntityId(m_Entity);
	return id != 0 && id == m_EntityId;
}

Entity* EntityHandle::GetEntity() const
{
	return IsValid() ? m_Entity : nullptr;
}

Entity* EntityHandle::operator->() const
{
	return GetEntity();
}

EntityHandle& EntityHandle::operator=(Entity* entity)
{
	m_Entity = entity;
	m_EntityId = entity ? entity->GetId() : 0;
	return *this;
}