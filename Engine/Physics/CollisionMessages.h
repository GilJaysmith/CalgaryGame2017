#pragma once

#include "Engine/Entities/Message.h"
#include "Engine/Entities/MessageTypes.h"

namespace physx
{
	class PxActor;
	class PxShape;
}

namespace CollisionMessageSubtype
{
	enum TYPE
	{
		Touch,
		COUNT
	};
};


struct Message_CollisionTouch : Message
{
	Message_CollisionTouch(physx::PxActor* my_actor, physx::PxActor* other_actor)
		: Message(MessageType::Collision, CollisionMessageSubtype::Touch)
		, m_MyActor(my_actor)
		, m_OtherActor(other_actor)
	{}

	physx::PxActor* m_MyActor;
	physx::PxActor* m_OtherActor;
};