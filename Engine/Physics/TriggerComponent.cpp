#include "Engine/Pch.h"

#include "Engine/Entities/Entity.h"
#include "Engine/Physics/CollisionMessages.h"
#include "Engine/Physics/TriggerComponent.h"

#include "sdks/PhysX/PhysX/Include/PxPhysicsAPI.h"

Component* TriggerComponent::CreateComponent(Entity* owner, const YAML::Node& properties)
{
	return MemNew(MemoryPool::Physics, TriggerComponent)(owner, properties);
}

TriggerComponent::~TriggerComponent()
{
}

bool TriggerComponent::OnMessage(Message* message)
{
	if (message->GetMessageType() == MessageType::Collision)
	{
		switch (message->GetMessageSubtype())
		{
		case CollisionMessageSubtype::Touch:
		{
			Message_CollisionTouch* mct = static_cast<Message_CollisionTouch*>(message);
			physx::PxActor* my_actor = mct->m_MyActor;
			physx::PxActor* other_actor = mct->m_OtherActor;

			Entity* other_entity = static_cast<Entity*>(other_actor->userData);
			if (other_entity)
			{
			}

			return true;
		}
		break;
		}
	}
	return false;
}

TriggerComponent::TriggerComponent(Entity* owner, const YAML::Node& properties)
	: Component(owner)
{
}
