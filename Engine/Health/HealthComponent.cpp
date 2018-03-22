#include "Engine/Pch.h"

#include "Engine/Entities/EntityManager.h"
#include "Engine/Health/DamageMessages.h"
#include "Engine/Health/HealthComponent.h"

#pragma optimize("", off)


Component* HealthComponent::CreateComponent(Entity* owner, const YAML::Node& properties)
{
	return MemNew(MemoryPool::Entities, HealthComponent)(owner, properties);
}

HealthComponent::~HealthComponent()
{
}

bool HealthComponent::OnMessage(Message* message)
{
	if (message->GetMessageType() == MessageType::Damage)
	{
		switch (message->GetMessageSubtype())
		{
			case DamageMessageSubtype::TakeDamage:
			{
				Message_DamageTakeDamage* dtd = static_cast<Message_DamageTakeDamage*>(message);
				unsigned int damage_to_take = dtd->m_Damage > m_Health ? m_Health : dtd->m_Damage;
				m_Health -= damage_to_take;
				if (m_Health == 0)
				{
					EntityManager::DestroyEntity(m_Entity);
				}
				return true;
			}
			break;

			case DamageMessageSubtype::GetHealth:
			{
				Message_DamageGetHealth* dgh = static_cast<Message_DamageGetHealth*>(message);
				dgh->m_Health = m_Health;
				return true;
			}
			break;
		}
	}
	return false;
}

HealthComponent::HealthComponent(Entity* owner, const YAML::Node& properties)
	: Component(owner)
	, m_Health(0)
{
	m_Health = properties["health"].as<unsigned int>();

	assert(m_Health > 0);
}
