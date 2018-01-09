#pragma once

#include <set>

class Entity;

class EntityManager
{
public:
	EntityManager();
	~EntityManager();

	Entity* CreateEntity();
	void DestroyEntity(Entity* entity);

protected:
	std::set<Entity*> m_AllEntities;
};
