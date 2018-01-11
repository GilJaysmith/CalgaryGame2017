#pragma once

#include <map>
#include <set>

#include "Utilities/Singleton.h"

class Entity;

class EntityManager : public Singleton<EntityManager>
{
public:
	EntityManager();
	~EntityManager();

	Entity* CreateEntity();
	void DestroyEntity(Entity* entity);

	void AddTag(Entity* entity, const std::string& tag);
	void RemoveTag(Entity* entity, const std::string& tag);

	void BroadcastMessage(Message* message);
	void BroadcastMessageToTag(Message* message, const std::string& tag);

	void DestroyPendingEntities();

protected:

	void ActuallyDestroyEntity(Entity* entity);

	std::set<Entity*> m_AllEntities;
	std::map<std::string, std::set<Entity*>> m_EntityTags;
	std::vector<Entity*> m_EntitiesToDelete;
};
