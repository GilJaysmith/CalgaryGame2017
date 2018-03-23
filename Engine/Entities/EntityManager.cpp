#include "Engine/Pch.h"

#include "Engine/Entities/Entity.h"
#include "Engine/Entities/EntityManager.h"
#include "Engine/GameStates/Time.h"

namespace EntityManager
{
	std::map<Entity*, unsigned int> s_EntityHandleMap;
	unsigned int s_NextEntityIndex = 1;
	std::map<std::string, std::set<Entity*>> s_EntityTags;
	std::set<Entity*> s_EntitiesToDelete;

	void ActuallyDestroyEntity(Entity* entity);

	void Initialize()
	{
	}

	void Terminate()
	{
		while (s_EntityHandleMap.size())
		{
			ActuallyDestroyEntity(s_EntityHandleMap.begin()->first);
		}
	}

	void Update(const Time& time, UpdatePass::TYPE update_pass)
	{
		if (time.toMilliseconds() == 0.0f)
		{
			return;
		}
		for (auto entity : s_EntityHandleMap)
		{
			entity.first->OnUpdate(time, update_pass);
		}
	}

	Entity * CreateEntity()
	{
		Entity* new_entity = MemNew(MemoryPool::Entities, Entity);
		s_EntityHandleMap[new_entity] = new_entity->GetEntityId();
		return new_entity;
	}

	void DestroyEntity(Entity* entity)
	{
		s_EntitiesToDelete.insert(entity);
	}

	void AddTag(Entity* entity, const std::string& tag)
	{
		s_EntityTags[tag].insert(entity);
	}

	void RemoveTag(Entity* entity, const std::string& tag)
	{
		auto it = s_EntityTags.find(tag);
		if (it != s_EntityTags.end())
		{
			it->second.erase(entity);
			if (!it->second.size())
			{
				s_EntityTags.erase(it);
			}
		}
	}

	void BroadcastMessage(Message* message)
	{
		for (auto it : s_EntityHandleMap)
		{
			it.first->OnMessage(message);
		}
	}

	void BroadcastMessageToTag(Message* message, const std::string& tag)
	{
		auto it = s_EntityTags.find(tag);
		if (it != s_EntityTags.end())
		{
			for (auto entity : it->second)
			{
				entity->OnMessage(message);
			}
		}
	}

	void DestroyPendingEntities()
	{
		for (auto entity : s_EntitiesToDelete)
		{
			ActuallyDestroyEntity(entity);
		}
		s_EntitiesToDelete.clear();
	}

	void ActuallyDestroyEntity(Entity* entity)
	{
		s_EntityHandleMap.erase(entity);
		for (auto it : s_EntityTags)
		{
			it.second.erase(entity);
		}
		MemDelete(entity);
	}

	unsigned int GetEntityId(Entity* entity)
	{
		auto it = s_EntityHandleMap.find(entity);
		return (it != s_EntityHandleMap.end()) ? it->second : 0;
	}
}

