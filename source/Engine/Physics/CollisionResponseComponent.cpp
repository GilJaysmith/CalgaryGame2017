#include "Engine/Pch.h"

#include "Engine/Entities/Entity.h"
#include "Engine/Health/DamageMessages.h"
#include "Engine/Physics/CollisionMessages.h"
#include "Engine/Physics/CollisionResponseComponent.h"

#include "sdks/PhysX/Include/PxPhysicsAPI.h"

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
				Message_CollisionTouch* mct = static_cast<Message_CollisionTouch*>(message);
				physx::PxActor* my_actor = mct->m_MyActor;
				physx::PxActor* other_actor = mct->m_OtherActor;

				// Damage ourselves and the other one if there is one.
				Message_DamageTakeDamage dtd(10);
				m_Entity->OnMessage(&dtd);

				Entity* other_entity = static_cast<Entity*>(other_actor->userData);
				if (other_entity)
				{
					other_entity->OnMessage(&dtd);
				}

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
