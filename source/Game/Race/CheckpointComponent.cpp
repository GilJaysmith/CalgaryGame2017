#include "Game/Pch.h"

#include "Engine/DebugDraw/DebugDraw.h"
#include "Engine/Entities/Entity.h"
#include "Engine/Health/DamageMessages.h"
#include "Engine/Physics/CollisionMessages.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Physics/PhysicsUtils.h"
#include "Game/Race/CheckpointComponent.h"
#include "Game/Race/RaceManager.h"

#include "sdks/PhysX/Include/PxPhysicsAPI.h"

Component* CheckpointComponent::CreateComponent(Entity* owner, const YAML::Node& properties)
{
	return MemNew(MemoryPool::Physics, CheckpointComponent)(owner, properties);
}

CheckpointComponent::~CheckpointComponent()
{
	if (m_Actor)
	{
		m_Actor->release();
	}
	m_DebugObject->Release();
}

bool CheckpointComponent::OnMessage(Message* message)
{
	if (message->GetMessageType() == MessageType::Collision)
	{
		switch (message->GetMessageSubtype())
		{
		case CollisionMessageSubtype::Trigger:
		{
			Message_CollisionTrigger* mct = static_cast<Message_CollisionTrigger*>(message);
			physx::PxActor* my_actor = mct->m_MyActor;
			physx::PxActor* other_actor = mct->m_OtherActor;

			if (other_actor)
			{
				Entity* other_entity = static_cast<Entity*>(other_actor->userData);
				if (mct->m_Touch)
				{
					if (RaceManager::CarReachesCheckpoint(other_entity, static_cast<Entity*>(my_actor->userData)))
					{
						m_DebugObject->SetColour(glm::vec3(1.0f, 0.0f, 0.0f));
					}
				}
			}

			return true;
		}
		break;
		}
	}
	return false;
}

CheckpointComponent::CheckpointComponent(Entity* owner, const YAML::Node& properties)
	: Component(owner)
	, m_Actor(nullptr)
{
	const glm::mat4& transform = owner->GetTransform();
	glm::vec3 position = transform[3];
	m_Actor = Physics::GetPhysics()->createRigidStatic(physx::PxTransform(physx::PxVec3(position.x, position.y, position.z)));

	physx::PxMaterial* trigger_material = Physics::GetMaterial("trigger");
	const auto& node = properties["shape"];
	physx::PxShape* shape = CreatePhysicsShape(node, *trigger_material, physx::PxShapeFlags(physx::PxShapeFlag::eTRIGGER_SHAPE | physx::PxShapeFlag::eVISUALIZATION));
	m_Actor->attachShape(*shape);

	physx::PxSphereGeometry geo;
	shape->getSphereGeometry(geo);
	m_DebugObject = DebugDraw::DebugSphere("TEST", geo.radius);
	m_DebugObject->SetExternalTransform(m_Entity->GetTransformPtr());
	m_DebugObject->SetColour(glm::vec3(0.0f, 1.0f, 0.0f));

	std::vector<unsigned int> filter = properties["simfilter"].as<std::vector<unsigned int>>();
	assert(filter.size() == 4);
	physx::PxFilterData filter_data(filter[0], filter[1], filter[2] | physx::PxPairFlag::eNOTIFY_TOUCH_FOUND, filter[3]);
	shape->setSimulationFilterData(filter_data);

	m_Actor->userData = static_cast<void*>(m_Entity);
	m_Actor->setActorFlag(physx::PxActorFlag::eVISUALIZATION, true);
	Physics::GetScene()->addActor(*m_Actor);
}
