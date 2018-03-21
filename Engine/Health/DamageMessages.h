#pragma once

#include "Engine/Entities/Message.h"
#include "Engine/Entities/MessageTypes.h"

namespace DamageMessageSubtype
{
	enum TYPE
	{
		TakeDamage,
		COUNT
	};
};


struct Message_DamageTakeDamage : Message
{
	Message_DamageTakeDamage()
		: Message(MessageType::Damage, DamageMessageSubtype::TakeDamage)
	{}
};