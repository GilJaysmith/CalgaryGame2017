#include "Engine/Pch.h"

#include "Engine/Entities/Entity.h"
#include "Engine/Entities/EntityManager.h"
#include "Engine/GameStates/Time.h"

namespace EntityManager
{
	std::set<Entity*> s_AllEntities;
	std::map<std::string, std::set<Entity*>> s_EntityTags;
	std::vector<Entity*> s_EntitiesToDelete;

	void ActuallyDestroyEntity(Entity* entity);

	void Initialize()
	{
	}

	void Terminate()
	{
	}

	void Update(const Time& time)
	{
		for (auto entity : s_AllEntities)
		{
			entity->OnUpdate(time);
		}
	}

	Entity * CreateEntity()
	{
		Entity* new_entity = new Entity();
		s_AllEntities.insert(new_entity);
		return new_entity;
	}

	void DestroyEntity(Entity* entity)
	{
		s_EntitiesToDelete.push_back(entity);
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
		for (auto it : s_AllEntities)
		{
			it->OnMessage(message);
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
		s_AllEntities.erase(entity);
		for (auto it : s_EntityTags)
		{
			it.second.erase(entity);
		}
		delete entity;
	}
}

