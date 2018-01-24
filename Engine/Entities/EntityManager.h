#pragma once

#include <map>
#include <set>

class Entity;
class Message;
class Time;

namespace EntityManager
{
	void Initialize();
	void Terminate();

	void Update(const Time& time);

	Entity* CreateEntity();
	void DestroyEntity(Entity* entity);

	void AddTag(Entity* entity, const std::string& tag);
	void RemoveTag(Entity* entity, const std::string& tag);

	void BroadcastMessage(Message* message);
	void BroadcastMessageToTag(Message* message, const std::string& tag);

	void DestroyPendingEntities();
}

