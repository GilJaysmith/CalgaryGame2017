#pragma once

#include "Engine/Entities/Message.h"
#include "Engine/Entities/MessageTypes.h"

namespace DamageMessageSubtype
{
	enum TYPE
	{
		TakeDamage,
		GetHealth,

		COUNT
	};
};


struct Message_DamageTakeDamage : Message
{
	Message_DamageTakeDamage(unsigned int damage)
		: Message(MessageType::Damage, DamageMessageSubtype::TakeDamage)
		, m_Damage(damage)
	{}

	unsigned int m_Damage;
};

struct Message_DamageGetHealth : Message
{
	Message_DamageGetHealth()
		: Message(MessageType::Damage, DamageMessageSubtype::GetHealth)
	{}

	unsigned int m_Health;
};