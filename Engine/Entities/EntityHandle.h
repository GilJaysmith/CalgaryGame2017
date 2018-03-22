#pragma once

class Entity;

class EntityHandle
{
public:
	EntityHandle();
	EntityHandle(Entity* entity);
	bool IsValid() const;
	Entity* GetEntity() const;
	Entity* operator->() const;
	EntityHandle& operator=(Entity* entity);

protected:
	Entity* m_Entity;
	unsigned int m_EntityId;
};
