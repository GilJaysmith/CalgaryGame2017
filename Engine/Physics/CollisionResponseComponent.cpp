#include "Engine/Pch.h"

#include "Engine/Physics/CollisionMessages.h"
#include "Engine/Physics/CollisionResponseComponent.h"


Component* CollisionResponseComponent::CreateComponent(Entity* owner, const YAML::Node& properties)
{
	return MemNew(MemoryPool::Physics, CollisionResponseComponent)(owner, properties);
}

CollisionResponseComponent::~CollisionResponseComponent()
{
}

bool CollisionResponseComponent::OnMessage(Message* message)
{
	if (message->GetMessageType() == MessageType::Collision)
	{
		switch (message->GetMessageSubtype())
		{
			case CollisionMessageSubtype::Touch:
			{
				return true;
			}
			break;
		}
	}
	return false;
}

CollisionResponseComponent::CollisionResponseComponent(Entity* owner, const YAML::Node& properties)
	: Component(owner)
{
}
