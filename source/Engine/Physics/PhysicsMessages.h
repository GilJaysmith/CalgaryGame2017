#pragma once

#include "Engine/Entities/Message.h"
#include "Engine/Entities/MessageTypes.h"

namespace PhysicsMessageSubtype
{
	enum TYPE
	{
		SetLinearVelocity,
		SetAngularVelocity,
		DisableGravity,		
		COUNT
	};
};


struct Message_PhysicsSetLinearVelocity : Message
{
	Message_PhysicsSetLinearVelocity(const glm::vec3& linear_velocity)
		: Message(MessageType::Physics, PhysicsMessageSubtype::SetLinearVelocity)
		, m_LinearVelocity(linear_velocity)
	{}

	glm::vec3 m_LinearVelocity;
};


struct Message_PhysicsSetAngularVelocity : Message
{
	Message_PhysicsSetAngularVelocity(const glm::vec3& angular_velocity)
		: Message(MessageType::Physics, PhysicsMessageSubtype::SetAngularVelocity)
		, m_AngularVelocity(angular_velocity)
	{}

	glm::vec3 m_AngularVelocity;
};


struct Message_PhysicsDisableGravity : Message
{
	Message_PhysicsDisableGravity(bool disable_gravity)
		: Message(MessageType::Physics, PhysicsMessageSubtype::DisableGravity)
		, m_DisableGravity(disable_gravity)
	{}

	bool m_DisableGravity;
};