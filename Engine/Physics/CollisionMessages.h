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
		Trigger,
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
struct Message_CollisionTrigger : Message
{
	Message_CollisionTrigger(physx::PxActor* my_actor, physx::PxShape* my_shape, physx::PxActor* other_actor, physx::PxShape* other_shape)
		: Message(MessageType::Collision, CollisionMessageSubtype::Trigger)
		, m_MyActor(my_actor)
		, m_MyShape(my_shape)
		, m_OtherActor(other_actor)
		, m_OtherShape(other_shape)
	{}

	physx::PxActor* m_MyActor;
	physx::PxShape* m_MyShape;
	physx::PxActor* m_OtherActor;
	physx::PxShape* m_OtherShape;
};