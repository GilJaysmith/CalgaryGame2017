#include "Engine/Pch.h"

#include "Engine/Health/DamageMessages.h"
#include "Engine/Health/HealthComponent.h"


Component* HealthComponent::CreateComponent(Entity* owner, const YAML::Node& properties)
{
	return MemNew(MemoryPool::Physics, HealthComponent)(owner, properties);
}

HealthComponent::~HealthComponent()
{
}

bool HealthComponent::OnMessage(Message* message)
{
	if (message->GetMessageType() == MessageType::Collision)
	{
		switch (message->GetMessageSubtype())
		{
		case DamageMessageSubtype::TakeDamage:
		{
			return true;
		}
		break;
		}
	}
	return false;
}

HealthComponent::HealthComponent(Entity* owner, const YAML::Node& properties)
	: Component(owner)
{
}
